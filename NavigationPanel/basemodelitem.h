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

QString stringType( BaseModelItem * item );

#endif // BASEMODELITEM_H
