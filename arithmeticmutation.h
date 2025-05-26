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
/// \brief The ArithmeticMutation class - мутация арифметических операторов
///
class ArithmeticMutation : public IMutation
{
public:
    void mutate(const QString &filePath) override;

    bool isAvailable(const QString &filePath) override;
};
