#ifndef TEXTTEMPLATEPARSER_H
#define TEXTTEMPLATEPARSER_H

#include <QString>
#include <QObject>
#include <QStringList>

//! Класс тестовых шаблонов.
/*! Шаблоны типа %.* будут заменены на соответствующие значения. */
class TextTemplateParser: public QObject
{
public:
    TextTemplateParser( QObject * parent = 0 );

    //! Получение текста и обработка его.
    static QString get( const QString & text );

    //! Получение списка шаблонов и их значений.
    static QStringList description();
};

#endif // TEXTTEMPLATEPARSER_H
