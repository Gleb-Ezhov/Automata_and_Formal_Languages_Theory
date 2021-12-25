#pragma once
#include "Grammar.h";

// ���������� �������
struct StackFunction
{
	// ������� ������������ ��-��������
	char currentState;
	char currentTerminal;
	std::string currentStackTop;

	// ������������, � ������� ����� ��������� ��-�������
	char nextState;  // � ��� ��������� ����� ���������
	std::string stackTopReplacement;  // ���� ����� �������� �������� �����
};

// ����� �������� � ���������� �������
class PushdownAutomaton
{
public:
	// ���� isExtended ���������� ����� �� ��������� ����������� ��-������� ��� �������
	PushdownAutomaton(Grammar& grammar, bool isExtended);

	// ����� ��� ������������� ������� ������ ����������. ������� ������������� ��������� � ��������.
	void InputStringRecognition(std::string input);

private:
	// �����, �������������� ������� ��-������� (����� ���� ������������������� � ���������� ������������)
	void ConstructCasualPushdownAutomaton();

	// �����, �������������� ����������� ��-������� (������ ��������������)
	void ConstructExtendedPushdownAutomaton();

	Grammar grammar;
	bool isExtended;

	std::vector<char> Q; // ��������� ��������� ��������
	std::vector<char> T; // ��������� �������� ������� ������ (���������)
	std::vector<char> N; // ��������� �������� �������� (��������� + �����������)
	std::vector<StackFunction> F; // ��������� ���������� �������
	std::vector<char> Z; // ��������� �������� ���������

	char q0 = 'q'; // ��������� ��������� ��������
	char N0; // ��������� ������ ��������
};