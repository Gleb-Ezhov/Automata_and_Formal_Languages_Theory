/*
    Author: Gleb Ezhov
*/

#include <iostream>
#include <string>
#include <fstream>
#include <vector>

// Грамматика
class Grammar
{
    friend class StringToGrammarParser;
    friend class GrammarTypeQualifier;

private:
    std::vector<char> terminals;
    std::vector<char> nonTerminals;
    std::vector<std::string> rules;
    char axioma;
};

// Парсер строки в грамматику. Заполняет поля переданного пустого объекта типа Grammar.
class StringToGrammarParser
{
public:
    StringToGrammarParser(std::string& stringToParse, Grammar& grammar) : grammar(grammar)
    {
        size_t terminalsStartPosition = terminalsParsing(stringToParse);
        size_t nonTerminalsStartPosition = nonTerminalsParsing(stringToParse, terminalsStartPosition);
        size_t rulesStartPosition = rulesParsing(stringToParse, nonTerminalsStartPosition);
        axiomaParsing(stringToParse, rulesStartPosition);
    }

private:
    Grammar& grammar;

    // Парсинг терминалов в вектор
    size_t terminalsParsing(std::string stringToParse)
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

    // Парсинг нетерминалов в вектор
    size_t nonTerminalsParsing(std::string stringToParse, size_t terminalsStartPosition)
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

    // Парсинг правил
    size_t rulesParsing(std::string stringToParse, size_t nonTerminalsStartPosition)
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

        return rulesStartPosition;
    }

    // Нахождение нетерминала аксиомы
    void axiomaParsing(std::string stringToParse, size_t rulesStartPosition)
    {
        size_t axiomaPosition = stringToParse.find("=", rulesStartPosition + 1);
        grammar.axioma = stringToParse.substr(axiomaPosition + 1, stringToParse.length()).at(0);

        std::cout << "Найдена аксиома: " << grammar.axioma << std::endl;
        std::cout << std::endl;
    }
};

class GrammarTypeQualifier
{
public:
    GrammarTypeQualifier(Grammar& grammar) : grammar(grammar)
    {
        doGuesses();
        qualifyTheType();
    }

    std::string getGrammarTypeDescription()
    {
        return grammarTypeDescription;
    }

private:
    Grammar& grammar;
    std::string grammarTypeDescription;

    // Флаги подозрений на типы
    bool itCanBeType0 = false;  // флаг на возможный тип 0
    bool itCanBeType1 = false;  // контекстно-зависимая грамматка
    bool itCanBeType2 = false;  // контекстно-свободная грамматика
    bool itCanBeType3 = false;  // регулярная грамматика
    bool canBeLeftRegular = false;  // регулярная правосторонняя грамматика
    bool canBeRightRegular = false; // ругулярная левосторнняя

    bool isNonTerminal(char& character)
    {
        for (char element : grammar.nonTerminals)
        {
            if (element == character) return true;
        }
        return false;
    }

    bool isTerminal(char& character)
    {
        for (char element : grammar.terminals)
        {
            if (element == character) return true;
        }
        return false;
    }

    // Заполняет флаги подозрений на типы
    void doGuesses()
    {
        for (auto rule : grammar.rules)
        {
            // Если вначале правила только один нетерминал, то начинаем проверку на типы 2 и 3
            if (isNonTerminal(rule[0]) && rule[1] == '>')
            {
                int terminalsCounter = 0;
                int nonTerminalsCounter = 0;

                // В цикле проходимся по всем правилам правой части. Делаем догадки на основе кол-ва терминалов/нетерминалов в правиле.
                for (int i = 2; i <= rule.length(); ++i)
                {
                    // Если встретили разделитель, начинаем анализировать правило
                    if (rule[i] == '|' || i == rule.length())
                    {
                        // если в правиле был 1 терминал и 1 нетерминал, то начинается проверка на регулярную грамматику
                        if (terminalsCounter + nonTerminalsCounter == 2)
                        {
                            // Устанавливаем догадку на (лево)праворегулярную грамматику по первому символу правой части правила
                            if (isTerminal(rule[i - 2])) canBeRightRegular = true;
                            else canBeLeftRegular = true;

                            itCanBeType3 = true;

                            /*if ((isTerminal(rule[i - 2]) && canBeRightRegular) || (isNonTerminal(rule[i - 2]) && canBeLeftRegular))
                            {
                                itCanBeType3 = true;
                            }*/
                        }
                        else if (!(terminalsCounter == 1 && nonTerminalsCounter == 0)) itCanBeType2 = true;
                        
                        terminalsCounter = 0;
                        nonTerminalsCounter = 0;
                    }
                    else
                    {
                        // Пока правило не закончилось, инкрементируем счётчики соответствующих символов
                        if (isTerminal(rule[i])) ++terminalsCounter;
                        else ++nonTerminalsCounter;
                    }
                }
            }
            else
            {
                itCanBeType1 = true;
            }
        }
    }

    // Описание грамматики по имеющимся догадкам
    void qualifyTheType()
    {
        if (itCanBeType1 && itCanBeType2 && itCanBeType3)
        {
            grammarTypeDescription = "Тип грамматики 0. На правила не наложено никаких \"внешних\" ограничений.";
        }
        else if (itCanBeType1)
        {
            grammarTypeDescription = "Тип грамматики 1 (Контекстно-зависимая). Вывод сентенциальной формы зависит от контекста.";
        }
        else if (itCanBeType2 || (canBeRightRegular && canBeLeftRegular))
        { // если есть догадки на леворегулярную и праворегулярную грамматику одновременно, то отрицаем трейтий тип
            grammarTypeDescription = "Тип грамматики 2 (Контекстно-свободная). Вывод из нетерминала любой сентенциальной формы.";
        }
        else if (itCanBeType3)
        {
            if (canBeRightRegular)
                grammarTypeDescription = "Тип грамматики 3 (Праворегулярная/автоматная/линейная). Все правила имеют вид A -> aB | a.";
            else
                grammarTypeDescription = "Тип грамматики 3 (Леворегулярная/автоматная/линейная). Все правила имеют вид A -> Ba | a.";
        }
        else
        {
            grammarTypeDescription = "Не удалось определить тип грамматики.";
        }
    }
};

std::string readFileIntoString(const std::string& path) {
    std::ifstream inputFile(path);
    inputFile.seekg(3, std::ios::beg); // пропуск служебных символов

    if (!inputFile.is_open()) {
        std::cerr << "Could not open the file - '" << path << "'" << std::endl;
        exit(EXIT_FAILURE);
    }
    return std::string((std::istreambuf_iterator<char>(inputFile)), std::istreambuf_iterator<char>());
}

int main()
{
    setlocale(LC_ALL, "");

    // содержимое файла в виде строки
    std::string str = readFileIntoString("D:\\Development\\VisualStudio\\PSUTI_Automata_and_Formal_Languages_Theory\\Lab2_Grammar_type_definition\\grammar.txt");

    Grammar* grammar = new Grammar();
    StringToGrammarParser parser(str, *grammar);  // только что созданная грамматика заполняется парсером строки
    GrammarTypeQualifier grammarTypeQualifier(*grammar);

    std::cout << str << std::endl << std::endl;  // изначальное содержимое файла
    std::cout << grammarTypeQualifier.getGrammarTypeDescription() << std::endl;  // вывод типа грамматики

    return EXIT_SUCCESS;
}