#ifndef TAGSNAVIGATIONWIDGET_H
#define TAGSNAVIGATIONWIDGET_H

#include <QMainWindow>

namespace Ui {
    class TagsNavigationWidget;
}

class QModelIndex;
class TagsModel;
class Note;

class TagsNavigationWidget : public QMainWindow
{
    Q_OBJECT

public:
    explicit TagsNavigationWidget( QWidget * parent = 0 );
    ~TagsNavigationWidget();

    void setModel( TagsModel * );

private:
    Ui::TagsNavigationWidget * ui;
    TagsModel *	model;

public slots:
    void sl_ExpandAll();
    void sl_CollapseAll();

private slots:
    void sl_UpdateStates();

    void on_tButtonExpandAll_clicked();
    void on_tButtonCollapseAll_clicked();

    void on_treeView_clicked ( const QModelIndex & );
    void on_treeView_doubleClicked ( const QModelIndex & );

signals:
    void sg_NoteClicked( Note * );
    void sg_NoteDoubleClicked( Note * );
};

#endif // TAGSNAVIGATIONWIDGET_H
