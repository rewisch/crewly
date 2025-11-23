#include <fcntl.h>
#include <getopt.h>
#include <stdio.h>
#include <unistd.h>

#include "common.c"
#include "database.c"

int crewly_print_usage(char *argv[])
{
    printf("Usage: %s -n -f <database filename>\n", argv[0]);
    printf("\t-n - create new database file\n");
    printf("\t-f - (required) path to database file\n");
    return STATUS_SUCCESS;
}

int main(int argc, char *argv[])
{

    char crewly_main_database_create_flag = FALSE;
    char *crewly_main_database_filename = NULL;
    int crewly_main_option;
    int crewly_main_database_file_descriptor = -1;

    while ((crewly_main_option = getopt(argc, argv, "nf:")) != -1)
    {
        switch (crewly_main_option)
        {
        case 'n':
            crewly_main_database_create_flag = TRUE;
            break;
        case 'f':
            crewly_main_database_filename = optarg;
            break;
        case '?':
            printf("Unknown option -%c\n",
                   crewly_main_option); // NOTE: Logic probably corrupted, prints '?'
        }
    }

    if (crewly_main_database_filename == NULL)
    {
        printf("Filepath is a required argument\n");
        crewly_print_usage(argv);
        return STATUS_ERROR;
    }

    if (crewly_main_database_create_flag)
    {

        crewly_main_database_file_descriptor =
            crewly_database_create_database(crewly_main_database_filename);
        if (crewly_main_database_file_descriptor < 0)
        {
            return STATUS_ERROR;
        }
        printf("The filehandler of the newly created file we obtained is %d\n",
               crewly_main_database_file_descriptor);
    }
    else
    {
        crewly_main_database_file_descriptor =
            crewly_database_open_database(crewly_main_database_filename);
        if (crewly_main_database_file_descriptor < 0)
        {
            return STATUS_ERROR;
        }
        printf("The filehandler of already existing file we obtained is %d\n",
               crewly_main_database_file_descriptor);
    }

    // CLEANUP
    if (close(crewly_main_database_file_descriptor) < 0)
    {
        perror("close");
        return STATUS_ERROR;
    };

    return STATUS_SUCCESS;
}
