#include <fcntl.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

#define CREWLY_DATABASE_IMPLEMENTATION

#include "common.h"
#include "models.h"
#include "database.h"

int crewly_check_endianness(void)
{
    unsigned int x = 0x01020304;
    unsigned char *p = (unsigned char *)&x;

    if (p[0] != 0x04)
    {
        printf("You are not on a little endian machine. My laziness keeps you from using this "
               "program!\n");
        printf("Sorry for that!\n");
        return STATUS_ERROR;
    }

    return STATUS_SUCCESS;
}
int crewly_print_usage(char *argv[])
{
    printf("Usage: %s -n -f <database filename>\n", argv[0]);
    printf("\t-n - create new database file\n");
    printf("\t-f - (required) path to database file\n");
    return STATUS_SUCCESS;
}

int main(int argc, char *argv[])
{

    if (crewly_check_endianness() == STATUS_ERROR)
    {
        return STATUS_ERROR;
    }

    char crewly_main_database_create_flag = FALSE;
    char *crewly_main_database_filename = NULL;
    int crewly_main_option;
    int crewly_main_database_file_descriptor = -1;
    struct crewly_models_databaseheader_struct crewly_main_file_header = {0};

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

        struct crewly_models_databaseheader_struct *new_database_header =
            malloc(sizeof(struct crewly_models_databaseheader_struct));

        crewly_database_create_database_header(crewly_main_database_file_descriptor,
                                               new_database_header);

        write(crewly_main_database_file_descriptor, new_database_header,
              sizeof(struct crewly_models_databaseheader_struct));

        free(new_database_header);
    }
    else
    {
        crewly_main_database_file_descriptor =
            crewly_database_open_database(crewly_main_database_filename);
        if (crewly_main_database_file_descriptor < 0)
        {
            return STATUS_ERROR;
        }

        // Make sure the file wasn't tempered with
        if (crewly_databaseheader_validate_header(crewly_main_database_file_descriptor) ==
            STATUS_ERROR)
        {
            return STATUS_ERROR;
        }
    }

    // CLEANUP
    if (close(crewly_main_database_file_descriptor) < 0)
    {
        perror("close");
        return STATUS_ERROR;
    };

    return STATUS_SUCCESS;
}
