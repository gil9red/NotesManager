#include "page_trash.h"
#include "ui_page_trash.h"

#include "Manager/Manager.h"

Page_Trash::Page_Trash( const QStringList & labels, QWidget * parent )
    : QMainWindow( parent ),
      ui( new Ui::Page_Trash ),
      settings( 0 )
{
    ui->setupUi( this );

    model = new QStandardItemModel();
    model->setHorizontalHeaderLabels( labels );

    sortModel = new QSortFilterProxyModel( this );
    sortModel->setSourceModel( model );

    ui->tableView->horizontalHeader()->setMovable( false );
    ui->tableView->setModel( sortModel );

    loadNotes();
}

Page_Trash::~Page_Trash()
{
    delete ui;
}

void Page_Trash::loadNotes()
{
    sortModel->setDynamicSortFilter( false );

//    QStringList labels;
//    for ( int i = 0; i < model->columnCount(); i++ )
//        labels << model->horizontalHeaderItem( i )->text();

//    model->clear();
//    model->setHorizontalHeaderLabels( labels );

    foreach ( const QString & path, QDir( getTrashPath() ).entryList( QDir::Dirs | QDir::NoDotAndDotDot ) )
        model->appendRow( toStandardItems( QDir::fromNativeSeparators( getTrashPath() + "/" + path  ) ) );

    sortModel->setDynamicSortFilter( true );
}

void Page_Trash::setSettings( QSettings * s )
{
    settings = s;
}
void Page_Trash::readSettings()
{
    if ( !settings )
        return;

    settings->beginGroup( "Page_Trash" );
    ui->tableView->horizontalHeader()->restoreState( settings->value( "HorizontalHeader" ).toByteArray() );
    settings->endGroup();
}
void Page_Trash::writeSettings()
{
    if ( !settings )
        return;

    settings->beginGroup( "Page_Trash" );
    settings->setValue( "HorizontalHeader", ui->tableView->horizontalHeader()->saveState() );
    settings->endGroup();
}
