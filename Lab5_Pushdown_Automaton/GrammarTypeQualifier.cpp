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

void GrammarTypeQualifier::qualifyTheType()
{
    if (itCanBeType1 && itCanBeType2 && itCanBeType3)
    {
        grammarTypeDescription = "Тип грамматики 0. На правила не наложено никаких \"внешних\" ограничений.";
        grammar.setGrammarType(0);
    }
    else if (itCanBeType1)
    {
        grammarTypeDescription = "Тип грамматики 1 (Контекстно-зависимая). Вывод сентенциальной формы зависит от контекста.";
        grammar.setGrammarType(1);
    }
    else if (itCanBeType2 || (canBeRightRegular && canBeLeftRegular))
    { // если есть догадки на леворегулярную и праворегулярную грамматику одновременно, то отрицаем трейтий тип
        grammarTypeDescription = "Тип грамматики 2 (Контекстно-свободная). Вывод из нетерминала любой сентенциальной формы.";
        grammar.setGrammarType(2);
    }
    else if (itCanBeType3)
    {
        if (canBeRightRegular)
        {
            grammarTypeDescription = "Тип грамматики 3 (Праворегулярная/автоматная/линейная). Все правила имеют вид A -> aB | a.";
            grammar.setGrammarType(3);
        }
        else
        {
            grammarTypeDescription = "Тип грамматики 3 (Леворегулярная/автоматная/линейная). Все правила имеют вид A -> Ba | a.";
            grammar.setGrammarType(3);
        }
    }
    else
    {
        grammarTypeDescription = "Не удалось определить тип грамматики.";
        grammar.setGrammarType(-1);
    }
}