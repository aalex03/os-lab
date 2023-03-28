/*
Write a C program that receives as
params multiple arguments representing
paths to regular files, directories and sym links.
Depending on the file type (r,f,d,se) print
an interactive menu
*/

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/sysmacros.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

void handleMenu(char filename[],struct stat buff)
{
    char options[10];
    if(S_ISREG(buff.st_mode))
    {
        printf("Regular file\nEnter options:\n-n (file name)\n-d(dim/size)\n-h (number of hard links)\n-m (time of last modif)\n-a (access rights)\n-l [filename] (create a symbolic link)\n");
        scanf("-%10s",options);
        for (int i = 0; i < strlen(options); i++)
        {
            switch (options[i])
            {
            case 'n':
                printf("%s\n",filename);
                break;
            case 'd':
                printf("%s\n",buff.st_size);
                break;
            default:
                break;
            }
        }
        
    }
}

int main(int argc, char *argv[])
{
    if(argc<2)
    {
        printf("Usage: %s [file1 file2 ...]\n",argv[0]);
        exit(EXIT_FAILURE);
    }
    struct stat buff;

    for (int i = 1; i < argc; i++)
    {
        int result = lstat(argv[i],&buff);  //returns 0 on success and -1 on failure
        if(result==0)
        {
            handleMenu(argv[i],buff);
        }
        else
        {
            printf("could not read info about file");
        }
    }
    
}