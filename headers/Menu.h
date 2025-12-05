#pragma once
#include <memory>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <unordered_map> 
#include <stack>
#include <vector>
#include "Repository.h" 
#include "Person.h"
#include "Reader.h"
#include "Librarian.h"
#include "Director.h"
#include "Book.h"
#include "Author.h"
#include "BorrowedBook.h"
#include "PersonFactory.h"

class LibrarySystem {
private:
    Repository<Book> booksRepo;
    Repository<Author> authorsRepo;

    std::map<std::string, std::shared_ptr<Person>> usersMap;

    std::vector<BorrowedBook> borrowedBooks;
    std::shared_ptr<Person> currentUser;
    std::stack<std::string> actionHistory;

    bool shouldExit = false;

public:
    LibrarySystem();
    ~LibrarySystem();

    void run();
    bool login();
    void logout() { currentUser.reset(); }
    bool hasCurrentUser() const { return currentUser != nullptr; }
    void setExitFlag(bool flag) { shouldExit = flag; }
    bool shouldExitProgram() const { return shouldExit; }

    void registerReader();
    void registerLibrarian();
    void registerDirector();
    void registerUser();

    void showAuthMenu();
    void showUserMenu();
    void handleUserChoice(int choice);
    void manageBooks();

    void addBook();
    void editBook();
    void deleteBook();
    void showAllBooks() const;
    void searchBooks();
    void sortBooks();

    void loadAllData();
    void saveAllData();
    void createTestUsers();
    void createTestBooks();

    void generateReport();
    void ageBasedRecommendations();
    void showStatistics() const;

    void addToActionHistory(const std::string& action);
    void showActionHistory();

    std::shared_ptr<Book> findBookById(const std::string& id) const;
    std::shared_ptr<Person> findUserById(const std::string& id) const;
    std::shared_ptr<Author> findAuthorById(const std::string& id) const;

    std::shared_ptr<Person> getCurrentUser() const { return currentUser; }

    template<typename T>
    void addToRepository(Repository<T>& repo, std::shared_ptr<T> item) {
        repo.add(item);
    }

    template<typename T>
    bool removeFromRepository(Repository<T>& repo, const std::string& id) {
        return repo.remove(id);
    }
};