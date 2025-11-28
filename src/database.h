#ifndef CREWLY_DATABASE_H

#define CREWLY_DATABASE_H

int crewly_database_create_database(char *database_filename);
int crewly_database_open_database(char *database_filename);
int crewly_database_create_database_header(
    int database_file_descriptor, struct crewly_models_databaseheader_struct **database_header_OUT);

#define HEADER_MAGIC 0x4c4c4144

#endif // CREWLY_DATABASE_H

#ifdef CREWLY_DATABASE_IMPLEMENTATION

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

int crewly_database_create_database_header(
    int database_file_descriptor, struct crewly_models_databaseheader_struct **database_header_OUT)
{
    struct crewly_models_databaseheader_struct *database_header =
        calloc(1, sizeof(struct crewly_models_databaseheader_struct));
    database_header->version = 0x1;
    database_header->count = 0;
    database_header->magic = HEADER_MAGIC;
    database_header->filesize = sizeof(struct crewly_models_databaseheader_struct);

    *database_header_OUT = database_header;

    return STATUS_SUCCESS;
}

#endif // CREWLY_DATABASE_IMPLEMENTATION
