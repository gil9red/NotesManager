#include "page_notes.h"

#include <QMenu>
#include <QColorDialog>
#include <QDomDocument>
#include "appinfo.h"
#include "utils/func.h"
#include "fullscreenshotcropper.h"
#include <QClipboard>
#include <QDesktopWidget>
#include "NavigationPanel/src/hierarchymodel.h"
#include "NavigationPanel/src/datenavigationwidget.h"
#include "NavigationPanel/src/notebook.h"
#include "NavigationPanel/src/noteeditwidget.h"
#include "NavigationPanel/src/basemodelitem.h"

Page_Notes::Page_Notes( QWidget * parent ) :
    QMainWindow( parent ),
    ui( new Ui::Page_notes ),
    settings(0)
{
    ui->setupUi( this );

    QObject::connect(ui->tab_Notes, SIGNAL(sg_NoteDoubleClicked(Note*)), ui->tabWidget_EditNotes, SLOT(openNote(Note*)));
    QObject::connect(ui->tab_Tags, SIGNAL(sg_NoteDoubleClicked(Note*)), ui->tabWidget_EditNotes, SLOT(openNote(Note*)));
    QObject::connect(ui->tab_Dates, SIGNAL(sg_NoteDoubleClicked(Note*)), ui->tabWidget_EditNotes, SLOT(openNote(Note*)));

    QObject::connect( ui->tabWidget_Navigation, SIGNAL(currentChanged(int)), SIGNAL(sg_About_UpdateStates()) );
    QObject::connect( ui->tab_Notes, SIGNAL(sg_SelectedItemsActionsListChanged()), SIGNAL(sg_About_UpdateStates()) );
    QObject::connect( Notebook::instance(), SIGNAL(sg_ItemRegistered(Tag*)), SIGNAL(sg_About_UpdateStates()) );
    QObject::connect( Notebook::instance(), SIGNAL(sg_ItemRegistered(Note*)), SIGNAL(sg_About_UpdateStates()) );
    QObject::connect( Notebook::instance(), SIGNAL(sg_ItemRegistered(Folder*)), SIGNAL(sg_About_UpdateStates()) );
    QObject::connect( Notebook::instance(), SIGNAL(sg_ItemUnregistered(Tag*)), SIGNAL(sg_About_UpdateStates()) );
    QObject::connect( Notebook::instance(), SIGNAL(sg_ItemUnregistered(Tag*)), SIGNAL(sg_About_UpdateStates()) );
    QObject::connect( Notebook::instance(), SIGNAL(sg_ItemUnregistered(Tag*)), SIGNAL(sg_About_UpdateStates()) );
}
Page_Notes::~Page_Notes()
{
    delete ui;
}

void Page_Notes::setSettings( QSettings * s )
{
    settings = s;
}

void Page_Notes::read( QIODevice * device )
{
    QDomDocument xmlDomDocument;
    xmlDomDocument.setContent( device );

    // Корнем является тэг Notebook
    QDomElement root = xmlDomDocument.documentElement();

    // Загрузка заметок в модель
    {
        Notebook::instance()->read( root );

        ui->tab_Notes->setModel( Notebook::instance()->hierarchyModel() );
        ui->tab_Tags->SetModel( Notebook::instance()->tagsModel() );
        ui->tab_Dates->SetCreationModel( Notebook::instance()->creationDateModel() );
        ui->tab_Dates->SetModificationModel( Notebook::instance()->modificationDateModel() );
    }

    // Загрузка вкладок
    {
        QDomElement rootTabs = root.firstChildElement( "Tabs" );
        QDomElement dom_tab = rootTabs.firstChildElement();
        while( !dom_tab.isNull() )
        {
            const QString & id = dom_tab.attribute( "id" );
            if ( id.isEmpty() )
            {
                dom_tab = dom_tab.nextSiblingElement();
                continue;
            }

            Note * note = Notebook::instance()->getNoteFromId( id );
            ui->tabWidget_EditNotes->openNote( note );

            // Восстанавливаем состояние редактора заметок
            {
                QWidget * tab =  ui->tabWidget_EditNotes->getWidgetTab( note );
                NoteEditWidget * noteEditWidget = dynamic_cast < NoteEditWidget * > ( tab );
                if ( !noteEditWidget )
                {
                    WARNING( "Another type tab" );
                    dom_tab = dom_tab.nextSiblingElement();
                    continue;
                }

                const QString & state = dom_tab.attribute( "state_edit" );;
                if ( !state.isNull() )
                    noteEditWidget->restoreStateNoteEdit( state );
            }

            dom_tab = dom_tab.nextSiblingElement();
        }
        int index = rootTabs.attribute( "current_index", "-1" ).toInt();
        ui->tabWidget_EditNotes->setCurrentIndex( index );
    }

    // TODO: думаю, лучше сделать это настраиваемым
    ui->tab_Notes->expandAll();
    ui->tab_Tags->expandAll();
    ui->tab_Dates->expandAll();
}
void Page_Notes::write( QIODevice * device )
{
    QDomDocument xmlDomDocument;

    // Корень - тэг Notebook
    QDomElement root = xmlDomDocument.createElement( "Notebook" );
    xmlDomDocument.appendChild( root );

    // Создаем xml, в котором будет описано иерархическое дерево
    {
        Notebook::instance()->write( root, xmlDomDocument );
    }

    // Сохранение вкладок
    {
        QDomElement rootTabs = xmlDomDocument.createElement( "Tabs" );
        rootTabs.setAttribute( "current_index", ui->tabWidget_EditNotes->currentIndex() );
        root.appendChild( rootTabs );

        // Делаем перебор всех вкладок
        for ( int index = 0; index < ui->tabWidget_EditNotes->count(); index++ )
        {
            Note * note = ui->tabWidget_EditNotes->note( index );
            // С помощью указателя получаем id заметки, а id заметки на момент написания коммента было названием папки заметки
            const QString & id_name = Notebook::instance()->getIdFromNote( note );

            // Создаем элемент и указываем id
            QDomElement element = xmlDomDocument.createElement( "Note" );
            element.setAttribute( "id", id_name );

            // Запоминаем состояние редактора заметок
            {
                QWidget * tab =  ui->tabWidget_EditNotes->widget( index );
                NoteEditWidget * noteEditWidget = dynamic_cast < NoteEditWidget * > ( tab );
                if ( !noteEditWidget )
                {
                    WARNING( "Another type tab" );
                    continue;
                }

                const QString & state = noteEditWidget->saveStateNoteEdit();
                element.setAttribute( "state_edit", state );
            }

            // Добавляем в xml дерево, в узел Tabs
            rootTabs.appendChild( element );
        }
    }

    const int indentSize = 4;
    QTextStream out( device );
    out.setCodec( "UTF-8" );
    out << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    xmlDomDocument.save( out, indentSize );
}

void Page_Notes::readSettings()
{
    if ( !settings )
    {
        WARNING( "null pointer!" );
        return;
    }

    settings->beginGroup( "Page_Notes" );
    restoreState( settings->value( "State" ).toByteArray() );
    ui->splitter->restoreState( settings->value( "Splitter_Main" ).toByteArray() );
    settings->endGroup();

    ui->tabWidget_Navigation->setCurrentWidget( ui->tab_Notes );
}
void Page_Notes::writeSettings()
{
    if ( !settings )
    {
        WARNING( "null pointer!" );
        return;
    }

    settings->beginGroup( "Page_Notes" );
    settings->setValue( "State", saveState() );
    settings->setValue( "Splitter_Main", ui->splitter->saveState() );
    settings->endGroup();
    settings->sync();
}

bool Page_Notes::hasCurrent()
{
    return ui->tab_Notes->hasCurrentItem();
}
bool Page_Notes::currentIsNote()
{
    return ( ui->tab_Notes->getCurrentNote() != 0 );
}
bool Page_Notes::currentIsTrash()
{
    return ( ui->tab_Notes->getCurrentFolder() == Notebook::instance()->trashFolder() );
}
bool Page_Notes::currentNoteIsVisible()
{
    Note * note = ui->tab_Notes->getCurrentNote();
    if ( !note )
    {
        WARNING( "Null pointer! Current is don't note" );
        return false;
    }

    RichTextNote * richTextNote = note->getRichTextNote();
    if ( !richTextNote )
    {
        WARNING( "Null pointer!" );
        return false;
    }

    return richTextNote->isVisible();
}
bool Page_Notes::currentNoteIsTop()
{
    Note * note = ui->tab_Notes->getCurrentNote();
    if ( !note )
    {
        WARNING( "Null pointer! Current is don't note" );
        return false;
    }

    RichTextNote * richTextNote = note->getRichTextNote();
    if ( !richTextNote )
    {
        WARNING( "Null pointer!" );
        return false;
    }

    return richTextNote->isTop();
}
bool Page_Notes::currentIsChildTrash()
{
    BaseModelItem * item = ui->tab_Notes->getCurrentItem();
    if ( !item )
    {
        WARNING( "Null pointer!" );
        return false;
    }

    BaseModelItem * parentItem = item->parent();
    while ( parentItem )
    {
        if ( Notebook::instance()->hierarchyModel()->getItem( parentItem ) == Notebook::instance()->trashFolder() )
            return true;

        parentItem = parentItem->parent();
    }
    return false;
}
bool Page_Notes::trashIsEmpty()
{
    return ( Notebook::instance()->trashFolder()->Items.Count() == 0 );
}

void Page_Notes::sl_AddFolder()
{
    qApp->setOverrideCursor( Qt::WaitCursor );
    ui->tab_Notes->sl_AddFolderAction_Triggered();
    qApp->restoreOverrideCursor();
}
void Page_Notes::sl_AddNote()
{
    qApp->setOverrideCursor( Qt::WaitCursor );
    ui->tab_Notes->sl_AddNoteAction_Triggered();
    qApp->restoreOverrideCursor();
}
void Page_Notes::sl_AddNoteFromClipboard()
{
    qApp->setOverrideCursor( Qt::WaitCursor );

    RichTextNote * richTextNote = new RichTextNote();
    richTextNote->createNew( false );

    bool successful = ui->tab_Notes->sl_AddNote( richTextNote );
    if ( !successful )
    {
        CRITICAL( "Error when add new note" );
        richTextNote->remove();
        return;
    }

    richTextNote->setText( qApp->clipboard()->text() );
    richTextNote->save();

    qApp->restoreOverrideCursor();
}
void Page_Notes::sl_AddNoteFromScreen()
{
    FullscreenshotCropper cropper;
    cropper.setImage( QPixmap::grabWindow( QApplication::desktop()->winId() ) );
    cropper.showFullScreen();
    if ( !cropper.exec() )
        return;
    const QPixmap & screenshot = cropper.cropperImage();


    qApp->setOverrideCursor( Qt::WaitCursor );

    RichTextNote * richTextNote = new RichTextNote();
    richTextNote->createNew( false );

    bool successful = ui->tab_Notes->sl_AddNote( richTextNote );
    if ( !successful )
    {
        CRITICAL( "Error when add new note" );
        richTextNote->remove();
        return;
    }

    richTextNote->insertImage( screenshot );
    richTextNote->save();

    qApp->restoreOverrideCursor();
}
void Page_Notes::sl_Delete()
{
    qApp->setOverrideCursor( Qt::WaitCursor );
    ui->tab_Notes->sl_DeleteItemAction_Triggered();
    qApp->restoreOverrideCursor();
}
void Page_Notes::sl_ClearTrash()
{
    qApp->setOverrideCursor( Qt::WaitCursor );
    ui->tab_Notes->sl_ClearTrashAction_Triggered();
    qApp->restoreOverrideCursor();
}
void Page_Notes::sl_RemoveToTrash()
{
    qApp->setOverrideCursor( Qt::WaitCursor );
    ui->tab_Notes->sl_MoveToBinAction_Triggered();
    qApp->restoreOverrideCursor();
}

void Page_Notes::sl_SaveNote()
{
    Note * note = ui->tab_Notes->getCurrentNote();
    if ( !note )
    {
        WARNING( "null pointer on note!" )
        return;
    }

    RichTextNote * richTextNote = note->getRichTextNote();
    if ( !richTextNote )
    {
        WARNING( "null pointer on richTextNote!" )
        return;
    }

    richTextNote->save();
    emit sg_About_UpdateStates();
}
void Page_Notes::sl_SaveAsNote()
{
    Note * note = ui->tab_Notes->getCurrentNote();
    if ( !note )
    {
        WARNING( "null pointer on note!" )
        return;
    }

    RichTextNote * richTextNote = note->getRichTextNote();
    if ( !richTextNote )
    {
        WARNING( "null pointer on richTextNote!" )
        return;
    }

    richTextNote->saveAs();
}
void Page_Notes::sl_ShowNote()
{
    Note * note = ui->tab_Notes->getCurrentNote();
    if ( !note )
    {
        WARNING( "null pointer on note!" )
        return;
    }

    RichTextNote * richTextNote = note->getRichTextNote();
    if ( !richTextNote )
    {
        WARNING( "null pointer on richTextNote!" )
        return;
    }

    if ( richTextNote->isVisible() )
        return;
    richTextNote->show();

    emit sg_About_UpdateStates();
}
void Page_Notes::sl_HideNote()
{
    Note * note = ui->tab_Notes->getCurrentNote();
    if ( !note )
    {
        WARNING( "null pointer on note!" )
        return;
    }

    RichTextNote * richTextNote = note->getRichTextNote();
    if ( !richTextNote )
    {
        WARNING( "null pointer on richTextNote!" )
        return;
    }

    if ( richTextNote->isHidden() )
        return;
    richTextNote->hide();

    emit sg_About_UpdateStates();
}
void Page_Notes::sl_ShowAllNotes()
{
    qApp->setOverrideCursor( Qt::WaitCursor );

    foreach ( Note * note, Notebook::instance()->notesList() )
    {
        RichTextNote * richTextNote = note->getRichTextNote();
        if ( richTextNote->isVisible() )
            continue;

        richTextNote->show();
        qApp->processEvents();
    }

    qApp->restoreOverrideCursor();
    emit sg_About_UpdateStates();
}
void Page_Notes::sl_HideAllNotes()
{
    qApp->setOverrideCursor( Qt::WaitCursor );

    foreach ( Note * note, Notebook::instance()->notesList() )
    {
        RichTextNote * richTextNote = note->getRichTextNote();
        if ( richTextNote->isHidden() )
            continue;

        richTextNote->hide();
        qApp->processEvents();
    }

    qApp->restoreOverrideCursor();
    emit sg_About_UpdateStates();
}
void Page_Notes::sl_SaveAllNotes()
{
    qApp->setOverrideCursor( Qt::WaitCursor );

    foreach ( Note * note, Notebook::instance()->notesList() )
    {
        RichTextNote * richTextNote = note->getRichTextNote();
        richTextNote->save();
        qApp->processEvents();
    }

    qApp->restoreOverrideCursor();
    emit sg_About_UpdateStates();
}
void Page_Notes::sl_SetTopNote( bool top )
{
    Note * note = ui->tab_Notes->getCurrentNote();
    if ( !note )
    {
        WARNING( "null pointer on note!" )
        return;
    }

    RichTextNote * richTextNote = note->getRichTextNote();
    if ( !richTextNote )
    {
        WARNING( "null pointer on richTextNote!" )
        return;
    }

    richTextNote->setTop( top );

    emit sg_About_UpdateStates();
}
void Page_Notes::sl_PrintNote()
{
    Note * note = ui->tab_Notes->getCurrentNote();
    if ( !note )
    {
        WARNING( "null pointer on note!" )
        return;
    }

    RichTextNote * richTextNote = note->getRichTextNote();
    if ( !richTextNote )
    {
        WARNING( "null pointer on richTextNote!" )
        return;
    }

    richTextNote->print();
}
void Page_Notes::sl_PreviewPrintNote()
{
    Note * note = ui->tab_Notes->getCurrentNote();
    if ( !note )
    {
        WARNING( "null pointer on note!" )
        return;
    }

    RichTextNote * richTextNote = note->getRichTextNote();
    if ( !richTextNote )
    {
        WARNING( "null pointer on richTextNote!" )
        return;
    }

    richTextNote->previewPrint();
}
