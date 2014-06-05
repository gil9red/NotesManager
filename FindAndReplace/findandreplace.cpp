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

    setHighlight( true );
    updateStates();

    timer.setSingleShot( true );
    QObject::connect( &timer, SIGNAL( timeout() ), SLOT( rehighlight() ) );
    setDelayInterval( 250 );

    QObject::connect( ui->findEdit, SIGNAL( cleansingText() ), SLOT( rehighlight() ) );
    QObject::connect( ui->findEdit, SIGNAL( findFlagsChanged() ), SLOT( rehighlight() ) );
}
FindAndReplace::~FindAndReplace()
{
    delete ui;
}

void FindAndReplace::removeBottomVerticalSpacer()
{
    layout()->removeItem( ui->bottomVerticalSpacer );
}
void FindAndReplace::setAutoRaiseNextAndPrevious( bool autoRaise )
{
    ui->next->setAutoRaise( autoRaise );
    ui->previous->setAutoRaise( autoRaise );
}

void FindAndReplace::setDelayInterval( int msec )
{
    timer.setInterval( msec );
}
int FindAndReplace::delayInterval()
{
    return timer.interval();
}

void FindAndReplace::setHighlight( bool highlight )
{
    d_highlight = highlight;
}
bool FindAndReplace::highlight()
{
    return d_highlight;
}

void FindAndReplace::on_findEdit_textEdited( const QString & text )
{
    Q_UNUSED( text )

    timer.start();
    updateStates();
}
void FindAndReplace::on_previous_clicked()
{
    find( false );
}
void FindAndReplace::on_next_clicked()
{
    find();
}

void FindAndReplace::on_replace_clicked()
{
    QTextCursor textCursor = d_editor->textCursor();
    if ( textCursor.hasSelection() )
        textCursor.insertText( ui->replaceEdit->text() );

    find();
}
void FindAndReplace::on_replaceAll_clicked()
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
    if ( !d_highlight )
        return;

    QLineEdit * find = ui->findEdit;
    const QString & text = find->text();
    Shared::FindFlags findFlags = ui->findEdit->findOptions();

    if ( highlighter )
    {
        highlighter->setHighlightText( text, findFlags );
        highlighter->rehighlight();
    }
}
void FindAndReplace::updateStates()
{
    const QString & textFind = ui->findEdit->text();
    bool isEmpty = textFind.isEmpty();

    ui->next->setEnabled( !isEmpty );
    ui->previous->setEnabled( !isEmpty );
}

void FindAndReplace::hideEvent( QHideEvent * event )
{
    emit visibilityChanged( false );
    if ( highlighter && d_highlight )
        highlighter->clearHighlight();
    QWidget::hideEvent( event );
}
void FindAndReplace::showEvent( QShowEvent * event )
{
    emit visibilityChanged( true );
    ui->findEdit->setFocus();

    if ( highlighter && d_highlight )
        highlighter->rehighlight();

    QWidget::showEvent( event );
}
