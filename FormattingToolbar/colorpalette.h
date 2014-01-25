#ifndef COLORPALETTE_H
#define COLORPALETTE_H

#include <QLabel>
#include <QMouseEvent>

//! Палитра цветов.
/*! Класс позволяет получить значение пикселя на своем окне. */
class ColorPalette: public QLabel
{
    Q_OBJECT

public:
    //! Конструктор.
    /*! \param path путь до файла картинки, которая и есть палитра
     *  \param parent указатель на виджет-родитель
     */
    ColorPalette( const QString & path, QWidget * parent = 0 );

    /*! Перегруженный конструктор. */
    ColorPalette( QWidget * parent = 0 );

public slots:
    //! Выбора картинки-палитры.
    /*! \param path путь до файла картинки, которая и есть палитра */
    void setImage( const QString & path );

protected:
    void mousePressEvent( QMouseEvent * event );

signals:
    //! При выборе цвета, отсылается сигнал.
    void selectedColor( QColor color );
};

#endif // COLORPALETTE_H
