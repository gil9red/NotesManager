#include "noteeditwidget.h"
#include "ui_noteeditwidget.h"

NoteEditWidget::NoteEditWidget( QWidget * parent ) :
    QMainWindow( parent ),
    ui( new Ui::NoteEditWidget ),
    d_note(0)
{
    ui->setupUi( this );
    setContextMenuPolicy( Qt::NoContextMenu );

    formattingToolbar = new FormattingToolbar( this );
    formattingToolbar->setAlterActivityComponents( false );
    formattingToolbar->hide();
    formattingToolbar->installConnect( ui->editor );

    QToolBar * toolBar = formattingToolbar->mainToolBar();
    toolBar->setIconSize( QSize( 17, 17 ) );
    toolBar->setMovable( false );
    addToolBar( toolBar );
}
NoteEditWidget::~NoteEditWidget()
{
    delete ui;
}

void NoteEditWidget::setNote( Note * note )
{
    if ( !note )
    {
        WARNING( "null pointer!" );
        return;
    }

    d_note = note;
    RichTextNote * richTextNote = d_note->getRichTextNote();
    formattingToolbar->setNote( richTextNote );

    ui->title->setText( richTextNote->title() );
    ui->editor->setDocument( richTextNote->document() );

    QObject::connect( ui->title, SIGNAL( editingFinished() ), SLOT( titleChange() ) );
    QObject::connect( richTextNote, SIGNAL( changed(int) ), SLOT( noteChange(int) ) );
}
Note * NoteEditWidget::note()
{
    return d_note;
}

QString NoteEditWidget::saveStateNoteEdit()
{
    QStringList list;

    QTextCursor cursor = ui->editor->textCursor();
    int pos = cursor.selectionStart();
    int lenght = cursor.selectionEnd() - cursor.selectionStart();

    list << QString::number( pos ) << QString::number( lenght );
    return list.join(",");
}
void NoteEditWidget::restoreStateNoteEdit( const QString & state )
{
    int pos;
    int lenght;

    QStringList list = state.split(",");
    pos = list.takeFirst().toInt();
    lenght = list.takeFirst().toInt();

    QTextCursor cursor = ui->editor->textCursor();
    cursor.setPosition( pos );
    cursor.movePosition( QTextCursor::Right, QTextCursor::KeepAnchor, lenght );
    ui->editor->setTextCursor( cursor );
    ui->editor->ensureCursorVisible();
}

TextEditor * NoteEditWidget::getEditor()
{
    return ui->editor;
}

void NoteEditWidget::noteChange( int event )
{
    if ( !d_note )
    {
        WARNING( "no init!" );
        return;
    }

    RichTextNote * richTextNote = d_note->getRichTextNote();

    switch ( event )
    {
    case EventsNote::ChangeTitle:
        ui->title->setText( richTextNote->title() );
        break;
    }
}
void NoteEditWidget::titleChange()
{
    if ( !d_note )
    {
        WARNING( "no init!" );
        return;
    }

    RichTextNote * richTextNote = d_note->getRichTextNote();
    richTextNote->setTitle( ui->title->text() );
}
