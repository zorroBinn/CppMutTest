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
/// \brief The ExceptionsMutation class - мутация исключений и обработчиков исключений
///
class ExceptionsMutation : public IMutation
{
protected:
    QVector<QPair<int, QString>> findOps(const QString &masked, const QRegularExpression &rx) const override;

public:
    void mutate(const QString &filePath) override;

    bool isAvailable(const QString &filePath) override;
};
