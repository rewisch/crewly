#ifndef CREWLY_MODELS_H
#define CREWLY_MODELS_H

#define HEADER_MAGIC 0x43524C59

struct crewly_models_databaseheader_struct
{
    unsigned int magic;
    unsigned short version;
    unsigned short count;
    unsigned int filesize;
};

struct crewly_models_employee_struct
{
    char name[256];
    char address[256];
    unsigned int hoursWorkedTotal;
};
#endif // CREWLY_MODELS_H
