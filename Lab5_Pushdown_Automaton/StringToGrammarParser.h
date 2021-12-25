#pragma once
#include "Grammar.h"
#include <iostream>

// ������ ������ � ����������. ��������� ���� ����������� ������� ������� ���� Grammar.
class StringToGrammarParser
{
public:
    StringToGrammarParser(std::string& stringToParse, Grammar& grammar);

private:
    Grammar& grammar;

    // ������� ���������� � ������
    size_t terminalsParsing(std::string stringToParse);

    // ������� ������������ � ������
    size_t nonTerminalsParsing(std::string stringToParse, size_t terminalsStartPosition);

    // ������� ������
    size_t rulesParsing(std::string stringToParse, size_t nonTerminalsStartPosition);

    // ���������� ����������� �������
    void axiomaParsing(std::string stringToParse, size_t rulesStartPosition);
};
