/* A Hex value Editor
   May be linked with other Hex value Editors to
   edit memory blocks
   It is directly linked to an unsigned int variable */

#include <QColor>
#include <QLineEdit>
#include <QSize>
#include <QString>
#include <QWidget>


class HexValueEditor : public QLineEdit 
{
    Q_OBJECT

private:
    unsigned int *data; // associated data
    unsigned int mask; // data mask
    HexValueEditor *next; // next editor
signals:
    void bufferModified ();

private slots:
    void tryUpdate (const QString& string);
    void slotLostFocus ();

public:
    HexValueEditor (QWidget *parent, unsigned int *dataptr,
		    unsigned int mask, HexValueEditor *next = 0);
    virtual QSize sizeHint () const;
    virtual QSize minimumSizeHint () const;
    void setNextEditor (HexValueEditor *nex);
    HexValueEditor *nextEditor ();
    void setNewAdress (unsigned int *);
    void setColor (const QColor& color);
};
