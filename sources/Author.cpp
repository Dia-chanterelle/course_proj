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
    std::string idStr(idSize, '\0');
    file.read(&idStr[0], idSize);
    id = std::move(idStr);

    size_t nameSize;
    file.read(reinterpret_cast<char*>(&nameSize), sizeof(nameSize));
    std::string fullName(nameSize, '\0');
    file.read(&fullName[0], nameSize);

    size_t space1 = fullName.find(' ');
    size_t space2 = fullName.find(' ', space1 + 1);
    if (space1 != std::string::npos && space2 != std::string::npos) {
        fio = FIO(fullName.substr(0, space1),
            fullName.substr(space1 + 1, space2 - space1 - 1),
            fullName.substr(space2 + 1));
    }

    size_t bioSize;
    file.read(reinterpret_cast<char*>(&bioSize), sizeof(bioSize));
    std::string bioStr(bioSize, '\0');
    file.read(&bioStr[0], bioSize);
    biography = std::move(bioStr);

    size_t bookCount;
    file.read(reinterpret_cast<char*>(&bookCount), sizeof(bookCount));
    bookIds.clear();
    for (size_t i = 0; i < bookCount; ++i) {
        size_t bookIdSize;
        file.read(reinterpret_cast<char*>(&bookIdSize), sizeof(bookIdSize));
        std::string b(bookIdSize, '\0');
        file.read(&b[0], bookIdSize);
        bookIds.push_back(std::move(b));
    }
}

