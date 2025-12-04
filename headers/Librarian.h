#pragma once
#include "Person.h"
#include <vector>
#include <map>

class Librarian : public Person {
private:
    std::string department;

public:
    Librarian(const FIO& f = FIO(), const std::string& log = "",
        const std::string& pwd = "", int age = 0,
        const std::string& librarianId = "", const std::string& dept = "Main");

    void showMenu() override;
    std::string getRole() const override { return "Librarian"; }

    void saveToBinaryFile(std::ofstream& file) const override;
    void loadFromBinaryFile(std::ifstream& file) override;

    static void saveAllToBinaryFile(const std::map<std::string, std::shared_ptr<Librarian>>& librariansMap,
        const std::string& filename);
    static std::map<std::string, std::shared_ptr<Librarian>> loadAllFromBinaryFile(const std::string& filename);

protected:
    // ÈÑÏĞÀÂËÅÍÎ: òîëüêî îáúÿâëåíèå
    std::string generateId() const override;
};