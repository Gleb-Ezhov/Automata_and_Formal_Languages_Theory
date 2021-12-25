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

    // ����� ���������� �� ����
    bool itCanBeType0 = false;  // ���� �� ��������� ��� 0
    bool itCanBeType1 = false;  // ����������-��������� ���������
    bool itCanBeType2 = false;  // ����������-��������� ����������
    bool itCanBeType3 = false;  // ���������� ����������
    bool canBeLeftRegular = false;  // ���������� �������������� ����������
    bool canBeRightRegular = false; // ���������� ������������

    bool isNonTerminal(char& character);

    bool isTerminal(char& character);

    // ��������� ����� ���������� �� ����
    void doGuesses();

    // �������� ���������� �� ��������� ��������
    void qualifyTheType();
};