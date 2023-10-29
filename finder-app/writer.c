#include <stdio.h>
#include <syslog.h>

int main(int argc, char *argv[])
{
    openlog("writer", LOG_PID, LOG_USER);

    if( argc != 3 ) {
        printf("Usage: writer writefile writestr");
        syslog(LOG_ERR, "Usage: writer writefile writestr");
        return 1;
    }

    FILE *file;
    if ((file = fopen(argv[1], "w")))
    {
        syslog(LOG_DEBUG, "Writing %s to %s", argv[1], argv[2]);
        fprintf(file, "%s", argv[2]);
        fclose(file);
        return 0;
    }

    syslog(LOG_ERR, "Error opening file %s", argv[1]);
    return 1;
}