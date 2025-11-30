#ifndef CREWLY_DATABASE_H
#define CREWLY_DATABASE_H

#include "common.h"
#include <stddef.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "models.h"

int crewly_database_create_database(char *database_filename);
int crewly_database_open_database(char *database_filename);
int crewly_database_create_database_header(
    struct crewly_models_databaseheader_struct *database_header_OUT);

#endif // CREWLY_DATABASE_H

#ifdef CREWLY_DATABASE_IMPLEMENTATION

int crewly_database_print_header_corruption(void)
{
    printf("Corrupt database header. Exit ...\n");

    return STATUS_SUCCESS;
}

int crewly_database_read_employees(int fd, struct crewly_models_employee_struct *employees,
                                   size_t number_of_employees_to_read)
{
    if (fd < 0)
    {
        printf("%d is an invalid file descriptor\n", fd);
        return STATUS_ERROR;
    }

    // make sure we alrways start to read employees after the header.
    lseek(fd, sizeof(struct crewly_models_databaseheader_struct), 0);

    size_t bytes_to_read =
        number_of_employees_to_read * sizeof(struct crewly_models_employee_struct);
    ssize_t bytes_read = 0;

    bytes_read = read(fd, employees, bytes_to_read);

    if (bytes_read < 0 || (size_t)bytes_read != bytes_to_read)
    {
        printf(
            "Error while reading the file. I know, according to the documentation it is possible "
            "and proabbly even common that one read does not give the entire asked size back. If "
            "you see this, contact me and be angy and demand that i write better software!");
        return STATUS_ERROR;
    }

    return STATUS_SUCCESS;
}

int crewly_databbase_write_database(int fd, struct crewly_models_databaseheader_struct *header,
                                    struct crewly_models_employee_struct *employees,
                                    int number_of_employees_to_write)
{
    ssize_t employee_bytes_to_write =
        number_of_employees_to_write * sizeof(struct crewly_models_employee_struct);
    ssize_t header_bytes_to_write = sizeof(struct crewly_models_databaseheader_struct);

    lseek(fd, 0, 0);
    if (write(fd, header, header_bytes_to_write) != header_bytes_to_write)
    {
        perror("write");
        return STATUS_ERROR;
    }
    if (write(fd, employees, employee_bytes_to_write) != employee_bytes_to_write)
    {
        perror("write");
        return STATUS_ERROR;
    }
    if (ftruncate(fd, lseek(fd, 0, SEEK_CUR)) != 0)
    {
        perror("ftruncate");
        return STATUS_ERROR;
    }

    printf("Written %ld bytes... Sucessfully saved!",
           employee_bytes_to_write + header_bytes_to_write);
    return STATUS_SUCCESS;
}

int crewly_databaseheader_validate_header(int fd,
                                          struct crewly_models_databaseheader_struct *main_header)
{
    if (fd < 0)
    {
        printf("%d is an invalid file-descriptor\n", fd);
        return STATUS_ERROR;
    }

    struct crewly_models_databaseheader_struct *header =
        calloc(1, sizeof(struct crewly_models_databaseheader_struct));
    if (header == NULL)
    {
        printf("Failed io allocate memory\n");
        return STATUS_ERROR;
    }

    // make sure we always read the header from the start of the file
    // although this function should probably not read the file by itself ... it does ...
    lseek(fd, 0, 0);
    if (read(fd, header, sizeof(struct crewly_models_databaseheader_struct)) !=
        sizeof(struct crewly_models_databaseheader_struct))
    {
        printf("error while reading the header from database file\n");
        free(header);
        return STATUS_ERROR;
    }

    struct stat dbstat = {0};
    fstat(fd, &dbstat);

    if (header->version != 1 || header->magic != HEADER_MAGIC || header->filesize != dbstat.st_size)
    {
        crewly_database_print_header_corruption();
        free(header);
        return STATUS_ERROR;
    }

    main_header->magic = header->magic;
    main_header->version = header->version;
    main_header->count = header->count;
    main_header->filesize = header->filesize;

    free(header);
    return STATUS_SUCCESS;
}

int crewly_database_create_database(char *database_filename)
{
    // check if file already exists and panic if that's the case
    int crewly_database_file_descriptor = open(database_filename, O_RDWR);
    if (crewly_database_file_descriptor > -1)
    {
        close(crewly_database_file_descriptor);
        printf("File %s already exists\n", database_filename);
        return STATUS_ERROR;
    }

    crewly_database_file_descriptor = open(database_filename, O_RDWR | O_CREAT, 0644);

    if (crewly_database_file_descriptor < 0)
    {
        perror("open");
        return STATUS_ERROR;
    }

    return crewly_database_file_descriptor;
}

int crewly_database_open_database(char *database_filename)
{
    int crewly_database_file_descriptor = open(database_filename, O_RDWR);
    if (crewly_database_file_descriptor < 0)
    {
        perror("open");
        return STATUS_ERROR;
    }
    else
    {
        return crewly_database_file_descriptor;
    }
}

int crewly_database_create_database_header(struct crewly_models_databaseheader_struct *out_header)
{
    if (out_header == NULL)
    {
        return STATUS_ERROR;
    }
    out_header->version = 0x1;
    out_header->count = 0;
    out_header->magic = HEADER_MAGIC;
    out_header->filesize = sizeof(struct crewly_models_databaseheader_struct);

    return STATUS_SUCCESS;
}

#endif // CREWLY_DATABASE_IMPLEMENTATION
