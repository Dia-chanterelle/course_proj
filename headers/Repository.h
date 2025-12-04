#pragma once
#include <map>
#include <memory>
#include <vector>
#include <string>
#include <fstream>
#include <functional>
#include <algorithm>

template<typename T>
class Repository {
private:
    std::map<std::string, std::shared_ptr<T>> items;

public:
    // Конструктор
    Repository() = default;

    // Добавление элемента
    void add(std::shared_ptr<T> item) {
        if (item) {
            items[item->getId()] = item;
        }
    }

    // Удаление элемента по ID
    bool remove(const std::string& id) {
        return items.erase(id) > 0;
    }

    // Получение элемента по ID
    std::shared_ptr<T> get(const std::string& id) const {
        auto it = items.find(id);
        return (it != items.end()) ? it->second : nullptr;
    }

    // Проверка существования элемента
    bool contains(const std::string& id) const {
        return items.find(id) != items.end();
    }

    // Получение всех элементов
    std::vector<std::shared_ptr<T>> getAll() const {
        std::vector<std::shared_ptr<T>> result;
        result.reserve(items.size());
        for (const auto& pair : items) {
            result.push_back(pair.second);
        }
        return result;
    }

    // Получение всех элементов в виде map
    const std::map<std::string, std::shared_ptr<T>>& getAllAsMap() const {
        return items;
    }

    // Количество элементов
    size_t size() const {
        return items.size();
    }

    // Очистка репозитория
    void clear() {
        items.clear();
    }

    // Проверка на пустоту
    bool empty() const {
        return items.empty();
    }

    // Шаблонный метод для поиска по предикату
    template<typename Predicate>
    std::vector<std::shared_ptr<T>> findIf(Predicate pred) const {
        std::vector<std::shared_ptr<T>> result;
        for (const auto& pair : items) {
            if (pred(pair.second)) {
                result.push_back(pair.second);
            }
        }
        return result;
    }

    // Сохранение в бинарный файл (требуется, чтобы T имел метод saveToBinaryFile)
    bool saveToBinaryFile(const std::string& filename) const {
        std::ofstream file(filename, std::ios::binary);
        if (!file.is_open()) {
            return false;
        }

        try {
            size_t count = items.size();
            file.write(reinterpret_cast<const char*>(&count), sizeof(count));

            for (const auto& pair : items) {
                pair.second->saveToBinaryFile(file);
            }

            file.close();
            return true;
        }
        catch (const std::exception&) {
            return false;
        }
    }

    // Загрузка из бинарного файла (требуется, чтобы T имел конструктор по умолчанию и метод loadFromBinaryFile)
    template<typename... Args>
    bool loadFromBinaryFile(const std::string& filename, Args&&... args) {
        std::ifstream file(filename, std::ios::binary);
        if (!file.is_open()) {
            return false;
        }

        try {
            items.clear();
            size_t count;
            file.read(reinterpret_cast<char*>(&count), sizeof(count));

            for (size_t i = 0; i < count; ++i) {
                auto item = std::make_shared<T>();
                item->loadFromBinaryFile(file, std::forward<Args>(args)...);
                items[item->getId()] = item;
            }

            file.close();
            return true;
        }
        catch (const std::exception&) {
            items.clear();
            return false;
        }
    }

    // Итераторы для использования в range-based for
    auto begin() { return items.begin(); }
    auto end() { return items.end(); }
    auto begin() const { return items.begin(); }
    auto end() const { return items.end(); }

    // Оператор [] для доступа по ключу
    std::shared_ptr<T> operator[](const std::string& id) const {
        return get(id);
    }
};