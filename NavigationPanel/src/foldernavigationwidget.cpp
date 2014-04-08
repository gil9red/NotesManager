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

#include "foldernavigationwidget.h"
#include "hierarchymodel.h"
#include "abstractfolderitem.h"
#include "basemodelitem.h"
#include "foldermodelitem.h"
#include "notemodelitem.h"
#include "folder.h"
#include "note.h"
#include "modelitemdelegate.h"
#include "utils/func.h"
#include "notebook.h"
#include "tag.h"

#include <QVBoxLayout>
#include <QColorDialog>
#include <QMessageBox>
#include <QKeyEvent>
#include <QRegExp>
#include <QStack>
#include <QDebug>


FolderNavigationWidget::FolderNavigationWidget(QWidget *parent)
    : QWidget(parent)
{
	treeView = new QTreeView();
	treeView->setContextMenuPolicy(Qt::CustomContextMenu);
	treeView->setDragEnabled(true);
	treeView->setDropIndicatorShown(true);
	treeView->setDragDropMode(QAbstractItemView::InternalMove);
	treeView->viewport()->setAcceptDrops(true);
	treeView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    treeView->setEditTriggers( QAbstractItemView::NoEditTriggers );
    treeView->setItemDelegate( new ModelItemDelegate() );   
	treeView->installEventFilter(this);
	treeView->setHeaderHidden(true);
	treeView->setEnabled(false);

    QObject::connect(treeView, SIGNAL(customContextMenuRequested(QPoint)), SLOT(sl_TreeView_ContextMenuRequested(QPoint)));
    QObject::connect(treeView, SIGNAL(clicked(QModelIndex)), SLOT(sl_View_clicked(QModelIndex)));
    QObject::connect(treeView, SIGNAL(doubleClicked(QModelIndex)), SLOT(sl_View_doubleClicked(QModelIndex)));
    QObject::connect(treeView, SIGNAL(expanded(QModelIndex)), SLOT(sl_View_Expanded(QModelIndex)));
    QObject::connect(treeView, SIGNAL(collapsed(QModelIndex)), SLOT(sl_View_Collapsed(QModelIndex)));

    treeView->viewport()->installEventFilter(this);

	model = 0;

	currentRootLabel = new QLabel();
	currentRootLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Preferred);
	currentRootLabel->setAlignment(Qt::AlignRight | Qt::AlignCenter);

    pinFolderButton = new QToolButton();
    pinFolderButton->setText( tr( "Pin folder" ) );
    pinFolderButton->setToolTip( tr( "Pin folder" ) );
    pinFolderButton->setIcon( QIcon( ":/fugue-icons/pin" ) );
    pinFolderButton->setAutoRaise( true );
    pinFolderButton->setCheckable( true );
    pinFolderButton->setChecked( false );
    pinFolderButton->setFocusPolicy( Qt::NoFocus );
    QObject::connect(pinFolderButton, SIGNAL(toggled(bool)), SLOT(sl_PinFolderButton_Toggled(bool)));


    QVBoxLayout* layout = new QVBoxLayout();
    QHBoxLayout* pinnedFolderLayout = new QHBoxLayout();
    pinnedFolderLayout->setMargin(0);
    pinnedFolderLayout->addWidget(currentRootLabel);
    pinnedFolderLayout->setStretchFactor(currentRootLabel, 0);
    pinnedFolderLayout->addWidget(pinFolderButton);
    layout->addLayout(pinnedFolderLayout);
    layout->addWidget(treeView);
    setLayout(layout);

    // Item actions
    addNoteAction = new QAction( QIcon( ":/fugue-icons/plus" ), tr( "Add Note" ), this );
    QObject::connect(addNoteAction, SIGNAL(triggered()), SLOT(sl_AddNoteAction_Triggered()));

    addFolderAction = new QAction( QIcon( ":/fugue-icons/folder--plus" ), tr( "Add Folder" ), this );
    QObject::connect(addFolderAction, SIGNAL(triggered()), SLOT(sl_AddFolderAction_Triggered()));

    deleteItemAction = new QAction( QIcon( ":/fugue-icons/cross" ), tr( "Delete" ), this );
    QObject::connect(deleteItemAction, SIGNAL(triggered()), SLOT(sl_DeleteItemAction_Triggered()));

    moveToBinAction = new QAction( QIcon( ":/fugue-icons/minus" ), tr( "Move to Bin" ), this );
    QObject::connect(moveToBinAction, SIGNAL(triggered()), SLOT(sl_MoveToBinAction_Triggered()));

    itemForeColorMenu = new QMenu( tr( "Set Text Color" ), this );
    itemForeColorMenu->setIcon( QIcon( ":/fugue-icons/edit-color" ) );
    itemBackColorMenu = new QMenu( tr( "Set Back Color" ), this );
    itemBackColorMenu->setIcon( QIcon( ":/fugue-icons/paint-can-color" ) );

    itemDefaultForeColorAction = new QAction( QIcon( ":/fugue-icons/ui-color-picker-default" ), tr( "Default Color" ), this );
    QObject::connect(itemDefaultForeColorAction, SIGNAL(triggered()), SLOT(sl_DefaultForeColor_Triggered()));

    itemCustomForeColorAction = new QAction( QIcon( ":/fugue-icons/color-swatch" ), tr( "Custom Color" ), this );
    QObject::connect(itemCustomForeColorAction, SIGNAL(triggered()), SLOT(sl_CustomForeColor_Triggered()));

    itemDefaultBackColorAction = new QAction( QIcon( ":/fugue-icons/ui-color-picker-default" ), tr( "Default Color" ), this);
    QObject::connect(itemDefaultBackColorAction, SIGNAL(triggered()), SLOT(sl_DefaultBackColor_Triggered()));

    itemCustomBackColorAction = new QAction( QIcon( ":/fugue-icons/color-swatch" ), tr( "Custom Color" ), this );
    QObject::connect(itemCustomBackColorAction, SIGNAL(triggered()), SLOT(sl_CustomBackColor_Triggered()));

    clearTrashAction = new QAction( QIcon( ":/Manager/remove_all" ), tr( "Clear Trash" ), this );
    QObject::connect(clearTrashAction, SIGNAL(triggered()), SLOT(sl_ClearTrashAction_Triggered()));

    openNoteAction = new QAction( QIcon( ":/fugue-icons/book-open" ), tr( "Open" ), this);
    QObject::connect(openNoteAction, SIGNAL(triggered()), SLOT(sl_OpenNoteAction_Triggered()));

    renameItemAction = new QAction( QIcon( ":/fugue-icons/document-rename" ), tr( "Rename" ), this);
    QObject::connect(renameItemAction, SIGNAL(triggered()), SLOT(sl_RenameItemAction_Triggered()));

    itemForeColorMenu->addAction(itemDefaultForeColorAction);
    itemForeColorMenu->addAction(itemCustomForeColorAction);
    itemBackColorMenu->addAction(itemDefaultBackColorAction);
    itemBackColorMenu->addAction(itemCustomBackColorAction);


    sl_UpdateStates();

    QObject::connect( Notebook::instance(), SIGNAL(sg_RichTextNote_About_EventChange(RichTextNote*,int)), SLOT(sl_NoteEventChange(RichTextNote*,int)) );
}
void FolderNavigationWidget::setModel(HierarchyModel* m)
{
    if (model)
        QObject::disconnect(model, 0, this, 0);

    model = m;

    if (model)
    {
        QObject::connect(model, SIGNAL(sg_ApplySelection(QModelIndexList)), SLOT(sl_Model_ApplySelection(QModelIndexList)));
        QObject::connect(model, SIGNAL(sg_DisplayRootItemChanged()), SLOT(sl_Model_DisplayRootItemChanged()));
    }

    treeView->setModel(model);
    if (treeView->model() != 0)
    {
        for (int i = 0; i < treeView->model()->columnCount(); i++)
            treeView->resizeColumnToContents(i);
        QObject::connect(treeView->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)), SLOT(sl_View_SelectionChanged(QItemSelection,QItemSelection)));
        restoreExpandedIndexes();
    }

    treeView->setEnabled(model != 0);
    sl_UpdateStates();

    if (model)
    {
        pinFolderButton->blockSignals(true);
        pinFolderButton->setChecked(model->getPinnedFolder() != 0);
        pinFolderButton->blockSignals(false);
        updatePinnedFolderData();
    } else
    {
        pinFolderButton->blockSignals(true);
        pinFolderButton->setChecked(false);
        pinFolderButton->blockSignals(false);
        updatePinnedFolderData();
    }
}
void FolderNavigationWidget::updatePinnedFolderData()
{
    if (model && model->getPinnedFolder())
    {
        QString path = model->getPinnedFolder()->getPath();
        currentRootLabel->setText(path);
        currentRootLabel->setToolTip(path);
    } else
    {
        currentRootLabel->setText("");
        currentRootLabel->setToolTip("");
    }
}

bool FolderNavigationWidget::hasCurrentItem()
{
    return treeView->currentIndex().isValid();
}

BaseModelItem * FolderNavigationWidget::getItem( const QModelIndex & index )
{
    if ( !index.isValid() )
    {
        WARNING( "is not valid index!" );
        return 0;
    }

    BaseModelItem * item = static_cast < BaseModelItem * > ( index.internalPointer() );
    return item;
}
BaseModelItem * FolderNavigationWidget::getCurrentItem()
{
    const QModelIndex & index = treeView->currentIndex();
    if ( !index.isValid() )
    {
        WARNING( "is not valid index!" );
        return 0;
    }

    return getItem( index );
}
Note * FolderNavigationWidget::getNote( const QModelIndex & index )
{
    BaseModelItem * item = getItem( index );
    if ( item->DataType() == BaseModelItem::note )
    {
        Note * note = dynamic_cast < NoteModelItem * > ( item )->getStoredData();
        if ( !note )
        {
            WARNING("Casting error");
            return 0;
        }

        return note;
    }

    return 0;
}
Folder * FolderNavigationWidget::getFolder( const QModelIndex & index )
{
    BaseModelItem * item = getItem( index );
    if ( item->DataType() == BaseModelItem::folder )
    {
        Folder * folder = dynamic_cast < FolderModelItem * > ( item )->getStoredData();
        if ( !folder )
        {
            WARNING("Casting error");
            return 0;
        }

        return folder;
    }

    return 0;
}
Note * FolderNavigationWidget::getCurrentNote()
{
    const QModelIndex & index = treeView->currentIndex();
    return getNote( index );
}
Folder * FolderNavigationWidget::getCurrentFolder()
{
    const QModelIndex & index = treeView->currentIndex();
    return getFolder( index );
}

void FolderNavigationWidget::deleteItems(QModelIndexList& indexesList, bool permanently)
{
    QString details;
    if (indexesList.isEmpty())
    {
        WARNING("List is empty");
        return;
    }

    foreach (QModelIndex index, indexesList)
        details.append("\n").append(index.model()->data(index, Qt::DisplayRole).toString());

    const QString & message = permanently ? tr( "Delete these items?" ) : tr( "Put these items to Bin?" );
    bool hasCancel = QMessageBox::question( this, tr( "Confirm deletion" ), message + details, QMessageBox::Yes | QMessageBox::No ) != QMessageBox::Yes;
    if ( hasCancel )
        return;

    // FIXME: fix situation when parent folder was deleted and we try to delete child item

    qApp->setOverrideCursor( Qt::WaitCursor );

    deleteChildIndexes(indexesList);

    foreach (QModelIndex index, indexesList)
    {
        if (!index.isValid())
        {
            WARNING("Invalid index in list");
            continue;
        }

        BaseModelItem* modelItemToDelete = static_cast<BaseModelItem*>(index.internalPointer());

        bool isFolder = modelItemToDelete->DataType() == BaseModelItem::folder;
        bool isNote = modelItemToDelete->DataType() == BaseModelItem::note;

        AbstractFolderItem* itemToDelete = 0;
        if ( isFolder )
        {
            FolderModelItem* fmi = dynamic_cast<FolderModelItem*>(modelItemToDelete);
            itemToDelete = fmi->getStoredData();

            if ( itemToDelete == Notebook::instance()->trashFolder() )
            {
                QMessageBox::information(0, tr( "Information" ), tr( "You cannot delete system folders" ) );
                continue;
            }
        } else if ( isNote )
        {
            NoteModelItem* fmi = dynamic_cast<NoteModelItem*>(modelItemToDelete);
            itemToDelete = fmi->getStoredData();
        }
        if (!itemToDelete)
        {
            WARNING("Could not find an item for deletion");
            continue;
        }

        Folder * parentFolder = itemToDelete->getParent();
        if (!parentFolder)
        {
            WARNING("Cannot delete item without parent");
            continue;
        }

        if (permanently)
        {
            WARNING("permanently delete");
            parentFolder->child.Remove(itemToDelete);
            delete itemToDelete;

        } else
            parentFolder->child.Move(itemToDelete, Notebook::instance()->trashFolder());
    }

    qApp->restoreOverrideCursor();
}
void FolderNavigationWidget::deleteChildIndexes(QModelIndexList& list) const
{
    QMutableListIterator<QModelIndex> iterator(list);

    while(iterator.hasNext())
    {
        QModelIndex index = iterator.next();

        bool deleteCurrentIndex = false;
        QModelIndex parent = index.parent();
        while(parent.isValid())
        {
            if (list.contains(parent))
            {
                deleteCurrentIndex = true;
                break;
            }
            parent = parent.parent();
        }
        if (deleteCurrentIndex)
            iterator.remove();
    }
}
void FolderNavigationWidget::restoreExpandedIndexes()
{
    QStack<QModelIndex> indexes;
    indexes.push(treeView->rootIndex());

    while (!indexes.isEmpty())
    {
        QModelIndex index = indexes.pop();
        for (int i = 0; i < model->rowCount(index); i++)
        {
            QModelIndex child = model->index(i, 0, index);
            if (child.isValid())
                indexes.push(child);
        }

        if (!index.isValid()) continue;
        if (index.internalPointer() == 0) continue;
        BaseModelItem* modelItemToEdit = static_cast<BaseModelItem*>(index.internalPointer());

        if (modelItemToEdit->DataType() == BaseModelItem::folder)
        {
            Folder* folder = (dynamic_cast<FolderModelItem*>(modelItemToEdit))->getStoredData();
            if (folder->isExpanded())
                treeView->expand(index);
        }
    }
}

void FolderNavigationWidget::sl_TreeView_ContextMenuRequested(const QPoint& p)
{
    const QItemSelectionModel * selectionModel = treeView->selectionModel();
    if ( !selectionModel )
        return;

    QMenu menu;
    bool isEmptyTrash = Notebook::instance()->trashFolder()->child.Count() == 0;
    const QModelIndexList & indexesList = selectionModel->selectedIndexes();

    // 0 items
    if (indexesList.isEmpty())
    {
        menu.addAction(addNoteAction);
        menu.addAction(addFolderAction);

        if (model->getPinnedFolder() != 0)
        {
            // Edd context menu items for pinned folder
            if (model->getPinnedFolder() == Notebook::instance()->trashFolder() )
            {
                menu.addSeparator();
                menu.addAction(clearTrashAction);

                clearTrashAction->setEnabled( !isEmptyTrash );
            }
        }

    } else if (indexesList.size() == 1) // 1 item
    {
        const QModelIndex index = indexesList.first();
        if ( index.isValid() )
        {
            const BaseModelItem * modelitem = static_cast < BaseModelItem * > ( index.internalPointer() );
            if (modelitem->DataType() == BaseModelItem::folder)
            {
                const FolderModelItem * folderModelItem = dynamic_cast < const FolderModelItem * > ( modelitem );
                const AbstractFolderItem * folderItem = folderModelItem->getStoredData();
                if ( folderItem == Notebook::instance()->trashFolder() )
                {
                    menu.addSeparator();
                    menu.addAction(clearTrashAction);

                    clearTrashAction->setEnabled( !isEmptyTrash );

                } else
                {
                    menu.addAction(addNoteAction);
                    menu.addAction(addFolderAction);
                    menu.addSeparator();
                    menu.addAction(renameItemAction);
                    menu.addAction(itemForeColorMenu->menuAction());
                    menu.addAction(itemBackColorMenu->menuAction());
                    menu.addSeparator();
                    menu.addAction(moveToBinAction);
                    menu.addAction(deleteItemAction);
                }

            } else if ( modelitem->DataType() == BaseModelItem::note )
            {
                menu.addAction(openNoteAction);
                menu.addSeparator();
                menu.addAction(renameItemAction);
                menu.addAction(itemForeColorMenu->menuAction());
                menu.addAction(itemBackColorMenu->menuAction());
                menu.addSeparator();
                menu.addAction(moveToBinAction);
                menu.addAction(deleteItemAction);
            }
        }

    } else // few items
    {
        menu.addAction(moveToBinAction);
        menu.addAction(deleteItemAction);
        menu.addSeparator();
        menu.addAction(itemForeColorMenu->menuAction());
        menu.addAction(itemBackColorMenu->menuAction());
    }

    if ( menu.isEmpty() )
        return;

    const QPoint & globalPos = treeView->viewport()->mapToGlobal(p);
    menu.exec( globalPos );
}
void FolderNavigationWidget::sl_View_clicked (const QModelIndex& index)
{
    if (!index.isValid())
        return;

    BaseModelItem* item = static_cast<BaseModelItem*>(index.internalPointer());
    if (item->DataType() == BaseModelItem::note)
    {
        Note* n = dynamic_cast<NoteModelItem*>(item)->getStoredData();
        if (!n)
        {
            WARNING("Casting error");
            return;
        }
        emit sg_NoteClicked(n);
    }
}
void FolderNavigationWidget::sl_View_doubleClicked (const QModelIndex& index)
{
    if (!index.isValid())
        return;

    BaseModelItem* item = static_cast<BaseModelItem*>(index.internalPointer());
    if (item->DataType() == BaseModelItem::note)
    {
        Note* n = dynamic_cast<NoteModelItem*>(item)->getStoredData();
        if (!n) {
            WARNING("Casting error");
            return;
        }
        emit sg_NoteDoubleClicked(n);
    }
}
void FolderNavigationWidget::sl_View_SelectionChanged(const QItemSelection&, const QItemSelection&)
{
    sl_UpdateStates();

    emit sg_SelectedItemsActionsListChanged();
}
void FolderNavigationWidget::sl_View_Expanded(const QModelIndex& index)
{
    if (!index.isValid())
        return;
    if (index.internalPointer() == 0)
        return;

    BaseModelItem* modelItemToEdit = static_cast<BaseModelItem*>(index.internalPointer());

    if (modelItemToEdit->DataType() == BaseModelItem::folder)
    {
        Folder* folder = (dynamic_cast<FolderModelItem*>(modelItemToEdit))->getStoredData();
        folder->setExpanded(true);
    }
}
void FolderNavigationWidget::sl_View_Collapsed(const QModelIndex& index)
{
    if (!index.isValid())
        return;
    if (index.internalPointer() == 0)
        return;

    BaseModelItem* modelItemToEdit = static_cast<BaseModelItem*>(index.internalPointer());

    if (modelItemToEdit->DataType() == BaseModelItem::folder)
    {
        Folder* folder = (dynamic_cast<FolderModelItem*>(modelItemToEdit))->getStoredData();
        folder->setExpanded(false);
    }
}

void FolderNavigationWidget::sl_PinFolderButton_Toggled(bool toggle)
{
    if (!model)
    {
        currentRootLabel->setText("");
        currentRootLabel->setToolTip("");

        pinFolderButton->blockSignals(true);
        pinFolderButton->setChecked(false);
        pinFolderButton->blockSignals(false);
        return;
    }

    Folder* folderToPin = 0;
    if (toggle)
    {
        QModelIndex index = treeView->currentIndex();
        if (!index.isValid())
        {
            WARNING("Invalid index");
            return;
        }

        BaseModelItem* modelItem = static_cast<BaseModelItem*>(index.internalPointer());
        if (modelItem->DataType() != BaseModelItem::folder)
        {
            pinFolderButton->blockSignals(true);
            pinFolderButton->setChecked(false);
            pinFolderButton->blockSignals(false);
            return;
        }

        folderToPin = dynamic_cast<FolderModelItem*>(modelItem)->getStoredData();
    }

    Notebook::instance()->setPinnedFolder( folderToPin );
    updatePinnedFolderData();
    sl_UpdateStates();
}

void FolderNavigationWidget::sl_Model_ApplySelection(const QModelIndexList& list)
{
    treeView->selectionModel()->clearSelection();

    if ( list.isEmpty() )
        return;

    treeView->selectionModel()->setCurrentIndex(list.at(0), QItemSelectionModel::NoUpdate);

    foreach(QModelIndex index, list)
    {
        if (index.parent().isValid())
            treeView->expand(index.parent());
        treeView->selectionModel()->select(index, QItemSelectionModel::Select);
    }
}
void FolderNavigationWidget::sl_Model_DisplayRootItemChanged()
{
    restoreExpandedIndexes();
    emit sg_SelectedItemsActionsListChanged();
}

void FolderNavigationWidget::sl_UpdateStates()
{
    if ( !model )
    {
        pinFolderButton->setEnabled( false );
        return;
    }

    if ( model->getPinnedFolder() )
        pinFolderButton->setEnabled( true );
    else
        pinFolderButton->setEnabled( hasCurrentItem() && getCurrentFolder() );
}

void FolderNavigationWidget::sl_NoteEventChange( RichTextNote * richTextNote, int event )
{
    if ( !richTextNote )
    {
        WARNING("null pointer!");
        return;
    }

    switch (event)
    {
    case EventsNote::Remove:
        WARNING( "Delete pointer RichTextNote and dir" );
//        // Удаление из модели
//        Note * note = Notebook::instance()->getNoteFromRichTextNote( richTextNote );
//        if ( !note )
//        {
//            WARNING("null pointer!");
//            return;
//        }

//        Folder * parent = note->getParent();
//        if ( !parent )
//        {
//            WARNING("null pointer!");
//            return;
//        }

//        parent->child.Remove( note );
//        delete note;

        break;
    }
}

void FolderNavigationWidget::sl_ExpandAll()
{
    treeView->expandAll();
}

bool FolderNavigationWidget::sl_AddNote( RichTextNote * richTextNote )
{
    if ( !richTextNote )
    {
        WARNING("null pointer!");
        return false;
    }

    Folder * root = Notebook::instance()->rootFolder();

    // Если нет выделенного элемента, добавляем в корень
    if ( !hasCurrentItem() && !model->getPinnedFolder() )
    {
        Note * note = new Note();
        note->setRichTextNote( richTextNote );
        root->child.Add( note );

        return true;
    }

    QModelIndexList indexesList = treeView->selectionModel()->selectedIndexes();
    if (indexesList.size() > 1)
    {
        WARNING("Wrong item selection");
        return false;
    }

    Folder* parentFolder = 0;

    if (indexesList.size() == 0)
        parentFolder = ( !model->getPinnedFolder() ? root : model->getPinnedFolder() );
    else
    {
        BaseModelItem* modelitem = static_cast<BaseModelItem*>(indexesList.value(0).internalPointer());
        if (modelitem->DataType() != BaseModelItem::folder)
        {
            WARNING("Parent item is not a folder");
            return false;
        }
        parentFolder = dynamic_cast<FolderModelItem*>(modelitem)->getStoredData();

        if (parentFolder == Notebook::instance()->trashFolder())
        {
            WARNING("Cannot create notes in bin");
            return false;
        }
    }

    Note * note = new Note();
    note->setRichTextNote( richTextNote );
    parentFolder->child.Add( note );

    return true;
}
void FolderNavigationWidget::sl_AddNoteAction_Triggered()
{
    RichTextNote * richTextNote = new RichTextNote();
    bool successful = sl_AddNote( richTextNote );
    if ( !successful )
    {
        CRITICAL( "Error when add new note" );
        delete richTextNote;
        return;
    }
    richTextNote->createNew();
}
void FolderNavigationWidget::sl_AddFolderAction_Triggered()
{   
    // Если нет выделенного элемента, добавляем в корень
    if ( !hasCurrentItem() && !model->getPinnedFolder() )
    {
        Notebook::instance()->rootFolder()->child.Add( new Folder() );
        return;
    }

	QModelIndexList indexesList = treeView->selectionModel()->selectedIndexes();
    if (indexesList.size() > 1)
    {
		WARNING("Wrong item selection");
		return;
	}

    Folder * parentFolder = 0;

    if (indexesList.size() == 0)
        parentFolder = (model->getPinnedFolder() == 0 ? Notebook::instance()->rootFolder() : model->getPinnedFolder());

    else
    {
        BaseModelItem* modelitem = static_cast<BaseModelItem*>(indexesList.value(0).internalPointer());

        if (modelitem->DataType() != BaseModelItem::folder)
        {
			WARNING("Parent item is not a folder");
			return;
		}
        parentFolder = dynamic_cast<FolderModelItem*>(modelitem)->getStoredData();
        if (parentFolder == Notebook::instance()->rootFolder())
        {
			WARNING("Cannot create new items in system folders");
			return;
		}
	}

    if ( !parentFolder )
    {
        WARNING("Pointer 'parentFolder' is null!");
        return;
    }
    parentFolder->child.Add( new Folder() );
}
void FolderNavigationWidget::sl_MoveToBinAction_Triggered()
{
    QModelIndexList list = treeView->selectionModel()->selectedIndexes(); // TODO использовать ссылку
    deleteItems(list, false); // Пусть пока перемещает в корзину
}
void FolderNavigationWidget::sl_DeleteItemAction_Triggered()
{
    QModelIndexList list = treeView->selectionModel()->selectedIndexes(); // TODO использовать ссылку
    deleteItems(list, true); // Удаление
}

void FolderNavigationWidget::sl_DefaultForeColor_Triggered()
{
	QModelIndexList indexesList = treeView->selectionModel()->selectedIndexes();
    if (indexesList.size() == 0)
    {
		WARNING("Wrong selection for this operation");
		return;
	}

    for (int i = 0; i < indexesList.size(); i++)
    {
        if (!indexesList.value(i).isValid())
        {
			WARNING("Invalid index");
			continue;
		}

        BaseModelItem* modelItemToEdit = static_cast<BaseModelItem*>(indexesList.value(i).internalPointer());

        if (modelItemToEdit->DataType() == BaseModelItem::folder)
        {
            Folder* f = (dynamic_cast<FolderModelItem*>(modelItemToEdit))->getStoredData();
            if (!f)
            {
				WARNING("Casting error");
				continue;
			}
            if (f->getType() != Folder::UserFolder)
            {
				WARNING("Cannot perform action on system folder");
				return;
			}
            f->setNameForeColor(f->getDefaultForeColor());

        } else if (modelItemToEdit->DataType() == BaseModelItem::note)
        {
            Note* n = (dynamic_cast<NoteModelItem*>(modelItemToEdit))->getStoredData();
            if (!n)
            {
				WARNING("Casting error");
				continue;
			}
            n->setNameForeColor(n->getDefaultForeColor());
		}
	}
}
void FolderNavigationWidget::sl_DefaultBackColor_Triggered()
{
	QModelIndexList indexesList = treeView->selectionModel()->selectedIndexes();
    if (indexesList.size() == 0)
    {
		WARNING("Wrong selection for this operation");
		return;
	}

    for (int i = 0; i < indexesList.size(); i++)
    {
        if (!indexesList.value(i).isValid())
        {
			WARNING("Invalid index");
			continue;
		}
        BaseModelItem* modelItemToEdit = static_cast<BaseModelItem*>(indexesList.value(i).internalPointer());

        if (modelItemToEdit->DataType() == BaseModelItem::folder)
        {
            Folder* f = (dynamic_cast<FolderModelItem*>(modelItemToEdit))->getStoredData();
            if (!f)
            {
				WARNING("Casting error");
				continue;
			}
            if (f->getType() != Folder::UserFolder)
            {
				WARNING("Cannot perform action on system folder");
				return;
			}
            f->setNameBackColor(f->getDefaultBackColor());

        } else if (modelItemToEdit->DataType() == BaseModelItem::note)
        {
            Note* n = (dynamic_cast<NoteModelItem*>(modelItemToEdit))->getStoredData();
            if (!n)
            {
				WARNING("Casting error");
				continue;
            }
            n->setNameBackColor(n->getDefaultBackColor());
		}
	}
}
void FolderNavigationWidget::sl_CustomForeColor_Triggered()
{
	QModelIndexList indexesList = treeView->selectionModel()->selectedIndexes();
    if (indexesList.size() == 0)
    {
		WARNING("Wrong selection for this operation");
		return;
	}

	QColor oldColor = QColor();
    if (indexesList.size() == 1)
    {
        BaseModelItem* modelItemToEdit = static_cast<BaseModelItem*>(indexesList.value(0).internalPointer());

        if (modelItemToEdit->DataType() == BaseModelItem::folder)
        {
            Folder* f = (dynamic_cast<FolderModelItem*>(modelItemToEdit))->getStoredData();
            if (f)
                oldColor = f->getNameForeColor();

        } else if (modelItemToEdit->DataType() == BaseModelItem::note)
        {
            Note* n = (dynamic_cast<NoteModelItem*>(modelItemToEdit))->getStoredData();
            if (n)
                oldColor = n->getNameForeColor();
		}
	}

	QColor newColor = QColorDialog::getColor(oldColor);
    if (!newColor.isValid())
        return;

    for (int i = 0; i < indexesList.size(); i++)
    {
        if (!indexesList.value(i).isValid())
        {
			WARNING("Invalid index");
			continue;
		}
        BaseModelItem* modelItemToEdit = static_cast<BaseModelItem*>(indexesList.value(i).internalPointer());

        if (modelItemToEdit->DataType() == BaseModelItem::folder)
        {
            Folder* f = (dynamic_cast<FolderModelItem*>(modelItemToEdit))->getStoredData();
            if (!f)
            {
				WARNING("Casting error");
				continue;
			}
            if (f->getType() != Folder::UserFolder)
            {
				WARNING("Cannot perform action on system folder");
				return;
			}
            f->setNameForeColor(newColor);

        } else if (modelItemToEdit->DataType() == BaseModelItem::note)
        {
            Note* n = (dynamic_cast<NoteModelItem*>(modelItemToEdit))->getStoredData();
            if (!n)
            {
				WARNING("Casting error");
				continue;
			}
            n->setNameForeColor(newColor);
		}
	}
}
void FolderNavigationWidget::sl_CustomBackColor_Triggered()
{
	QModelIndexList indexesList = treeView->selectionModel()->selectedIndexes();
    if (indexesList.size() == 0)
    {
		WARNING("Wrong selection for this operation");
		return;
	}

	QColor oldColor = QColor();
    if (indexesList.size() == 1)
    {
        BaseModelItem* modelItemToEdit = static_cast<BaseModelItem*>(indexesList.value(0).internalPointer());

        if (modelItemToEdit->DataType() == BaseModelItem::folder)
        {
            Folder* f = (dynamic_cast<FolderModelItem*>(modelItemToEdit))->getStoredData();
            if (f)
                oldColor = f->getNameBackColor();

        } else if (modelItemToEdit->DataType() == BaseModelItem::note)
        {
            Note* n = (dynamic_cast<NoteModelItem*>(modelItemToEdit))->getStoredData();
            if (n)
                oldColor = n->getNameBackColor();
		}
	}

	QColor newColor = QColorDialog::getColor(oldColor);
    if (!newColor.isValid())
        return;

    for (int i = 0; i < indexesList.size(); i++)
    {
        if (!indexesList.value(i).isValid())
        {
			WARNING("Invalid index");
			continue;
		}
        BaseModelItem* modelItemToEdit = static_cast<BaseModelItem*>(indexesList.value(i).internalPointer());

        if (modelItemToEdit->DataType() == BaseModelItem::folder)
        {
            Folder* f = (dynamic_cast<FolderModelItem*>(modelItemToEdit))->getStoredData();
            if (!f)
            {
				WARNING("Casting error");
				continue;
			}
            if (f->getType() != Folder::UserFolder)
            {
				WARNING("Cannot perform action on system folder");
				return;
			}
            f->setNameBackColor(newColor);

        } else if (modelItemToEdit->DataType() == BaseModelItem::note)
        {
            Note* n = (dynamic_cast<NoteModelItem*>(modelItemToEdit))->getStoredData();
            if (!n)
            {
				WARNING("Casting error");
				continue;
			}
            n->setNameBackColor(newColor);
		}
	}
}
void FolderNavigationWidget::sl_ClearTrashAction_Triggered()
{
    Folder * f = Notebook::instance()->trashFolder();
    while (f->child.Count() > 0)
    {
        AbstractFolderItem * item = f->child.ItemAt(0);
        f->child.Remove(item);
		delete item;
	}
}
void FolderNavigationWidget::sl_OpenNoteAction_Triggered()
{
	QModelIndexList indexesList = treeView->selectionModel()->selectedIndexes();

    if (indexesList.size() == 0)
    {
		WARNING("Empty list");
		return;
	}

    foreach (const QModelIndex index, indexesList)
		sl_View_doubleClicked(index);
}

void FolderNavigationWidget::sl_RenameItemAction_Triggered()
{
    if (!hasCurrentItem())
    {
		WARNING("Invalid index");
		return;
	}
    treeView->edit( treeView->currentIndex() );
}

bool FolderNavigationWidget::eventFilter (QObject* watched, QEvent* event)
{
    if ( watched == treeView )
    {
        if (event->type() != QEvent::KeyPress)
            return false;

        ModelItemDelegate* delegate = dynamic_cast<ModelItemDelegate*>(treeView->itemDelegate());
        if (delegate != 0 && delegate->isEditing())
            return false;

        QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
        if (keyEvent->key() == Qt::Key_Delete)
        {
            // Delete selected items by pressing 'Delete' button
            if (treeView->selectionModel()->selectedIndexes().size() == 0)
                return false;
            bool permanently = false;
            if ((keyEvent->modifiers() & Qt::ShiftModifier) || false)
                //if ((keyEvent->modifiers() & Qt::ShiftModifier) || !Application::I()->settings.getMoveItemsToBin())
                permanently = true;

            QModelIndexList list = treeView->selectionModel()->selectedIndexes();
            deleteItems(list, permanently);

        } else if (keyEvent->key() == Qt::Key_Enter || keyEvent->key() == Qt::Key_Return)
        {
            // Open selected notes by pressing 'Enter' button
            if (treeView->selectionModel()->selectedIndexes().size() == 0)
                return false;

            QModelIndexList list = treeView->selectionModel()->selectedIndexes();
            foreach (QModelIndex index, list)
            {
                if (!index.isValid()) continue;

                BaseModelItem* item = static_cast<BaseModelItem*>(index.internalPointer());
                if (item->DataType() == BaseModelItem::note)
                {
                    Note* note = dynamic_cast<NoteModelItem*>(item)->getStoredData();
                    if (!note)
                    {
                        WARNING("Casting error");
                        continue;
                    }
                    emit sg_NoteDoubleClicked(note);
                }
            }
        }

    } else if ( watched == treeView->viewport() )
    {
        // Если кликнули на пустую область иерархического дерева, тогда убираем выделение
        if ( event->type() == QEvent::MouseButtonRelease )
        {
            QMouseEvent * mouseEvent = static_cast < QMouseEvent * > ( event );
            const QModelIndex & index = treeView->indexAt( mouseEvent->pos() );
            if ( !index.isValid() )
                treeView->setCurrentIndex( QModelIndex() );
        }

        return false;
    }

    return false;
}
