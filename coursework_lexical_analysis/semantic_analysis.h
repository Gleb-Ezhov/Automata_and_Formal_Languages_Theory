#pragma once
#include "program_analysis.h"

class SemanticAnalysis
{
public:
	SemanticAnalysis(ProgramAnalysis* analysis);

	std::vector<std::string> expressionStack; // стэк для анализа выражения

	// Семантическая обработка описаний идентификаторов переменных
	void descriptionSemanticProcessing(std::vector<int> identifiers, int type);

	// Проверка, все ли идентификаторы были описаны
	void identifiersSemanticCheck();

	// Перевод чисел в машинный код
	void allNumbersToMachineRepresentation();

	// Анализ выражения на совместимость типов операндов и операций
	void expressionAnalysis();

private:
	ProgramAnalysis* analysis;

	void decimalStrToBinary(int index, std::string number);
	void realStrToBinary(int index, std::string number);
	void hexStrToBinary(int index, std::string number);
	void binStrToBinary(int index, std::string number);
	void octalStrToBinary(int index, std::string number);
};