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
    QStringList list;
    list << QString( "d" )    + "=" + tr( "the day as number without a leading zero (1 to 31)" )
         << QString( "dd" )   + "=" + tr( "the day as number with a leading zero (01 to 31)" )
         << QString( "ddd" )  + "=" + tr( "the abbreviated localized day name (e.g. 'Mon' to 'Sun')" )
         << QString( "dddd" ) + "=" + tr( "the long localized day name (e.g. 'Monday' to 'Qt::Sunday')" )
         << QString( "M" )    + "=" + tr( "the month as number without a leading zero (1-12)" )
         << QString( "MM" )   + "=" + tr( "the month as number with a leading zero (01-12)" )
         << QString( "MMM" )  + "=" + tr( "the abbreviated localized month name (e.g. 'Jan' to 'Dec')" )
         << QString( "MMMM" ) + "=" + tr( "the long localized month name (e.g. 'January' to 'December')" )
         << QString( "yy" )   + "=" + tr( "the year as two digit number (00-99)" )
         << QString( "yyyy" ) + "=" + tr( "the year as four digit number" );

    list << QString( "h" )   + "=" + tr( "the hour without a leading zero (0 to 23 or 1 to 12 if AM/PM display)" )
         << QString( "hh" )  + "=" + tr( "the hour with a leading zero (00 to 23 or 01 to 12 if AM/PM display)" )
         << QString( "m" )   + "=" + tr( "the minute without a leading zero (0 to 59)" )
         << QString( "mm" )  + "=" + tr( "the minute with a leading zero (00 to 59)" )
         << QString( "s" )   + "=" + tr( "the second without a leading zero (0 to 59)" )
         << QString( "ss" )  + "=" + tr( "the second with a leading zero (00 to 59)" )
         << QString( "z" )   + "=" + tr( "the milliseconds without leading zeroes (0 to 999)" )
         << QString( "zzz" ) + "=" + tr( "the milliseconds with leading zeroes (000 to 999)" )
         << QString( "AP" )  + "=" + tr( "use AM/PM display. AP will be replaced by either \"AM\" or \"PM\"" )
         << QString( "ap" )  + "=" + tr( "use am/pm display. ap will be replaced by either \"am\" or \"pm\"" );

    return list;
}
QString TextTemplateParser::descriptionToHtmlTable()
{
    QString table = "<table>";
    foreach ( QString row, description() )
    {
        QStringList columns = row.split( "=" );
        QString value = columns.at( 0 );
        QString desc = columns.at( 1 );

        table += "<tr>";
        table += QString( "<td><b>%1</b></td><td>%2</td>" ).arg( value ).arg( desc );
        table += "</tr>";
    }
    table += "</table>";
    return table;
}
