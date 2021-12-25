#pragma once
#include <vector>
#include <string>

// Грамматика
class Grammar
{
    friend class StringToGrammarParser;
    friend class GrammarTypeQualifier;

public:
    void setGrammarType(int type) { this->grammarType = type; };

    int type() { return this->grammarType; };

    std::vector<char>& terminals_v() { return this->terminals; }

    std::vector<char>& nonTerminals_v() { return this->nonTerminals; }

    char getAxioma() { return this->axioma; }

    std::vector<std::string>& getRules() { return rules; }

private:
    std::vector<char> terminals;
    std::vector<char> nonTerminals;
    std::vector<std::string> rules;
    char axioma;
    int grammarType;
};