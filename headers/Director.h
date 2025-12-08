#pragma once
#include "Librarian.h"
#include <map>

class Director : public Librarian {
public:
    /*Director(const FIO& f = FIO(), const std::string& log = "",
        const std::string& pwd = "", int age = 0,
        const std::string& directorId = "");*/
    Director() : Librarian(FIO(), "", "", 0, "", "Administration", true) {}

    Director(const FIO& f, const std::string& log, const std::string& pwd,
        int age, const std::string& directorId, bool isEncrypted = false);

    void showMenu() override;
    std::string getRole() const override { return "Director"; }

    void saveToBinaryFile(std::ofstream& file) const override {
        Librarian::saveToBinaryFile(file);
    }

    void loadFromBinaryFile(std::ifstream& file) override {
        Librarian::loadFromBinaryFile(file);
    }

    static void saveAllToBinaryFile(const std::map<std::string, std::shared_ptr<Director>>& directorsMap,
        const std::string& filename);
    static std::map<std::string, std::shared_ptr<Director>> loadAllFromBinaryFile(const std::string& filename);

protected:
    //std::string generateId() const override;
};