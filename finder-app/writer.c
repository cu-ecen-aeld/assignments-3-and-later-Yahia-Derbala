#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>

int main(int argc, char *argv[]) {
    // Check if the required arguments are provided
    if (argc != 3) {
        fprintf(stderr, "Error: Two arguments are required.\n");
        exit(1);
    }

    const char *writefile = argv[1];
    const char *writestr = argv[2];

    // Open the syslog connection with LOG_USER facility
    openlog("writer", LOG_PID, LOG_USER);

    // Log a debug message with the content being written
    syslog(LOG_DEBUG, "Writing %s to %s", writestr, writefile);

    // Create a file and write the content to it
    FILE *file = fopen(writefile, "w");
    if (file == NULL) {
        syslog(LOG_ERR, "Error: Failed to create the file.");
        fprintf(stderr, "Error: Failed to create the file.\n");
        closelog();
        exit(1);
    }

    fprintf(file, "%s", writestr);
    fclose(file);

    // Close the syslog connection
    closelog();

    // Print a success message
    printf("File created: %s\n", writefile);

    return 0;
}

