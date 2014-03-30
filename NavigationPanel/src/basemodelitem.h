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

#ifndef QNM_ABSTRACTMODELITEM_H
#define QNM_ABSTRACTMODELITEM_H

#include <QList>
#include <QVariant>

/*
  Base class for all model items
*/

class BaseModel;

class BaseModelItem : public QObject
{
    Q_OBJECT
public:
    enum ItemType
    {
        note,
        folder,
        tag,
        date,
        dotDot, // 'Two dots' item. Used to move to parent folder. Not implemented, reserved for future use
        SearchResult,
        Null, // Null item, doesn't return any data, may contain other items though
        Separator // Separator item
    };

private:
    BaseModelItem*			parentItem;
    QList<BaseModelItem*>	childrenList;
    const ItemType itemType;

    bool sorted;
    Qt::SortOrder sortOrder;

    int findInsertIndex_Sorted(const BaseModelItem*) const;
    int findInsertIndex_Simple(const BaseModelItem*) const;

    class IndexCache
    {
    public:
        IndexCache() : prt(0), cachedIndex(-1) {}
        const BaseModelItem* prt;
        int cachedIndex;

        void Clear() {prt = 0; cachedIndex = -1;}
        bool IsValid() {return cachedIndex >= 0;}
        void Set(BaseModelItem* p, int i) {prt = p; cachedIndex = i;}
    };

    IndexCache insertIndexCache;

public:
    BaseModelItem(ItemType type = Null);
    virtual ~BaseModelItem();

    ItemType DataType() const;

    virtual QVariant data(int role) const;
    virtual bool setData(const QVariant& value, int role);
    virtual BaseModelItem* parent() const;
    virtual Qt::ItemFlags flags () const;

    virtual void AddChild(BaseModelItem* n);
    virtual void AddChildTo(BaseModelItem* n, int position);
    virtual void RemoveChild(BaseModelItem* n);
    virtual int IndexOfChild(BaseModelItem*) const;
    virtual int ChildrenCount() const;
    virtual BaseModelItem* ChildAt(int index) const;
    void ClearChildrenList();
    bool IsOffspringOf(const BaseModelItem*) const;

    int FindInsertIndex(const BaseModelItem*) const;

    virtual bool LessThan(const BaseModelItem*) const;
    bool IsSorted() const;
    void SetSorted(const bool);
    Qt::SortOrder GetSortOrder() const;
    void SetSortOrder(const Qt::SortOrder);

signals:
    void sg_DataChanged(BaseModelItem*);
};

#endif // QNM_ABSTRACTMODELITEM_H
