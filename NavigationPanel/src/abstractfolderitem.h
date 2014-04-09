/*
This file is part of qNotesManager.

qNotesManager is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

qNotesManager is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with qNotesManager. If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef QNM_ABSTRACTFOLDERITEM_H
#define QNM_ABSTRACTFOLDERITEM_H

#include <QObject>

/*
AbstractFolderItem is an interface for items, that Folder may contain
*/

#include <QDateTime>
#include <QColor>
#include <QIcon>

class Folder;

class AbstractFolderItem : public QObject
{
    Q_OBJECT

    friend class FolderItemCollection;

public:
    virtual ~AbstractFolderItem();

    enum ItemType
    {
        Type_Folder,
        Type_Note
    };

    ItemType getItemType() const;
    Folder * getParent() const;
    bool isOffspringOf(const Folder*) const;


    QString getName() const;
    void setName(const QString &);

    QIcon getIcon() const;
    void setIcon(const QIcon &);

    QDateTime getCreationDate() const;
    QDateTime getModificationDate() const;

    QColor getNameForeColor() const;
    void setNameForeColor(const QColor &);

    QColor getNameBackColor() const;
    void setNameBackColor(const QColor &);

    QColor getDefaultForeColor() const;
    QColor getDefaultBackColor() const;


private:
    Folder * parent;
    const ItemType itemType;

    void setParent( Folder * newParent );

protected:
    QString name;
    QIcon icon;
    const QColor defaultForeColor;
    const QColor defaultBackColor;
    QColor nameForeColor;
    QColor nameBackColor;
    QDateTime creationDate;
    QDateTime modificationDate;

protected:
    explicit AbstractFolderItem(ItemType, const QString & n = QString()); // Make it unable to create an instance of this class
    void onChange();

signals:
    void sg_ParentChanged(const Folder* newParent);
    void sg_VisualPropertiesChanged(); // emitted when name or icon or other properties has been changed
    void sg_DataChanged();
};


#endif // QNM_ABSTRACTFOLDERITEM_H
