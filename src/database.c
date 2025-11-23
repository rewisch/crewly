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
