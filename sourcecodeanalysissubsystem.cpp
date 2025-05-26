#include "sourcecodeanalysissubsystem.h"

void SourceCodeAnalysisSubsystem::removeClass(const QString &name)
{
    for (int i = 0; i < userClasses.size(); i++) {
        if (userClasses[i].getClassName() == name) {
            userClasses.removeAt(i);
            break;
        }
    }
}

void SourceCodeAnalysisSubsystem::extractClassNames()
{
    userClasses.clear();

    QDir sourceDir(folderPath);
    QFileInfoList sourceFiles = sourceDir.entryInfoList(QStringList() << "*.h", QDir::Files | QDir::NoSymLinks | QDir::Readable, QDir::Name);

    QRegularExpression classRegex(R"(\bclass\s+(\w+))");

    for (const QFileInfo &file : sourceFiles)
    {
        QFile f(file.absoluteFilePath());
        if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) {
            continue;
        }

        QTextStream in(&f);
        QString content = in.readAll();

        auto matches = classRegex.globalMatch(content);
        while (matches.hasNext()) {
            auto match = matches.next();
            QString className = match.captured(1);
            UserClassInfo classInfo(className);
            classInfo.setHFilePath(file.absoluteFilePath());
            userClasses.append(classInfo);
        }

        f.close();
    }
}

void SourceCodeAnalysisSubsystem::associateCppFilesWithClasses(QStringList &log)
{
    QDir sourceDir(folderPath);
    QFileInfoList cppFiles = sourceDir.entryInfoList(QStringList() << "*.cpp", QDir::Files | QDir::NoSymLinks | QDir::Readable, QDir::Name);

    for (int i = 0; i < userClasses.size(); i++) {
        QString className = userClasses[i].getClassName();
        QString matchingFile = "";
        QString hFileName = QFileInfo(userClasses[i].getHFilePath()).fileName();

        for (const QFileInfo &file : cppFiles) {
            QFile f(file.absoluteFilePath());
            if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) {
                continue;
            }

            QString content = QTextStream(&f).readAll();
            f.close();

            QRegularExpression includePattern(QString(R"(#include\s*(?:<%1>|\"%1\"))").arg(QRegularExpression::escape(hFileName)));
            if (!includePattern.match(content).hasMatch()) {
                continue;
            }

            QRegularExpression methodImplPattern(QString(R"(%1::)").arg(className));
            if (methodImplPattern.match(content).hasMatch()) {
                matchingFile = file.absoluteFilePath();
                break;
            }
        }

        if (!matchingFile.isEmpty()) {
            userClasses[i].setCppFilePath(matchingFile);
        }
        else {
            removeClass(className);
            i--;
            log << "Класс " + className + " был исключен. Причина: не обнаружено файла реализации .cpp";
        }
    }
}

void SourceCodeAnalysisSubsystem::identifyMethods()
{
    for (UserClassInfo &userClass : userClasses)
    {
        QString cppFile = userClass.getCppFilePath();
        if (cppFile.isEmpty()) {
            continue;
        }

        QFile file(cppFile);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            continue;
        }

        QTextStream in(&file);
        QString content = in.readAll();
        file.close();

        QString className = userClass.getClassName();

        QRegularExpression methodRegex(QString(R"(%1::(\w+)\s*\()").arg(QRegularExpression::escape(className)));
        auto matches = methodRegex.globalMatch(content);

        while (matches.hasNext()) {
            QRegularExpressionMatch match = matches.next();
            QString methodName = match.captured(1);
            if (!methodName.isEmpty()) {
                userClass.addMethodName(methodName);
            }
        }
    }
}

void SourceCodeAnalysisSubsystem::analyzeAvailableMutationsForClasses()
{
    for (UserClassInfo &userClass : userClasses)
    {
        QVector<SupportedMutations> detectedMutations;
        QString cppFile = userClass.getCppFilePath();
        if (cppFile.isEmpty()) {
            continue;
        }

        std::unique_ptr<IMutation> mutAO = std::make_unique<ArithmeticMutation>(),
                                    mutLO  = std::make_unique<LogicalMutation>(),
                                    mutSC  = std::make_unique<StringConstantsMutation>(),
                                    mutNC  = std::make_unique<NumericConstantsMutation>(),
                                    mutEM  = std::make_unique<ExceptionsMutation>();

        if (mutAO->isAvailable(cppFile)) {
            detectedMutations.append(SupportedMutations::ArithmeticOperator);
        }
        if (mutLO->isAvailable(cppFile)) {
            detectedMutations.append(SupportedMutations::LogicalOperator);
        }
        if (mutSC->isAvailable(cppFile)) {
            detectedMutations.append(SupportedMutations::StringConstants);
        }
        if (mutNC->isAvailable(cppFile)) {
            detectedMutations.append(SupportedMutations::NumericConstants);
        }
        if (mutEM->isAvailable(cppFile)) {
            detectedMutations.append(SupportedMutations::Exceptions);
        }

        userClass.setAvailableMutations(detectedMutations);
    }
}

void SourceCodeAnalysisSubsystem::identifyTestsForClasses(QStringList &log)
{
    QDir testsDir(folderTestsPath);
    QFileInfoList testFiles = testsDir.entryInfoList(QStringList() << "*.cpp", QDir::Files | QDir::NoSymLinks);

    for (const QFileInfo &testFile : testFiles)
    {
        QFile file(testFile.absoluteFilePath());
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            continue;
        }

        QString content = QTextStream(&file).readAll();
        file.close();

        QMap<QString, QVector<QString>> classToTestNames;

        for (const UserClassInfo &userClass : userClasses)
        {
            QString className = userClass.getClassName();
            QVector<QString> foundTests;

            switch (testFramework) {
            case SupportedTestFrameworks::GoogleTest: {
                //Ищем TEST(ExactClassName, TestName)
                QRegularExpression gtestRegex(R"(TEST\s*\(\s*(\w+)\s*,\s*(\w+)\s*\))");
                auto matches = gtestRegex.globalMatch(content);
                while (matches.hasNext()) {
                    auto match = matches.next();
                    QString groupName = match.captured(1);
                    QString testName = match.captured(2);
                    if (groupName == className) {
                        foundTests.append(groupName + "." + testName);
                    }
                }
                break;
            }
            case SupportedTestFrameworks::BoostTest: {
                //Ищем #define BOOST_TEST_MODULE <ClassName>
                QRegularExpression moduleRegex(R"(#define\s+BOOST_TEST_MODULE\s+(\w+))");
                QRegularExpressionMatch match = moduleRegex.match(content);
                if (match.hasMatch()) {
                    QString moduleName = match.captured(1);
                    if (moduleName == className) {
                        //Ищем все BOOST_AUTO_TEST_CASE(Имя)
                        QRegularExpression boostRegex(R"(BOOST_(AUTO|FIXTURE)_TEST_CASE\s*\(\s*([\w_]+)\s*\))");
                        auto matches = boostRegex.globalMatch(content);
                        while (matches.hasNext()) {
                            auto testMatch = matches.next();
                            QString testName = testMatch.captured(2);
                            foundTests.append(testName);
                        }
                    }
                }
                break;
            }
            default: break;
            }

            if (!foundTests.isEmpty()) {
                classToTestNames[className] = foundTests;
            }
        }

        if (classToTestNames.size() == 1) {
            QString targetClass = classToTestNames.firstKey();
            QVector<QString> testNames = classToTestNames[targetClass];

            for (UserClassInfo &userClass : userClasses) {
                if (userClass.getClassName() == targetClass) {
                    userClass.addTestFilePath(testFile.absoluteFilePath());
                    for (const QString &name : testNames) {
                        userClass.addUnitTestName(name);
                    }
                    break;
                }
            }
        }
        else if (classToTestNames.size() > 1) {
            log << testFile.absoluteFilePath() + " исключен. Причина: распознано несколько тестируемых классов. Ознакомьтесь с документацией";
        }
        else {
            log << testFile.absoluteFilePath() + " исключен. Причина: не найдено тестируемых классов. Ознакомьтесь с документацией";
        }
    }
}

void SourceCodeAnalysisSubsystem::removeClassesWithoutTests(QStringList &log)
{
    for (int i = 0; i < userClasses.size(); i++) {
        if (!userClasses[i].hasUnitTests()) {
            userClasses.removeAt(i);
            log << "Класс " + userClasses[i].getClassName() + " был исключен. Причина: не обнаружено тестов."
                                                              " Ознакомьтесь с документацией";
            i--;
        }
    }
}

void SourceCodeAnalysisSubsystem::removeClassesWithMultipleBoostTestFiles(QStringList &log)
{
    if (testFramework != SupportedTestFrameworks::BoostTest) {
        return;
    }

    for (const UserClassInfo &userClass : userClasses) {
        if (userClass.getTestFilePaths().size() > 1) {
            QString className = userClass.getClassName();
            removeClass(className);
            log << "Класс " + className + " был исключен. Причина: имеет несколько тестовых файлов при использовании тестового фреймворка Boost.Test."
                                          " Ознакомьтесь с документацией";
        }
    }
}

SourceCodeAnalysisSubsystem::SourceCodeAnalysisSubsystem(const QString &folder)
    : folderPath(QDir::toNativeSeparators(folder)),
    folderTestsPath(QDir::toNativeSeparators(folder) + QDir::separator() + "tests"),
    testFramework(SupportedTestFrameworks::None) {}

QString SourceCodeAnalysisSubsystem::getFolderPath() const
{
    return folderPath;
}

QString SourceCodeAnalysisSubsystem::getFolderTestsPath() const
{
    return folderTestsPath;
}

QVector<UserClassInfo> SourceCodeAnalysisSubsystem::getUserClasses() const
{
    return userClasses;
}

SupportedTestFrameworks SourceCodeAnalysisSubsystem::getTestFramework() const
{
    return testFramework;
}

void SourceCodeAnalysisSubsystem::setTestFramework(SupportedTestFrameworks framework)
{
    testFramework = framework;
}

bool SourceCodeAnalysisSubsystem::isProjectStructureValid(const QString &folder, QStringList &log)
{
    QDir dir(folder);
    if (!dir.exists()) {
        log << folder + " не существует";
        return false;
    }

    QFileInfoList fileList = dir.entryInfoList(QStringList() << "*.cpp" << "*.h", QDir::Files | QDir::NoSymLinks, QDir::Name);
    if (fileList.isEmpty()) {
        log << folder + " не содержит .h и .cpp файлов";
        return false;
    }

    QDir testsDir(folder + QDir::separator() + "tests");
    if (!testsDir.exists()) {
        log << folder + " не содержит папки \"tests\"";
        return false;
    }

    QFileInfoList testFiles = testsDir.entryInfoList(QStringList() << "*.cpp", QDir::Files | QDir::NoSymLinks, QDir::Name);
    if (testFiles.isEmpty()) {
        log << folder + QDir::separator() + "tests" + " не содержит .cpp файлов c юнит-тестами";
        return false;
    }

    return true;
}

bool SourceCodeAnalysisSubsystem::analyzeSourceFiles(QStringList &log)
{
    if (testFramework == SupportedTestFrameworks::None) {
        log << "Не определен тестовый фреймворк";
        return false;
    }
    extractClassNames();
    associateCppFilesWithClasses(log);
    identifyTestsForClasses(log);
    removeClassesWithoutTests(log);
    removeClassesWithMultipleBoostTestFiles(log);
    identifyMethods();
    analyzeAvailableMutationsForClasses();
    return !userClasses.isEmpty();
}

bool SourceCodeAnalysisSubsystem::isTestFrameworkValid(QStringList &log) const
{
    QDir testsDir(folderTestsPath);
    QFileInfoList testFiles = testsDir.entryInfoList(QStringList() << "*.cpp", QDir::Files | QDir::NoSymLinks);

    if (testFiles.isEmpty()) {
        log << folderTestsPath + " не содержит .cpp файлов c юнит-тестами";
        return false;
    }

    QSet<SupportedTestFrameworks> frameworksDetected;

    for (const QFileInfo &testFile : testFiles) {
        QFile file(testFile.absoluteFilePath());
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            continue;
        }

        QTextStream in(&file);
        QString content = in.readAll();
        file.close();

        SupportedTestFrameworks fileFramework = SupportedTestFrameworks::None;
        bool frameworkFound = false;

        if (content.contains("#include <boost/test/included/unit_test.hpp>") || content.contains("#include \"boost/test/included/unit_test.hpp\"") ||
            content.contains("#include <boost/test/unit_test.hpp>") || content.contains("#include \"boost/test/unit_test.hpp\"") ||
            content.contains("BOOST_AUTO_TEST_CASE(")) {
            fileFramework = SupportedTestFrameworks::BoostTest;
            frameworkFound = true;
        }
        else if (content.contains("#include <gtest/gtest.h>") || content.contains("#include \"gtest/gtest.h\"") ||
                 content.contains("TEST(")) {
            fileFramework = SupportedTestFrameworks::GoogleTest;
            frameworkFound = true;
        }

        if (!frameworkFound) {
            log << testFile.absoluteFilePath() + " не содержит признаков поддерживаемых тестовых фреймворков. Ознакомьтесь с документацией";
            return false;
        }

        frameworksDetected.insert(fileFramework);

        if (frameworksDetected.size() > 1) {
            log << testFile.absoluteFilePath() + " содержит признаки нескольких тестовых фреймворков. Ознакомьтесь с документацией";
            return false;
        }
    }

    return !frameworksDetected.isEmpty() && frameworksDetected.contains(testFramework);
}

bool SourceCodeAnalysisSubsystem::runOriginalTests(SupportedCompilers compiler, QStringList &log)
{
    QString tempDirPath = folderPath + QDir::separator() + "CppMutTest_PreWorkingDirectory";
    QDir tempDir(tempDirPath);
    if (tempDir.exists()) {
        if (!tempDir.removeRecursively()) {
            log << "Ошибка: Не удалось удалить существующую временную директорию " + tempDirPath;
            return false;
        }
    }
    if (!tempDir.mkpath(tempDirPath)) {
        log << "Ошибка: Не удалось создать временную директорию " + tempDirPath;
        return false;
    }

    //Мьютекс для логирования
    QMutex logMutex;
    bool allSuccess = true;

    auto processClass = [&](UserClassInfo &userClass) {
        BuildAndRunSubsystem localBuildRun;

        //Создаём поддиректорию для класса
        QString classDirName = userClass.getClassName();
        QString classDirPath = QDir::toNativeSeparators(tempDirPath + QDir::separator() + classDirName);
        QDir classDir(tempDirPath);
        if (!classDir.mkpath(classDirName)) {
            QMutexLocker locker(&logMutex);
            log << "Ошибка: Не удалось создать директорию для класса " + classDirName;
            allSuccess = false;
            return;
        }

        QStringList sourceFiles;
        QStringList testFiles = userClass.getTestFilePaths();
        QDir projectDir(folderPath);
        QStringList cppFiles = projectDir.entryList(QStringList() << "*.cpp", QDir::Files);
        for (const QString &file : cppFiles) {
            sourceFiles << folderPath + QDir::separator() + file;
        }

#ifdef Q_OS_WIN
        QString outputExePath = classDirPath + QDir::separator() + "test_" + classDirName + ".exe";
#else
        QString outputExePath = classDirPath + QDir::separator() + "test_" + classDirName;
#endif

        QString compileOutput;
        QStringList localLog;
        CompilationResults result = localBuildRun.buildAndRun(folderPath, sourceFiles,
                                                              testFiles, outputExePath,
                                                              compileOutput, compiler,
                                                              testFramework, localLog);

        if (result != CompilationResults::Success) {
            QMutexLocker locker(&logMutex);
            log << "Ошибка для класса " + classDirName + ": " + compileOutput;
            log << localLog;
            allSuccess = false;
            return;
        }

        QMap<QString, QString> testResults = localBuildRun.getTestResults();
        userClass.setOriginalTestsResults(testResults);

        if (testResults.isEmpty()) {
            QMutexLocker locker(&logMutex);
            log << "Предупреждение: Пустые результаты тестов для класса " + classDirName;
            allSuccess = false;
        }
    };

    QFuture<void> future = QtConcurrent::map(userClasses, processClass);

    future.waitForFinished();

    if (!tempDir.removeRecursively()) {
        log << "Предупреждение: Не удалось удалить временную директорию " + tempDirPath;
    }

    return allSuccess;
}
