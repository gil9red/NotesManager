#include "FadLineEdit.h"

FadLineEdit::FadLineEdit( const QString & text, QWidget * parent )
    : QLineEdit( text, parent )
{
    init();
}
FadLineEdit::FadLineEdit( QWidget * parent )
    : QLineEdit( parent )
{
    init();
}

void FadLineEdit::addButton( QAbstractButton * button, Side side )
{
    if ( side == Right )
    {
        d_rightSideButtons << button;
        rightSideLayout.addWidget( button, 0, Qt::AlignRight | Qt::AlignCenter );

    } else
    {
        d_leftSideButtons << button;
        leftSideLayout.insertWidget( 0, button, 0, Qt::AlignLeft | Qt::AlignCenter );
    }

    refreshTextMargins();
}
QAbstractButton * FadLineEdit::removeButton( QAbstractButton * button, Side side )
{
    if ( side == Right )
    {
        if ( d_rightSideButtons.removeOne( button ) )
        {
            rightSideLayout.removeWidget( button );
            button->setParent( 0 );

        } else
            return 0;

    } else
    {
        if ( d_rightSideButtons.removeOne( button ) )
        {
            leftSideLayout.removeWidget( button );
            button->setParent( 0 );

        } else
            return 0;
    }

    refreshTextMargins();
    return button;
}

QList < QAbstractButton * > * FadLineEdit::leftSideButtons()
{
    return &d_leftSideButtons;
}
QList < QAbstractButton * > * FadLineEdit::rightSideButtons()
{
    return &d_rightSideButtons;
}

FadLineEdit::Side FadLineEdit::sideButton( QAbstractButton * button )
{
    Side side = None;

    if ( d_leftSideButtons.indexOf( button ) != -1 )
        side = Left;

    else if ( d_rightSideButtons.indexOf( button ) != -1 )
        side = Right;

    return side;
}


void FadLineEdit::init()
{
    QHBoxLayout * mainLayout = new QHBoxLayout();
    setLayout( mainLayout );

    leftSideLayout.addStretch();
    rightSideLayout.addStretch();

    leftSideLayout.setSpacing( 0 );
    leftSideLayout.setContentsMargins( 0, 0, 0, 0 );

    rightSideLayout.setSpacing( 0 );
    rightSideLayout.setContentsMargins( 0, 0, 0, 0 );

    mainLayout->setSpacing( 0 );
    mainLayout->setMargin( 0 );
    mainLayout->addLayout( &leftSideLayout );
    mainLayout->addLayout( &rightSideLayout );
}
void FadLineEdit::refreshTextMargins()
{
    QMargins margins = textMargins();

    if ( !d_leftSideButtons.isEmpty() )
        margins.setLeft( leftSideLayout.sizeHint().width() );

    if ( !d_rightSideButtons.isEmpty() )
        margins.setRight( rightSideLayout.sizeHint().width() );

    setTextMargins( margins );
}
