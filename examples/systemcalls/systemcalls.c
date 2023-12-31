#include "systemcalls.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdarg.h>
#include <fcntl.h>
/**
 * @param cmd the command to execute with system()
 * @return true if the command in @param cmd was executed
 *   successfully using the system() call, false if an error occurred,
 *   either in invocation of the system() call, or if a non-zero return
 *   value was returned by the command issued in @param cmd.
*/
bool do_system(const char *cmd)
{

/*
 * TODO  add your code here
 *  Call the system() function with the command set in the cmd
 *   and return a boolean true if the system() call completed with success
 *   or false() if it returned a failure
*/
int exit_status = system(cmd);  // Execute the system command and get its exit status

    if (exit_status == 0) {
        return true;  // The command completed successfully
    } else {
        return false; // The command returned an error
    }
    return true;
}

/**
* @param count -The numbers of variables passed to the function. The variables are command to execute.
*   followed by arguments to pass to the command
*   Since exec() does not perform path expansion, the command to execute needs
*   to be an absolute path.
* @param ... - A list of 1 or more arguments after the @param count argument.
*   The first is always the full path to the command to execute with execv()
*   The remaining arguments are a list of arguments to pass to the command in execv()
* @return true if the command @param ... with arguments @param arguments were executed successfully
*   using the execv() call, false if an error occurred, either in invocation of the
*   fork, waitpid, or execv() command, or if a non-zero return value was returned
*   by the command issued in @param arguments with the specified arguments.
*/

bool do_exec(int count, ...)
{
    va_list args;
    va_start(args, count);
    
    char *command[count + 1];
    int i;
    for(i = 0; i < count; i++) {
        command[i] = va_arg(args, char *);
    }
    command[count] = NULL;
    
    va_end(args);

    pid_t child_pid = fork();

    if (child_pid == -1) {
        // Forking failed
        return false;
    }

    if (child_pid == 0) {
        // This code is executed by the child process
        execv(command[0], command);
        // If execv returns, it indicates an error
        exit(1);
    } else {
        // This code is executed by the parent process

        int status;
        waitpid(child_pid, &status, 0);

        if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
            return true;  // Command executed successfully
        } else {
            return false;  // Command failed
        }
    }
}

/**
* @param outputfile - The full path to the file to write with command output.
*   This file will be closed at completion of the function call.
* All other parameters, see do_exec above
*/

bool do_exec_redirect(const char *outputfile, int count, ...)
{
    va_list args;
    va_start(args, count);

    char *command[count + 1];
    int i;
    for (i = 0; i < count; i++) {
        command[i] = va_arg(args, char *);
    }
    command[count] = NULL;

    va_end(args);

    // Open the output file for writing
    int fd = open(outputfile, O_WRONLY | O_TRUNC | O_CREAT, 0644);
    if (fd < 0) {
        perror("open");
        return false;
    }

    pid_t kidpid;
    switch (kidpid = fork()) {
        case -1:
            perror("fork");
            return false;
        case 0:
            // Child process
            if (dup2(fd, 1) < 0) {
                perror("dup2");
                abort();
            }
            close(fd);
            execvp(command[0], command);
            perror("execvp");
            abort();
        default:
            close(fd);
            int status;
            if (waitpid(kidpid, &status, 0) == -1) {
                perror("waitpid");
                return false;
            }
            return WIFEXITED(status) && WEXITSTATUS(status) == 0;
    }
}