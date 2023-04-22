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

void printAccessRights(struct stat *file)
{
    printf("USER\n");
    printf("\tRead: %s\n", (file->st_mode & S_IRUSR) ? "Yes" : "No");
    printf("\tWrite: %s\n", (file->st_mode & S_IWUSR) ? "Yes" : "No");
    printf("\tExecute: %s\n", (file->st_mode & S_IXUSR) ? "Yes" : "No");
    printf("GROUP\n");
    printf("\tRead: %s\n", (file->st_mode & S_IRGRP) ? "Yes" : "No");
    printf("\tWrite: %s\n", (file->st_mode & S_IWGRP) ? "Yes" : "No");
    printf("\tExecute: %s\n", (file->st_mode & S_IXGRP) ? "Yes" : "No");
    printf("OTHER\n");
    printf("\tRead: %s\n", (file->st_mode & S_IROTH) ? "Yes" : "No");
    printf("\tWrite: %s\n", (file->st_mode & S_IWOTH) ? "Yes" : "No");
    printf("\tExecute: %s\n", (file->st_mode & S_IXOTH) ? "Yes" : "No");
}

void createSymlink(char filename[])
{
    puts("Enter linkname: ");
    char linkname[100];
    scanf("%s", linkname);
    symlink(filename, linkname);
}

void printLinkedFile(char filename[])
{
    char linkname[100];
    readlink(filename, linkname, 100);
    struct stat buff;
    lstat(linkname, &buff);
    printf("Size of target: %ld\n", buff.st_size);
}

int countDirectoryCfiles(char filename[])
{
    DIR *dir;
    dir = opendir(filename);
    struct dirent *entry;
    int count = 0;
    while ((entry = readdir(dir)) != NULL)
    {
        char *name = entry->d_name;
        if (strstr(name, ".c") != NULL)
        {
            count++;
        }
    }

    return count;
}

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
    char input[10];
    if (S_ISREG(buff->st_mode))
    {
        printf("Regular file: %s\nEnter options:\n-n (file name)\n-d(dim/size)\n-h (number of hard links)\n-m (time of last modif)\n-a (access rights)\n-l [filename] (create a symbolic link)\n", filename);
        fgets(input, 10, stdin);
        char options[10];
        sscanf(input, "-%10s", options);
        // TODO fork here
        handleRegularFile(filename, buff, options);
    }
    else if (S_ISLNK(buff->st_mode))
    {
        printf("Symbolic link: %s\nEnter options:\n-n (link name)\n-l (delete link)\n-d (size of link)\n-z (size of target)\n-a (access rights for symbolic link)\n", filename);
        fgets(input, 10, stdin);
        char options[10];
        sscanf(input, "-%10s", options);
        handleSymbolicLink(filename, buff, options);
    }
    else if (S_ISDIR(buff->st_mode))
    {
        printf("Directory: %s\nEnter options:\n-n (name)\n-d(dim/size)\n-a (access rights)\n-c (total number of .c files)\n", filename);
        fgets(input, 10, stdin);
        char options[10];
        sscanf(input, "-%10s", options);
        handleDirectory(filename, buff, options);
    }
}
void handleCfile(char filename[])
{
    printf("C file: %s\n", filename);
    char *args[] = {"./compileScript.sh", filename, NULL};
    pid_t pid = fork();
    if (pid == 0)
    {
        execvp(args[0], args);
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
    pid_t pids[argc - 1];
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
    for (int i = 0; i < argc - 1; i++)
    {
        waitpid(pids[i], NULL, 0);
    }
}