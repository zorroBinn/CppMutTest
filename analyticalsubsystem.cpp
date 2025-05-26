#include "analyticalsubsystem.h"

MutantStates AnalyticalSubsystem::calculateMutantState(const QMap<QString, QString> &testResults) const
{
    //Если результаты тестов пусты, мутант считается убитым
    if (testResults.isEmpty()) {
        return MutantStates::Dead;
    }

    //Мутант убит, если хотя бы один тест не пройден
    for (const QString &result : testResults) {
        if (result == "FAILED") {
            return MutantStates::Dead;
        }
    }

    //Если все тесты пройдены или результаты неизвестны, мутант выжил
    return MutantStates::Survived;
}

AnalyticalSubsystem::AnalyticalSubsystem(const QMap<QString, SupportedMutations> &mutantsTypes,
                                         const QMap<QString, QMap<QString, QString> > &mutantTestResults)
    : mutantsTypes(mutantsTypes), mutantTestResults(mutantTestResults)
{
    //Инициализация состояний мутантов
    for (auto it = mutantTestResults.constBegin(); it != mutantTestResults.constEnd(); it++) {
        mutantStates[it.key()] = calculateMutantState(it.value());
    }
}

double AnalyticalSubsystem::calculateMSI() const
{
    if (mutantTestResults.isEmpty()) {
        return 0.0;
    }

    int deadMutants = 0;
    for (const MutantStates &state : mutantStates) {
        if (state == MutantStates::Dead) {
            deadMutants++;
        }
    }

    //Вычисление MSI c округлением до одного знака
    double msi = (static_cast<double>(deadMutants) / mutantTestResults.size()) * 100.0;
    return std::round(msi * 10.0) / 10.0;
}

double AnalyticalSubsystem::calculateMSIByMutationType(SupportedMutations mutationType) const
{
    int totalMutants = 0;
    int deadMutants = 0;

    for (auto it = mutantsTypes.constBegin(); it != mutantsTypes.constEnd(); it++) {
        if (it.value() == mutationType) {
            totalMutants++;
            if (mutantStates.value(it.key()) == MutantStates::Dead) {
                deadMutants++;
            }
        }
    }

    if (totalMutants == 0) {
        return 0.0;
    }

    //Вычисление MSI по типу мутации c округлением до одного знака
    double msi = (static_cast<double>(deadMutants) / totalMutants) * 100.0;
    return std::round(msi * 10.0) / 10.0;
}

QMap<QString, MutantStates> AnalyticalSubsystem::getMutantStates() const
{
    return mutantStates;
}


