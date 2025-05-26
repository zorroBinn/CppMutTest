#pragma once
#include <QVector>
#include "listofsupportedmutations.h"
#include "listofsupportedcompilers.h"
#include "listofsupportedtestframeworks.h"
#include "userclassinfo.h"

///
/// \brief The ConfigSubsystem class - подсистема конфигурации
///
class ConfigSubsystem
{
private:
    QString folderPath; //Путь к пользовательской папке проекта
    UserClassInfo selectedClass; //Выбранный для анализа класс
    QVector<SupportedMutations> selectedMutation; //Список выбранных типов мутаций
    unsigned short numberOfMutations; //Лимит на количество мутаций
    SupportedCompilers compiler; //Выбранный компилятор
    SupportedTestFrameworks testFramework; //Используемый тестовый фреймворк
    QStringList sourceFiles; //Список .cpp файлов проекта
    unsigned short threadDiv; //Ограничитель используемых ядер процессора - делитель для количества логических ядер

public:
    ///
    /// \brief ConfigSubsystem::ConfigSubsystem - конструктор с параметрами
    /// \param folderPath - путь к пользовательской папке проекта
    /// \param selectedClass - выбранный для анализа класс
    /// \param selectedMutation - список выбранных типов мутаций
    /// \param numberOfMutations - лимит на количество мутаций
    /// \param compiler - выбранный компилятор
    /// \param testFramework - используемый тестовый фреймворк
    /// \param sourceFiles - список .cpp файлов проекта
    ///
    ConfigSubsystem(const QString &folderPath, const UserClassInfo &selectedClass,
                    const QVector<SupportedMutations> &selectedMutation,
                    unsigned short numberOfMutations, SupportedCompilers compiler,
                    SupportedTestFrameworks testFramework, const QStringList &sourceFiles, unsigned short threadDiv);

    ///
    /// \brief getFolderPath - возвращает путь к пользовательской папке проекта
    /// \return QString - абсолютный путь к папке проекта
    ///
    QString getFolderPath() const;
    ///
    /// \brief setFolderPath - задаёт путь к пользовательской папке проекта
    /// \param path - абсолютный путь к папке проекта
    ///
    void setFolderPath(const QString &path);

    ///
    /// \brief getSelectedClass - возвращает выбранный для анализа класс
    /// \return UserClassInfo - выбранный класс
    ///
    UserClassInfo getSelectedClass() const;
    ///
    /// \brief setSelectedClass - задаёт выбранный для анализа класс
    /// \param userClass - экземпляр UserClassInfo
    ///
    void setSelectedClass(const UserClassInfo &userClass);

    ///
    /// \brief getSelectedMutations - возвращает список выбранных типов мутаций
    /// \return QVector<SupportedMutations> - список мутаций
    ///
    QVector<SupportedMutations> getSelectedMutations() const;
    ///
    /// \brief setSelectedMutations - задаёт список выбранных мутаций
    /// \param mutations - список мутаций
    ///
    void setSelectedMutations(const QVector<SupportedMutations> &mutations);

    ///
    /// \brief getNumberOfMutations - возвращает лимит на количество мутаций
    /// \return unsigned short - количество мутаций
    ///
    unsigned short getNumberOfMutations() const;
    ///
    /// \brief setNumberOfMutations - задаёт лимит на количество мутаций
    /// \param number - количество мутаций
    ///
    void setNumberOfMutations(unsigned short number);

    ///
    /// \brief getCompiler - возвращает выбранный компилятор
    /// \return SupportedCompilers - выбранный компилятор
    ///
    SupportedCompilers getCompiler() const;
    ///
    /// \brief setCompiler - задаёт выбранный компилятор
    /// \param compiler - компилятор
    ///
    void setCompiler(SupportedCompilers compiler);

    ///
    /// \brief getTestFramework - возвращает используемый тестовый фреймворк
    /// \return SupportedTestFrameworks - фреймворк тестирования
    ///
    SupportedTestFrameworks getTestFramework() const;
    ///
    /// \brief setTestFramework - задаёт используемый тестовый фреймворк
    /// \param framework - фреймворк тестирования
    ///
    void setTestFramework(SupportedTestFrameworks framework);

    ///
    /// \brief getSourceFiles - возвращает список абсолютных путей .cpp файлов проекта
    /// \return QStringList - список абсолютных путей .cpp файлов
    ///
    QStringList getSourceFiles() const;
    ///
    /// \brief setSourceFiles - задаёт список абсолютных путей .cpp файлов проекта
    /// \param files - список абсолютных путей .cpp файлов
    ///
    void setSourceFiles(const QStringList &files);

    ///
    /// \brief getThreadDiv - возвращает ограничитель (делитель) на используемые ядра процессора
    /// \return unsigned short - 1 (100% логических ядер), 2(50% - логических ядер)...
    ///
    unsigned short getThreadDiv() const;

    ///
    /// \brief setThreadDiv - задаёт ограничитель (делитель) на используемые ядра процессора
    /// \param newThreadDiv - 1 (100% логических ядер), 2(50% - логических ядер)...
    ///
    void setThreadDiv(unsigned short newThreadDiv);
};
