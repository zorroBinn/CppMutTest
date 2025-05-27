#pragma once
#include <QHash>
#include <QMap>
#include <QString>
#include "listofsupportedmutations.h"
#include "listofmutantstates.h"
#include "mutationmanagersubsystem.h"

///
/// \brief The AnalyticalSubsystem class - аналитическая подсистема
///
class AnalyticalSubsystem
{
private:
    QMap<QString, SupportedMutations> mutantsTypes; //Словарь <путь к мутанту, тип мутации>
    QMap<QString, QHash<QString, QString>> mutantTestResults; //Словарь <путь к мутанту, результаты тестов>
    QHash<QString, MutantStates> mutantStates; //Словарь <путь к мутанту, состояние>

    ///
    /// \brief calculateMutantState - определяет состояние мутанта
    /// \param testResults - результаты тестов для мутанта
    /// \return MutantStates - состояние мутанта
    ///
    MutantStates calculateMutantState(const QHash<QString, QString> &testResults) const;

public:
    ///
    /// \brief Конструктор AnalyticalSubsystem
    /// \param mutantsTypes - словарь путей к мутантам и их типов мутаций
    /// \param mutantTestResults - словарь путей к мутантам и их результатов тестов
    ///
    AnalyticalSubsystem(const QMap<QString, SupportedMutations> &mutantsTypes,
                        const QMap<QString, QHash<QString, QString>> &mutantTestResults);

    ///
    /// \brief calculateMSI - вычисляет общий Mutation Score Indicator (MSI)
    /// \return double - MSI округленный до одного знака после запятой
    ///
    double calculateMSI() const;

    ///
    /// \brief calculateMSIByMutationType - вычисляет MSI для определенного типа мутации
    /// \param mutationType - тип мутации
    /// \return double - MSI для типа мутации, округленный до одного знака после запятой
    ///
    double calculateMSIByMutationType(SupportedMutations mutationType) const;

    ///
    /// \brief getMutantStates - возвращает словарь состояний мутантов
    /// \return QHash<QString, MutantStates> - состояния мутантов
    ///
    QHash<QString, MutantStates> getMutantStates() const;
};
