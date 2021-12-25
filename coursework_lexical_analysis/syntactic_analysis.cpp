#include "syntactic_analysis.h"

SyntacticAnalysis::SyntacticAnalysis(ProgramAnalysis* analysis, SemanticAnalysis* semantic, std::vector<CodePair> lexemes)
{
	this->analysis = analysis;
	this->semantic = semantic;
	this->lexemes = lexemes;
	syntacticAnalysis();
}

// �������� ������� "���������"
void SyntacticAnalysis::syntacticAnalysis()
{
	int currentLexemesNumber; // ���� ���� ������� ��������� "��������", ������������� ���-�� ������ ������ �� ���.

	if (lexemes.front() == CodePair(0, 0)) // lexeme == 'begin'
	{
		lexemes.erase(lexemes.begin());

		// ������� ���� ������� ������ ���������
		while (currentLexemesNumber = lexemes.size(), analysis->errorCode == 0 && lexemes.size() != 0 && lexemes.front() != CodePair(0, 3)) // lexeme != 'end'
		{
			while (variablesDescription()); // ������ �������� ����������
			operatorRule(); // ������ ���������

			if (currentLexemesNumber == lexemes.size()) break; // �������� �� "����� �������"
		}

		// �������� �� ������� ��������� "end"
		if (lexemes.size() != 0 && lexemes.front() == CodePair(0, 3))
		{
			lexemes.erase(lexemes.begin());
		}
		else
		{
			// � ������ ��������������� ������� ����� ������������� ���������� ������ ���������,
			// ������� ����� �������� ������ �� ����� ��������������, ����� ����� ��������� ������� �� �������������.
			if (analysis->errorCode < 3) analysis->errorCode = 3; // �������� ��������� ���������
		}

		// ���� ����� ��������� ��������� ��� �������� �����-�� ��������� �������, �� ��������� ������.
		if (lexemes.size() != 0)
		{
			if (analysis->errorCode < 3) analysis->errorCode = 3;
		}

		// ������������� ��������, ���� �� ������� ��� �������������� ����������
		semantic->identifiersSemanticCheck();
		// ������� ����� � �������� �������������
		semantic->allNumbersToMachineRepresentation();
	}
	else
	{
		analysis->errorCode = 3; // �������� ��������� ���������
	}
}

// ������� "��������"
void SyntacticAnalysis::operatorRule()
{
	// ���� ��� ��������� ��������
	if (lexemes.front() == CodePair(0, 0)) // lexeme == "begin"
	{
		compoundOperatorRule();
	}

	// ���� ��� �������� ������������
	else if (lexemes.front().tableNum == 3)
	{
		assignmentOperator();
	}

	// ���� ��� �������� ��������
	else if (lexemes.front() == CodePair(0, 6)) // lexeme == "if"
	{
		conditionalOperator();
	}
	
	// ���� ��� ������������� ����
	else if (lexemes.front() == CodePair(0, 5)) // lexeme == "for"
	{
		fixedLoopOperator();
	}

	// ���� ��� �������� ��������� �����
	else if (lexemes.front() == CodePair(0, 14)) // lexeme == "while"
	{
		conditionalLoopOperator();
	}

	// ���� ��� �������� �����
	else if (lexemes.front() == CodePair(0, 9)) // lexeme == "readln"
	{
		inputOperator();
	}

	// ���� ��� �������� ������
	else if (lexemes.front() == CodePair(0, 15)) // lexeme == "writeln"
	{
		outputOperator();
	}
}

// ������� "��������� ��������"
void SyntacticAnalysis::compoundOperatorRule()
{
	lexemes.erase(lexemes.begin());

	do
	{
		if (lexemes.front() == CodePair(1, 11))
			lexemes.erase(lexemes.begin());
		operatorRule();
	} while (lexemes.size() != 0 && lexemes.front() == CodePair(1, 11)); // lexeme == ';'

	if (lexemes.front() == CodePair(0, 3)) // lexeme == "end"
		lexemes.erase(lexemes.begin());
	else analysis->errorCode = 5; // �������� ��������� ���������� ���������
}

// "�������� ������������"
void SyntacticAnalysis::assignmentOperator()
{
	lexemes.erase(lexemes.begin());

	if (lexemes.front() == CodePair(1, 10)) // lexeme == ":="
		lexemes.erase(lexemes.begin());
	else analysis->errorCode = 6; // �������� ��������� ��������� ������������

	expression();
}

// "�������� ��������"
void SyntacticAnalysis::conditionalOperator()
{
	lexemes.erase(lexemes.begin());

	if (lexemes.front() == CodePair(1, 0)) // lexeme = '('
	{
		lexemes.erase(lexemes.begin());
		expression();
		if (lexemes.front() == CodePair(1, 1)) // lexeme == ')'
			lexemes.erase(lexemes.begin());
		else analysis->errorCode = 8; // �������� ��������� ��������� ���������

		operatorRule();

		if (lexemes.front() == CodePair(0, 2)) // lexeme = "else"
		{
			lexemes.erase(lexemes.begin());
			operatorRule();
		}
	}
	else
	{
		analysis->errorCode = 8; // �������� ��������� ��������� ���������
	}
}

// ������� "�������� �������������� �����"
void SyntacticAnalysis::fixedLoopOperator()
{
	lexemes.erase(lexemes.begin());
	assignmentOperator();

	if (lexemes.front() == CodePair(0, 12)) // lexeme == "to"
		lexemes.erase(lexemes.begin());
	else analysis->errorCode = 9; // �������� ��������� �������������� �����

	expression();

	if (lexemes.front() == CodePair(0, 11)) // lexeme == "step"
	{
		lexemes.erase(lexemes.begin());
		expression();
	}

	operatorRule();
	if (lexemes.front() == CodePair(0, 8)) // lexeme == "next"
		lexemes.erase(lexemes.begin());
	else analysis->errorCode = 9; // �������� ��������� �������������� �����
}

// "�������� ��������� �����"
void SyntacticAnalysis::conditionalLoopOperator()
{
	lexemes.erase(lexemes.begin());

	if (lexemes.front() == CodePair(1, 0)) // lexeme == '('
		lexemes.erase(lexemes.begin());
	else analysis->errorCode = 10; // �������� ��������� ��������� �����

	expression();

	if (lexemes.front() == CodePair(1, 1)) // lexeme == ')'
		lexemes.erase(lexemes.begin());
	else analysis->errorCode = 10; // �������� ��������� ��������� �����

	operatorRule();
}

// ������� "�������� �����"
void SyntacticAnalysis::inputOperator()
{
	lexemes.erase(lexemes.begin());

	do
	{
		if (lexemes.front() == CodePair(1, 5))
			lexemes.erase(lexemes.begin());
		
		if (lexemes.front().tableNum == 3)
			lexemes.erase(lexemes.begin());
		else analysis->errorCode = 11; // �������� ��������� ��������� �����

	} while (lexemes.size() != 0 && lexemes.front() == CodePair(1, 5)); // lexeme == ','
}

// ������� "�������� ������"
void SyntacticAnalysis::outputOperator()
{
	lexemes.erase(lexemes.begin());

	do
	{
		if (lexemes.front() == CodePair(1, 5))
			lexemes.erase(lexemes.begin());

		expression();
	} while (lexemes.size() != 0 && lexemes.front() == CodePair(1, 5)); // lexeme == ','
}

// ������� "���������"
void SyntacticAnalysis::expression()
{
	do
	{
		// lexeme == '<>' || '=' || '<' || '<=' || '>' || '>='
		if (lexemes.front() == CodePair(1, 14) || lexemes.front() == CodePair(1, 15) || lexemes.front() == CodePair(1, 12) || lexemes.front() == CodePair(1, 13) || lexemes.front() == CodePair(1, 16) || lexemes.front() == CodePair(1, 17))
			lexemes.erase(lexemes.begin());
		operand();
	} while (lexemes.size() != 0 &&
		(lexemes.front() == CodePair(1, 14) || lexemes.front() == CodePair(1, 15) || lexemes.front() == CodePair(1, 12) || lexemes.front() == CodePair(1, 13) || lexemes.front() == CodePair(1, 16) || lexemes.front() == CodePair(1, 17)));
}

// ������� "�������"
void SyntacticAnalysis::operand()
{
	do
	{
		// lexeme == '+' || '-' || "or"
		if (lexemes.front() == CodePair(1, 4) || lexemes.front() == CodePair(1, 6) || lexemes.front() == CodePair(1, 20))
			lexemes.erase(lexemes.begin());
		term();
	} while (lexemes.size() != 0 &&
		(lexemes.front() == CodePair(1, 4) || lexemes.front() == CodePair(1, 6) || lexemes.front() == CodePair(1, 20)));
}

// ������� "���������"
void SyntacticAnalysis::term()
{
	do
	{
		// lexeme == '*' || '/' || "and"
		if (lexemes.front() == CodePair(1, 2) || lexemes.front() == CodePair(1, 7) || lexemes.front() == CodePair(1, 18))
			lexemes.erase(lexemes.begin());
		factor();
	} while (lexemes.size() != 0 &&
		(lexemes.front() == CodePair(1, 2) || lexemes.front() == CodePair(1, 7) || lexemes.front() == CodePair(1, 18)));
}

// ������� "���������"
void SyntacticAnalysis::factor()
{
	if (lexemes.front().tableNum == 3 || lexemes.front().tableNum == 2 || lexemes.front() == CodePair(0, 13) || lexemes.front() == CodePair(0, 4))
		lexemes.erase(lexemes.begin());
	else if (lexemes.front() == CodePair(1, 19)) // lexeme == "not"
	{
		lexemes.erase(lexemes.begin());
		factor();
	}
	else if (lexemes.front() == CodePair(1, 0)) // lexeme = '('
	{
		lexemes.erase(lexemes.begin());
		expression();
		if (lexemes.front() == CodePair(1, 1)) // lexeme == ')'
			lexemes.erase(lexemes.begin());
		else analysis->errorCode = 7; // �������� ��������� ���������
	}
	else
	{
		analysis->errorCode = 7; // �������� ��������� ���������
	}
}

// ������� "�������� ����������"
bool SyntacticAnalysis::variablesDescription()
{
	bool isThereDescription = false; // ���� ��� ����������� ������� "�������� ����������" � ��������

	// ���� ��� ������ ����������� ';' � ����� ��� ����� ��� ������, �� ���������� ������ �������� ����������
	for (int i = 0; i < lexemes.size(); ++i)
	{
		if (lexemes.at(i) == CodePair(1, 11) && i != 0) // lexeme == ';'
		{
			if (lexemes.at(i - 1) == CodePair(0, 1) || lexemes.at(i - 1) == CodePair(0, 7) || lexemes.at(i - 1) == CodePair(0, 10))
			{
				isThereDescription = true;
				break;
			}
		}	
	}
	if (!isThereDescription) return false;

	std::vector<int> identifiers; // ������ ����������� ��������������� ��� ��������� �� ��������
	do
	{
		if (lexemes.front() == CodePair(1, 5))
			lexemes.erase(lexemes.begin());

		if (lexemes.front().tableNum == 3)
		{
			identifiers.push_back((*lexemes.begin()).indexNum);
			lexemes.erase(lexemes.begin());
		}
		else
		{
			analysis->errorCode = 4; // �������� ��������� �������� ����������
			return false;
		}
	}
	while (lexemes.size() != 0 && lexemes.front() == CodePair(1, 5)); // lexeme == ','
		
	if (lexemes.front() == CodePair(1, 9)) // lexeme == ':'
		lexemes.erase(lexemes.begin());
	else
	{
		analysis->errorCode = 4; // �������� ��������� �������� ����������
		return false;
	}

	// lexeme == "boolean" || "integer" || "real"
	if (lexemes.front() == CodePair(0, 1) || lexemes.front() == CodePair(0, 7) || lexemes.front() == CodePair(0, 10))
	{
		semantic->descriptionSemanticProcessing(identifiers, lexemes.front().indexNum); // ������������� ��������� ��������� ��������������� ����������
		lexemes.erase(lexemes.begin());
	}
	else
	{
		analysis->errorCode = 4; // �������� ��������� �������� ����������
		return false;
	}

	if (lexemes.front() == CodePair(1, 11)) // lexeme == ';'
		lexemes.erase(lexemes.begin());
	else
	{
		analysis->errorCode = 4; // �������� ��������� �������� ����������
		return false;
	}

	return true;
}