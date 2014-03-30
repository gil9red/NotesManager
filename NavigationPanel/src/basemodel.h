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

#ifndef QNM_BASEMODEL_H
#define QNM_BASEMODEL_H

#include <QAbstractItemModel>

class BaseModelItem;

class BaseModel : public QAbstractItemModel
{
    Q_OBJECT

private:
    BaseModelItem* displayRootItem;
    BaseModelItem* rootItem;

protected:
    void SetDisplayRootItem(BaseModelItem*);
    BaseModelItem* const GetDisplayRootItem() const;
    void SetRootItem(BaseModelItem*);
    BaseModelItem* GetRootItem() const;

public:
    explicit BaseModel(QObject *parent = 0);
    virtual ~BaseModel();

    QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex& child) const;
    int rowCount(const QModelIndex& parent = QModelIndex()) const;
    int columnCount(const QModelIndex& parent = QModelIndex()) const;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
    bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole);
    Qt::ItemFlags flags (const QModelIndex& index) const;

signals:
    void sg_ApplySelection(const QModelIndexList&);
    void sg_DisplayRootItemChanged();

public slots:
    void EmitDataChanged(QModelIndex&, QModelIndex&);

};

#endif // QNM_BASEMODEL_H
