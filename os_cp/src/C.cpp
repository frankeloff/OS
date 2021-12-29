#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <semaphore.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdarg.h>
#include <signal.h>

int32_t sem_get(sem_t *sem)
{
    int s;
    sem_getvalue(sem, &s);
    return s;
}

void sem_set(sem_t *sem, int n)
{
    while (sem_get(sem) < n)
    {
        sem_post(sem);
    }
    while (sem_get(sem) > n)
    {
        sem_wait(sem);
    }
}

int main(int args, char* argv[])
{
    int32_t fdAC[2];
    int32_t fdBC[2];
    fdAC[0] = atoi(argv[0]);
    fdBC[1] = atoi(argv[1]);
    sem_t *semA = sem_open("SemA", O_CREAT, S_IRWXU, 1);
    sem_t *semB = sem_open("SemB", O_CREAT, S_IRWXU, 2);
    sem_t *semC = sem_open("SemC", O_CREAT, S_IRWXU, 2);
    if ((semA == SEM_FAILED)||(semB == SEM_FAILED)||(semC == SEM_FAILED))
    {
        std::cout << "SEM_FAILED" << std::endl;
        return 0;
    }
    while(1)
    {
        while(sem_get(semC) == 2)
        {
            continue;
        }
        if (sem_get(semC) == 0)
        {
            sem_close(semA);
            sem_close(semB);
            sem_close(semC);
            sem_destroy(semA);
            sem_destroy(semB);
            sem_destroy(semC);
            close(fdAC[0]);
            close(fdBC[1]);
            exit(0);
        }
        int size;
        std::string str;
        read(fdAC[0], &size, sizeof(int));
        int count_size = 0;
        for (int i = 0; i < size; ++i)
        {
            char c;
            read(fdAC[0], &c, sizeof(char));
            str.push_back(c);
            count_size++;
        }
        std::cout << "C process " << str << std::endl;
        write(fdBC[1], &count_size, sizeof(int));
        sem_set(semB, 1);
        sem_set(semC, 2);
    }
}