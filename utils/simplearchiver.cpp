#include "simplearchiver.h"
#include "quazipfile.h"
#include <QDir>

bool SimpleArchiver::copyData( QIODevice & inFile, QIODevice & outFile )
{
    while ( !inFile.atEnd() )
    {
        char buf[4096];
        qint64 readLen = inFile.read( buf, 4096 );
        if ( readLen <= 0 )
            return false;
        if ( outFile.write( buf, readLen ) != readLen )
            return false;
    }
    return true;
}
bool SimpleArchiver::compressFile( QuaZip * zip, const QString & fileName, const QString & fileDest )
{
    if ( zip->getMode() != QuaZip::mdCreate
         && zip->getMode() != QuaZip::mdAppend
         && zip->getMode() != QuaZip::mdAdd )
        return false;

    QFile inFile;
    inFile.setFileName( fileName );
    if ( !inFile.open( QIODevice::ReadOnly ) )
        return false;

    QuaZipFile outFile( zip );
    if ( !outFile.open( QIODevice::WriteOnly, QuaZipNewInfo( fileDest, inFile.fileName() ) ) )
        return false;

    if ( !copyData( inFile, outFile ) || outFile.getZipError() != UNZ_OK )
        return false;

    inFile.close();
    outFile.close();
    if ( outFile.getZipError() != UNZ_OK )
        return false;

    return true;
}
bool SimpleArchiver::compressSubDir( QuaZip * zip, const QString & dir, const QString & origDir )
{
    foreach (const QFileInfo & file, QDir(dir).entryInfoList( QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot ))
    {
        QString filename = QFileInfo(origDir).fileName() + "/" + QDir(origDir).relativeFilePath(file.absoluteFilePath());

        if ( file.isDir() )
        {
            QuaZipFile outFile( zip );
            outFile.open( QIODevice::WriteOnly, QuaZipNewInfo( filename + "/" ) );
            outFile.close();

            if ( !compressSubDir( zip, file.absoluteFilePath(), origDir ) )
                return false;
        } else
        {
            if ( file.absoluteFilePath() == zip->getZipName() )
                continue;

            if ( !compressFile( zip, file.absoluteFilePath(), filename ) )
                return false;
        }
    }

    return true;
}
bool SimpleArchiver::extractFile( QuaZip * zip, const QString & fileName, const QString & fileDest )
{
    if ( !fileName.isEmpty() )
        zip->setCurrentFile( fileName );

    QuaZipFile inFile( zip );
    if ( !inFile.open( QIODevice::ReadOnly ) || inFile.getZipError() != UNZ_OK )
        return false;

    QFileInfo fileInfo( fileDest );
    if ( !QDir().mkpath( fileInfo.absolutePath() ) )
        return false;

    QuaZipFileInfo zipFileInfo;
    if ( !zip->getCurrentFileInfo( &zipFileInfo ) )
        return false;

    if ( fileDest.endsWith( '/' ) && fileInfo.isDir() )
        return QFile( fileDest ).setPermissions( zipFileInfo.getPermissions() );

    QFile outFile;
    outFile.setFileName( fileDest );
    if ( !outFile.open( QIODevice::WriteOnly ) )
        return false;

    if ( !copyData( inFile, outFile ) || inFile.getZipError() != UNZ_OK )
    {
        outFile.close();
        return false;
    }

    inFile.close();
    outFile.close();
    if ( inFile.getZipError() != UNZ_OK )
        return false;

    return outFile.setPermissions( zipFileInfo.getPermissions() );
}

bool SimpleArchiver::compressDir( const QString & fileCompressed, const QString & dir )
{
    QuaZip zip( fileCompressed );
    zip.setFileNameCodec( "IBM866" );

    if ( !zip.open( QuaZip::mdCreate ) )
        return false;

    if ( !compressSubDir( &zip, dir, dir ) )
        return false;

    zip.close();
    if ( zip.getZipError() != UNZ_OK )
        return false;

    return true;
}
bool SimpleArchiver::extractDir( const QString & zipFileName, const QString & dest )
{
    QuaZip zip( zipFileName );
    zip.setFileNameCodec( "IBM866" );

    if ( !zip.open( QuaZip::mdUnzip ) )
        return false;

    for ( bool f = zip.goToFirstFile(); f; f = zip.goToNextFile() )
    {
        QString filePath = zip.getCurrentFileName();
        QString absPath = dest + "/" + filePath;

        if ( absPath.endsWith( "/" ) )
        {
            if ( !QDir().mkpath( absPath ) )
                return false;
        }else
        {
            if ( !extractFile( &zip, filePath, absPath ) )
                return false;
        }
    }

    return true;
}
