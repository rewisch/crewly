#include <fcntl.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <threads.h>
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
    printf("\t-D - Display all Employees\n");
    return STATUS_SUCCESS;
}

int main(int argc, char *argv[])
{

    if (crewly_check_endianness() == STATUS_ERROR)
    {
        return STATUS_ERROR;
    }

    char crewly_main_database_create_flag = FALSE;
    char crewly_main_add_employee_flag = FALSE;
    char *crewly_main_database_filename = NULL;
    char crewly_main_display_all = FALSE;
    char *crewly_main_addstring = NULL;
    int crewly_main_option;
    int crewly_main_database_file_descriptor = -1;
    struct crewly_models_employee_struct *crewly_main_employees = NULL;
    struct crewly_models_databaseheader_struct *crewly_main_file_header =
        calloc(1, sizeof(struct crewly_models_databaseheader_struct));

    while ((crewly_main_option = getopt(argc, argv, "nf:a:D")) != -1)
    {
        switch (crewly_main_option)
        {
        case 'n':
            crewly_main_database_create_flag = TRUE;
            break;
        case 'a':
            crewly_main_add_employee_flag = TRUE;
            crewly_main_addstring = optarg;
            break;
        case 'D':
            crewly_main_display_all = TRUE;
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

        crewly_database_create_database_header(new_database_header);

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
        if (crewly_databaseheader_validate_header(crewly_main_database_file_descriptor,
                                                  crewly_main_file_header) == STATUS_ERROR)
        {
            return STATUS_ERROR;
        }
    }

    if (crewly_main_file_header->count != 0)
    {
        if ((crewly_main_employees = calloc(crewly_main_file_header->count,
                                            sizeof(struct crewly_models_employee_struct))) == NULL)
        {
            printf("Failed to allocate memory for employees\n");
            return -1;
        }

        crewly_database_read_employees(crewly_main_database_file_descriptor, crewly_main_employees,
                                       crewly_main_file_header->count);
    }

    if (crewly_main_add_employee_flag)
    {
        struct crewly_models_employee_struct nemp = {};

        char *name = strtok(crewly_main_addstring, ",");
        char *address = strtok(NULL, ",");
        char *hours_worked = strtok(NULL, ",");

        strcpy(nemp.name, name);
        strcpy(nemp.address, address);
        nemp.hoursWorkedTotal = atoi(hours_worked);

        crewly_main_employees =
            realloc(crewly_main_employees, (crewly_main_file_header->count + 1) *
                                               sizeof(struct crewly_models_employee_struct));
        crewly_main_employees[crewly_main_file_header->count] = nemp;

        crewly_main_file_header->count += 1;
        crewly_main_file_header->filesize += sizeof(struct crewly_models_employee_struct);

        crewly_databbase_write_database(crewly_main_database_file_descriptor,
                                        crewly_main_file_header, crewly_main_employees,
                                        crewly_main_file_header->count);
    }

    if (crewly_main_display_all)
    {
        int loops = crewly_main_file_header->count;
        for (int i = 0; i < loops; i++)
        {
            printf("Name: %s\n", crewly_main_employees[i].name);
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
