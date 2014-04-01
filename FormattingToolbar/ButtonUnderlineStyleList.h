#ifndef UBUTTONUNDERLINESTYLELIST_H
#define UBUTTONUNDERLINESTYLELIST_H

#include "ButtonList.h"
#include "ColorButton.h"

#include <QMenu>
#include <QVBoxLayout>
#include <QTextCharFormat>

//! Кнопка с списком "стили подчеркивающей текст линии".
class ButtonUnderlineStyleList: public ButtonList
{
  Q_OBJECT

public:
    ButtonUnderlineStyleList( QWidget * parent = 0 )
        : ButtonList( parent )
    {
        setCheckable( true );
        setPopupMode( QToolButton::MenuButtonPopup );
        setIcon( QIcon( ":/FormattingToolbar/underline" ) );
        setDefaultId( QTextCharFormat::SingleUnderline );

        const QSize size = QSize(160, 20);
        const QSize iconSize = QSize(155, 20);

        tButtonNoUnderline = new QToolButton();
        tButtonNoUnderline->setText( tr( "No underline" ) );
        tButtonNoUnderline->setFixedSize( size );

        tButtonSingleUnderline = Create::Button::get( size, iconSize, QIcon( ":/FormattingToolbar/underline-single" ));
        tButtonDashUnderline = Create::Button::get( size, iconSize, QIcon( ":/FormattingToolbar/underline-dash" ));
        tButtonDotLine = Create::Button::get( size, iconSize, QIcon( ":/FormattingToolbar/underline-dot" ));
        tButtonDashDotLine = Create::Button::get( size, iconSize, QIcon( ":/FormattingToolbar/underline-dash-dot" ));
        tButtonDashDotDotLine = Create::Button::get( size, iconSize, QIcon( ":/FormattingToolbar/underline-dash-dot-dot" ));
        tButtonWaveUnderline = Create::Button::get( size, iconSize, QIcon( ":/FormattingToolbar/underline-wave" ));

        colorButton = new ColorButton();
        colorButton->setToolButtonStyle( Qt::ToolButtonTextBesideIcon );
        colorButton->setText( tr( "Color underline" ) );
        colorButton->setFixedWidth( size.width() );


        QMenu * menu = new QMenu();
        setMenu( menu );
        menu->setLayout( new QVBoxLayout() );
        menu->layout()->setSpacing( 0 );
        menu->layout()->setMargin( 0 );
        menu->layout()->addWidget( tButtonNoUnderline );
        menu->layout()->addWidget( tButtonSingleUnderline );
        menu->layout()->addWidget( tButtonDashUnderline );
        menu->layout()->addWidget( tButtonDotLine );
        menu->layout()->addWidget( tButtonDashDotLine );
        menu->layout()->addWidget( tButtonDashDotDotLine );
        menu->layout()->addWidget( tButtonWaveUnderline );
        menu->layout()->addWidget( colorButton );

        addButton( tButtonNoUnderline, QTextCharFormat::NoUnderline );
        addButton( tButtonSingleUnderline, QTextCharFormat::SingleUnderline );
        addButton( tButtonDashUnderline, QTextCharFormat::DashUnderline );
        addButton( tButtonDotLine, QTextCharFormat::DotLine );
        addButton( tButtonDashDotLine, QTextCharFormat::DashDotLine );
        addButton( tButtonDashDotDotLine, QTextCharFormat::DashDotDotLine );
        addButton( tButtonWaveUnderline, QTextCharFormat::WaveUnderline );

        QObject::connect( colorButton, SIGNAL( selectedColor(QColor) ), SIGNAL( selected(QColor) ) );
    }

private:
    QToolButton * tButtonNoUnderline;
    QToolButton * tButtonSingleUnderline;
    QToolButton * tButtonDashUnderline;
    QToolButton * tButtonDotLine;
    QToolButton * tButtonDashDotLine;
    QToolButton * tButtonDashDotDotLine;
    QToolButton * tButtonWaveUnderline;
    ColorButton * colorButton;

public slots:
    void setColor( const QColor & color )
    {
        colorButton->setColor( color );
    }

signals:
    void selected(QColor);
};

#endif
