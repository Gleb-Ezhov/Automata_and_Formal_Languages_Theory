#pragma once
#include "program_analysis.h"

class SemanticAnalysis
{
public:
	SemanticAnalysis(ProgramAnalysis* analysis);

	std::vector<std::string> expressionStack; // ���� ��� ������� ���������

	// ������������� ��������� �������� ��������������� ����������
	void descriptionSemanticProcessing(std::vector<int> identifiers, int type);

	// ��������, ��� �� �������������� ���� �������
	void identifiersSemanticCheck();

	// ������� ����� � �������� ���
	void allNumbersToMachineRepresentation();

	// ������ ��������� �� ������������� ����� ��������� � ��������
	void expressionAnalysis();

private:
	ProgramAnalysis* analysis;

	void decimalStrToBinary(int index, std::string number);
	void realStrToBinary(int index, std::string number);
	void hexStrToBinary(int index, std::string number);
	void binStrToBinary(int index, std::string number);
	void octalStrToBinary(int index, std::string number);
};