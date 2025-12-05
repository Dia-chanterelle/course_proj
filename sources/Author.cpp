#include "../headers/Author.h"
#include <fstream>
#include <algorithm>
#include <cstring>

int Author::author_count = 0;

Author::Author(const FIO& f, const std::string& bio, const std::string& authorId)
    : fio(f), biography(bio) {
    if (authorId.empty()) {
        id = generateId();
        author_count++;
    }
    else {
        id = authorId;
        try {
            std::string numStr = authorId.substr(1);
            int num = std::stoi(numStr);
            if (num >= author_count) {
                author_count = num + 1;
            }
        }
        catch (const std::exception& e) {
            std::cerr << "Îøèáêà: " << e.what() << std::endl;
        }
    }
}

std::string Author::generateId() const {
    return "A" + std::to_string(author_count + 1);
}

std::ostream& operator<<(std::ostream& os, const Author& author) {
    os << "[" << author.id << "] " << author.fio << " - " << author.biography;
    os << " (Êíèã: " << author.bookIds.size() << ")";
    return os;
}

bool Author::operator==(const Author& other) const {
    return id == other.id;
}

void Author::addBook(const std::string& bookId) {
    if (!hasBook(bookId)) {
        bookIds.push_back(bookId);
    }
}

void Author::removeBook(const std::string& bookId) {
    bookIds.erase(std::remove(bookIds.begin(), bookIds.end(), bookId), bookIds.end());
}

bool Author::hasBook(const std::string& bookId) const {
    return std::find(bookIds.begin(), bookIds.end(), bookId) != bookIds.end();
}

void Author::saveToBinaryFile(std::ofstream& file) const {
    size_t idSize = id.size();
    file.write(reinterpret_cast<const char*>(&idSize), sizeof(idSize));
    file.write(id.c_str(), idSize);

    std::string fullName = fio.getFullName();
    size_t nameSize = fullName.size();
    file.write(reinterpret_cast<const char*>(&nameSize), sizeof(nameSize));
    file.write(fullName.c_str(), nameSize);

    size_t bioSize = biography.size();
    file.write(reinterpret_cast<const char*>(&bioSize), sizeof(bioSize));
    file.write(biography.c_str(), bioSize);

    size_t bookCount = bookIds.size();
    file.write(reinterpret_cast<const char*>(&bookCount), sizeof(bookCount));

    for (const auto& bookId : bookIds) {
        size_t bookIdSize = bookId.size();
        file.write(reinterpret_cast<const char*>(&bookIdSize), sizeof(bookIdSize));
        file.write(bookId.c_str(), bookIdSize);
    }
}

void Author::loadFromBinaryFile(std::ifstream& file) {
    size_t idSize;
    file.read(reinterpret_cast<char*>(&idSize), sizeof(idSize));
    char* idBuffer = new char[idSize + 1];
    file.read(idBuffer, idSize);
    idBuffer[idSize] = '\0';
    id = idBuffer;
    delete[] idBuffer;

    size_t nameSize;
    file.read(reinterpret_cast<char*>(&nameSize), sizeof(nameSize));
    char* nameBuffer = new char[nameSize + 1];
    file.read(nameBuffer, nameSize);
    nameBuffer[nameSize] = '\0';
    std::string fullName = nameBuffer;
    delete[] nameBuffer;

    size_t space1 = fullName.find(' ');
    size_t space2 = fullName.find(' ', space1 + 1);
    if (space1 != std::string::npos && space2 != std::string::npos) {
        std::string lastName = fullName.substr(0, space1);
        std::string firstName = fullName.substr(space1 + 1, space2 - space1 - 1);
        std::string patronymic = fullName.substr(space2 + 1);
        fio = FIO(lastName, firstName, patronymic);
    }

    size_t bioSize;
    file.read(reinterpret_cast<char*>(&bioSize), sizeof(bioSize));
    char* bioBuffer = new char[bioSize + 1];
    file.read(bioBuffer, bioSize);
    bioBuffer[bioSize] = '\0';
    biography = bioBuffer;
    delete[] bioBuffer;

    size_t bookCount;
    file.read(reinterpret_cast<char*>(&bookCount), sizeof(bookCount));

    bookIds.clear();
    for (size_t i = 0; i < bookCount; ++i) {
        size_t bookIdSize;
        file.read(reinterpret_cast<char*>(&bookIdSize), sizeof(bookIdSize));
        char* bookIdBuffer = new char[bookIdSize + 1];
        file.read(bookIdBuffer, bookIdSize);
        bookIdBuffer[bookIdSize] = '\0';
        bookIds.push_back(bookIdBuffer);
        delete[] bookIdBuffer;
    }
}
