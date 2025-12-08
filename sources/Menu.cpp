#include "../headers/Menu.h"
#include <iostream>
#include <fstream>
#include <algorithm>
#include <limits>
#include <vector>
#include <sstream>

LibrarySystem::LibrarySystem() {
    loadAllData();
}

LibrarySystem::~LibrarySystem() {
    saveAllData();
}

void LibrarySystem::loadAllData() {
    std::cout << "Загрузка данных...\n";

    if (!authorsRepo.loadFromBinaryFile("authors.bin")) {
        std::cout << "Файл authors.bin не найден или поврежден. Создание тестовых авторов...\n";
    }

    if (!booksRepo.loadFromBinaryFile("books.bin", authorsRepo.getAllAsMap())) {
        std::cout << "Файл books.bin не найден или поврежден. Создание тестовых книг...\n";
        createTestBooks();
    }

    usersMap = PersonFactory::loadAllFromBinaryFile("users.bin");

    if (usersMap.empty()) {
        std::cout << "Создание тестовых пользователей...\n";
        createTestUsers();
    }

    auto borrowedBooksMap = BorrowedBook::loadAllFromBinaryFile("borrowed.bin");
    borrowedBooks.clear();
    for (const auto& pair : borrowedBooksMap) {
        borrowedBooks.push_back(pair.second);
    }

    std::cout << "Данные загружены:\n";
    std::cout << " - Книг: " << booksRepo.size() << "\n";
    std::cout << " - Авторов: " << authorsRepo.size() << "\n";
    std::cout << " - Пользователей: " << usersMap.size() << "\n";
    std::cout << " - Взятых книг: " << borrowedBooks.size() << "\n";
}

void LibrarySystem::saveAllData() {
    std::cout << "Сохранение данных...\n";

    if (!authorsRepo.saveToBinaryFile("authors.bin")) {
        std::cout << "Ошибка сохранения авторов!\n";
    }

    if (!booksRepo.saveToBinaryFile("books.bin")) {
        std::cout << "Ошибка сохранения книг!\n";
    }

    PersonFactory::saveAllToBinaryFile(usersMap, "users.bin");

    std::map<std::string, BorrowedBook> borrowedBooksMap;
    int counter = 0;
    for (const auto& book : borrowedBooks) {
        std::string key = "borrowed_" + std::to_string(counter++);
        borrowedBooksMap[key] = book;
    }
    BorrowedBook::saveAllToBinaryFile(borrowedBooksMap, "borrowed.bin");

    std::cout << "Данные сохранены в бинарные файлы.\n";
}

void LibrarySystem::createTestUsers() {
    auto r1 = PersonFactory::createReader(FIO("Иванов", "Иван", "Иванович"), "ivanov", "12345", 25);
    auto r2 = PersonFactory::createReader(FIO("Петров", "Пётр", "Петрович"), "petrov", "54321", 30);

    auto l1 = PersonFactory::createLibrarian(FIO("Сидоров", "Сидор", "Сидорович"), "sidorov", "libpass", 40, "Science");
    auto d1 = PersonFactory::createDirector(FIO("Кузнецов", "Кузьма", "Кузьмич"), "kuznetsov", "dirpass", 50);

    usersMap.emplace(r1->getId(), r1);
    usersMap.emplace(r2->getId(), r2);
    usersMap.emplace(l1->getId(), l1);
    usersMap.emplace(d1->getId(), d1);


    std::cout << "Создано тестовых пользователей: " << usersMap.size() << "\n";
    
    std::cout << "Тестовые пользователи:\n";
    std::cout << "Reader1 -> Логин: " << r1->getLogin() << ", Пароль: 12345\n";
    std::cout << "Reader2 -> Логин: " << r2->getLogin() << ", Пароль: 54321\n";
    std::cout << "Librarian -> Логин: " << l1->getLogin() << ", Пароль: libpass\n";
    std::cout << "Director -> Логин: " << d1->getLogin() << ", Пароль: dirpass\n";
}


void LibrarySystem::createTestBooks() {
    if (authorsRepo.empty()) {
        auto author1 = std::make_shared<Author>(FIO("Толстой", "Лев", "Николаевич"), "Русский писатель");
        auto author2 = std::make_shared<Author>(FIO("Достоевский", "Федор", "Михайлович"), "Русский писатель");

        authorsRepo.add(author1);
        authorsRepo.add(author2);
    }

    auto allAuthors = authorsRepo.getAll();
    if (!allAuthors.empty()) {
        auto author1 = allAuthors[0];
        auto author2 = allAuthors[1];
        auto book1 = std::make_shared<Book>("Война и мир", author1, "978-5-389-00001-1", 1869, 5, "Роман");
        auto book2 = std::make_shared<Book>("Преступление и наказание", author2, "978-5-389-00002-2", 1866, 3, "Роман");

        booksRepo.add(book1);
        booksRepo.add(book2);
    }

    std::cout << "Созданы тестовые книги\n";
}

void LibrarySystem::showAuthMenu() {
    std::cout << "\n=== Библиотечная система ===\n";
    std::cout << "1. Вход в систему\n";
    std::cout << "2. Регистрация (только как читатель)\n";
    std::cout << "3. Выход из программы\n";
    std::cout << "Выберите действие: ";

    int choice;
    std::cin >> choice;

    switch (choice) {
    case 1:
        login();
        break;
    case 2:
        registerReader();
        break;
    case 3:
        shouldExit = true;
        break;
    default:
        std::cout << "Неверный выбор!\n";
    }
}

void LibrarySystem::showUserMenu() {
    if (!currentUser) return;

    currentUser->showMenu();

    int choice;
    std::cout << "Выберите действие: ";
    std::cin >> choice;

    handleUserChoice(choice);
}

bool LibrarySystem::login() {
    std::string login, password;
    std::cout << "Введите логин: ";
    std::cin >> login;
    std::cout << "Введите пароль: ";
    std::cin >> password;

    for (const auto& pair : usersMap) {
        if (pair.second->getLogin() == login && pair.second->authenticate(password)) {
            currentUser = pair.second;
            std::cout << " Успешный вход! Добро пожаловать, "
                << pair.second->getFIO().getFullName() << "!\n";
            std::cout << "Роль: " << pair.second->getRole() << "\n";
            addToActionHistory("Вход пользователя: " + login);
            return true;
        }
    }
    std::cout << " Ошибка: неверный логин или пароль!\n";
    return false;
}

void LibrarySystem::registerReader() {
    FIO fio;
    std::string login, password;
    int age;

    std::cout << "=== Регистрация читателя ===\n";
    std::cout << "Введите фамилию: ";
    std::cin >> fio;
    std::cout << "Введите логин: ";
    std::cin >> login;
    std::cout << "Введите пароль: ";
    std::cin >> password;
    std::cout << "Введите возраст: ";
    std::cin >> age;

    for (const auto& pair : usersMap) {
        if (pair.second->getLogin() == login) {
            std::cout << " Ошибка: пользователь с таким логином уже существует!\n";
            return;
        }
    }

    auto newReader = PersonFactory::createReader(fio, login, password, age);
    usersMap.emplace(newReader->getId(), newReader);

    PersonFactory::saveAllToBinaryFile(usersMap, "users.bin");

    std::cout << " Читатель успешно зарегистрирован!\n";
    std::cout << "ID нового читателя: " << newReader->getId() << "\n";
    addToActionHistory("Зарегистрирован читатель: " + login);
}

void LibrarySystem::registerLibrarian() {
    FIO fio;
    std::string login, password, department;
    int age;

    std::cout << "=== Регистрация библиотекаря ===\n";
    std::cout << "Введите фамилию: ";
    std::cin >> fio;
    std::cout << "Введите логин: ";
    std::cin >> login;
    std::cout << "Введите пароль: ";
    std::cin >> password;
    std::cout << "Введите возраст: ";
    std::cin >> age;
    std::cout << "Введите отдел: ";
    std::cin >> department;

    for (const auto& pair : usersMap) {
        if (pair.second->getLogin() == login) {
            std::cout << " Ошибка: пользователь с таким логином уже существует!\n";
            return;
        }
    }

    auto newLibrarian = PersonFactory::createLibrarian(fio, login, password, age, department);
    usersMap.emplace(newLibrarian->getId(), newLibrarian);

    PersonFactory::saveAllToBinaryFile(usersMap, "users.bin");

    std::cout << "Библиотекарь успешно зарегистрирован!\n";
    std::cout << "ID нового библиотекаря: " << newLibrarian->getId() << "\n";
    addToActionHistory("Зарегистрирован библиотекарь: " + login);
}

void LibrarySystem::registerDirector() {
    FIO fio;
    std::string login, password;
    int age;

    std::cout << "=== Регистрация директора ===\n";
    std::cout << "Введите фамилию: ";
    std::cin >> fio;
    std::cout << "Введите логин: ";
    std::cin >> login;
    std::cout << "Введите пароль: ";
    std::cin >> password;
    std::cout << "Введите возраст: ";
    std::cin >> age;

    for (const auto& pair : usersMap) {
        if (pair.second->getLogin() == login) {
            std::cout << " Ошибка: пользователь с таким логином уже существует!\n";
            return;
        }
    }

    auto newDirector = PersonFactory::createDirector(fio, login, password, age);
    usersMap.emplace(newDirector->getId(), newDirector);

    PersonFactory::saveAllToBinaryFile(usersMap, "users.bin");

    std::cout << " Директор успешно зарегистрирован!\n";
    std::cout << "ID нового директора: " << newDirector->getId() << "\n";
    addToActionHistory("Зарегистрирован директор: " + login);
}

void LibrarySystem::registerUser() {
    if (!currentUser) {
        registerReader();
    }
    else {
        std::cout << "=== Регистрация нового пользователя ===\n";
        std::cout << "Выберите роль для регистрации:\n";
        std::cout << "1. Читатель\n";
        std::cout << "2. Библиотекарь\n";
        std::cout << "3. Директор\n";
        std::cout << "0. Назад\n";
        std::cout << "Ваш выбор: ";
        int choice;
        std::cin >> choice;

        switch (choice) {
        case 1:
            registerReader();
            break;
        case 2:
            registerLibrarian();
            break;
        case 3:
            registerDirector();
            break;
        case 0:
            return;
        default:
            std::cout << "Неверный выбор!\n";
        }
    }
}

void LibrarySystem::addToActionHistory(const std::string& action) {
    actionHistory.push(action); 
}


void LibrarySystem::showActionHistory() {
    std::cout << "=== История действий (LIFO) ===\n";

    std::stack<std::string> temp = actionHistory;

    while (!temp.empty()) {
        std::cout << temp.top() << "\n"; 
        temp.pop();                      
    }
}


void LibrarySystem::manageBooks() {
    int choice;
    do {
        std::cout << "\n=== Управление книгами ===\n";
        std::cout << "1. Добавить книгу\n";
        std::cout << "2. Редактировать книгу\n";
        std::cout << "3. Удалить книгу\n";
        std::cout << "4. Показать все книги\n";
        std::cout << "5. Поиск книг\n";
        std::cout << "6. Сортировка книг\n";
        std::cout << "0. Назад\n";
        std::cout << "Выберите действие: ";
        std::cin >> choice;

        switch (choice) {
        case 1:
            addBook();
            break;
        case 2:
            editBook();
            break;
        case 3:
            deleteBook();
            break;
        case 4:
            showAllBooks();
            break;
        case 5:
            searchBooks();
            break;
        case 6:
            sortBooks();
            break;
        case 0:
            return;
        default:
            std::cout << "Неверный выбор!\n";
        }
    } while (choice != 0);
}

void LibrarySystem::addBook() {
    std::string title, isbn, genre;
    int year, quantity;

    std::cout << "=== Добавление новой книги ===\n";
    std::cout << "Введите название книги: ";
    std::cin.ignore();
    std::getline(std::cin, title);

    std::cout << "Введите ISBN: ";
    std::getline(std::cin, isbn);

    std::cout << "Введите год издания: ";
    std::cin >> year;

    std::cout << "Введите количество: ";
    std::cin >> quantity;

    std::cout << "Введите жанр: ";
    std::cin.ignore();
    std::getline(std::cin, genre);

    auto newBook = std::make_shared<Book>(title, nullptr, isbn, year, quantity, genre);

    booksRepo.add(newBook);

    booksRepo.saveToBinaryFile("books.bin");

    addToActionHistory("Добавлена книга: " + title);
    std::cout << "Книга успешно добавлена! ID: " << newBook->getId() << "\n";
}

void LibrarySystem::editBook() {
    std::string bookId;
    std::cout << "Введите ID книги для редактирования: ";
    std::cin >> bookId;

    auto book = booksRepo.get(bookId);
    if (!book) {
        std::cout << "Книга не найдена.\n";
        return;
    }

    std::cout << "Редактирование книги [" << book->getId() << "] "
        << book->getTitle() << "\n";
    std::cout << "1. Изменить название\n";
    std::cout << "2. Изменить автора\n";
    std::cout << "3. Изменить ISBN\n";
    std::cout << "4. Изменить год\n";
    std::cout << "5. Изменить количество\n";
    std::cout << "6. Изменить жанр\n";
    int choice; std::cin >> choice;

    switch (choice) {
    case 1: {
        std::string newTitle;
        std::cout << "Новое название: ";
        std::cin.ignore();
        std::getline(std::cin, newTitle);
        book->setTitle(newTitle);
        break;
    }
    case 2: { // изменить автора
        std::cout << "1. Выбрать существующего автора\n";
        std::cout << "2. Создать нового автора\n";
        int sub; std::cin >> sub;
        if (sub == 1) {
            for (auto& a : authorsRepo.getAll()) {
                std::cout << *a << "\n";
            }
            std::string authorId;
            std::cout << "Введите ID нового автора: ";
            std::cin >> authorId;
            auto newAuthor = authorsRepo.get(authorId);
            if (newAuthor) {
                book->setAuthor(newAuthor);
                newAuthor->addBook(book->getId());
                std::cout << "Автор изменён.\n";
            }
            else {
                std::cout << "Автор не найден.\n";
            }
        }
        else if (sub == 2) {
            FIO fio;
            std::cout << "Введите ФИО нового автора: ";
            std::cin >> fio;
            std::string bio;
            std::cout << "Введите биографию: ";
            std::cin.ignore();
            std::getline(std::cin, bio);
            auto newAuthor = std::make_shared<Author>(fio, bio);
            authorsRepo.add(newAuthor);
            book->setAuthor(newAuthor);
            newAuthor->addBook(book->getId());
            std::cout << "Новый автор создан и назначен.\n";
        }
        break;
    }

    case 3: { std::string newIsbn; std::cin >> newIsbn; book->setIsbn(newIsbn); break; }
    case 4: { int newYear; std::cin >> newYear; book->setYear(newYear); break; }
    case 5: { int newQty; std::cin >> newQty; book->setQuantity(newQty); break; }
    case 6: { std::string newGenre; std::cin >> newGenre; book->setGenre(newGenre); break; }
    }

    booksRepo.saveToBinaryFile("books.bin");
    std::cout << "Книга обновлена.\n";
}


void LibrarySystem::deleteBook() {
    std::string bookId;
    std::cout << "=== Удаление книги ===\n";
    std::cout << "Введите ID книги для удаления: ";
    std::cin >> bookId;

    auto book = booksRepo.get(bookId);
    if (!book) {
        std::cout << "Книга с ID " << bookId << " не найдена.\n";
        return;
    }

    std::string bookTitle = book->getTitle();

    if (booksRepo.remove(bookId)) {
        if (auto a = book->getAuthor()) {
            a->removeBook(bookId);
        }
        booksRepo.saveToBinaryFile("books.bin");

        addToActionHistory("Удалена книга: " + bookTitle);
        std::cout << "Книга успешно удалена!\n";
    }
    else {
        std::cout << "Ошибка при удалении книги!\n";
    }
}

void LibrarySystem::showAllBooks() const {
    std::cout << "=== Список всех книг (" << booksRepo.size() << ") ===\n";
    if (booksRepo.empty()) {
        std::cout << "В библиотеке нет книг.\n";
        return;
    }

    for (const auto& book : booksRepo.getAll()) {
        std::cout << *book << std::endl;
    }
}

void LibrarySystem::searchBooks() {
    std::string query;
    std::cout << "Введите поисковый запрос (название, автор или жанр): ";
    std::cin.ignore();
    std::getline(std::cin, query);

    std::cout << "Результаты поиска:\n";
    bool found = false;

    auto results = booksRepo.findIf([&query](const std::shared_ptr<Book>& book) {
        return book->getTitle().find(query) != std::string::npos ||
            (book->getAuthor() && book->getAuthor()->getFIO().getFullName().find(query) != std::string::npos) ||
            book->getGenre().find(query) != std::string::npos;
        });

    for (const auto& book : results) {
        std::cout << *book << std::endl;
        found = true;
    }

    if (!found) {
        std::cout << "Книги по запросу не найдены.\n";
    }
}

void LibrarySystem::sortBooks() {
    std::cout << "Сортировка книг:\n";
    std::cout << "1. По названию\n";
    std::cout << "2. По году издания\n";
    std::cout << "3. По автору\n";
    std::cout << "4. По количеству\n";
    std::cout << "Выберите критерий сортировки: ";

    int choice;
    std::cin >> choice;

    auto books = booksRepo.getAll();

    switch (choice) {
    case 1:
        std::sort(books.begin(), books.end(),
            [](const std::shared_ptr<Book>& a, const std::shared_ptr<Book>& b) {
                return a->getTitle() < b->getTitle();
            });
        break;
    case 2:
        std::sort(books.begin(), books.end(),
            [](const std::shared_ptr<Book>& a, const std::shared_ptr<Book>& b) {
                return a->getYear() < b->getYear();
            });
        break;
    case 3:
        std::sort(books.begin(), books.end(),
            [](const std::shared_ptr<Book>& a, const std::shared_ptr<Book>& b) {
                std::string authorA = a->getAuthor() ? a->getAuthor()->getFIO().getFullName() : "";
                std::string authorB = b->getAuthor() ? b->getAuthor()->getFIO().getFullName() : "";
                return authorA < authorB;
            });
        break;
    case 4:
        std::sort(books.begin(), books.end(),
            [](const std::shared_ptr<Book>& a, const std::shared_ptr<Book>& b) {
                return a->getQuantity() < b->getQuantity();
            });
        break;
    default:
        std::cout << "Неверный выбор!\n";
        return;
    }

    std::cout << "=== Отсортированные книги ===\n";
    for (const auto& book : books) {
        std::cout << *book << std::endl;
    }
}

void LibrarySystem::generateReport() {
    std::ofstream report("report.txt");
    if (!report.is_open()) {
        std::cout << "Ошибка: не удалось создать файл отчёта.\n";
        return;
    }

    report << "=== Отчёт о библиотеке ===\n\n";

    report << "Книг в системе: " << booksRepo.size() << "\n";
    report << "Авторов: " << authorsRepo.size() << "\n";
    report << "Пользователей: " << usersMap.size() << "\n";
    report << "Взятых книг: " << borrowedBooks.size() << "\n\n";

    report << "--- Список книг ---\n";
    for (auto& b : booksRepo.getAll()) {
        report << *b << "\n";
    }

    report << "\n--- Список авторов ---\n";
    for (auto& a : authorsRepo.getAll()) {
        report << *a << "\n";
    }

    report << "\n--- Список пользователей ---\n";
    for (auto& p : usersMap) {
        report << *p.second << "\n";
    }

    report << "\n--- Взятые книги ---\n";
    for (auto& bb : borrowedBooks) {
        report << "Книга: " << bb.getBookId()
            << ", Читатель: " << bb.getReaderId()
            << ", Дата выдачи: " << bb.getBorrowDate()
            << (bb.isReturned() ? ", возвращена" : ", на руках")
            << "\n";
    }

    report.close();
    std::cout << "Отчёт сформирован: report.txt\n";
}


void LibrarySystem::ageBasedRecommendations() {
    if (!currentUser) return;

    int age = currentUser->getAge();
    std::cout << "Рекомендации для возраста " << age << " лет:\n";

    if (age < 18) {
        std::cout << "- Детская литература\n";
        std::cout << "- Подростковые романы\n";
        std::cout << "- Образовательные книги\n";

        auto childrenBooks = booksRepo.findIf([](const std::shared_ptr<Book>& book) {
            return book->getGenre().find("Детская") != std::string::npos ||
                book->getGenre().find("Для детей") != std::string::npos;
            });

        if (!childrenBooks.empty()) {
            std::cout << "\nПодходящие книги в каталоге:\n";
            for (const auto& book : childrenBooks) {
                std::cout << "- " << book->getTitle() << " (" << book->getGenre() << ")\n";
            }
        }
    }
    else if (age < 30) {
        std::cout << "- Современная проза\n";
        std::cout << "- Научная фантастика\n";
        std::cout << "- Деловая литература\n";
    }
    else {
        std::cout << "- Классическая литература\n";
        std::cout << "- Исторические романы\n";
        std::cout << "- Мемуары и биографии\n";
    }
}

void LibrarySystem::showStatistics() const {
    std::cout << "=== СТАТИСТИКА БИБЛИОТЕКИ ===\n";
    std::cout << "Всего пользователей: " << Person::getPersonCount() << "\n";
    std::cout << "Всего авторов: " << Author::getAuthorCount() << "\n";
    std::cout << "Книг в каталоге: " << booksRepo.size() << "\n";

    int readers = 0, librarians = 0, directors = 0;
    for (const auto& pair : usersMap) {
        if (pair.second->getRole() == "Reader") readers++;
        else if (pair.second->getRole() == "Librarian") librarians++;
        else if (pair.second->getRole() == "Director") directors++;
    }

    std::cout << "Читателей: " << readers << "\n";
    std::cout << "Библиотекарей: " << librarians << "\n";
    std::cout << "Директоров: " << directors << "\n";

    std::map<std::string, int> genreStats;
    int totalBooks = 0;

    for (const auto& book : booksRepo.getAll()) {
        genreStats[book->getGenre()]++;
        totalBooks += book->getQuantity();
    }

    std::cout << "\nСтатистика по жанрам:\n";
    for (const auto& stat : genreStats) {
        std::cout << "- " << stat.first << ": " << stat.second << " наименований\n";
    }
    std::cout << "Всего экземпляров книг: " << totalBooks << "\n";
}

void LibrarySystem::handleUserChoice(int choice) {
    if (!currentUser) return;

    std::string role = currentUser->getRole();

    // === ЧИТАТЕЛЬ ===
    if (role == "Reader") {
        switch (choice) {
        case 1: { // Просмотр книг
            std::cout << "=== Просмотр книг ===\n";
            std::cout << "1. Все книги\n";
            std::cout << "2. Поиск книг\n";
            std::cout << "3. Фильтрация книг\n";
            std::cout << "4. Просмотр авторов\n";
            std::cout << "5. Поиск авторов\n";
            std::cout << "6. Просмотр книг по автору\n";
            std::cout << "0. Выход\n";
            int sub; std::cin >> sub;
            switch (sub) {
            case 1: showAllBooks(); break;
            case 2: searchBooks(); break;
            case 3: filterBooks(); break;
            case 4: showAllAuthors(); break;
            case 5: searchAuthors(); break;
            case 6: showBooksByAuthor(); break;
            case 0: break;
            }
            break;
        }
        case 2: { // Мои книги
            std::dynamic_pointer_cast<Reader>(currentUser)->showBorrowedBooks();
            break;
        }
        case 3: logout(); break;
        default: std::cout << "Неверный выбор!\n";
        }
    }

    // === БИБЛИОТЕКАРЬ ===
    else if (role == "Librarian") {
        switch (choice) {
        case 1: { // Работа с книгами
            std::cout << "=== Работа с книгами ===\n";
            std::cout << "1. Все книги\n";
            std::cout << "2. Поиск книг\n";
            std::cout << "3. Фильтрация книг\n";
            std::cout << "4. Просмотр авторов\n";
            std::cout << "5. Поиск авторов\n";
            std::cout << "6. Просмотр книг по автору\n";
            std::cout << "0. Выход\n";
            int sub; std::cin >> sub;
            switch (sub) {
            case 1: showAllBooks(); break;
            case 2: searchBooks(); break;
            case 3: filterBooks(); break;
            case 4: showAllAuthors(); break;
            case 5: searchAuthors(); break;
            case 6: showBooksByAuthor(); break;
            case 0: break;
            }
            break;
        }
        case 2: { // Редактирование книг
            std::cout << "=== Редактирование книг ===\n";
            std::cout << "1. Добавить книгу\n";
            std::cout << "2. Редактировать книгу\n";
            std::cout << "3. Удалить книгу\n";
            std::cout << "0. Выход\n";
            int sub; std::cin >> sub;
            switch (sub) {
            case 1: addBook(); break;
            case 2: editBook(); break;
            case 3: deleteBook(); break;
            case 0: break;
            }
            break;
        }
        case 3: { // Работа с читателями
            std::cout << "=== Работа с читателями ===\n";
            std::cout << "1. Просмотреть всех читателей\n";
            std::cout << "2. Просмотреть книги конкретного читателя\n";
            std::cout << "3. Зарегистрировать нового читателя\n";
            std::cout << "0. Выход\n";
            int sub; std::cin >> sub;
            switch (sub) {
            case 1: {
                for (const auto& pair : usersMap) {
                    if (pair.second->getRole() == "Reader")
                        std::cout << *pair.second << "\n";
                }
                break;
            }
            case 2: {
                std::string rid;
                std::cout << "Введите ID читателя: ";
                std::cin >> rid;
                showReaderBorrowedBooks(rid);
                break;
            }
            case 3: registerReader(); break;
            case 0: break;
            }
            break;
        }
        case 4: { // Выдача/приём книг
            std::cout << "=== Выдача/приём книг ===\n";
            std::cout << "1. Выдать книгу\n";
            std::cout << "2. Принять книгу\n";
            std::cout << "0. Выход\n";
            int sub; std::cin >> sub;
            switch (sub) {
            case 1: issueBook(); break;
            case 2: acceptBook(); break;
            case 0: break;
            }
            break;
        }
        case 5: logout(); break;
        default: std::cout << "Неверный выбор!\n";
        }
    }

    // === ДИРЕКТОР ===
    else if (role == "Director") {
        switch (choice) {
        case 1: { // Просмотр книг
            std::cout << "=== Просмотр книг ===\n";
            std::cout << "1. Все книги\n";
            std::cout << "2. Поиск книг\n";
            std::cout << "3. Фильтрация книг\n";
            std::cout << "4. Просмотр авторов\n";
            std::cout << "5. Поиск авторов\n";
            std::cout << "6. Просмотр книг по автору\n";
            std::cout << "0. Выход\n";
            int sub; std::cin >> sub;
            switch (sub) {
            case 1: showAllBooks(); break;
            case 2: searchBooks(); break;
            case 3: filterBooks(); break;
            case 4: showAllAuthors(); break;
            case 5: searchAuthors(); break;
            case 6: showBooksByAuthor(); break;
            case 0: break;
            }
            break;
        }
        case 2: { // Редактирование книг
            std::cout << "=== Редактирование книг ===\n";
            std::cout << "1. Добавить книгу\n";
            std::cout << "2. Редактировать книгу\n";
            std::cout << "3. Удалить книгу\n";
            std::cout << "0. Выход\n";
            int sub; std::cin >> sub;
            switch (sub) {
            case 1: addBook(); break;
            case 2: editBook(); break;
            case 3: deleteBook(); break;
            case 0: break;
            }
            break;
        }
        case 3: { // Работа с пользователями
            std::cout << "=== Работа с пользователями ===\n";
            std::cout << "1. Просмотреть всех пользователей\n";
            std::cout << "2. Зарегистрировать библиотекаря\n";
            std::cout << "3. Зарегистрировать директора\n";
            std::cout << "4. Удалить пользователя\n";
            std::cout << "0. Выход\n";
            int sub; std::cin >> sub;
            switch (sub) {
            case 1: for (auto& p : usersMap) std::cout << *p.second << "\n"; break;
            case 2: registerLibrarian(); break;
            case 3: registerDirector(); break;
            case 4: {
                std::string uid;
                std::cout << "Введите ID пользователя: ";
                std::cin >> uid;
                deleteUser(uid);
                break;
            }
            case 0: break;
            }
            break;
        }
        case 4: { // Выдача/приём книг
            std::cout << "=== Выдача/приём книг ===\n";
            std::cout << "1. Выдать книгу\n";
            std::cout << "2. Принять книгу\n";
            std::cout << "0. Выход\n";
            int sub; std::cin >> sub;
            switch (sub) {
            case 1: issueBook(); break;
            case 2: acceptBook(); break;
            case 0: break;
            }
            break;
        }
        case 5: {
            std::cout << "=== Статистика и отчёты ===\n";
            std::cout << "1. Показать статистику\n";
            std::cout << "2. Сформировать текстовый отчёт\n";
            std::cout << "0. Выход\n";
            int sub; std::cin >> sub;
            switch (sub) {
            case 1: showStatistics(); break;
            case 2: generateReport(); break;
            case 0: break;
            }
            break;
        }
        case 6: showActionHistory(); break;
        case 7: logout(); break;
        default: std::cout << "Неверный выбор!\n";
        }
    }
}


void LibrarySystem::run() {
    std::cout << "=== Запуск библиотечной системы ===\n";

    while (!shouldExit) {
        if (!currentUser) {
            showAuthMenu();
        }
        else {
            showUserMenu();
        }
    }

    std::cout << "Завершение работы системы...\n";
}

std::shared_ptr<Book> LibrarySystem::findBookById(const std::string& id) const {
    return booksRepo.get(id);
}

std::shared_ptr<Person> LibrarySystem::findUserById(const std::string& id) const {
    auto it = usersMap.find(id);
    return (it != usersMap.end()) ? it->second : nullptr;
}

std::shared_ptr<Author> LibrarySystem::findAuthorById(const std::string& id) const {
    return authorsRepo.get(id);
}

void LibrarySystem::issueBook() {
    std::cout << "=== Выдача книги ===\n";
    std::string readerId, bookId;

    std::cout << "Введите ID читателя: ";
    std::cin >> readerId;
    auto reader = std::dynamic_pointer_cast<Reader>(findUserById(readerId));
    if (!reader) {
        std::cout << "Пользователь не найден или не является читателем.\n";
        return;
    }

    std::cout << "Введите ID книги: ";
    std::cin >> bookId;
    auto book = booksRepo.get(bookId);
    if (!book) {
        std::cout << "Книга не найдена.\n";
        return;
    }
    if (book->getQuantity() <= 0) {
        std::cout << "Нет доступных экземпляров для выдачи.\n";
        return;
    }

    // выбор даты выдачи
    std::cout << "Вы хотите ввести дату вручную? (1 - да, 0 - нет): ";
    int choice; std::cin >> choice;
    Date borrowDate;
    if (choice == 1) {
        std::cout << "Введите дату (дд.мм.гггг): ";
        std::cin >> borrowDate;
        if (!Date::isValid(borrowDate.getDay(), borrowDate.getMonth(), borrowDate.getYear())) {
            std::cout << "Ошибка: некорректная дата.\n";
            return;
        }
    }
    else {
        borrowDate = Date::today();
    }

    book->setQuantity(book->getQuantity() - 1);

    reader->borrowBook(bookId, borrowDate);

    // создаём запись о выдаче (срок возврата = borrowDate + 14 дней)
    BorrowedBook bb(bookId, readerId, borrowDate);
    borrowedBooks.push_back(bb);

    booksRepo.saveToBinaryFile("books.bin");
    PersonFactory::saveAllToBinaryFile(usersMap, "users.bin");

    std::map<std::string, BorrowedBook> borrowedBooksMap;
    int counter = 0;
    for (const auto& b : borrowedBooks) {
        std::string key = b.getBookId() + "_" + b.getReaderId() + "_" + std::to_string(counter++);
        borrowedBooksMap[key] = b;
    }
    BorrowedBook::saveAllToBinaryFile(borrowedBooksMap, "borrowed.bin");

    addToActionHistory("Выдана книга: " + bookId + " читателю " + readerId);
    std::cout << "Книга выдана. Срок возврата: " << bb.getReturnDate() << "\n";
}


void LibrarySystem::acceptBook() {
    std::cout << "=== Приём книги ===\n";
    std::string readerId, bookId;

    std::cout << "Введите ID читателя: ";
    std::cin >> readerId;
    auto reader = std::dynamic_pointer_cast<Reader>(findUserById(readerId));
    if (!reader) {
        std::cout << "Пользователь не найден или не является читателем.\n";
        return;
    }

    std::cout << "Введите ID книги: ";
    std::cin >> bookId;
    auto book = booksRepo.get(bookId);
    if (!book) {
        std::cout << "Книга не найдена.\n";
        return;
    }

    bool found = false;
    for (auto& b : borrowedBooks) {
        if (b.getBookId() == bookId && b.getReaderId() == readerId && !b.isReturned()) {
            b.markReturned();

            Date today = Date::today();
            Date deadline = b.getReturnDate();

            // проверка просрочки
            if (today.getYear() > deadline.getYear() ||
                (today.getYear() == deadline.getYear() &&
                    (today.getMonth() > deadline.getMonth() ||
                        (today.getMonth() == deadline.getMonth() &&
                            today.getDay() > deadline.getDay())))) {
                std::cout << "Книга возвращена с просрочкой! Срок был до: " << deadline << "\n";
            }
            else {
                std::cout << "Книга возвращена вовремя. Срок был до: " << deadline << "\n";
            }

            found = true;
            break;
        }
    }

    if (found) {
        book->setQuantity(book->getQuantity() + 1);
        reader->returnBook(bookId);
    }
    else {
        std::cout << "Запись о выдаче не найдена.\n";
    }

    booksRepo.saveToBinaryFile("books.bin");
    PersonFactory::saveAllToBinaryFile(usersMap, "users.bin");

    std::map<std::string, BorrowedBook> borrowedBooksMap;
    int counter = 0;
    for (const auto& b : borrowedBooks) {
        std::string key = b.getBookId() + "_" + b.getReaderId() + "_" + std::to_string(counter++);
        borrowedBooksMap[key] = b;
    }
    BorrowedBook::saveAllToBinaryFile(borrowedBooksMap, "borrowed.bin");

    addToActionHistory("Принята книга: " + bookId + " от читателя " + readerId);
}



void LibrarySystem::filterBooks() {
    std::cout << "=== Фильтрация книг ===\n";
    std::cout << "1. По жанру\n";
    std::cout << "2. По диапазону годов\n";
    std::cout << "3. Только доступные (quantity > 0)\n";
    std::cout << "Выберите вариант: ";
    int choice;
    std::cin >> choice;

    std::vector<std::shared_ptr<Book>> filtered;
    if (choice == 1) {
        std::cout << "Введите жанр: ";
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::string genre;
        std::getline(std::cin, genre);
        filtered = booksRepo.findIf([&](const std::shared_ptr<Book>& b) {
            return b->getGenre().find(genre) != std::string::npos;
            });
    }
    else if (choice == 2) {
        int y1, y2;
        std::cout << "Введите начальный год: ";
        std::cin >> y1;
        std::cout << "Введите конечный год: ";
        std::cin >> y2;
        if (y1 > y2) std::swap(y1, y2);
        filtered = booksRepo.findIf([=](const std::shared_ptr<Book>& b) {
            return b->getYear() >= y1 && b->getYear() <= y2;
            });
    }
    else if (choice == 3) {
        filtered = booksRepo.findIf([](const std::shared_ptr<Book>& b) {
            return b->getQuantity() > 0;
            });
    }
    else {
        std::cout << "Неверный выбор.\n";
        return;
    }

    if (filtered.empty()) {
        std::cout << "По фильтру книги не найдены.\n";
    }
    else {
        std::cout << "Результаты:\n";
        for (auto& b : filtered) std::cout << *b << "\n";
    }
}

void LibrarySystem::showReaderBorrowedBooks(const std::string& readerId) {
    auto person = findUserById(readerId);
    auto reader = std::dynamic_pointer_cast<Reader>(person);
    if (!reader) {
        std::cout << "Пользователь не найден или не является читателем.\n";
        return;
    }
    reader->showBorrowedBooks();
}

void LibrarySystem::deleteUser(const std::string& userId) {
    auto person = findUserById(userId);
    if (!person) {
        std::cout << "Пользователь не найден.\n";
        return;
    }

    auto reader = std::dynamic_pointer_cast<Reader>(person);
    if (reader) {
        for (const auto& b : reader->getBorrowedBooks()) {
            if (!b.isReturned()) {
                std::cout << "Ошибка: читатель не вернул книги. Удаление запрещено.\n";
                return;
            }
        }
    }

    usersMap.erase(userId);
    PersonFactory::saveAllToBinaryFile(usersMap, "users.bin");
    std::cout << "Пользователь удалён.\n";
}

void LibrarySystem::showAllAuthors() const {
    std::cout << "=== Авторы ===\n";
    for (auto& a : authorsRepo.getAll()) {
        std::cout << *a << "\n";
    }
}

void LibrarySystem::searchAuthors() {
    std::cout << "Введите фамилию или имя автора: ";
    std::string query;
    std::cin.ignore();
    std::getline(std::cin, query);

    auto results = authorsRepo.findIf([&](const std::shared_ptr<Author>& a) {
        return a->getFIO().getFullName().find(query) != std::string::npos;
        });

    if (results.empty()) {
        std::cout << "Авторы не найдены.\n";
    }
    else {
        for (auto& a : results) std::cout << *a << "\n";
    }
}

void LibrarySystem::showBooksByAuthor() {
    std::cout << "Введите фамилию или имя автора: ";
    std::string query;
    std::cin.ignore();
    std::getline(std::cin, query);

    auto results = authorsRepo.findIf([&](const std::shared_ptr<Author>& a) {
        return a->getFIO().getFullName().find(query) != std::string::npos;
        });

    if (results.empty()) {
        std::cout << "Автор не найден.\n";
        return;
    }

    for (auto& author : results) {
        std::cout << "Книги автора " << author->getFIO().getFullName() << ":\n";
        for (const auto& bookId : author->getBookIds()) {
            auto book = booksRepo.get(bookId);
            if (book) std::cout << *book << "\n";
        }
    }
}
