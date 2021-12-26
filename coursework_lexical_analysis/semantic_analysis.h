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

	std::vector<CodePair> expressionStack; // ���� ��� ������� ���������

	// ������������� ��������� �������� ��������������� ����������
	void descriptionSemanticProcessing(std::vector<int> identifiers, int type);

	// ��������, ��� �� �������������� ���� �������
	void identifiersSemanticCheck();

	// ������� ����� � �������� ���
	void allNumbersToMachineRepresentation();

	// ������ ��������� �� ������������� ����� ��������� � ��������.
	// ������� ���������� ������ � �������������� ����� ���������.
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