#include "dialoginserthorizontalline.h"
#include "ui_dialoginserthorizontalline.h"

DialogInsertHorizontalLine::DialogInsertHorizontalLine(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogInsertHorizontalLine)
{
    ui->setupUi(this);
    on_type_activated( ui->type->currentText() );
}
DialogInsertHorizontalLine::~DialogInsertHorizontalLine()
{
    delete ui;
}

void DialogInsertHorizontalLine::accept()
{
    QString width = QString::number( ui->width->value() );
    if ( ui->type->currentText() == "%" )
        width += "%";

    emit sg_insertLine( width );
}

void DialogInsertHorizontalLine::on_type_activated(const QString &arg1)
{
    if ( arg1 == "%" )
        ui->width->setRange( 1, 100 );
    else
        ui->width->setRange( 1, INT_MAX );
}
