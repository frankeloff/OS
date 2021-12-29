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
    int32_t fdAB[2];
    int32_t fdBC[2];
    fdAB[0] = atoi(argv[0]);
    fdBC[0] = atoi(argv[1]);
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
        while(sem_get(semB) == 2)
        {
            continue;
        }
        if (sem_get(semB) == 0)
        {
            sem_close(semA);
            sem_close(semB);
            sem_close(semC);
            sem_destroy(semA);
            sem_destroy(semB);
            sem_destroy(semC);
            close(fdAB[0]);
            close(fdBC[0]);
            exit(0);
        }
        int size;
        read(fdAB[0], &size, sizeof(int));
        std::cout << "B process: Number of input symbols is " << size << std::endl;
        sem_set(semC, 1);
        sem_set(semB, 2);
        while (sem_get(semB) == 2)
        {
            continue;
        }
        if (sem_get(semB) == 0)
        {
            sem_close(semA);
            sem_close(semB);
            sem_close(semC);
            sem_destroy(semA);
            sem_destroy(semB);
            sem_destroy(semC);
            close(fdAB[0]);
            close(fdBC[0]);
            exit(0);
        }
        read(fdBC[0], &size, sizeof(int));
        std::cout << "B process: Number of output symbols is " << size << std::endl;
        sem_set(semA, 1);
        sem_set(semB, 2);
    }
}