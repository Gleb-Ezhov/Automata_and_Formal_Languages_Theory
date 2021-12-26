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
		if (counters[index] > 1)
			analysis->textEditStatusLogs->append(QString("Ошибка СеА1. Идентификатор описан более одного раза.")),
			analysis->errorCode = 12;
	}

	delete[] counters;
}

void SemanticAnalysis::identifiersSemanticCheck()
{
	for (int i = 0; i < analysis->identifiersTable->rowCount(); ++i)
	{
		if (!analysis->identifiersTable->item(i, 2))
		{
			analysis->textEditStatusLogs->append(QString("Ошибка СеА2. Некоторые из идентификаторов не были описаны."));
			analysis->errorCode = 13;
		}
	}
}

std::string SemanticAnalysis::expressionAnalysis()
{
	std::vector<std::string> strStack; // стэк для выражения в строковом представлении
	for (CodePair el : expressionStack)
	{
		if (el.tableNum == 3) // добавляется тип идентификатора
		{
			strStack.push_back(analysis->identifiersTable->item(el.indexNum, 3)->text().toStdString());
		}
		else if (el.tableNum == 1) // добавляется операция
		{
			strStack.push_back(analysis->separatorsTable->item(el.indexNum, 1)->text().toStdString());
		}
		else if (el.tableNum == 2) // добавляется тип числа
		{
			strStack.push_back(analysis->numbersTable->item(el.indexNum, 3)->text().toStdString());
		}
		else if (el.tableNum == 0) // если встречена логическая константа в выражении (true/false)
		{
			strStack.push_back("boolean");
		}
	}

	if (strStack.size() == 1) return strStack.at(0); // возврат одиночного значения в выражении

	int i = 0;
	while (strStack.size() != 1 && analysis->errorCode != 14)
	{
		for (i = 0; i < strStack.size(); ++i)
		{
			if (strStack.at(i) == "*" || strStack.at(i) == "+" || strStack.at(i) == "-" || strStack.at(i) == "/")
			{
				operationTypeValidation1(i, strStack);
				break;
			}
			else if (strStack.at(i) == "<" || strStack.at(i) == "<=" || strStack.at(i) == "<>" ||
				strStack.at(i) == ">" || strStack.at(i) == ">=" || strStack.at(i) == "=")
			{
				operationTypeValidation2(i, strStack);
				break;
			}
			else if (strStack.at(i) == "and" || strStack.at(i) == "not" || strStack.at(i) == "or")
			{
				operationTypeValidation3(i, strStack);
				break;
			}
		}
	}
	return strStack.at(0);
}

void SemanticAnalysis::operationTypeValidation1(int& index, std::vector<std::string>& strStack)
{
	if (strStack.at(index - 1) == "integer" && strStack.at(index + 1) == "integer")
	{
		// удаление обработанной операции из стэка выражения
		strStack.erase(strStack.begin());
		strStack.erase(strStack.begin());
		strStack.erase(strStack.begin());

		// добавление результата операции в стэк
		if (strStack.at(index) == "/") strStack.insert(strStack.begin(), "real");
		else strStack.insert(strStack.begin(), "integer");
	}
	else if (strStack.at(index - 1) == "real" && strStack.at(index + 1) == "integer" ||
		strStack.at(index - 1) == "integer" && strStack.at(index + 1) == "real" ||
		strStack.at(index - 1) == "real" && strStack.at(index + 1) == "real")
	{
		// удаление обработанной операции из стэка выражения
		strStack.erase(strStack.begin());
		strStack.erase(strStack.begin());
		strStack.erase(strStack.begin());

		// добавление результата операции в стэк
		strStack.insert(strStack.begin(), "real");
	}
	else if (strStack.at(index - 1) == "boolean" || strStack.at(index + 1) == "boolean")
	{
		analysis->textEditStatusLogs->append(QString("Ошибка СеА3. Несовместимые типы в выражении."));
		analysis->errorCode = 14;
	}
}

void SemanticAnalysis::operationTypeValidation2(int& index, std::vector<std::string>& strStack)
{
	if (strStack.at(index - 1) == "integer" && strStack.at(index + 1) == "integer")
	{
		strStack.erase(strStack.begin());
		strStack.erase(strStack.begin());
		strStack.erase(strStack.begin());

		strStack.insert(strStack.begin(), "boolean");
	}
	else if (strStack.at(index - 1) == "real" && strStack.at(index + 1) == "integer" ||
		strStack.at(index - 1) == "integer" && strStack.at(index + 1) == "real" ||
		strStack.at(index - 1) == "real" && strStack.at(index + 1) == "real")
	{
		strStack.erase(strStack.begin());
		strStack.erase(strStack.begin());
		strStack.erase(strStack.begin());

		strStack.insert(strStack.begin(), "boolean");
	}
	else if (strStack.at(index - 1) == "boolean" || strStack.at(index + 1) == "boolean")
	{
		if (strStack.at(index) == "<>" || strStack.at(index) == "=")
		{
			strStack.erase(strStack.begin());
			strStack.erase(strStack.begin());
			strStack.erase(strStack.begin());

			strStack.insert(strStack.begin(), "boolean");
		}
		else
		{
			analysis->textEditStatusLogs->append(QString("Ошибка СеА3. Несовместимые типы в выражении."));
			analysis->errorCode = 14;
		}
	}
}

void SemanticAnalysis::operationTypeValidation3(int& index, std::vector<std::string>& strStack)
{
	if (strStack.at(index - 1) == "boolean" && strStack.at(index + 1) == "boolean")
	{
		strStack.erase(strStack.begin());
		strStack.erase(strStack.begin());
		strStack.erase(strStack.begin());

		strStack.insert(strStack.begin(), "boolean");
	}
	else
	{
		analysis->textEditStatusLogs->append(QString("Ошибка СеА3. Несовместимые типы в выражении."));
		analysis->errorCode = 14;
	}
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