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
    Repository() = default;

    void add(std::shared_ptr<T> item) {
        if (item) {
            items[item->getId()] = item;
        }
    }

    bool remove(const std::string& id) {
        return items.erase(id) > 0;
    }

    std::shared_ptr<T> get(const std::string& id) const {
        auto it = items.find(id);
        return (it != items.end()) ? it->second : nullptr;
    }

    bool contains(const std::string& id) const {
        return items.find(id) != items.end();
    }

    std::vector<std::shared_ptr<T>> getAll() const {
        std::vector<std::shared_ptr<T>> result;
        result.reserve(items.size());
        for (const auto& pair : items) {
            result.push_back(pair.second);
        }
        return result;
    }

    const std::map<std::string, std::shared_ptr<T>>& getAllAsMap() const {
        return items;
    }

    size_t size() const {
        return items.size();
    }

    void clear() {
        items.clear();
    }

    bool empty() const {
        return items.empty();
    }

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

    auto begin() { return items.begin(); }
    auto end() { return items.end(); }
    auto begin() const { return items.begin(); }
    auto end() const { return items.end(); }

    std::shared_ptr<T> operator[](const std::string& id) const {
        return get(id);
    }
};