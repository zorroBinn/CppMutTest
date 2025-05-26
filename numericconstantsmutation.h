#pragma once
#include <QFile>
#include <QTextStream>
#include <QRegularExpression>
#include <QRandomGenerator>
#include <QDebug>
#include "IMutation.h"

///
/// \brief The NumericConstantsMutation class - мутация числовых констант
///
class NumericConstantsMutation : public IMutation
{
public:
    void mutate(const QString &filePath) override;

    bool isAvailable(const QString &filePath) override;
};
