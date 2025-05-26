#pragma once
#include <QString>
#include <QRegularExpression>

///
/// \brief The IMutation class - абстрактный класс для всех типов мутаций
///
class IMutation
{
protected:
    ///
    /// \brief maskContent - маскирует строки, комментарии и директивы препроцессора
    /// \param src - исходный текст
    /// \return QString - маскированный текст той же длины
    ///
    virtual QString maskContent(const QString &src) const;

    ///
    /// \brief findOps - находит позиции и значения по регулярному выражению
    /// \param masked - маскированный код
    /// \param rx - регулярное выражение для поиска операторов
    /// \return QVector пар <позиция, оператор>
    ///
    virtual QVector<QPair<int, QString>> findOps(const QString &masked, const QRegularExpression &rx) const;

public:
    virtual ~IMutation() = default;

    ///
    /// \brief mutate - применяет мутацию к .cpp файлу
    /// \param filePath - абсолютный путь к .cpp файлу, который нужно мутировать
    ///
    virtual void mutate(const QString &filePath) = 0;

    ///
    /// \brief isAvailable - поиск наличия точек мутации в .cpp файле
    /// \param filePath - абсолютный путь к .cpp файлу
    /// \return true, если мутация доступна для исходного кода .cpp файла
    ///
    virtual bool isAvailable(const QString &filePath) = 0;
};
