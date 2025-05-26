#pragma once

#include "buildandrunsubsystem.h"
#include "sourcecodeanalysissubsystem.h"
#include "configsubsystem.h"
#include "mutationmanagersubsystem.h"
#include "analyticalsubsystem.h"

///
/// \brief The Controller class - класс-посредник между моделью и представлением
///
class Controller
{
private:
    QStringList log;
    QString folderPath;
    QString selectedClass;
    QString currentMutClass;
    QString currentMutant;
    SupportedCompilers selectedCompiler;
    SourceCodeAnalysisSubsystem *SCAS = nullptr;
    ConfigSubsystem *config = nullptr;
    BuildAndRunSubsystem *buildRun = nullptr;
    MutationManagerSubsystem *mutationManager = nullptr;

public:
    Controller();

    QStringList getLog() const;

    bool isGppCompilerAvailable();
    bool isClangCompilerAvailable();

    bool isProjectStructureValid(const QString &folderPath);

    QString getFolderPath() const;
    void setFolderPath(const QString &newFolderPath);

    QString getSelectedClass() const;
    void setSelectedClass(const QString &newSelectedClass);

    QString getCurrentMutClass() const;
    void setCurrentMutClass(const QString &newCurrentMutClass);

    QString getCurrentMutant() const;
    void setCurrentMutant(const QString &newCurrentMutant);

    SupportedCompilers getSelectedCompiler() const;
    void setSelectedCompiler(SupportedCompilers newSelectedCompiler);
};
