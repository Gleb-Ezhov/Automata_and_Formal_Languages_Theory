#pragma once
#include <QTableWidget>
#include <QHeaderView>
#include <QTextEdit>
#include <QLineEdit>

class ProgramAnalysis
{
public:
	ProgramAnalysis();

	void startAnalysis();

	// Добавление лексемы в одну из таблиц
	void addItemToTableWidget(QTableWidget* table, std::string& lexeme);

	// Заполнение семантических параметров идентификатора
	void ProgramAnalysis::identifierSemanticFill(int index, int descrCounter, std::string type);

	std::vector<std::string> keywords = { "begin", "boolean", "else", "end", "false", "for", "if", "integer", "next", "readln", "real", "step", "to", "true", "while", "writeln" };
	std::vector<std::string> separators = { "(", ")", "*", "*/", "+", ",", "-", "/", "/*", ":", ":=", ";", "<", "<=", "<>", "=", ">", ">=", "and", "not", "or" };
	std::vector<std::string> numbers;
	std::vector<std::string> identifiers;

	QTableWidget* keywordsTable;
	QTableWidget* separatorsTable;
	QTableWidget* numbersTable;
	QTableWidget* identifiersTable;

	QTextEdit* textEditInput;
	QTextEdit* textEditOutput;
	QLineEdit* statusBar;
	int errorCode = 0;

private:
    // Создание объектов таблиц (ключевые слова, разделители, числа, идентификаторы).
	void tablesInitialization();

	// Создание объектов полей для текста
	void textEditsInitialization();
};