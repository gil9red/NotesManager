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
