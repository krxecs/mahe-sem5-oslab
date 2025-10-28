#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>

void list_files_recursively(const char *base_path) {
    char path[1024];
    DIR *dir = opendir(base_path);
    if (!dir) {
        perror("opendir failed");
        return;
    }

    struct dirent *entry;
    while ((entry = readdir(dir))) {
        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
            snprintf(path, sizeof(path), "%s/%s", base_path, entry->d_name);
            
            struct stat statbuf;
            if (stat(path, &statbuf) == -1) {
                perror("stat failed");
                continue;
            }

            if (S_ISDIR(statbuf.st_mode)) {
                printf("Directory: %s\n", path);
                list_files_recursively(path);
            } else {
                printf("File: %s\n", path);
            }
        }
    }

    closedir(dir);
}

int main(int argc, char *argv[]) {
    const char *start_dir = ".";
    if (argc > 1) {
        start_dir = argv[1];
    }
    puts("Starting file listing...");
    list_files_recursively(start_dir);
    puts("Listing complete.");
    return 0;
}

