#pragma once
#include <QString>
#include <QStringList>
#include "listofsupportedcompilers.h"
#include "listofsupportedtestframeworks.h"

///
/// \brief The ICompiler class - интерфейс компилятора для сборки исходных файлов
///
class ICompiler
{
public:
    virtual ~ICompiler() = default;

    ///
    /// \brief isAvailable - проверяет установлен ли компилятор
    /// \return true - доступен, false - недоступен
    ///
    virtual bool isAvailable() const = 0;

    ///
    /// \brief getName - возвращает название компилятора
    /// \return QString - строка названия компилятора
    ///
    virtual QString getName() const = 0;

    ///
    /// \brief getNameFromSupportedCompilers - возвращает вариант компилятора из supportedcompilers
    /// \return SupportedCompilers - вариант компилятора
    ///
    virtual SupportedCompilers getNameFromSupportedCompilers() const = 0;

    ///
    /// \brief compile - компилирует исходники и файлы с юнит-тестами для выбранного класса
    /// \param folderPath - путь выбранной папки с приложением пользователя
    /// \param sourceFiles - .cpp файлы приложения, исключая main
    /// \param testFiles - .cpp файлы с юнит-тестами для выбранного класса
    /// \param outputExecutablePath - путь для результирующего исполняемого файла
    /// \param compileOutput - строка для вывода компилятора
    /// \return true - компиляция успешна, fause - ошибка компиляции
    ///
    virtual bool compile(const QString &folderPath, const QStringList &sourceFiles,
                         const QStringList &testFiles, const QString &outputExecutablePath,
                         QString &compileOutput, SupportedTestFrameworks framework) = 0;
};
