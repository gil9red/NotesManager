#include "page_notes.h"

#include <QMenu>
#include <QColorDialog>
#include <QDomDocument>
#include "appinfo.h"
#include "utils/func.h"
#include "noteeditor.h"
#include "NavigationPanel/trashmodelitem.h"
#include "NavigationPanel/foldermodelitem.h"
#include "RegisterNote.h"
#include "fullscreenshotcropper.h"
#include <QClipboard>
#include <QDesktopWidget>

BaseModelItem * createFromType( BaseModelItem::Type type )
{
    switch ( type )
    {
    case BaseModelItem::BASE:
        return new BaseModelItem();

    case BaseModelItem::NOTE:
        return new NoteModelItem();

    case BaseModelItem::FOLDER:
        return new FolderModelItem();

    case BaseModelItem::TRASH:
        return new TrashModelItem();
    }

    return 0;
}
BaseModelItem * createFromType( const QString & type )
{
    if ( type == "Base" )
        return new BaseModelItem();

    else if ( type == "Note" )
        return new NoteModelItem();

    else if ( type == "Folder" )
        return new FolderModelItem();

    else if ( type == "Trash" )
        return new TrashModelItem();

    return 0;
}

Page_Notes::Page_Notes( QWidget * parent ) :
    QMainWindow( parent ),
    ui( new Ui::Page_notes ),
    settings(0),
    itemTrash(0)
{
    ui->setupUi( this );

    model.setHorizontalHeaderLabels( QStringList() << App::name );
    ui->treeNotes->setModel( &model );
    ui->treeNotes->setContextMenuPolicy( Qt::CustomContextMenu );
    connect( ui->treeNotes, SIGNAL( customContextMenuRequested( QPoint ) ), SLOT( showContextMenu(QPoint) ) );
    connect( ui->treeNotes, SIGNAL( doubleClicked( QModelIndex ) ), SLOT( open() ) );
    connect( ui->treeNotes->selectionModel(), SIGNAL( selectionChanged(QItemSelection,QItemSelection) ), SIGNAL( about_updateStates() ) );

    connect( ui->tabNotes, SIGNAL( aboutCurrentModelItem(QStandardItem*) ), SLOT( setCurrentItem(QStandardItem*) ) );

    ui->treeNotes->viewport()->installEventFilter( this );

    connect( &model, SIGNAL( itemChanged( QStandardItem * ) ), SLOT( noteChanged(QStandardItem*) ) );
}
Page_Notes::~Page_Notes()
{
    delete ui;
}

void Page_Notes::setSettings( QSettings * s )
{
    settings = s;
}

bool Page_Notes::isEmpty()
{
    // Если в модели есть только 1 элемент и это Корзина
    return ( ( model.rowCount() == 1 ) && ( model.item(0) == itemTrash ) );
}
bool Page_Notes::trashIsEmpty()
{
    if ( !itemTrash )
    {
        WARNING( "null pointer!" );
        return false;
    }
    return ( itemTrash->rowCount() == 0 );
}
bool Page_Notes::currentIsChildTrash()
{
    const QModelIndex & index = ui->treeNotes->currentIndex();
    QStandardItem * currentItem = model.itemFromIndex( index );
    QStandardItem * parentItem = currentItem->parent();
    while ( parentItem )
    {
        if ( parentItem == itemTrash )
            return true;
        parentItem = parentItem->parent();
    }

    return false;
}
bool Page_Notes::hasCurrent()
{
    return ui->treeNotes->currentIndex().isValid();
}
BaseModelItem * Page_Notes::baseItemFromIndex( const QModelIndex & index )
{
    if ( !index.isValid() )
    {
        WARNING( "index is not valid!" );
        return 0;
    }

    QStandardItem * item = model.itemFromIndex( index );
    if ( !item )
    {
        WARNING( "null pointer!" );
        return 0;
    }

    BaseModelItem * baseItem = static_cast < BaseModelItem * > ( item );
    if ( !baseItem )
    {
        WARNING( "null pointer!" );
        return 0;
    }

    return baseItem;
}
bool Page_Notes::currentIsNote()
{
    const QModelIndex & index = ui->treeNotes->currentIndex();
    return baseItemFromIndex( index )->isNote();
}
bool Page_Notes::currentIsFolder()
{
    const QModelIndex & index = ui->treeNotes->currentIndex();
    return baseItemFromIndex( index )->isFolder();
}
bool Page_Notes::currentIsTrash()
{
    const QModelIndex & index = ui->treeNotes->currentIndex();
    return baseItemFromIndex( index )->isTrash();
}
NoteModelItem * Page_Notes::noteItemFromIndex( const QModelIndex & index )
{
    BaseModelItem * baseItem = baseItemFromIndex( index );
    if ( !baseItem )
    {
        WARNING( "null pointer!" );
        return 0;
    }

    NoteModelItem * noteItem = static_cast < NoteModelItem * > ( baseItem );
    if ( !noteItem )
    {
        WARNING( "null pointer!" );
        return 0;
    }

    return noteItem;
}
RichTextNote * Page_Notes::noteFromIndex( const QModelIndex & index )
{
    if ( !index.isValid() )
    {
        WARNING( "index is not valid!" );
        return 0;
    }

    NoteModelItem * noteItem = noteItemFromIndex( index );
    if ( !noteItem )
    {
        WARNING( "null pointer!" );
        return 0;
    }

    RichTextNote * note = noteItem->note();
    if ( !note )
    {
        WARNING( "null pointer!" );
        return 0;
    }

    return note;
}
bool Page_Notes::currentNoteIsVisible()
{
    const QModelIndex & index = ui->treeNotes->currentIndex();
    return noteFromIndex( index )->isVisible();
}
bool Page_Notes::currentNoteIsTop()
{
    const QModelIndex & index = ui->treeNotes->currentIndex();
    return noteFromIndex( index )->isTop();
}

BaseModelItem * Page_Notes::createItemOfDomElement( const QDomElement & element )
{
    BaseModelItem * item = createFromType( element.tagName() );
    if ( !item )
    {
        WARNING( "null pointer!" );
        return 0;
    }
    if ( !item->isTrash() )
    {
        if ( item->isFolder() )
            item->setText( element.attribute( "name" ) );
        item->setForeground( QColor( element.attribute( "text_color", "#000000" ) ) );
        item->setBackground( QColor( element.attribute( "back_color", "#FFFFFF" ) ) );

        if ( item->isNote() )
        {
            const QString & fileName = getNotesPath() + "/" + element.attribute( "id" );
            RichTextNote * note = new RichTextNote( fileName );
            note->load();

            NoteModelItem * noteItem = static_cast < NoteModelItem * > ( item );
            noteItem->setNote( note );

            item->setText( note->title() );

            hashItemNote.insert( noteItem, note );
            connect( note, SIGNAL( changed(int) ), SLOT( noteChanged(int) ) );
        }
    }

    return item;
}
void Page_Notes::parseDomElement( BaseModelItem * node, QDomElement & element )
{
    QDomElement child = element.firstChildElement();
    while( !child.isNull() )
    {
        BaseModelItem * child_item = createItemOfDomElement( child );
        node->appendRow( child_item );

        if ( child.hasChildNodes() )
            parseDomElement( child_item, child );

        child = child.nextSiblingElement();
    }
}
QDomElement Page_Notes::createDomElementOfItem( BaseModelItem * item, QDomDocument & xmlDomDocument )
{
    QDomElement element = xmlDomDocument.createElement( stringType( item ) );
    if ( !item->isTrash() )
    {
        if ( item->isFolder() )
            element.setAttribute( "name", item->text() );
        element.setAttribute( "text_color", item->foreground().color().name() );
        element.setAttribute( "back_color", item->background().color().name() );

        if ( item->isNote() )
        {
            NoteModelItem * noteItem = static_cast < NoteModelItem * > ( item );
            RichTextNote * note = noteItem->note();

            element.setAttribute( "id", RegisterNote::instance()->getIdName( note ) );

            note->save(); // TODO: remove this
        }
    }

    return element;
}
void Page_Notes::parseItem( BaseModelItem * node, QDomElement & element, QDomDocument & xmlDomDocument )
{
    for ( int i = 0; i < node->rowCount(); i++ )
    {
        BaseModelItem * itemChild = static_cast < BaseModelItem * > ( node->child(i) );
        QDomElement domChild = createDomElementOfItem( itemChild, xmlDomDocument );
        element.appendChild( domChild );
        parseItem( itemChild, domChild, xmlDomDocument );
    }
}
void Page_Notes::removeItem( BaseModelItem * item, QStandardItem * parent )
{
    if ( item->isFolder() )
    {
        while ( item->rowCount() )
        {
            BaseModelItem * child = static_cast < BaseModelItem * > ( item->child(0) );
            removeItem( child, item );
        }

    } else if ( item->isNote() )
    {
        NoteModelItem * noteItem = static_cast < NoteModelItem * > ( item );
        RichTextNote * note = noteItem->note();
        note->remove();
        hashItemNote.remove( noteItem );
    }

    parent->removeRow( item->row() );
}

void Page_Notes::noteChanged( int event )
{
    RichTextNote * note = static_cast < RichTextNote * > ( sender() );
    QStandardItem * item = hashItemNote.key( note );

    switch ( event )
    {
    case EventsNote::ChangeTitle:
        item->setText( note->title() );
        break;

    case EventsNote::Remove:
        removeToTrash( item );
        break;
    }

    emit about_updateStates();
}
void Page_Notes::noteChanged( QStandardItem * item )
{
    BaseModelItem * baseItem = static_cast < BaseModelItem * > ( item );
    // Работаем только с элементами типа "Заметка"
    if ( !baseItem->isNote() )
        return;

    RichTextNote * note = hashItemNote.value( item );
    if ( !note )
    {
        WARNING( "null pointer!" );
        return;
    }

    note->setTitle( item->text() );
    ui->tabNotes->onChangeNoteItems( item );
}

bool Page_Notes::read( QIODevice * device )
{
    QDomDocument xmlDomDocument;
    xmlDomDocument.setContent( device );

    // Корнем является тэг Notebook
    QDomElement root = xmlDomDocument.documentElement();

    // Ищем тэг Notes, в нем описано иерархическое дерево с заметками и папками
    QDomElement rootNotes = root.firstChildElement( "Notes" );

    // Переходим к первому ребенку этого тэга, это может быть Note, Trash, или Folder
    rootNotes = rootNotes.firstChildElement();
    while( !rootNotes.isNull() )
    {
        // Создаем элемент
        BaseModelItem * topLevelItem = createItemOfDomElement( rootNotes );
        if ( !topLevelItem )
        {
            WARNING( "null pointer!" );
            rootNotes = rootNotes.nextSiblingElement();
        }
        // Если это Trash, сохраняем указатель
        if ( topLevelItem->isTrash() )
        {
            // Если уже существует элемент Корзина, то пропускаем добавление его
            if ( itemTrash )
            {
                WARNING( "Duplicate item Trash!" );
                rootNotes = rootNotes.nextSiblingElement();
            }

            itemTrash = static_cast < TrashModelItem * > ( topLevelItem );
        }

        // Добавляем элемент в дерево
        model.appendRow( topLevelItem );

        // Если у элемента есть дети, отправляем его в рекурсивную функцию,
        // в которой будет идти разбор его детей, детей детей и т.д.
        if ( rootNotes.hasChildNodes() )
            parseDomElement( topLevelItem, rootNotes );

        // Переходим к следующему ребенку тэга Notes
        rootNotes = rootNotes.nextSiblingElement();
    }

    // Если среди детей не нашелся Trash, создаем и добавляем
    if ( !itemTrash )
    {
        itemTrash = new TrashModelItem();
        model.appendRow( itemTrash );
    }

    /*TODO: сделать это настраиваемым, т.е. можно указать, что после загрузки нужно развернуть все ветви*/
    ui->treeNotes->expandAll();


    // Ищем тэг Tabs, в нем описаны последнии перед закрытием программы открытые вкладки
    QDomElement rootTabs = root.firstChildElement( "Tabs" );
    int index = rootTabs.attribute( "CurrentIndex" ).toInt();

    // Переходим к первому ребенку этого тэга
    rootTabs = rootTabs.firstChildElement();
    while( !rootTabs.isNull() )
    {
        const QString & name_id = rootTabs.attribute( "id" );
        RichTextNote * note = RegisterNote::instance()->get( name_id );
        QStandardItem * item = hashItemNote.key( note );
        NoteModelItem * noteItem = static_cast < NoteModelItem * > ( item );
        ui->tabNotes->openTab( noteItem );
        rootTabs = rootTabs.nextSiblingElement();
    }

    ui->tabNotes->setCurrentIndex( index );

    return true;
}
bool Page_Notes::write( QIODevice * device )
{
    const int indentSize = 4;
    QTextStream out( device );
    out.setCodec( "UTF-8" );
    out << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";

    QDomDocument xmlDomDocument;
    // Корень - тэг Notebook
    QDomElement root = xmlDomDocument.createElement( "Notebook" );
    xmlDomDocument.appendChild( root );

    // У Notebook есть дети Notes и Tabs
    QDomElement rootNotes = xmlDomDocument.createElement( "Notes" );
    root.appendChild( rootNotes );

    for ( int i = 0; i < model.rowCount(); i++ )
    {
        BaseModelItem * topLevelItem = static_cast < BaseModelItem * > ( model.item(i) );

        QDomElement element = createDomElementOfItem( topLevelItem, xmlDomDocument );
        rootNotes.appendChild( element );

        parseItem( topLevelItem, element, xmlDomDocument );
    }


    QDomElement rootTabs = xmlDomDocument.createElement( "Tabs" );
    rootTabs.setAttribute( "CurrentIndex", ui->tabNotes->currentIndex() );

    root.appendChild( rootTabs );

    // Делаем перебор всех вкладок
    for ( int i = 0; i < ui->tabNotes->count(); i++ )
    {
        // Получаем указатель на заметку, которая "закреплена" за вкладкой
        RichTextNote * note = ui->tabNotes->note(i);

        // Через регистратор заметок, с помощью указателя на заметку, получаем id заметки,
        // а id заметки на момент написания коммента было название папки заметки
        const QString & id_name = RegisterNote::instance()->getIdName( note );

        // Создаем элемент и указываем id
        QDomElement element = xmlDomDocument.createElement( "Note" );
        element.setAttribute( "id", id_name );

        // Добавляем в xml дерево, в узел Tabs
        rootTabs.appendChild( element );
    }

    xmlDomDocument.save( out, indentSize );
    return true;
}

void Page_Notes::readSettings()
{
    if ( !settings )
    {
        WARNING( "null pointer!" );
        return;
    }

    settings->beginGroup( "Page_Notes" );
    ui->splitter->restoreState( settings->value( "Splitter_Main" ).toByteArray() );
    settings->endGroup();
}
void Page_Notes::writeSettings()
{
    if ( !settings )
    {
        WARNING( "null pointer!" );
        return;
    }

    settings->beginGroup( "Page_Notes" );
    settings->setValue( "Splitter_Main", ui->splitter->saveState() );
    settings->endGroup();
    settings->sync();
}

void Page_Notes::addItemToModel( BaseModelItem * baseItem )
{
    qApp->setOverrideCursor( Qt::WaitCursor );

    // Определим куда разместить элемент
    const QModelIndex & index = ui->treeNotes->currentIndex();
    BaseModelItem * currentItem = static_cast < BaseModelItem * > ( model.itemFromIndex( index ) );

    // Если нет текущего элемента, то добавляем в конец дерева
    if ( !index.isValid() )
        model.appendRow( baseItem );
    else
    {
        if ( !currentItem )
        {
            WARNING( "null pointer!" );
            return;
        }

        // Если текущий элемент папка, то добавляем в нее элемент
        if ( currentItem->isFolder() )
            currentItem->appendRow( baseItem );

        // Если текущий элемент корзина, то добавляем в конец дерева
        else if ( currentItem->isTrash() )
            model.appendRow( baseItem );

        // Если текущий элемент заметка, то добавляем в ниже ее
        else if ( currentItem->isNote() )
        {
            QStandardItem * parent = currentItem->parent();
            if ( parent )
                parent->insertRow( currentItem->row() + 1, baseItem );
            else
                model.insertRow( currentItem->row() + 1, baseItem );
        }
    }

    // Делаем "текущим" добавленный элемент
    ui->treeNotes->setCurrentIndex( baseItem->index() );
    qApp->restoreOverrideCursor();
}
void Page_Notes::addFolder()
{   
    FolderModelItem * folderItem = new FolderModelItem( tr( "New folder" ) );
    addItemToModel( folderItem );
}
void Page_Notes::addNote()
{
    qApp->setOverrideCursor( Qt::WaitCursor );

    RichTextNote * note = new RichTextNote();
    note->createNew();

    addNoteToModel( note );
    qApp->restoreOverrideCursor();
}
void Page_Notes::addNoteFromClipboard()
{
    qApp->setOverrideCursor( Qt::WaitCursor );

    RichTextNote * note = new RichTextNote();
    note->createNew( false );
    note->setText( qApp->clipboard()->text() );
    note->save();

    addNoteToModel( note );
    qApp->restoreOverrideCursor();
}
void Page_Notes::addNoteFromScreen()
{
    FullscreenshotCropper cropper;
    cropper.setImage( QPixmap::grabWindow( QApplication::desktop()->winId() ) );
    cropper.showFullScreen();
    if ( !cropper.exec() )
        return;

    const QPixmap & screenshot = cropper.cropperImage();

    qApp->setOverrideCursor( Qt::WaitCursor );

    RichTextNote * note = new RichTextNote();
    note->createNew( false );
    note->insertImage( screenshot );
    note->save();

    addNoteToModel( note );
    qApp->restoreOverrideCursor();
}
void Page_Notes::addNoteToModel( RichTextNote * note )
{
    NoteModelItem * noteItem = new NoteModelItem( note->title() );
    noteItem->setNote( note );

    QStandardItem * item = static_cast < QStandardItem * > ( noteItem );
    if ( !item )
    {
        WARNING( "null pointer!" );
        return;
    }

    hashItemNote.insert( item, note );
    connect( note, SIGNAL( changed(int) ), SLOT( noteChanged(int) ) );

    addItemToModel( noteItem );
}

void Page_Notes::saveAllNotes()
{
    qApp->setOverrideCursor( Qt::WaitCursor );

    QHash < QStandardItem *, RichTextNote * >::iterator it;
    for ( it = hashItemNote.begin(); it != hashItemNote.end(); it++ )
    {
        RichTextNote * note = it.value();
        note->save();
        qApp->processEvents();
    }

    qApp->restoreOverrideCursor();
    emit about_updateStates();
}
void Page_Notes::showAllNotes()
{
    qApp->setOverrideCursor( Qt::WaitCursor );

    QHash < QStandardItem *, RichTextNote * >::iterator it;
    for ( it = hashItemNote.begin(); it != hashItemNote.end(); it++ )
    {
        RichTextNote * note = it.value();
        if ( note->isVisible() )
            continue;

        note->show();
        qApp->processEvents();
    }

    qApp->restoreOverrideCursor();
    emit about_updateStates();
}
void Page_Notes::hideAllNotes()
{
    qApp->setOverrideCursor( Qt::WaitCursor );

    QHash < QStandardItem *, RichTextNote * >::iterator it;
    for ( it = hashItemNote.begin(); it != hashItemNote.end(); it++ )
    {
        RichTextNote * note = it.value();
        if ( note->isHidden() )
            continue;

        note->hide();
        qApp->processEvents();
    }

    qApp->restoreOverrideCursor();
    emit about_updateStates();
}

void Page_Notes::saveAsNote()
{
    const QModelIndex & index = ui->treeNotes->currentIndex();
    RichTextNote * note = noteFromIndex( index );
    note->saveAs();
}
void Page_Notes::saveNote()
{
    const QModelIndex & index = ui->treeNotes->currentIndex();
    RichTextNote * note = noteFromIndex( index );
    note->save();
    emit about_updateStates();
}
void Page_Notes::showNote()
{
    const QModelIndex & index = ui->treeNotes->currentIndex();
    RichTextNote * note = noteFromIndex( index );
    if ( note->isVisible() )
        return;
    note->show();
    emit about_updateStates();
}
void Page_Notes::hideNote()
{
    const QModelIndex & index = ui->treeNotes->currentIndex();
    RichTextNote * note = noteFromIndex( index );
    if ( note->isHidden() )
        return;
    note->hide();
    emit about_updateStates();
}
void Page_Notes::printNote()
{
    const QModelIndex & index = ui->treeNotes->currentIndex();
    RichTextNote * note = noteFromIndex( index );
    note->print();
}
void Page_Notes::previewPrintNote()
{
    const QModelIndex & index = ui->treeNotes->currentIndex();
    RichTextNote * note = noteFromIndex( index );
    note->previewPrint();
}
void Page_Notes::setTopNote( bool top )
{
    const QModelIndex & index = ui->treeNotes->currentIndex();
    RichTextNote * note = noteFromIndex( index );
    note->setTop( top );
    emit about_updateStates();
}

void Page_Notes::rename()
{
    ui->treeNotes->edit( ui->treeNotes->currentIndex() );
}
void Page_Notes::open()
{
    BaseModelItem * item = static_cast < BaseModelItem * > ( model.itemFromIndex( ui->treeNotes->currentIndex() ) );
    if ( !item )
    {
        WARNING( "null pointer!" );
        return;
    }

    if ( item->isNote() )
    {
        NoteModelItem * noteItem = static_cast < NoteModelItem * > ( item );
        ui->tabNotes->openTab( noteItem );
    }
}

void Page_Notes::removeToTrash( QStandardItem * item )
{
    if ( !item )
    {
        WARNING( "null pointer!" );
        return;
    }

    QStandardItem * parentItem = item->parent();
    // Если есть родитель - тогда это под элемент
    // иначе - элемент верхнего уровня
    if ( parentItem )
    {
        int row = item->row();
        item = parentItem->takeChild( row );
        parentItem->removeRow( row );
    } else
    {
        int row = item->row();
        item = model.takeItem( row );
        model.removeRow( row );
    }

    itemTrash->appendRow( item );
}
void Page_Notes::removeToTrash()
{
    QStandardItem * removedItem = model.itemFromIndex( ui->treeNotes->currentIndex() );
    removeToTrash( removedItem );

}
void Page_Notes::removeAllToTrash()
{
    int i = 0;
    while ( model.rowCount() != 1 )
    {
        QStandardItem * item = model.item(i);
        if ( item == itemTrash )
        {
            i++;
            continue;
        }

        int row = item->row();
        item = model.takeItem( row );
        model.removeRow( row );

        itemTrash->appendRow( item );
    }
}
void Page_Notes::removeFromTrash()
{
    BaseModelItem * item = static_cast < BaseModelItem * > ( model.itemFromIndex( ui->treeNotes->currentIndex() ) );
    BaseModelItem * parent = static_cast < BaseModelItem * > ( item->parent() );
    removeItem( item, parent );
}
void Page_Notes::clearTrash()
{
    while ( itemTrash->rowCount() )
    {
        BaseModelItem * child = static_cast < BaseModelItem * > ( itemTrash->child(0) );
        removeItem( child, itemTrash );
    }
}

void Page_Notes::textColor()
{
    const QModelIndex & index = ui->treeNotes->currentIndex();
    QStandardItem * currentItem = model.itemFromIndex( index );

    const QColor & color = QColorDialog::getColor( currentItem->foreground().color(), this, tr( "Select the text color" ) );
    if ( !color.isValid() )
        return;

    currentItem->setForeground( color );
}
void Page_Notes::defaultTextColor()
{
    const QModelIndex & index = ui->treeNotes->currentIndex();
    QStandardItem * currentItem = model.itemFromIndex( index );
    currentItem->setForeground( Qt::black );
}
void Page_Notes::backColor()
{
    const QModelIndex & index = ui->treeNotes->currentIndex();
    QStandardItem * currentItem = model.itemFromIndex( index );

    const QColor & color = QColorDialog::getColor( currentItem->background().color(), this, tr( "Select a background color" ) );
    if ( !color.isValid() )
        return;

    currentItem->setBackground( color );
}
void Page_Notes::defaultBackColor()
{
    const QModelIndex & index = ui->treeNotes->currentIndex();
    QStandardItem * currentItem = model.itemFromIndex( index );
    currentItem->setBackground( Qt::white );
}

void Page_Notes::showContextMenu( const QPoint & pos )
{    
    QMenu menu( this );
    menu.addAction( QIcon( ":/Manager/add" ), tr( "Add note" ), this, SLOT( addNote() ) );
    menu.addAction( QIcon( ":/Manager/add-from_clipboard" ), tr( "Add note from clipboard" ), this, SLOT( addNoteFromClipboard() ) );
    menu.addAction( QIcon( ":/Manager/screenshot" ), tr( "Add note from screen" ), this, SLOT( addNoteFromScreen() ) );
    menu.addAction( QIcon( ":/NavigationPanel/folder" ), tr( "Add folder" ), this, SLOT( addFolder() ) );
    menu.addSeparator();
    QAction * actionRename = menu.addAction( QIcon( "" ), tr( "Rename" ), this, SLOT( rename() ), QKeySequence( "" ) );
    QAction * actionOpen = menu.addAction( QIcon( "" ), tr( "Open" ), this, SLOT( open() ), QKeySequence( "" ) );
    menu.addSeparator();
    QMenu * menuTextColor = menu.addMenu( QIcon( "" ), tr( "Text color" ) );
    menuTextColor->addAction( QIcon( "" ), tr( "Default" ), this, SLOT( defaultTextColor() ), QKeySequence( "" ) );
    menuTextColor->addAction( QIcon( "" ), tr( "Select color" ), this, SLOT( textColor() ), QKeySequence( "" ) );
    QMenu * menuBackColor = menu.addMenu( QIcon( "" ), tr( "Background color" ) );
    menuBackColor->addAction( QIcon( "" ), tr( "Default" ), this, SLOT( defaultBackColor() ), QKeySequence( "" ) );
    menuBackColor->addAction( QIcon( "" ), tr( "Select color" ), this, SLOT( backColor() ), QKeySequence( "" ) );
    menu.addSeparator();
    QAction * actionDelete = menu.addAction( QIcon( ":/Manager/remove" ), tr( "Delete" ), this, SLOT( removeFromTrash() ), QKeySequence( "" ) );
    QAction * actionClearTrash = menu.addAction( QIcon( "" ), tr( "Clear trash" ), this, SLOT( clearTrash() ), QKeySequence( "" ) );
    QAction * actionRemoveToTrash = menu.addAction( QIcon( ":/NavigationPanel/trash" ), tr( "Remove to trash" ), this, SLOT( removeToTrash() ), QKeySequence( "" ) );
    QAction * actionRemoveAllToTrash = menu.addAction( QIcon( "" ), tr( "Remove all to trash" ), this, SLOT( removeAllToTrash() ), QKeySequence( "" ) );

    actionRename->setEnabled( false );
    actionOpen->setEnabled( false );
    menuTextColor->setEnabled( false );
    menuBackColor->setEnabled( false );

    actionRemoveToTrash->setEnabled( false );
    actionRemoveAllToTrash->setEnabled( !isEmpty() ); // все в корзину
    actionDelete->setEnabled( false );
    actionClearTrash->setEnabled( !trashIsEmpty() );

    bool isCurrent = hasCurrent();
    if ( isCurrent )
    {
        bool isNoteOrFolder = currentIsNote() || currentIsFolder();
        actionRename->setEnabled( isNoteOrFolder );
        actionOpen->setEnabled( isNoteOrFolder );
        menuTextColor->setEnabled( isNoteOrFolder );
        menuBackColor->setEnabled( isNoteOrFolder );

        bool isChildTrash = currentIsChildTrash(); // если элемент есть в корзине
        actionRemoveToTrash->setEnabled( !currentIsTrash() && !isChildTrash ); // переместить в корзину
        actionDelete->setEnabled( isChildTrash );
        actionClearTrash->setEnabled( currentIsTrash() || isChildTrash ); // если корзина не пустая
    }


    const QPoint & globalPos = ui->treeNotes->viewport()->mapToGlobal( pos );
    menu.exec( globalPos );
}
void Page_Notes::setCurrentItem( QStandardItem * item )
{
    ui->treeNotes->setCurrentIndex( item->index() );
}

bool Page_Notes::eventFilter( QObject * object, QEvent * event )
{
    // Если кликнули на пустую область иерархического дерева, тогда убираем выделение
    if ( object == ui->treeNotes->viewport() )
    {
        if ( event->type() == QEvent::MouseButtonRelease )
        {
            QMouseEvent * mouseEvent = static_cast < QMouseEvent * > ( event );
            const QModelIndex & index = ui->treeNotes->indexAt( mouseEvent->pos() );
            if ( !index.isValid() )
                ui->treeNotes->setCurrentIndex( QModelIndex() );
        }

        return false;
    }

    return QMainWindow::eventFilter( object, event );
}
