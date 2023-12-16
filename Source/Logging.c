#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <time.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>
#include "RoboticSystem.h"

// Function to ensure that a directory exists
void ensureDirectoryExists(const char *filePath) {
    char tempPath[1024];
    strcpy(tempPath, filePath);

    // Remove the file name to get only the directory path
    char *lastSlash = strrchr(tempPath, '/');
    if (lastSlash != NULL) {
        *lastSlash = '\0';
    }

    // Check if the directory exists
    struct stat st = {0};
    if (stat(tempPath, &st) == -1) {
        printf("directory does not exist");
        
        // Create the directory if it doesn't exist
        if (mkdir(tempPath, 0700) == -1) {
            fprintf(stderr, "[L] Error creating directory: %s\n", strerror(errno));
        }
    }
}

// Function to write to log
void writeToLog(const char *file_path, const char *message) {
    ensureDirectoryExists(file_path);

    FILE *logFile = fopen(file_path, "a"); // 'a' for appending to the file
    if (logFile == NULL) {
        perror("[L] Error while opening the log file.");
        return;
    }

    // Add timestamp
    time_t rawtime;
    struct tm *timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime);

    fprintf(logFile, "[%04d-%02d-%02d %02d:%02d:%02d] %s\n",
            timeinfo->tm_year + 1900, timeinfo->tm_mon + 1, timeinfo->tm_mday,
            timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec, message);

    fclose(logFile);
}