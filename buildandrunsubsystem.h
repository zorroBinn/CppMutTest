#pragma once
#include <QString>
#include <QStringList>
#include <QMap>
#include <QProcess>
#include <QTextStream>
#include <QRegularExpression>
#include <QXmlStreamReader>
#include "listofsupportedcompilers.h"
#include "listofsupportedtestframeworks.h"
#include "listofcompilationresults.h"
#include "gppcompiler.h"
#include "clangcompiler.h"

///
/// \brief The BuildAndRunSubsystem class - подсистема сборки и запуска
///
class BuildAndRunSubsystem
{
private:
    QMap<QString, QString> testResults; //Словарь <название теста, результат>

    ///
    /// \brief runExecutable - выполняет запуск исполняемого файла
    /// \param executablePath - путь к исполняемому файлу (.exe)
    /// \param output - строка, куда будет записан stdout исполняемой программы
    /// \param framework - используемый тестовый фреймворк
    /// \return true - если запуск завершился успешно
    ///
    bool runExecutable(const QString &executablePath, QString &output, SupportedTestFrameworks framework);

    ///
    /// \brief parseGoogleTestOutput - парсит вывод для Google Test
    /// \param xmlPath - абсолютный путь к .xml файлу с результатами
    ///
    void parseGoogleTestOutput(const QString &xmlPath);

    ///
    /// \brief parseBoostTestOutput - парсит вывод для Boost.Test
    /// \param output - вывод программы после запуска
    /// \param stream - поток для чтения вывода
    ///
    void parseBoostTestOutput(const QString &output);

    ///
    /// \brief parseTestOutput - парсит вывод stdout и извлекает результаты выполнения тестов
    /// \param output - вывод программы после запуска
    /// \param framework - используемый тестовый фреймворк
    /// \param executablePath - абсолютный путь к .xml файлу с результатами для gtest
    ///
    void parseTestOutput(const QString &output, SupportedTestFrameworks framework, const QString &executablePath);

public:
    ///
    /// \brief BuildAndRunSubsystem - конструктор по умолчанию
    ///
    BuildAndRunSubsystem();

    ///
    /// \brief getTestResults - возвращает словарь <название теста - результат>
    /// \return QMap<QString, QString> - результаты выполнения тестов
    ///
    QMap<QString, QString> getTestResults() const;

    ///
    /// \brief compilerIsAvailable - проверяет доступность компилятора
    /// \param compiler - тип компилятора
    /// \param log - ссылка для логгирования
    /// \return true - если доступен
    ///
    bool compilerIsAvailable(SupportedCompilers compiler, QStringList &log);

    ///
    /// \brief buildAndRun - компилирует и запускает тесты над проектом
    /// \param folderPath - путь выбранной папки с приложением пользователя
    /// \param sourceFiles - .cpp файлы приложения, исключая main
    /// \param testFiles - .cpp файлы с юнит-тестами для выбранного класса
    /// \param outputExecutablePath - путь для результирующего исполняемого файла
    /// \param compileOutput - строка для вывода компилятора
    /// \param compilerType - используемый компилятор
    /// \param framework - используемый фреймворк тестирования
    /// \param log - ссылка для логгирования
    /// \return true - если сборка и запуск прошли успешно
    ///
    CompilationResults buildAndRun(const QString &folderPath, const QStringList &sourceFiles,
                     const QStringList &testFiles, const QString &outputExecutablePath,
                     QString &compileOutput, SupportedCompilers compilerType,
                     SupportedTestFrameworks framework, QStringList &log);
};
