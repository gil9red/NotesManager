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
