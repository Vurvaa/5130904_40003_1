#ifndef COMMAND_PROCESSOR_HPP
#define COMMAND_PROCESSOR_HPP

#include "dictionary.hpp"
#include <string>
#include <vector>
#include <iostream>

class CommandProcessor
{
public:
    CommandProcessor(Dictionary& dict);
    ~CommandProcessor() = default;

    CommandProcessor(const CommandProcessor& other) = delete;
    CommandProcessor(CommandProcessor&& other) = delete;
    CommandProcessor& operator= (const CommandProcessor&) = delete;
    CommandProcessor& operator= (CommandProcessor&& other) = delete;

    void run(std::istream& in, std::ostream& out);

private:
    Dictionary& dict_;

    void handleAdd(std::istream& in, std::ostream& out);
    void handleFind(std::istream& in, std::ostream& out);
    void handleDel(std::istream& in, std::ostream& out);
    void handleHelp(std::ostream& out);
};

#endif
