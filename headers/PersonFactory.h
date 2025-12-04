#pragma once
#include "Person.h"
#include "Reader.h"
#include "Librarian.h"
#include "Director.h"
#include <map>
#include <memory>

class PersonFactory {
public:
    static std::shared_ptr<Person> createPersonFromBinaryFile(std::ifstream& file);

    static std::map<std::string, std::shared_ptr<Person>> loadAllFromBinaryFile(const std::string& filename);

    static void saveAllToBinaryFile(const std::map<std::string, std::shared_ptr<Person>>& personsMap,
        const std::string& filename);
};