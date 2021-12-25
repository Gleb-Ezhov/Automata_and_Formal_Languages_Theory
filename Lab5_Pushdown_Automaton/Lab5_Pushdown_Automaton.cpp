#include "auxiliary_algorithms.h"
#include "StringToGrammarParser.h"
#include "GrammarTypeQualifier.h"
#include "PushdownAutomaton.h"

int main()
{
    setlocale(LC_ALL, "");

    // содержимое файла в виде строки
    std::string str = readFileIntoString("D:\\Development\\VisualStudio\\PSUTI_Automata_and_Formal_Languages_Theory\\Lab5_Pushdown_Automaton\\grammar2.txt");

    Grammar* grammar = new Grammar();
    StringToGrammarParser parser(str, *grammar);  // только что созданная грамматика заполняется парсером строки
    GrammarTypeQualifier grammarTypeQualifier(*grammar);

    std::cout << str << std::endl << std::endl;  // изначальное содержимое файла
    std::cout << grammarTypeQualifier.getGrammarTypeDescription() << std::endl;  // вывод типа грамматики

    if (grammar->type() == 2)
    {
        PushdownAutomaton casualAutomaton(*grammar, false);
        PushdownAutomaton extendedAutomaton(*grammar, true);
        std::cout << std::endl;
        extendedAutomaton.InputStringRecognition("dobd=k");
    }
    else
    {
        std::cout << std::endl << "Для построения автомата с магазинной памятью на вход нужно подать контекстно-свободную грамматику." << std::endl;
    }

    return EXIT_SUCCESS;
}