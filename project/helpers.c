#include "helpers.h"
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

pid_t handleCfile(char filename[])
{
    printf("C file: %s\n", filename);
    char *args[] = {"./compileScript.sh", filename, NULL};
    pid_t pid = fork();
    if (pid == 0)
    {
        execvp(args[0], args);
        exit(0);
    }
    return pid;
}

int ends_with_c_extension(char filename[])
{
    int len = strlen(filename);
    if (len < 3) { // The string is too short to contain a ".c" extension
        return 0;
    }
    return (strcmp(&filename[len-2], ".c") == 0);
}