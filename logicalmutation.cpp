#include "logicalmutation.h"

QString LogicalMutation::maskContent(const QString &src) const
{
    QString masked = src;

    QRegularExpression singleComment(R"(//[^\n]*)");
    QRegularExpression multiComment(R"(/\*.*?\*/)", QRegularExpression::DotMatchesEverythingOption);
    QRegularExpression directive(R"(^\s*#.*$)", QRegularExpression::MultilineOption);
    QRegularExpression stringLiteral(R"("([^"\\]|\\.)*")");
    QRegularExpression templateRx(R"(\b\w+\s*<([^\n<>]*?(<[^<>]*?>[^\n<>]*?)?)>)");

    for (const auto &rx : { singleComment, multiComment, directive, stringLiteral, templateRx }) {
        auto it = rx.globalMatch(masked);
        while (it.hasNext()) {
            auto match = it.next();
            masked.replace(match.capturedStart(), match.capturedLength(), QString(match.capturedLength(), ' '));
        }
    }

    return masked;
}

void LogicalMutation::mutate(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return;
    }

    QString original = QTextStream(&file).readAll();
    file.close();

    QString masked = maskContent(original);

    QRegularExpression rxCmp(R"((?<![\w<>])(==|!=|>=|<=|>|<)(?![\w<>]))"); //Сравнения
    QRegularExpression rxLogic(R"((\&\&|\|\|))"); //И/ИЛИ
    QRegularExpression rxNot(R"((!\s*\w))"); //NOT

    auto cmpOps = findOps(masked, rxCmp);
    auto logicOps = findOps(masked, rxLogic);

    QVector<QPair<int, QString>> notOps;
    auto it = rxNot.globalMatch(masked);
    while (it.hasNext()) {
        auto match = it.next();
        notOps.append({ match.capturedStart(0), "!" });
    }

    struct Op {
        int position;
        QString op;
        QStringList group;
    };

    QVector<Op> all;
    if (!cmpOps.isEmpty()) {
        QStringList group = { "==", "!=", ">=", "<=", ">", "<" };
        for (auto &p : cmpOps) {
            all.append({ p.first, p.second, group });
        }
    }
    if (!logicOps.isEmpty()) {
        QStringList group = { "&&", "||" };
        for (auto &p : logicOps) {
            all.append({ p.first, p.second, group });
        }
    }
    if (!notOps.isEmpty()) {
        QStringList group = { "" }; //Удаление
        for (auto &p : notOps) {
            all.append({ p.first, p.second, group });
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

bool LogicalMutation::isAvailable(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return false;
    }

    QString content = QTextStream(&file).readAll();
    file.close();

    QString masked = maskContent(content);

    QRegularExpression rxCmp(R"((?<![\w<>])(==|!=|>=|<=|>|<)(?![\w<>]))"); //Сравнения
    QRegularExpression rxLogic(R"((\&\&|\|\|))"); //И/ИЛИ
    QRegularExpression rxNot(R"((!\s*\w))"); //NOT

    if (!content.contains(rxCmp) && !content.contains(rxLogic) && !content.contains(rxNot)) {
        return false;
    }

    return true;
}
