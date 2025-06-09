#include "exceptionsmutation.h"

QVector<QPair<int, QString> > ExceptionsMutation::findOps(const QString &masked, const QRegularExpression &rx) const
{
    QVector<QPair<int, QString>> out;
    auto it = rx.globalMatch(masked);
    while (it.hasNext()) {
        auto match = it.next();
        out.append({ match.capturedStart(0), match.captured(0) });
    }
    return out;
}

void ExceptionsMutation::mutate(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return;
    }

    QString original = QTextStream(&file).readAll();
    file.close();

    QString masked = maskContent(original);

    //Поиск catch(...) { ... }
    QRegularExpression catchRx(R"(catch\s*\([^\)]*\)\s*\{)");
    QVector<QPair<int, int>> catchBlocks;
    auto catchIt = catchRx.globalMatch(masked);
    while (catchIt.hasNext()) {
        auto match = catchIt.next();
        int start = match.capturedStart(0);
        int braceStart = masked.indexOf('{', start);
        if (braceStart == -1) {
            continue;
        }

        int position = braceStart + 1;
        int depth = 1;
        while (position < masked.size() && depth > 0) {
            if (masked[position] == '{') {
                ++depth;
            }
            else if (masked[position] == '}') {
                --depth;
            }
            position++;
        }

        if (depth == 0) {
            catchBlocks.append({ braceStart + 1, position - braceStart - 2 }); //Исключение {}
        }
    }

    //Поиск throw ...;
    QRegularExpression throwRx(R"(\bthrow\b)");
    QVector<QPair<int, int>> throwExprs;
    auto throwIt = throwRx.globalMatch(masked);
    while (throwIt.hasNext()) {
        auto match = throwIt.next();
        int start = match.capturedStart(0);
        int position = start;
        bool found = false;
        while (position < masked.size()) {
            if (masked[position] == ';') {
                found = true;
                break;
            }
            position++;
        }
        if (found && position > start) {
            throwExprs.append({ start, position - start }); //Оставляем пустой оператор ;
        }
    }

    struct Op {
        int position;
        int len;
    };

    QVector<Op> all;
    for (const auto &p : catchBlocks) {
        all.append({ p.first, p.second });
    }
    for (const auto &p : throwExprs) {
        all.append({ p.first, p.second });
    }

    if (all.isEmpty()) {
        return;
    }

    int idx = QRandomGenerator::global()->bounded(all.size());
    Op chosen = all.at(idx);

    QString mutated = original;
    mutated.replace(chosen.position, chosen.len, QString(chosen.len, ' '));

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return;
    }

    QTextStream(&file) << mutated;
    file.close();
}

bool ExceptionsMutation::isAvailable(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return false;
    }

    QString content = QTextStream(&file).readAll();
    file.close();

    QString masked = maskContent(content);

    if (!masked.contains(QRegularExpression(R"(\b(throw|catch)\b)"))) {
        return false;
    }

    return true;
}
