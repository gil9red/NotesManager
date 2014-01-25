/*************************************************************************
**
** Copyright (C) 2013-2014 by Ilya Petrash
** All rights reserved.
** Contact: gil9red@gmail.com
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


#include "find_highlighter.h"

#include <QDebug>
#include <QTextDocument>
#include <QTextCursor>


Find_Highlighter::Find_Highlighter( QTextDocument * document )
    : QSyntaxHighlighter( document )
{
    setHighlightColor( Qt::yellow );
}
void Find_Highlighter::setHighlightText( const QString & text, Shared::FindFlags flags )
{
    expression = text;
    findFlags = flags;
}
void Find_Highlighter::setHighlightColor( const QColor & color )
{
    format.setBackground( color );
}


void Find_Highlighter::clearHighlight()
{
    QString temp = expression;
    expression = "";
    rehighlight();
    expression = temp;
}


void Find_Highlighter::highlightBlock( const QString & text )
{
    if ( expression.isEmpty() )
        return;

    QTextDocument document( text );
    QTextDocument::FindFlags flags;
    if ( findFlags.testFlag( Shared::CaseSensitive ) )
        flags |= QTextDocument::FindCaseSensitively;

    if ( findFlags.testFlag( Shared::WholeWords ) )
        flags |= QTextDocument::FindWholeWords;

    QTextCursor textCursor;
    do
    {
        if ( findFlags.testFlag( Shared::RegExp ) )
            textCursor = document.find( QRegExp( expression ), textCursor, flags );
        else
            textCursor = document.find( expression, textCursor, flags );

        if ( textCursor.hasSelection() )
        {
            int start = textCursor.selectionStart();
            int end = textCursor.selectionEnd();
            int length = end - start;
            setFormat( start, length, format );
        }

    } while ( !textCursor.isNull() );
}
