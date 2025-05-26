#pragma once
#include <QString>
#include <QVector>
#include <QDir>
#include <QFileInfoList>
#include <QFile>
#include <QRegularExpression>
#include <QTextStream>
#include <QDebug>
#include <QFuture>
#include <QMutex>
#include <QtConcurrent>
#include "userclassinfo.h"
#include "listofsupportedtestframeworks.h"
#include "buildandrunsubsystem.h"
#include "arithmeticmutation.h"
#include "logicalmutation.h"
#include "stringconstantsmutation.h"
#include "numericconstantsmutation.h"
#include "exceptionsmutation.h"

///
/// \brief The SourceCodeAnalysisSubsystem class - подсистема анализа исходного кода
///
class SourceCodeAnalysisSubsystem
{
private:
    QString folderPath; //Абсолютный путь выбранной пользователем папки с проектом
    QString folderTestsPath; //Абсолютный путь к папке с тестами ./tests/ для пользовательского проекта
    QVector<UserClassInfo> userClasses; //Классы, обнаруженные в пользовательском проекте
    SupportedTestFrameworks testFramework; //Название определенного фреймворка тестирования

    ///
    /// \brief removeClass - удаляет класс из userClasses по его названию
    /// \param name - название класса
    ///
    void removeClass(const QString &name);

    ///
    /// \brief extractClassNames - поиск и выделение классов в выбранной пользователем папке с проектом
    ///
    void extractClassNames();

    ///
    /// \brief associateCppFilesWithClasses - сопоставляет .cpp файлы с найденными классами
    /// \param log - ссылка для логгирования
    ///
    void associateCppFilesWithClasses(QStringList &log);

    ///
    /// \brief identifyTestsForClasses - поиск файлов с юнит-тестами и выделение юнит-тестов для классов
    /// \param log - ссылка для логгирования
    ///
    void identifyTestsForClasses(QStringList &log);

    ///
    /// \brief removeClassesWithoutTests - удаляет из userClasses классы без тестов
    /// \param log - ссылка для логгирования
    ///
    void removeClassesWithoutTests(QStringList &log);

    ///
    /// \brief identifyMethods - поиск методов в классах
    ///
    void identifyMethods();

    ///
    /// \brief analyzeAvailableMutations - определяет доступные операторы мутаций в классах
    ///
    void analyzeAvailableMutationsForClasses();

    ///
    /// \brief removeClassesWithMultipleBoostTestFiles
    /// \param log - ссылка для логгирования
    ///
    void removeClassesWithMultipleBoostTestFiles(QStringList &log);

public:
    ///
    /// \brief SourceCodeAnalysisSubsystem - конструктор с параметром
    /// \param folder - абсолютный путь к выбранной пользователем папке с проектом
    ///
    SourceCodeAnalysisSubsystem(const QString &folder);

    ///
    /// \brief getFolderPath - возвращает абсолютный путь к папке с проектом пользователя
    /// \return QString - абсолютный путь
    ///
    QString getFolderPath() const;

    ///
    /// \brief getFolderTestsPath - возвращает абсолютный путь к папке с тестами ./tests/ проекта пользователя
    /// \return - абсолютный путь
    ///
    QString getFolderTestsPath() const;

    ///
    /// \brief getUserClasses - возвращает список классов проекта пользователя
    /// \return QVector<UserClassInfo> - список классов
    ///
    QVector<UserClassInfo> getUserClasses() const;

    ///
    /// \brief getTestFramework - возвращает название фреймворка тестирования, используемого в тестах проекта пользователя
    /// \return QString - название фреймворка тестирования
    ///
    SupportedTestFrameworks getTestFramework() const;

    ///
    /// \brief setTestFramework - задаёт используемый пользователем тестовый фреймворк
    /// \param framework - тестовый фреймворк
    ///
    void setTestFramework(SupportedTestFrameworks framework);

    ///
    /// \brief isProjectStructureValid - проверяет наличие исходных файлов и папки с тестами
    /// \param folder - путь к проектной папке
    /// \param log - ссылка для логгирования
    /// \return - true, если структура проекта корректна
    ///
    bool isProjectStructureValid(const QString &folder, QStringList &log);

    ///
    /// \brief analyzeSourceFiles - анализирует исходный код проекта, создаёт объекты UserClassInfo и заполняет их
    /// \param log - ссылка для логгирования
    /// \return - true, если классы были успешно проанализированы
    ///
    bool analyzeSourceFiles(QStringList &log);

    ///
    /// \brief isTestFrameworkValid - проверяет валидность указанного пользователем тестового фреймворка
    /// \param log - ссылка для логгирования
    /// \return true, если проверка пройдена
    ///
    bool isTestFrameworkValid(QStringList &log) const;

    ///
    /// \brief runOriginalTests - запускает тесты на классах оригинального проекта
    /// \param buildRun - экземпляр системы сборки и запуска
    /// \param compiler - выбранный компилятор
    /// \param log - ссылка для логгирования
    /// \return - true, если тестирование завершено успешно
    ///
    bool runOriginalTests(SupportedCompilers compiler, QStringList &log);
};
