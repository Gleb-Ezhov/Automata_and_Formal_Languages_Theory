#include "GrammarTypeQualifier.h"

GrammarTypeQualifier::GrammarTypeQualifier(Grammar& grammar) : grammar(grammar)
{
	doGuesses();
	qualifyTheType();
}

bool GrammarTypeQualifier::isNonTerminal(char& character)
{
    for (char element : grammar.nonTerminals)
    {
        if (element == character) return true;
    }
    return false;
}

bool GrammarTypeQualifier::isTerminal(char& character)
{
    for (char element : grammar.terminals)
    {
        if (element == character) return true;
    }
    return false;
}

void GrammarTypeQualifier::doGuesses()
{
    for (auto rule : grammar.rules)
    {
        // ���� ������� ������� ������ ���� ����������, �� �������� �������� �� ���� 2 � 3
        if (isNonTerminal(rule[0]) && rule[1] == '>')
        {
            int terminalsCounter = 0;
            int nonTerminalsCounter = 0;

            // � ����� ���������� �� ���� �������� ������ �����. ������ ������� �� ������ ���-�� ����������/������������ � �������.
            for (int i = 2; i <= rule.length(); ++i)
            {
                // ���� ��������� �����������, �������� ������������� �������
                if (rule[i] == '|' || i == rule.length())
                {
                    // ���� � ������� ��� 1 �������� � 1 ����������, �� ���������� �������� �� ���������� ����������
                    if (terminalsCounter + nonTerminalsCounter == 2)
                    {
                        // ������������� ������� �� (����)��������������� ���������� �� ������� ������� ������ ����� �������
                        if (isTerminal(rule[i - 2])) canBeRightRegular = true;
                        else canBeLeftRegular = true;

                        itCanBeType3 = true;
                    }
                    else if (!(terminalsCounter == 1 && nonTerminalsCounter == 0)) itCanBeType2 = true;

                    terminalsCounter = 0;
                    nonTerminalsCounter = 0;
                }
                else
                {
                    // ���� ������� �� �����������, �������������� �������� ��������������� ��������
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

void GrammarTypeQualifier::qualifyTheType()
{
    if (itCanBeType1 && itCanBeType2 && itCanBeType3)
    {
        grammarTypeDescription = "��� ���������� 0. �� ������� �� �������� ������� \"�������\" �����������.";
        grammar.setGrammarType(0);
    }
    else if (itCanBeType1)
    {
        grammarTypeDescription = "��� ���������� 1 (����������-���������). ����� �������������� ����� ������� �� ���������.";
        grammar.setGrammarType(1);
    }
    else if (itCanBeType2 || (canBeRightRegular && canBeLeftRegular))
    { // ���� ���� ������� �� �������������� � ��������������� ���������� ������������, �� �������� ������� ���
        grammarTypeDescription = "��� ���������� 2 (����������-���������). ����� �� ����������� ����� �������������� �����.";
        grammar.setGrammarType(2);
    }
    else if (itCanBeType3)
    {
        if (canBeRightRegular)
        {
            grammarTypeDescription = "��� ���������� 3 (���������������/����������/��������). ��� ������� ����� ��� A -> aB | a.";
            grammar.setGrammarType(3);
        }
        else
        {
            grammarTypeDescription = "��� ���������� 3 (��������������/����������/��������). ��� ������� ����� ��� A -> Ba | a.";
            grammar.setGrammarType(3);
        }
    }
    else
    {
        grammarTypeDescription = "�� ������� ���������� ��� ����������.";
        grammar.setGrammarType(-1);
    }
}