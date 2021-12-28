#include "syntactic_analysis.h"

SyntacticAnalysis::SyntacticAnalysis(ProgramAnalysis* analysis, SemanticAnalysis* semantic, std::vector<CodePair> lexemes)
{
	this->analysis = analysis;
	this->semantic = semantic;
	this->lexemes = lexemes;
	syntacticAnalysis();
}

// Корневое правило "Программа"
void SyntacticAnalysis::syntacticAnalysis()
{
	int currentLexemesNumber; // Если цикл разбора программы "зависнет", сохраняющееся кол-во лексем укажет на это.

	if (lexemes.front() == CodePair(0, 0)) // lexeme == 'begin'
	{
		lexemes.erase(lexemes.begin());

		// Главный цикл разбора текста программы
		while (currentLexemesNumber = lexemes.size(), lexemes.size() != 0 && lexemes.front() != CodePair(0, 3)) // lexeme != 'end'
		{
			while (variablesDescription()); // разбор описаний переменных
			operatorRule(); // разбор оператора

			if (currentLexemesNumber == lexemes.size()) break; // проверка на "тупик разбора"
		}

		// Проверка на наличие конечного "end"
		if (lexemes.size() != 0 && lexemes.front() == CodePair(0, 3))
		{
			lexemes.erase(lexemes.begin());
		}
		else
		{
			// В основе синтаксического анализа лежит левосторонний нисходящий разбор программы,
			// поэтому более глубокие ошибки не нужно перезаписывать, чтобы лучше наблюдать порядок их возникновения.
			if (analysis->errorCode < 3)
				analysis->textEditStatusLogs->append(QString("Ошибка СиА1. Нарушена структура программы.")),
				analysis->errorCode = 3;
		}

		// Если после окончания программы ещё остались какие-то сторонние лексемы, то выводится ошибка.
		if (lexemes.size() != 0)
		{
			if (analysis->errorCode < 3)
				analysis->textEditStatusLogs->append(QString("Ошибка СиА1. Нарушена структура программы.")),
				analysis->errorCode = 3;
		}

		// Семантическая проверка, были ли описаны все идентификаторы переменных
		semantic->identifiersSemanticCheck();
		// Перевод чисел в двоичное представление
		semantic->allNumbersToMachineRepresentation();
	}
	else
	{
		analysis->textEditStatusLogs->append(QString("Ошибка СиА1. Нарушена структура программы."));
		analysis->errorCode = 3;
	}
}

// Правило "Оператор"
void SyntacticAnalysis::operatorRule()
{
	// Если это составной оператор
	if (lexemes.front() == CodePair(0, 0)) // lexeme == "begin"
	{
		compoundOperatorRule();
	}

	// Если это оператор присваивания
	else if (lexemes.front().tableNum == 3)
	{
		assignmentOperator();
	}

	// Если это условный оператор
	else if (lexemes.front() == CodePair(0, 6)) // lexeme == "if"
	{
		conditionalOperator();
	}
	
	// Если это фиксированный цикл
	else if (lexemes.front() == CodePair(0, 5)) // lexeme == "for"
	{
		fixedLoopOperator();
	}

	// Если это оператор условного цикла
	else if (lexemes.front() == CodePair(0, 14)) // lexeme == "while"
	{
		conditionalLoopOperator();
	}

	// Если это оператор ввода
	else if (lexemes.front() == CodePair(0, 9)) // lexeme == "readln"
	{
		inputOperator();
	}

	// Если это оператор вывода
	else if (lexemes.front() == CodePair(0, 15)) // lexeme == "writeln"
	{
		outputOperator();
	}
}

// Правило "Составной оператор"
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
	else
		analysis->textEditStatusLogs->append(QString("Ошибка СиА3. Нарушена структура составного оператора.")),
		analysis->errorCode = 5;
}

// "Оператор присваивания"
void SyntacticAnalysis::assignmentOperator()
{
	std::string identifierType;

	if (analysis->identifiersTable->item(lexemes.front().indexNum, 2) != nullptr) // только если идентификатор был описан
	{
		// тип переменной справа от оператора присваивания
		identifierType = analysis->identifiersTable->item(lexemes.front().indexNum, 3)->text().toStdString();
	}
	lexemes.erase(lexemes.begin());

	if (lexemes.size() != 0 && lexemes.front() == CodePair(1, 10)) // lexeme == ":="
		lexemes.erase(lexemes.begin());
	else
	{
		analysis->textEditStatusLogs->append(QString("Ошибка СиА4. Нарушена структура оператора присваивания."));
		analysis->errorCode = 6;
		if (lexemes.size() == 0) return; // выход из анализа, если в конце программы не было "end", но встретился очередной идентификатор
	}

	// expression() возвращает тип правой части присваивания. Если типы не соответствуют друг другу – выводится ошибка.
	if (identifierType != expression())
	{
		analysis->textEditStatusLogs->append(QString("Ошибка СеА4. Тип идентификатора не соответсвует типу выражения в операторе присваивания."));
		analysis->errorCode = 15;
	}
}

// "Условный оператор"
void SyntacticAnalysis::conditionalOperator()
{
	lexemes.erase(lexemes.begin());

	if (lexemes.front() == CodePair(1, 0)) // lexeme = '('
	{
		lexemes.erase(lexemes.begin());

		if (expression() != "boolean") // Отрабатывает expression() + вывод ошибки в случае неправильного типа выражения
		{
			analysis->textEditStatusLogs->append(QString("Ошибка СеА5. Результирующий тип выражения в условном операторе отличен от boolean."));
			analysis->errorCode = 16;
		}

		if (lexemes.front() == CodePair(1, 1)) // lexeme == ')'
			lexemes.erase(lexemes.begin());
		else
			analysis->textEditStatusLogs->append(QString("Ошибка СиА6. Нарушена структура условного оператора.")),
			analysis->errorCode = 8;

		operatorRule();

		if (lexemes.front() == CodePair(0, 2)) // lexeme = "else"
		{
			lexemes.erase(lexemes.begin());
			operatorRule();
		}
	}
	else
	{
		analysis->textEditStatusLogs->append(QString("Ошибка СиА6. Нарушена структура условного оператора."));
		analysis->errorCode = 8;
	}
}

// Правило "Оператор фиксированного цикла"
void SyntacticAnalysis::fixedLoopOperator()
{
	lexemes.erase(lexemes.begin());
	assignmentOperator();

	if (lexemes.front() == CodePair(0, 12)) // lexeme == "to"
		lexemes.erase(lexemes.begin());
	else
		analysis->textEditStatusLogs->append(QString("Ошибка СиА7. Нарушена структура фиксированного цикла.")),
		analysis->errorCode = 9;

	if (expression() != "boolean") // Отрабатывает expression() + вывод ошибки в случае неправильного типа выражения
	{
		analysis->textEditStatusLogs->append(QString("Ошибка СеА6. Результирующий тип выражения в операторе фиксированного цикла отличен от boolean."));
		analysis->errorCode = 17;
	}

	if (lexemes.front() == CodePair(0, 11)) // lexeme == "step"
	{
		lexemes.erase(lexemes.begin());
		expression();
	}

	operatorRule();
	if (lexemes.front() == CodePair(0, 8)) // lexeme == "next"
		lexemes.erase(lexemes.begin());
	else
		analysis->textEditStatusLogs->append(QString("Ошибка СиА7. Нарушена структура фиксированного цикла.")),
		analysis->errorCode = 9;
}

// "Оператор условного цикла"
void SyntacticAnalysis::conditionalLoopOperator()
{
	lexemes.erase(lexemes.begin());

	if (lexemes.front() == CodePair(1, 0)) // lexeme == '('
		lexemes.erase(lexemes.begin());
	else
		analysis->textEditStatusLogs->append(QString("Ошибка СиА8. Нарушена структура условного цикла.")),
		analysis->errorCode = 10;

	if (expression() != "boolean") // Отрабатывает expression() + вывод ошибки в случае неправильного типа выражения
	{
		analysis->textEditStatusLogs->append(QString("Ошибка СеА7. Результирующий тип выражения в операторе условного цикла отличен от boolean."));
		analysis->errorCode = 18;
	}

	if (lexemes.front() == CodePair(1, 1)) // lexeme == ')'
		lexemes.erase(lexemes.begin());
	else
		analysis->textEditStatusLogs->append(QString("Ошибка СиА8. Нарушена структура условного цикла.")),
		analysis->errorCode = 10;

	operatorRule();
}

// Правило "Оператор ввода"
void SyntacticAnalysis::inputOperator()
{
	lexemes.erase(lexemes.begin());

	do
	{
		if (lexemes.front() == CodePair(1, 5))
			lexemes.erase(lexemes.begin());
		
		if (lexemes.front().tableNum == 3)
			lexemes.erase(lexemes.begin());
		else
			analysis->textEditStatusLogs->append(QString("Ошибка СиА9. Нарушена структура оператора ввода.")),
			analysis->errorCode = 11;

	} while (lexemes.size() != 0 && lexemes.front() == CodePair(1, 5)); // lexeme == ','
}

// Правило "Оператор вывода"
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

// Правило "Выражение"
std::string SyntacticAnalysis::expression()
{
	semantic->expressionStack.clear(); // очистка стэка выражения перед новым выражением

	do
	{
		// lexeme == '<>' || '=' || '<' || '<=' || '>' || '>='
		if (lexemes.front() == CodePair(1, 14) || lexemes.front() == CodePair(1, 15) || lexemes.front() == CodePair(1, 12) || lexemes.front() == CodePair(1, 13) || lexemes.front() == CodePair(1, 16) || lexemes.front() == CodePair(1, 17))
			semantic->expressionStack.push_back(lexemes.front()), // в стэк выражения попадает операция (для семантического анализа)
			lexemes.erase(lexemes.begin());

		operand();
	} while (lexemes.size() != 0 &&
		(lexemes.front() == CodePair(1, 14) || lexemes.front() == CodePair(1, 15) || lexemes.front() == CodePair(1, 12) || lexemes.front() == CodePair(1, 13) || lexemes.front() == CodePair(1, 16) || lexemes.front() == CodePair(1, 17)));

	return semantic->expressionAnalysis(); // Семантический анализ только что прочитанного выражения (проверка на совместимость типов операндов и операций)
}

// Правило "Операнд"
void SyntacticAnalysis::operand()
{
	do
	{
		// lexeme == '+' || '-' || "or"
		if (lexemes.front() == CodePair(1, 4) || lexemes.front() == CodePair(1, 6) || lexemes.front() == CodePair(1, 20))
			semantic->expressionStack.push_back(lexemes.front()), // в стэк выражения попадает операция (для семантического анализа)
			lexemes.erase(lexemes.begin());

		term();
	} while (lexemes.size() != 0 &&
		(lexemes.front() == CodePair(1, 4) || lexemes.front() == CodePair(1, 6) || lexemes.front() == CodePair(1, 20)));
}

// Правило "Слагаемое"
void SyntacticAnalysis::term()
{
	do
	{
		// lexeme == '*' || '/' || "and"
		if (lexemes.front() == CodePair(1, 2) || lexemes.front() == CodePair(1, 7) || lexemes.front() == CodePair(1, 18))
			semantic->expressionStack.push_back(lexemes.front()), // в стэк выражения попадает операция (для семантического анализа)
			lexemes.erase(lexemes.begin());

		factor();
	} while (lexemes.size() != 0 &&
		(lexemes.front() == CodePair(1, 2) || lexemes.front() == CodePair(1, 7) || lexemes.front() == CodePair(1, 18)));
}

// Правило "Множитель"
void SyntacticAnalysis::factor()
{
	if (lexemes.front().tableNum == 3 || lexemes.front().tableNum == 2 || lexemes.front() == CodePair(0, 13) || lexemes.front() == CodePair(0, 4))
	{
		// перед удалением входящей лексемы, она заносится в стэк выражения для дальнейшего семантического анализа
		semantic->expressionStack.push_back(lexemes.front());
		lexemes.erase(lexemes.begin());
	}
	else if (lexemes.front() == CodePair(1, 19)) // lexeme == "not"
	{
		semantic->expressionStack.push_back(lexemes.front());
		lexemes.erase(lexemes.begin());
		factor();
	}
	else if (lexemes.front() == CodePair(1, 0)) // lexeme = '('
	{
		lexemes.erase(lexemes.begin());
		expression();
		if (lexemes.front() == CodePair(1, 1)) // lexeme == ')'
			lexemes.erase(lexemes.begin());
		else
			analysis->textEditStatusLogs->append(QString("Ошибка СиА5. Нарушена структура выражения.")),
			analysis->errorCode = 7;
	}
	else
	{
		analysis->textEditStatusLogs->append(QString("Ошибка СиА5. Нарушена структура выражения."));
		analysis->errorCode = 7;
	}
}

// Правило "Описание переменных"
bool SyntacticAnalysis::variablesDescription()
{
	bool isThereDescription = false; // флаг для определения наличия "описания переменных" в лексемах

	// Если был найден разделитель ';' и перед ним стоит тип данных, то продолжаем разбор описания переменных
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

	std::vector<int> identifiers; // номера встреченных идентификаторов для обработки их описания
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
			analysis->textEditStatusLogs->append(QString("Ошибка СиА2. Нарушена структура описания переменных."));
			analysis->errorCode = 4;
			return false;
		}
	}
	while (lexemes.size() != 0 && lexemes.front() == CodePair(1, 5)); // lexeme == ','
		
	if (lexemes.front() == CodePair(1, 9)) // lexeme == ':'
		lexemes.erase(lexemes.begin());
	else
	{
		analysis->textEditStatusLogs->append(QString("Ошибка СиА2. Нарушена структура описания переменных."));
		analysis->errorCode = 4;
		return false;
	}

	// lexeme == "boolean" || "integer" || "real"
	if (lexemes.front() == CodePair(0, 1) || lexemes.front() == CodePair(0, 7) || lexemes.front() == CodePair(0, 10))
	{
		semantic->descriptionSemanticProcessing(identifiers, lexemes.front().indexNum); // семантическая обработка описанных идентификаторов переменных
		lexemes.erase(lexemes.begin());
	}
	else
	{
		analysis->textEditStatusLogs->append(QString("Ошибка СиА2. Нарушена структура описания переменных."));
		analysis->errorCode = 4;
		return false;
	}

	if (lexemes.front() == CodePair(1, 11)) // lexeme == ';'
		lexemes.erase(lexemes.begin());
	else
	{
		analysis->textEditStatusLogs->append(QString("Ошибка СиА2. Нарушена структура описания переменных."));
		analysis->errorCode = 4;
		return false;
	}

	return true;
}