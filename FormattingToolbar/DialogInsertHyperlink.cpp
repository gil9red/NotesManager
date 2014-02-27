/*************************************************************************
**
** Copyright (C) 2013-2014 by Ilya Petrash
** All rights reserved.
** Contact: gil9red@gmail.com, ip1992@inbox.ru
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 3 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the
** Free Software Foundation, Inc.,
** 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
**
**************************************************************************/


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
