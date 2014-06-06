#include "AttachPanel.h"
#include "ui_AttachPanel.h"

#include "Note/RichTextNote.h"

#include <QItemSelectionModel>
#include <QDir>
#include <QMessageBox>
#include <QClipboard>
#include <QMimeData>
#include <QUrl>
#include <QDesktopServices>


AttachPanel::AttachPanel( QWidget * parent )
    : QWidget( parent ),
      ui( new Ui::AttachPanel ),
      note( 0 ),
      model( 0 )
{
    ui->setupUi( this );
    refresh();
}

AttachPanel::~AttachPanel()
{
    delete ui;
}

void AttachPanel::setNote( RichTextNote * note )
{
    if ( this->note )
        disconnect( ui->listView->selectionModel(), SIGNAL( selectionChanged(QItemSelection,QItemSelection) ), this, SLOT( updateStates() ) );

    if ( !note )
    {
        this->note = 0;
        model = 0;
        ui->listView->setModel( 0 );
        updateStates();
        return;
    }

    this->note = note;
    model = &this->note->attachModel;

    ui->listView->setModel( model );
    QObject::connect( ui->listView->selectionModel(), SIGNAL( selectionChanged(QItemSelection,QItemSelection) ), this, SLOT( updateStates() ) );

    refresh();
}


void AttachPanel::refresh()
{
    updateStates();

    if ( !note || !model )
        return;

    ui->listView->setUpdatesEnabled( false );
    note->updateAttachList();
    ui->listView->setUpdatesEnabled( true );    
}


void AttachPanel::updateStates()
{       
    if ( !note || !model )
    {
        ui->refreshList->setEnabled( false );
        ui->attach->setEnabled( false );
        ui->removeAll->setEnabled( false );
        ui->remove->setEnabled( false );
        ui->copyToClipboard->setEnabled( false );
        ui->run->setEnabled( false );
        return;
    }

    ui->refreshList->setEnabled( true );
    ui->attach->setEnabled( true );

    bool isEmpty = model->rowCount() == 0;
    bool hasSelection = ui->listView->selectionModel()->hasSelection();

    ui->removeAll->setEnabled( !isEmpty );
    ui->remove->setEnabled( hasSelection );
    ui->copyToClipboard->setEnabled( hasSelection );
    ui->run->setEnabled( hasSelection );
}

void AttachPanel::on_attach_clicked()
{
    note->selectAttach();
}
void AttachPanel::on_remove_clicked()
{
    QDir dir( note->attachDirPath() );

    int row = ui->listView->currentIndex().row();
    int col = ui->listView->currentIndex().column();
    QString fileName = model->item( row, col )->text();

    bool successful = dir.remove( fileName );
    if ( !successful )
    {
        QMessageBox::warning( this, tr( "Error" ), tr( "Can not delete file" ) );
        return;
    }

    refresh();

    emit note->changed( EventsNote::ChangeAttach );
}
void AttachPanel::on_removeAll_clicked()
{
    QDir dir( note->attachDirPath() );

    for ( int row = 0; row < model->rowCount(); row++ )
    {
        QString fileName = model->item( row, 0 )->text();

        bool successful = dir.remove( fileName );
        if ( !successful )
        {
            QMessageBox::warning( this, tr( "Error" ), tr( "Can not delete file" ) + QString( ":\n%1" ).arg( fileName ) );
            continue;
        }
    }

    refresh();
    emit note->changed( EventsNote::ChangeAttach );
}
void AttachPanel::on_refreshList_clicked()
{
    refresh();
}
void AttachPanel::on_copyToClipboard_clicked()
{
    int row = ui->listView->currentIndex().row();
    int col = ui->listView->currentIndex().column();
    QString fileName = note->attachDirPath() + "/" + model->item( row, col )->text();

    QList < QUrl > urls;
    urls << QUrl::fromLocalFile( fileName );

    QMimeData * mime = new QMimeData();
    mime->setUrls( urls );
    QApplication::clipboard()->setMimeData( mime );
}
void AttachPanel::on_run_clicked()
{
    int row = ui->listView->currentIndex().row();
    int col = ui->listView->currentIndex().column();
    QString fileName = note->attachDirPath() + "/" + model->item( row, col )->text();
    QDesktopServices::openUrl( QUrl::fromLocalFile( fileName ) );
}
