#ifndef EDIT_H
#define EDIT_H

#include <stdio.h>

typedef struct
{
    char *filename;
    char *edit_option;
    char *new_data;
} EditInfo;

int edit_mp3(EditInfo *editInfo);

#endif