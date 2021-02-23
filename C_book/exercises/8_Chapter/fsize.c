/* fsize: get files from directories recursively, printing file size 
 *
 * created by Anvesh G. Jhuboo
 * on Feb/22/2021
 */

#include <stdio.h>
#include <string.h>
#include "syscall.h"
#include <fcntl.h>      /* flags for read and write */
#include <sys/types.h>  /* typedefs */
#include <sys/stat.h>   /* structure returned by stat */
#include "dirent.h"
#include <time.h>

void fsize(char *);

/* print file sizes */
int main(int argc, char *argv[])
{
    if (argc == 1)      /* default: current directory */
        fsize(".");
    else
        while (--argc > 0)
            fsize(*++argv);
    return 0;
}

int stat(char *, struct stat *);
void dirwalk(char *, void (*fcn)(char *));

/* fsize: print size of file "name" */
void fsize(char *name)
{
    struct stat stbuf;

    if (stat(name, &stbuf) == -1) {
        fprintf(stderr, "fsize: can't access %s\n", name);
        return;
    }

    if ((stbuf.st_mode & S_IFMT) == S_IFDIR)
        dirwalk(name, fsize);

    /* user & group id */
    printf("%d %d ", stbuf.st_uid, stbuf.st_gid);

    /* more specific info */
    printf("%5u %6o %3u %8ld\n", stbuf.st_ino,
            stbuf.st_mode, stbuf.st_nlink, stbuf.st_size);

    /* time last modified */
    struct tm mt;
    mt = *localtime(&stbuf.st_mtime);
    printf("%4d-%02d-%02d %02d:%02d ", (1900 + mt.tm_year), mt.tm_mon,
            mt.tm_mday, mt.tm_hour, mt.tm_min);

    /* file name */
    printf("%s\n", name);
}


#define MAX_PATH 1024

/* dirwalk: apply fcn to all files in dir */
void dirwalk(char *dir, void (*fcn)(char *))
{
    char name[MAX_PATH];
    Dirent *dp;
    DIR *dfd;

    if ((dfd = opendir(dir)) == NULL) {
        fprintf(stderr, "dirwalk: can't open %s\n", dir);
        return;
    }

    while ((dp = readdir(dfd)) != NULL) {
        if (strcmp(dp->name, ".") == 0
                || strcmp(dp->name, "..") == 0)
            continue;       /* skip self and parent */

        if (strlen(dir)+strlen(dp->name)+2 > sizeof(name))
            fprintf(stderr, "dirwalk: name %s/%s too long\n", dir, dp->name);
        else {
            sprintf(name, "%s/%s", dir, dp->name);
            (*fcn)(name);
        }
    }
    closedir(dfd);
}

int fstat(int fd, struct stat *);

/* opendir: open a directory for readdir calls */
DIR *opendir(char *dirname)
{
    int fd;
    struct stat stbuf;
    DIR *dp;

    if ((fd = open(dirname, O_RDONLY, 0)) == -1
            || fstat(fd, &stbuf) == -1
            || (stbuf.st_mode & S_IFMT) != S_IFDIR
            || (dp = (DIR *) malloc(sizeof(DIR))) == NULL)
        return NULL;

    dp->fd = fd;
    return dp;
}


/* closedir: close directory opened by opendir */
void closedir(DIR *dp)
{
    if (dp) {
        close(dp->fd);
        free(dp);
    }
}

#include <sys/dir.h>    /* local directory structure */

/* readdir: read directory entries in sequence */
Dirent *readdir(DIR *dp)
{
    struct direct dirbuf;   /* local directory structure */
    static Dirent d;        /* return: portable structure */

    while (read(dp->fd, (char *) &dirbuf, sizeof(dirbuf))
            == sizeof(dirbuf)) {
        if (dirbuf.d_ino == 0)      /* slot not in use */
            continue;
        d.ino = dirbuf.d_ino;
        strncpy(d.name, dirbuf.d_name, DIRSIZ);
        d.name[DIRSIZ] = '\0';      /* ensure termination */
        return &d;
    }

    return NULL;
}
