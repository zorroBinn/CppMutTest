#include "maincppmuttest.h"
#include "ui_maincppmuttest.h"

MainCppMutTest::MainCppMutTest(QWidget *parent)
    : QStackedWidget(parent)
    , ui(new Ui::MainCppMutTest)
{
    setWindowIcon(QIcon(":/new/icons/CMT.ico"));
    ui->setupUi(this);
    setCurrentIndex(0);
    QHeaderView* header = ui->tableWidget_tests->horizontalHeader();
    header->setSectionResizeMode(0, QHeaderView::Stretch);
    header->setSectionResizeMode(1, QHeaderView::Fixed);
    ui->tableWidget_tests->setColumnWidth(1, 150);

    connect(ui->pushButton_Doc_1, &QPushButton::clicked, this, &MainCppMutTest::openDocumentation);
    connect(ui->pushButton_Doc_2, &QPushButton::clicked, this, &MainCppMutTest::openDocumentation);
}

MainCppMutTest::~MainCppMutTest()
{
    delete ui;
    if (srcAnalysis) {
        delete srcAnalysis;
    }
    if (config) {
        delete config;
    }
    if (mutationManager) {
        delete mutationManager;
    }
}

void MainCppMutTest::on_pushButton_goToSettingsAndLogs_2_clicked()
{
    setCurrentIndex(0);
}

void MainCppMutTest::on_pushButton_goToAnalisis_1_clicked()
{
    if(!srcAnalysis || srcAnalysis->getFolderPath().isEmpty() || srcAnalysis->getTestFramework() == SupportedTestFrameworks::None) {
        QMessageBox::warning(this, "Предварительные настройки не завершены!",
                             "Выберите корректную директорию с проектом, тестовый фреймворк и компилятор. ",
                             "Ознакомьтесь с документацией.");
        return;
    }
    if (srcAnalysis->getUserClasses().isEmpty()) {
        if (!srcAnalysis->analyzeSourceFiles(log)) {
            QMessageBox::warning(this, "Ошибка анализа исходного кода!",
                                 "Не обнаружено ни одного класса для мутационного анализа. ",
                                 "Проверьте логи и ознакомьтесь с документацией.");
            updateLogs();
            return;
        }

        updateLogs();

        QVector<UserClassInfo> classes = srcAnalysis->getUserClasses();
        for (const UserClassInfo& userClass : classes) {
            QListWidgetItem *item = new QListWidgetItem(userClass.getClassName());
            item->setData(Qt::UserRole, userClass.getCppFilePath());
            ui->listWidget_userClasses->addItem(item);
        }
    }

    setCurrentIndex(1);
}

void MainCppMutTest::on_lineEdit_folderPath_editingFinished()
{
    QString folderPath = ui->lineEdit_folderPath->text().trimmed();
    if (!folderPath.isEmpty()) {
        openAndCheckNewFolder(folderPath);
    }
}

void MainCppMutTest::on_pushButton_selectFolder_clicked()
{
    QString folder = QFileDialog::getExistingDirectory(this, "Выберите папку");
    if (!folder.isEmpty()) {
        openAndCheckNewFolder(folder);
    }
}

void MainCppMutTest::on_radioButton_selectGtest_toggled(bool checked)
{
    if (!checked) {
        return;
    }

    if (!srcAnalysis) {
        return;
    }

    srcAnalysis->setTestFramework(SupportedTestFrameworks::GoogleTest);
    if (!srcAnalysis->isTestFrameworkValid(log)) {
        QMessageBox::warning(this, "Ошибка с тестами",
                             "Возникла проблема с фреймворком тестирования. "
                             "Ознакомьтесь с документацией.");

        srcAnalysis->setTestFramework(SupportedTestFrameworks::None);
        updateLogs();
        ui->buttonGroup_testFrameworks->setExclusive(false);
        ui->radioButton_selectGtest->setChecked(false);
        ui->radioButton_selectBoostTest->setChecked(false);
        ui->buttonGroup_testFrameworks->setExclusive(true);
    }

    checkConditionsForAnalysis();
}

void MainCppMutTest::on_radioButton_selectBoostTest_toggled(bool checked)
{
    if (!checked) {
        return;
    }

    if (!srcAnalysis) {
        return;
    }

    srcAnalysis->setTestFramework(SupportedTestFrameworks::BoostTest);
    if (!srcAnalysis->isTestFrameworkValid(log)) {
        QMessageBox::warning(this, "Ошибка с тестами",
                             "Возникла проблема с фреймворком тестирования. "
                             "Ознакомьтесь с документацией.");
        srcAnalysis->setTestFramework(SupportedTestFrameworks::None);
        updateLogs();
        ui->buttonGroup_testFrameworks->setExclusive(false);
        ui->radioButton_selectBoostTest->setChecked(false);
        ui->radioButton_selectGtest->setChecked(false);
        ui->buttonGroup_testFrameworks->setExclusive(true);
    }

    checkConditionsForAnalysis();
}

void MainCppMutTest::on_radioButton_gpp_toggled(bool checked)
{
    if(!checked) {
        return;
    }

    std::unique_ptr<BuildAndRunSubsystem> buildRun = std::make_unique<BuildAndRunSubsystem>();
    if(!buildRun->compilerIsAvailable(SupportedCompilers::Gpp, log)) {
        updateLogs();
        QMessageBox::critical(this, "Ошибка компилятора", "Компилятор g++ недоступен.");
        return;
    }

    selectedCompiler = SupportedCompilers::Gpp;

    checkConditionsForAnalysis();
}

void MainCppMutTest::on_radioButton_clangpp_toggled(bool checked)
{
    if(!checked) {
        return;
    }

    std::unique_ptr<BuildAndRunSubsystem> buildRun = std::make_unique<BuildAndRunSubsystem>();
    if(!buildRun->compilerIsAvailable(SupportedCompilers::Clang, log)) {
        updateLogs();
        QMessageBox::critical(this, "Ошибка компилятора", "Компилятор clang++ недоступен.");
        return;
    }

    selectedCompiler = SupportedCompilers::Clang;

    checkConditionsForAnalysis();
}

void MainCppMutTest::on_radioButton_orig_toggled(bool checked)
{
    if (!config || currentClass.isEmpty()) return;
    if (checked) {
        displaySRC(config->getSelectedClass().getCppFilePath());
    }
    else if (!currentMutant.isEmpty()) {
        QString originalPath = config->getSelectedClass().getCppFilePath();
        compareAndHighlightCode(originalPath, currentMutant);
    }
}

void MainCppMutTest::on_listWidget_userClasses_itemClicked(QListWidgetItem *item)
{
    currentClass = item->text();
    QString cppPath = item->data(Qt::UserRole).toString();

    ui->radioButton_orig->setChecked(true);
    ui->textBrowser_src->clear();
    ui->tableWidget_tests->setRowCount(0);
    ui->listWidget_mutants->clear();

    if (cppPath.isEmpty()) {
        ui->textBrowser_src->setPlainText("Файл не найден.");
        return;
    }

    for (const UserClassInfo &userClass : srcAnalysis->getUserClasses()) {
        if (userClass.getClassName() == currentClass) {
            updateMutationCheckboxesForClass(userClass);
            displaySRC(cppPath);
            displayTests(currentClass, false);
            if (mutationManager && selectedClassForAnalysis == currentClass) {
                QDir dir(mutationManager->getWorkingDirectoryPath());
                for (const QFileInfo &info : dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot)) {
                    ui->listWidget_mutants->addItem(info.fileName());
                }
                showMSIAndUpdateUI();
            }
            else {
                ui->listWidget_mutants->clear();
                cleanMSI();
            }
            break;
        }
    }
}

void MainCppMutTest::on_listWidget_mutants_itemClicked(QListWidgetItem *item)
{
    if (!config || !mutationManager) {
        return;
    }

    QString dirName = item->text();
    QString mutantPath = mutationManager->getWorkingDirectoryPath() + QDir::separator() + dirName;
    QDir dir(mutantPath);
    QStringList files = dir.entryList(QStringList() << "*.cpp", QDir::Files, QDir::Name);

    if (files.isEmpty()) {
        ui->textBrowser_src->setPlainText("Ошибка: файл мутанта не найден.");
        return;
    }

    currentMutant = mutantPath + QDir::separator() + files.first();
    QString originalPath = config->getSelectedClass().getCppFilePath();
    compareAndHighlightCode(originalPath, currentMutant);
    ui->radioButton_orig->setChecked(false);

    displayTests(currentMutant, true);
}

void MainCppMutTest::openDocumentation()
{
    if (!docWindow) {
        docWindow = new Documentation();
        docWindow->setAttribute(Qt::WA_DeleteOnClose);
        docWindow->setWindowFlags(Qt::Window);

        connect(docWindow, &QWidget::destroyed, this, [=]() {
            docWindow = nullptr;
        });

        docWindow->show();
    }
    else {
        docWindow->raise();
        docWindow->activateWindow();
    }
}

void MainCppMutTest::openAndCheckNewFolder(const QString &folder)
{
    if (folder.isEmpty()) {
        return;
    }

    cleanWorkingSpace();

    folderPath = folder;

    ui->pushButton_selectFolder->setFocus();

    srcAnalysis = new SourceCodeAnalysisSubsystem(folder);

    QPalette palette = ui->lineEdit_folderPath->palette();

    if (!srcAnalysis->isProjectStructureValid(folder, log)) {
        QMessageBox::warning(this, "Ошибка структуры проекта",
                             "Выбранная папка не соответствует требуемой структуре проекта. "
                             "Ознакомьтесь с документацией.");
        palette.setColor(QPalette::Base, Qt::red);
        ui->lineEdit_folderPath->setPalette(palette);
        ui->lineEdit_folderPath->setText(folder);
        updateLogs();

        checkConditionsForAnalysis();
        return;
    }

    ui->lineEdit_folderPath->setText(srcAnalysis->getFolderPath());
    ui->lineEdit_folderTestPath->setText(srcAnalysis->getFolderTestsPath());
    palette.setColor(QPalette::Base, Qt::green);
    ui->lineEdit_folderPath->setPalette(palette);

    ui->radioButton_selectGtest->setEnabled(true);
    ui->radioButton_selectBoostTest->setEnabled(true);

    checkConditionsForAnalysis();
}

void MainCppMutTest::checkConditionsForAnalysis()
{
    if (!srcAnalysis) {
        ui->pushButton_goToAnalisis_1->setEnabled(false);
        return;
    }

    if (!srcAnalysis->getFolderPath().isEmpty()
        && !(srcAnalysis->getTestFramework() == SupportedTestFrameworks::None)
        && (ui->radioButton_gpp->isChecked() || ui->radioButton_clangpp->isChecked())) {
        ui->pushButton_goToAnalisis_1->setEnabled(true);
    }
    else {
        ui->pushButton_goToAnalisis_1->setEnabled(false);
    }
}

void MainCppMutTest::displaySRC(const QString &cppFilePath)
{
    QFile file(cppFilePath);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        in.setEncoding(QStringConverter::Utf8);
        QString content = in.readAll();
        ui->textBrowser_src->setPlainText(content);
        file.close();
    }
    else {
        ui->textBrowser_src->setPlainText("Не удалось открыть файл.");
    }
}

void MainCppMutTest::displayTests(const QString &identifier, bool isMutant)
{
    if (!srcAnalysis) {
        return;
    }

    ui->tableWidget_tests->setRowCount(0);
    ui->tableWidget_tests->setColumnCount(2);
    ui->tableWidget_tests->setHorizontalHeaderLabels({"Тест", "Результат"});

    QVector<QString> testNames;
    QHash<QString, QString> testResults;

    if (isMutant) {
        if (!mutationManager) {
            return;
        }
        QMap<QString, QHash<QString, QString>> mutantTestResults = mutationManager->getMutantTestResults();
        testResults = mutantTestResults.value(identifier);
        QVector<UserClassInfo> classes = srcAnalysis->getUserClasses();
        for (const UserClassInfo& userClass : classes) {
            if (userClass.getClassName() == selectedClassForAnalysis) {
                testNames = userClass.getUnitTestNames();
                break;
            }
        }
    }
    else {
        QVector<UserClassInfo> classes = srcAnalysis->getUserClasses();
        for (const UserClassInfo& userClass : classes) {
            if (userClass.getClassName() == identifier) {
                testNames = userClass.getUnitTestNames();
                testResults = userClass.getOriginalTestsResults();
                break;
            }
        }
    }

    ui->tableWidget_tests->setRowCount(testNames.size());

    for (int i = 0; i < testNames.size(); i++) {
        QTableWidgetItem* nameItem = new QTableWidgetItem(testNames[i]);
        ui->tableWidget_tests->setItem(i, 0, nameItem);
    }

    for (int row = 0; row < testNames.size(); row++) {
        QTableWidgetItem* testNameItem = ui->tableWidget_tests->item(row, 0);
        if (!testNameItem) {
            continue;
        }

        QString name = testNameItem->text();
        QString status = testResults.value(name, "UNKNOWN");

        QTableWidgetItem* resultItem = new QTableWidgetItem(status);
        if (status == "PASSED") {
            resultItem->setForeground(QColor("green"));
        }
        else if (status == "FAILED") {
            resultItem->setForeground(QColor("red"));
        }
        else {
            resultItem->setForeground(QColor("gray"));
        }

        ui->tableWidget_tests->setItem(row, 1, resultItem);
    }
}

void MainCppMutTest::updateMutationCheckboxesForClass(const UserClassInfo &userClass)
{
    cleanSelectedMutationsTypes();

    QVector<SupportedMutations> available = userClass.getAvailableMutations();

    if (!available.contains(SupportedMutations::ArithmeticOperator)) {
        ui->checkBox_ArithmeticOperator->setEnabled(false);
    }
    if (!available.contains(SupportedMutations::LogicalOperator)) {
        ui->checkBox_LogicalOperator->setEnabled(false);
    }
    if (!available.contains(SupportedMutations::StringConstants)) {
        ui->checkBox_StringConstants->setEnabled(false);
    }
    if (!available.contains(SupportedMutations::NumericConstants)) {
        ui->checkBox_NumericConstants->setEnabled(false);
    }
    if (!available.contains(SupportedMutations::Exceptions)) {
        ui->checkBox_Exceptions->setEnabled(false);
    }
}

void MainCppMutTest::showMSIAndUpdateUI()
{
    if (!mutationManager) {
        return;
    }

    AnalyticalSubsystem analyzer(mutationManager->getMutantsTypes(), mutationManager->getMutantTestResults());

    double overallMSI = analyzer.calculateMSI();
    ui->label_MSI->setText(QString("MSI: %1%").arg(overallMSI, 0, 'f', 1));

    QPalette palette = ui->label_MSI->palette();
    if (overallMSI < 50.0) {
        palette.setColor(QPalette::WindowText, Qt::red);
    }
    else if (overallMSI < 75.0) {
        palette.setColor(QPalette::WindowText, QColor(255, 165, 0));
    }
    else {
        palette.setColor(QPalette::WindowText, Qt::green);
    }
    ui->label_MSI->setPalette(palette);

    QSet<SupportedMutations> types;
    for (const SupportedMutations &mutation : mutationManager->getMutantsTypes().values()) {
        types.insert(mutation);
    }

    if (types.contains(SupportedMutations::ArithmeticOperator)) {
        double msi = analyzer.calculateMSIByMutationType(SupportedMutations::ArithmeticOperator);
        ui->label_MSI_AO->setText(QString("AO: %1%").arg(msi, 0, 'f', 1));
        QPalette p;
        p.setColor(QPalette::WindowText, msi < 50.0 ? Qt::red : (msi < 75.0 ? QColor(255, 165, 0) : Qt::green));
        ui->label_MSI_AO->setPalette(p);
    }
    else {
        ui->label_MSI_AO->setText("AO: _%");
        ui->label_MSI_AO->setPalette(QPalette(Qt::black));
    }

    if (types.contains(SupportedMutations::LogicalOperator)) {
        double msi = analyzer.calculateMSIByMutationType(SupportedMutations::LogicalOperator);
        ui->label_MSI_LO->setText(QString("LO: %1%").arg(msi, 0, 'f', 1));
        QPalette p;
        p.setColor(QPalette::WindowText, msi < 50.0 ? Qt::red : (msi < 75.0 ? QColor(255, 165, 0) : Qt::green));
        ui->label_MSI_LO->setPalette(p);
    }
    else {
        ui->label_MSI_LO->setText("LO: _%");
        ui->label_MSI_LO->setPalette(QPalette(Qt::black));
    }

    if (types.contains(SupportedMutations::StringConstants)) {
        double msi = analyzer.calculateMSIByMutationType(SupportedMutations::StringConstants);
        ui->label_MSI_SC->setText(QString("SC: %1%").arg(msi, 0, 'f', 1));
        QPalette p;
        p.setColor(QPalette::WindowText, msi < 50.0 ? Qt::red : (msi < 75.0 ? QColor(255, 165, 0) : Qt::green));
        ui->label_MSI_SC->setPalette(p);
    }
    else {
        ui->label_MSI_SC->setText("SC: _%");
        ui->label_MSI_SC->setPalette(QPalette(Qt::black));
    }

    if (types.contains(SupportedMutations::NumericConstants)) {
        double msi = analyzer.calculateMSIByMutationType(SupportedMutations::NumericConstants);
        ui->label_MSI_NC->setText(QString("NC: %1%").arg(msi, 0, 'f', 1));
        QPalette p;
        p.setColor(QPalette::WindowText, msi < 50.0 ? Qt::red : (msi < 75.0 ? QColor(255, 165, 0) : Qt::green));
        ui->label_MSI_NC->setPalette(p);
    }
    else {
        ui->label_MSI_NC->setText("NC: _%");
        ui->label_MSI_NC->setPalette(QPalette(Qt::black));
    }

    if (types.contains(SupportedMutations::Exceptions)) {
        double msi = analyzer.calculateMSIByMutationType(SupportedMutations::Exceptions);
        ui->label_MSI_EM->setText(QString("EM: %1%").arg(msi, 0, 'f', 1));
        QPalette p;
        p.setColor(QPalette::WindowText, msi < 50.0 ? Qt::red : (msi < 75.0 ? QColor(255, 165, 0) : Qt::green));
        ui->label_MSI_EM->setPalette(p);
    }
    else {
        ui->label_MSI_EM->setText("EM: _%");
        ui->label_MSI_EM->setPalette(QPalette(Qt::black));
    }


    QHash<QString, MutantStates> mutantStates = analyzer.getMutantStates();

    for (int i = 0; i < ui->listWidget_mutants->count(); i++) {
        QListWidgetItem *item = ui->listWidget_mutants->item(i);
        if (!item) {
            continue;
        }

        QString dirName = item->text();
        QString mutantPath = mutationManager->getWorkingDirectoryPath() + QDir::separator() + dirName;

        QDir dir(mutantPath);
        QStringList files = dir.entryList(QStringList() << "*.cpp", QDir::Files);
        if (files.isEmpty()) {
            continue;
        }

        QString mutantFilePath = mutantPath + QDir::separator() + files.first();

        if (mutantStates.contains(mutantFilePath) &&
            mutantStates[mutantFilePath] == MutantStates::Survived) {
            item->setForeground(Qt::red);
        }
        else {
            item->setForeground(Qt::black);
        }
    }
}

void MainCppMutTest::compareAndHighlightCode(const QString &originalPath, const QString &mutantPath)
{
    QFile originalFile(originalPath);
    QFile mutantFile(mutantPath);

    if (!originalFile.open(QIODevice::ReadOnly | QIODevice::Text) ||
        !mutantFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        ui->textBrowser_src->setPlainText("Ошибка: не удалось открыть один из файлов.");
        return;
    }

    QTextStream originalStream(&originalFile);
    QTextStream mutantStream(&mutantFile);
    originalStream.setEncoding(QStringConverter::Utf8);
    mutantStream.setEncoding(QStringConverter::Utf8);

    QStringList originalLines = originalStream.readAll().split("\n");
    QStringList mutantLines = mutantStream.readAll().split("\n");

    originalFile.close();
    mutantFile.close();

    QString htmlContent = "<pre style=\"font-family: 'Cascadia Mono', monospace; font-size: 9pt;\">";
    int i = 0, j = 0;

    while (i < originalLines.size() || j < mutantLines.size()) {
        QString originalLine = i < originalLines.size() ? originalLines[i] : "";
        QString mutantLine = j < mutantLines.size() ? mutantLines[j] : "";
        QString displayLine = mutantLine;

        displayLine = displayLine.toHtmlEscaped();
        bool isChanged = (originalLine != mutantLine || originalLine.isEmpty() != mutantLine.isEmpty());

        if (isChanged && !displayLine.isEmpty()) {
            htmlContent += QString("<span style=\"background-color: #81dbeb;\">%1</span>\n").arg(displayLine);
        }
        else {
            htmlContent += displayLine + "\n";
        }

        if (!originalLine.isEmpty() || !mutantLine.isEmpty()) {
            if (!mutantLine.isEmpty()) {
                j++;
            }
            if (!originalLine.isEmpty()) {
                i++;
            }
        }
        else {
            i++;
            j++;
        }
    }

    htmlContent += "</pre>";
    ui->textBrowser_src->setHtml(htmlContent);
}

void MainCppMutTest::updateLogs()
{
    ui->textBrowser_logs->clear();

    QStringList formattedLogs;
    for (const QString& logEntry : log) {
        formattedLogs << QString(">> %1").arg(logEntry);
    }

    ui->textBrowser_logs->setPlainText(formattedLogs.join("\n"));

    ui->textBrowser_logs->verticalScrollBar()->setValue(ui->textBrowser_logs->verticalScrollBar()->maximum());
}

void MainCppMutTest::on_horizontalSlider_mutationCount_valueChanged(int value)
{
    ui->spinBox_mutationCount->setValue(value);
}

void MainCppMutTest::on_spinBox_mutationCount_valueChanged(int arg1)
{
    ui->horizontalSlider_mutationCount->setValue(arg1);
}

void MainCppMutTest::on_pushButton_runTestsOnSRC_clicked()
{
    if (!srcAnalysis) {
        QMessageBox::warning(this, "Ошибка", "Сначала выберите проект.");
        return;
    }

    if (selectedCompiler != SupportedCompilers::Gpp && selectedCompiler != SupportedCompilers::Clang) {
        QMessageBox::warning(this, "Ошибка", "Сначала выберите компилятор.");
        return;
    }

    QProgressDialog* progressDialog = new QProgressDialog("Запуск модульных тестов...", nullptr, 0, 0, this);
    progressDialog->setWindowTitle("Выполнение тестов");
    progressDialog->setWindowModality(Qt::WindowModal);
    progressDialog->setCancelButton(nullptr);
    progressDialog->setWindowFlags(progressDialog->windowFlags() & ~Qt::WindowCloseButtonHint);
    progressDialog->setMinimumDuration(0);
    progressDialog->show();

    QFutureWatcher<bool>* watcher = new QFutureWatcher<bool>(this);

    connect(watcher, &QFutureWatcher<bool>::finished, this, [this, watcher, progressDialog]() {
        bool success = watcher->result();
        watcher->deleteLater();
        progressDialog->close();
        progressDialog->deleteLater();

        if (!success) {
            QMessageBox::warning(this, "Ошибка", "Ошибка тестирования исходных классов.");
        }
        else if (ui->radioButton_orig->isChecked() && !currentClass.isEmpty()) {
            displayTests(currentClass, false);
        }

        updateLogs();
    });

    QFuture<bool> future = QtConcurrent::run([this]() -> bool {
        return srcAnalysis->runOriginalTests(selectedCompiler, log);
    });

    watcher->setFuture(future);
}

void MainCppMutTest::cleanWorkingSpace()
{
    ui->lineEdit_folderPath->clear();
    ui->lineEdit_folderTestPath->clear();
    ui->buttonGroup_testFrameworks->setExclusive(false);
    ui->radioButton_selectGtest->setChecked(false);
    ui->radioButton_selectBoostTest->setChecked(false);
    ui->buttonGroup_testFrameworks->setExclusive(true);
    ui->radioButton_selectGtest->setEnabled(false);
    ui->radioButton_selectBoostTest->setEnabled(false);
    ui->textBrowser_logs->clear();
    ui->listWidget_userClasses->clear();
    ui->listWidget_mutants->clear();
    ui->textBrowser_src->clear();
    ui->radioButton_orig->setChecked(true);
    ui->tableWidget_tests->setRowCount(0);
    ui->horizontalSlider_mutationCount->setValue(0);
    ui->spinBox_mutationCount->setValue(1);

    cleanSelectedMutationsTypes();
    cleanMSI();

    delete srcAnalysis;
    srcAnalysis = nullptr;
    delete config;
    config = nullptr;
    delete mutationManager;
    mutationManager = nullptr;

    log.clear();
    folderPath.clear();
    selectedClassForAnalysis.clear();
    currentClass.clear();
    currentMutant.clear();
}

void MainCppMutTest::cleanSelectedMutationsTypes()
{
    ui->checkBox_ArithmeticOperator->setChecked(false);
    ui->checkBox_LogicalOperator->setChecked(false);
    ui->checkBox_StringConstants->setChecked(false);
    ui->checkBox_NumericConstants->setChecked(false);
    ui->checkBox_Exceptions->setChecked(false);

    ui->checkBox_ArithmeticOperator->setEnabled(true);
    ui->checkBox_LogicalOperator->setEnabled(true);
    ui->checkBox_StringConstants->setEnabled(true);
    ui->checkBox_NumericConstants->setEnabled(true);
    ui->checkBox_Exceptions->setEnabled(true);
}

void MainCppMutTest::cleanMSI()
{
    QPalette palette;

    ui->label_MSI->setText("MSI: _%");
    palette.setColor(QPalette::WindowText, Qt::black);
    ui->label_MSI->setPalette(palette);

    ui->label_MSI_AO->setText("AO: _%");
    palette.setColor(QPalette::WindowText, Qt::black);
    ui->label_MSI_AO->setPalette(palette);

    ui->label_MSI_LO->setText("LO: _%");
    palette.setColor(QPalette::WindowText, Qt::black);
    ui->label_MSI_LO->setPalette(palette);

    ui->label_MSI_SC->setText("SC: _%");
    palette.setColor(QPalette::WindowText, Qt::black);
    ui->label_MSI_SC->setPalette(palette);

    ui->label_MSI_NC->setText("NC: _%");
    palette.setColor(QPalette::WindowText, Qt::black);
    ui->label_MSI_NC->setPalette(palette);

    ui->label_MSI_EM->setText("EM: _%");
    palette.setColor(QPalette::WindowText, Qt::black);
    ui->label_MSI_EM->setPalette(palette);
}

void MainCppMutTest::on_pushButton_zoomIn_clicked()
{
    ui->textBrowser_src->zoomIn();
}

void MainCppMutTest::on_pushButton_zoomOut_clicked()
{
    ui->textBrowser_src->zoomOut();
}

void MainCppMutTest::on_pushButton_startMut_clicked()
{
    if (!srcAnalysis) {
        QMessageBox::warning(this, "Ошибка", "Сначала выберите проект.");
        return;
    }

    if (selectedCompiler != SupportedCompilers::Gpp && selectedCompiler != SupportedCompilers::Clang) {
        QMessageBox::warning(this, "Ошибка", "Сначала выберите компилятор.");
        return;
    }

    if (currentClass.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Сначала выберите класс для анализа.");
        return;
    }

    UserClassInfo selectedClass(currentClass);
    bool found = false;
    for (const UserClassInfo &userClass : srcAnalysis->getUserClasses()) {
        if (userClass.getClassName() == currentClass) {
            selectedClass = userClass;
            found = true;
            break;
        }
    }

    if (!found) {
        QMessageBox::warning(this, "Ошибка", "Класс не найден.");
        return;
    }

    QVector<SupportedMutations> mutations;

    if (ui->checkBox_ArithmeticOperator->isChecked()) {
        mutations.append(SupportedMutations::ArithmeticOperator);
    }
    if (ui->checkBox_LogicalOperator->isChecked()) {
        mutations.append(SupportedMutations::LogicalOperator);
    }
    if (ui->checkBox_StringConstants->isChecked()) {
        mutations.append(SupportedMutations::StringConstants);
    }
    if (ui->checkBox_NumericConstants->isChecked()) {
        mutations.append(SupportedMutations::NumericConstants);
    }
    if (ui->checkBox_Exceptions->isChecked()) {
        mutations.append(SupportedMutations::Exceptions);
    }

    if (mutations.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Выберите хотя бы одну мутацию.");
        return;
    }

    unsigned short mutationCount = ui->horizontalSlider_mutationCount->value();
    SupportedTestFrameworks framework = srcAnalysis->getTestFramework();

    QStringList cppFiles;
    QDir dir(folderPath);
    for (const QFileInfo &file : dir.entryInfoList(QStringList() << "*.cpp", QDir::Files)) {
        cppFiles << file.filePath();
    }

    if (!ui->radioButton_orig->isChecked()) {
        ui->radioButton_orig->setChecked(true);
    }

    if (mutationManager) {
        delete mutationManager;
        mutationManager = nullptr;
    }

    ui->listWidget_mutants->clear();

    if (config) {
        delete config;
        config = nullptr;
    }

    config = new ConfigSubsystem(srcAnalysis->getFolderPath(), selectedClass, mutations, mutationCount,
                                 selectedCompiler, framework, cppFiles, threadDiv);

    log << "///" << QDateTime::currentDateTime().toString("HH:mm:ss") + " -- Инициализация мутационного анализа" << "///";
    mutationManager = new MutationManagerSubsystem(config, log);

    QProgressDialog *progressDialog = new QProgressDialog("Мутационный анализ выполняется...", "Остановить", 0, 0, this);
    progressDialog->setWindowModality(Qt::WindowModal);
    progressDialog->setWindowTitle("Анализ");
    progressDialog->setWindowFlags(progressDialog->windowFlags() & ~Qt::WindowCloseButtonHint);
    progressDialog->setMinimumDuration(0);
    progressDialog->show();

    connect(progressDialog, &QProgressDialog::canceled, this, [this]() {
        if (mutationManager) {
            mutationManager->stopMutationProcess();
        }
    });

    QFutureWatcher<void> *watcher = new QFutureWatcher<void>(this);

    connect(watcher, &QFutureWatcher<void>::finished, this, [this, watcher, progressDialog]() {
        watcher->deleteLater();
        progressDialog->close();
        progressDialog->deleteLater();
        updateLogs();
        onMutationAnalysisFinished();
    });

    QFuture<void> future = QtConcurrent::run([this]() {
        if (mutationManager && !mutationManager->startMutationProcess(log)) {
            log << "Ошибка мутационного анализа.";
        }
    });
    watcher->setFuture(future);
}

void MainCppMutTest::onMutationAnalysisFinished()
{
    if (!mutationManager || mutationManager->getMutantTestResults().isEmpty()) {
        QMessageBox::critical(this, "Ошибка", "Мутационный анализ завершился с ошибкой.");
        return;
    }

    QDir mDir(mutationManager->getWorkingDirectoryPath());
    for (const QFileInfo &info : mDir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot)) {
        ui->listWidget_mutants->addItem(info.fileName());
    }

    selectedClassForAnalysis = config->getSelectedClass().getClassName();
    ui->radioButton_orig->setChecked(true);

    showMSIAndUpdateUI();
}

void MainCppMutTest::on_radioButton_fullThread_toggled(bool checked)
{
    if (!checked) {
        return;
    }

    threadDiv = 1;
}

void MainCppMutTest::on_radioButton_halfThread_toggled(bool checked)
{
    if (!checked) {
        return;
    }

    threadDiv = 2;
}
