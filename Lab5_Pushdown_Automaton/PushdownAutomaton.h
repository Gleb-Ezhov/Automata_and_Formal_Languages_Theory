#pragma once
#include "Grammar.h";

// Магазинная функция
struct StackFunction
{
	// текущая конфигурация МП-автомата
	char currentState;
	char currentTerminal;
	std::string currentStackTop;

	// конфигурация, в которую нужно перевести МП-автомат
	char nextState;  // в это состояние нужно перевести
	std::string stackTopReplacement;  // этим нужно заменить верхушку стека
};

// Класс автомата с магазинной памятью
class PushdownAutomaton
{
public:
	// Флаг isExtended определяет будет ли составлен расширенный МП-автомат или обычный
	PushdownAutomaton(Grammar& grammar, bool isExtended);

	// Метод для распознавания входной строки грамматики. Процесс распознавания выводится в терминал.
	void InputStringRecognition(std::string input);

private:
	// Метод, конструирующий обычный МП-автомат (может быть недетерминированным с некоторыми грамматиками)
	void ConstructCasualPushdownAutomaton();

	// Метод, конструирующий расширенный МП-автомат (всегда детерминирован)
	void ConstructExtendedPushdownAutomaton();

	Grammar grammar;
	bool isExtended;

	std::vector<char> Q; // множество состояний автомата
	std::vector<char> T; // множество символов входной строки (терминалы)
	std::vector<char> N; // множество символов магазина (терминалы + нетерминалы)
	std::vector<StackFunction> F; // множество магазинных функций
	std::vector<char> Z; // множество конечных состояний

	char q0 = 'q'; // начальное состояние автомата
	char N0; // начальный символ магазина
};