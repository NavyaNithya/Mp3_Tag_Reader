#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "edit.h"

unsigned int read_syncsafe_size(unsigned char *buffer)
{
    unsigned int size = 0;
    int i;

    for (i = 0; i < 4; i++)
    {
        size = (size << 7) | (buffer[i] & 0x7F);
    }

    return size;
}

void write_syncsafe_size(unsigned int size, unsigned char *buffer)
{
    buffer[0] = (size >> 21) & 0x7F;
    buffer[1] = (size >> 14) & 0x7F;
    buffer[2] = (size >> 7) & 0x7F;
    buffer[3] = size & 0x7F;
}

int check_edit_option(char *option)
{
    if (strcmp(option, "-t") == 0)
        return 1;

    if (strcmp(option, "-a") == 0)
        return 1;

    if (strcmp(option, "-A") == 0)
        return 1;

    if (strcmp(option, "-y") == 0)
        return 1;

    if (strcmp(option, "-c") == 0)
        return 1;

    if (strcmp(option, "-g") == 0)
        return 1;

    return 0;
}

char *get_edit_tag(char *option)
{
    if (strcmp(option, "-t") == 0)
        return "TIT2";

    if (strcmp(option, "-a") == 0)
        return "TPE1";

    if (strcmp(option, "-A") == 0)
        return "TALB";

    if (strcmp(option, "-y") == 0)
        return "TYER";

    if (strcmp(option, "-c") == 0)
        return "TCON";

    if (strcmp(option, "-g") == 0)
        return "COMM";

    return NULL;
}

int edit_mp3(EditInfo *editInfo)
{
    FILE *src;
    FILE *dest;

    unsigned char header[10];
    unsigned char tag[4];
    unsigned char size_buffer[4];
    unsigned char flag_encoding[3];

    unsigned int size;
    unsigned int new_size;

    unsigned char *data;
    unsigned char *new_data;

    char temp_file[] = "temp.mp3";
    char *edit_tag;

    int i;

    if (editInfo == NULL)
    {
        return 0;
    }

    if (check_edit_option(editInfo->edit_option) == 0)
    {
        printf("ERROR: Invalid edit option\n");
        return 0;
    }

    if (editInfo->filename == NULL ||
        editInfo->new_data == NULL)
    {
        printf("ERROR: Invalid input\n");
        return 0;
    }

    src = fopen(editInfo->filename, "rb");

    if (src == NULL)
    {
        printf("ERROR: Unable to open MP3 file\n");
        return 0;
    }

    if (fread(header, 1, 10, src) != 10)
    {
        printf("ERROR: Unable to read ID3 header\n");
        fclose(src);
        return 0;
    }

    if (header[0] != 'I' ||
        header[1] != 'D' ||
        header[2] != '3')
    {
        printf("ERROR: ID3 signature not found\n");
        fclose(src);
        return 0;
    }

    dest = fopen(temp_file, "wb");

    if (dest == NULL)
    {
        printf("ERROR: Unable to create temp.mp3\n");
        fclose(src);
        return 0;
    }

    /* Copy header 10 bytes from src to dest */
    fwrite(header, 1, 10, dest);

    /* Move pointer is already at 10th offset */

    edit_tag = get_edit_tag(editInfo->edit_option);

    if (edit_tag == NULL)
    {
        fclose(src);
        fclose(dest);
        remove(temp_file);
        return 0;
    }

    /*
     * Read and process the frames.
     * Six frames are processed according to the view procedure.
     */
    for (i = 0; i < 6; i++)
    {
        if (fread(tag, 1, 4, src) != 4)
        {
            break;
        }

        if (fread(size_buffer, 1, 4, src) != 4)
        {
            break;
        }

        /*
         * Convert the frame size to integer.
         */
        size = read_syncsafe_size(size_buffer);

        /*
         * If tag is NOT same:
         * copy tag
         * copy size as it is
         * copy next 3 bytes
         * copy size-1 bytes data
         */
        if (memcmp(tag, edit_tag, 4) != 0)
        {
            fwrite(tag, 1, 4, dest);

            fwrite(size_buffer, 1, 4, dest);

            if (fread(flag_encoding, 1, 3, src) != 3)
            {
                break;
            }

            fwrite(flag_encoding, 1, 3, dest);

            if (size > 1)
            {
                data = malloc(size - 1);

                if (data == NULL)
                {
                    fclose(src);
                    fclose(dest);
                    remove(temp_file);
                    return 0;
                }

                if (fread(data, 1, size - 1, src) != size - 1)
                {
                    free(data);
                    fclose(src);
                    fclose(dest);
                    remove(temp_file);
                    return 0;
                }

                fwrite(data, 1, size - 1, dest);

                free(data);
            }
        }
        else
        {
            /*
             * Tag is SAME.
             *
             * Find size of new data:
             * length + 1
             */
            new_size = strlen(editInfo->new_data) + 1;

            if (new_size > 0x0FFFFFFF)
            {
                printf("ERROR: New data is too large\n");

                fclose(src);
                fclose(dest);
                remove(temp_file);
                return 0;
            }

            /*
             * Copy tag to destination.
             */
            fwrite(tag, 1, 4, dest);

            /*
             * Convert new size to big-endian/syncsafe
             * and store it.
             */
            write_syncsafe_size(new_size, size_buffer);

            fwrite(size_buffer, 1, 4, dest);

            /*
             * Copy next 3 bytes:
             * 2 flag bytes + 1 encoding byte
             */
            if (fread(flag_encoding, 1, 3, src) != 3)
            {
                break;
            }

            fwrite(flag_encoding, 1, 3, dest);

            /*
             * Store new data.
             * First byte is the encoding byte,
             * which was already copied above.
             *
             * Therefore write only the actual new text.
             */
            if (strlen(editInfo->new_data) > 0)
            {
                new_data = malloc(strlen(editInfo->new_data));

                if (new_data == NULL)
                {
                    fclose(src);
                    fclose(dest);
                    remove(temp_file);
                    return 0;
                }

                memcpy(new_data,
                       editInfo->new_data,
                       strlen(editInfo->new_data));

                fwrite(new_data,
                       1,
                       strlen(editInfo->new_data),
                       dest);

                free(new_data);
            }

            /*
             * Skip old data:
             * size - 1 bytes
             */
            if (size > 1)
            {
                fseek(src, size - 1, SEEK_CUR);
            }
        }
    }

    /*
     * Copy remaining data from source to destination.
     */
    while ((i = fgetc(src)) != EOF)
    {
        fputc(i, dest);
    }

    fclose(src);
    fclose(dest);

    /*
     * Delete original MP3.
     */
    if (remove(editInfo->filename) != 0)
    {
        printf("ERROR: Unable to delete original file\n");
        remove(temp_file);
        return 0;
    }

    /*
     * Rename temp.mp3 to original MP3.
     */
    if (rename(temp_file, editInfo->filename) != 0)
    {
        printf("ERROR: Unable to rename temp.mp3\n");
        return 0;
    }

    printf("Tag edited successfully\n");

    return 1;
}