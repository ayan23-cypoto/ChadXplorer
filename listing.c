#include <conio.h>
#include <sys/stat.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <wchar.h>
#include <locale.h>
#include <stdio.h>

#define ROOT_PATH "/"

int main() {
    system("color 0a");
    struct dirent *d;
    struct stat st;

    DIR *dr;
    char drive;
    char path[100];

    printf("Select the drive (C or D): ");
    scanf(" %c", &drive);

    if (drive == 'C' || drive == 'c') {
        strcpy(path, "C:\\");
    } else if (drive == 'D' || drive == 'd') {
        strcpy(path, "D:\\");
    } else {
        printf("Invalid drive selection. Exiting...\n");
        return 0;
    }

    char* previousPath = NULL;

    while (1) {
        dr = opendir(path);

        if (dr != NULL) {
            int counter = 1;
            while ((d = readdir(dr)) != NULL) {
                // Skip current directory and parent directory entries
                if (strcmp(d->d_name, ".") == 0 || strcmp(d->d_name, "..") == 0) {
                    continue;
                }

                char* type = "";

                if (stat(d->d_name, &st) == 0) {
                    if (S_ISDIR(st.st_mode)) {
                        type = "Folder";
                    } else if (S_ISREG(st.st_mode)) {
                        type = "File";
                    } else {
                        type = "Unknown";
                    }
                }

                printf("%d. %s || %s\n", counter, d->d_name, type);
                counter++;
            }

            closedir(dr);

            int selectedCount;
            printf("Enter the count to change directory (-1 to go back, 0 to exit): ");
            scanf("%d", &selectedCount);

            if (selectedCount == 0) {
                printf("Exiting...\n");
                break;
            }

            if (selectedCount == -1 && previousPath != NULL) {
                strcpy(path, previousPath);
                free(previousPath);
                previousPath = NULL;
                printf("Changed directory to: %s\n", path);
                continue;
            } else if (previousPath == NULL && selectedCount == -1) {
                printf("Select the drive (C or D): ");
                scanf(" %c", &drive);

                if (drive == 'C' || drive == 'c') {
                    strcpy(path, "C:\\");
                } else if (drive == 'D' || drive == 'd') {
                    strcpy(path, "D:\\");
                } else {
                    printf("Invalid drive selection. Exiting...\n");
                    return 0;
                }
            } else {
                dr = opendir(path);
                counter = 1;

                while ((d = readdir(dr)) != NULL) {
                    // Skip current directory and parent directory entries
                    if (strcmp(d->d_name, ".") == 0 || strcmp(d->d_name, "..") == 0) {
                        continue;
                    }

                    if (counter == selectedCount) {

                        char newPath[100];
                        snprintf(newPath, sizeof(newPath), "%s\\%s", path, d->d_name);

                        if (stat(newPath, &st) == 0) {
                            if (S_ISDIR(st.st_mode)) {
                                if (previousPath != NULL) {
                                    free(previousPath);
                                }

                                previousPath = strdup(path);
                                strcpy(path, newPath);
                                printf("Changed directory to: %s\n", path);

                                break;
                            } else if (S_ISREG(st.st_mode)) {
                                printf("Opening file: %s\n", newPath);
                                ShellExecute(NULL, "open", newPath, NULL, NULL, SW_SHOWNORMAL);
                            } else {
                                printf("Invalid entry. Please select a folder or file.\n");
                            }
                        } else {
                            printf("Invalid entry. Please select a folder or file.\n");
                        }
                    }

                    counter++;
                }

                closedir(dr);
                system("cls");
            }
        } else {
            printf("Could not open directory.\n");
        }

        char searchQuery[100];
        printf("Enter a directory name to search (or enter to skip): ");
        fgets(searchQuery, sizeof(searchQuery), stdin);
        searchQuery[strcspn(searchQuery, "\n")] = '\0';

        if (strlen(searchQuery) > 0 && searchQuery != "!delete" && searchQuery != "!insert") {
            dr = opendir(path);

            if (dr != NULL) {
                int counter = 1;
                int isSearchMatched = 0;

                while ((d = readdir(dr)) != NULL) {
                    // Skip current directory and parent directory entries
                    if (strcmp(d->d_name, ".") == 0 || strcmp(d->d_name, "..") == 0) {
                        continue;
                    }

                    if (strstr(d->d_name, searchQuery) != NULL) {
                        char newPath[100];
                        snprintf(newPath, sizeof(newPath), "%s\\%s", path, d->d_name);

                        if (stat(newPath, &st) == 0 && S_ISDIR(st.st_mode)) {
                            printf("Match found: %s\n", newPath);
                            isSearchMatched = 1;

                            printf("Do you want to open this directory? (Y/N): ");
                            char choice;
                            scanf(" %c", &choice);

                            if (choice == 'Y' || choice == 'y') {
                                if (S_ISDIR(st.st_mode)) {
                                    if (previousPath != NULL) {
                                        free(previousPath);
                                    }

                                    previousPath = strdup(path);
                                    strcpy(path, newPath);
                                    printf("Changed directory to: %s\n", path);

                                    break;
                                } else if (S_ISREG(st.st_mode)) {
                                    printf("Opening file: %s\n", newPath);
                                    ShellExecute(NULL, "open", newPath, NULL, NULL, SW_SHOWNORMAL);
                                } else {
                                    printf("Invalid entry. Please select a folder or file.\n");
                                }
                            }
                        }
                    }
                }

                closedir(dr);

                if (!isSearchMatched) {
                    printf("No matches found.\n");
                }
            } else {
                printf("Could not open directory.\n");
            }

            getchar();
            system("cls");
        } else if (strcmp(searchQuery, "!delete") == 0) {
            printf("Enter the count of the item to delete: ");
            int deleteCount;
            scanf("%d", &deleteCount);

            dr = opendir(path);
            int counter = 1;
            int found = 0;

            while ((d = readdir(dr)) != NULL) {
                if (strcmp(d->d_name, ".") == 0 || strcmp(d->d_name, "..") == 0) {
                    continue;
                }

                if (counter == deleteCount) {
                    found = 1;
                    char deletePath[100];
                    snprintf(deletePath, sizeof(deletePath), "%s\\%s", path, d->d_name);

                    if (stat(deletePath, &st) == 0) {
                        if (S_ISDIR(st.st_mode)) {
                            if (rmdir(deletePath) == 0) {
                                printf("Directory deleted: %s\n", deletePath);
                            } else {
                                printf("Failed to delete directory: %s\n", deletePath);
                            }
                        } else if (S_ISREG(st.st_mode)) {
                            if (remove(deletePath) == 0) {
                                printf("File deleted: %s\n", deletePath);
                            } else {
                                printf("Failed to delete file: %s\n", deletePath);
                            }
                        } else {
                            printf("Invalid entry. Please select a folder or file.\n");
                        }
                    } else {
                        printf("Invalid entry. Please select a folder or file.\n");
                    }

                    break;
                }

                counter++;
            }
            closedir(dr);

if (strcmp(searchQuery, "!insert") == 0) {
    int insertCount;
    printf("Enter the count of the item to insert: ");
    scanf("%d", &insertCount);

    dr = opendir(path);
    int counter = 1;
    int found = 0;

    while ((d = readdir(dr)) != NULL) {
        if (strcmp(d->d_name, ".") == 0 || strcmp(d->d_name, "..") == 0) {
            continue;
        }

        if (counter == insertCount) {
            found = 1;
            char insertPath[100];
            snprintf(insertPath, sizeof(insertPath), "%s\\%s.txt", path, d->d_name);

            char content[1000];
            printf("Enter the content to insert: ");
            scanf(" %[^\n]s", content);

            FILE *file;
            file = fopen(insertPath, "w");

            if (file == NULL) {
                printf("Unable to create the file.\n");
                return 1;
            }

            fprintf(file, "%s\n", content);

            fclose(file);

            printf("File created and content inserted successfully.\n");
            break;
        }

        counter++;
    }

    if (!found) {
        printf("Invalid count. Please try again.\n");
    }

    closedir(dr);
    getchar();
    system("cls");
}
            

            if (!found) {
                printf("Invalid count. Please try again.\n");
            }

            closedir(dr);
            getchar();
            system("cls");
        }
    }

    if (previousPath != NULL) {
        free(previousPath);
    }

    return 0;
}