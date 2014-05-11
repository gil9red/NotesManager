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
    {
        Note * note = new Note();
        return note;

    } else if ( type == "Folder" )
        return new Folder();

    else if ( type == "Trash" )
        return Notebook::instance()->getTrashFolder();

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

    bool isFolder = item->getItemType() == AbstractFolderItem::Type_Folder;
    bool isNote = item->getItemType() == AbstractFolderItem::Type_Note;

    if ( isFolder )
    {
        Folder * folder = static_cast < Folder * > ( item );
        folder->setName( element.attribute( "name" ) );
    }

    item->setNameForeColor( QColor( element.attribute( "fore_color", "#000000" ) ) );
    item->setNameBackColor( QColor( element.attribute( "back_color", "#FFFFFF" ) ) );

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
        if ( item->getItemType() == AbstractFolderItem::Type_Note )
            folder->child.Add( item );

        else if ( item->getItemType() == AbstractFolderItem::Type_Folder )
        {
            folder->child.Add( item );
            parseDomElement( static_cast < Folder * > ( item ), child );
        }

        child = child.nextSiblingElement();
    }
}

QDomElement createDomElementOfItem( AbstractFolderItem * item, QDomDocument & xmlDomDocument )
{
    bool isFolder = item->getItemType() == AbstractFolderItem::Type_Folder;
    bool isNote = item->getItemType() == AbstractFolderItem::Type_Note;

    Folder * folder = static_cast < Folder * > ( item );

    QString tagName;
    if ( isNote )
        tagName = "Note";

    else if ( isFolder )
    {
        if ( folder->getType() == Folder::UserFolder )
            tagName = "Folder";

    } else
    {
        WARNING( "Unknown type" );
        return QDomElement();
    }

    QDomElement element = xmlDomDocument.createElement( tagName );
    element.setAttribute( "fore_color", item->getNameForeColor().name() );
    element.setAttribute( "back_color", item->getNameBackColor().name() );

    if ( isFolder )
    {
        element.setAttribute( "name", folder->getName() );

    } else if ( isNote )
    {
        Note * note = static_cast < Note * > ( item );
        RichTextNote * richTextNote = note->getRichTextNote();
        richTextNote->save();

        const QString & id = getIdFromRichTextNote( richTextNote );
        element.setAttribute( "id", id );
    }

    return element;
}
void parseItem( Folder * folder, QDomElement & element, QDomDocument & xmlDomDocument )
{
    for ( int i = 0; i < folder->child.Count(); i++ )
    {
        AbstractFolderItem * child = folder->child.ItemAt(i);
        QDomElement domChild = createDomElementOfItem( child, xmlDomDocument );
        element.appendChild( domChild );
        if ( child->getItemType() == AbstractFolderItem::Type_Folder )
            parseItem( static_cast < Folder * > ( child ), domChild, xmlDomDocument );
    }
}


#include <QDebug>

Notebook * Notebook::self = 0;

Notebook::Notebook( QObject * parent )
    : QObject( parent )
{
    self = this;

    rootFolder = new Folder( "_root_", Folder::SystemFolder );
    QObject::connect( rootFolder, SIGNAL(sg_ItemAdded(AbstractFolderItem * const, int)), SLOT(sl_Folder_ItemAdded(AbstractFolderItem* const)) );
    QObject::connect( rootFolder, SIGNAL(sg_ItemRemoved(AbstractFolderItem*const)), SLOT(sl_Folder_ItemRemoved(AbstractFolderItem*const)) );

    trashFolder = new Folder( QString(), Folder::TrashFolder );
    QObject::connect( trashFolder, SIGNAL(sg_ItemAdded(AbstractFolderItem*const, int)), SLOT(sl_Folder_ItemAdded(AbstractFolderItem* const)) );
    QObject::connect( trashFolder, SIGNAL(sg_ItemRemoved(AbstractFolderItem*const)), SLOT(sl_Folder_ItemRemoved(AbstractFolderItem*const)) );

    hierarchyModel = new HierarchyModel( this );
    tagsModel = new TagsModel( this );
    creationDateModel = new DatesModel( DatesModel::CreationDate, this );
    modificationDateModel = new DatesModel( DatesModel::ModifyDate, this );


    QObject::connect( this, SIGNAL(sg_ItemRegistered(Note*)), SIGNAL(notesChange()) );
    QObject::connect( this, SIGNAL(sg_ItemRegistered(Folder*)), SIGNAL(notesChange()) );
    QObject::connect( this, SIGNAL(sg_ItemUnregistered(Note*)), SIGNAL(notesChange()) );
    QObject::connect( this, SIGNAL(sg_ItemUnregistered(Folder*)), SIGNAL(notesChange()) );
    QObject::connect( this, SIGNAL(sg_RichTextNote_About_EventChange(RichTextNote*,int)), SIGNAL(notesChange()) );
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

void Notebook::read( QDomElement & root )
{
    QDomElement rootNotes = root.firstChildElement( "Notes" );
    QDomElement rootTrash = root.firstChildElement( "Trash" );

    parseDomElement( getRootFolder(), rootNotes );
    parseDomElement( getTrashFolder(), rootTrash );
}
void Notebook::write( QDomElement & root, QDomDocument & xmlDomDocument )
{
    // У Notebook есть дети Notes и Tabs
    QDomElement rootNotes = xmlDomDocument.createElement( "Notes" );
    QDomElement rootTrash = xmlDomDocument.createElement( "Trash" );

    root.appendChild( rootNotes );
    root.appendChild( rootTrash );

    parseItem( getRootFolder(), rootNotes, xmlDomDocument );
    parseItem( getTrashFolder(), rootTrash, xmlDomDocument );
}

void Notebook::setRootFolder( Folder * f ) { rootFolder = f; }
void Notebook::setTrashFolder( Folder * f ) { trashFolder = f; }
void Notebook::setPinnedFolder( Folder * f ) { getHierarchyModel()->setPinnedFolder(f); }

Folder * Notebook::getRootFolder() { return rootFolder; }
Folder * Notebook::getTrashFolder() { return trashFolder; }
Folder * Notebook::getPinnedFolder() { return getHierarchyModel()->getPinnedFolder(); }

QList < Note * > Notebook::getNotesList()
{
    return allNotes;
}
QList < Folder * > Notebook::getFoldersList()
{
    return allFolders;
}
QList < Tag * > Notebook::getTagsList()
{
    return allTags;
}

HierarchyModel * Notebook::getHierarchyModel()
{
    return hierarchyModel;
}
TagsModel * Notebook::getTagsModel()
{
    return tagsModel;
}
DatesModel * Notebook::getCreationDateModel()
{
    return creationDateModel;
}
DatesModel * Notebook::getModificationDateModel()
{
    return modificationDateModel;
}

QString Notebook::getIdFromNote( Note * note )
{
    return hash_Id_Note.key( note );
}
Note * Notebook::getNoteFromId( const QString & id )
{
    return hash_Id_Note.value( id, 0 );
}
RichTextNote * Notebook::getRichTextNoteFromId( const QString & id )
{
    if ( hash_Id_Note.contains( id ) )
        return hash_Id_Note.value( id )->getRichTextNote();
    return 0;
}
Note * Notebook::getNoteFromRichTextNote( RichTextNote * richTextNote )
{
    return hash_Note_RichTextNote.key( richTextNote, 0 );
}
RichTextNote * Notebook::getRichTextNoteFromNote( Note * note )
{
    return hash_Note_RichTextNote.value( note, 0 );
}

void Notebook::sl_RichTextNote_EventChange( int event )
{
    RichTextNote * richTextNote = qobject_cast < RichTextNote * > ( QObject::sender() );
    if ( !richTextNote )
    {
        WARNING( "bad cast!" );
        return;
    }

    emit sg_RichTextNote_About_EventChange( richTextNote, event );
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

void Notebook::registerItem(AbstractFolderItem* item)
{
    if (item->getItemType() == AbstractFolderItem::Type_Folder)
    {
        Folder* f = dynamic_cast<Folder*>(item);
        f->QObject::setParent(this); // QObject parentship

        QObject::connect(f, SIGNAL(sg_ItemAdded(AbstractFolderItem*const, int)), SLOT(sl_Folder_ItemAdded(AbstractFolderItem*const)));
        QObject::connect(f, SIGNAL(sg_ItemRemoved(AbstractFolderItem*const)), SLOT(sl_Folder_ItemRemoved(AbstractFolderItem*const)));        

        allFolders.append(f);
        emit sg_ItemRegistered(f);

        for (int i = 0; i < f->child.Count(); i++)
            registerItem(f->child.ItemAt(i));

    } else if (item->getItemType() == AbstractFolderItem::Type_Note)
    {       
        Note * note = dynamic_cast < Note * > ( item );
        note->QObject::setParent(this);  // QObject parentship

        QObject::connect(note, SIGNAL(sg_TagAdded(Tag*)), SLOT(sl_Note_TagAdded(Tag*)));
        QObject::connect(note, SIGNAL(sg_TagRemoved(Tag*)), SLOT(sl_Note_TagRemoved(Tag*)));

        RichTextNote * richTextNote = note->getRichTextNote();
        QObject::connect( richTextNote, SIGNAL(changed(int)), SLOT(sl_RichTextNote_EventChange(int)) );

        allNotes.append(note);
        hash_Id_Note.insert( getIdFromRichTextNote( richTextNote ), note );
        hash_Note_RichTextNote.insert( note, richTextNote );
        emit sg_ItemRegistered(note);
    }
}
void Notebook::unregisterItem(AbstractFolderItem* item)
{
    if (item->getItemType() == AbstractFolderItem::Type_Folder)
    {
        Folder* f = dynamic_cast<Folder*>(item);
        QObject::disconnect(f, 0, this, 0);

        allFolders.removeAll(f);
        emit sg_ItemUnregistered(f);

        for (int i = 0; i < f->child.Count(); i++)
            unregisterItem(f->child.ItemAt(i));

    } else if (item->getItemType() == AbstractFolderItem::Type_Note)
    {
        Note * note = dynamic_cast < Note * > ( item );
        QObject::disconnect(note, 0, this, 0);

        note->Tags.Clear(); // ? Tags must be unregistered, but this line modifies note.

        allNotes.removeAll(note);
        hash_Id_Note.remove( hash_Id_Note.key( note ) );
        hash_Note_RichTextNote.remove( note );
        emit sg_ItemUnregistered( note );
    }
}

void Notebook::registerTag(Tag* tag)
{
    allTags.append(tag);
    tag->setParent(this);
    tagsByName.insert(tag->getName(), tag);

    emit sg_ItemRegistered(tag);
}
void Notebook::unregisterTag(Tag* tag)
{
    allTags.removeAll(tag);
    tag->setParent(0);
    tagsByName.remove(tag->getName());

    emit sg_ItemUnregistered(tag);
    delete tag;
}
