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

            if ( selected == "td" )
                replaceText = dateTime.toString( Qt::TextDate );

            else if ( selected == "iso" )
                replaceText = dateTime.toString( Qt::ISODate );

            else if ( selected == "short" )
                replaceText = dateTime.toString( Qt::SystemLocaleShortDate );

            else if ( selected == "long" )
                replaceText = dateTime.toString( Qt::SystemLocaleLongDate );

            else
                replaceText = dateTime.toString( selected );

            if ( !replaceText.isEmpty() )
                textCursor.insertText( replaceText );
        }

    } while ( !textCursor.isNull() );

    return document.toPlainText();
}
#include <QTranslator>
QStringList TextTemplateParser::description()
{
    QStringList list;
    list << QString( "d" )    + "=" + QTranslator::tr( "the day as number without a leading zero (1 to 31)" )
         << QString( "dd" )   + "=" + QTranslator::tr( "the day as number with a leading zero (01 to 31)" )
         << QString( "ddd" )  + "=" + QTranslator::tr( "the abbreviated localized day name (e.g. 'Mon')" )
         << QString( "dddd" ) + "=" + QTranslator::tr( "the long localized day name (e.g. 'Monday')" )
         << QString( "M" )    + "=" + QTranslator::tr( "the month as number without a leading zero (1-12)" )
         << QString( "MM" )   + "=" + QTranslator::tr( "the month as number with a leading zero (01-12)" )
         << QString( "MMM" )  + "=" + QTranslator::tr( "the abbreviated localized month name (e.g. 'Jan')" )
         << QString( "MMMM" ) + "=" + QTranslator::tr( "the long localized month name (e.g. 'January')" )
         << QString( "yy" )   + "=" + QTranslator::tr( "the year as two digit number (00-99)" )
         << QString( "yyyy" ) + "=" + QTranslator::tr( "the year as four digit number" );

    list << QString( "h" )   + "=" + QTranslator::tr( "the hour without a leading zero (0 to 23 or 1 to 12 if AM/PM display)" )
         << QString( "hh" )  + "=" + QTranslator::tr( "the hour with a leading zero (00 to 23 or 01 to 12 if AM/PM display)" )
         << QString( "m" )   + "=" + QTranslator::tr( "the minute without a leading zero (0 to 59)" )
         << QString( "mm" )  + "=" + QTranslator::tr( "the minute with a leading zero (00 to 59)" )
         << QString( "s" )   + "=" + QTranslator::tr( "the second without a leading zero (0 to 59)" )
         << QString( "ss" )  + "=" + QTranslator::tr( "the second with a leading zero (00 to 59)" )
         << QString( "z" )   + "=" + QTranslator::tr( "the milliseconds without leading zeroes (0 to 999)" )
         << QString( "zzz" ) + "=" + QTranslator::tr( "the milliseconds with leading zeroes (000 to 999)" )
         << QString( "AP" )  + "=" + QTranslator::tr( "use AM/PM display. AP will be replaced by either \"AM\" or \"PM\"" )
         << QString( "ap" )  + "=" + QTranslator::tr( "use am/pm display. ap will be replaced by either \"am\" or \"pm\"" );

    list << QString( "td" )    + "=" + QTranslator::tr( "the format, which includes the day and month name,"
                                                        "the day number in the month, and the year in full."
                                                        "The day and month names will be short, localized names" )
         << QString( "iso" )   + "=" + QTranslator::tr( "ISO 8601 extended format" )
         << QString( "short" ) + "=" + QTranslator::tr( "the short format used by the operating system" )
         << QString( "long" )  + "=" + QTranslator::tr( "the long format used by the operating system" );

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
