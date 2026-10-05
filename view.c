#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "view.h"

unsigned int read_view_size(unsigned char *buffer)
{
    unsigned int size = 0;
    int i;

    for (i = 0; i < 4; i++)
    {
        size = (size << 7) | (buffer[i] & 0x7F);
    }

    return size;
}

void print_tag(char *tag, char *data)
{
    if (strcmp(tag, "TIT2") == 0)
    {
        printf("Title       : %s\n", data);
    }
    else if (strcmp(tag, "TPE1") == 0)
    {
        printf("Artist      : %s\n", data);
    }
    else if (strcmp(tag, "TALB") == 0)
    {
        printf("Album       : %s\n", data);
    }
    else if (strcmp(tag, "TYER") == 0)
    {
        printf("Year        : %s\n", data);
    }
    else if (strcmp(tag, "TCON") == 0)
    {
        printf("Content     : %s\n", data);
    }
    else if (strcmp(tag, "COMM") == 0)
    {
        printf("Comment     : %s\n", data);
    }
}

int view_mp3(ViewInfo *viewInfo)
{
    FILE *fp;

    unsigned char header[10];
    unsigned char tag[5];
    unsigned char size_buffer[4];

    unsigned int size;

    char *metadata;

    int i;

    if (viewInfo == NULL || viewInfo->filename == NULL)
    {
        printf("ERROR: Invalid input\n");
        return 0;
    }

    /*
     * Validate .mp3 extension.
     */
    if (strstr(viewInfo->filename, ".mp3") == NULL)
    {
        printf("ERROR: File is not an MP3 file\n");
        return 0;
    }

    fp = fopen(viewInfo->filename, "rb");

    if (fp == NULL)
    {
        printf("ERROR: Unable to open MP3 file\n");
        return 0;
    }

    /*
     * Read ID3 header.
     */
    if (fread(header, 1, 10, fp) != 10)
    {
        printf("ERROR: Unable to read ID3 header\n");
        fclose(fp);
        return 0;
    }

    /*
     * Validate ID3 signature.
     */
    if (header[0] != 'I' ||
        header[1] != 'D' ||
        header[2] != '3')
    {
        printf("ERROR: ID3 signature not found\n");
        fclose(fp);
        return 0;
    }

    /*
     * Pointer is now at 10th byte.
     */

    printf("--------------------------------------------------\n");
    printf("MP3 TAG INFORMATION\n");
    printf("--------------------------------------------------\n");

    /*
     * Loop 6 times.
     */
    for (i = 0; i < 6; i++)
    {
        /*
         * Read 4 byte tag.
         */
        if (fread(tag, 1, 4, fp) != 4)
        {
            break;
        }

        tag[4] = '\0';

        /*
         * Read 4 byte size.
         */
        if (fread(size_buffer, 1, 4, fp) != 4)
        {
            break;
        }

        /*
         * Convert syncsafe size to integer.
         */
        size = read_view_size(size_buffer);

        /*
         * Skip 3 bytes:
         * 2 flag bytes + 1 encoding byte
         */
        fseek(fp, 3, SEEK_CUR);

        /*
         * Read size - 1 bytes of metadata.
         */
        if (size > 1)
        {
            metadata = malloc(size);

            if (metadata == NULL)
            {
                fclose(fp);
                return 0;
            }

            memset(metadata, 0, size);

            if (fread(metadata, 1, size - 1, fp) != size - 1)
            {
                free(metadata);
                break;
            }

            metadata[size - 1] = '\0';

            /*
             * Print only required tags.
             */
            print_tag((char *)tag, metadata);

            free(metadata);
        }
        else
        {
            print_tag((char *)tag, "");
        }
    }

    printf("--------------------------------------------------\n");

    fclose(fp);

    return 1;
}