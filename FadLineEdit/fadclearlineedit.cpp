#include "fadclearlineedit.h"

FadClearLineEdit::FadClearLineEdit( const QString & text, QWidget * parent )
    : FadLineEdit( text, parent )
{
    init();
    updateStates();
}
FadClearLineEdit::FadClearLineEdit( QWidget * parent )
    : FadLineEdit( parent )
{
    init();
    updateStates();
}

void FadClearLineEdit::init()
{
    tButtonClearText.setCursor( Qt::ArrowCursor );
    tButtonClearText.setFocusPolicy( Qt::NoFocus );
    tButtonClearText.setStyleSheet( "QToolButton { border: none; }" );
    tButtonClearText.setToolButtonStyle( Qt::ToolButtonIconOnly );
    tButtonClearText.setIcon( QIcon( ":/text-clear" ) );
    tButtonClearText.setIconSize( QSize( 14, 14 ) );
    tButtonClearText.setToolTip( tr( "Clear text" ) );
    tButtonClearText.setStatusTip( tr( "Clear text" ) );
    tButtonClearText.setWhatsThis( tr( "Clear text" ) );
    addButton( &tButtonClearText );

    connect( &tButtonClearText, SIGNAL( clicked() ), SLOT( clear() ) );
    connect( &tButtonClearText, SIGNAL( clicked() ), SIGNAL( cleansingText() ) );
    connect( &tButtonClearText, SIGNAL( clicked() ), SLOT( updateStates() ) );

    connect( this, SIGNAL( textEdited(QString) ), SLOT( updateStates() ) );
}


void FadClearLineEdit::updateStates()
{
    tButtonClearText.setVisible( !text().isEmpty() );
}
