#include "dictionary.hpp"
#include "command_processor.hpp"
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <windows.h>
#include <clocale>

void expect(bool cond, const std::string& msg, int& pass, int& total);
void displayTranslations(Dictionary& dict, const std::string& key);

int main()
{
    setlocale(LC_ALL, ".UTF8");
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    int passed = 0, total = 0;
    Dictionary dict;

    expect(dict.insert("Cat", "Кошка"), "Insert Cat–Кошка", passed, total);
    expect(!dict.insert("Cat", "Кошка"), "Reject duplicate", passed, total);
    expect(dict.insert("Cat", "Кот"), "Insert another translation", passed, total);

    expect(!dict.insert("Cat1", "Кошка"), "Reject key with digit", passed, total);
    expect(!dict.insert("C@t#", "Кошка"), "Reject key with symbols", passed, total);
    expect(!dict.insert("Mo ther", "Мама"), "Reject key with space", passed, total);
    expect(!dict.insert("Лес", "Forest"), "Reject swapped key/value", passed, total);

    expect(!dict.insert("Ё", "1"), "Reject translation as number", passed, total);
    expect(!dict.insert("1", "Ё"), "Reject key as number", passed, total);
    expect(!dict.insert("!Ё", "Ё"), "Reject key starting with symbol", passed, total);

    std::stringstream buf1;
    bool ok1 = dict.search("Cat1", buf1);
    expect(!ok1, "Search rejected (digit in key)", passed, total);

    std::stringstream buf2;
    bool ok2 = dict.search("C@t#", buf2);
    expect(!ok2, "Search rejected (symbol in key)", passed, total);

    try
    {
        dict.remove("");
        expect(false, "Reject empty key on remove", passed, total);
    }
    catch (const std::invalid_argument&)
    {
        expect(true, "Throw on empty key in remove", passed, total);
    }

    expect(!dict.remove("Cat1"), "Reject removal of digit-key", passed, total);
    expect(!dict.remove("C@t#"), "Reject removal of symbol-key", passed, total);

    displayTranslations(dict, "Cat");
    expect(dict.remove("Cat"), "Remove Cat", passed, total);
    displayTranslations(dict, "Cat");

    expect(dict.insert("High-school", "Средняя-школа"), "Insert key and translation with single hyphen", passed, total);
    displayTranslations(dict, "High-school");

    std::cout << "Загрузка словаря с несколькими словами:\n\n";
    expect(dict.insert("Library", "Библиотека"), "Insert Library – Библиотека", passed, total);
    expect(dict.insert("Library", "Читальня"), "Insert Library – Читальня", passed, total);
    expect(dict.insert("Computer", "Компьютер"), "Insert Computer – Компьютер", passed, total);
    expect(dict.insert("Computer", "Вычислитель"), "Insert Computer – Вычислитель", passed, total);
    expect(dict.insert("Science", "Наука"), "Insert Science – Наука", passed, total);
    expect(dict.insert("Science", "Естествознание"), "Insert Science – Естествознание\n", passed, total);

    displayTranslations(dict, "Library");
    displayTranslations(dict, "Computer");
    displayTranslations(dict, "Science");

    std::cout << "\nTest Summary: " << passed << '/' << total << " passed.\n";
    std::cout << "\nEntering interactive mode. Type HELP to see commands.\n";


    Dictionary testdict;
    CommandProcessor cp(testdict);

    cp.run(std::cin, std::cout);
    return 0;
}

void expect(bool cond, const std::string& msg, int& pass, int& total)
{
    ++total;
    if (cond)
    {
        ++pass;
        std::cout << "[OK] " << msg << '\n';
    }
    else
    {
        std::cout << "[FAIL] " << msg << '\n';
    }
}

void displayTranslations(Dictionary& dict, const std::string& key)
{
    std::cout << "Содержимое по ключу \"" << key << "\": ";
    if (!dict.search(key, std::cout))
    {
        std::cout << "NOT FOUND";
    }
    std::cout << '\n';
}
