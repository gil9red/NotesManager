/*************************************************************************
**
** Copyright (C) 2013-2014 by Ilya Petrash
** All rights reserved.
** Contact: gil9red@gmail.com, ip1992@inbox.ru
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 3 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the
** Free Software Foundation, Inc.,
** 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
**
**************************************************************************/


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
