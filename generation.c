#include <stdio.h>
#include "record.h"

int main()
{
    FILE *file = fopen("file", "w");
    for (int i = 0; i < 10; i++)
    {
        struct record_s record = {0};
        sprintf(record.name, "Name_%2d", i);
        sprintf(record.address, "Street_%2d", i);
        record.semester = i % 7;
        fwrite(&record, sizeof(struct record_s), 1, file);
    }
    fclose(file);
    return 0;
}