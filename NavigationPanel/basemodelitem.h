#ifndef BASEMODELITEM_H
#define BASEMODELITEM_H

#include <QStandardItemModel>

class BaseModelItem: public QStandardItem
{
public:
    enum Type { BASE, NOTE, FOLDER, TRASH, ERROR = -1,
                ROLE_TYPE = Qt::UserRole + 1,
                NOTE_TYPE = ROLE_TYPE + 1
              };

public:
    explicit BaseModelItem( const QString & text = QString() );
    virtual ~BaseModelItem() {}

    bool isEmpty();
    bool isBase();
    bool isNote();
    bool isFolder();
    bool isTrash();

    BaseModelItem::Type type();
    void setType( Type t );
};

static QString stringType( BaseModelItem * item )
{
    QString name;
    if ( item->isBase() )
        name = "Base";
    else if ( item->isNote() )
        name = "Note";
    else if ( item->isFolder() )
        name = "Folder";
    else if ( item->isTrash() )
        name = "Trash";
    return name;
}

#endif // BASEMODELITEM_H
