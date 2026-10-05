#include <stdio.h>
#include <string.h>

#include "edit.h"
#include "view.h"
#include "help.h"

int main(int argc, char *argv[])
{
    ViewInfo viewInfo;
    EditInfo editInfo;

    /*
     * Check command line arguments.
     */
    if (argc < 2)
    {
        display_help();
        return 0;
    }

    /*
     * Help operation.
     */
    if (strcmp(argv[1], "-h") == 0)
    {
        display_help();
        return 0;
    }

    /*
     * View operation.
     *
     * ./a.out -v filename.mp3
     */
    if (strcmp(argv[1], "-v") == 0)
    {
        if (argc != 3)
        {
            printf("ERROR: Invalid arguments for view\n");
            printf("Usage: ./a.out -v filename.mp3\n");
            return 0;
        }

        viewInfo.filename = argv[2];

        view_mp3(&viewInfo);

        return 0;
    }

    /*
     * Edit operation.
     *
     * ./a.out -e -t filename.mp3 "new title"
     */
    if (strcmp(argv[1], "-e") == 0)
    {
        if (argc != 5)
        {
            printf("ERROR: Invalid arguments for edit\n");
            printf("Usage: ./a.out -e <option> filename.mp3 \"new data\"\n");
            return 0;
        }

        editInfo.edit_option = argv[2];
        editInfo.filename = argv[3];
        editInfo.new_data = argv[4];

        edit_mp3(&editInfo);

        return 0;
    }

    /*
     * Invalid operation.
     */
    printf("ERROR: Invalid operation\n");
    printf("Use -v for view, -e for edit and -h for help\n");

    return 0;
}