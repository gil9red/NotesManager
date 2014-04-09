#include "tagsnavigationwidget.h"
#include "ui_tagsnavigationwidget.h"

#include "tagsmodel.h"
#include "note.h"
#include "basemodelitem.h"
#include "notemodelitem.h"
#include "notebook.h"

TagsNavigationWidget::TagsNavigationWidget(QWidget *parent)
    : QMainWindow( parent ),
      ui( new Ui::TagsNavigationWidget ),
      model(0)
{
    ui->setupUi(this);

    QObject::connect( Notebook::instance(), SIGNAL(sg_ItemRegistered(Tag*)), SLOT(sl_UpdateStates()) );
    QObject::connect( Notebook::instance(), SIGNAL(sg_ItemUnregistered(Tag*)), SLOT(sl_UpdateStates()) );
    sl_UpdateStates();
}
TagsNavigationWidget::~TagsNavigationWidget()
{
    delete ui;
}

void TagsNavigationWidget::setModel( TagsModel * m )
{
    model = m;
    ui->treeView->setModel(model);

    if ( ui->treeView->model() )
        for ( int i = 0; i < ui->treeView->model()->columnCount(); i++ )
            ui->treeView->resizeColumnToContents(i);
}

void TagsNavigationWidget::sl_ExpandAll()
{
    ui->treeView->expandAll();
}
void TagsNavigationWidget::sl_CollapseAll()
{
    ui->treeView->collapseAll();
}

void TagsNavigationWidget::sl_UpdateStates()
{
    bool hasEmpty = Notebook::instance()->getTagsList().isEmpty();
    ui->tButtonExpandAll->setEnabled( !hasEmpty );
    ui->tButtonCollapseAll->setEnabled( !hasEmpty );
}

void TagsNavigationWidget::on_tButtonExpandAll_clicked()
{
    sl_ExpandAll();
}
void TagsNavigationWidget::on_tButtonCollapseAll_clicked()
{
    sl_CollapseAll();
}

void TagsNavigationWidget::on_treeView_clicked( const QModelIndex & index )
{
    if ( !index.isValid() )
        return;

    BaseModelItem * item = static_cast < BaseModelItem * > ( index.internalPointer() );
    if ( item->DataType() == BaseModelItem::note )
    {
        Note* n = dynamic_cast < NoteModelItem * > ( item )->getStoredData();
        if ( !n )
        {
            WARNING("Casting error");
            return;
        }
        emit sg_NoteClicked(n);
    }
}
void TagsNavigationWidget::on_treeView_doubleClicked( const QModelIndex & index )
{
    if ( !index.isValid() )
        return;

    BaseModelItem * item = static_cast < BaseModelItem * > ( index.internalPointer() );
    if ( item->DataType() == BaseModelItem::note )
    {
        Note * n = dynamic_cast < NoteModelItem * > ( item )->getStoredData();
        if ( !n )
        {
            WARNING("Casting error");
            return;
        }
        emit sg_NoteDoubleClicked(n);
    }
}
