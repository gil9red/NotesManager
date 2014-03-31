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

#include "tagsnavigationwidget.h"

#include "notebook.h"
#include "utils/func.h"
#include "tagsmodel.h"
#include "basemodelitem.h"
#include "notemodelitem.h"

#include <QVBoxLayout>
#include <QHeaderView>
#include <QDebug>



TagsNavigationWidget::TagsNavigationWidget(QWidget *parent)
    : QWidget(parent)
{
	model = 0;

	treeView = new QTreeView();
	treeView->setHeaderHidden(true);
	treeView->setEditTriggers(QAbstractItemView::SelectedClicked | QAbstractItemView::EditKeyPressed);

	QObject::connect(treeView, SIGNAL(clicked(QModelIndex)), SLOT(sl_View_clicked(QModelIndex)));
	QObject::connect(treeView, SIGNAL(doubleClicked(QModelIndex)), SLOT(sl_View_doubleClicked(QModelIndex)));

	QVBoxLayout* layout = new QVBoxLayout();
	layout->addWidget(treeView);

	setLayout(layout);
}

void TagsNavigationWidget::SetModel(TagsModel* m) {
    model = m;
	treeView->setModel(model);

    if (treeView->model() != 0)
    {
        for (int i = 0; i < treeView->model()->columnCount(); i++)
			treeView->resizeColumnToContents(i);		
	}
}

void TagsNavigationWidget::sl_View_clicked (const QModelIndex& index) {
	if (!index.isValid()) {return;}

	BaseModelItem* item = static_cast<BaseModelItem*>(index.internalPointer());
	if (item->DataType() == BaseModelItem::note) {
		Note* n = dynamic_cast<NoteModelItem*>(item)->GetStoredData();
		if (!n) {
			WARNING("Casting error");
			return;
		}
		emit sg_NoteClicked(n);
	}
}

void TagsNavigationWidget::sl_View_doubleClicked (const QModelIndex& index) {
	if (!index.isValid()) {return;}

	BaseModelItem* item = static_cast<BaseModelItem*>(index.internalPointer());
	if (item->DataType() == BaseModelItem::note) {
		Note* n = dynamic_cast<NoteModelItem*>(item)->GetStoredData();
		if (!n) {
			WARNING("Casting error");
			return;
		}
		emit sg_NoteDoubleClicked(n);
	}
}
