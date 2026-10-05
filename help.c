#include <stdio.h>
#include "help.h"

void display_help(void)
{
    printf("\nMP3 TAG READER AND EDITOR\n\n");

    printf("VIEW MODE:\n");
    printf("./a.out -v filename.mp3\n\n");

    printf("EDIT MODE:\n");
    printf("./a.out -e -t filename.mp3 \"new title\"\n");
    printf("./a.out -e -a filename.mp3 \"new artist\"\n");
    printf("./a.out -e -A filename.mp3 \"new album\"\n");
    printf("./a.out -e -y filename.mp3 \"new year\"\n");
    printf("./a.out -e -c filename.mp3 \"new content\"\n");
    printf("./a.out -e -g filename.mp3 \"new comment\"\n\n");

    printf("EDIT OPTIONS:\n");
    printf("-t  Edit Title\n");
    printf("-a  Edit Artist\n");
    printf("-A  Edit Album\n");
    printf("-y  Edit Year\n");
    printf("-c  Edit Content\n");
    printf("-g  Edit Comment\n");
}