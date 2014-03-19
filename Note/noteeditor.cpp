#include "noteeditor.h"

#include "FormattingToolbar.h"

NoteEditor::NoteEditor( QWidget * parent ) :
    QMainWindow( parent ),
    ui( new Ui::NoteEditor ),
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
NoteEditor::~NoteEditor()
{
    delete ui;
}

void NoteEditor::setNote( RichTextNote * note )
{
    d_note = note;

    ui->title->setText( d_note->title() );
    ui->editor->setDocument( d_note->document() );

    connect( ui->title, SIGNAL( editingFinished() ), SLOT( titleChange() ) );
    connect( d_note, SIGNAL( changed(int) ), SLOT( noteChange(int) ) );
}
RichTextNote * NoteEditor::note()
{
    return d_note;
}

void NoteEditor::noteChange( int event )
{
    switch ( event )
    {
    case EventsNote::ChangeTitle:
        ui->title->setText( d_note->title() );
        break;
    }
}

void NoteEditor::titleChange()
{
    d_note->setTitle( ui->title->text() );
}
