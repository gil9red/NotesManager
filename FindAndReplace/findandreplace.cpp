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

#include "ui_findandreplace.h"
#include "findandreplace.h"

#include <QDebug>

FindAndReplace::FindAndReplace( QTextEdit * textedit, QWidget * parent )
    : QWidget( parent ),
      ui( new Ui::FindAndReplace ),
      d_editor ( textedit )
{
    ui->setupUi( this );
    highlighter = new Find_Highlighter( d_editor->document() );

    rehighlight();
    updateStates();

    timer.setSingleShot( true );
    connect( &timer, SIGNAL( timeout() ), SLOT( rehighlight() ) );
    setDelayInterval( 250 );

    connect( ui->findEdit, SIGNAL( cleansingText() ), SLOT( rehighlight() ) );
    connect( ui->findEdit, SIGNAL( findFlagsChanged() ), SLOT( rehighlight() ) );
}
FindAndReplace::~FindAndReplace()
{
    delete ui;
}

void FindAndReplace::setDelayInterval( int msec )
{
    timer.setInterval( msec );
}
int FindAndReplace::delayInterval()
{
    return timer.interval();
}


void FindAndReplace::on_findEdit_textEdited( const QString & text )
{
    Q_UNUSED( text )

    timer.start();
    updateStates();
}
void FindAndReplace::on_tButtonPrevious_clicked()
{
    find( false );
}
void FindAndReplace::on_tButtonNext_clicked()
{
    find();
}

void FindAndReplace::on_tButtonReplace_clicked()
{
    QTextCursor textCursor = d_editor->textCursor();
    if ( textCursor.hasSelection() )
        textCursor.insertText( ui->replaceEdit->text() );

    find();
}
void FindAndReplace::on_tButtonReplaceAll_clicked()
{
    d_editor->moveCursor( QTextCursor::Start, QTextCursor::MoveAnchor );

    find();

    d_editor->textCursor().beginEditBlock();
    while ( d_editor->textCursor().hasSelection() )
    {
        d_editor->textCursor().insertText( ui->replaceEdit->text() );
        find();
    }
    d_editor->textCursor().endEditBlock();
}

void FindAndReplace::find( bool next )
{
    QLineEdit * find = ui->findEdit;
    const QString & text = find->text();

    if ( text.isEmpty() )
        return;

    bool backward = !next;
    Shared::FindFlags findFlags = ui->findEdit->findOptions();
    bool caseSensitive = findFlags.testFlag( Shared::CaseSensitive );
    bool wholeWords = findFlags.testFlag( Shared::WholeWords );
    bool regExp = findFlags.testFlag( Shared::RegExp );

    QTextDocument::FindFlags flags;
    if ( backward )
        flags |= QTextDocument::FindBackward;

    if ( caseSensitive )
        flags |= QTextDocument::FindCaseSensitively;

    if ( wholeWords )
        flags |= QTextDocument::FindWholeWords;

    QTextDocument * document = d_editor->document();
    QTextCursor textCursor = d_editor->textCursor();
    do
    {
        if ( regExp )
            textCursor = document->find( QRegExp( text ), textCursor, flags );
        else
            textCursor = document->find( text, textCursor, flags );

        if ( textCursor.isNull() )
        {
            d_editor->moveCursor( next ? QTextCursor::Start : QTextCursor::End, QTextCursor::MoveAnchor );

            if ( regExp )
                textCursor = document->find( QRegExp( text ), textCursor, flags );
            else
                textCursor = document->find( text, textCursor, flags );
        }

        if ( textCursor.hasSelection() )
        {
            d_editor->setTextCursor( textCursor );
            break;
        }

    } while ( !textCursor.isNull() );

    d_editor->ensureCursorVisible();
    d_editor->setFocus();
}
void FindAndReplace::rehighlight()
{
    QLineEdit * find = ui->findEdit;
    const QString & text = find->text();
    Shared::FindFlags findFlags = ui->findEdit->findOptions();

    highlighter->setHighlightText( text, findFlags );
    highlighter->rehighlight();
}
void FindAndReplace::updateStates()
{
    const QString & textFind = ui->findEdit->text();
    bool isEmptyFind = textFind.isEmpty();

    ui->tButtonNext->setEnabled( !isEmptyFind );
    ui->tButtonPrevious->setEnabled( !isEmptyFind );
}


void FindAndReplace::hideEvent( QHideEvent * event )
{
    emit visibilityChanged( false );
    highlighter->clearHighlight();
    QWidget::hideEvent( event );
}
void FindAndReplace::showEvent( QShowEvent * event )
{
    emit visibilityChanged( true );
    highlighter->rehighlight();
    QWidget::showEvent( event );
}
