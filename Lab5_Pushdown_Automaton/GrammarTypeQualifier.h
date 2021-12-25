#pragma once
#include "Grammar.h"

class GrammarTypeQualifier
{
public:
    GrammarTypeQualifier(Grammar& grammar);

    std::string getGrammarTypeDescription()
    {
        return grammarTypeDescription;
    }

private:
    Grammar& grammar;
    std::string grammarTypeDescription;

    // ‘лаги подозрений на типы
    bool itCanBeType0 = false;  // флаг на возможный тип 0
    bool itCanBeType1 = false;  // контекстно-зависима€ грамматка
    bool itCanBeType2 = false;  // контекстно-свободна€ грамматика
    bool itCanBeType3 = false;  // регул€рна€ грамматика
    bool canBeLeftRegular = false;  // регул€рна€ правосторонн€€ грамматика
    bool canBeRightRegular = false; // ругул€рна€ левосторнн€€

    bool isNonTerminal(char& character);

    bool isTerminal(char& character);

    // «аполн€ет флаги подозрений на типы
    void doGuesses();

    // ќписание грамматики по имеющимс€ догадкам
    void qualifyTheType();
};