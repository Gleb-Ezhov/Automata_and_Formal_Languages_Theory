#pragma once
#include "Grammar.h"
#include <iostream>

// Парсер строки в грамматику. Заполняет поля переданного пустого объекта типа Grammar.
class StringToGrammarParser
{
public:
    StringToGrammarParser(std::string& stringToParse, Grammar& grammar);

private:
    Grammar& grammar;

    // Парсинг терминалов в вектор
    size_t terminalsParsing(std::string stringToParse);

    // Парсинг нетерминалов в вектор
    size_t nonTerminalsParsing(std::string stringToParse, size_t terminalsStartPosition);

    // Парсинг правил
    size_t rulesParsing(std::string stringToParse, size_t nonTerminalsStartPosition);

    // Нахождение нетерминала аксиомы
    void axiomaParsing(std::string stringToParse, size_t rulesStartPosition);
};
