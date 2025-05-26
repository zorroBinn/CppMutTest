#include "userclassinfo.h"

UserClassInfo::UserClassInfo(const QString &className) : className(className), hFilePath(""), cppFilePath(""), testFilePaths({}), availableMutations({}) {}

bool UserClassInfo::hasUnitTests() const
{
    return !testFilePaths.isEmpty();
}

QString UserClassInfo::getClassName() const
{
    return className;
}

void UserClassInfo::setClassName(const QString &name)
{
    className = name;
}

QString UserClassInfo::getHFilePath() const
{
    return hFilePath;
}

void UserClassInfo::setHFilePath(const QString &path)
{
    hFilePath = path;
}

QString UserClassInfo::getCppFilePath() const
{
    return cppFilePath;
}

void UserClassInfo::setCppFilePath(const QString &path)
{
    cppFilePath = QDir::toNativeSeparators(path);
}

QVector<QString> UserClassInfo::getTestFilePaths() const
{
    return testFilePaths;
}

void UserClassInfo::setTestFilePaths(const QVector<QString> &paths)
{
    testFilePaths.clear();
    for (const QString &path : paths) {
        testFilePaths.append(QDir::toNativeSeparators(path));
    }
}

void UserClassInfo::addTestFilePath(const QString &path)
{
    testFilePaths.append(QDir::toNativeSeparators(path));
}

QVector<QString> UserClassInfo::getMethodNames() const
{
    return methodNames;
}

void UserClassInfo::setMethodNames(const QVector<QString> &names)
{
    methodNames = names;
}

void UserClassInfo::addMethodName(const QString &name)
{
    methodNames.append(name);
}

QVector<QString> UserClassInfo::getUnitTestNames() const
{
    return unitTestNames;
}

void UserClassInfo::setUnitTestNames(const QVector<QString> &names)
{
    unitTestNames = names;
}

void UserClassInfo::addUnitTestName(const QString &name)
{
    unitTestNames.append(name);
}

QVector<SupportedMutations> UserClassInfo::getAvailableMutations() const
{
    return availableMutations;
}

void UserClassInfo::setAvailableMutations(const QVector<SupportedMutations> &mutations)
{
    availableMutations = mutations;
}

QMap<QString, QString> UserClassInfo::getOriginalTestsResults() const
{
    return originalTestsResults;
}

void UserClassInfo::setOriginalTestsResults(const QMap<QString, QString> &newOriginalTestsResults)
{
    originalTestsResults = newOriginalTestsResults;
}
