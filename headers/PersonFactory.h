#pragma once
#include <memory>
#include <map>
#include <string>
#include "Reader.h"
#include "Librarian.h"
#include "Director.h"

class PersonFactory {
private:
    static int readerCount;
    static int librarianCount;
    static int directorCount;

public:
    static std::shared_ptr<Reader> createReader(const FIO& f,
        const std::string& log,
        const std::string& pwd,
        int age);

    static std::shared_ptr<Librarian> createLibrarian(const FIO& f,
        const std::string& log,
        const std::string& pwd,
        int age,
        const std::string& dept);

    static std::shared_ptr<Director> createDirector(const FIO& f,
        const std::string& log,
        const std::string& pwd,
        int age);

    static void saveAllToBinaryFile(const std::map<std::string, std::shared_ptr<Person>>& users,
        const std::string& filename);

    static std::map<std::string, std::shared_ptr<Person>> loadAllFromBinaryFile(const std::string& filename);
};
