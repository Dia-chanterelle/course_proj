#include "../headers/Book.h"
#include <fstream>
#include <algorithm>
#include <cstring>

Book::Book(const std::string& t, std::shared_ptr<Author> a, const std::string& i,
    int y, int q, const std::string& g, const std::string& bookId)
    : title(t), author(a), isbn(i), year(y), quantity(q), genre(g) {

    if (bookId.empty()) {
        static int bookCounter = 0;
        id = "B" + std::to_string(++bookCounter);
    }
    else {
        id = bookId;
    }

    if (author) {
        author->addBook(id);
    }
}

std::ostream& operator<<(std::ostream& os, const Book& book) {
    os << "[" << book.id << "] " << book.title << " (" << book.year << ") - ";
    if (book.author) {
        os << book.author->getFIO().getFullName();
    }
    else {
        os << "Неизвестный автор";
    }
    os << " - " << book.genre << " (доступно: " << book.quantity << ")";
    return os;
}

bool Book::operator==(const Book& other) const {
    return id == other.id;
}

void Book::saveToBinaryFile(std::ofstream& file) const {
    size_t idSize = id.size();
    file.write(reinterpret_cast<const char*>(&idSize), sizeof(idSize));
    file.write(id.c_str(), idSize);

    size_t titleSize = title.size();
    file.write(reinterpret_cast<const char*>(&titleSize), sizeof(titleSize));
    file.write(title.c_str(), titleSize);

    std::string authorId = author ? author->getId() : "NULL";
    size_t authorIdSize = authorId.size();
    file.write(reinterpret_cast<const char*>(&authorIdSize), sizeof(authorIdSize));
    file.write(authorId.c_str(), authorIdSize);

    size_t isbnSize = isbn.size();
    file.write(reinterpret_cast<const char*>(&isbnSize), sizeof(isbnSize));
    file.write(isbn.c_str(), isbnSize);

    file.write(reinterpret_cast<const char*>(&year), sizeof(year));
    file.write(reinterpret_cast<const char*>(&quantity), sizeof(quantity));

    size_t genreSize = genre.size();
    file.write(reinterpret_cast<const char*>(&genreSize), sizeof(genreSize));
    file.write(genre.c_str(), genreSize);
}

void Book::loadFromBinaryFile(std::ifstream& file,
    const std::map<std::string, std::shared_ptr<Author>>& authorsMap) {

    size_t idSize;
    file.read(reinterpret_cast<char*>(&idSize), sizeof(idSize));
    std::string idStr(idSize, '\0');
    file.read(&idStr[0], idSize);
    id = std::move(idStr);

    size_t titleSize;
    file.read(reinterpret_cast<char*>(&titleSize), sizeof(titleSize));
    std::string titleStr(titleSize, '\0');
    file.read(&titleStr[0], titleSize);
    title = std::move(titleStr);

    size_t authorIdSize;
    file.read(reinterpret_cast<char*>(&authorIdSize), sizeof(authorIdSize));
    std::string authorId(authorIdSize, '\0');
    file.read(&authorId[0], authorIdSize);

    author = nullptr;
    if (authorId != "NULL") {
        auto it = authorsMap.find(authorId);
        if (it != authorsMap.end()) {
            author = it->second;
        }
    }

    size_t isbnSize;
    file.read(reinterpret_cast<char*>(&isbnSize), sizeof(isbnSize));
    std::string isbnStr(isbnSize, '\0');
    file.read(&isbnStr[0], isbnSize);
    isbn = std::move(isbnStr);

    file.read(reinterpret_cast<char*>(&year), sizeof(year));
    file.read(reinterpret_cast<char*>(&quantity), sizeof(quantity));

    size_t genreSize;
    file.read(reinterpret_cast<char*>(&genreSize), sizeof(genreSize));
    std::string genreStr(genreSize, '\0');
    file.read(&genreStr[0], genreSize);
    genre = std::move(genreStr);
}
