#include "foldernavigationwidget.h"
#include "ui_foldernavigationwidget.h"

#include "modelitemdelegate.h"
#include "hierarchymodel.h"
#include "folder.h"
#include "notebook.h"
#include "basemodelitem.h"
#include "notemodelitem.h"
#include "foldermodelitem.h"
#include <QStack>
#include <QColorDialog>
#include <QListView>

FolderNavigationWidget::FolderNavigationWidget( QWidget * parent )
    : QMainWindow( parent ),
      ui( new Ui::FolderNavigationWidget ),
      model(0)
{
    ui->setupUi( this );
    ui->treeView->setItemDelegate( new ModelItemDelegate() );
    ui->treeView->installEventFilter(this);
    ui->treeView->viewport()->installEventFilter(this);

    // Item actions
    {
        actionRenameItem = new QAction( QIcon( ":/fugue-icons/document-rename" ), tr( "Rename" ), this );
        QObject::connect(actionRenameItem, SIGNAL(triggered()), SLOT(sl_RenameItemAction_Triggered()));

        actionOpenNote = new QAction( QIcon( ":/fugue-icons/book-open" ), tr( "Open" ), this );
        QObject::connect(actionOpenNote, SIGNAL(triggered()), SLOT(sl_OpenNoteAction_Triggered()));


        menuItemForeColor = new QMenu( tr( "Set Text Color" ), this );
        menuItemForeColor->setIcon( QIcon( ":/fugue-icons/edit-color" ) );
        menuItemBackColor = new QMenu( tr( "Set Back Color" ), this );
        menuItemBackColor->setIcon( QIcon( ":/fugue-icons/paint-can-color" ) );

        actionItemDefaultForeColor = new QAction( QIcon( ":/fugue-icons/ui-color-picker-default" ), tr( "Default Color" ), this );
        QObject::connect( actionItemDefaultForeColor, SIGNAL(triggered()), SLOT(sl_DefaultForeColor_Triggered()) );

        actionItemCustomForeColor = new QAction( QIcon( ":/fugue-icons/color-swatch" ), tr( "Custom Color" ), this );
        QObject::connect( actionItemCustomForeColor, SIGNAL(triggered()), SLOT(sl_CustomForeColor_Triggered()) );

        actionItemDefaultBackColor = new QAction( QIcon( ":/fugue-icons/ui-color-picker-default" ), tr( "Default Color" ), this );
        QObject::connect( actionItemDefaultBackColor, SIGNAL(triggered()), SLOT(sl_DefaultBackColor_Triggered()) );

        actionItemCustomBackColor = new QAction( QIcon( ":/fugue-icons/color-swatch" ), tr( "Custom Color" ), this );
        QObject::connect( actionItemCustomBackColor, SIGNAL(triggered()), SLOT(sl_CustomBackColor_Triggered()) );

        menuItemForeColor->addAction( actionItemDefaultForeColor );
        menuItemForeColor->addAction( actionItemCustomForeColor );
        menuItemBackColor->addAction( actionItemDefaultBackColor );
        menuItemBackColor->addAction( actionItemCustomBackColor );
    }

    QObject::connect( Notebook::instance(), SIGNAL(sg_ItemRegistered(Note*)), SLOT(sl_UpdateStates()) );
    QObject::connect( Notebook::instance(), SIGNAL(sg_ItemUnregistered(Note*)), SLOT(sl_UpdateStates()) );
    sl_UpdateStates();
}
FolderNavigationWidget::~FolderNavigationWidget()
{
    delete ui;
}

void FolderNavigationWidget::setModel(HierarchyModel* m)
{
    if (model)
        QObject::disconnect(model, 0, this, 0);

    model = m;

    if (model)
    {
        QObject::connect( model, SIGNAL(sg_ApplySelection(QModelIndexList)), SLOT(sl_Model_ApplySelection(QModelIndexList)) );
        QObject::connect( model, SIGNAL(sg_DisplayRootItemChanged()), SLOT(sl_Model_DisplayRootItemChanged()) );
    }

    ui->treeView->setModel(model);
    if ( ui->treeView->model() )
    {
        for (int i = 0; i < ui->treeView->model()->columnCount(); i++)
            ui->treeView->resizeColumnToContents(i);
        QObject::connect(ui->treeView->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)), SLOT(sl_treeView_SelectionChanged(QItemSelection,QItemSelection)));
        restoreExpandedIndexes();
    }

    ui->treeView->setEnabled( model );
    sl_UpdateStates();

    if (model)
    {
        ui->tButtonPinFolder->setChecked( model->getPinnedFolder() );
        updatePinnedFolderData();
    } else
    {
        ui->tButtonPinFolder->setChecked(false);
        updatePinnedFolderData();
    }
}
void FolderNavigationWidget::updatePinnedFolderData()
{
    if (model && model->getPinnedFolder())
    {
        QString path = model->getPinnedFolder()->getPath();
        ui->labelCurrentPinFolder->setText(path);
        ui->labelCurrentPinFolder->setToolTip(path);
    } else
    {
        ui->labelCurrentPinFolder->setText("");
        ui->labelCurrentPinFolder->setToolTip("");
    }
}

bool FolderNavigationWidget::hasCurrentItem()
{
    return ui->treeView->currentIndex().isValid();
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
    const QModelIndex & index = ui->treeView->currentIndex();
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
    const QModelIndex & index = ui->treeView->currentIndex();
    return getNote( index );
}
Folder * FolderNavigationWidget::getCurrentFolder()
{
    const QModelIndex & index = ui->treeView->currentIndex();
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

    const QString & title = permanently ? tr( "Confirm deletion" ) : tr( "Confirm moving" );
    const QString & message = permanently ? tr( "Delete these items?" ) : tr( "Put these items to Bin?" );
    foreach ( const QModelIndex & index, indexesList )
        details.append( index.model()->data( index, Qt::DisplayRole ).toString() + "\n" );

    QMessageBox * messageBox = new QMessageBox( QMessageBox::Question, title, message, QMessageBox::Yes | QMessageBox::No, this );
    messageBox->setDetailedText( details );
    messageBox->resize( messageBox->minimumSizeHint() );
    bool hasCancel = messageBox->exec() != QMessageBox::Yes;
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

        BaseModelItem * modelItemToDelete = static_cast < BaseModelItem * > ( index.internalPointer() );

        bool isFolder = modelItemToDelete->DataType() == BaseModelItem::folder;
        bool isNote = modelItemToDelete->DataType() == BaseModelItem::note;

        AbstractFolderItem* itemToDelete = 0;
        if ( isFolder )
        {
            FolderModelItem* fmi = dynamic_cast<FolderModelItem*>(modelItemToDelete);
            itemToDelete = fmi->getStoredData();

            if ( itemToDelete == Notebook::instance()->getTrashFolder() )
            {
                QMessageBox::information(0, tr( "Information" ), tr( "You cannot delete system folders" ) );
                continue;
            }
        } else if ( isNote )
        {
            NoteModelItem * fmi = dynamic_cast<NoteModelItem*>(modelItemToDelete);
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
            parentFolder->child.Remove(itemToDelete);
            delete itemToDelete;

        } else
            parentFolder->child.Move(itemToDelete, Notebook::instance()->getTrashFolder());
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
    QStack < QModelIndex > indexes;
    indexes.push( ui->treeView->rootIndex() );

    while (!indexes.isEmpty())
    {
        QModelIndex index = indexes.pop();
        for ( int i = 0; i < model->rowCount( index ); i++ )
        {
            QModelIndex child = model->index(i, 0, index);
            if (child.isValid())
                indexes.push(child);
        }

        if ( !index.isValid() )
            continue;
        if ( !index.internalPointer() )
            continue;
        BaseModelItem * modelItemToEdit = static_cast < BaseModelItem * > ( index.internalPointer() );

        if ( modelItemToEdit->DataType() == BaseModelItem::folder )
        {
            Folder * folder = dynamic_cast < FolderModelItem * > ( modelItemToEdit )->getStoredData();
            if ( folder->isExpanded() )
                ui->treeView->expand( index );
        }
    }
}

void FolderNavigationWidget::sl_ExpandAll()
{
    ui->treeView->expandAll();
}
void FolderNavigationWidget::sl_CollapseAll()
{
    ui->treeView->collapseAll();
}

bool FolderNavigationWidget::sl_AddNote( RichTextNote * richTextNote )
{
    if ( !richTextNote )
    {
        WARNING("null pointer!");
        return false;
    }

    Folder * root = Notebook::instance()->getRootFolder();

    // Если нет выделенного элемента, добавляем в корень
    if ( !hasCurrentItem() && !model->getPinnedFolder() )
    {
        Note * note = new Note();
        note->setRichTextNote( richTextNote );
        root->child.Add( note );

        return true;
    }

    QModelIndexList indexesList = ui->treeView->selectionModel()->selectedIndexes();
    if ( indexesList.size() > 1 )
    {
        WARNING("Wrong item selection");
        return false;
    }

    Folder * parentFolder = 0;

    if ( !indexesList.size() )
        parentFolder = ( !model->getPinnedFolder() ? root : model->getPinnedFolder() );
    else
    {
        BaseModelItem * modelitem = static_cast < BaseModelItem * > ( indexesList.value(0).internalPointer() );

        // Если текущий элементом является заметка, то добавим новую заметку ниже ее
        if ( modelitem->DataType() == BaseModelItem::note )
        {
            NoteModelItem * currentNoteItem = dynamic_cast < NoteModelItem * > ( modelitem );
            Note * currentNote = currentNoteItem->getStoredData();
            int index = currentNote->getParent()->child.IndexOf( currentNote );

            Note * note = new Note();
            note->setRichTextNote( richTextNote );
            currentNote->getParent()->child.AddTo( note, index + 1 );

            return true;
        }

        parentFolder = dynamic_cast < FolderModelItem * > ( modelitem )->getStoredData();

        // Если текущим элементом является корзина, то добавляем в корень
        if ( parentFolder == Notebook::instance()->getTrashFolder() )
            parentFolder = root;
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
    Folder * root = Notebook::instance()->getRootFolder();

    // Если нет выделенного элемента, добавляем в корень
    if ( !hasCurrentItem() && !model->getPinnedFolder() )
    {
        root->child.Add( new Folder() );
        return;
    }

    QModelIndexList indexesList = ui->treeView->selectionModel()->selectedIndexes();
    if ( indexesList.size() > 1 )
    {
        WARNING("Wrong item selection");
        return;
    }

    Folder * parentFolder = 0;

    if ( indexesList.size() == 0 )
        parentFolder = ( !model->getPinnedFolder() ? root : model->getPinnedFolder() );

    else
    {
        BaseModelItem * modelitem = static_cast < BaseModelItem * > ( indexesList.value(0).internalPointer() );

        // Если текущий элементом является заметка, то добавим новую папку ниже ее
        if ( modelitem->DataType() == BaseModelItem::note )
        {
            NoteModelItem * currentNoteItem = dynamic_cast < NoteModelItem * > ( modelitem );
            Note * currentNote = currentNoteItem->getStoredData();
            int index = currentNote->getParent()->child.IndexOf( currentNote );
            currentNote->getParent()->child.AddTo( new Folder(), index + 1 );

            return;
        }

        parentFolder = dynamic_cast<FolderModelItem*>(modelitem)->getStoredData();
        if ( parentFolder == Notebook::instance()->getTrashFolder() )
            parentFolder = root;
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
    QModelIndexList list = ui->treeView->selectionModel()->selectedIndexes(); // TODO использовать ссылку
    deleteItems(list, false); // Пусть пока перемещает в корзину
}
void FolderNavigationWidget::sl_DeleteItemAction_Triggered()
{
    QModelIndexList list = ui->treeView->selectionModel()->selectedIndexes(); // TODO использовать ссылку
    deleteItems(list, true); // Удаление
}

void FolderNavigationWidget::sl_DefaultForeColor_Triggered()
{
    QModelIndexList indexesList = ui->treeView->selectionModel()->selectedIndexes();
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
    QModelIndexList indexesList = ui->treeView->selectionModel()->selectedIndexes();
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
    QModelIndexList indexesList = ui->treeView->selectionModel()->selectedIndexes();
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
    QModelIndexList indexesList = ui->treeView->selectionModel()->selectedIndexes();
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
    const QString & title = tr( "Confirm deletion" );
    const QString & message = tr( "Delete all items?" );
    bool hasCancel = QMessageBox::question( this, title, message, QMessageBox::Yes | QMessageBox::No ) != QMessageBox::Yes;
    if ( hasCancel )
        return;

    Folder * f = Notebook::instance()->getTrashFolder();
    while (f->child.Count() > 0)
    {
        AbstractFolderItem * item = f->child.ItemAt(0);
        f->child.Remove(item);
        delete item;
    }
}
void FolderNavigationWidget::sl_OpenNoteAction_Triggered()
{
    QModelIndexList indexesList = ui->treeView->selectionModel()->selectedIndexes();

    if (indexesList.size() == 0)
    {
        WARNING("Empty list");
        return;
    }

    foreach (const QModelIndex index, indexesList)
        on_treeView_doubleClicked(index);
}

void FolderNavigationWidget::sl_RenameItemAction_Triggered()
{
    if (!hasCurrentItem())
    {
        WARNING("Invalid index");
        return;
    }
    ui->treeView->edit( ui->treeView->currentIndex() );
}

void FolderNavigationWidget::sl_UpdateStates()
{
    if ( !model )
    {
        ui->tButtonPinFolder->setEnabled( false );
        return;
    }

    if ( model->getPinnedFolder() )
        ui->tButtonPinFolder->setEnabled( true );
    else
        ui->tButtonPinFolder->setEnabled( hasCurrentItem() && getCurrentFolder() );


    bool hasEmpty = ( Notebook::instance()->getTrashFolder()->child.Count() == 0 ) && ( Notebook::instance()->getRootFolder()->child.Count() == 0 );
    ui->tButtonExpandAll->setEnabled( !hasEmpty );
    ui->tButtonCollapseAll->setEnabled( !hasEmpty );
}

void FolderNavigationWidget::sl_Model_ApplySelection(const QModelIndexList& list)
{
    QItemSelectionModel * selectionModel = ui->treeView->selectionModel();
    selectionModel->clearSelection();

    if ( list.isEmpty() )
        return;

    selectionModel->setCurrentIndex(list.at(0), QItemSelectionModel::NoUpdate);

    foreach(QModelIndex index, list)
    {
        if (index.parent().isValid())
            ui->treeView->expand(index.parent());
        selectionModel->select(index, QItemSelectionModel::Select);
    }
}
void FolderNavigationWidget::sl_Model_DisplayRootItemChanged()
{
    restoreExpandedIndexes();
    emit sg_SelectedItemsActionsListChanged();
}


void FolderNavigationWidget::on_tButtonPinFolder_clicked( bool checked )
{
    if (!model)
    {
        ui->labelCurrentPinFolder->setText("");
        ui->labelCurrentPinFolder->setToolTip("");

        ui->tButtonPinFolder->setChecked(false);
        return;
    }

    Folder * folderToPin = 0;
    if ( checked )
    {
        QModelIndex index = ui->treeView->currentIndex();
        if ( !index.isValid() )
        {
            WARNING("Invalid index");
            return;
        }

        BaseModelItem * modelItem = static_cast < BaseModelItem * > ( index.internalPointer() );
        if ( modelItem->DataType() != BaseModelItem::folder )
        {
            ui->tButtonPinFolder->setChecked( false );
            return;
        }

        folderToPin = dynamic_cast < FolderModelItem * > ( modelItem )->getStoredData();
    }

    Notebook::instance()->setPinnedFolder( folderToPin );
    updatePinnedFolderData();
    sl_UpdateStates();
}
void FolderNavigationWidget::on_tButtonExpandAll_clicked()
{
    sl_ExpandAll();
}
void FolderNavigationWidget::on_tButtonCollapseAll_clicked()
{
    sl_CollapseAll();
}

void FolderNavigationWidget::on_treeView_customContextMenuRequested(const QPoint &pos)
{
    const QItemSelectionModel * selectionModel = ui->treeView->selectionModel();
    if ( !selectionModel )
        return;

    QMenu menu;
    const QModelIndexList & indexesList = selectionModel->selectedIndexes();

    menu.addAction( actionAddNote );
    menu.addAction( actionAddFolder );

    // 0 items
    if ( indexesList.isEmpty() )
    {
        if ( model->getPinnedFolder() )
        {
            // Edd context menu items for pinned folder
            if (model->getPinnedFolder() == Notebook::instance()->getTrashFolder() )
            {
                menu.addSeparator();
                menu.addAction( actionClearTrash );
            }
        }

    } else if ( indexesList.size() == 1 ) // 1 item
    {
        const QModelIndex index = indexesList.first();
        if ( index.isValid() )
        {
            const BaseModelItem * modelitem = static_cast < BaseModelItem * > ( index.internalPointer() );
            if (modelitem->DataType() == BaseModelItem::folder)
            {
                const FolderModelItem * folderModelItem = dynamic_cast < const FolderModelItem * > ( modelitem );
                const AbstractFolderItem * folderItem = folderModelItem->getStoredData();
                if ( folderItem == Notebook::instance()->getTrashFolder() )
                {
                    menu.clear();
                    menu.addAction( actionClearTrash );

                } else
                {
                    menu.addSeparator();
                    menu.addAction( actionRenameItem );
                    menu.addAction( menuItemForeColor->menuAction() );
                    menu.addAction( menuItemBackColor->menuAction() );
                    menu.addSeparator();
                    menu.addAction( actionMoveToBin );
                    menu.addAction( actionDeleteItem );
                }

            } else if ( modelitem->DataType() == BaseModelItem::note )
            {
                menu.addSeparator();
                menu.addAction( actionOpenNote );
                menu.addSeparator();
                menu.addAction( actionRenameItem );
                menu.addAction( menuItemForeColor->menuAction() );
                menu.addAction( menuItemBackColor->menuAction() );
                menu.addSeparator();
                menu.addAction( actionMoveToBin );
                menu.addAction( actionDeleteItem );
            }
        }

    } else // few items
    {
        menu.addSeparator();
        menu.addAction( actionOpenNote );
        menu.addSeparator();
        menu.addAction( actionMoveToBin );
        menu.addAction( actionDeleteItem );
        menu.addSeparator();
        menu.addAction( menuItemForeColor->menuAction() );
        menu.addAction( menuItemBackColor->menuAction() );
    }

    if ( menu.isEmpty() )
        return;

    const QPoint & globalPos = ui->treeView->viewport()->mapToGlobal( pos );
    menu.exec( globalPos );
}
void FolderNavigationWidget::on_treeView_clicked(const QModelIndex &index)
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
void FolderNavigationWidget::on_treeView_doubleClicked(const QModelIndex &index)
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
void FolderNavigationWidget::on_treeView_expanded(const QModelIndex &index)
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
void FolderNavigationWidget::on_treeView_collapsed(const QModelIndex &index)
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
void FolderNavigationWidget::sl_treeView_SelectionChanged(const QItemSelection&, const QItemSelection&)
{
    sl_UpdateStates();
    emit sg_SelectedItemsActionsListChanged();
}

bool FolderNavigationWidget::eventFilter (QObject* watched, QEvent* event)
{
    if ( watched == ui->treeView )
    {
        if (event->type() != QEvent::KeyPress)
            return false;

        ModelItemDelegate * delegate = dynamic_cast<ModelItemDelegate*>(ui->treeView->itemDelegate());
        if (delegate && delegate->isEditing())
            return false;

        QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
        if (keyEvent->key() == Qt::Key_Delete)
        {
            // Delete selected items by pressing 'Delete' button
            if (ui->treeView->selectionModel()->selectedIndexes().size() == 0)
                return false;
            bool permanently = false;
            if ((keyEvent->modifiers() & Qt::ShiftModifier) || false)
                //if ((keyEvent->modifiers() & Qt::ShiftModifier) || !Application::I()->settings.getMoveItemsToBin())
                permanently = true;

            QModelIndexList list = ui->treeView->selectionModel()->selectedIndexes();
            deleteItems(list, permanently);

        } else if (keyEvent->key() == Qt::Key_Enter || keyEvent->key() == Qt::Key_Return)
        {
            // Open selected notes by pressing 'Enter' button
            if (ui->treeView->selectionModel()->selectedIndexes().size() == 0)
                return false;

            QModelIndexList list = ui->treeView->selectionModel()->selectedIndexes();
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

    } else if ( watched == ui->treeView->viewport() )
    {
        // Если кликнули на пустую область иерархического дерева, тогда убираем выделение
        if ( event->type() == QEvent::MouseButtonRelease )
        {
            QMouseEvent * mouseEvent = static_cast < QMouseEvent * > ( event );
            const QModelIndex & index = ui->treeView->indexAt( mouseEvent->pos() );
            if ( !index.isValid() )
                ui->treeView->setCurrentIndex( QModelIndex() );
        }

        return false;
    }

    return false;
}
