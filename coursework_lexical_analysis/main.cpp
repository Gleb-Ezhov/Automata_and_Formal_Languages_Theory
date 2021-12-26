#include "coursework_lexical_analysis.h" /* Заголовок окна по умолчанию от QT Designer'а */
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QGridLayout>
#include <QTableView>
#include <QPushButton>

#include "lexical_analysis.h"

ProgramAnalysis* analysis;

void startAnalysisButtonHandler()
{
    analysis->errorCode = 0;
    analysis->startAnalysis();
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //coursework_lexical_analysis w;
    //w.show();

    analysis = new ProgramAnalysis();

    QPushButton* startAnalysisButton = new QPushButton{};
    startAnalysisButton->setText("Старт");
    startAnalysisButton->setFixedSize(62, 62);
    // Если кнопка работает вне экземпляра класса, то можно игнорировать макросы SIGNAL и SLOT.
    // Сигнал и функция для него прикрепляются через указатели на соответствующие функции.
    startAnalysisButton->connect(startAnalysisButton, &QPushButton::clicked, &startAnalysisButtonHandler);

    // Главное окно
    QMainWindow window;

    // Центральный виджет с моделью размещения QVBoxLayout
    QWidget* centralWidget = new QWidget();
    QVBoxLayout* centralLayout = new QVBoxLayout(centralWidget);

    // Верхний виджет с моделью размещения QHBoxLayout для таблиц и текстовых виджетов
    QWidget* upperWidget = new QWidget();
    QHBoxLayout* upperLayout = new QHBoxLayout(upperWidget);

    // Нижний виджет с кнопкой и строкой состояния
    QWidget* bottomWidget = new QWidget();
    bottomWidget->setFixedHeight(80);
    QHBoxLayout* bottomLayout = new QHBoxLayout(bottomWidget);

    // Левый виджет с компоновкой QGridLayout с таблицами
    QWidget* gridWidget = new QWidget();
    QGridLayout* gridLayout = new QGridLayout(gridWidget);

    // добавление виджетов в соответствующие родительские виджеты
    centralLayout->addWidget(upperWidget);
    centralLayout->addWidget(bottomWidget);

    upperLayout->addWidget(gridWidget);
    upperLayout->addWidget(analysis->textEditInput);
    upperLayout->addWidget(analysis->textEditOutput);

    bottomLayout->addWidget(startAnalysisButton);
    bottomLayout->addWidget(analysis->textEditStatusLogs);

    gridLayout->addWidget(analysis->keywordsTable, 0, 0);
    gridLayout->addWidget(analysis->separatorsTable, 0, 1);
    gridLayout->addWidget(analysis->numbersTable, 1, 0);
    gridLayout->addWidget(analysis->identifiersTable, 1, 1);

    window.setCentralWidget(centralWidget);
    window.resize(1600, 720);
    window.show();

    return a.exec();
}
