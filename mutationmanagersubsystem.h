#pragma once
#include <QHash>
#include <QMap>
#include <QSet>
#include <QString>
#include <QRandomGenerator>
#include <QMutex>
#include <QFutureSynchronizer>
#include <QThreadPool>
#include <QtConcurrent>
#include <atomic>
#include <QFutureWatcher>
#include "configsubsystem.h"
#include "buildandrunsubsystem.h"
#include "listofsupportedmutations.h"
#include "arithmeticmutation.h"
#include "logicalmutation.h"
#include "stringconstantsmutation.h"
#include "numericconstantsmutation.h"
#include "exceptionsmutation.h"

///
/// \brief The MutationManagerSubsystem class - подсистема управления мутациями
///
class MutationManagerSubsystem
{
private:
    QString workingDirectoryPath; //Абсолютный путь к рабочей директории "CppMutTest_WorkingDirectory"
    QMap<QString, SupportedMutations> mutantsTypes; //Словарь <абсолютный путь к мутанту, тип мутации>
    QMap<QString, QHash<QString, QString>> mutantTestResults; //Словарь <абсолютный путь к мутанту, результаты тестов>
    ConfigSubsystem* config; //Ссылка на объект ConfigSubsystem для получения информации
    QMutex mutex; //Мьютекс для синхронизации доступа к mutantTestResults
    QMutex logMutex; //Мьютекс для синхронизации доступа к log
    std::atomic<bool> isStopping{false}; //Флаг для остановки процесса мутационного анализа
    QSet<QString> processedMutants; //Контейнер для путей обработанных мутантов

    ///
    /// \brief createWorkingDirectory - создаёт рабочую директорию "CppMutTest_WorkingDirectory"
    /// \param log - ссылка для логгирования
    ///
    bool createWorkingDirectory(QStringList &log);

    ///
    /// \brief distributeMutants - создаёт поддиректории и распределяет типы мутаций
    /// \param log - ссылка для логгирования
    ///
    bool distributeMutants(QStringList &log);

    ///
    /// \brief generateMutants - генерирует мутанты
    /// \param log - ссылка для логгирования
    ///
    void generateMutants(QStringList &log);

    ///
    /// \brief runTestsOnMutants - запускает тесты на каждом мутанте
    /// \param log - ссылка для логгирования
    ///
    void runTestsOnMutants(QStringList &log);

    ///
    /// \brief cleanupWorkingDirectory - удаляет рабочую директорию и всё её содержимое
    ///
    void cleanupWorkingDirectory();

    ///
    /// \brief cleanupWorkingDirectory - удаляет директории незавершенных мутантов из рабочей директории
    ///
    void cleanupUnanalyzedMutants();

public:
    ///
    /// \brief MutationManagerSubsystem - конструктор с параметрами
    /// \param config - указатель на объект ConfigSubsystem
    /// \param log - ссылка для логгирования
    ///
    MutationManagerSubsystem(ConfigSubsystem *config, QStringList &log);

    ~MutationManagerSubsystem();

    ///
    /// \brief startMutationProcess - запускает процесс мутационного анализа
    /// \param log - ссылка для логгирования
    ///
    bool startMutationProcess(QStringList &log);

    ///
    /// \brief getMutantTestResults - возвращает словарь с результами выполнения тестов для всех мутантов
    /// \return QMap<QString, QHash<QString, QString>> - результаты тестов для мутантов
    ///
    QMap<QString, QHash<QString, QString>> getMutantTestResults() const;

    ///
    /// \brief getWorkingDirectoryPath - возвращает абсолютный путь к рабочей директории
    /// \return QString - абсолютный путь к рабочей директории
    ///
    QString getWorkingDirectoryPath() const;

    ///
    /// \brief getMutantsTypes - возвращает словарь с типом каждого мутанта
    /// \return QMap<QString, SupportedMutations> - словарь <абсолютный путь мутанта, тип>
    ///
    QMap<QString, SupportedMutations> getMutantsTypes() const;

    ///
    /// \brief stopMutationProcess - останавливает процесс мутационного анализа
    ///
    void stopMutationProcess();
};
