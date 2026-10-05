#ifndef VIEW_H
#define VIEW_H

#include <stdio.h>

typedef struct
{
    char *filename;
} ViewInfo;

int view_mp3(ViewInfo *viewInfo);

#endif