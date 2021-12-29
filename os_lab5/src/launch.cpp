#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <dlfcn.h>
#include <math.h>

int main(){
    void* handle = NULL;
    int (*GCF)(int A, int B);
    float (*Derivative)(float A, float deltaX);
    int start_lib;
    const char* lib_arr[] = {"libd1.so", "libd2.so"};
    std:: cout << "Enter start library: " << std:: endl;
    std:: cout << "1 for using first library" << std:: endl;
    std:: cout << "2 for using second library" << std:: endl;
    std:: cin >> start_lib;
    int curlib;
    while(true){
        if(start_lib == 1){
            curlib = 0;
            break;
        }
        else if(start_lib == 2){
            curlib = 1;
            break;
        }
        else{
            std:: cout << "You should enter only 1 or 2!" << std:: endl;
            std:: cin >> start_lib;
        }
    }
    handle = dlopen(lib_arr[curlib], RTLD_LAZY); //rtld lazy выполняется поиск только тех символов, на которые есть ссылки из кода
                                                 //RTLD_LAZY включает режим отложенного связывания, в котором поиск функции в библиотеке выполняется в момент первого обращения к ней
    if (!handle) {
        std:: cout << "An error while opening library has been detected" << std:: endl;
        exit(EXIT_FAILURE);
    }
    GCF = (int(*)(int, int))dlsym(handle, "GCF"); //возвращаем адрес функции из памяти библиотеки (если адрес не найден, вовзращается null)
    if (!GCF){
        std::cout << "The function is missing" << std::endl;
        dlclose(handle);
        exit(EXIT_FAILURE);
    }
    Derivative = (float(*)(float, float))dlsym(handle, "Derivative"); //dlsym присваивает указателю на функцию, объявленному в начале, ее адрес в библиотеке
    if (!Derivative){
        std::cout << "Derivative is missing" << std::endl;
        dlclose(handle);
        exit(EXIT_FAILURE);
    }
    int command;
    std:: cout << "Hello there! Please enter your command according to next rules: " << std:: endl;
    std:: cout << '\t' << "0 for changing the contract;" << std:: endl;
    std:: cout << '\t' << "1 for calculating greatest common divisor; " << std:: endl;
    std:: cout << '\t' << "2 for calculating the Derivative; " << std:: endl;
    std:: cout << '\t' << "3 for exit; " << std:: endl;
    while (printf("Please enter your command: ") && (scanf("%d", &command)) != EOF) { 
        if (command == 0) {
            dlclose(handle); //освобождает указатель на библиотеку и программа перестает ей пользоваться
            if (curlib == 0) {
                curlib = 1 - curlib;
                handle = dlopen(lib_arr[curlib], RTLD_LAZY);
                if (!handle) {
                    std:: cout << "An error while opening library has been detected" << std:: endl;
                    exit(EXIT_FAILURE);
                }
                GCF = (int(*)(int, int))dlsym(handle, "GCF");
                if (!GCF){
                    std::cout << "GCF is missing" << std::endl;
                    dlclose(handle);
                    exit(EXIT_FAILURE);
                }
                Derivative = (float(*)(float, float))dlsym(handle, "Derivative");
                if (!Derivative){
                    std::cout << "Derivative is missing" << std::endl;
                    dlclose(handle);
                    exit(EXIT_FAILURE);
                }
            }
            else if (curlib == 1) {
                curlib = 1 - curlib;
                handle = dlopen(lib_arr[curlib], RTLD_LAZY);
                if (!handle) {
                    std:: cout << "An error while opening library has been detected" << std:: endl;
                    exit(EXIT_FAILURE);
                }
                GCF = (int(*)(int, int))dlsym(handle, "GCF");
                if (!GCF){
                    std::cout << "GCF is missing" << std::endl;
                    dlclose(handle);
                    exit(EXIT_FAILURE);
                }
                Derivative = (float(*)(float, float))dlsym(handle, "Derivative");
                if (!Derivative){
                    std::cout << "Derivative is missing" << std::endl;
                    dlclose(handle);
                    exit(EXIT_FAILURE);
                }
            }
        std:: cout << "You have changed contracts!" << std:: endl;
        }
        else if (command == 1) {
            int a,b;
            std:: cout << "Please enter numbers x, y ";
            std:: cin >> a >> b;
            if (a <= 0 || b <= 0) {
                std:: cout << "Please enter a positive numbers!" << std:: endl;
            }
            else {
                std:: cout << "Result " << GCF(a, b) << std:: endl;
            }
        }
        else if (command == 2) {
            float A, B;
            std:: cout << "Please enter  A and delta X increment: ";
            std:: cin >> A >> B;
            std::cout << Derivative(A, B) << std:: endl;
        }
        else if (command == 3){
            dlclose(handle);
            return 0;
        }
        else {
            std:: cout << "You had to enter only 0, 1, 2 or 3!" << std:: endl;
        }
    }       
    return 0;
}