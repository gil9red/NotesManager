#include "texteditor.h"

#include <QApplication>
#include <QDebug>
#include <QDesktopServices>
#include <QMenu>
#include <QToolTip>

TextEditor::TextEditor( QWidget * parent ):
    QTextBrowser( parent )
{
    // Возвращаем функциональность текстового редактора и настраиваем работу с гиперссылками
    setUndoRedoEnabled( true );
    setReadOnly( false );

    anchorTooltipTimer.setInterval( 1000 );
    anchorTooltipTimer.setSingleShot( true );
    QObject::connect( &anchorTooltipTimer, SIGNAL(timeout()), SLOT(sl_AnchorTooltipTimer_Timeout()) );
}

void TextEditor::sl_AnchorTooltipTimer_Timeout()
{
    QString href = anchorAt( mapFromGlobal( QCursor::pos() ) );
    if ( !href.isEmpty() )
        QToolTip::showText( QCursor::pos(), href + "\n" + tr( "Ctrl+Click to go" ), this );
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

void TextEditor::mouseMoveEvent( QMouseEvent * event )
{
    QTextBrowser::mouseMoveEvent( event );

    const QString & href = anchorAt( event->pos() );
    if ( !href.isEmpty() )
    {
        anchorTooltipTimer.start();

        if ( event->modifiers() == Qt::ControlModifier && viewport()->cursor().shape() != Qt::PointingHandCursor )
            viewport()->setCursor( Qt::PointingHandCursor );
    } else
    {
        if ( viewport()->cursor().shape() != Qt::IBeamCursor )
            viewport()->setCursor( Qt::IBeamCursor );

        if ( anchorTooltipTimer.isActive() )
            anchorTooltipTimer.stop();
        if ( QToolTip::isVisible() )
            QToolTip::hideText();
    }
}
void TextEditor::mousePressEvent( QMouseEvent * event )
{
    QTextBrowser::mousePressEvent( event );

    if ( event->modifiers() == Qt::ControlModifier && event->button() == Qt::LeftButton )
    {
        const QString & href = anchorAt( event->pos() );
        if ( !href.isEmpty() )
        {
            QUrl url( href );

            QDesktopServices::openUrl( QFileInfo( href ).isFile() ? QUrl::fromLocalFile( href ) : url );
            emit sg_LinkClicked( url );
        }
    }
}
