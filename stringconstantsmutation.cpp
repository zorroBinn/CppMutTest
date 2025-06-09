#include "stringconstantsmutation.h"

QString StringConstantsMutation::maskContent(const QString &src) const
{
    QString masked = src;

    QRegularExpression singleComment(R"(//[^\n]*)");
    QRegularExpression multiComment(R"(/\*.*?\*/)", QRegularExpression::DotMatchesEverythingOption);
    QRegularExpression directive(R"(^\s*#.*$)", QRegularExpression::MultilineOption);

    for (const auto &rx : { singleComment, multiComment, directive }) {
        auto it = rx.globalMatch(masked);
        while (it.hasNext()) {
            auto match = it.next();
            masked.replace(match.capturedStart(), match.capturedLength(), QString(match.capturedLength(), ' '));
        }
    }

    return masked;
}

QVector<QPair<int, QString>> StringConstantsMutation::findOps(const QString &masked, const QRegularExpression &rx) const
{
    QVector<QPair<int, QString>> out;
    auto it = rx.globalMatch(masked);
    while (it.hasNext()) {
        auto m = it.next();
        out.append({ m.capturedStart(0), m.captured(0) });
    }
    return out;
}

QVector<QPair<int, QString> > StringConstantsMutation::filterStringsInsideRawStrings(const QVector<QPair<int, QString> > &stringOps,
                                                                                    const QVector<QPair<int, QString> > &rawOps)
{
    //Создание множества позиций, которые находятся внутри сырых строк
    QSet<int> rawOpsPositions;
    for (const auto &rawOp : rawOps) {
        int start = rawOp.first;
        int end = start + rawOp.second.length();
        for (int position = start; position < end; position++) {
            rawOpsPositions.insert(position);
        }
    }

    //Фильтр stringOps, с исключением тех, которые находятся внутри сырых строк
    QVector<QPair<int, QString>> filteredStringOps;
    for (const auto &strOp : stringOps) {
        bool insideRawString = false;
        int start = strOp.first;
        int end = start + strOp.second.length();

        //Проверка пересечения текущей строки с сырой строкой
        for (int position = start; position < end; position++) {
            if (rawOpsPositions.contains(position)) {
                insideRawString = true;
                break;
            }
        }

        if (!insideRawString) {
            filteredStringOps.append(strOp);
        }
    }

    return filteredStringOps;
}

void StringConstantsMutation::mutate(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return;
    }

    QString original = QTextStream(&file).readAll();
    file.close();

    QString masked = maskContent(original);

    QRegularExpression rxString(R"("([^"\\]|\\.)*")"); //Поиск обычных строк "..."

    //Поиск сырых строк R"delimiter(...)delimiter"
    QRegularExpression rxRaw(R"(R"([^ ()\\]{0,16})\((.*?)\)\1")", QRegularExpression::DotMatchesEverythingOption);

    QVector<QPair<int, QString>> stringOps = findOps(masked, rxString);
    QVector<QPair<int, QString>> rawOps = findOps(masked, rxRaw);

    QVector<QPair<int, QString>> filteredStringOps = filterStringsInsideRawStrings(stringOps, rawOps);

    struct Op {
        int position;
        QString original;
        QString replacement;
    };

    QVector<Op> all;

    for (const auto &p : filteredStringOps) {
        QString replacement;
        //Проверка пустой строки
        bool isEmpty = (p.second == "\"\"");
        if (!isEmpty && QRandomGenerator::global()->bounded(2) == 0) {
            replacement = "\"\"";
        }
        else {
            int len = QRandomGenerator::global()->bounded(1, 101);
            QString randomStr;
            for (int i = 0; i < len; i++) {
                QChar ch;
                do {
                    ch = QChar(' ' + QRandomGenerator::global()->bounded(95));
                } while (ch == '"' || ch == '\\');
                randomStr.append(ch);
            }
            replacement = "\"" + randomStr + "\"";
        }
        all.append({ p.first, p.second, replacement });
    }

    for (const auto &p : rawOps) {
        QString delimiterMatch;
        QRegularExpression delimiterExtract(R"(^R"([^ ()\\]{0,16})\()");
        QRegularExpressionMatch match = delimiterExtract.match(p.second);
        if (match.hasMatch()) {
            delimiterMatch = match.captured(1);
        }
        else {
            continue;
        }

        QString replacement;
        //Проверка пустой сырой строки
        bool isEmptyRaw = QRegularExpression(QString(R"(^R"(%1)\(\)\1")").arg(QRegularExpression::escape(delimiterMatch))).match(p.second).hasMatch();
        if (!isEmptyRaw && QRandomGenerator::global()->bounded(2) == 0) {
            replacement = "R\"" + delimiterMatch + "()" + delimiterMatch + "\"";
        }
        else {
            int len = QRandomGenerator::global()->bounded(1, 101);
            QString randomStr;
            for (int i = 0; i < len; i++) {
                QChar ch = QChar(' ' + QRandomGenerator::global()->bounded(95));
                randomStr.append(ch);
            }
            replacement = "R\"" + delimiterMatch + "(" + randomStr + ")" + delimiterMatch + "\"";
        }
        all.append({ p.first, p.second, replacement });
    }

    if (all.isEmpty()) {
        return;
    }

    int idx = QRandomGenerator::global()->bounded(all.size());
    Op chosen = all.at(idx);

    QString mutated = original;
    mutated.replace(chosen.position, chosen.original.length(), chosen.replacement);

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return;
    }

    QTextStream(&file) << mutated;
    file.close();
}

bool StringConstantsMutation::isAvailable(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return false;
    }

    QString content = QTextStream(&file).readAll();
    file.close();

    QString masked = maskContent(content);

    if (!masked.contains(QRegularExpression(R"("([^"\\]|\\.)*")"))) {
        return false;
    }

    return true;
}
