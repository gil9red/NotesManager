#include "completer.h"

#include <QScrollBar>
#include <QKeyEvent>
#include <QObject>
#include <QDebug>

Completer * Completer::self = 0;

static bool isEndOfWordChar(const QChar &c)
{
    return !c.isLetterOrNumber() && c.category() != QChar::Punctuation_Connector;
}
static bool acceptsCompletionPrefix(const QString &prefix)
{
    return prefix.length() >= 3;
}


Completer::Completer( QObject * parent )
    : QCompleter( parent ),
      autocomplete( true ),
      d_editor( 0 )
{
    self = this;
}
Completer::~Completer()
{
    self = 0;
}
void Completer::addDictionary( Dictionary * dict )
{
    dict->setParent( this );
    dictionaries[ dict->language() ] = dict;
}
Dictionary * Completer::dictionary( const QString & lang )
{
    QMapIterator < QString, Dictionary * > it( dictionaries );
    while ( it.hasNext() )
    {
        it.next();

        Dictionary * dict = it.value();
        if ( dict->language().indexOf( lang ) != -1 || lang.indexOf( dict->language() ) != -1 )
            return dict;
    }

    return 0;
}
bool Completer::setDictionary( const QString & lang )
{
   Dictionary * dict = dictionary( lang );
   if ( !dict )
       return false;

   setModel( dict->model() );
   return true;
}
void Completer::setEditor( QTextEdit * textEdit )
{
    if ( textEdit == d_editor )
        return;

    if ( d_editor )
        disconnect( this, SIGNAL( activated(QString) ), this, SLOT( insertCompletion(QString) ) );

    d_editor = textEdit;
    setWidget( d_editor );
    setCompletionMode( QCompleter::PopupCompletion );
    setCaseSensitivity( Qt::CaseInsensitive );

    connect( this, SIGNAL( activated(QString) ), this, SLOT( insertCompletion(QString) ) );
}
QTextEdit * Completer::editor()
{
    return d_editor;
}

QStringList Completer::languages()
{
    QStringList list;

    QMapIterator < QString, Dictionary * > it( dictionaries );
    while ( it.hasNext() )
    {
        it.next();
        list << it.key();
    }

    return list;
}
int Completer::count()
{
    return dictionaries.size();
}


QString Completer::textUnderCursor()
{
    QTextCursor textCursor = d_editor->textCursor();
    textCursor.select( QTextCursor::WordUnderCursor );
    return textCursor.selectedText();
}


void Completer::insertCompletion( const QString & completion )
{
    int extra = completion.length() - completionPrefix().length();
    d_editor->textCursor().movePosition( QTextCursor::Left );
    d_editor->textCursor().movePosition( QTextCursor::EndOfWord );
    d_editor->textCursor().insertText( completion.right( extra ) );
}

void Completer::setAutocomplete( bool b )
{
    autocomplete = b;
}
bool Completer::isAutocomplete()
{
    return autocomplete;
}

bool Completer::eventFilter( QObject * object, QEvent * event )
{
    if ( object == d_editor && autocomplete )
    {
        QKeyEvent * keyEvent = static_cast < QKeyEvent * > ( event );
        if ( keyEvent )
        {
            if ( d_editor && popup()->isVisible())
            {
                // The following keys are forwarded by the completer to the widget
                switch (keyEvent->key())
                {
                case Qt::Key_Enter:
                case Qt::Key_Return:
                case Qt::Key_Escape:
                case Qt::Key_Tab:
                case Qt::Key_Backtab:
                    keyEvent->ignore();
                    return true; // let the completer do default behavior
                default:
                    break;
                }
            }

            const bool isShortcut = ((keyEvent->modifiers() & Qt::ControlModifier) && keyEvent->key() == Qt::Key_Space);
            if (d_editor == 0 || !isShortcut) // do not process the shortcut when we have a completer
                return QCompleter::eventFilter( object, event );

            const bool ctrlOrShift = keyEvent->modifiers() & (Qt::ControlModifier | Qt::ShiftModifier);
            if (d_editor == 0 || (ctrlOrShift && keyEvent->text().isEmpty()))
                return true;

            const bool hasModifier = (keyEvent->modifiers() != Qt::NoModifier) && !ctrlOrShift;
            const QString newCompletionPrefix = textUnderCursor();
            const QChar lastChar = keyEvent->text().isEmpty() ? QChar() : keyEvent->text().right(1).at(0);

            if (!isShortcut && (hasModifier || keyEvent->text().isEmpty() || isEndOfWordChar(lastChar) || !acceptsCompletionPrefix(newCompletionPrefix)))
            {
                popup()->hide();
                return true;
            }

            if ( newCompletionPrefix != completionPrefix() )
            {
                setCompletionPrefix(newCompletionPrefix);
                popup()->setCurrentIndex( completionModel()->index(0, 0));
            }
            QRect cr = d_editor->cursorRect();
            cr.setWidth(popup()->sizeHintForColumn(0) + popup()->verticalScrollBar()->sizeHint().width());
            complete(cr); // popup it up!
        }
    }

    return QCompleter::eventFilter( object, event );
}
