#include "configsubsystem.h"

ConfigSubsystem::ConfigSubsystem(const QString &folderPath, const UserClassInfo &selectedClass,
                                 const QVector<SupportedMutations> &selectedMutation, unsigned short numberOfMutations,
                                 SupportedCompilers compiler, SupportedTestFrameworks testFramework,
                                 const QStringList &srcFiles, unsigned short threadDiv)
    : folderPath(QDir::toNativeSeparators(folderPath)), selectedClass(selectedClass),
    selectedMutation(selectedMutation), numberOfMutations(numberOfMutations),
    compiler(compiler), testFramework(testFramework), threadDiv(threadDiv)
{
    sourceFiles.clear();
    for (const QString &file : srcFiles) {
        sourceFiles.append(QDir::toNativeSeparators(file));
    }
}

unsigned short ConfigSubsystem::getThreadDiv() const
{
    return threadDiv;
}

void ConfigSubsystem::setThreadDiv(unsigned short newStreamsDiv)
{
    threadDiv = newStreamsDiv;
}

QString ConfigSubsystem::getFolderPath() const
{
    return folderPath;
}

void ConfigSubsystem::setFolderPath(const QString &path)
{
    folderPath = QDir::toNativeSeparators(path);
}

UserClassInfo ConfigSubsystem::getSelectedClass() const
{
    return selectedClass;
}

void ConfigSubsystem::setSelectedClass(const UserClassInfo &userClass)
{
    selectedClass = userClass;
}

QVector<SupportedMutations> ConfigSubsystem::getSelectedMutations() const
{
    return selectedMutation;
}

void ConfigSubsystem::setSelectedMutations(const QVector<SupportedMutations> &mutations)
{
    selectedMutation = mutations;
}

unsigned short ConfigSubsystem::getNumberOfMutations() const
{
    return numberOfMutations;
}

void ConfigSubsystem::setNumberOfMutations(unsigned short number)
{
    numberOfMutations = number;
}

SupportedCompilers ConfigSubsystem::getCompiler() const
{
    return compiler;
}

void ConfigSubsystem::setCompiler(SupportedCompilers compil)
{
    compiler = compil;
}

SupportedTestFrameworks ConfigSubsystem::getTestFramework() const
{
    return testFramework;
}

void ConfigSubsystem::setTestFramework(SupportedTestFrameworks framework)
{
    testFramework = framework;
}

QStringList ConfigSubsystem::getSourceFiles() const
{
    return sourceFiles;
}

void ConfigSubsystem::setSourceFiles(const QStringList &files)
{
    sourceFiles.clear();
    for (const QString &file : files) {
        sourceFiles.append(QDir::toNativeSeparators(file));
    }
}
