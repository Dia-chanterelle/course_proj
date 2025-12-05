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
    auto librarian = std::make_shared<Librarian>(
        FIO("Иванова", "Мария", "Петровна"),
        "librarian",
        "lib123",
        35
    );
    usersMap[librarian->getId()] = librarian;

    auto director = std::make_shared<Director>(
        FIO("Петров", "Алексей", "Сергеевич"),
        "director",
        "admin123",
        45
    );
    usersMap[director->getId()] = director;

    std::cout << "Созданы тестовые пользователи:\n";
    std::cout << "   - Библиотекарь: логин 'librarian', пароль 'lib123'\n";
    std::cout << "   - Директор: логин 'director', пароль 'admin123'\n";
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

        auto book1 = std::make_shared<Book>("Война и мир", author1, "978-5-389-00001-1", 1869, 5, "Роман");
        auto book2 = std::make_shared<Book>("Преступление и наказание", author1, "978-5-389-00002-2", 1866, 3, "Роман");

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

    auto newReader = std::make_shared<Reader>(fio, login, password, age);
    usersMap[newReader->getId()] = newReader;

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

    auto newLibrarian = std::make_shared<Librarian>(fio, login, password, age, "", department);
    usersMap[newLibrarian->getId()] = newLibrarian;

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

    auto newDirector = std::make_shared<Director>(fio, login, password, age);
    usersMap[newDirector->getId()] = newDirector;

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
    if (actionHistory.size() > 100) {
        std::stack<std::string> temp;
        while (!actionHistory.empty()) {
            temp.push(actionHistory.top());
            actionHistory.pop();
        }
        int count = 0;
        while (!temp.empty() && count < 50) {
            actionHistory.push(temp.top());
            temp.pop();
            count++;
        }
    }
}

void LibrarySystem::showActionHistory() {
    std::cout << "=== История действий ===\n";
    if (actionHistory.empty()) {
        std::cout << "История пуста.\n";
        return;
    }

    auto temp = actionHistory;
    std::vector<std::string> historyList;
    while (!temp.empty()) {
        historyList.push_back(temp.top());
        temp.pop();
    }

    for (int i = historyList.size() - 1; i >= 0; --i) {
        std::cout << "- " << historyList[i] << "\n";
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
    std::cout << "=== Редактирование книги ===\n";
    std::cout << "Введите ID книги для редактирования: ";
    std::cin >> bookId;

    auto book = booksRepo.get(bookId);
    if (!book) {
        std::cout << "Книга с ID " << bookId << " не найдена.\n";
        return;
    }

    std::string title, isbn, genre;
    int year, quantity;

    std::cout << "Текущее название: " << book->getTitle() << "\n";
    std::cout << "Введите новое название (или оставьте пустым для сохранения текущего): ";
    std::cin.ignore();
    std::getline(std::cin, title);
    if (!title.empty()) {
        book->setTitle(title);
    }

    std::cout << "Текущий ISBN: " << book->getIsbn() << "\n";
    std::cout << "Введите новый ISBN (или оставьте пустым): ";
    std::getline(std::cin, isbn);
    if (!isbn.empty()) {
        book->setIsbn(isbn);
    }

    std::cout << "Текущий год: " << book->getYear() << "\n";
    std::cout << "Введите новый год (или -1 для сохранения текущего): ";
    std::cin >> year;
    if (year != -1) {
        book->setYear(year);
    }

    std::cout << "Текущее количество: " << book->getQuantity() << "\n";
    std::cout << "Введите новое количество (или -1 для сохранения текущего): ";
    std::cin >> quantity;
    if (quantity != -1) {
        book->setQuantity(quantity);
    }

    std::cout << "Текущий жанр: " << book->getGenre() << "\n";
    std::cout << "Введите новый жанр (или оставьте пустым): ";
    std::cin.ignore();
    std::getline(std::cin, genre);
    if (!genre.empty()) {
        book->setGenre(genre);
    }

    booksRepo.saveToBinaryFile("books.bin");

    addToActionHistory("Отредактирована книга: " + book->getTitle());
    std::cout << "Книга успешно отредактирована!\n";
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
    std::ofstream report("library_report.txt");
    if (!report.is_open()) {
        std::cout << "Ошибка создания отчета!\n";
        return;
    }

    report << "=== ОТЧЕТ БИБЛИОТЕКИ ===\n\n";
    report << "Общая статистика:\n";
    report << "Количество пользователей: " << usersMap.size() << "\n";
    report << "Количество книг: " << booksRepo.size() << "\n";
    report << "Количество авторов: " << authorsRepo.size() << "\n";
    report << "Выданных книг: " << borrowedBooks.size() << "\n\n";

    report << "Список книг:\n";
    for (const auto& book : booksRepo.getAll()) {
        report << *book << "\n";
    }

    report << "\nСписок пользователей:\n";
    for (const auto& pair : usersMap) {
        report << "ID: " << pair.second->getId()
            << ", Логин: " << pair.second->getLogin()
            << ", Роль: " << pair.second->getRole() << "\n";
    }

    report.close();
    std::cout << "Отчет сохранен в файл 'library_report.txt'\n";
    addToActionHistory("Сгенерирован отчет библиотеки");
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

    if (role == "Reader") {
        switch (choice) {
        case 1:
            showAllBooks();
            break;
        case 2:
            searchBooks();
            break;
        case 3:
            if (auto reader = std::dynamic_pointer_cast<Reader>(currentUser)) {
                reader->showBorrowedBooks();
            }
            break;
        case 4:
            std::cout << "Функция изменения профиля\n";
            break;
        case 5:
            logout();
            break;
        default:
            std::cout << "Неверный выбор!\n";
        }
    }
    else if (role == "Librarian") {
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
            std::cout << "Функция выдачи книги\n";
            break;
        case 5:
            std::cout << "Функция приема книги\n";
            break;
        case 6:
            std::cout << "Список читателей:\n";
            for (const auto& pair : usersMap) {
                if (pair.second->getRole() == "Reader") {
                    std::cout << *(pair.second) << std::endl;
                }
            }
            break;
        case 7:
            ageBasedRecommendations();
            break;
        case 8:
            registerReader();
            break;
        case 9:
            logout();
            break;
        default:
            std::cout << "Неверный выбор!\n";
        }
    }
    else if (role == "Director") {
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
            generateReport();
            break;
        case 5:
            registerUser();
            break;
        case 6:
            showStatistics();
            break;
        case 7:
            showActionHistory();
            break;
        case 8:
            logout();
            break;
        default:
            std::cout << "Неверный выбор!\n";
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