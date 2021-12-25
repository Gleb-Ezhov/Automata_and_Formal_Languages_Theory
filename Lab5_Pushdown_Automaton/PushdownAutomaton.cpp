#include <iostream>
#include "PushdownAutomaton.h"

PushdownAutomaton::PushdownAutomaton(Grammar& grammar, bool isExtended) : grammar(grammar), isExtended(isExtended)
{
	if (isExtended)
	{
		ConstructExtendedPushdownAutomaton();
	}
	else
	{
		ConstructCasualPushdownAutomaton();
	}
}

void PushdownAutomaton::ConstructCasualPushdownAutomaton()
{
	// ШАГ 1-ый алгоритма
	Q.push_back('q'); // только одно состояние в данном автомате

	// множество символов магазина
	N.insert(N.end(), grammar.terminals_v().begin(), grammar.terminals_v().end());
	N.insert(N.end(), grammar.nonTerminals_v().begin(), grammar.nonTerminals_v().end());

	// множество символов входной строки
	T.insert(T.end(), grammar.terminals_v().begin(), grammar.terminals_v().end());

	N0 = grammar.getAxioma(); // начальный символ магазина

	// ШАГ 2-ой алгоритма
	StackFunction function;

	// части функции которые не меняются в процессе записи
	function.currentState = 'q';
	function.currentTerminal = 'e';
	function.nextState = 'q';

	for (std::string rule : grammar.getRules())
	{
		function.currentStackTop = rule[0];

		for (size_t i = rule.find(">", 0), delimiter = i; i <= rule.length(); ++i)
		{
			if (rule[i] == '|' || i == rule.length())
			{
				// с каждой новой правой частью правила во множество магазинных функций добавляется новый экземпляр
				function.stackTopReplacement = rule.substr(delimiter + 1, i - (delimiter + 1));
				F.push_back(function);
				delimiter = i;
			}
			function.stackTopReplacement;
		}
	}

	// ШАГ 3-ий алгоритма
	function.stackTopReplacement = 'e';

	for (char terminal : T)
	{
		function.currentTerminal = terminal;
		function.currentStackTop = terminal;
		F.push_back(function);
	}
}

void PushdownAutomaton::ConstructExtendedPushdownAutomaton()
{
	// ШАГ 1-ый алгоритма
	Q.push_back('q');
	Q.push_back('r');

	Z.push_back('r');

	// множество символов магазина
	N.insert(N.end(), grammar.terminals_v().begin(), grammar.terminals_v().end());
	N.insert(N.end(), grammar.nonTerminals_v().begin(), grammar.nonTerminals_v().end());
	N.push_back('#');

	// множество символов входной строки
	T.insert(T.end(), grammar.terminals_v().begin(), grammar.terminals_v().end());

	N0 = '#'; // начальный символ магазина

	// ШАГ 2-ой алгоритма
	StackFunction function;

	// части функции которые не меняются в процессе записи
	function.currentState = 'q';
	function.currentTerminal = 'e';
	function.nextState = 'q';

	for (std::string rule : grammar.getRules())
	{
		function.stackTopReplacement = rule[0];

		for (size_t i = rule.find(">", 0), delimiter = i; i <= rule.length(); ++i)
		{
			if (rule[i] == '|' || i == rule.length())
			{
				// с каждой новой правой частью правила во множество магазинных функций добавляется новый экземпляр
				function.currentStackTop = rule.substr(delimiter + 1, i - (delimiter + 1));
				F.push_back(function);
				delimiter = i;
			}
			function.stackTopReplacement;
		}
	}

	// ШАГ 3-ий алгоритма
	function.currentStackTop = 'e';

	for (char terminal : T)
	{
		function.currentTerminal = terminal;
		function.stackTopReplacement = terminal;
		F.push_back(function);
	}

	// ШАГ 4-ый алгоритма
	function.currentTerminal = 'e';
	function.currentStackTop = std::string({'#', grammar.getAxioma()});
	function.nextState = 'r';
	function.stackTopReplacement = 'e';
	F.push_back(function);
}

void PushdownAutomaton::InputStringRecognition(std::string input)
{
	int i, differenceBetweenStackAndFunc, num = 2;
	std::string stackStr;
	bool firstFunctionWorked = false;

	std::cout << "№ конфигурации" << '\t' << "Состояние" << '\t' << "Входная строка" << '\t' << "Содержимое магазина" << std::endl;
	std::cout << "1" << "\t\t" << q0 << "\t\t" << input << "\t\t" << N0 << std::endl;

	std::vector<char> stack;
	stack.push_back(N0);

	if (isExtended)
	{
		// Алгоритм разбора строки расширенным МП-автоматом. Разбор всегда детерминирован.
		while (input.size() != 0)
		{
			for (StackFunction func : F)
			{
				for (char c : stack) stackStr.push_back(c);

				// условие не будет работать для длинных стэков со множеством повторяющихся элементов (пока как заглушка для разбора небольшой строки)
				differenceBetweenStackAndFunc = stackStr.size() - func.currentStackTop.size();
				if (stackStr.find(func.currentStackTop) == differenceBetweenStackAndFunc && differenceBetweenStackAndFunc != -1)
				{
					for (i = 0; i < func.currentStackTop.size(); ++i) stack.pop_back();
					stack.push_back(func.stackTopReplacement[0]);

					
					stackStr.clear();
					for (char c : stack) stackStr.push_back(c);
					std::cout << num << "\t\t" << q0 << "\t\t" << input << "\t\t" << stackStr << std::endl;
					stackStr.clear();

					firstFunctionWorked = true;
					num++;
					break;
				}
				stackStr.clear();
			}

			if (firstFunctionWorked == false && input.size() > 0)
			{
				for (StackFunction func : F)
				{

					if (func.currentTerminal == input.front())
					{
						stack.push_back(func.stackTopReplacement[0]);
						input.erase(0, 1);

						for (char c : stack) stackStr.push_back(c);
						std::cout << num << "\t\t" << q0 << "\t\t" << input << "\t\t" << stackStr << std::endl;
						stackStr.clear();
						break;
					}
				}
			}
			
			firstFunctionWorked = false;
		}
		std::cout << num << "\t\t" << 'r' << "\t\t" << 'e' << "\t\t" << 'e' << std::endl;
	}
	else
	{
		// Разбор строки обычным МП-автоматом.
		// Отслеживания предыдущих действий нету. Алгоритм недетерминированный.
		while (num < 10)
		{
			for (StackFunction func : F)
			{
				if (func.currentStackTop[0] == stack.front())
				{
					stack.pop_back();

					if (func.currentTerminal == input.front() && input.front() == stack.front())
					{
						input.erase(0, 1);
					}
					else
					{
						for (i = 0; i < func.stackTopReplacement.size(); ++i)
						{
							stack.insert(stack.begin() + i, func.stackTopReplacement.at(i));
						}
					}

					for (char c : stack) stackStr.push_back(c);
					std::cout << num << "\t\t" << q0 << "\t\t" << input << "\t\t" << stackStr << std::endl;
					stackStr.clear();
					num++;
					break;
				}
			}
		}
	}
}