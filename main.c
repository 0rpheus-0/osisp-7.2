#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "record.h"

int record_count;

void lock(FILE *file, int id)
{
    int fd = fileno(file);
    struct flock lock = {
        .l_type = F_WRLCK,
        .l_whence = SEEK_SET,
        .l_start = id * sizeof(struct record_s),
        .l_len = sizeof(struct record_s)};
    fcntl(fd, F_SETLKW, &lock);
}

void unlock(FILE *file, int id)
{
    int fd = fileno(file);
    struct flock lock = {
        .l_type = F_UNLCK,
        .l_whence = SEEK_SET,
        .l_start = id * sizeof(struct record_s),
        .l_len = sizeof(struct record_s)};
    fcntl(fd, F_SETLKW, &lock);
}

struct record_s read_record(FILE *file, int id)
{
    struct record_s temp;
    fseek(file, id * sizeof(struct record_s), SEEK_SET);
    fread(&temp, sizeof(struct record_s), 1, file);
    return temp;
}

void list(FILE *file)
{
    struct record_s temp;
    for (int i = 0; i < record_count; i++)
    {
        temp = read_record(file, i);
        printf("№ %d\t%s\t%s\t%d\n",
               i,
               temp.name,
               temp.address,
               temp.semester);
    }
}
void get_record(FILE *file, int id)
{
    if (id >= record_count)
    {
        printf("Bad id\n");
        return;
    }
    struct record_s temp = read_record(file, id);
    printf("№ %d\t%s\t%s\t%d\n",
           id,
           temp.name,
           temp.address,
           temp.semester);
}

void put_record(FILE *file, int id, struct record_s record)
{
    fseek(file, id * sizeof(struct record_s), SEEK_SET);
    fwrite(&record, sizeof(struct record_s), 1, file);
}

void modify_record(FILE *file, int id)
{
    if (id >= record_count)
    {
        printf("Bad id\n");
        return;
    }
    struct record_s temp;
    while (1)
    {
        struct record_s temp_save = read_record(file, id), buffer;
        memcpy(&buffer, &temp_save, sizeof(struct record_s));
        printf("№ %d\t%s\t%s\t%d\n",
               id,
               temp_save.name,
               temp_save.address,
               temp_save.semester);
        lock(file, id);

        printf("New name: ");
        scanf("%79s", temp.name);
        printf("New address: ");
        scanf("%79s", temp.address);
        printf("New semester: ");
        scanf("%d", &temp.semester);

        struct record_s temp_new = read_record(file, id);
        if (memcmp(&temp_new, &buffer, sizeof(struct record_s)) != 0)
        {
            unlock(file, id);
            //--------------------------------//
            printf("Record was modified by another process. Retrying...\n");
            continue;
        }
        break;
    }
    put_record(file, id, temp);
    unlock(file, id);
}

void new_record(FILE *file)
{
    struct record_s temp;
    printf("New name: ");
    scanf("%79s", temp.name);
    printf("New address: ");
    scanf("%79s", temp.address);
    printf("New semester: ");
    scanf("%d", &temp.semester);
    put_record(file, record_count++, temp);
}

int main()
{
    FILE *file = fopen("file", "r+");
    if (!file)
    {
        printf("Error create file\n");
        exit(1);
    }
    struct stat st;
    if (stat("file", &st) == -1)
    {
        printf("Error stat\n");
        exit(1);
    }
    record_count = st.st_size / sizeof(struct record_s);
    char opt[256];
    printf("Start program\n");
    while (scanf("%s", opt))
    {
        if (!strcmp(opt, "l"))
            list(file);
        if (!strcmp(opt, "n"))
            new_record(file);
        if (!strcmp(opt, "q"))
            break;
        int id = 0;
        if (sscanf(opt, "g%d", &id))
            get_record(file, id);
        if (sscanf(opt, "m%d", &id))
            modify_record(file, id);
    }
    fclose(file);
    return 0;
}
