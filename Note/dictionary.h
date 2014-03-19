#ifndef DICTIONARY_H
#define DICTIONARY_H

#include <QStringListModel>
#include <QFileInfo>

//! Класс-словарь.
/*! Хранит список слов, умеет списки слов загружать из файлов.
 *  \sa Completer
 */
class Dictionary: public QObject
{
  Q_OBJECT

public:
    Dictionary( QObject * parent = 0 );

    //! Функция загружает из файла список слов.
    /*! \param fileName путь к файлу
     *  \return true, если загрузка прошла удачно, иначе false
     */
    bool load( const QString & fileName );

    //! Функция вернет true, если загрузка из файла прошла успешно.
    bool isLoad();

    //! Функция вернет указатель на модель словаря
    QStringListModel * model();

    //! Функция вернет название языка словаря
    QString language();

private:
    QStringListModel d_model; //!< Модель
    QString d_language;       //!< Название языка
    bool d_isLoad;            //!< true - если загрузка прошла удачно
};

#endif // DICTIONARY_H
