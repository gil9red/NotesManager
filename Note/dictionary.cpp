#include "dictionary.h"

#include <QTextStream>
#include <QFile>

Dictionary::Dictionary( QObject * parent )
    : QObject( parent ),
      d_language( "unknown" ),
      d_isLoad( false )
{

}
bool Dictionary::load( const QString & fileName )
{
    d_language = QFileInfo( fileName ).baseName();
    d_isLoad = false;

    QFile file( fileName );
    if ( !file.open( QFile::ReadOnly ) )
        return d_isLoad;

    QTextStream in( &file );
    in.setCodec( "utf8" );

    QStringList words;
    while ( !in.atEnd() )
        words << in.readLine();

    d_model.setStringList( words );
    d_isLoad = true;
    return d_isLoad;
}
bool Dictionary::isLoad()
{
    return d_isLoad;
}
QStringListModel * Dictionary::model()
{
    return &d_model;
}
QString Dictionary::language()
{
    return d_language;
}
