#ifndef UBUTTONORDEREDLIST_H
#define UBUTTONORDEREDLIST_H

#include "ButtonList.h"

#include <QMenu>
#include <QTextListFormat>
#include <QVBoxLayout>

//! Кнопка с списком "упорядоченный текст".
class ButtonOrderedList: public ButtonList
{
  Q_OBJECT

public:
    ButtonOrderedList( QWidget *parent = 0 )
        : ButtonList( parent )
    {
        setCheckable( true );
        setPopupMode( QToolButton::MenuButtonPopup );
        setIcon( QIcon( ":/list-decimal" ) );
        setDefaultId( QTextListFormat::ListDecimal );

        QSize size( 60, 60 );
        QSize iconSize( 50, 50 );

        tButtonDecimal = Create::Button::get( size, iconSize, QIcon( ":/list-decimal" ));
        tButtonAlphaLower = Create::Button::get( size, iconSize, QIcon( ":/list-lower-alpha" ));
        tButtonAlphaUpper = Create::Button::get( size, iconSize, QIcon( ":/list-upper-alpha" ));
        tButtonRomanLower = Create::Button::get( size, iconSize, QIcon( ":/list-lower-roman" ));
        tButtonRomanUpper = Create::Button::get( size, iconSize, QIcon( ":/list-upper-roman" ));

        QMenu * menu = new QMenu();
        menu->setLayout( new QVBoxLayout() );
        setMenu( menu );
        menu->layout()->setSpacing( 0 );
        menu->layout()->setMargin( 0 );
        menu->layout()->addWidget( tButtonDecimal );
        menu->layout()->addWidget( tButtonAlphaLower );
        menu->layout()->addWidget( tButtonAlphaUpper );
        menu->layout()->addWidget( tButtonRomanLower );
        menu->layout()->addWidget( tButtonRomanUpper );

        addButton( tButtonDecimal, QTextListFormat::ListDecimal );
        addButton( tButtonAlphaLower, QTextListFormat::ListLowerAlpha );
        addButton( tButtonAlphaUpper, QTextListFormat::ListUpperAlpha );
        addButton( tButtonRomanLower, QTextListFormat::ListLowerRoman );
        addButton( tButtonRomanUpper, QTextListFormat::ListUpperRoman );
    }

private:
    QToolButton * tButtonDecimal;
    QToolButton * tButtonAlphaLower;
    QToolButton * tButtonAlphaUpper;
    QToolButton * tButtonRomanLower;
    QToolButton * tButtonRomanUpper;
};

#endif
