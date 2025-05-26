#include "clangcompiler.h"

bool ClangCompiler::isAvailable() const
{
    QProcess process;
    process.start("clang++ --version");
    process.waitForFinished(5000);
    return process.exitStatus() == QProcess::NormalExit && process.exitCode() == 0;
}

QString ClangCompiler::getName() const
{
    return "clang++";
}

SupportedCompilers ClangCompiler::getNameFromSupportedCompilers() const
{
    return SupportedCompilers::Clang;
}

bool ClangCompiler::compile(const QString &folderPath, const QStringList &sourceFiles,
                            const QStringList &testFiles, const QString &outputExecutablePath,
                            QString &compileOutput, SupportedTestFrameworks framework)
{
    QStringList arguments;
    arguments << sourceFiles << testFiles;
    QString appDir = QCoreApplication::applicationDirPath();

    QString includePath;
    QString libPath;
    QStringList libs;

    switch (framework) {
    case SupportedTestFrameworks::GoogleTest: {
        includePath = appDir + QDir::separator() + "gtest" + QDir::separator() + "include";
        libPath = appDir + QDir::separator() + "gtest" + QDir::separator() + "lib";
        libs << "-lgtest" << "-lgtest_main";
        break;
    }
    case SupportedTestFrameworks::BoostTest: {
        includePath = appDir + QDir::separator() + "boost" + QDir::separator() + "include";
        libPath = appDir + QDir::separator() + "boost" + QDir::separator() + "lib";
        libs << "-lboost_unit_test_framework";
        break;
    }
    default: {
        compileOutput = "Неподдерживаемый фреймворк тестирования.";
        return false;
    }
    }
    arguments << "-std=c++17" << "-I" + includePath << "-I" + folderPath;

    if (!libPath.isEmpty()) {
        arguments << "-L" + libPath;
    }

    arguments += libs;

    arguments << "-pthread";

    arguments << "-o" << outputExecutablePath;

    QProcess compiler;
    compiler.setProgram("clang++");
    compiler.setArguments(arguments);
    compiler.setWorkingDirectory(folderPath);
    compiler.start();
    compiler.waitForFinished();

    compileOutput = compiler.readAllStandardOutput() + compiler.readAllStandardError();
    return compiler.exitCode() == 0;
}
