#include "findfadlineedit.h"
#include "utils/execmenu.h"

#include <QDebug>

FindFadLineEdit::FindFadLineEdit( const QString & text, QWidget * parent )
    : FadClearLineEdit( text, parent )
{
    init();
}
FindFadLineEdit::FindFadLineEdit( QWidget * parent )
    : FadClearLineEdit( parent )
{
    init();
}

void FindFadLineEdit::setFindOptions( Shared::FindFlags flags )
{
    findFlags = flags;
}
Shared::FindFlags FindFadLineEdit::findOptions()
{
    return findFlags;
}


void FindFadLineEdit::init()
{
    menuOptions = new QMenu( this );
    actionCaseSensitive = new QAction( this );
    actionWholeWords = new QAction( this );
    actionRegExp = new QAction( this );

    menuOptions->addAction( actionCaseSensitive );
    menuOptions->addAction( actionWholeWords );
    menuOptions->addAction( actionRegExp );

    tButtonOptions.setCursor( Qt::ArrowCursor );
    tButtonOptions.setFocusPolicy( Qt::NoFocus );
    tButtonOptions.setStyleSheet( "QToolButton { border: none; }" );
    tButtonOptions.setToolButtonStyle( Qt::ToolButtonIconOnly );
    tButtonOptions.setIconSize( QSize( 17, 17 ) );
    tButtonOptions.setToolTip( tr( "Options" ) );
    tButtonOptions.setStatusTip( tr( "Options" ) );
    tButtonOptions.setWhatsThis( tr( "Options" ) );
    connect( &tButtonOptions, SIGNAL( clicked() ), SLOT( invokeShowOptions() ) );

    addButton( &tButtonOptions, Left );

    actionCaseSensitive->setText( tr( "Case sensitive" ) );
    actionCaseSensitive->setCheckable( true );
    actionCaseSensitive->setIcon( QIcon( ":/casesensitively" ) );

    actionWholeWords->setText( tr( "Whole words" ) );
    actionWholeWords->setCheckable( true );
    actionWholeWords->setIcon( QIcon( ":/wholewords" ) );

    actionRegExp->setText( tr( "Use regular expression" ) );
    actionRegExp->setCheckable( true );
    actionRegExp->setIcon( QIcon( ":/regexp" ) );

    QActionGroup * findActionGroup = new QActionGroup( this );
    findActionGroup->setExclusive( false );
    findActionGroup->addAction( actionCaseSensitive );
    findActionGroup->addAction( actionWholeWords );
    findActionGroup->addAction( actionRegExp );

    connect( findActionGroup, SIGNAL( triggered(QAction*) ), SLOT( invokeFindFlagsChanged() ) );

    updateIcons();
}


void FindFadLineEdit::invokeShowOptions()
{
    Utils::execMenuAtWidget( menuOptions, &tButtonOptions );
}

void FindFadLineEdit::updateIcons()
{
    tButtonOptions.setIcon( Find::iconForFindFlags( findFlags ) );
}
void FindFadLineEdit::invokeFindFlagsChanged()
{
    findFlags = 0;

    if ( actionCaseSensitive->isChecked() )
        findFlags |= Shared::CaseSensitive;

    if ( actionWholeWords->isChecked() )
        findFlags |= Shared::WholeWords;

    if ( actionRegExp->isChecked() )
        findFlags |= Shared::RegExp;

    updateIcons();
    emit findFlagsChanged();
}
