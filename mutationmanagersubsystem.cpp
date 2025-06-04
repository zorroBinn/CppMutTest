#include "mutationmanagersubsystem.h"

bool MutationManagerSubsystem::createWorkingDirectory(QStringList &log)
{
    QDir dir(workingDirectoryPath);
    if (dir.exists()) {
        if (dir.removeRecursively()) {
            log << "Старая рабочая директория успешно удалена: " << workingDirectoryPath;
        }
        else {
            log << "Ошибка удаления старой рабочей директории: " << workingDirectoryPath;
            return false;
        }
    }

    if (!dir.mkpath(workingDirectoryPath)) {
        log << "Невозможно создать рабочую директорию " + workingDirectoryPath;
        return false;
    }

    log << "Рабочая директория создана: " + workingDirectoryPath;
    return true;
}

bool MutationManagerSubsystem::distributeMutants(QStringList &log)
{
    mutantsTypes.clear();

    QVector<SupportedMutations> selectedMutations = config->getSelectedMutations();
    unsigned short numberOfMutations = config->getNumberOfMutations();
    QString cppFilePath = config->getSelectedClass().getCppFilePath();
    QString baseFileName = QFileInfo(cppFilePath).completeBaseName();

    if (selectedMutations.isEmpty()) {
        log << "Не выбраны типы мутаций";
        return false;
    }
    if (numberOfMutations == 0) {
        log << "Количество мутаций равно нулю";
        return false;
    }
    if (cppFilePath.isEmpty()) {
        log << "Файл реализации класса не найден";
        return false;
    }

    //Формирование списка мутаций с учетом лимита
    QVector<SupportedMutations> mutationPool;
    for (unsigned short i = 0; i < numberOfMutations; i++) {
        int idx = QRandomGenerator::global()->bounded(selectedMutations.size());
        mutationPool.append(selectedMutations[idx]);
    }

    //Создание поддиректорий и копирование файлов
    for (int i = 0; i < mutationPool.size(); i++) {
        if (isStopping) {
            log << "Инициирована ручная остановка процесса.";
            return false;
        }
        SupportedMutations mutation = mutationPool[i];
        QString shortName;
        switch (mutation) {
        case SupportedMutations::ArithmeticOperator: {
            shortName = "AO";
            break;
        }
        case SupportedMutations::LogicalOperator: {
            shortName = "LO";
            break;
        }
        case SupportedMutations::StringConstants: {
            shortName = "SC";
            break;
        }
        case SupportedMutations::NumericConstants: {
            shortName = "NC";
            break;
        }
        case SupportedMutations::Exceptions: {
            shortName = "EM";
            break;
        }
        }

        //Форматированный номер мутанта с ведущими нулями
        QString mutantIndex;
        short int numMut = config->getNumberOfMutations();
        if (numMut < 100) {
            mutantIndex = QString("%1").arg(i + 1, 2, 10, QChar('0'));
        }
        else if (numMut < 1000) {
            mutantIndex = QString("%1").arg(i + 1, 3, 10, QChar('0'));
        }
        else if (numMut < 10000) {
            mutantIndex = QString("%1").arg(i + 1, 4, 10, QChar('0'));
        }
        else {
            mutantIndex = QString("%1").arg(i + 1, 5, 10, QChar('0'));
        }

        //Создание поддиректории mutant_<i>_<shortName>
        QString mutantDirName = QString("mutant_%1_%2").arg(mutantIndex).arg(shortName);
        QString mutantDirPath = workingDirectoryPath + QDir::separator() + mutantDirName;
        QDir mutantDir(mutantDirPath);
        if (!mutantDir.mkpath(mutantDirPath) && !mutantDir.exists()) {
            log << "Ошибка создания директории: " + mutantDirPath;
            continue;
        }

        //Копирование .cpp файла с типизированным именем <baseName>_mutant_<номер>_<короткое обозначение>.cpp
        QString mutantFileName = QString("%1_mutant_%2_%3.cpp").arg(baseFileName).arg(mutantIndex).arg(shortName);
        QString mutantFilePath = mutantDirPath + QDir::separator() + mutantFileName;
        if (!QFile::copy(cppFilePath, mutantFilePath)) {
            log << "Ошибка копирования файла: " + cppFilePath + " в " + mutantFilePath;
            continue;
        }

        mutantsTypes[mutantFilePath] = mutation;
    }

    return !mutantsTypes.isEmpty();
}

void MutationManagerSubsystem::generateMutants(QStringList &log)
{
    if (mutantsTypes.isEmpty()) {
        log << "Нет мутантов для генерации";
        return;
    }

    //Итерация по мутантам в mutantsTypes
    for (auto it = mutantsTypes.constBegin(); it != mutantsTypes.constEnd(); it++) {
        if (isStopping) {
            log << "Инициирована ручная остановка процесса.";
            return;
        }

        QString mutantFilePath = it.key();
        SupportedMutations mutationType = it.value();

        //Выбор соответствующего класса мутации
        std::unique_ptr<IMutation> mutation;
        QString mutationShortName;
        switch (mutationType) {
        case SupportedMutations::ArithmeticOperator: {
            mutation = std::make_unique<ArithmeticMutation>();
            mutationShortName = "AO";
            break;
        }
        case SupportedMutations::LogicalOperator: {
            mutation = std::make_unique<LogicalMutation>();
            mutationShortName = "LO";
            break;
        }
        case SupportedMutations::StringConstants: {
            mutation = std::make_unique<StringConstantsMutation>();
            mutationShortName = "SC";
            break;
        }
        case SupportedMutations::NumericConstants: {
            mutation = std::make_unique<NumericConstantsMutation>();
            mutationShortName = "NC";
            break;
        }
        case SupportedMutations::Exceptions: {
            mutation = std::make_unique<ExceptionsMutation>();
            mutationShortName = "EM";
            break;
        }
        }

        if (mutation) {
            mutation->mutate(mutantFilePath);
            log << "Мутация " + mutationShortName + " применена к файлу " + mutantFilePath;
        }
        else {
            log << "Не удалось создать объект мутации для файла " + mutantFilePath;
        }
    }

    log << QDateTime::currentDateTime().toString("HH:mm:ss") + QString(" -- Генерация %1 мутантов завершена").arg(mutantsTypes.size());
}

void MutationManagerSubsystem::runTestsOnMutants(QStringList &log)
{
    if (mutantsTypes.isEmpty()) {
        log << "Нет мутантов для тестирования";
        return;
    }

    QMutexLocker locker(&mutex);
    mutantTestResults.clear();
    locker.unlock();

    QString projectFolderPath = config->getFolderPath();
    QStringList testFiles = config->getSelectedClass().getTestFilePaths();
    SupportedCompilers compilerType = config->getCompiler();
    SupportedTestFrameworks framework = config->getTestFramework();
    QString originalCppFilePath = config->getSelectedClass().getCppFilePath();
    QStringList sourceFiles = config->getSourceFiles();

    if (!sourceFiles.contains(originalCppFilePath)) {
        log << QString("Оригинальный файл %1 не найден в списке исходных файлов").arg(originalCppFilePath);
        return;
    }

    //Настройка пула потоков
    QThreadPool threadPool;
    int maxThreads = qMax(1, QThread::idealThreadCount() / config->getThreadDiv());
    threadPool.setMaxThreadCount(maxThreads);
    if (maxThreads == 1) {
        log << "Используется 1 поток из-за ограничений системы или настроек.";
    }

    QList<QFuture<void>> futures;
    QStringList localLogs; //Локальный список для накопления логов из всех потоков

    //Итерация по мутантам
    for (auto it = mutantsTypes.constBegin(); it != mutantsTypes.constEnd(); it++) {
        QString mutantFilePath = it.key();

        //Формирование пути к директории мутанта
        QString mutantDirPath = QDir::toNativeSeparators(QFileInfo(mutantFilePath).absolutePath());
        QString mutantDirName = QFileInfo(mutantDirPath).fileName();
        QString mutantFileName = QFileInfo(mutantFilePath).completeBaseName();
#ifdef Q_OS_WIN
        QString outputExecutablePath = mutantDirPath + QDir::separator() + mutantFileName + ".exe";
#else
        QString outputExecutablePath = mutantDirPath + QDir::separator() + mutantFileName;
#endif

        //Замена оригинального .cpp файла на файл мутанта в списке исходных файлов
        QStringList mutantSourceFiles = sourceFiles;
        mutantSourceFiles.removeAll(originalCppFilePath);
        mutantSourceFiles.append(mutantFilePath);

        //Лямбда-функция для выполнения компиляции и запуска
        auto runMutantTests = [this, &localLogs, mutantFilePath, mutantDirName, projectFolderPath, mutantSourceFiles,
                               testFiles, outputExecutablePath, compilerType, framework]() mutable {
            if (isStopping) {
                return;
            }

            BuildAndRunSubsystem localBuildRun;
            QString compileOutput;
            QStringList localLog;

            CompilationResults result = localBuildRun.buildAndRun(projectFolderPath, mutantSourceFiles,
                                                                    testFiles, outputExecutablePath,
                                                                    compileOutput, compilerType, framework);

            //Получение результатов тестов (включая пустой результат при ошибке)
            QHash<QString, QString> testResults = localBuildRun.getTestResults();

            //Синхронизация доступа к mutantTestResults
            QMutexLocker locker(&mutex);
            mutantTestResults[mutantFilePath] = testResults;

            switch (result) {
            case CompilationResults::Success: {
                localLog << QString("Тесты для мутанта %1 выполнены успешно: %2").arg(mutantDirName).arg(mutantFilePath);
                break;
            }
            case CompilationResults::CompilerError: {
                localLog << QString("Ошибка компилятора для мутанта %1").arg(mutantDirName);
                break;
            }
            case CompilationResults::CompilationError: {
                localLog << QString("Ошибка компиляции мутанта %1: %2").arg(mutantDirName).arg(mutantFilePath);
                localLog << compileOutput;
                break;
            }
            case CompilationResults::RunningError: {
                localLog << QString("Ошибка запуска исполняемого файла для мутанта %1: %2").arg(mutantDirName).arg(mutantFilePath);
                break;
            }
            case CompilationResults::EmptyTestResults: {
                localLog << QString("Ошибка тестирования мутанта %1: %2").arg(mutantDirName).arg(mutantFilePath);
                break;
            }
            }

            QMutexLocker logLocker(&logMutex);
            localLogs.append(localLog);

            processedMutants.insert(mutantFilePath);
        };

        QFuture<void> future = QtConcurrent::run(&threadPool, runMutantTests);
        futures.append(future);
    }

    for (auto &future : futures) {
        future.waitForFinished(); //Блокировка только для уже запущенных задач
    }

    cleanupUnanalyzedMutants();

    QMutexLocker logLocker(&logMutex);
    log.append(localLogs);

    log << QDateTime::currentDateTime().toString("HH:mm:ss") + QString(" -- Мутационный анализ завершён. Общее количество мутантов: %1").arg(mutantsTypes.size());
}

void MutationManagerSubsystem::cleanupWorkingDirectory()
{
    QDir dir(workingDirectoryPath);
    if (dir.exists()) {
        dir.removeRecursively();
    }
}

void MutationManagerSubsystem::cleanupUnanalyzedMutants()
{
    QDir dir(workingDirectoryPath);
    if (!dir.exists()) {
        return;
    }

    QStringList unanalyzedMutants;

    for (const QString &mutantPath : mutantsTypes.keys()) {
        if (!processedMutants.contains(mutantPath)) {
            unanalyzedMutants.append(mutantPath);
        }
    }

    for (const QString &mutantPath : unanalyzedMutants) {
        QDir mutantDir(QFileInfo(mutantPath).absolutePath());
        if (mutantDir.exists()) {
            mutantDir.removeRecursively();
        }
        mutantsTypes.remove(mutantPath); //Удаляем мутанта из mutantsTypes
    }
}

MutationManagerSubsystem::MutationManagerSubsystem(ConfigSubsystem *config, QStringList &log)
    : config(config)
{
    QString basePath = config->getFolderPath();
    workingDirectoryPath = basePath + QDir::separator() + "CppMutTest_WorkingDirectory";
    log << "Рабочая директория инициализарована: " + workingDirectoryPath;
}

MutationManagerSubsystem::~MutationManagerSubsystem()
{
    cleanupWorkingDirectory();
}

bool MutationManagerSubsystem::startMutationProcess(QStringList &log)
{
    if (!createWorkingDirectory(log)) {
        return false;
    }

    if (!distributeMutants(log)) {
        return false;
    }

    generateMutants(log);

    runTestsOnMutants(log);

    return !mutantTestResults.isEmpty();
}

QMap<QString, QHash<QString, QString>> MutationManagerSubsystem::getMutantTestResults() const
{
    return mutantTestResults;
}

QString MutationManagerSubsystem::getWorkingDirectoryPath() const
{
    return workingDirectoryPath;
}

QMap<QString, SupportedMutations> MutationManagerSubsystem::getMutantsTypes() const
{
    return mutantsTypes;
}

void MutationManagerSubsystem::stopMutationProcess()
{
    isStopping = true;
}
