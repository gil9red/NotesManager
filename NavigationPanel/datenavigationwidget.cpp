#include "datenavigationwidget.h"
#include "ui_datenavigationwidget.h"

#include "utils/func.h"
#include "basemodelitem.h"
#include "notemodelitem.h"
#include "datesmodel.h"
#include "notebook.h"

DateNavigationWidget::DateNavigationWidget( QWidget * parent )
    : QMainWindow( parent ),
      ui( new Ui::DateNavigationWidget ),
      creationDateModel(0),
      modificationDateModel(0)
{
    ui->setupUi( this );

    QObject::connect( Notebook::instance(), SIGNAL(sg_ItemRegistered(Note*)), SLOT(sl_UpdateStates()) );
    QObject::connect( Notebook::instance(), SIGNAL(sg_ItemUnregistered(Note*)), SLOT(sl_UpdateStates()) );

    sl_updateTreeModel();
    sl_UpdateStates();
}
DateNavigationWidget::~DateNavigationWidget()
{
    delete ui;
}

void DateNavigationWidget::setCreationModel( DatesModel * model )
{
    creationDateModel = model;
    sl_updateTreeModel();
    sl_UpdateStates();
}
void DateNavigationWidget::setModificationModel( DatesModel * model )
{
    modificationDateModel = model;
    sl_updateTreeModel();
    sl_UpdateStates();
}

void DateNavigationWidget::sl_ExpandAll()
{
    ui->treeView->expandAll();
}
void DateNavigationWidget::sl_CollapseAll()
{
    ui->treeView->collapseAll();
}

void DateNavigationWidget::sl_UpdateStates()
{
    bool hasEmpty = ( ui->treeView->model() ? ( ui->treeView->model()->rowCount() == 0 ) : false );
    ui->tButtonExpandAll->setEnabled( !hasEmpty );
    ui->tButtonCollapseAll->setEnabled( !hasEmpty );
}

void DateNavigationWidget::on_tButtonExpandAll_clicked()
{
    sl_ExpandAll();
}
void DateNavigationWidget::on_tButtonCollapseAll_clicked()
{
    sl_CollapseAll();
}

void DateNavigationWidget::on_treeView_clicked(const QModelIndex& index)
{
    if ( !index.isValid() )
        return;

    BaseModelItem * item = static_cast < BaseModelItem * > ( index.internalPointer() );
    if ( item->DataType() == BaseModelItem::note )
    {
        NoteModelItem * noteItem = dynamic_cast < NoteModelItem * > ( item );
        if ( !noteItem )
        {
            WARNING("Casting error");
            return;
        }
        Note * n = noteItem->getStoredData();
        emit sg_NoteClicked(n);
    }
}
void DateNavigationWidget::on_treeView_doubleClicked( const QModelIndex & index )
{
    if ( !index.isValid() )
        return;

    BaseModelItem * item = static_cast < BaseModelItem * > ( index.internalPointer() );
    if ( item->DataType() == BaseModelItem::note )
    {
        NoteModelItem * noteItem = dynamic_cast < NoteModelItem * > ( item );
        if ( !noteItem )
        {
            WARNING("Casting error");
            return;
        }
        Note * n = noteItem->getStoredData();
        emit sg_NoteDoubleClicked(n);
    }
}

void DateNavigationWidget::on_rButtonCreationDate_clicked( bool checked )
{
    sl_updateTreeModel( checked );
    sl_UpdateStates();
}
void DateNavigationWidget::on_rButtonModificationDate_clicked( bool checked )
{
    sl_updateTreeModel( checked );
    sl_UpdateStates();
}

void DateNavigationWidget::sl_updateTreeModel( bool )
{
    if ( ui->rButtonCreationDate->isChecked() )
        ui->treeView->setModel( creationDateModel );

    else if ( ui->rButtonModificationDate->isChecked() )
        ui->treeView->setModel( modificationDateModel );

    if ( ui->treeView->model() )
        for ( int i = 0; i < ui->treeView->model()->columnCount(); i++ )
            ui->treeView->resizeColumnToContents(i);
}
