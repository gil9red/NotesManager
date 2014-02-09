/*************************************************************************
**
** Copyright (C) 2013-2014 by Ilya Petrash
** All rights reserved.
** Contact: gil9red@gmail.com
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


#include "ui_findandreplace.h"
#include "quickfind.h"

QuickFind::QuickFind( QTextEdit * textedit, QWidget * parent )
    : FindAndReplace( textedit, parent )
{
    ui->findEdit->setPlaceholderText( tr( "Quick find" ) );

    ui->labelTextFind->deleteLater();
    ui->labelTextReplaceWith->deleteLater();
    ui->replaceEdit->deleteLater();
    ui->tButtonReplace->deleteLater();
    ui->tButtonReplaceAll->deleteLater();

    layout()->removeItem( ui->verticalSpacer );
    layout()->removeItem( ui->horizontalLayoutReplace );
    layout()->removeItem( ui->horizontalSpacerReplace );

    ui->horizontalLayoutReplace->deleteLater();
}
