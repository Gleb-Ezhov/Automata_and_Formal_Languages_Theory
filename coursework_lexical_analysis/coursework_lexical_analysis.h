#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_coursework_lexical_analysis.h"

class coursework_lexical_analysis : public QMainWindow
{
    Q_OBJECT

public:
    coursework_lexical_analysis(QWidget *parent = Q_NULLPTR);

private:
    Ui::coursework_lexical_analysisClass ui;
};
