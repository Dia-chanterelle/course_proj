#include "../headers/PersonFactory.h"
#include <fstream>

// Счётчики для уникальных ID
int PersonFactory::readerCount = 0;
int PersonFactory::librarianCount = 0;
int PersonFactory::directorCount = 0;

// === Создание новых пользователей ===
std::shared_ptr<Reader> PersonFactory::createReader(const FIO& f,
    const std::string& log,
    const std::string& pwd,
    int age) {
    std::string id = "R" + std::to_string(++readerCount);
    return std::make_shared<Reader>(f, log, pwd, age, id);
}

std::shared_ptr<Librarian> PersonFactory::createLibrarian(const FIO& f,
    const std::string& log,
    const std::string& pwd,
    int age,
    const std::string& dept) {
    std::string id = "L" + std::to_string(++librarianCount);
    return std::make_shared<Librarian>(f, log, pwd, age, id, dept);
}

std::shared_ptr<Director> PersonFactory::createDirector(const FIO& f,
    const std::string& log,
    const std::string& pwd,
    int age) {
    std::string id = "D" + std::to_string(++directorCount);
    return std::make_shared<Director>(f, log, pwd, age, id);
}

//=====
void PersonFactory::saveAllToBinaryFile(
    const std::map<std::string, std::shared_ptr<Person>>& users,
    const std::string& filename) {

    std::ofstream out(filename, std::ios::binary | std::ios::trunc);
    if (!out.is_open()) return;

    size_t size = users.size();
    out.write(reinterpret_cast<const char*>(&size), sizeof(size));

    for (const auto& [key, person] : users) {
        int role = 0;
        if (std::dynamic_pointer_cast<Director>(person)) role = 2;
        else if (std::dynamic_pointer_cast<Librarian>(person)) role = 1;
        else if (std::dynamic_pointer_cast<Reader>(person)) role = 0;

        out.write(reinterpret_cast<const char*>(&role), sizeof(role));

        const std::string& login = person->getLogin();
        size_t lenLogin = login.size();
        out.write(reinterpret_cast<const char*>(&lenLogin), sizeof(lenLogin));
        out.write(login.data(), lenLogin);

        const std::string& pwd = person->getEncryptedPassword();
        size_t lenPwd = pwd.size();
        out.write(reinterpret_cast<const char*>(&lenPwd), sizeof(lenPwd));
        out.write(pwd.data(), lenPwd);

        const std::string& id = person->getId();
        size_t lenId = id.size();
        out.write(reinterpret_cast<const char*>(&lenId), sizeof(lenId));
        out.write(id.data(), lenId);

        int age = person->getAge();
        out.write(reinterpret_cast<const char*>(&age), sizeof(age));

        std::string fullName = person->getFIO().getFullName();
        size_t lenName = fullName.size();
        out.write(reinterpret_cast<const char*>(&lenName), sizeof(lenName));
        out.write(fullName.data(), lenName);

        // department for Librarian
        if (role == 1) {
            auto lib = std::dynamic_pointer_cast<Librarian>(person);
            const std::string& dept = lib->getDepartment();
            size_t lenDept = dept.size();
            out.write(reinterpret_cast<const char*>(&lenDept), sizeof(lenDept));
            out.write(dept.data(), lenDept);
        }
    }
}

std::map<std::string, std::shared_ptr<Person>>
PersonFactory::loadAllFromBinaryFile(const std::string& filename) {
    std::map<std::string, std::shared_ptr<Person>> users;

    std::ifstream in(filename, std::ios::binary);
    if (!in.is_open()) return users;

    size_t size = 0;
    in.read(reinterpret_cast<char*>(&size), sizeof(size));

    for (size_t i = 0; i < size; ++i) {
        int role = 0;
        in.read(reinterpret_cast<char*>(&role), sizeof(role));

        size_t lenLogin = 0;
        in.read(reinterpret_cast<char*>(&lenLogin), sizeof(lenLogin));
        std::string login(lenLogin, '\0');
        in.read(&login[0], lenLogin);

        size_t lenPwd = 0;
        in.read(reinterpret_cast<char*>(&lenPwd), sizeof(lenPwd));
        std::string encryptedPwd(lenPwd, '\0');
        in.read(&encryptedPwd[0], lenPwd);

        size_t lenId = 0;
        in.read(reinterpret_cast<char*>(&lenId), sizeof(lenId));
        std::string id(lenId, '\0');
        in.read(&id[0], lenId);

        int age = 0;
        in.read(reinterpret_cast<char*>(&age), sizeof(age));

        size_t lenName = 0;
        in.read(reinterpret_cast<char*>(&lenName), sizeof(lenName));
        std::string fullName(lenName, '\0');
        in.read(&fullName[0], lenName);

        FIO fio;
        if (!fullName.empty()) {
            size_t s1 = fullName.find(' ');
            size_t s2 = fullName.find(' ', s1 == std::string::npos ? 0 : s1 + 1);
            if (s1 != std::string::npos && s2 != std::string::npos) {
                fio = FIO(fullName.substr(0, s1),
                    fullName.substr(s1 + 1, s2 - s1 - 1),
                    fullName.substr(s2 + 1));
            }
            else {
                fio = FIO(fullName, "", "");
            }
        }

        if (role == 0) {
            auto p = std::make_shared<Reader>(fio, login, encryptedPwd, age, id, true);
            users.emplace(id, p);
        }
        else if (role == 1) {
            // department
            size_t lenDept = 0;
            in.read(reinterpret_cast<char*>(&lenDept), sizeof(lenDept));
            std::string dept(lenDept, '\0');
            in.read(&dept[0], lenDept);

            auto p = std::make_shared<Librarian>(fio, login, encryptedPwd, age, id, dept, true);
            users.emplace(id, p);
        }
        else if (role == 2) {
            auto p = std::make_shared<Director>(fio, login, encryptedPwd, age, id, true);
            users.emplace(id, p);
        }
        else {
            continue;
        }
    }

    in.close();

    // обновить счётчики
    for (const auto& [uid, person] : users) {
        if (!uid.empty()) {
            if (uid[0] == 'R') {
                int num = std::stoi(uid.substr(1));
                readerCount = std::max(readerCount, num);
            }
            else if (uid[0] == 'L') {
                int num = std::stoi(uid.substr(1));
                librarianCount = std::max(librarianCount, num);
            }
            else if (uid[0] == 'D') {
                int num = std::stoi(uid.substr(1));
                directorCount = std::max(directorCount, num);
            }
        }
    }

    return users;
}


