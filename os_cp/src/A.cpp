#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <semaphore.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <stdarg.h>

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

int main()
{
    int32_t fdAC[2]; //pipe между А и С
    int32_t fdAB[2]; //pipe между А и В
    int32_t fdBC[2]; //pipe между В и С
    pipe(fdAC);
    pipe(fdAB);
    pipe(fdBC);
    if(sem_unlink("SemA") == -1 || sem_unlink("SemB") == -1 || sem_unlink("SemC") == -1)
    {
        std::cout << "Unlink failed" << std::endl;
    }
    sem_t *semA = sem_open("SemA", O_CREAT, S_IRWXU, 1); //получаем адрес семафора,чтение, выполнение и запись
    sem_t *semB = sem_open("SemB", O_CREAT, S_IRWXU, 2);
    sem_t *semC = sem_open("SemC", O_CREAT, S_IRWXU, 2);
    if ((semA == SEM_FAILED)||(semB == SEM_FAILED)||(semC == SEM_FAILED))
    {
        std::cout << "SEM_FAILED" << std::endl;
        return 0;
    }
    std::cout << "Enter some strings:\n";
    pid_t A;
    switch(A = fork())
    {
        case -1:
            std::cout << "Error fork" << std::endl;
            return -1;
            break;
        case 0:
            pid_t C;
            switch(C = fork())
            {
                case -1:
                    std::cout << "Error fork" << std::endl;
                    return -1;
                    break;
                case 0:
                    execl("B", std::to_string(fdAB[0]).c_str(), std::to_string(fdBC[0]).c_str(), NULL);
                    break;
                default:
                    execl("C", std::to_string(fdAC[0]).c_str(), std::to_string(fdBC[1]).c_str(), NULL);
                    break;
            }
            break;
        default:
            while(1)
            {
                while (sem_get(semA) == 2)
                {
                    continue;
                }
                std::cout << "A process" << std::endl;
                std::string str;
                std::cin >> str;
                if (str == "exit")
                {
                    sem_set(semA, 0);
                    sem_set(semB, 0);
                    sem_set(semC, 0);
                    sem_close(semA);
                    sem_close(semB);
                    sem_close(semC);
                    sem_destroy(semA);
                    sem_destroy(semB);
                    sem_destroy(semC);
                    close(fdAC[0]);
                    close(fdAC[1]);
                    close(fdAB[0]);
                    close(fdAB[1]);
                    close(fdBC[0]);
                    close(fdBC[1]);
                    return 0;
                }
                int size = str.length();
                write(fdAC[1], &size, sizeof(int));
                write(fdAB[1], &size, sizeof(int));
                for (int i = 0; i < size; ++i)
                {
                    write(fdAC[1], &str[i], sizeof(char));
                }
                sem_set(semB, 1);
                sem_set(semA, 2);
            }
            break;
    }
}