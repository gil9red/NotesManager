#include "DialogInsertHyperlink.h"
#include "ui_DialogInsertHyperlink.h"

#include <QPushButton>


DialogInsertHyperlink::DialogInsertHyperlink( QWidget * parent )
    : QDialog( parent ),
      ui( new Ui::DialogInsertHyperlink )
{
    ui->setupUi( this );

    QObject::connect( ui->hyperlink, SIGNAL( textChanged(QString) ), SLOT( updateStates() ) );
    QObject::connect( ui->textHyperlink, SIGNAL( textChanged(QString) ), SLOT( updateStates() ) );

    updateStates();
}
DialogInsertHyperlink::~DialogInsertHyperlink()
{
    delete ui;
}

void DialogInsertHyperlink::setHyperlink( const QString & hyperlink )
{
    ui->hyperlink->setText( hyperlink );
}
void DialogInsertHyperlink::setTextHyperlink( const QString & text )
{
    ui->textHyperlink->setText( text );
}

QString DialogInsertHyperlink::getHyperlink()
{
    return ui->hyperlink->text();
}
QString DialogInsertHyperlink::getTextHyperlink()
{
    return ui->textHyperlink->text();
}


void DialogInsertHyperlink::updateStates()
{
    bool hyperlinkIsEmpty = ui->hyperlink->text().isEmpty();
    bool textIsEmpty = ui->textHyperlink->text().isEmpty();

    QPushButton * ok = ui->buttonBox->button( QDialogButtonBox::Ok );
    ok->setEnabled( !hyperlinkIsEmpty && !textIsEmpty );
}
