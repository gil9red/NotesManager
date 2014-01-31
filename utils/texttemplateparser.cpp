#include "texttemplateparser.h"

#include <QTextDocument>
#include <QTextCursor>
#include <QDateTime>
#include <QDebug>

TextTemplateParser::TextTemplateParser( QObject * parent )
    : QObject( parent )
{
}

QString TextTemplateParser::get( const QString & text )
{
    QTextDocument document( text );
    QTextCursor textCursor;
    QTextDocument::FindFlags flags = QTextDocument::FindCaseSensitively;
    QRegExp expression( "%[^ %]+%" );
    QDateTime dateTime = QDateTime::currentDateTime();

    do
    {
        textCursor = document.find( expression, textCursor, flags );

        if ( textCursor.hasSelection() )
        {
            QString selected = textCursor.selectedText();
            QString replaceText;

            // Удаление символов %, с начала и с конца строки
            selected = selected.remove( 0, 1 );
            selected = selected.remove( selected.size() - 1, 1 );

            if ( selected == "TextDate" )
                replaceText = dateTime.toString( Qt::TextDate );
            else if ( selected == "ISODate" )
                replaceText = dateTime.toString( Qt::ISODate );
            else if ( selected == "SystemLocaleShortDate" )
                replaceText = dateTime.toString( Qt::SystemLocaleShortDate );
            else if ( selected == "SystemLocaleLongDate" )
                replaceText = dateTime.toString( Qt::SystemLocaleLongDate );
            else
                replaceText = dateTime.toString( selected );

            if ( !replaceText.isEmpty() )
                textCursor.insertText( replaceText );
        }

    } while ( !textCursor.isNull() );

    return document.toPlainText();
}
QStringList TextTemplateParser::description()
{
    // TODO: закончить
    QStringList list;

    return list;
}
