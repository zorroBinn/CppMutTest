#pragma once
#include <QListWidgetItem>
#include <QStackedWidget>
#include <QProgressDialog>
#include <QFutureWatcher>
#include <QtConcurrent>
#include <QFileDialog>
#include <QMessageBox>
#include <QSet>
#include <QHash>
#include <QMap>
#include <QScrollBar>
#include "configsubsystem.h"
#include "documentation.h"
#include "listofsupportedcompilers.h"
#include "mutationmanagersubsystem.h"
#include "sourcecodeanalysissubsystem.h"
#include "analyticalsubsystem.h"
#include "buildandrunsubsystem.h"

namespace Ui {
class MainCppMutTest;
}

class MainCppMutTest : public QStackedWidget
{
    Q_OBJECT

public:
    explicit MainCppMutTest(QWidget *parent = nullptr);
    ~MainCppMutTest();

private slots:
    ///
    /// \brief on_pushButton_goToSettingsAndLogs_2_clicked - Переключает на страницу настроек и логов
    ///
    void on_pushButton_goToSettingsAndLogs_2_clicked();

    ///
    /// \brief on_pushButton_goToAnalisis_1_clicked - Переключает на страницу анализа и заполняет список классов
    ///
    void on_pushButton_goToAnalisis_1_clicked();

    ///
    /// \brief on_lineEdit_folderPath_editingFinished - Обрабатывает введенный путь к папке проекта
    ///
    void on_lineEdit_folderPath_editingFinished();

    ///
    /// \brief on_pushButton_selectFolder_clicked - Открывает диалог для выбора папки проекта
    ///
    void on_pushButton_selectFolder_clicked();

    ///
    /// \brief on_radioButton_selectGtest_toggled - Обрабатывает выбор фреймворка Google Test
    /// \param checked - Указывает, выбран ли переключатель
    ///
    void on_radioButton_selectGtest_toggled(bool checked);

    ///
    /// \brief on_radioButton_selectBoostTest_toggled - Обрабатывает выбор фреймворка Boost Test
    /// \param checked - Указывает, выбран ли переключатель
    ///
    void on_radioButton_selectBoostTest_toggled(bool checked);

    ///
    /// \brief on_radioButton_gpp_toggled - Обрабатывает выбор компилятора g++
    /// \param checked - Указывает, выбран ли переключатель
    ///
    void on_radioButton_gpp_toggled(bool checked);

    ///
    /// \brief on_radioButton_clangpp_toggled - Обрабатывает выбор компилятора clang++
    /// \param checked - Указывает, выбран ли переключатель
    ///
    void on_radioButton_clangpp_toggled(bool checked);

    ///
    /// \brief on_radioButton_orig_toggled - Переключает между отображением исходного кода и мутанта
    /// \param checked - Указывает, выбран ли исходный код
    ///
    void on_radioButton_orig_toggled(bool checked);

    ///
    /// \brief on_listWidget_userClasses_itemClicked - Обрабатывает выбор пользовательского класса для анализа
    /// \param item - Выбранный элемент класса в списке
    ///
    void on_listWidget_userClasses_itemClicked(QListWidgetItem *item);

    ///
    /// \brief on_listWidget_mutants_itemClicked - Обрабатывает выбор мутанта для анализа
    /// \param item - Выбранный элемент мутанта в списке
    ///
    void on_listWidget_mutants_itemClicked(QListWidgetItem *item);

    ///
    /// \brief on_horizontalSlider_mutationCount_valueChanged - Обновляет количество мутаций из ползунка
    /// \param value - Новое значение ползунка
    ///
    void on_horizontalSlider_mutationCount_valueChanged(int value);

    ///
    /// \brief on_spinBox_mutationCount_valueChanged - Обновляет количество мутаций из счетчика
    /// \param arg1 - Новое значение счетчика
    ///
    void on_spinBox_mutationCount_valueChanged(int arg1);

    ///
    /// \brief on_pushButton_runTestsOnSRC_clicked - Запускает модульные тесты на исходном коде
    ///
    void on_pushButton_runTestsOnSRC_clicked();

    ///
    /// \brief on_pushButton_zoomIn_clicked - Увеличивает масштаб отображения исходного кода
    ///
    void on_pushButton_zoomIn_clicked();

    ///
    /// \brief on_pushButton_zoomOut_clicked - Уменьшает масштаб отображения исходного кода
    ///
    void on_pushButton_zoomOut_clicked();

    ///
    /// \brief on_pushButton_startMut_clicked - Запускает процесс мутационного анализа
    ///
    void on_pushButton_startMut_clicked();

    ///
    /// \brief onMutationAnalysisFinished - Обрабатывает завершение мутационного анализа
    ///
    void onMutationAnalysisFinished();

    ///
    /// \brief on_radioButton_fullThread_toggled - Обрабатывает выбор 100% логических ядер процессора для использования
    /// \param checked - Указывает, выбран ли переключатель
    ///
    void on_radioButton_fullThread_toggled(bool checked);

    ///
    /// \brief on_radioButton_halfThread_toggled - Обрабатывает выбор 50% логических ядер процессора для использования
    /// \param checked - Указывает, выбран ли переключатель
    ///
    void on_radioButton_halfThread_toggled(bool checked);

private:
    Ui::MainCppMutTest *ui; //Указатель на форму пользовательского интерфейса
    Documentation *docWindow = nullptr; //Указатель на окно документации
    QStringList log; //Список сообщений логов
    QString folderPath; //Путь к выбранной папке проекта
    SupportedCompilers selectedCompiler; //Текущий выбранный компилятор
    QString selectedClassForAnalysis; //Имя класса, выбранного для анализа
    QString currentClass; //Имя текущего отображаемого класса
    QString currentMutant; //Путь к текущему отображаемому мутанту
    SourceCodeAnalysisSubsystem *srcAnalysis = nullptr; //Подсистема анализа исходного кода
    ConfigSubsystem *config = nullptr; //Подсистема управления конфигурацией
    MutationManagerSubsystem *mutationManager = nullptr; //Подсистема управления мутациями
    unsigned short threadDiv = 1; //Ограничитель используемых ядер процессора - делитель для количества логических ядер

    ///
    /// \brief cleanWorkingSpace - Сбрасывает рабочее пространство в начальное состояние
    ///
    void cleanWorkingSpace();

    ///
    /// \brief cleanSelectedMutationsTypes - Очищает выбранные типы мутаций в интерфейсе
    ///
    void cleanSelectedMutationsTypes();

    ///
    /// \brief cleanMSI - Сбрасывает отображение индикатора мутационного счета (MSI)
    ///
    void cleanMSI();

    ///
    /// \brief openDocumentation - Открывает или активирует окно документации
    ///
    void openDocumentation();

    ///
    /// \brief openAndCheckNewFolder - Проверяет и устанавливает новую папку проекта
    /// \param folder - Путь к проверяемой папке
    ///
    void openAndCheckNewFolder(const QString &folder);

    ///
    /// \brief checkConditionsForAnalysis - Проверяет выполнение условий для анализа
    ///
    void checkConditionsForAnalysis();

    ///
    /// \brief displaySRC - Отображает исходный код из файла в текстовом браузере
    /// \param cppFilePath - Путь к файлу .cpp
    ///
    void displaySRC(const QString &cppFilePath);

    ///
    /// \brief displayTests - Отображает результаты тестов для класса или мутанта
    /// \param identifier - Имя класса или путь к мутанту
    /// \param isMutant - True, если отображаются тесты мутанта
    ///
    void displayTests(const QString &identifier, bool isMutant);

    ///
    /// \brief updateMutationCheckboxesForClass - Обновляет флажки мутаций на основе возможностей класса
    /// \param userClass - Информация о пользовательском классе
    ///
    void updateMutationCheckboxesForClass(const UserClassInfo &userClass);

    ///
    /// \brief showMSIAndUpdateUI - Отображает MSI и обновляет интерфейс результатами анализа
    ///
    void showMSIAndUpdateUI();

    ///
    /// \brief compareAndHighlightCode - Сравнивает и выделяет различия между исходным кодом и мутантом
    /// \param originalPath - Путь к исходному файлу .cpp
    /// \param mutantPath - Путь к файлу мутанта .cpp
    ///
    void compareAndHighlightCode(const QString &originalPath, const QString &mutantPath);

    ///
    /// \brief updateLogs - Обновляет отображение логов с форматированными сообщениями
    ///
    void updateLogs();
};
