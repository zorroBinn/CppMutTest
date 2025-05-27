#pragma once
#include <QHash>
#include <QString>
#include <QVector>
#include <QDir>
#include "listofsupportedmutations.h"

///
/// \brief The UserClassInfo class - информация об одном пользовательском классе, обнаруженном в исходном коде проекта
///
class UserClassInfo
{
private:
    QString className; //Название пользовательского класса
    QString hFilePath; //Абсолютный путь к заголовочному файлу
    QString cppFilePath; //Абсолютный путь к файлу реализации
    QVector<QString> testFilePaths; //Пути ко всем файлам, в которых содержатся тесты на данный класс
    QVector<QString> methodNames; //Названия методов класса
    QVector<QString> unitTestNames; //Названия юнит-тестов класса
    QVector<SupportedMutations> availableMutations; //Доступные операторы мутаций для класса
    QHash<QString, QString> originalTestsResults; //Оригинальные результаты тестов

public:
    ///
    /// \brief UserClassInfo - конструктор с параметром
    /// \param className - название пользовательского класса
    ///
    UserClassInfo(const QString &className);

    ///
    /// \brief hasUnitTests - проверяет, есть ли тесты
    /// \return - true, если хотя бы один тест найден
    ///
    bool hasUnitTests() const;

    ///
    /// \brief getClassName - возвращает название пользовательского класса
    /// \return QString - название класса
    ///
    QString getClassName() const;
    ///
    /// \brief setClassName - задаёт название класса
    /// \param name - название класса
    ///
    void setClassName(const QString &name);

    ///
    /// \brief getHFilePath - возвращает абсолютный путь к .h файлу
    /// \return QString - абсолютный путь к файлу реализации
    ///
    QString getHFilePath() const;
    ///
    /// \brief setHFilePath - задаёт путь к .h файлу с реализацией
    /// \param path - абсолютный путь к файлу
    ///
    void setHFilePath(const QString &path);

    ///
    /// \brief getCppFilePath - возвращает абсолютный путь к .cpp файлу
    /// \return QString - абсолютный путь к файлу реализации
    ///
    QString getCppFilePath() const;
    ///
    /// \brief setCppFilePath - задаёт путь к .cpp файлу с реализацией
    /// \param path - абсолютный путь к файлу
    ///
    void setCppFilePath(const QString &path);

    ///
    /// \brief getTestFilePaths - возвращает список путей для файлов с юнит-тестами
    /// \return QVector<QString> - список абсолютных путей
    ///
    QVector<QString> getTestFilePaths() const;
    ///
    /// \brief setTestFilePaths - задаёт список с путями до тестовых файлов
    /// \param paths - список абсолютных путей
    ///
    void setTestFilePaths(const QVector<QString> &paths);
    ///
    /// \brief addTestFilePath - добавляет один путь к файлу теста
    /// \param path - абсолютный путь к тестовому файлу
    ///
    void addTestFilePath(const QString &path);

    ///
    /// \brief getMethodNames - возвращает список названий методов класса
    /// \return QVector<QString> - список названий методов
    ///
    QVector<QString> getMethodNames() const;
    ///
    /// \brief setMethodNames - задаёт список с названиями методов класса
    /// \param names - названия методов
    ///
    void setMethodNames(const QVector<QString> &names);
    ///
    /// \brief addMethodName - добавляет одно название метода класса
    /// \param name - название метода
    ///
    void addMethodName(const QString &name);

    ///
    /// \brief getUnitTestNames - возвращает список названий тестов, связанных с этим классом
    /// \return QVector<QString> - список названий юнит-тестов
    ///
    QVector<QString> getUnitTestNames() const;
    ///
    /// \brief setUnitTestNames - задаёт список названий тестов
    /// \param names - список названий тестов
    ///
    void setUnitTestNames(const QVector<QString> &names);
    ///
    /// \brief addUnitTestName - добавляет название одного теста
    /// \param name - название юнит-теста
    ///
    void addUnitTestName(const QString &name);

    ///
    /// \brief getAvailableMutations - возвращает список доступных для класса операторов мутаций
    /// \return QVector<SupportedMutations> - список операторов мутаций
    ///
    QVector<SupportedMutations> getAvailableMutations() const;
    ///
    /// \brief setAvailableMutations - задаёт список доступных операторов мутаций
    /// \param mutations - список операторов мутаций
    ///
    void setAvailableMutations(const QVector<SupportedMutations> &mutations);

    ///
    /// \brief getOriginalTestsResults - возвращает список оригинальных результатов тестов для класса
    /// \return QHash<QString, QString> - результаты тестов для класса
    ///
    QHash<QString, QString> getOriginalTestsResults() const;
    ///
    /// \brief setOriginalTestsResults - задаёт список оригинальных результатов тестов для класса
    /// \param newOriginalTestsResults - результаты тестов для класса
    ///
    void setOriginalTestsResults(const QHash<QString, QString> &newOriginalTestsResults);
};
