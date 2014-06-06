#include "noteeditwidget.h"
#include "ui_noteeditwidget.h"
#include <QDockWidget>

QAction * addSeparator( QObject * parent = 0 )
{
    QAction * separator = new QAction( parent );
    separator->setSeparator( true );
    return separator;
}

NoteEditWidget::NoteEditWidget( QWidget * parent ) :
    QMainWindow( parent ),
    ui( new Ui::NoteEditWidget ),
    d_note(0)
{
    ui->setupUi( this );
    setContextMenuPolicy( Qt::NoContextMenu );

    // Форматирование текста
    {
        formattingToolbar = new FormattingToolbar( this );
        formattingToolbar->setAlterActivityComponents( false );
        formattingToolbar->hide();
        formattingToolbar->installConnect( ui->editor );

        QToolBar * toolBar = formattingToolbar->mainToolBar();        
        toolBar->setMovable( false );
        addToolBar( toolBar );
    }

    // Поиск и замена
    {
        FindAndReplace * findAndReplace = new FindAndReplace( ui->editor );
        findAndReplace->setHighlight( false );
        findAndReplace->removeBottomVerticalSpacer();
        findAndReplace->setAutoRaiseNextAndPrevious( true );
        findAndReplace->setVisible( false );

        ui->editorLayout->addWidget( findAndReplace );

        ui->actionFindAndReplace->setChecked( findAndReplace->isVisible() );

        QObject::connect( findAndReplace, SIGNAL(visibilityChanged(bool)), ui->actionFindAndReplace, SLOT(setChecked(bool)) );
        QObject::connect( ui->actionFindAndReplace, SIGNAL(triggered(bool)), findAndReplace, SLOT(setVisible(bool)) );

        ui->editor->addAction( addSeparator() );
        ui->editor->addAction( ui->actionFindAndReplace );
    }


    ui->properties->setVisible( ui->showProperties->arrowType() == Qt::UpArrow );

    QVBoxLayout * propertiesLayout = new QVBoxLayout();
    ui->properties->setLayout( propertiesLayout );

    attachPanel = new AttachPanel( this );
    propertiesLayout->addWidget( attachPanel );
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
    attachPanel->setNote( richTextNote );

    ui->title->setText( richTextNote->title() );
    ui->editor->setDocument( richTextNote->document() );
    ui->editor->setNote( richTextNote );

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

QList < QAction * > NoteEditWidget::editActions()
{
    QList < QAction * > list = getEditor()->createStandardContextMenu()->actions();
    if ( !ui->editor->actions().isEmpty() )
    {
        list << addSeparator();
        list << ui->editor->actions();
    }
    return list;
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
void NoteEditWidget::on_showProperties_clicked()
{
    Qt::ArrowType arrowType = ui->showProperties->arrowType();
    if ( arrowType == Qt::UpArrow )
    {
        ui->showProperties->setArrowType( Qt::DownArrow );
        ui->properties->setVisible( false );

    } else if ( arrowType == Qt::DownArrow )
    {
        ui->showProperties->setArrowType( Qt::UpArrow );
        ui->properties->setVisible( true );

    } else
        ui->showProperties->setArrowType( Qt::NoArrow );
}
