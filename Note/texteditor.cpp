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

#include "texteditor.h"

#include <QApplication>
#include <QDebug>
#include <QDesktopServices>
#include <QMenu>


TextEditor::TextEditor( QWidget * parent ):
    QTextBrowser( parent )
{
    // Возвращаем функциональность текстового редактора и настраиваем
    // работы с гиперссылками
    setUndoRedoEnabled( true );
    setReadOnly( false );
    setOpenExternalLinks( false );
    setOpenLinks( false );
    setActivateLink( false );
    viewport()->setCursor( QCursor( Qt::IBeamCursor ) );
    connect( this, SIGNAL( anchorClicked(QUrl) ), SLOT( openLink(QUrl) ) );
}
void TextEditor::setActivateLink( bool activate )
{
    Qt::TextInteractionFlags flags;
    if ( activate )
        flags = Qt::LinksAccessibleByMouse;
    else
        flags = Qt::TextEditorInteraction;

    setTextInteractionFlags( flags );
}


void TextEditor::openLink( QUrl url )
{
    bool successful = QDesktopServices::openUrl( url );
    emit isOpenLink( successful );
    qDebug() << "TextEditor::openLink;" << successful << url;
}


void TextEditor::focusInEvent( QFocusEvent * event )
{
    // Если словарь не пуст
    if ( Completer::instance()->count() )
    {
        Completer::instance()->setEditor( this );

        // язык назначается при назначении редактора и меняется при смене раскладки
        QString lang = getTheCurrentLanguageKeyboardLayouts();
        Completer::instance()->setDictionary( lang );
    }

    QTextBrowser::focusInEvent( event );
}
void TextEditor::keyPressEvent( QKeyEvent * event )
{
    if( event->modifiers() == Qt::AltModifier )
        setActivateLink( true );

    QTextEdit::keyPressEvent( event );
}
void TextEditor::keyReleaseEvent( QKeyEvent * event )
{
    setActivateLink( false );
    QTextEdit::keyReleaseEvent( event );
}
void TextEditor::changeEvent( QEvent * event )
{
    if ( event->type() == QEvent::KeyboardLayoutChange )
    {
        // Если словарь не пуст
        if ( Completer::instance()->count() )
        {
            // язык назначается при назначении редактора и меняется при смене раскладки
            QString lang = getTheCurrentLanguageKeyboardLayouts();
            Completer::instance()->setDictionary( lang );
        }

    }

    QTextBrowser::changeEvent(event);
}
void TextEditor::contextMenuEvent( QContextMenuEvent * event )
{
    QMenu * menu = createStandardContextMenu();
    menu->addSeparator();

    QTextCursor cursor = cursorForPosition( event->pos() );
    qDebug();
    qDebug() << textCursor().currentTable()
             << textCursor().currentList()
             << textCursor().currentFrame();

    qDebug() << cursor.currentTable()
             << cursor.currentList()
             << cursor.currentFrame();

    if ( cursor.currentTable() )
        menu->addAction( "Table" );

    if ( cursor.currentList() )
        menu->addAction( "List" );

    if ( cursor.currentFrame() )
    {
        menu->addAction( "Frame" );
    }

    menu->exec( event->globalPos() );
    delete menu;
}
