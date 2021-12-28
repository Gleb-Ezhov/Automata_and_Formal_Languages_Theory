#include "program_analysis.h"
#include "lexical_analysis.h"
#include "syntactic_analysis.h"
#include "semantic_analysis.h"

ProgramAnalysis::ProgramAnalysis()
{
    tablesInitialization();
    textEditsInitialization();
}

void ProgramAnalysis::startAnalysis()
{
    errorCode = 0;
    textEditStatusLogs->clear();
    textEditOutput->clear();
    numbersTable->setRowCount(0);
    identifiersTable->setRowCount(0);
    numbers.clear();
    identifiers.clear();

    LexicalAnalysis* lexical = new LexicalAnalysis(this);
    if (errorCode == 0) // Синтаксический анализ не будет начинаться, если была найдена ошибка на этапе лексического анализа
    {
        SemanticAnalysis* semantic = new SemanticAnalysis(this);
        SyntacticAnalysis* syntactic = new SyntacticAnalysis(this, semantic, lexical->getLexicalOutput());
    }

    if (!errorCode)
        textEditStatusLogs->append(QString::fromStdString("Этап анализа успешно завершён."));
    else
        textEditStatusLogs->append(QString::fromStdString("Ошибка анализа."));

    // Старый способ отображения ошибок для QLineEdit статус бара.
    /*if (!errorCode)
        textEditStatusLogs->append(QString::fromStdString("Этап анализа успешно завершён."));
    else if (errorCode == 1)
        textEditStatusLogs->append(QString::fromStdString("Ошибка ЛА1. Ошибка формирования числа."));
    else if (errorCode == 2)
        textEditStatusLogs->append(QString::fromStdString("Ошибка ЛА2. Разделитель не найден в таблице разделителей."));
    else if (errorCode == 3)
        textEditStatusLogs->append(QString("Ошибка СиА1. Нарушена структура программы."));
    else if (errorCode == 4)
        textEditStatusLogs->append(QString("Ошибка СиА2. Нарушена структура описания переменных."));
    else if (errorCode == 5)
        textEditStatusLogs->append(QString("Ошибка СиА3. Нарушена структура составного оператора."));
    else if (errorCode == 6)
        textEditStatusLogs->append(QString("Ошибка СиА4. Нарушена структура оператора присваивания."));
    else if (errorCode == 7)
        textEditStatusLogs->append(QString("Ошибка СиА5. Нарушена структура выражения."));
    else if (errorCode == 8)
        textEditStatusLogs->append(QString("Ошибка СиА6. Нарушена структура условного оператора."));
    else if (errorCode == 9)
        textEditStatusLogs->append(QString("Ошибка СиА7. Нарушена структура фиксированного цикла."));
    else if (errorCode == 10)
        textEditStatusLogs->append(QString("Ошибка СиА8. Нарушена структура условного цикла."));
    else if (errorCode == 11)
        textEditStatusLogs->append(QString("Ошибка СиА9. Нарушена структура оператора ввода."));
    else if (errorCode == 12)
        textEditStatusLogs->append(QString("Ошибка СеА1. Идентификатор описан более одного раза."));
    else if (errorCode == 13)
        textEditStatusLogs->append(QString("Ошибка СеА2. Некоторые из идентификаторов не были описаны."));

        analysis->textEditStatusLogs->append(QString("Ошибка СеА3. Несовместимые типы в выражении."));
        analysis->errorCode = 14;

        analysis->textEditStatusLogs->append(QString("Ошибка СеА4. Тип идентификатора не соответсвует типу выражения в операторе присваивания."));
        analysis->errorCode = 15;

        analysis->textEditStatusLogs->append(QString("Ошибка СеА5. Результирующий тип выражения в условном операторе отличен от boolean."));
        analysis->errorCode = 16;

        analysis->textEditStatusLogs->append(QString("Ошибка СеА6. Результирующий тип выражения в операторе фиксированного цикла отличен от boolean."));
        analysis->errorCode = 17;

        analysis->textEditStatusLogs->append(QString("Ошибка СеА7. Результирующий тип выражения в операторе условного цикла отличен от boolean."));
        analysis->errorCode = 18;
    else
        textEditStatusLogs->append(QString::fromStdString("Ошибка анализа."));*/
}

void ProgramAnalysis::addItemToTableWidget(QTableWidget* table, std::string& lexeme, bool isNumber, std::string type)
{
    int rowCount = table->rowCount();
    table->insertRow(rowCount);
    table->setItem(rowCount, 0, new QTableWidgetItem(QString::fromStdString(std::to_string(rowCount))));
    table->setItem(rowCount, 1, new QTableWidgetItem(QString::fromStdString(lexeme)));

    if (isNumber)
        table->setItem(rowCount, 3, new QTableWidgetItem(QString::fromStdString(type)));
}

void ProgramAnalysis::textEditsInitialization()
{
    textEditInput = new QTextEdit();
    QFont font = QFont();
    font.setPointSize(14);
    textEditInput->setFont(font);
    textEditInput->setTabStopDistance(20);
    textEditInput->setText(QString::fromStdString(R"(begin
	x, y, z: real;
	flag: boolean;
	i, o, h, b: integer;

	begin
		readln x, y;
		z := -76.1E-3;
		flag := false;

        o := 71O;
        h := 458efbaH;
        b := 010101010B;
	end

	while (x < z) begin
		x := x + 0.01
	end

	if (y <> x) flag := true
	else y := y + x

	for i := 0 to i < 10 step i + 0.1
		begin
			y := y - 0.001 + z;
		end
	next

	writeln x, y, z
end)"));

    textEditOutput = new QTextEdit();
    textEditOutput->setFont(font);
    textEditOutput->setReadOnly(true);

    textEditStatusLogs = new QTextEdit();
    textEditStatusLogs->setReadOnly(true);
    textEditStatusLogs->setFixedHeight(60);
}

void ProgramAnalysis::tablesInitialization()
{
    keywordsTable = new QTableWidget();
    keywordsTable->setColumnCount(2);
    keywordsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    QStringList headers1 = { "№", "Ключевое слово" };
    keywordsTable->setHorizontalHeaderLabels(headers1);
    keywordsTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    keywordsTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    keywordsTable->verticalHeader()->setVisible(false);

    for (int i = 0; i < keywords.size(); ++i)
    {
        addItemToTableWidget(keywordsTable, keywords[i]);
    }

    separatorsTable = new QTableWidget();
    separatorsTable->setColumnCount(2);
    separatorsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    QStringList headers2 = { "№", "Разделитель" };
    separatorsTable->setHorizontalHeaderLabels(headers2);
    separatorsTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    separatorsTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    separatorsTable->verticalHeader()->setVisible(false);

    for (int i = 0; i < separators.size(); ++i)
    {
        addItemToTableWidget(separatorsTable, separators[i]);
    }

    numbersTable = new QTableWidget();
    numbersTable->setColumnCount(4);
    numbersTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    QStringList headers3 = { "№", "Число", "Машинное представ.", "Тип" };
    numbersTable->setHorizontalHeaderLabels(headers3);
    numbersTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    numbersTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    numbersTable->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    numbersTable->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    numbersTable->verticalHeader()->setVisible(false);
    numbersTable->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);

    identifiersTable = new QTableWidget();
    identifiersTable->setColumnCount(4);
    identifiersTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    QStringList headers4 = { "№", "Идентификатор", "Описан", "Тип" };
    identifiersTable->setHorizontalHeaderLabels(headers4);
    identifiersTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    identifiersTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    identifiersTable->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    identifiersTable->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    identifiersTable->verticalHeader()->setVisible(false);
}

void ProgramAnalysis::identifierSemanticFill(int index, int descrCounter, std::string type)
{
    identifiersTable->setItem(index, 2, new QTableWidgetItem(QString::fromStdString(std::to_string(descrCounter))));
    identifiersTable->setItem(index, 3, new QTableWidgetItem(QString::fromStdString(type)));
}