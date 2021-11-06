#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>

void generate_hex_nums(const char *filename, size_t bytes, size_t count)
{
    int fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU);
    // O_WRONLY - Открыть на запись
    // O_CREAT - Создать если не существует 
    // O_TRUNC - обрезать до начала если существует и в нем что-то написано
    // S_IRWXU - Выдать права на файл (Чтение, Запись, Исполнение)
    char buf[bytes];

    srand(time(NULL)); // Ставим сид для рандомного генератора от текущего времени
    
    for (int i = 0; i < count; ++i) {
        for (int i = 0; i < bytes; ++i) {
            if (rand() % 2 == 0) {
                buf[i] = '0' + (rand() % 10);
            } else {
                buf[i] = 'A' + (rand() % 6);
            }
        }
        write(fd, &buf, bytes);
        write(fd, "\n", 1);
    }
    close(fd);
}

int main(int argc, const char **argv)
{
    const char *filename;
    int count;
    int bytes = 32;

    if (argc < 3 || argc > 4)
    {
        printf("Usage: %s [filename] [count] (optional [bytes])\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    else
    {
        filename = argv[1];
        count = atoi(argv[2]);
        if (argc == 4) bytes = atoi(argv[3]);
        
    }

    generate_hex_nums(filename, bytes, count);

    return 0;
}