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
/// \brief The LogicalMutation class - мутация логических операторов
///
class LogicalMutation : public IMutation
{
protected:
    QString maskContent(const QString &src) const override;

public:
    void mutate(const QString &filePath) override;

    bool isAvailable(const QString &filePath) override;
};
