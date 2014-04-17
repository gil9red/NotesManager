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

#include "ScriptModule/scriptengine.h"
#include <QMetaProperty>
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
    QObject::connect( Notebook::instance()->getTrashFolder(), SIGNAL(sg_ItemMoved(AbstractFolderItem*const,int,Folder*)), SIGNAL(sg_About_UpdateStates()) );
    QObject::connect( Notebook::instance()->getTrashFolder(), SIGNAL(sg_ItemAdded(AbstractFolderItem*const,int)), SIGNAL(sg_About_UpdateStates()) );
    QObject::connect( Notebook::instance()->getTrashFolder(), SIGNAL(sg_ItemRemoved(AbstractFolderItem*const)), SIGNAL(sg_About_UpdateStates()) );

    QObject::connect( this, SIGNAL(sg_About_UpdateStates()), SLOT(sl_UpdateStates()) );

    // Действия тулбаров
    {
        QObject::connect( ui->actionAddNote, SIGNAL( triggered() ), SLOT( addNote() ) );
        QObject::connect( ui->actionAddNoteFromClipboard, SIGNAL( triggered() ), SLOT( addNoteFromClipboard() ) );
        QObject::connect( ui->actionAddNoteFromScreen, SIGNAL( triggered() ), SLOT( addNoteFromScreen() ) );
        QObject::connect( ui->actionAddFolder, SIGNAL( triggered() ), SLOT( addFolder() ) );
        QObject::connect( ui->actionRemoveToTrash, SIGNAL( triggered() ), SLOT( sl_RemoveToTrash() ) );
        QObject::connect( ui->actionDelete, SIGNAL( triggered() ), SLOT( sl_Delete() ) );
        QObject::connect( ui->actionClearTrash, SIGNAL( triggered() ), SLOT( sl_ClearTrash() ) );

        QObject::connect( ui->actionSaveAllNotes, SIGNAL( triggered() ), SLOT( saveAllNotes() ) );
        QObject::connect( ui->actionShowAllNotes, SIGNAL( triggered() ), SLOT( showAllNotes() ) );
        QObject::connect( ui->actionHideAllNotes, SIGNAL( triggered() ), SLOT( hideAllNotes() ) );
        QObject::connect( ui->actionSaveNoteAs, SIGNAL( triggered() ), SLOT( sl_SaveAsNote() ) );
        QObject::connect( ui->actionSaveNote, SIGNAL( triggered() ), SLOT( sl_SaveNote() ) );
        QObject::connect( ui->actionShowNote, SIGNAL( triggered() ), SLOT( sl_ShowNote() ) );
        QObject::connect( ui->actionHideNote, SIGNAL( triggered() ), SLOT( sl_HideNote() ) );
        QObject::connect( ui->actionPrintNote, SIGNAL( triggered() ), SLOT( sl_PrintNote() ) );
        QObject::connect( ui->actionPreviewPrintNote, SIGNAL( triggered() ), SLOT( sl_PreviewPrintNote() ) );
        QObject::connect( ui->actionTopNote, SIGNAL( triggered(bool) ), SLOT( sl_SetTopNote(bool) ) );


        ui->tab_Notes->actionAddNote = ui->actionAddNote;
        ui->tab_Notes->actionAddFolder = ui->actionAddFolder;
        ui->tab_Notes->actionMoveToBin = ui->actionRemoveToTrash;
        ui->tab_Notes->actionDeleteItem = ui->actionDelete;
        ui->tab_Notes->actionClearTrash = ui->actionClearTrash;
    }

    // Сценарии
    {
        Script::ScriptEngine * engine = Script::ScriptEngine::instance();
        QScriptValue scriptPageNotes = engine->newQObject( this );

        engine->globalObject().setProperty( "PageNotes", scriptPageNotes );
    }

    sl_UpdateStates();
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

        ui->tab_Notes->setModel( Notebook::instance()->getHierarchyModel() );
        ui->tab_Tags->setModel( Notebook::instance()->getTagsModel() );
        ui->tab_Dates->setCreationModel( Notebook::instance()->getCreationDateModel() );
        ui->tab_Dates->setModificationModel( Notebook::instance()->getModificationDateModel() );
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

    ui->tab_Notes->sl_ExpandAll();
    ui->tab_Tags->sl_ExpandAll();
    ui->tab_Dates->sl_ExpandAll();
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
void Page_Notes::writeToXmlStateNotes()
{
    // Передача xml файла, в котором будет описано иерархическое дерево с заметками
    const QString & fileName = qApp->applicationDirPath() + "/notes/notebook.xml";
    QFile file( fileName );
    if ( !file.open( QFile::WriteOnly | QFile::Text ) )
    {
        QMessageBox::warning( this, tr( "Error" ), tr( "Cannot write file %1:\n%2." ).arg( fileName ).arg( file.errorString() ) );
        return;
    }
    write( &file );
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
    return ( ui->tab_Notes->getCurrentFolder() == Notebook::instance()->getTrashFolder() );
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
        if ( Notebook::instance()->getHierarchyModel()->getItem( parentItem ) == Notebook::instance()->getTrashFolder() )
            return true;

        parentItem = parentItem->parent();
    }
    return false;
}
bool Page_Notes::trashIsEmpty()
{
    return ( Notebook::instance()->getTrashFolder()->child.Count() == 0 );
}

void Page_Notes::addFolder()
{
    qApp->setOverrideCursor( Qt::WaitCursor );
    ui->tab_Notes->sl_AddFolderAction_Triggered();
    writeToXmlStateNotes();
    qApp->restoreOverrideCursor();
}
void Page_Notes::addNote()
{
    qApp->setOverrideCursor( Qt::WaitCursor );
    ui->tab_Notes->sl_AddNoteAction_Triggered();
    writeToXmlStateNotes();
    qApp->restoreOverrideCursor();
}
void Page_Notes::addNoteFromClipboard()
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
    writeToXmlStateNotes();
    qApp->restoreOverrideCursor();
}
void Page_Notes::addNoteFromScreen()
{
    FullscreenshotCropper cropper;
    cropper.setImage( QPixmap::grabWindow( qApp->desktop()->winId() ) );
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
        qApp->restoreOverrideCursor();
        return;
    }

    richTextNote->insertImage( screenshot );
    richTextNote->save();
    writeToXmlStateNotes();
    qApp->restoreOverrideCursor();
}
void Page_Notes::sl_Delete()
{
    ui->tab_Notes->sl_DeleteItemAction_Triggered();
    writeToXmlStateNotes();
}
void Page_Notes::sl_ClearTrash()
{
    ui->tab_Notes->sl_ClearTrashAction_Triggered();
    writeToXmlStateNotes();
}
void Page_Notes::sl_RemoveToTrash()
{
    ui->tab_Notes->sl_MoveToBinAction_Triggered();
    writeToXmlStateNotes();
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
void Page_Notes::setVisibleAllNotes( bool visible )
{
    qApp->setOverrideCursor( Qt::WaitCursor );

    foreach ( Note * note, Notebook::instance()->getNotesList() )
    {
        RichTextNote * richTextNote = note->getRichTextNote();
        if ( visible )
        {
            if ( richTextNote->isVisible() )
                continue;
        } else
        {
            if ( richTextNote->isHidden() )
                continue;
        }

        richTextNote->setVisible( visible );
        qApp->processEvents();
    }

    qApp->restoreOverrideCursor();
    emit sg_About_UpdateStates();
}
void Page_Notes::showAllNotes()
{
    setVisibleAllNotes( true );
}
void Page_Notes::hideAllNotes()
{
    setVisibleAllNotes( false );
}
void Page_Notes::saveAllNotes()
{
    qApp->setOverrideCursor( Qt::WaitCursor );

    foreach ( Note * note, Notebook::instance()->getNotesList() )
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

void Page_Notes::sl_UpdateStates()
{
    bool isEmpty = Notebook::instance()->getNotesList().isEmpty();
    ui->actionHideAllNotes->setEnabled( !isEmpty );
    ui->actionShowAllNotes->setEnabled( !isEmpty );
    ui->actionSaveAllNotes->setEnabled( !isEmpty );

    ui->actionSaveNote->setEnabled( false );
    ui->actionSaveNoteAs->setEnabled( false );
    ui->actionPrintNote->setEnabled( false );
    ui->actionPreviewPrintNote->setEnabled( false );
    ui->actionHideNote->setEnabled( false );
    ui->actionShowNote->setEnabled( false );
    ui->actionTopNote->setChecked( false );

    ui->actionDelete->setEnabled( false );
    ui->actionRemoveToTrash->setEnabled( false );
    ui->actionClearTrash->setEnabled( Notebook::instance()->getTrashFolder()->child.Count() > 0 );

    bool hasCurrent = this->hasCurrent();
    if ( hasCurrent )
    {
        bool isNote = currentIsNote();
        if ( isNote )
        {
            ui->actionSaveNote->setEnabled( true );
            ui->actionSaveNoteAs->setEnabled( true );
            ui->actionPrintNote->setEnabled( true );
            ui->actionPreviewPrintNote->setEnabled( true );

            bool currentNoteIsVisible = this->currentNoteIsVisible();
            ui->actionHideNote->setEnabled( currentNoteIsVisible );
            ui->actionShowNote->setEnabled( !currentNoteIsVisible );

            bool currentNoteIsTop = this->currentNoteIsTop();
            ui->actionTopNote->setChecked( currentNoteIsTop );
        }

        bool isChildTrash = this->currentIsChildTrash(); // если элемент есть в корзине
        bool isTrash = this->currentIsTrash();
        ui->actionRemoveToTrash->setEnabled( !isTrash && !isChildTrash ); // переместить в корзину
        ui->actionDelete->setEnabled( isChildTrash );
    }
}
