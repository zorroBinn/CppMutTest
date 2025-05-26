#pragma once
#include <QProcess>
#include <QFileInfo>
#include <QDirIterator>
#include <QCoreApplication>
#include "ICompiler.h"

///
/// \brief The GppCompiler class - компилятор g++
///
class GppCompiler : public ICompiler
{
public:
    GppCompiler() = default;

    ~GppCompiler() override = default;

    bool isAvailable() const override;

    QString getName() const override;

    SupportedCompilers getNameFromSupportedCompilers() const override;

    bool compile(const QString &folderPath, const QStringList &sourceFiles,
                 const QStringList &testFiles, const QString &outputExecutablePath,
                 QString &compileOutput, SupportedTestFrameworks framework) override;
};

