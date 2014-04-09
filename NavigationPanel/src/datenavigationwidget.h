#ifndef DATENAVIGATIONWIDGET_H
#define DATENAVIGATIONWIDGET_H

#include <QMainWindow>

namespace Ui
{
    class DateNavigationWidget;
}

class Note;
class DatesModel;
class QModelIndex;

class DateNavigationWidget : public QMainWindow
{
    Q_OBJECT

public:
    explicit DateNavigationWidget( QWidget * parent = 0 );
    ~DateNavigationWidget();

    void setCreationModel( DatesModel * );
    void setModificationModel( DatesModel * );

private:
    Ui::DateNavigationWidget * ui;
    DatesModel * creationDateModel;
    DatesModel * modificationDateModel;

public slots:
    void sl_ExpandAll();
    void sl_CollapseAll();

private slots:
    void sl_UpdateStates();

    void on_tButtonExpandAll_clicked();
    void on_tButtonCollapseAll_clicked();

    void on_treeView_clicked( const QModelIndex & );
    void on_treeView_doubleClicked( const QModelIndex & );

    void on_rButtonCreationDate_clicked( bool checked );
    void on_rButtonModificationDate_clicked( bool checked );

    void sl_updateTreeModel( bool = true );

signals:
    void sg_NoteClicked(Note*);
    void sg_NoteDoubleClicked(Note*);


};

#endif // DATENAVIGATIONWIDGET_H
