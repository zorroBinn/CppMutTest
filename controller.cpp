#include "controller.h"

QString Controller::getFolderPath() const
{
    return folderPath;
}

void Controller::setFolderPath(const QString &newFolderPath)
{
    folderPath = newFolderPath;
}

QString Controller::getSelectedClass() const
{
    return selectedClass;
}

void Controller::setSelectedClass(const QString &newSelectedClass)
{
    selectedClass = newSelectedClass;
}

QString Controller::getCurrentMutClass() const
{
    return currentMutClass;
}

void Controller::setCurrentMutClass(const QString &newCurrentMutClass)
{
    currentMutClass = newCurrentMutClass;
}

QString Controller::getCurrentMutant() const
{
    return currentMutant;
}

void Controller::setCurrentMutant(const QString &newCurrentMutant)
{
    currentMutant = newCurrentMutant;
}

SupportedCompilers Controller::getSelectedCompiler() const
{
    return selectedCompiler;
}

void Controller::setSelectedCompiler(SupportedCompilers newSelectedCompiler)
{
    selectedCompiler = newSelectedCompiler;
}

Controller::Controller() {}

QStringList Controller::getLog() const
{
    return log;
}

bool Controller::isGppCompilerAvailable()
{
    std::unique_ptr<BuildAndRunSubsystem> buildRun = std::make_unique<BuildAndRunSubsystem>();
    return buildRun->compilerIsAvailable(SupportedCompilers::Gpp, log);
}

bool Controller::isClangCompilerAvailable()
{
    std::unique_ptr<BuildAndRunSubsystem> buildRun = std::make_unique<BuildAndRunSubsystem>();
    return buildRun->compilerIsAvailable(SupportedCompilers::Clang, log);
}
