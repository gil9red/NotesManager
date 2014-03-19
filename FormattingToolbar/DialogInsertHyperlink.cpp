#include "DialogInsertHyperlink.h"
#include "ui_DialogInsertHyperlink.h"

#include <QPushButton>


DialogInsertHyperlink::DialogInsertHyperlink( QWidget * parent )
    : QDialog( parent ),
      ui( new Ui::DialogInsertHyperlink )
{
    ui->setupUi( this );

    connect( ui->lEditInputHyperlink, SIGNAL( textChanged(QString) ), SLOT( updateStates() ) );
    connect( ui->lEditInputTextHyperlink, SIGNAL( textChanged(QString) ), SLOT( updateStates() ) );

    updateStates();
}
DialogInsertHyperlink::~DialogInsertHyperlink()
{
    delete ui;
}

QString DialogInsertHyperlink::hyperlink()
{
    return ui->lEditInputHyperlink->text();
}
QString DialogInsertHyperlink::text()
{
    return ui->lEditInputTextHyperlink->text();
}


void DialogInsertHyperlink::updateStates()
{
    bool hyperlinkIsEmpty = ui->lEditInputHyperlink->text().isEmpty();
    bool textIsEmpty = ui->lEditInputTextHyperlink->text().isEmpty();

    QPushButton * ok = ui->buttonBox->button( QDialogButtonBox::Ok );
    ok->setEnabled( !hyperlinkIsEmpty && !textIsEmpty );
}
