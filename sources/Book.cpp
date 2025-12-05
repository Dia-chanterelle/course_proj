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

void Book::loadFromBinaryFile(std::ifstream& file, const std::map<std::string, std::shared_ptr<Author>>& authorsMap) {
    size_t idSize;
    file.read(reinterpret_cast<char*>(&idSize), sizeof(idSize));
    char* idBuffer = new char[idSize + 1];
    file.read(idBuffer, idSize);
    idBuffer[idSize] = '\0';
    id = idBuffer;
    delete[] idBuffer;

    size_t titleSize;
    file.read(reinterpret_cast<char*>(&titleSize), sizeof(titleSize));
    char* titleBuffer = new char[titleSize + 1];
    file.read(titleBuffer, titleSize);
    titleBuffer[titleSize] = '\0';
    title = titleBuffer;
    delete[] titleBuffer;

    size_t authorIdSize;
    file.read(reinterpret_cast<char*>(&authorIdSize), sizeof(authorIdSize));
    char* authorIdBuffer = new char[authorIdSize + 1];
    file.read(authorIdBuffer, authorIdSize);
    authorIdBuffer[authorIdSize] = '\0';
    std::string authorId = authorIdBuffer;
    delete[] authorIdBuffer;

    author = nullptr;
    if (authorId != "NULL") {
        auto it = authorsMap.find(authorId);
        if (it != authorsMap.end()) {
            author = it->second;
        }
    }

    size_t isbnSize;
    file.read(reinterpret_cast<char*>(&isbnSize), sizeof(isbnSize));
    char* isbnBuffer = new char[isbnSize + 1];
    file.read(isbnBuffer, isbnSize);
    isbnBuffer[isbnSize] = '\0';
    isbn = isbnBuffer;
    delete[] isbnBuffer;

    file.read(reinterpret_cast<char*>(&year), sizeof(year));
    file.read(reinterpret_cast<char*>(&quantity), sizeof(quantity));

    size_t genreSize;
    file.read(reinterpret_cast<char*>(&genreSize), sizeof(genreSize));
    char* genreBuffer = new char[genreSize + 1];
    file.read(genreBuffer, genreSize);
    genreBuffer[genreSize] = '\0';
    genre = genreBuffer;
    delete[] genreBuffer;
}
