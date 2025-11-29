#ifndef CREWLY_DATABASE_H
#define CREWLY_DATABASE_H

#include "common.h"
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
    int database_file_descriptor, struct crewly_models_databaseheader_struct *database_header_OUT);

#endif // CREWLY_DATABASE_H

#ifdef CREWLY_DATABASE_IMPLEMENTATION

int crewly_database_print_header_corruption(struct crewly_models_databaseheader_struct **header)
{
    printf("Corrupt database header. Exit ...\n");
    free(*header);

    return STATUS_SUCCESS;
}

int crewly_databaseheader_validate_header(int fd)
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
        printf("Failed to allocate memory\n");
        return STATUS_ERROR;
    }

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
        crewly_database_print_header_corruption(&header);
        free(header);
        return STATUS_ERROR;
    }

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

int crewly_database_create_database_header(int database_file_descriptor,
                                           struct crewly_models_databaseheader_struct *out_header)
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
