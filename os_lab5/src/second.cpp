#include <math.h>
#include <iostream>
extern "C" int GCF (int A, int B); // Глобальная функция, которую можно использовать в других файлах программы
extern "C" float Derivative(float A, float deltaX);
int GCF(int A, int B){
    std::cout << "You are on the second. Your result - ";
    int a;
    if(A == B) return A;
    else if(A > B){
        a = B;
    }
    else{
        a = A;
    }
    while(A%a != 0 || B%a != 0){
        a--;
    }
    return a;
}

float Derivative(float A, float deltaX){
    std::cout << "You are on the second. Your result - ";
    return (cos(A + deltaX) - cos(A-deltaX))/(2*deltaX);
}