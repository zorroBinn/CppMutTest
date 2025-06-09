#include "arithmeticmutation.h"

void ArithmeticMutation::mutate(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return;
    }
    QString original = QTextStream(&file).readAll();
    file.close();

    QString masked = maskContent(original);

    QRegularExpression rxUnary(R"((\+\+|--))"); //Инкремент/декремент
    QRegularExpression rxCompound(R"((\+=|-=|\*=|/=|%=))"); //Составные операторы
    QRegularExpression rxSingle(R"((?<![\w+\-*/%])([+\-*/%])(?![+\-*/%=]))"); //Односимвольные операторы

    auto unaryOps = findOps(masked, rxUnary);
    auto compoundOps = findOps(masked, rxCompound);
    auto singleOps = findOps(masked, rxSingle);

    struct Op {
        int position;
        QString op;
        QStringList group;
    };

    QVector<Op> all;
    if (!unaryOps.isEmpty()) {
        QStringList list = { "++", "--" };
        for (const auto &p : unaryOps) {
            all.append({ p.first, p.second, list });
        }
    }
    if (!compoundOps.isEmpty()) {
        QStringList list = { "+=", "-=", "*=", "/=", "%=" };
        for (const auto &p : compoundOps) {
            all.append({ p.first, p.second, list });
        }
    }
    if (!singleOps.isEmpty()) {
        QStringList list = { "+", "-", "*", "/", "%" };
        for (const auto &p : singleOps) {
            all.append({ p.first, p.second, list });
        }
    }

    if (all.isEmpty()) {
        return;
    }

    int idx = QRandomGenerator::global()->bounded(all.size());
    const Op &chosen = all.at(idx);

    QStringList opts = chosen.group;
    opts.removeAll(chosen.op);
    QString replacement = opts.at(QRandomGenerator::global()->bounded(opts.size()));

    QString mutated = original;
    mutated.replace(chosen.position, chosen.op.length(), replacement);

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return;
    }
    QTextStream(&file) << mutated;
    file.close();
}

bool ArithmeticMutation::isAvailable(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return false;
    }

    QString content = QTextStream(&file).readAll();
    file.close();

    QString masked = maskContent(content);

    QRegularExpression rxUnary(R"((\+\+|--))"); //Инкремент/декремент
    QRegularExpression rxCompound(R"((\+=|-=|\*=|/=|%=))"); //Составные операторы
    QRegularExpression rxSingle(R"((?<![\w+\-*/%])([+\-*/%])(?![+\-*/%=]))"); //Односимвольные операторы

    if (!masked.contains(rxUnary) && !masked.contains(rxCompound) && !masked.contains(rxSingle)) {
        return false;
    }

    return true;
}
