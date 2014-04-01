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

#include "datenavigationwidget.h"

#include "datesmodel.h"
#include "basemodelitem.h"
#include "notemodelitem.h"

#include "notebook.h"
#include "utils/func.h"

#include <QVBoxLayout>
#include <QHeaderView>
#include <QDebug>

DateNavigationWidget::DateNavigationWidget(QWidget *parent)
    : QWidget(parent)
{
	creationDateModel = 0;
	modificationDateModel = 0;

	treeView = new QTreeView();
	treeView->setHeaderHidden(true);
	treeView->setEditTriggers(QAbstractItemView::SelectedClicked | QAbstractItemView::EditKeyPressed);

	QObject::connect(treeView, SIGNAL(clicked(QModelIndex)),
					 this, SLOT(sl_View_clicked(QModelIndex)));
	QObject::connect(treeView, SIGNAL(doubleClicked(QModelIndex)),
					 this, SLOT(sl_View_doubleClicked(QModelIndex)));

    creationDateButton = new QRadioButton( tr( "Creation date" ), this);
	QObject::connect(creationDateButton, SIGNAL(toggled(bool)),
					 this, SLOT(sl_updateTreeModel(bool)));
    modificationDateButton = new QRadioButton( tr( "Modification date" ), this);
	QObject::connect(modificationDateButton, SIGNAL(toggled(bool)),
					 this, SLOT(sl_updateTreeModel(bool)));

	QVBoxLayout* buttonsLayout = new QVBoxLayout();
	buttonsLayout->addWidget(creationDateButton);
	buttonsLayout->addWidget(modificationDateButton);

    groupBox = new QGroupBox( tr( "Traced date" ), this);
	groupBox->setLayout(buttonsLayout);

	QVBoxLayout* layout = new QVBoxLayout();
	layout->addWidget(groupBox);
	layout->addWidget(treeView);

	setLayout(layout);

	creationDateButton->toggle();
}

void DateNavigationWidget::sl_View_clicked (const QModelIndex& index) {
	if (!index.isValid()) {return;}

	BaseModelItem* item = static_cast<BaseModelItem*>(index.internalPointer());
	if (item->DataType() == BaseModelItem::note) {
		NoteModelItem* noteItem = dynamic_cast<NoteModelItem*>(item);
		if (noteItem == 0) {
			WARNING("Casting error");
			return;
		}
		Note* n = noteItem->GetStoredData();
		emit sg_NoteClicked(n);
	}
}

void DateNavigationWidget::sl_View_doubleClicked (const QModelIndex& index) {
	if (!index.isValid()) {return;}

	BaseModelItem* item = static_cast<BaseModelItem*>(index.internalPointer());
	if (item->DataType() == BaseModelItem::note) {
		NoteModelItem* noteItem = dynamic_cast<NoteModelItem*>(item);
		if (noteItem == 0) {
			WARNING("Casting error");
			return;
		}
		Note* n = noteItem->GetStoredData();
		emit sg_NoteDoubleClicked(n);
	}
}

void DateNavigationWidget::SetCreationModel(DatesModel* model) {
	creationDateModel = model;
	sl_updateTreeModel();
}

void DateNavigationWidget::SetModificationModel(DatesModel* model) {
	modificationDateModel = model;
	sl_updateTreeModel();
}

void DateNavigationWidget::expandAll()
{
    treeView->expandAll();
}

void DateNavigationWidget::sl_updateTreeModel(bool) {
    if (creationDateButton->isChecked())
		treeView->setModel(creationDateModel);

    else if (modificationDateButton->isChecked())
		treeView->setModel(modificationDateModel);

    if (treeView->model() != 0)
    {
        for (int i = 0; i < treeView->model()->columnCount(); i++)
			treeView->resizeColumnToContents(i);		
	}
}
