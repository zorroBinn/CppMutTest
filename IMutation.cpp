#include "IMutation.h"

QString IMutation::maskContent(const QString &src) const
{
    QString masked = src;

    QRegularExpression singleComment(R"(//[^\n]*)");
    QRegularExpression multiComment(R"(/\*.*?\*/)", QRegularExpression::DotMatchesEverythingOption);
    QRegularExpression directive(R"(^\s*#.*$)", QRegularExpression::MultilineOption);
    QRegularExpression stringLiteral(R"("([^"\\]|\\.)*")");

    for (const auto &rx : { singleComment, multiComment, directive, stringLiteral }) {
        auto it = rx.globalMatch(masked);
        while (it.hasNext()) {
            auto match = it.next();
            masked.replace(match.capturedStart(), match.capturedLength(), QString(match.capturedLength(), ' '));
        }
    }

    return masked;
}

QVector<QPair<int, QString>> IMutation::findOps(const QString &masked, const QRegularExpression &rx) const
{
    QVector<QPair<int, QString>> out;
    auto it = rx.globalMatch(masked);

    while (it.hasNext()) {
        auto m = it.next();
        if (m.capturedStart(1) >= 0 && !m.captured(1).isEmpty()) {
            out.append({ m.capturedStart(1), m.captured(1) });
        }
    }

    return out;
}
