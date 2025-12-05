#pragma once
#include "FIO.h"
#include <string>
#include <memory>
#include <iostream>
#include <fstream>
#include <map>

class Person {
protected:
    FIO fio;
    std::string login;
    std::string encryptedPassword;
    int age;
    std::string id;

    static int person_count;

public:
    Person(const FIO& f = FIO(), const std::string& log = "",
        const std::string& pwd = "", int age = 0, const std::string& personId = "");
    virtual ~Person() = default;

    virtual void showMenu() = 0;
    virtual std::string getRole() const = 0;

    virtual bool authenticate(const std::string& pwd) const;
    virtual void changePassword(const std::string& newPwd);

    FIO getFIO() const { return fio; }
    std::string getLogin() const { return login; }
    int getAge() const { return age; }
    std::string getId() const { return id; }

    static int getPersonCount() { return person_count; }
    static void incrementPersonCount() { person_count++; }
    static void setPersonCount(int count) { person_count = count; }

    friend std::ostream& operator<<(std::ostream& os, const Person& person);
    friend std::istream& operator>>(std::istream& is, Person& person);

    virtual void saveToBinaryFile(std::ofstream& file) const;
    virtual void loadFromBinaryFile(std::ifstream& file);

    virtual std::string generateId() const;
};