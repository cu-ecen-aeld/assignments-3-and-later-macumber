#include "systemcalls.h"

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

/**
 * @param cmd the command to execute with system()
 * @return true if the command in @param cmd was executed
 *   successfully using the system() call, false if an error occurred,
 *   either in invocation of the system() call, or if a non-zero return
 *   value was returned by the command issued in @param cmd.
*/
bool do_system(const char *cmd)
{
    int result = system(cmd);

    return (result == 0);
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
    if (count < 1)
    {
        return false;
    }

    va_list args;
    va_start(args, count);
    char * command[count+1];
    int i;
    for(i=0; i<count; i++)
    {
        command[i] = va_arg(args, char *);
    }
    command[count] = NULL;
    va_end(args);

    fflush(stdout);

    pid_t pid = fork();

    if (pid == -1) {
        perror("error with fork");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        // this is the child
        int result = execv(command[0], command);
        if (result == 0)
        {
            exit(EXIT_SUCCESS);
        }
        exit(EXIT_FAILURE);

    } else {                    
        // this is the parent

        // wait for the child to complete
        int wstatus;
        pid_t pid2 = waitpid(pid, &wstatus, 0);
        if (pid2 == -1) {
            perror("waitpid");
            return false;
        }

        if (WIFEXITED(wstatus)) {
            if (WEXITSTATUS(wstatus) == 0)
            {
                return true;
            }
        }
    }
    
    return false;
}

/**
* @param outputfile - The full path to the file to write with command output.
*   This file will be closed at completion of the function call.
* All other parameters, see do_exec above
*/
bool do_exec_redirect(const char *outputfile, int count, ...)
{
    int fd = open(outputfile, O_WRONLY|O_TRUNC|O_CREAT, 0644);
    if (fd < 0) { 
        perror("open"); 
        abort(); 
    }

    if (count < 1)
    {
        return false;
    }

    va_list args;
    va_start(args, count);
    char * command[count+1];
    int i;
    for(i=0; i<count; i++)
    {
        command[i] = va_arg(args, char *);
    }
    command[count] = NULL;
    va_end(args);

    fflush(stdout);

    pid_t pid = fork();

    if (pid == -1) {
        perror("error with fork");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        // this is the child
        if (dup2(fd, 1) < 0) { 
            perror("dup2"); 
            abort();
        }
    
        int result = execv(command[0], command);
        close(fd);

        if (result == 0)
        {
            exit(EXIT_SUCCESS);
        }
        exit(EXIT_FAILURE);

    } else {                    
        // this is the parent
        close(fd);

        // wait for the child to complete
        int wstatus;
        pid_t pid2 = waitpid(pid, &wstatus, 0);
        if (pid2 == -1) {
            perror("waitpid");
            return false;
        }

        if (WIFEXITED(wstatus)) {
            if (WEXITSTATUS(wstatus) == 0)
            {
                return true;
            }
        }
    }
    
    return false;
}
