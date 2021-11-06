#include <iostream>
#include <unistd.h>
#include <inttypes.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <fstream>
#include <sys/mman.h>
#define WRITE 1
#define WAIT 2
#define STOP 0

int32_t primaryTest(int32_t n) 
{
    for (int i = 2; i <= n / 2; i++)
        if (n % i == 0)
            return 0;
 
    return 1;
}

int main()
{
    int32_t *buf;
    buf = (int32_t*)mmap(0, sizeof(int32_t) * 2, PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS, 0, 0); //MAP_ANONYMOUS - область памяти, которую мы зарезервировали, и она никак не связана с файлом (наша память не привязана к файлу)
    if (buf == MAP_FAILED) {
        std:: cout << "An error with mmap function has been detected" << std:: endl;
        return EXIT_FAILURE;
    }
    buf[1] = WAIT;
    pid_t pid; // Идентификатор текущего потока

    std::string fileName; // Имя файла
    int32_t number; // Полученное число
    std::ofstream out; // Поток вывода

    std::cout << "Введите имя файла: ";
    std::getline(std::cin, fileName);

    switch (pid = fork())
    {
    case -1: // Ошибка создания потока
        std::cout << "При создании потока произошла ошибка!";
        return 1;
    
    case 0:  // Код потомка
        while(1) {
            while(buf[1] == WAIT) continue;
            if (buf[0] <= 0 || primaryTest(buf[0])) 
            {
                std::cout << "Дочерний процесс завершился\n";
                buf[1] = STOP;
                exit(0);
            }
            else {
                out.open(fileName, std::ios_base::app); //app - добавить в конец
                out << buf[0] << "\n";
                out.close();
                std::cout << "Число добавлено" << '\n';
                buf[1] = WAIT;
            }
        }

    default: // Код родителя
        while(1) {
            // std::getline(std::cin, line);
            std::cin >> number;
            // number = atoi(line.data());
            buf[0] = number;
            buf[1] = WRITE;
            while(buf[1] == WRITE) continue;
            if (buf[1] == STOP)
            {
                munmap(buf, sizeof(int32_t) * 2);
                std::cout << "Родительский процесс завершился" << std::endl;
                return 0;
            }
        }
    }
}
// -fsanitize=address

