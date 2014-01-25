#include "fullscreenshotcropper.h"
#include <QLabel>


FullscreenshotCropper::FullscreenshotCropper( QWidget * parent )
    : QDialog( parent )
{
    setWindowState( Qt::WindowFullScreen );

    QVBoxLayout * layout = new QVBoxLayout();
    layout->setMargin( 0 );
    layout->addWidget( &imageCropper );
    setLayout( layout );

    QToolButton * tButtonFullscreen = new QToolButton();
    tButtonFullscreen->setText( tr( "Fullscreen" ) );
    connect( tButtonFullscreen, SIGNAL( clicked() ), SLOT( fullscreen() ) );

    QToolButton * tButtonCropper = new QToolButton();
    tButtonCropper->setText( tr( "Cut" ) );
    connect( tButtonCropper, SIGNAL( clicked() ), SLOT( cropper() ) );

    QToolButton * tButtonQuit = new QToolButton();
    tButtonQuit->setText( tr( "Cancel" ) );
    connect( tButtonQuit, SIGNAL( clicked() ), SLOT( reject() ) );

    QLabel * label = new QLabel();
    QString textHint;
    textHint += "<b><font size=16 color=\"#18a0ff\" face=\"Times new roman\">";
    // Для снимка нажмите SPACE, ENTER или среднюю кнопку мыши
    // Для закрытия нажмите ESCAPE, Alt+F4, или правую кнопку мыши
    textHint += tr( "For the picture, press SPACE, ENTER or the middle mouse button" ) + "<br>";
    textHint += tr( "To close, press ESCAPE, Alt + F4, or right-click" );
    textHint += "</font></b>";
    label->setText( textHint );

    Qt::Alignment align = Qt::AlignTop | Qt::AlignLeft;

    QHBoxLayout * layoutImageCropper = new QHBoxLayout();
    layoutImageCropper->setMargin( 0 );
    layoutImageCropper->setSpacing( 0 );
    layoutImageCropper->addWidget( tButtonFullscreen, 0, align );
    layoutImageCropper->addWidget( tButtonCropper, 0, align );
    layoutImageCropper->addWidget( tButtonQuit, 0, align );
    layoutImageCropper->addStretch();


    QVBoxLayout * layoutMainImageCropper = new QVBoxLayout();
    layoutMainImageCropper->addLayout( layoutImageCropper );
    layoutMainImageCropper->addWidget( label, 0, Qt::AlignCenter );
    layoutMainImageCropper->addStretch();

    imageCropper.setLayout( layoutMainImageCropper );

    imageCropper.installEventFilter( this );
    installEventFilter( this );
}

void FullscreenshotCropper::setImage( const QPixmap & image )
{
    sourceImage = image;
    imageCropper.setImage( sourceImage );
}

QPixmap FullscreenshotCropper::cropperImage() const
{
    return cropImage;
}


void FullscreenshotCropper::cropper()
{
    cropImage = imageCropper.cropImage();
    accept();
}
void FullscreenshotCropper::fullscreen()
{
    cropImage = sourceImage;
    accept();
}


bool FullscreenshotCropper::eventFilter( QObject * object, QEvent * event )
{
    if ( event->type() == QEvent::KeyRelease )
    {
        QKeyEvent * keyEvent = static_cast < QKeyEvent * > ( event );
        switch ( keyEvent->key() )
        {
        case Qt::Key_Return:
        case Qt::Key_Enter:
        case Qt::Key_Space:
            cropper();
            break;

        case Qt::Key_Escape:
            reject();
            break;
        }

    } else if ( event->type() == QEvent::MouseButtonRelease )
    {
        QMouseEvent * mouseEvent = static_cast < QMouseEvent * > ( event );
        switch ( (int)mouseEvent->button() )
        {
        case Qt::RightButton:
            reject();
            break;

        case Qt::MiddleButton:
            cropper();
            break;
        }
    }

    return QDialog::eventFilter( object, event );
}
