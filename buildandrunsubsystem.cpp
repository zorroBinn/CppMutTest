#include "buildandrunsubsystem.h"

bool BuildAndRunSubsystem::runExecutable(const QString &executablePath, QString &output, SupportedTestFrameworks framework)
{
    QProcess process;
    process.setProgram(executablePath);

    QStringList arguments;
    if (framework == SupportedTestFrameworks::GoogleTest) {
        QFileInfo exeInfo(executablePath);
        QString xmlPath = exeInfo.absolutePath() + QDir::separator() + "out.xml";
        arguments << QString("--gtest_output=xml:%1").arg(xmlPath);
    } else if (framework == SupportedTestFrameworks::BoostTest) {
        arguments << "--report_level=detailed" << "--report_format=XML";
    }

    process.setArguments(arguments);
    process.start();
    if (!process.waitForFinished(60000)) {
        output = process.errorString();
        return false;
    }

    output = process.readAllStandardError();
    output += process.readAllStandardOutput();
    return true;
}

void BuildAndRunSubsystem::parseGoogleTestOutput(const QString &xmlPath)
{
    QFile xmlFile(xmlPath);
    if (!xmlFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return;
    }

    QXmlStreamReader xml(&xmlFile);
    while (!xml.atEnd()) {
        xml.readNext();
        if (xml.isStartElement() && xml.name() == QString("testcase")) {
            QString testName = xml.attributes().value("name").toString();
            QString className = xml.attributes().value("classname").toString();
            QString fullTestName = className + "." + testName;

            QString result = "PASSED";
            while (!xml.atEnd() && !(xml.isEndElement() && xml.name() == QString("testcase"))) {
                xml.readNext();
                if (xml.isStartElement() && xml.name() == QString("failure")) {
                    result = "FAILED";
                    break;
                }
            }

            testResults[fullTestName] = result;
        }
    }

    xmlFile.close();
}

void BuildAndRunSubsystem::parseBoostTestOutput(const QString &output)
{
    QXmlStreamReader xml(output);

    while (!xml.atEnd()) {
        xml.readNext();
        if (xml.isStartElement() && xml.name() == QString("TestResult")) {
            while (!xml.atEnd()) {
                xml.readNext();
                if (xml.isStartElement() && xml.name() == QString("TestCase")) {
                    QString testName = xml.attributes().value("name").toString();
                    QString result = xml.attributes().value("result").toString();
                    testResults[testName] = (result == "passed") ? "PASSED" : "FAILED";
                }
            }
        }
    }
}

void BuildAndRunSubsystem::parseTestOutput(const QString &output, SupportedTestFrameworks framework, const QString &executablePath)
{
    QTextStream stream(const_cast<QString*>(&output), QIODevice::ReadOnly);

    switch (framework) {
    case SupportedTestFrameworks::GoogleTest: {
        QFileInfo exeInfo(executablePath);
        QString xmlPath = QDir::toNativeSeparators(exeInfo.absolutePath()) + QDir::separator() + "out.xml";
        parseGoogleTestOutput(xmlPath);
        if (!testResults.isEmpty()) {
            QFile::remove(xmlPath);
        }
        break;
    }
    case SupportedTestFrameworks::BoostTest: {
        parseBoostTestOutput(output);
        break;
    }
    default: break;
    }

    if (!testResults.isEmpty()) {
        QFile::remove(executablePath);
    }
}

BuildAndRunSubsystem::BuildAndRunSubsystem() {}

bool BuildAndRunSubsystem::compilerIsAvailable(SupportedCompilers compilerType, QStringList &log)
{
    std::unique_ptr<ICompiler> compiler;

    if (compilerType == SupportedCompilers::Gpp) {
        compiler = std::make_unique<GppCompiler>();
    }
    else if (compilerType == SupportedCompilers::Clang) {
        compiler = std::make_unique<ClangCompiler>();
    }
    else {
        log << "Выбран неподдерживаемый компилятор";
        return false;
    }

    return compiler->isAvailable();
}

CompilationResults BuildAndRunSubsystem::buildAndRun(const QString &folderPath, const QStringList &sourceFiles,
                                       const QStringList &testFiles, const QString &outputExecutablePath,
                                       QString &compileOutput, SupportedCompilers compilerType,
                                       SupportedTestFrameworks framework, QStringList &log)
{
    testResults.clear();
    std::unique_ptr<ICompiler> compiler;
    if (compilerType == SupportedCompilers::Gpp) {
        compiler = std::make_unique<GppCompiler>();
    }
    else if (compilerType == SupportedCompilers::Clang) {
        compiler = std::make_unique<ClangCompiler>();
    }
    else {
        compileOutput = "Неподдерживаемый компилятор.";
        return CompilationResults::CompilerError;
    }

    bool compilationSuccess = compiler->compile(folderPath, sourceFiles, testFiles, outputExecutablePath, compileOutput, framework);

    if (!compilationSuccess) {
        return CompilationResults::CompilationError;
    }

    QString output;
    if (!runExecutable(outputExecutablePath, output, framework)) {
        return CompilationResults::RunningError;
    }

    parseTestOutput(output, framework, outputExecutablePath);

    if (testResults.isEmpty()) {
        return CompilationResults::EmptyTestResults;
    }

    return CompilationResults::Success;
}

QMap<QString, QString> BuildAndRunSubsystem::getTestResults() const
{
    return testResults;
}
