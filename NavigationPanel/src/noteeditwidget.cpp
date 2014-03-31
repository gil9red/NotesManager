#include "noteeditwidget.h"
#include "ui_noteeditwidget.h"
#include "FormattingToolbar/FormattingToolbar.h"

NoteEditWidget::NoteEditWidget( QWidget * parent ) :
    QMainWindow( parent ),
    ui( new Ui::NoteEditWidget ),
    d_note(0)
{
    ui->setupUi( this );
    setContextMenuPolicy( Qt::NoContextMenu );

    FormattingToolbar * formattingToolbar = new FormattingToolbar( this );
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

    ui->title->setText( richTextNote->title() );
    ui->editor->setDocument( richTextNote->document() );

    connect( ui->title, SIGNAL( editingFinished() ), SLOT( titleChange() ) );
    connect( richTextNote, SIGNAL( changed(int) ), SLOT( noteChange(int) ) );
}
Note * NoteEditWidget::note()
{
    return d_note;
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
