#include <iostream>
extern "C" int GCF (int A, int B); // Глобальная функция, которую можно использовать в других файлах программы
extern "C" float Derivative(float A, float deltaX);

int main(){
    int com;
    while(printf("Please, enter your command:") && scanf("%d", &com) != EOF)
    {
        switch(com){
            case 1:
                int first;
                int second;
                std::cin >> first >> second;
                if(first <= 0 || second <= 0){
                    std::cout << "Please, enter only positive numbers!" << std::endl;
                }
                else{
                    std::cout << "Result: " << GCF(first, second) << std::endl;
                }
                break;
            case 2:
                float A;
                float deltaX;
                std::cin >> A >> deltaX;
                std::cout << "Result: " << Derivative(A, deltaX) << std::endl;
                break;
            case 3:
                exit(0);
            default:
                std::cout << "You must enter only 1 or 2!" << std::endl;
                break;
        }
    }
    return 0;
}