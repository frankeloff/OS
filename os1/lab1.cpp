#include <iostream>
#include <unistd.h>
#include <inttypes.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <fstream>

const int32_t EXITOK = 0;
const int32_t EXITNOK = 1;

int32_t primaryTest(int32_t n) 
{
    for (int i = 2; i <= n / 2; i++)
        if (n % i == 0)
            return 0;
 
    return 1;
}

int main()
{
    // char a[2];
    // a[3] = 'f';
    int32_t p_c[2]; // Pipe между родителем и потомком
    int32_t c_p[2]; // Pipe между потомком и родителем
    pipe(p_c);
    pipe(c_p);
    
    pid_t pid; // Идентификатор текущего потока
    int32_t exit_code; // Код возврата для дочернего потока

    std::string line; // Считываемая строка
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
        close(p_c[1]);
        // close(c_p[0]);
        while(1) {
            read(p_c[0], &number, sizeof(int32_t));

            if (number <= 0 || primaryTest(number)) 
            {
                write(c_p[1], &EXITNOK, sizeof(int32_t));
                std::cout << "The child process is completed\n";
                close(p_c[0]);
                close(c_p[1]);
                exit(0);
            }
            else {
                out.open(fileName, std::ios_base::app);
                out << number << "\n";
                out.close();
                std::cout << "Add number" << '\n';
                write(c_p[1], &EXITOK, sizeof(int32_t));
            }
        }

    default: // Код родителя
        close(p_c[0]);
        close(c_p[1]);
        while(1) {
            // std::getline(std::cin, line);
            std::cin >> number;
            // number = atoi(line.data());

            write(p_c[1], &number, sizeof(int32_t));
            read(c_p[0], &exit_code, sizeof(int32_t));


            if (exit_code)
            {
                close(p_c[1]);
                close(c_p[0]);
                return 0;
            }
        }
    }
}
// -fsanitize=address

