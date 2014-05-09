#ifndef FOLDERNAVIGATIONWIDGET_H
#define FOLDERNAVIGATIONWIDGET_H

#include <QMainWindow>

namespace Ui
{
    class FolderNavigationWidget;
}

#include "Note/RichTextNote.h"

class HierarchyModel;
class Note;
class Folder;
class BaseModelItem;

class FolderNavigationWidget : public QMainWindow
{
    Q_OBJECT

public:
    explicit FolderNavigationWidget( QWidget * parent = 0 );
    ~FolderNavigationWidget();

    void setModel( HierarchyModel * );
    void updatePinnedFolderData();

    bool hasCurrentItem();
    BaseModelItem * getItem( const QModelIndex & );
    BaseModelItem * getCurrentItem();
    Note * getNote( const QModelIndex & );
    Folder * getFolder( const QModelIndex & );
    Note * getCurrentNote();
    Folder * getCurrentFolder();

private:
    Ui::FolderNavigationWidget * ui;
    HierarchyModel * model;

    QMenu * menuItemForeColor;
    QMenu * menuItemBackColor;

public:
    QAction * actionAddNote;
    QAction * actionAddFolder;
    QAction * actionMoveToBin;
    QAction * actionDeleteItem;
    QAction * actionClearTrash;
    QAction * actionItemDefaultForeColor;
    QAction * actionItemCustomForeColor;
    QAction * actionItemDefaultBackColor;
    QAction * actionItemCustomBackColor;
    QAction * actionOpenNote;
    QAction * actionRenameItem;

private:
    void deleteItems(QModelIndexList& list, bool permanently = false);
    void deleteChildIndexes(QModelIndexList& list) const; // Delete indexes, which parents also in the list
    void restoreExpandedIndexes();

public slots:
    void sl_ExpandAll();
    void sl_CollapseAll();

    bool sl_AddNote( RichTextNote * richTextNote );

    // Actions event handlers
    void sl_AddNoteAction_Triggered();
    void sl_AddFolderAction_Triggered();
    void sl_MoveToBinAction_Triggered();
    void sl_DeleteItemAction_Triggered();
    void sl_DefaultForeColor_Triggered();
    void sl_DefaultBackColor_Triggered();
    void sl_CustomForeColor_Triggered();
    void sl_CustomBackColor_Triggered();
    void sl_ClearTrashAction_Triggered();
    void sl_OpenNoteAction_Triggered();
    void sl_RenameItemAction_Triggered();

private slots:
    void sl_UpdateStates();

    void sl_Model_ApplySelection(const QModelIndexList& list);
    void sl_Model_DisplayRootItemChanged();

    void on_tButtonPinFolder_clicked( bool checked );
    void on_tButtonExpandAll_clicked();
    void on_tButtonCollapseAll_clicked();

    void on_treeView_customContextMenuRequested(const QPoint &pos);
    void on_treeView_clicked(const QModelIndex &index);
    void on_treeView_doubleClicked(const QModelIndex &index);
    void on_treeView_expanded(const QModelIndex &index);
    void on_treeView_collapsed(const QModelIndex &index);
    void sl_treeView_SelectionChanged(const QItemSelection&, const QItemSelection&);

protected:
    bool eventFilter (QObject* watched, QEvent* event);

signals:
    void sg_NoteClicked(Note*);
    void sg_NoteDoubleClicked(Note*);
    void sg_SelectedItemsActionsListChanged();
};

#endif // FOLDERNAVIGATIONWIDGET_H
