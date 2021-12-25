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
	// ��� 1-�� ���������
	Q.push_back('q'); // ������ ���� ��������� � ������ ��������

	// ��������� �������� ��������
	N.insert(N.end(), grammar.terminals_v().begin(), grammar.terminals_v().end());
	N.insert(N.end(), grammar.nonTerminals_v().begin(), grammar.nonTerminals_v().end());

	// ��������� �������� ������� ������
	T.insert(T.end(), grammar.terminals_v().begin(), grammar.terminals_v().end());

	N0 = grammar.getAxioma(); // ��������� ������ ��������

	// ��� 2-�� ���������
	StackFunction function;

	// ����� ������� ������� �� �������� � �������� ������
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
				// � ������ ����� ������ ������ ������� �� ��������� ���������� ������� ����������� ����� ���������
				function.stackTopReplacement = rule.substr(delimiter + 1, i - (delimiter + 1));
				F.push_back(function);
				delimiter = i;
			}
			function.stackTopReplacement;
		}
	}

	// ��� 3-�� ���������
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
	// ��� 1-�� ���������
	Q.push_back('q');
	Q.push_back('r');

	Z.push_back('r');

	// ��������� �������� ��������
	N.insert(N.end(), grammar.terminals_v().begin(), grammar.terminals_v().end());
	N.insert(N.end(), grammar.nonTerminals_v().begin(), grammar.nonTerminals_v().end());
	N.push_back('#');

	// ��������� �������� ������� ������
	T.insert(T.end(), grammar.terminals_v().begin(), grammar.terminals_v().end());

	N0 = '#'; // ��������� ������ ��������

	// ��� 2-�� ���������
	StackFunction function;

	// ����� ������� ������� �� �������� � �������� ������
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
				// � ������ ����� ������ ������ ������� �� ��������� ���������� ������� ����������� ����� ���������
				function.currentStackTop = rule.substr(delimiter + 1, i - (delimiter + 1));
				F.push_back(function);
				delimiter = i;
			}
			function.stackTopReplacement;
		}
	}

	// ��� 3-�� ���������
	function.currentStackTop = 'e';

	for (char terminal : T)
	{
		function.currentTerminal = terminal;
		function.stackTopReplacement = terminal;
		F.push_back(function);
	}

	// ��� 4-�� ���������
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

	std::cout << "� ������������" << '\t' << "���������" << '\t' << "������� ������" << '\t' << "���������� ��������" << std::endl;
	std::cout << "1" << "\t\t" << q0 << "\t\t" << input << "\t\t" << N0 << std::endl;

	std::vector<char> stack;
	stack.push_back(N0);

	if (isExtended)
	{
		// �������� ������� ������ ����������� ��-���������. ������ ������ ��������������.
		while (input.size() != 0)
		{
			for (StackFunction func : F)
			{
				for (char c : stack) stackStr.push_back(c);

				// ������� �� ����� �������� ��� ������� ������ �� ���������� ������������� ��������� (���� ��� �������� ��� ������� ��������� ������)
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
		// ������ ������ ������� ��-���������.
		// ������������ ���������� �������� ����. �������� �������������������.
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