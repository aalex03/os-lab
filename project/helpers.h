#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/sysmacros.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <dirent.h>
#include <sys/wait.h>
void createSymlink(char filename[]);
void printLinkedFile(char filename[]);
int countDirectoryCfiles(char filename[]);
pid_t handleCfile(char filename[]);
void printAccessRights(struct stat *file);
int ends_with_c_extension(char filename[]);