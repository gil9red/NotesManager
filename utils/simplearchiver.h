#ifndef SIMPLEARCHIVER_H
#define SIMPLEARCHIVER_H

#include "quazip.h"

class SimpleArchiver
{
private:
    static bool copyData( QIODevice & inFile, QIODevice & outFile );
    static bool compressFile( QuaZip * zip, const QString & fileName, const QString & fileDest );
    static bool compressSubDir( QuaZip * zip, const QString & dir, const QString & origDir );
    static bool extractFile( QuaZip * zip, const QString & fileName, const QString & fileDest );

public:
    static bool compressDir( const QString & fileCompressed, const QString & dir );
    static bool extractDir( const QString & zipFileName, const QString & dest );
};

static QString getDirInZip( const QString & zipFileName )
{
    QuaZip zip( zipFileName );
    zip.setFileNameCodec( "IBM866" );

    if ( !zip.open( QuaZip::mdUnzip ) )
        return false;

    zip.goToFirstFile();
    QString dir = zip.getCurrentFileName();

    if ( dir.endsWith( "/" ) )
        return dir;
    else
        return QString();

    return QString();
}

#endif // SIMPLEARCHIVER_H
