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

BaseModelItem * createFromType( BaseModelItem::Type type )
{
    BaseModelItem * item = 0;
    switch ( type )
    {
    case BaseModelItem::BASE:
        item = new BaseModelItem();
        break;
    case BaseModelItem::NOTE:
        item = new NoteModelItem();
        break;
    case BaseModelItem::FOLDER:
        item = new FolderModelItem();
        break;
    case BaseModelItem::TRASH:
        item = new TrashModelItem();
        break;
    default:
        return item;
    }
    return item;
}
BaseModelItem * createFromType( const QString & type )
{
    BaseModelItem * item = 0;

    if ( type == "Base" )
        item = new BaseModelItem();

    else if ( type == "Note" )
        item = new NoteModelItem();

    else if ( type == "Folder" )
        item = new FolderModelItem();

    else if ( type == "Trash" )
        item = new TrashModelItem();

    return item;
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
void Page_Notes::removeItem( BaseModelItem * item, BaseModelItem * parent )
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
    }
}
void Page_Notes::noteChanged( QStandardItem * item )
{
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

    // TODO: вкладки тоже сохранять (брать путь к заметки в качестве id)
}

void Page_Notes::addTopLevelNote()
{
    RichTextNote * note = new RichTextNote();
    note->createNew();

    NoteModelItem * noteItem = new NoteModelItem( note->title() );
    noteItem->setNote( note );

    model.appendRow( noteItem );

    hashItemNote.insert( noteItem, note );
    connect( note, SIGNAL( changed(int) ), SLOT( noteChanged(int) ) );
}
void Page_Notes::addTopLevelFolder()
{
    model.appendRow( new FolderModelItem( tr( "Новая папка" ) ) );
}
void Page_Notes::addFolder()
{
    QStandardItem * currentItem = model.itemFromIndex( ui->treeNotes->currentIndex() );
    currentItem->appendRow( new FolderModelItem( tr( "Новая папка" ) ) );
}
void Page_Notes::addNote()
{
    RichTextNote * note = new RichTextNote();
    note->createNew();

    NoteModelItem * noteItem = new NoteModelItem( note->title() );
    noteItem->setNote( note );

    hashItemNote.insert( noteItem, note );
    connect( note, SIGNAL( changed(int) ), SLOT( noteChanged(int) ) );

    // Определим куда разместить только что созданную заметку
    const QModelIndex & index = ui->treeNotes->currentIndex();

    if ( !index.isValid() )
        model.appendRow( noteItem );
    else
    {
        BaseModelItem * currentItem = static_cast < BaseModelItem * > ( model.itemFromIndex( ui->treeNotes->currentIndex() ) );
        if ( !currentItem )
        {
            WARNING( "null pointer!" );
            return;
        }

        if ( currentItem->isFolder() )
            currentItem->appendRow( noteItem );

        else if ( currentItem->isTrash() )
            model.appendRow( noteItem );

        else if ( currentItem->isNote() )
        {
            QStandardItem * parent = currentItem->parent();
            if ( parent )
                parent->insertRow( currentItem->row() + 1, noteItem );
            else
                model.insertRow( currentItem->row() + 1, noteItem );
        }
    }

    ui->treeNotes->setCurrentIndex( noteItem->index() );
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
void Page_Notes::removeToTrash()
{
    QStandardItem * removedItem = model.itemFromIndex( ui->treeNotes->currentIndex() );
    if ( !removedItem )
    {
        WARNING( "null pointer!" );
        return;
    }

    QStandardItem * parentItem = removedItem->parent();
    // Если есть родитель - тогда это под элемент
    // иначе - элемент верхнего уровня
    if ( parentItem )
    {
        int row = removedItem->row();
        removedItem = parentItem->takeChild( row );
        parentItem->removeRow( row );
    } else
    {
        int row = removedItem->row();
        removedItem = model.takeItem( row );
        model.removeRow( row );
    }

    itemTrash->appendRow( removedItem );
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

    const QColor & color = QColorDialog::getColor( currentItem->foreground().color(), this );
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

    const QColor & color = QColorDialog::getColor( currentItem->background().color(), this );
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
    const QModelIndex & index = ui->treeNotes->currentIndex();
    if ( !index.isValid() )
        return;

    BaseModelItem * currentItem = static_cast < BaseModelItem * > ( model.itemFromIndex( index ) );

    bool isFolder = currentItem->isFolder();
    bool isNote = currentItem->isNote();
    bool isTrash = currentItem->isTrash();
    bool isEmptyTrash = itemTrash->isEmpty();
    bool isChildTrash = false;

    QStandardItem * parentItem = currentItem->parent();
    while ( parentItem )
    {
        if ( parentItem == itemTrash )
        {
            isChildTrash = true;
            break;
        }
        parentItem = parentItem->parent();
    }


    QMenu menu( this );
    menu.addAction( QIcon( "" ), tr( "Добавить заметку верхнего уровня" ), this, SLOT( addTopLevelNote() ), QKeySequence( "" ) );
    menu.addAction( QIcon( "" ), tr( "Добавить папку верхнего уровня" ), this, SLOT( addTopLevelFolder() ), QKeySequence( "" ) );
    menu.addSeparator();

    if ( isFolder )
    {
        menu.addAction( QIcon( "" ), tr( "Добавить папку" ), this, SLOT( addFolder() ), QKeySequence( "" ) );
        menu.addAction( QIcon( "" ), tr( "Добавить заметку" ), this, SLOT( addNote() ), QKeySequence( "" ) );
    }

    if ( isNote || isFolder )
        menu.addAction( QIcon( "" ), tr( "Переименовать" ), this, SLOT( rename() ), QKeySequence( "" ) );

    if ( isNote )
        menu.addAction( QIcon( "" ), tr( "Открыть" ), this, SLOT( open() ), QKeySequence( "" ) );

    if ( (isNote || isFolder) && !isChildTrash )
        menu.addAction( QIcon( "" ), tr( "Переместить в корзину" ), this, SLOT( removeToTrash() ), QKeySequence( "" ) );

    if ( isChildTrash )
        menu.addAction( QIcon( "" ), tr( "Удалить" ), this, SLOT( removeFromTrash() ), QKeySequence( "" ) );

    if ( isChildTrash || ( isTrash && !isEmptyTrash ) )
        menu.addAction( QIcon( "" ), tr( "Очистить корзину" ), this, SLOT( clearTrash() ), QKeySequence( "" ) );

    bool isEmpty = ( model.rowCount() - 1 == 0 );
    if ( !isEmpty )
        menu.addAction( QIcon( "" ), tr( "Переместить все в корзину" ), this, SLOT( removeAllToTrash() ), QKeySequence( "" ) );

    menu.addSeparator();

    if ( isNote || isFolder )
    {
        QMenu * menuTextColor = menu.addMenu( QIcon( "" ), tr( "Изменить цвет текста" ) );
        menuTextColor->addAction( QIcon( "" ), tr( "Цвет по умолчанию" ), this, SLOT( defaultTextColor() ), QKeySequence( "" ) );
        menuTextColor->addAction( QIcon( "" ), tr( "Выбрать цвет" ), this, SLOT( textColor() ), QKeySequence( "" ) );

        QMenu * menuBackColor = menu.addMenu( QIcon( "" ), tr( "Изменить цвет фона" ) );
        menuBackColor->addAction( QIcon( "" ), tr( "Цвет по умолчанию" ), this, SLOT( defaultBackColor() ), QKeySequence( "" ) );
        menuBackColor->addAction( QIcon( "" ), tr( "Выбрать цвет" ), this, SLOT( backColor() ), QKeySequence( "" ) );
    }    

    menu.exec( ui->treeNotes->viewport()->mapToGlobal( pos ) );
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
