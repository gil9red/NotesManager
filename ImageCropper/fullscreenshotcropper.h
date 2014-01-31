#ifndef FULLSCREENSHOTCROPPER_H
#define FULLSCREENSHOTCROPPER_H

#include <QKeyEvent>
#include <QMouseEvent>
#include <QDialog>
#include <QVBoxLayout>
#include <QDebug>
#include <QToolButton>
#include "ImageCropper.h"

class FullscreenshotCropper: public QDialog
{
    Q_OBJECT

public:
    FullscreenshotCropper( QWidget * parent = 0 );
    void setImage( const QPixmap & image );
    QPixmap cropperImage() const;

private:
    ImageCropper imageCropper;
    QPixmap sourceImage;
    QPixmap cropImage;

public slots:
    void cropper();
    void fullscreen();

protected:
    bool eventFilter( QObject *, QEvent * );
};

#endif // FULLSCREENSHOTCROPPER_H
