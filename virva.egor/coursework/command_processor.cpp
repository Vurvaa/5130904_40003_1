#include "dictionary.hpp"
#include <stdexcept>
#include <cctype>

const double Dictionary::MAX_LOAD_FACTOR = 0.75;
const size_t Dictionary::DEFAULT_CAPACITY = 8;

Dictionary::Entry::Entry(const std::string& eng)
    : english_(eng), translations_(), next_(nullptr)
{
}

std::size_t Dictionary::hash(const std::string& s, std::size_t mod) const
{
    const std::size_t POLINOM = 31;
    std::size_t hash = 0;

    for (std::size_t i = 0; i < s.size(); ++i)
    {
        char ch = s[i];
        hash = (hash * POLINOM + ch) % mod;
    }

    return hash;
}

bool Dictionary::IsCorrectFormat(const std::string& s)
{
    if (s.empty())
    {
        return false;
    }

    if (!(s[0] >= 'A' && s[0] <= 'Z'))
    {
        return false;
    }

    for (std::size_t i = 1; i < s.size(); ++i)
    {
        char ch = s[i];

        if (ch == '-')
        {
            if (i + 1 >= s.size() || s[i + 1] == '-')
            {
                return false;
            }
        }
        else
        {
            if (!(ch >= 'a' && ch <= 'z'))
            {
                return false;
            }
        }
    }

    return true;
}

Dictionary::Entry* Dictionary::findEntry(const std::string& english) const
{
    std::size_t idx = hash(english, capacity_);
    Entry* temp = buckets_[idx].get();
    while (temp && temp->english_ != english)
    {
        temp = temp->next_.get();
    }
    return temp;
}

Dictionary::Dictionary(std::size_t initialCap)
    : capacity_(initialCap), size_(0), buckets_(initialCap)
{
}

bool Dictionary::insert(const std::string& eng, const std::string& rus)
{
    if (eng.empty() || rus.empty())
    {
        throw std::invalid_argument("empty key or value");
    }

    if (!IsCorrectFormat(eng))
    {
        std::cout << "Format error: \"" << eng << "\" must be FirstUpper+restlower english word\n";
        return false;
    }

    Entry* temp = findEntry(eng);
    if (temp)
    {
        auto result = temp->translations_.insert(rus);
        return result.second;
    }

    std::size_t idx = hash(eng, capacity_);
    auto newEntry = std::make_unique<Entry>(eng);
    newEntry->translations_.insert(rus);
    newEntry->next_ = std::move(buckets_.at(idx));
    buckets_.at(idx) = std::move(newEntry);
    ++size_;

    if (static_cast<double>(size_) / capacity_ > MAX_LOAD_FACTOR)
    {
        rehash();
    }

    return true;
}

bool Dictionary::search(const std::string& eng, std::ostream& out)
{
    if (eng.empty())
    {
        throw std::invalid_argument("empty key");
    }

    if (!IsCorrectFormat(eng))
    {
        std::cout << "Format error: \"" << eng << "\" must be FirstUpper+restlower\n";
        return false;
    }

    Entry* temp = findEntry(eng);
    if (!temp)
    {
        return false;
    }

    for (auto it = temp->translations_.begin(); it != temp->translations_.end(); ++it)
    {
        out << *it;
        if (std::next(it) != temp->translations_.end())
        {
            out << ' ';
        }
        else
        {
            out << '\n';
        }
    }

    return true;
}

bool Dictionary::remove(const std::string& eng)
{
    if (eng.empty())
    {
        throw std::invalid_argument("empty key");
    }

    if (!IsCorrectFormat(eng))
    {
        std::cerr << "Format error: \"" << eng << "\" must be FirstUpper+restlower\n";
        return false;
    }

    std::size_t idx = hash(eng, capacity_);
    Entry* prev = nullptr;
    Entry* curr = buckets_[idx].get();

    while (curr && curr->english_ != eng)
    {
        prev = curr;
        curr = curr->next_.get();
    }

    if (!curr)
    {
        return false;
    }

    if (!prev)
    {
        buckets_[idx] = std::move(curr->next_);
    }
    else
    {
        prev->next_ = std::move(curr->next_);
    }

    --size_;
    return true;
}

void Dictionary::rehash()
{
    std::size_t newCap = capacity_ * 2;
    std::vector<std::unique_ptr<Entry>> newBuckets(newCap);

    for (std::size_t i = 0; i < buckets_.size(); ++i)
    {
        std::unique_ptr<Entry>& bucket = buckets_[i];

        while (bucket)
        {
            Entry* raw = bucket.release();
            Entry* next = raw->next_.release();

            std::size_t idx = hash(raw->english_, newCap);
            raw->next_ = std::move(newBuckets[idx]);
            newBuckets[idx] = std::unique_ptr<Entry>(raw);

            bucket.reset(next);
        }
    }

    buckets_ = std::move(newBuckets);
    capacity_ = newCap;
}
