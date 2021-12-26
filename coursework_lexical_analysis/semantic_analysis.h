#pragma once
#include "program_analysis.h"
#include "lexical_analysis.h"

//class OperationTypesChecker
//{
//public:
//	OperationTypesChecker();
//};

class SemanticAnalysis
{
public:
	SemanticAnalysis(ProgramAnalysis* analysis);

	std::vector<CodePair> expressionStack; // стэк для анализа выражения

	// Семантическая обработка описаний идентификаторов переменных
	void descriptionSemanticProcessing(std::vector<int> identifiers, int type);

	// Проверка, все ли идентификаторы были описаны
	void identifiersSemanticCheck();

	// Перевод чисел в машинный код
	void allNumbersToMachineRepresentation();

	// Анализ выражения на совместимость типов операндов и операций.
	// Функция возвращает строку с результирующим типом выражения.
	std::string expressionAnalysis();

private:
	ProgramAnalysis* analysis;

	void operationTypeValidation1(int& index, std::vector<std::string>& strStack);
	void operationTypeValidation2(int& index, std::vector<std::string>& strStack);
	void operationTypeValidation3(int& index, std::vector<std::string>& strStack);

	void decimalStrToBinary(int index, std::string number);
	void realStrToBinary(int index, std::string number);
	void hexStrToBinary(int index, std::string number);
	void binStrToBinary(int index, std::string number);
	void octalStrToBinary(int index, std::string number);
};