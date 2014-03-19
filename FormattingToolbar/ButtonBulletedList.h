#ifndef ButtonBulletedList_h
#define ButtonBulletedList_h

#include "ButtonList.h"

#include <QMenu>
#include <QVBoxLayout>
#include <QTextCharFormat>

//! Кнопка с списком "маркированный текст".
class ButtonBulletedList: public ButtonList
{
  Q_OBJECT

public:
    ButtonBulletedList( QWidget * parent = 0 )
        : ButtonList( parent )
    {
        setCheckable( true );
        setPopupMode( QToolButton::MenuButtonPopup );
        setIcon( QIcon( ":/list-disc" ) );
        setDefaultId( QTextListFormat::ListDisc );

        QSize size( 60, 60 );
        QSize iconSize( 50, 50 );

        tButtonSquare = Create::Button::get( size, iconSize, QIcon( ":/list-square" ));
        tButtonDisc = Create::Button::get( size, iconSize, QIcon( ":/list-disc" ));
        tButtonCircle = Create::Button::get( size, iconSize, QIcon( ":/list-circle" ));

        QMenu * menu = new QMenu();
        menu->setLayout(new QVBoxLayout());
        setMenu( menu );
        menu->layout()->setSpacing( 0 );
        menu->layout()->setMargin( 0 );
        menu->layout()->addWidget( tButtonSquare );
        menu->layout()->addWidget( tButtonDisc );
        menu->layout()->addWidget( tButtonCircle );

        addButton( tButtonCircle, QTextListFormat::ListCircle );
        addButton( tButtonDisc, QTextListFormat::ListDisc );
        addButton( tButtonSquare, QTextListFormat::ListSquare );
    }

private:
    QToolButton * tButtonSquare;
    QToolButton * tButtonDisc;
    QToolButton * tButtonCircle;
};

#endif

