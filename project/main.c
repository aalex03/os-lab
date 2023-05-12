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
#include "helpers.h"

void handleMenu(char filename[], struct stat *buff);

void handleRegularFile(char filename[], struct stat *buff, char options[])
{
    for (int i = 0; i < strlen(options); i++)
    {
        switch (options[i])
        {
        case 'n':
            printf("%s\n", filename);
            break;
        case 'd':
            printf("%ld\n", buff->st_size);
            break;
        case 'h':
            printf("%ld\n", buff->st_nlink);
            break;
        case 'm':
            printf("%s", ctime(&buff->st_mtime));
            break;
        case 'a':
            printAccessRights(buff);
            break;
        case 'l':
            createSymlink(filename);
            break;
        default:
            printf("Invalid option: %c\n", options[i]);
            handleMenu(filename, buff);
            break;
        }
    }
}

void handleSymbolicLink(char filename[], struct stat *buff, char options[])
{
    for (int i = 0; i < strlen(options); i++)
    {
        switch (options[i])
        {
        case 'n':
            printf("%s\n", filename);
            break;
        case 'l':
            unlink(filename);
            return;
            break;
        case 'd':
            printf("%ld\n", buff->st_size);
            break;
        case 'z':
            printLinkedFile(filename);
            break;
        case 'a':
            printAccessRights(buff);
            break;
        default:
            break;
        }
    }
}

void handleDirectory(char filename[], struct stat *buff, char options[])
{
    for (int i = 0; i < strlen(options); i++)
    {
        switch (options[i])
        {
        case 'n':
            printf("%s\n", filename);
            break;
        case 'd':
            printf("%ld\n", buff->st_size);
            break;
        case 'a':
            printAccessRights(buff);
            break;
        case 'c':
            printf("%d\n", countDirectoryCfiles(filename));
            break;
        default:
            break;
        }
    }
}

void handleMenu(char filename[], struct stat *buff)
{
    char input[100];
    if (S_ISREG(buff->st_mode))
    {
        printf("Regular file: %s\nEnter options:\n-n (file name)\n-d (dim/size)\n-h (number of hard links)\n-m (time of last modif)\n-a (access rights)\n-l [filename] (create a symbolic link)\n", filename);
        fgets(input, 100, stdin);
        char options[10];
        sscanf(input, "-%10s", options);
        pid_t pid = fork();
        if (pid == 0) // first child process
        {
            handleRegularFile(filename, buff, options);
            exit(0);
        }
        if (ends_with_c_extension(filename))
        {
            int pipe_fd[2];
            pipe(pipe_fd);
            pid_t pid2 = fork();
            if (pid2 == 0)  // second child process
            {
                // close read end
                close(pipe_fd[0]);
                // redirect stdout to pipe
                dup2(pipe_fd[1], STDOUT_FILENO);
                handleCfile(filename);
                exit(0);
            }
            // close write end
            close(pipe_fd[1]);
            // redirect stdin to pipe
            dup2(pipe_fd[0], STDIN_FILENO);
            char buffer[2048];
            int r = read(pipe_fd[0], buffer, 2048);
            buffer[r] = '\0';
            int warnings,errors;
            sscanf(buffer,"warnings: %d",&warnings);
            sscanf(buffer,"errors: %d",&errors);

            int score = computeScore(warnings,errors);
            FILE *f = fopen("grades.txt","w");
            fprintf(f,"%s: %d",filename,score);
            fclose(f);
            printf("Score: %d\n",score);
        }
        else
        {
            pid_t pid2 = fork();
            if (pid2 == 0) // second child process
            {
                int lines = countLinesInFile(filename);
                printf("Lines: %d\n", lines);
                exit(0);
            }
        }
    }
    else if (S_ISLNK(buff->st_mode))
    {
        printf("Symbolic link: %s\nEnter options:\n-n (link name)\n-l (delete link)\n-d (size of link)\n-z (size of target)\n-a (access rights for symbolic link)\n", filename);
        fgets(input, 10, stdin);
        char options[10];
        sscanf(input, "-%10s", options);
        pid_t pid = fork();
        if (pid == 0)
        {
            handleSymbolicLink(filename, buff, options);
            exit(0);
        }

        pid_t pid2 = fork();
        if (pid2 == 0) // second child process
        {
            char *args[] = {"chmod","u=rwx,g=rw,o=", filename, NULL};
            execvp(args[0], args);
            exit(0);
        }
    }
    else if (S_ISDIR(buff->st_mode))
    {
        printf("Directory: %s\nEnter options:\n-n (name)\n-d(dim/size)\n-a (access rights)\n-c (total number of .c files)\n", filename);
        fgets(input, 10, stdin);
        char options[10];
        sscanf(input, "-%10s", options);
        pid_t pid = fork();
        if (pid == 0) // first child process
        {
            handleDirectory(filename, buff, options);
            exit(0);
        }

        pid_t pid2 = fork();
        if (pid2 == 0) // second child process
        {
            char *args[] = {"./directoryScript.sh", filename, NULL};
            execvp(args[0], args);
            exit(0);
        }
    }
}
int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("Usage: %s [file1 file2 ...]\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    struct stat buff;
    for (int i = 1; i < argc; i++)
    {
        int result = lstat(argv[i], &buff);
        if (result == -1)
        {
            perror("lstat");
            exit(EXIT_FAILURE);
        }
        handleMenu(argv[i], &buff);
    }
    int status = 0;
    pid_t pid = 0;
    while ((pid = wait(&status)) > 0)
    {
        printf("PID %d: ",pid);
        print_exit_status(status);   
    }
}