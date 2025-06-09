#include "numericconstantsmutation.h"

#define MAX_SHORT_VALUE 32768
#define MAX_INT_PART_USED_VALUE 1'000'000'000'000
#define MAX_FRAC_PART_USED_VALUE 1'000

void NumericConstantsMutation::mutate(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return;
    }

    QString original = QTextStream(&file).readAll();
    file.close();

    QString masked = maskContent(original);

    //Числа: целые десятичные и вещественные
    QRegularExpression rxNumbers(R"((\b\d{1,3}(?:'\d{3})*(?:\.\d+)?\b))");

    QVector<QPair<int, QString>> foundNumbers;
    auto it = rxNumbers.globalMatch(masked);
    while (it.hasNext()) {
        auto match = it.next();
        int start = match.capturedStart(1);
        QString value = match.captured(1);

        //Игнорируем, если подстрока
        bool insideExisting = false;
        for (const auto &existing : foundNumbers) {
            int exStart = existing.first;
            int exEnd = exStart + existing.second.length();
            if (start > exStart && start < exEnd) {
                insideExisting = true;
                break;
            }
        }
        if (!insideExisting) {
            foundNumbers.append({start, value});
        }
    }

    if (foundNumbers.isEmpty()) {
        return;
    }

    int idx = QRandomGenerator::global()->bounded(foundNumbers.size());
    auto chosen = foundNumbers.at(idx);

    QString replacement;
    bool isFloat = chosen.second.contains('.');
    bool isZero = (chosen.second == "0" || chosen.second == "0.0" || chosen.second == "0.00");

    if (!isZero && QRandomGenerator::global()->bounded(2) == 0) {
        replacement = "0";
    }
    else {
        if (isFloat) {
            quint64 intPartVal = QRandomGenerator::global()->bounded(static_cast<quint64>(1), static_cast<quint64>(MAX_INT_PART_USED_VALUE));
            int fracPartVal = QRandomGenerator::global()->bounded(MAX_FRAC_PART_USED_VALUE);
            QString replacementIntPart = QString::number(intPartVal);
            QString replacementFracPart = QString::number(fracPartVal);
            replacement = replacementIntPart + "." + replacementFracPart;
        }
        else {
            int val = QRandomGenerator::global()->bounded(1, MAX_SHORT_VALUE);
            replacement = QString::number(val);
        }
    }

    QString mutated = original;
    mutated.replace(chosen.first, chosen.second.length(), replacement);

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return;
    }

    QTextStream(&file) << mutated;
    file.close();
}

bool NumericConstantsMutation::isAvailable(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return false;
    }

    QString content = QTextStream(&file).readAll();
    file.close();

    QString masked = maskContent(content);

    if (!masked.contains(QRegularExpression(R"(\b\d+(\.\d+)?\b)"))) {
        return false;
    }

    return true;
}
