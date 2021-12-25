#include "StringToGrammarParser.h"

StringToGrammarParser::StringToGrammarParser(std::string& stringToParse, Grammar& grammar) : grammar(grammar)
{
    size_t terminalsStartPosition = terminalsParsing(stringToParse);
    size_t nonTerminalsStartPosition = nonTerminalsParsing(stringToParse, terminalsStartPosition);
    size_t rulesStartPosition = rulesParsing(stringToParse, nonTerminalsStartPosition);
    axiomaParsing(stringToParse, rulesStartPosition);
}

size_t StringToGrammarParser::terminalsParsing(std::string stringToParse)
{
    size_t terminalsStartPosition = stringToParse.find("{");
    size_t terminalsEndPosition = stringToParse.find("}");

    // Выделение подстроки множества терминалов
    std::string terminalsSubstring = stringToParse.substr(terminalsStartPosition + 1, terminalsEndPosition - terminalsStartPosition - 1);

    std::cout << "Выделено множество терминалов: " << terminalsSubstring << std::endl;

    // Удаление из подстроки разделителей в виде запятых с помощью итераторов и метода erase()
    std::string::iterator delimiterPos = std::find(terminalsSubstring.begin(), terminalsSubstring.end(), ',');
    while (delimiterPos != std::end(terminalsSubstring))
    {
        terminalsSubstring.erase(delimiterPos);
        delimiterPos = std::find(delimiterPos, terminalsSubstring.end(), ',');
    }

    // Заполнение вектора терминалов символами, которые остались в итоге в подстроке
    for (int i = 0; i < terminalsSubstring.size(); ++i)
    {
        grammar.terminals.push_back(terminalsSubstring.at(i));
    }

    for (char el : grammar.terminals)
    {
        std::cout << el << "\t";
    }
    std::cout << std::endl;

    return terminalsStartPosition;
}

size_t StringToGrammarParser::nonTerminalsParsing(std::string stringToParse, size_t terminalsStartPosition)
{
    size_t nonTerminalsStartPosition = stringToParse.find("{", terminalsStartPosition + 1);
    size_t nonTerminalsEndPosition = stringToParse.find("}", nonTerminalsStartPosition);

    // Выделение подстроки множества терминалов
    std::string nonTerminalsSubstring = stringToParse.substr(nonTerminalsStartPosition + 1, nonTerminalsEndPosition - nonTerminalsStartPosition - 1);

    std::cout << "Выделено множество нетерминалов: " << nonTerminalsSubstring << std::endl;

    // Удаление из подстроки разделителей в виде запятых с помощью итераторов и метода erase()
    std::string::iterator delimiterPos = std::find(nonTerminalsSubstring.begin(), nonTerminalsSubstring.end(), ',');
    while (delimiterPos != std::end(nonTerminalsSubstring))
    {
        nonTerminalsSubstring.erase(delimiterPos);
        delimiterPos = std::find(delimiterPos, nonTerminalsSubstring.end(), ',');
    }

    // Заполнение вектора нетерминалов символами, которые остались в итоге в подстроке
    for (int i = 0; i < nonTerminalsSubstring.size(); ++i)
    {
        grammar.nonTerminals.push_back(nonTerminalsSubstring.at(i));
    }

    for (char el : grammar.nonTerminals)
    {
        std::cout << el << "\t";
    }
    std::cout << std::endl;

    return nonTerminalsStartPosition;
}

size_t StringToGrammarParser::rulesParsing(std::string stringToParse, size_t nonTerminalsStartPosition)
{
    size_t rulesStartPosition = stringToParse.find("{", nonTerminalsStartPosition + 1);
    size_t rulesEndPosition = stringToParse.find("}", rulesStartPosition);

    // Выделение подстроки множества правил
    std::string rulesSubstring = stringToParse.substr(rulesStartPosition + 1, rulesEndPosition - rulesStartPosition - 1);
    std::cout << "Выделено множество правил: " << rulesSubstring << std::endl;

    // Выделение отдельных правил и их добавление в вектор
    std::string rule("");
    for (int i = 0; i < rulesSubstring.size(); ++i)
    {
        if (rulesSubstring.at(i) == ',')
        {
            grammar.rules.push_back(rule);
            rule.erase(0, rule.size());
            continue;
        }
        else
        {
            rule.append(1, rulesSubstring.at(i));
        }
    }
    grammar.rules.push_back(rule);

    for (std::string el : grammar.rules)
    {
        std::cout << el << "\t";
    }
    std::cout << std::endl;

    return rulesEndPosition;
}

void StringToGrammarParser::axiomaParsing(std::string stringToParse, size_t rulesStartPosition)
{
    size_t axiomaPosition = stringToParse.find("=", rulesStartPosition + 1);
    grammar.axioma = stringToParse.substr(axiomaPosition + 1, stringToParse.length()).at(0);

    std::cout << "Найдена аксиома: " << grammar.axioma << std::endl;
    std::cout << std::endl;
}