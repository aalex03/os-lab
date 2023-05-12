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

void handleCfile(char filename[])
{
    char *args[] = {"./compileScript.sh", filename, NULL};
    execvp(args[0], args);
}

int ends_with_c_extension(char filename[])
{
    int len = strlen(filename);
    if (len < 3)
    { // The string is too short to contain a ".c" extension
        return 0;
    }
    return (strcmp(&filename[len - 2], ".c") == 0);
}

void print_exit_status(int status)
{
    if (WIFEXITED(status))
    {
        printf("Child exited with status %d\n", WEXITSTATUS(status));
    }
    else if (WIFSIGNALED(status))
    {
        printf("Child was terminated by signal %d\n", WTERMSIG(status));

    }
    else if (WIFSTOPPED(status))
    {
        printf("Child was stopped by signal %d\n", WSTOPSIG(status));
    }
    else if (WIFCONTINUED(status))
    {
        printf("Child was resumed by delivery of SIGCONT\n");
    }
    else
    {
        printf("Unknown status\n");
    }
}

int computeScore(int errors, int warnings)
{
    if(errors == 0 && warnings == 0)
    {
        return 10;
    }
    else if(errors > 0)
    {
        return 1;
    }
    else if(warnings > 10)
    {
        return 2;
    }
    else
    {
        return 2 + 8*(10-warnings)/10;
    }
}

int countLinesInFile(char filename[])
{
    FILE *fp = fopen(filename, "r");
    int lineCount = 0;
    char ch,prevCh = '\0';
    ch = getc(fp);
    if(ch != EOF)
    {
        lineCount = 1; // we have at least one line
    }
    ungetc(ch, fp); // put the character back (we don't want to skip the first line)
    while((ch = fgetc(fp)) != EOF)
    {
        if(ch == '\n')
        {
            if(prevCh != '\n')  // exclude consecutive newlines
            {
                lineCount++;
            }
        }
        prevCh = ch;
    }
    fclose(fp);
    return lineCount;
}