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

#ifndef QNM_MODELITEMDELEGATE_H
#define QNM_MODELITEMDELEGATE_H

#include <QItemDelegate>

class ModelItemDelegate : public QItemDelegate {
    Q_OBJECT
private:
    mutable bool _isEditing;
public:
    explicit ModelItemDelegate(QObject *parent = 0);

    virtual void paint (QPainter* painter, const QStyleOptionViewItem& option,
                        const QModelIndex& index) const;

    void setEditorData(QWidget* editor, const QModelIndex& index) const;
    bool isEditing() const;

private slots:
    void sl_Delegate_closeEditor (QWidget*);
};


#endif // QNM_MODELITEMDELEGATE_H
