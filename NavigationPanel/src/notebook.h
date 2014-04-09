#ifndef QNM_NOTEBOOK_H
#define QNM_NOTEBOOK_H

#include <QObject>
#include "folder.h"
#include "hierarchymodel.h"
#include "datesmodel.h"
#include "tagsmodel.h"
#include <QStandardItemModel>
#include "Note/RichTextNote.h"
#include "note.h"

class QDomElement;
class QDomDocument;

class Notebook : public QObject
{
    Q_OBJECT

public:
    explicit Notebook( QObject * parent = 0 );
    ~Notebook();

    static Notebook * instance();

    void read( QDomElement & root );
    void write( QDomElement & root, QDomDocument & xmlDomDocument );

    void setRootFolder( Folder * f );
    void setTrashFolder( Folder * f );
    void setPinnedFolder( Folder * f );

    Folder * getRootFolder();
    Folder * getTrashFolder();
    Folder * getPinnedFolder();

    QList < Note * > getNotesList();
    QList < Folder * > getFoldersList();
    QList < Tag * > getTagsList();

    HierarchyModel * getHierarchyModel();
    TagsModel * getTagsModel();
    DatesModel * getCreationDateModel();
    DatesModel * getModificationDateModel();


    QString getIdFromNote( Note * note );
    Note * getNoteFromId( const QString & id );
    RichTextNote * getRichTextNoteFromId( const QString & id );
    Note * getNoteFromRichTextNote( RichTextNote * richTextNote );
    RichTextNote * getRichTextNoteFromNote( Note * note );


private:
    Folder * rootFolder;
    Folder * trashFolder;
    Folder * pinnedFolder;

    QList < Note * > allNotes;
    QList < Folder * > allFolders;
    QList < Tag * > allTags;


    QHash < QString, Note * > hash_Id_Note;
    QHash < Note *, RichTextNote * > hash_Note_RichTextNote;


    HierarchyModel * hierarchyModel;
    TagsModel * tagsModel;
    DatesModel * creationDateModel;
    DatesModel * modificationDateModel;


    QMap < QString, Tag * > tagsByName; // для быстрого нахождение тэга по имени

    static Notebook * self;

private slots:
    void sl_RichTextNote_EventChange( int event );

    void sl_Folder_ItemAdded(AbstractFolderItem* const item);
    void sl_Folder_ItemRemoved(AbstractFolderItem* const item);

    void sl_Note_TagAdded(Tag* tag);
    void sl_Note_TagRemoved(Tag* tag);

protected:
    void registerItem(AbstractFolderItem* item);
    void unregisterItem(AbstractFolderItem* item);

    void registerTag(Tag* tag);
    void unregisterTag(Tag* tag);

signals:
    void sg_RichTextNote_About_EventChange( RichTextNote * richTextNote , int event );

    void sg_ItemRegistered(Folder*);
    void sg_ItemRegistered(Note*);
    void sg_ItemRegistered(Tag*);

    void sg_ItemUnregistered(Folder*);
    void sg_ItemUnregistered(Note*);
    void sg_ItemUnregistered(Tag*);
};


#endif // QNM_NOTEBOOK_H
