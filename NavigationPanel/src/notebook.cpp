#include "notebook.h"
#include "folder.h"
#include "note.h"
#include "hierarchymodel.h"
#include "Note/RichTextNote.h"
#include "tag.h"
#include <QDomDocument>

QString getIdFromRichTextNote( RichTextNote * note )
{
    return QDir( note->fileName() ).dirName();
}

AbstractFolderItem * createFromType( const QString & type )
{
    if ( type == "Note" )
        return new Note();

    else if ( type == "Folder" )
        return new Folder();

    else if ( type == "Trash" )
        return Notebook::instance()->trashFolder();

    else if ( type == "Temp" )
        return Notebook::instance()->tempFolder();

    return 0;
}

AbstractFolderItem * createItemOfDomElement( const QDomElement & element )
{
    AbstractFolderItem * item = createFromType( element.tagName() );
    if ( !item )
    {
        WARNING( "null pointer!" );
        return 0;
    }

    bool isFolder = item->GetItemType() == AbstractFolderItem::Type_Folder;
    bool isNote = item->GetItemType() == AbstractFolderItem::Type_Note;

    if ( isFolder )
    {
        Folder * folder = static_cast < Folder * > ( item );
        folder->SetName( element.attribute( "name" ) );
    }

    item->SetNameForeColor( QColor( element.attribute( "fore_color", "#000000" ) ) );
    item->SetNameBackColor( QColor( element.attribute( "back_color", "#FFFFFF" ) ) );

    if ( isNote )
    {
        const QString & fileName = getNotesPath() + "/" + element.attribute( "id" );
        RichTextNote * richTextNote = new RichTextNote( fileName );
        richTextNote->load();

        Note * note = static_cast < Note * > ( item );
        note->setRichTextNote( richTextNote );
    }

    return item;
}
void parseDomElement( Folder * folder, QDomElement & element )
{
    QDomElement child = element.firstChildElement();
    while( !child.isNull() )
    {
        // Создаем элемент
        AbstractFolderItem * item = createItemOfDomElement( child );
        if ( item->GetItemType() == AbstractFolderItem::Type_Note )
            folder->Items.Add( item );

        else if ( item->GetItemType() == AbstractFolderItem::Type_Folder )
        {
            folder->Items.Add( item );
            parseDomElement( static_cast < Folder * > ( item ), child );
        }

        child = child.nextSiblingElement();
    }
}

QDomElement createDomElementOfItem( AbstractFolderItem * item, QDomDocument & xmlDomDocument )
{
    bool isFolder = item->GetItemType() == AbstractFolderItem::Type_Folder;
    bool isNote = item->GetItemType() == AbstractFolderItem::Type_Note;

    Folder * folder = static_cast < Folder * > ( item );

    QString tagName;
    if ( isNote )
        tagName = "Note";

    else if ( isFolder )
    {
        if ( folder->GetType() == Folder::UserFolder )
            tagName = "Folder";

    } else
    {
        WARNING( "Unknown type" );
        return QDomElement();
    }

    QDomElement element = xmlDomDocument.createElement( tagName );
    element.setAttribute( "fore_color", item->GetNameForeColor().name() );
    element.setAttribute( "back_color", item->GetNameBackColor().name() );

    if ( isFolder )
    {
        element.setAttribute( "name", folder->GetName() );

    } else if ( isNote )
    {
        Note * note = static_cast < Note * > ( item );
        RichTextNote * richTextNote = note->getRichTextNote();
        richTextNote->save(); // TODO: remove this

        const QString & id = getIdFromRichTextNote( richTextNote );
        element.setAttribute( "id", id );
    }

    return element;
}
void parseItem( Folder * folder, QDomElement & element, QDomDocument & xmlDomDocument )
{
    for ( int i = 0; i < folder->Items.Count(); i++ )
    {
        AbstractFolderItem * child = folder->Items.ItemAt(i);
        QDomElement domChild = createDomElementOfItem( child, xmlDomDocument );
        element.appendChild( domChild );
        if ( child->GetItemType() == AbstractFolderItem::Type_Folder )
            parseItem( static_cast < Folder * > ( child ), domChild, xmlDomDocument );
    }
}


#include <QDebug>

Notebook * Notebook::self = 0;

Notebook::Notebook( QObject * parent )
    : QObject( parent )
{
    self = this;

    p_rootFolder = new Folder( "_root_", Folder::SystemFolder );
    QObject::connect( p_rootFolder, SIGNAL(sg_ItemAdded(AbstractFolderItem * const, int)), SLOT(sl_Folder_ItemAdded(AbstractFolderItem* const)) );
    QObject::connect( p_rootFolder, SIGNAL(sg_ItemRemoved(AbstractFolderItem*const)), SLOT(sl_Folder_ItemRemoved(AbstractFolderItem*const)) );

    p_tempFolder = new Folder( QString(), Folder::TempFolder );
    QObject::connect( p_tempFolder, SIGNAL(sg_ItemAdded(AbstractFolderItem*const, int)), SLOT(sl_Folder_ItemAdded(AbstractFolderItem* const)) );
    QObject::connect( p_tempFolder, SIGNAL(sg_ItemRemoved(AbstractFolderItem*const)), SLOT(sl_Folder_ItemRemoved(AbstractFolderItem*const)) );

    p_trashFolder = new Folder( QString(), Folder::TrashFolder );
    QObject::connect( p_trashFolder, SIGNAL(sg_ItemAdded(AbstractFolderItem*const, int)), SLOT(sl_Folder_ItemAdded(AbstractFolderItem* const)) );
    QObject::connect( p_trashFolder, SIGNAL(sg_ItemRemoved(AbstractFolderItem*const)), SLOT(sl_Folder_ItemRemoved(AbstractFolderItem*const)) );

    p_hierarchyModel = new HierarchyModel( this );
    p_tagsModel = new TagsModel( this );
    p_creationDateModel = new DatesModel( DatesModel::CreationDate, this );
    p_modificationDateModel = new DatesModel( DatesModel::ModifyDate, this );
}
Notebook::~Notebook()
{
    self = 0;
}

Notebook * Notebook::instance()
{    
    if ( !self )
        self = new Notebook();

    return self;
}

void Notebook::read( QIODevice * device )
{
    QDomDocument xmlDomDocument;
    xmlDomDocument.setContent( device );

    // Корнем является тэг Notebook
    QDomElement root = xmlDomDocument.documentElement();
    QDomElement rootNotes = root.firstChildElement( "Notes" );
    QDomElement rootTemp = root.firstChildElement( "Temp" );
    QDomElement rootTrash = root.firstChildElement( "Trash" );

    parseDomElement( rootFolder(), rootNotes );
    parseDomElement( tempFolder(), rootTemp );
    parseDomElement( trashFolder(), rootTrash );
}
void Notebook::write( QIODevice * device )
{
        QDomDocument xmlDomDocument;
        // Корень - тэг Notebook
        QDomElement root = xmlDomDocument.createElement( "Notebook" );
        xmlDomDocument.appendChild( root );

        // У Notebook есть дети Notes и Tabs
        QDomElement rootNotes = xmlDomDocument.createElement( "Notes" );
        QDomElement rootTemp = xmlDomDocument.createElement( "Temp" );
        QDomElement rootTrash = xmlDomDocument.createElement( "Trash" );

        root.appendChild( rootNotes );
        root.appendChild( rootTemp );
        root.appendChild( rootTrash );

        parseItem( rootFolder(), rootNotes, xmlDomDocument );
        parseItem( tempFolder(), rootTemp, xmlDomDocument );
        parseItem( trashFolder(), rootTrash, xmlDomDocument );


        const int indentSize = 4;
        QTextStream out( device );
        out.setCodec( "UTF-8" );
        out << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
        xmlDomDocument.save( out, indentSize );
}

void Notebook::setRootFolder( Folder * f ) { p_rootFolder = f; }
void Notebook::setTempFolder( Folder * f ) { p_tempFolder = f; }
void Notebook::setTrashFolder( Folder * f ) { p_trashFolder = f; }
void Notebook::setPinnedFolder( Folder * f ) { hierarchyModel()->SetPinnedFolder(f); }

Folder * Notebook::rootFolder() { return p_rootFolder; }
Folder * Notebook::tempFolder() { return p_tempFolder; }
Folder * Notebook::trashFolder() { return p_trashFolder; }
Folder * Notebook::pinnedFolder() { return hierarchyModel()->GetPinnedFolder(); }

QList < Note * > Notebook::notesList()
{
    return allNotes;
}
QList < Folder * > Notebook::foldersList()
{
    return allFolders;
}
QList < Tag * > Notebook::tagsList()
{
    return allTags;
}

HierarchyModel * Notebook::hierarchyModel()
{
    return p_hierarchyModel;
}
TagsModel * Notebook::tagsModel()
{
    return p_tagsModel;
}
DatesModel * Notebook::creationDateModel()
{
    return p_creationDateModel;
}
DatesModel * Notebook::modificationDateModel()
{
    return p_modificationDateModel;
}

void Notebook::sl_Folder_ItemAdded(AbstractFolderItem* const item)
{
    registerItem(item);
}
void Notebook::sl_Folder_ItemRemoved(AbstractFolderItem* const item)
{
    unregisterItem(item);
}

void Notebook::sl_Note_TagAdded(Tag* tag)
{
    if (!tag)
    {
        WARNING("Null pointer recieved");
        return;
    }

    if (!allTags.contains(tag))
        registerTag(tag);
}
void Notebook::sl_Note_TagRemoved(Tag* tag)
{
    if (!tag)
    {
        WARNING("Null pointer recieved");
        return;
    }
    if (!allTags.contains(tag))
    {
        WARNING("Unknown tag");
    }

    if (tag->Owners.Count() == 0)
        unregisterTag(tag);
}

void Notebook::registerItem(AbstractFolderItem* const item)
{
    if (item->GetItemType() == AbstractFolderItem::Type_Folder)
    {
        Folder* f = dynamic_cast<Folder*>(item);
        f->setParent(this); // QObject parentship

//        if (f->GetIconID().isEmpty()) {
//            f->SetIconID(DefaultFolderIcon);
//        }

        QObject::connect(f, SIGNAL(sg_ItemAdded(AbstractFolderItem*const, int)), SLOT(sl_Folder_ItemAdded(AbstractFolderItem*const)));
        QObject::connect(f, SIGNAL(sg_ItemRemoved(AbstractFolderItem*const)), SLOT(sl_Folder_ItemRemoved(AbstractFolderItem*const)));

        allFolders.append(f);
        emit sg_ItemRegistered(f);

        for (int i = 0; i < f->Items.Count(); i++)
            registerItem(f->Items.ItemAt(i));

    } else if (item->GetItemType() == AbstractFolderItem::Type_Note)
    {
        Note* n = dynamic_cast<Note*>(item);
        n->setParent(this);  // QObject parentship

//        if (n->GetIconID().isEmpty()) {
//            n->SetIconID(DefaultNoteIcon);
//        }
//        QObject::connect(n, SIGNAL(sg_DataChanged()), this, SLOT(sl_ItemDataChanged()));
        QObject::connect(n, SIGNAL(sg_TagAdded(Tag*)), SLOT(sl_Note_TagAdded(Tag*)));
        QObject::connect(n, SIGNAL(sg_TagRemoved(Tag*)), SLOT(sl_Note_TagRemoved(Tag*)));

        allNotes.append(n);
        emit sg_ItemRegistered(n);
    }
}
void Notebook::unregisterItem(AbstractFolderItem* const item)
{
    if (item->GetItemType() == AbstractFolderItem::Type_Folder)
    {
        Folder* f = dynamic_cast<Folder*>(item);
        QObject::disconnect(f, 0, this, 0);

        allFolders.removeAll(f);
        emit sg_ItemUnregistered(f);

        for (int i = 0; i < f->Items.Count(); i++)
            unregisterItem(f->Items.ItemAt(i));

    } else if (item->GetItemType() == AbstractFolderItem::Type_Note)
    {
        Note* n = dynamic_cast<Note*>(item);
        QObject::disconnect(n, 0, this, 0);

        n->Tags.Clear(); // ? Tags must be unregistered, but this line modifies note.

        allNotes.removeAll(n);

        emit sg_ItemUnregistered(n);
    }
}

void Notebook::registerTag(Tag* tag)
{
    allTags.append(tag);
    tag->setParent(this);
    tagsByName.insert(tag->GetName(), tag);
    QStandardItem* i = new QStandardItem( QIcon(":/gui/tag"), tag->GetName());
    tagsListModel->appendRow(i);
    tagsListModel->sort(0, Qt::AscendingOrder);
    emit sg_ItemRegistered(tag);
}
void Notebook::unregisterTag(Tag* tag)
{
    allTags.removeAll(tag);
    tag->setParent(0);
    tagsByName.remove(tag->GetName());
    QList<QStandardItem*> itemsList = tagsListModel->findItems(tag->GetName(), Qt::MatchExactly, 0);
    QStandardItem* item = tagsListModel->takeItem(itemsList.at(0)->row());
    delete item;

    emit sg_ItemUnregistered(tag);
    delete tag;
}
