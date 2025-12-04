#include <iostream>
#include "headers/Menu.h"
#include <windows.h>

int main() {

    setlocale(LC_ALL, "Russian");
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);

    try {
        std::cout << "=== Библиотечная система ===\n";
        std::cout << "Инициализация системы...\n";

        auto system = std::make_unique<LibrarySystem>();
        system->run();

        std::cout << "Работа системы завершена.\n";
        return 0;
    }
    catch (const std::exception& e) {
        std::cerr << "Критическая ошибка: " << e.what() << std::endl;
        return 1;
    }
    catch (...) {
        std::cerr << "Неизвестная критическая ошибка!" << std::endl;
        return 1;
    }
}