#include "semantic_analysis.h"
#include "program_analysis.h"
#include <bitset>

SemanticAnalysis::SemanticAnalysis(ProgramAnalysis* analysis) : analysis(analysis)
{

};

void SemanticAnalysis::descriptionSemanticProcessing(std::vector<int> identifiers, int type)
{
	std::string typeStr;
	if (type == 1) typeStr = "boolean";
	else if (type == 7) typeStr = "integer";
	else if (type == 10) typeStr = "real";

	int* counters = new int[analysis->identifiers.size()]();
	for (int index : identifiers)
	{
		++counters[index];
	}

	for (int index : identifiers)
	{
		analysis->identifierSemanticFill(index, counters[index], typeStr);
		if (counters[index] > 1) analysis->errorCode = 12; // Идентификатор описан более одного раза
	}

	delete[] counters;
}

void SemanticAnalysis::identifiersSemanticCheck()
{
	for (int i = 0; i < analysis->identifiersTable->rowCount(); ++i)
	{
		if (!analysis->identifiersTable->item(i, 2))
		{
			analysis->errorCode = 13; // Некоторые из идентификаторв не были описаны
		}
	}
}

void SemanticAnalysis::expressionAnalysis()
{
	// todo написать анализ выражения
}

void SemanticAnalysis::allNumbersToMachineRepresentation()
{
	char lastChar;
	size_t realNumberDot, realNumberExponentBig, realNumberExponentSmall;

	for (int i = 0; i < analysis->numbers.size(); ++i)
	{
		lastChar = analysis->numbers.at(i).back();
		realNumberDot = analysis->numbers.at(i).find('.');
		realNumberExponentBig = analysis->numbers.at(i).find('E');
		realNumberExponentSmall = analysis->numbers.at(i).find('e');

		if (realNumberDot != std::string::npos || (realNumberExponentBig != std::string::npos && (lastChar != 'H' && lastChar != 'h'))
			|| realNumberExponentSmall != std::string::npos && (lastChar != 'H' && lastChar != 'h'))
		{
			// сюда падает вещественное число (нашлась точка или экспонента)
			realStrToBinary(i, analysis->numbers.at(i));
		}
		else if (lastChar == 'D' || lastChar == 'd' || lastChar >= '0' && lastChar <= '9')
		{
			decimalStrToBinary(i, analysis->numbers.at(i));
		}
		else if (lastChar == 'H' || lastChar == 'h')
		{
			hexStrToBinary(i, analysis->numbers.at(i));
		}
		else if (lastChar == 'B' || lastChar == 'b')
		{
			binStrToBinary(i, analysis->numbers.at(i));
		}
		else if (lastChar == 'O' || lastChar == 'o')
		{
			octalStrToBinary(i, analysis->numbers.at(i));
		}
	}
}

void SemanticAnalysis::decimalStrToBinary(int index, std::string number)
{
	if (number.back() == 'D' || number.back() == 'd') number.pop_back();
	
	int a = atoi(number.data());
	
	// В качестве строки передаётся bitset с количеством бит 8 * sizeof(a). Набор бит составляется автоматически
	// после передачи целого числа в конструктор bitset.
	analysis->numbersTable->setItem(index, 2, new QTableWidgetItem(QString::fromStdString(std::bitset<CHAR_BIT * sizeof(a)>(a).to_string())));
}

void SemanticAnalysis::realStrToBinary(int index, std::string number)
{
	// Байты вещественного числа копируются в переменную uint64_t размером 64 бита, как и double.
	// Целое число преобразуется в битовую последовательность внутри конструктора bitset.
	double realNumber = atof(number.data());
	uint64_t u;
	memcpy(&u, &realNumber, sizeof(realNumber));
	std::bitset<CHAR_BIT * sizeof(u)> bits(u);

	analysis->numbersTable->setItem(index, 2, new QTableWidgetItem(QString::fromStdString(bits.to_string())));
}

void SemanticAnalysis::hexStrToBinary(int index, std::string number)
{
	if (number.back() == 'H' || number.back() == 'h') number.pop_back();
	float a;

	if (number.front() == '-') a = atof(number.insert(1, "0x").data());
	else a = atof(number.insert(0, "0x").data());
	
	std::bitset<CHAR_BIT * sizeof(a)> bits(a);  // передавая в конструктор целое число в double переменной, оно неявно преобразуется к целому типу
	analysis->numbersTable->setItem(index, 2, new QTableWidgetItem(QString::fromStdString(bits.to_string())));
}

void SemanticAnalysis::binStrToBinary(int index, std::string number)
{
	if (number.back() == 'B' || number.back() == 'b') number.pop_back();
	// В конструктор передаётся строка с последовательностью бит, она и будет взята за основу bitset'ом.
	std::bitset<CHAR_BIT * sizeof(int)> bits(number);
	analysis->numbersTable->setItem(index, 2, new QTableWidgetItem(QString::fromStdString(bits.to_string())));
}

void SemanticAnalysis::octalStrToBinary(int index, std::string number)
{
	if (number.back() == 'O' || number.back() == 'o') number.pop_back();
	int a = std::stoi(number.data(), nullptr, 8);
	std::bitset<CHAR_BIT * sizeof(a)> bits(a);
	analysis->numbersTable->setItem(index, 2, new QTableWidgetItem(QString::fromStdString(bits.to_string())));
}