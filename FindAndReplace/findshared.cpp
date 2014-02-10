#include "findshared.h"

//! Содержит функции, связанные с виджетами-поисковиками.
namespace Find
{
    //! Функция, в зависимости от переданных флагов возвращает иконку.
    /*! \param flags поисковые флаги
     *  \return иконка
     */
    QIcon iconForFindFlags( Shared::FindFlags flags )
    {
        static const QPixmap casesensitiveIcon = QPixmap( ":/casesensitively" );
        static const QPixmap wholewordsIcon = QPixmap( ":/wholewords" );
        static const QPixmap regexpIcon = QPixmap( ":/regexp" );

        bool casesensitive = flags.testFlag( Shared::CaseSensitive );
        bool wholewords = flags.testFlag( Shared::WholeWords );
        bool regexp = flags.testFlag( Shared::RegExp );

        if ( casesensitive || wholewords || regexp )
        {
            int width = 0;
            if ( casesensitive )
                width += 6;

            if ( wholewords )
                width += 6;

            if ( regexp )
                width += 6;

            if ( width > 0 )
                --width;

            QPixmap pixmap( width, 17 );
            pixmap.fill( Qt::transparent );
            QPainter painter( &pixmap );

            int x = 0;
            if ( casesensitive )
            {
                painter.drawPixmap( x - 6, 0, casesensitiveIcon );
                x += 6;
            }
            if ( wholewords )
            {
                painter.drawPixmap( x - 6, 0, wholewordsIcon );
                x += 6;
            }
            if ( regexp )
            {
                painter.drawPixmap( x - 6, 0, regexpIcon );
                x += 6;
            }

            return QIcon( pixmap );
        }

        return QIcon( ":/magnifier" );
    }
}

