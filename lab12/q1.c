#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>

void print_permissions(mode_t mode) {
    putchar((S_ISDIR(mode)) ? 'd' : '-');
    putchar((mode & S_IRUSR) ? 'r' : '-');
    putchar((mode & S_IWUSR) ? 'w' : '-');
    putchar((mode & S_IXUSR) ? 'x' : '-');
    putchar((mode & S_IRGRP) ? 'r' : '-');
    putchar((mode & S_IWGRP) ? 'w' : '-');
    putchar((mode & S_IXGRP) ? 'x' : '-');
    putchar((mode & S_IROTH) ? 'r' : '-');
    putchar((mode & S_IWOTH) ? 'w' : '-');
    putchar((mode & S_IXOTH) ? 'x' : '-');
}

int main(int argc, char *argv[]) {
    const char *dir_path = ".";
    if (argc > 1) {
        dir_path = argv[1];
    }

    DIR *dir = opendir(dir_path);
    if (!dir) {
        perror("opendir failed");
        return EXIT_FAILURE;
    }

    struct dirent *entry;
    while ((entry = readdir(dir))) {
        char full_path[1024];
        snprintf(full_path, sizeof(full_path), "%s/%s", dir_path, entry->d_name);

        struct stat statbuf;
        if (stat(full_path, &statbuf) == -1) {
            perror("stat failed");
            continue;
        }

        print_permissions(statbuf.st_mode);
        printf(" %2lu", statbuf.st_nlink);

        struct passwd *pw = getpwuid(statbuf.st_uid);
        if (pw) {
            printf(" %s", pw->pw_name);
        } else {
            printf(" %d", statbuf.st_uid);
        }

        struct group *gr = getgrgid(statbuf.st_gid);
        if (gr) {
            printf(" %s", gr->gr_name);
        } else {
            printf(" %d", statbuf.st_gid);
        }

        printf(" %6ld", statbuf.st_size);

        char time_buf[20];
        strftime(time_buf, sizeof(time_buf), "%b %d %H:%M", localtime(&statbuf.st_mtime));
        printf(" %s", time_buf);

        printf(" %s\n", entry->d_name);
    }

    closedir(dir);
    return EXIT_SUCCESS;
}

