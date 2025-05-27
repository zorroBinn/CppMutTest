#pragma once
#include <QPair>
#include <QVector>
#include <QFile>
#include <QTextStream>
#include <QRegularExpression>
#include <QRandomGenerator>
#include <QDebug>
#include "IMutation.h"

///
/// \brief The StringConstantsMutation class - мутация строковых констант
///
class StringConstantsMutation : public IMutation
{
protected:
    QString maskContent(const QString &src) const override;

    QVector<QPair<int, QString>> findOps(const QString &masked, const QRegularExpression &rx) const override;

private:
    ///
    /// \brief filterStringsInsideRawStrings - Исключает найденные обычные строки из сырых строк
    /// \param stringOps - Список обнаруженных обычных строк
    /// \param rawOps - Список обнаруженных сырых строк
    /// \return QVector<QPair<int, QString>> - Отфильтрованный список обычных строк
    ///
    QVector<QPair<int, QString>> filterStringsInsideRawStrings(const QVector<QPair<int, QString>> &stringOps,
                                                                const QVector<QPair<int, QString>> &rawOps);

public:
    void mutate(const QString &filePath) override;

    bool isAvailable(const QString &filePath) override;
};
