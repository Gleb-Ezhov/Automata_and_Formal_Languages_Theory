#include "lexical_analysis.h"

LexicalAnalysis::LexicalAnalysis(ProgramAnalysis* analysis)
{
    this->analysis = analysis;
    lexicalAnalysis();
}

void LexicalAnalysis::lexicalAnalysis()
{
    input = analysis->textEditInput->toPlainText().toStdString();

    char next;              // следующий прочитанный символ
    std::string buffer;     // буфер для формирования лексемы
    int index;              // индекс найденного в таблице разделителя

    while (!input.empty())
    {
        buffer.clear();
        next = getCharacterFromInput();

        if (next == '\t' || next == '\n' || next == ' ') // пропуск пробела, табуляции, переноса строки
        {
            continue;
        }
        else if (next >= 'A' && next <= 'z') // буква
        {
            IdentifierState(buffer, next);
        }
        else if (next == '0' || next == '1')
        {
            BinaryState(buffer, next);
        }
        else if (next >= '2' && next <= '7') // 8сс
        {
            OctalState(buffer, next);
        }
        else if (next == '8' || next == '9')
        {
            DecimalState(buffer, next);
        }
        else if (next == '.')
        {
            RealState(buffer, next);
        }

        // Разбор разделителей начинается с if, чтобы поймать разделители из next после проверок в числах
        if (next == '/') // D1 state
        {
            if (input.front() == '*') // D1* state
            {
                while (next != -1) // прочитываем в никуда символы внутри комментария, пока не встретим закрывающую последовательность "*/"
                {
                    next = getCharacterFromInput();
                    if (next == '*')
                    {
                        next = getCharacterFromInput();
                        if (next == '/') break;
                    }
                }
            }
            else
            {
                addCodePairToOutput(1, 7);
            }
        }
        else if (next == '<') // D2 state
        {
            if (input.front() == '>') // D2' state
            {
                next = getCharacterFromInput();
                addCodePairToOutput(1, 14);
            }
            else if (input.front() == '=') // D2'' state
            {
                next = getCharacterFromInput();
                addCodePairToOutput(1, 13);
            }
            else
            {
                addCodePairToOutput(1, 12);
            }
        }
        else if (next == '>') // D3 state
        {
            if (input.front() == '=') // D3' state
            {
                next = getCharacterFromInput();
                addCodePairToOutput(1, 17);
            }
            else
            {
                addCodePairToOutput(1, 16);
            }
        }
        else if (next == ':') // D4 state
        {
            if (input.front() == '=') // D4' state
            {
                next = getCharacterFromInput();
                addCodePairToOutput(1, 10);
            }
            else
            {
                addCodePairToOutput(1, 9);
            }
        }
        else if (
            index = binarySearch(analysis->separators, 0, analysis->separators.size() - 1, std::string{ next });
            index != -1)
        {
            addCodePairToOutput(1, index);
        }
        else if ((next != '\t' && next != '\n' && next != ' ') && (next <= 'A' || next >= 'z' || next != 'd') && (next <= '0' || next >= '9')
            && index == -1 && next != -1)
        {
            // устанавливается флаг ошибки, если разделитель не был найден
            analysis->errorCode = 2;
        }
    }
}

// gc
char LexicalAnalysis::getCharacterFromInput()
{
    if (input.empty()) return -1;

    char next = input.front();
    input.erase(input.begin());
    return next;
}

void LexicalAnalysis::addCodePairToOutput(int tableNum, int indexNum)
{
    lexicalOutput.push_back(*(new CodePair(tableNum, indexNum)));

    std::string tableStr = std::to_string(tableNum).append(", ");
    std::string indexStr = std::to_string(indexNum).append(") ");
    std::string out = "(";
    out.append(tableStr);
    out.append(indexStr);
    analysis->textEditOutput->insertPlainText(QString::fromStdString(out));
}

// I state
void LexicalAnalysis::IdentifierState(std::string& buffer, char& next)
{
    buffer.push_back(next);
    while (next = getCharacterFromInput(), (next >= 'A' && next <= 'z' || next >= '0' && next <= '9')) // буква или цифра
    {
        buffer.push_back(next);
    }

    if (int index = binarySearch(analysis->keywords, 0, analysis->keywords.size() - 1, buffer); index != -1)
    {
        if (buffer == "end") // пока что оставляю дополнительное условие на end, если потребуется явное указание окончания программы
        {
            addCodePairToOutput(0, index);
        }
        else
        {
            addCodePairToOutput(0, index);
        }
    }
    else
    {
        if (int index = binarySearch(analysis->separators, 0, analysis->separators.size() - 1, buffer); index != -1)
        {
            addCodePairToOutput(1, index);
        }
        else
        {
            // сортировка, чтобы дальнейший поиск осуществлялся в отсортированных идентификаторах
            if (analysis->identifiers.size() > 1) quickSortIterative(analysis->identifiers, 0, analysis->identifiers.size() - 1);
            index = binarySearch(analysis->identifiers, 0, analysis->identifiers.size() - 1, buffer);

            if (index == -1)
            {
                analysis->identifiers.push_back(buffer);
                analysis->addItemToTableWidget(analysis->identifiersTable, buffer);
                addCodePairToOutput(3, analysis->identifiers.size() - 1);
            }
            else addCodePairToOutput(3, index);
        }
    }
}

// B state
void LexicalAnalysis::BinaryState(std::string& buffer, char& next)
{
    buffer.push_back(next);
    while (next = getCharacterFromInput(), (next == '0' || next == '1')) // 1 или 0
    {
        buffer.push_back(next);
    }

    if (next == 'B' || next == 'b') // состояние B'
    {
        buffer.push_back(next);
        next = getCharacterFromInput();
        if ((next >= 'A' && next <= 'F') || (next >= 'a' && next <= 'f') || (next >= '0' && next <= '9')) // переход к 16сс
        {
            HexadecimalState(buffer, next);
        }
        else if (next == 'H' || next == 'h') // переход в состояние 16* осуществляется в состоянии 16сс
        {
            HexadecimalState(buffer, next);
        }
        else if (std::string nextStr = { next }; binarySearch(analysis->separators, 0, analysis->separators.size() - 1, nextStr) != -1 ||
            next == '\t' || next == '\n' || next == ' ')  // в случае встречи разделителя, либо пробела, число формируется и выводится
        {
            analysis->numbers.push_back(buffer);
            analysis->addItemToTableWidget(analysis->numbersTable, buffer);
            addCodePairToOutput(2, analysis->numbers.size() - 1);
        }
        else // ошибка в случае любого другого встречного символа
        {
            analysis->errorCode = 1; // ошибка формирования числа
        }
    }
    else if (next >= '2' && next <= '7') // переход в состояние 8сс
    {
        OctalState(buffer, next);
    }
    else if (next == '8' || next == '9') // переход в состояние 10сс
    {
        DecimalState(buffer, next);
    }
    else if (next == '.') // переход в состояние Вещ.
    {
        RealState(buffer, next);
    }
    else if (next == 'E' || next == 'e') // переход в состояние Э.ф.
    {
        ExponentialFormState(buffer, next);
    }
    else if (next == 'O' || next == 'o') // переход в состояние 8* осуществляется в состоянии 8сс
    {
        OctalState(buffer, next);
    }
    else if (next == 'D' || next == 'd') // переход в состояние 10* осуществляется в состоянии 10сс
    {
        DecimalState(buffer, next);
    }
    else if (next == 'A' || next == 'C' || next == 'F' ||
        next == 'a' || next == 'c' || next == 'f') // переход в состояние 16сс
    {
        HexadecimalState(buffer, next);
    }
    else if (next == 'H' || next == 'h') // переход в состояние 16* осуществляется в состоянии 16сс
    {
        HexadecimalState(buffer, next);
    }
    else if (std::string nextStr = { next }; binarySearch(analysis->separators, 0, analysis->separators.size() - 1, nextStr) != -1 ||
        next == '\t' || next == '\n' || next == ' ')  // в случае встречи разделителя, либо пробела, число формируется и выводится
    {
        analysis->numbers.push_back(buffer);
        analysis->addItemToTableWidget(analysis->numbersTable, buffer);
        addCodePairToOutput(2, analysis->numbers.size() - 1);
    }
    else // ошибка в случае любого другого встречного символа
    {
        analysis->errorCode = 1; // ошибка формирования числа
    }
}

// 8сс state
void LexicalAnalysis::OctalState(std::string& buffer, char& next)
{
    // Если мы пришли сразу в состояние 8*
    if (next == 'O' || next == 'o')
    {
        buffer.push_back(next);
        next = getCharacterFromInput();
        if (std::string nextStr = { next }; binarySearch(analysis->separators, 0, analysis->separators.size() - 1, nextStr) != -1 ||
            next == '\t' || next == '\n' || next == ' ')  // в случае встречи разделителя, либо пробела, число формируется и выводится
        {
            analysis->numbers.push_back(buffer);
            analysis->addItemToTableWidget(analysis->numbersTable, buffer);
            addCodePairToOutput(2, analysis->numbers.size() - 1);
        }
        else // ошибка в случае любого другого встречного символа
        {
            analysis->errorCode = 1; // ошибка формирования числа
        }
        return;
    }

    buffer.push_back(next);
    while (next = getCharacterFromInput(), (next >= '0' && next <= '7')) // от 0 до 7
    {
        buffer.push_back(next);
    }

    if (next == 'O' || next == 'o') // состояние 8*
    {
        buffer.push_back(next);
        next = getCharacterFromInput();
        if (std::string nextStr = { next }; binarySearch(analysis->separators, 0, analysis->separators.size() - 1, nextStr) != -1 ||
            next == '\t' || next == '\n' || next == ' ')  // в случае встречи разделителя, либо пробела, число формируется и выводится
        {
            analysis->numbers.push_back(buffer);
            analysis->addItemToTableWidget(analysis->numbersTable, buffer);
            addCodePairToOutput(2, analysis->numbers.size() - 1);
        }
        else // ошибка в случае любого другого встречного символа
        {
            analysis->errorCode = 1; // ошибка формирования числа
        }
    }
    else if (next == '8' || next == '9') // переход в состояние 10сс
    {
        DecimalState(buffer, next);
    }
    else if (next == '.') // переход в состояние Вещ.
    {
        RealState(buffer, next);
    }
    else if (next == 'E' || next == 'e') // переход в состояние Э.ф.
    {
        ExponentialFormState(buffer, next);
    }
    else if (next == 'D' || next == 'd') // переход в состояние 10* осуществляется в состоянии 10сс
    {
        DecimalState(buffer, next);
    }
    else if (next == 'A' || next == 'B' || next == 'C' || next == 'F' ||
        next == 'a' || next == 'b' || next == 'c' || next == 'f') // переход в состояние 16сс
    {
        HexadecimalState(buffer, next);
    }
    else if (next == 'H' || next == 'h') // переход в состояние 16* осуществляется в состоянии 16сс
    {
        HexadecimalState(buffer, next);
    }
    else if (std::string nextStr = { next }; binarySearch(analysis->separators, 0, analysis->separators.size() - 1, nextStr) != -1 ||
        next == '\t' || next == '\n' || next == ' ')  // в случае встречи разделителя, либо пробела, число формируется и выводится
    {
        analysis->numbers.push_back(buffer);
        analysis->addItemToTableWidget(analysis->numbersTable, buffer);
        addCodePairToOutput(2, analysis->numbers.size() - 1);
    }
    else // ошибка в случае любого другого встречного символа
    {
        analysis->errorCode = 1; // ошибка формирования числа
    }
}

// 10сс state
void LexicalAnalysis::DecimalState(std::string& buffer, char& next)
{
    // Если мы пришли сразу в состояние 10*
    if (next == 'D' || next == 'd')
    {
        buffer.push_back(next);
        next = getCharacterFromInput();
        if ((next >= 'A' && next <= 'F') || (next >= 'a' && next <= 'f') || (next >= '0' && next <= '9')) // переход к 16сс
        {
            HexadecimalState(buffer, next);
        }
        else if (next == 'H' || next == 'h') // переход в состояние 16* осуществляется в состоянии 16сс
        {
            HexadecimalState(buffer, next);
        }
        else if (std::string nextStr = { next }; binarySearch(analysis->separators, 0, analysis->separators.size() - 1, nextStr) != -1 ||
            next == '\t' || next == '\n' || next == ' ')  // в случае встречи разделителя, либо пробела, число формируется и выводится
        {
            analysis->numbers.push_back(buffer);
            analysis->addItemToTableWidget(analysis->numbersTable, buffer);
            addCodePairToOutput(2, analysis->numbers.size() - 1);
        }
        else // ошибка в случае любого другого встречного символа
        {
            analysis->errorCode = 1; // ошибка формирования числа
        }
        return;
    }

    buffer.push_back(next);
    while (next = getCharacterFromInput(), (next >= '0' && next <= '9')) // от 0 до 9
    {
        buffer.push_back(next);
    }

    if (next == 'D' || next == 'd')
    {
        buffer.push_back(next);
        next = getCharacterFromInput();
        if ((next >= 'A' && next <= 'F') || (next >= 'a' && next <= 'f') || (next >= '0' && next <= '9')) // переход к 16сс
        {
            HexadecimalState(buffer, next);
        }
        else if (next == 'H' || next == 'h') // переход в состояние 16* осуществляется в состоянии 16сс
        {
            HexadecimalState(buffer, next);
        }
        else if (std::string nextStr = { next }; binarySearch(analysis->separators, 0, analysis->separators.size() - 1, nextStr) != -1 ||
            next == '\t' || next == '\n' || next == ' ')  // в случае встречи разделителя, либо пробела, число формируется и выводится
        {
            analysis->numbers.push_back(buffer);
            analysis->addItemToTableWidget(analysis->numbersTable, buffer);
            addCodePairToOutput(2, analysis->numbers.size() - 1);
        }
        else // ошибка в случае любого другого встречного символа
        {
            analysis->errorCode = 1; // ошибка формирования числа
        }
    }
    else if (next == '.') // переход в состояние Вещ.
    {
        RealState(buffer, next);
    }
    else if (next == 'E' || next == 'e') // переход в состояние Э.ф.
    {
        ExponentialFormState(buffer, next);
    }
    else if (next == 'A' || next == 'B' || next == 'C' || next == 'F' ||
        next == 'a' || next == 'b' || next == 'c' || next == 'f') // переход в состояние 16сс
    {
        HexadecimalState(buffer, next);
    }
    else if (next == 'H' || next == 'h') // переход в состояние 16* осуществляется в состоянии 16сс
    {
        HexadecimalState(buffer, next);
    }
    else if (std::string nextStr = { next }; binarySearch(analysis->separators, 0, analysis->separators.size() - 1, nextStr) != -1 ||
        next == '\t' || next == '\n' || next == ' ')  // в случае встречи разделителя, либо пробела, число формируется и выводится
    {
        analysis->numbers.push_back(buffer);
        analysis->addItemToTableWidget(analysis->numbersTable, buffer);
        addCodePairToOutput(2, analysis->numbers.size() - 1);
    }
    else // ошибка в случае любого другого встречного символа
    {
        analysis->errorCode = 1; // ошибка формирования числа
    }
}

// Вещ. state
void LexicalAnalysis::RealState(std::string& buffer, char& next)
{
    buffer.push_back(next);
    next = getCharacterFromInput();

    if (next >= '0' && next <= '9')
    {
        buffer.push_back(next);
        while (next = getCharacterFromInput(), (next >= '0' && next <= '9')) // от 0 до 9
        {
            buffer.push_back(next);
        }

        if (next == 'E' || next == 'e') // переход в состояние Э.ф.*
        {
            ExponentialFormExtraState(buffer, next);
        }
        else if (std::string nextStr = { next }; binarySearch(analysis->separators, 0, analysis->separators.size() - 1, nextStr) != -1 ||
            next == '\t' || next == '\n' || next == ' ')  // в случае встречи разделителя, либо пробела, число формируется и выводится
        {
            analysis->numbers.push_back(buffer);
            analysis->addItemToTableWidget(analysis->numbersTable, buffer);
            addCodePairToOutput(2, analysis->numbers.size() - 1);
        }
        else // ошибка в случае любого другого встречного символа
        {
            analysis->errorCode = 1; // ошибка формирования числа
        }
    }
    else // ошибка в случае любого другого встречного символа
    {
        analysis->errorCode = 1; // ошибка формирования числа
    }
}

// Э.ф. state
void LexicalAnalysis::ExponentialFormState(std::string& buffer, char& next)
{
    buffer.push_back(next);
    next = getCharacterFromInput();

    if (next == '+' || next == '-') // + или -
    {
        buffer.push_back(next);
        while (next = getCharacterFromInput(), (next >= '0' && next <= '9')) // от 0 до 9
        {
            buffer.push_back(next);
        }

        if (std::string nextStr = { next }; binarySearch(analysis->separators, 0, analysis->separators.size() - 1, nextStr) != -1 ||
            next == '\t' || next == '\n' || next == ' ')  // в случае встречи разделителя, либо пробела, число формируется и выводится
        {
            analysis->numbers.push_back(buffer);
            analysis->addItemToTableWidget(analysis->numbersTable, buffer);
            addCodePairToOutput(2, analysis->numbers.size() - 1);
        }
        else
        {
            analysis->errorCode = 1; // ошибка формирования числа
        }
    }
    else if (next >= '0' && next <= '9') // от 0 до 9
    {
        buffer.push_back(next);
        while (next = getCharacterFromInput(), (next >= '0' && next <= '9')) // от 0 до 9
        {
            buffer.push_back(next);
        }

        if ((next >= 'A' && next <= 'F') || (next >= 'a' && next <= 'f')) // переход к 16сс
        {
            HexadecimalState(buffer, next);
        }
        else if (next == 'H' || next == 'h') // переход в состояние 16* осуществляется в состоянии 16сс
        {
            HexadecimalState(buffer, next);
        }
        else if (std::string nextStr = { next }; binarySearch(analysis->separators, 0, analysis->separators.size() - 1, nextStr) != -1 ||
            next == '\t' || next == '\n' || next == ' ')  // в случае встречи разделителя, либо пробела, число формируется и выводится
        {
            analysis->numbers.push_back(buffer);
            analysis->addItemToTableWidget(analysis->numbersTable, buffer);
            addCodePairToOutput(2, analysis->numbers.size() - 1);
        }
        else
        {
            analysis->errorCode = 1; // ошибка формирования числа
        }
    }
    else if ((next >= 'A' && next <= 'F') || (next >= 'a' && next <= 'f'))
    {
        HexadecimalState(buffer, next);
    }
    else if (next == 'H' || next == 'h')
    {
        HexadecimalState(buffer, next);
    }
    else
    {
        analysis->errorCode = 1; // ошибка формирования числа
    }
}

// Э.ф.* state
void LexicalAnalysis::ExponentialFormExtraState(std::string& buffer, char& next)
{
    buffer.push_back(next);
    next = getCharacterFromInput();

    if ((next >= '0' && next <= '9') || next == '+' || next == '-') // от 0 до 9, + или -
    {
        buffer.push_back(next);
        while (next = getCharacterFromInput(), (next >= '0' && next <= '9')) // от 0 до 9
        {
            buffer.push_back(next);
        }

        if (std::string nextStr = { next }; binarySearch(analysis->separators, 0, analysis->separators.size() - 1, nextStr) != -1 ||
            next == '\t' || next == '\n' || next == ' ')  // в случае встречи разделителя, либо пробела, число формируется и выводится
        {
            analysis->numbers.push_back(buffer);
            analysis->addItemToTableWidget(analysis->numbersTable, buffer);
            addCodePairToOutput(2, analysis->numbers.size() - 1);
        }
        else // ошибка в случае любого другого встречного символа
        {
            analysis->errorCode = 1; // ошибка формирования числа
        }
    }
    else // ошибка в случае любого другого встречного символа
    {
        analysis->errorCode = 1; // ошибка формирования числа
    }
}

// 16сс state
void LexicalAnalysis::HexadecimalState(std::string& buffer, char& next)
{
    // Если мы пришли сразу в состояние 16*
    if (next == 'H' || next == 'h')
    {
        buffer.push_back(next);
        next = getCharacterFromInput();

        if (std::string nextStr = { next }; binarySearch(analysis->separators, 0, analysis->separators.size() - 1, nextStr) != -1 ||
            next == '\t' || next == '\n' || next == ' ')  // в случае встречи разделителя, либо пробела, число формируется и выводится
        {
            analysis->numbers.push_back(buffer);
            analysis->addItemToTableWidget(analysis->numbersTable, buffer);
            addCodePairToOutput(2, analysis->numbers.size() - 1);
        }
        else // ошибка в случае любого другого встречного символа
        {
            analysis->errorCode = 1; // ошибка формирования числа
        }
        return;
    }

    buffer.push_back(next);
    while (next = getCharacterFromInput(), (next >= '0' && next <= '9') ||
        (next >= 'A' && next <= 'F') || (next >= 'a' && next <= 'f')) // от 0 до 9 или A..f
    {
        buffer.push_back(next);
    }

    if (next == 'H' || next == 'h')
    {
        buffer.push_back(next);
        next = getCharacterFromInput();

        if (std::string nextStr = { next }; binarySearch(analysis->separators, 0, analysis->separators.size() - 1, nextStr) != -1 ||
            next == '\t' || next == '\n' || next == ' ')  // в случае встречи разделителя, либо пробела, число формируется и выводится
        {
            analysis->numbers.push_back(buffer);
            analysis->addItemToTableWidget(analysis->numbersTable, buffer);
            addCodePairToOutput(2, analysis->numbers.size() - 1);
        }
        else // ошибка в случае любого другого встречного символа
        {
            analysis->errorCode = 1; // ошибка формирования числа
        }
        return;
    }
    else
    {
        analysis->errorCode = 1; // ошибка формирования числа
    }
}