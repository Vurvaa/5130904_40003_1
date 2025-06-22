#ifndef DICTIONARY_HPP
#define DICTIONARY_HPP

#include <string>
#include <vector>
#include <memory>
#include <set>
#include <iostream>

class Dictionary
{
public:
    Dictionary(std::size_t initialCap = DEFAULT_CAPACITY);
    ~Dictionary() = default;

    Dictionary(const Dictionary& other) = delete;
    Dictionary(Dictionary&& other) = delete;
    Dictionary& operator= (const Dictionary&) = delete;
    Dictionary& operator= (Dictionary&& other) = delete;

    bool insert(const std::string& eng, const std::string& rus);
    bool search(const std::string& eng, std::ostream& out);
    bool remove(const std::string& eng);

private:
    struct Entry
    {
        std::string english_;
        std::set<std::string> translations_;
        std::unique_ptr<Entry> next_;

        Entry(const std::string& eng);
    };

    std::vector<std::unique_ptr<Entry>> buckets_;
    std::size_t capacity_;
    std::size_t size_;

    static const double MAX_LOAD_FACTOR;
    static const size_t DEFAULT_CAPACITY;

    std::size_t hash(const std::string& s, std::size_t mod) const;
    Entry* findEntry(const std::string& english) const;
    void rehash();
    bool IsCorrectFormat(const std::string& s);
};

#endif
