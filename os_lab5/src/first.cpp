#include <math.h>
#include <iostream>
extern "C" int GCF (int A, int B); // Глобальная функция, которую можно использовать в других файлах программы
extern "C" float Derivative(float A, float deltaX);

int GCF(int A, int B){
    if(A % B == 0)
    { 
        return B;
    }
    else if (B % A == 0){
        return A;
    }
    else{
        if(A > B){
            return GCF(A%B, B);
        }
        else{
            return GCF(A, B%A);
        }
    }
}

float Derivative(float A, float deltaX){
    std::cout << "You are on the first. Your result - ";
    return (cos(A + deltaX) - cos(A))/deltaX;
}
