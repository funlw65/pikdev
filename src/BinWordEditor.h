#ifndef BINWORDEDITOR_H
#define BINWORDEDITOR_H

#include <QLabel>
#include <QLineEdit>
#include <QString>
#include <QWidget>

#include "FragBuffer.hh"


class BinWordEditor : public QWidget
{
    Q_OBJECT

private:
    unsigned int *word;
    QLabel *val;

public:
    /* ----------------------------------------------------------------------- 
       Parameters:

       mem: the memory buffer
       index: index of edited word
       desc: descriptor string for this word
             (describe bit fields, name of bit field, and meaning of possible
             values)
       byteAddr: not used anymore
       ----------------------------------------------------------------------- */
    BinWordEditor (FragBuffer& mem, unsigned int index,
		   const QString& desc, bool byteAddr, QWidget *parent);

    /* ----------------------------------------------------------------------- 
       Parse word descriptor and extract the description of next bit field

       Parameters:
       desc: points to current bit field descriptor.
             This parameter is updated by the function, so, on exit, it points
             to the next descriptor.
       name: returned name of the decoded descriptor
       doc: returned documentation associated to descriptor
       b1: returned bit number of the first bit of the field
       b2: returned bit number of the last bit of the field
       
       Returns:
       true if successful, false if no more field
       ----------------------------------------------------------------------- */  
    bool parse (const char*& desc, QString& name, QString& doc, int& b1, int& b2);
  
    /* ----------------------------------------------------------------------- 
       Get word value
       
       Returns:
       the value
       ----------------------------------------------------------------------- */
    unsigned int getValue ();

    // Internally used by parse
    void parseDoc (const char*& p, QString& doc);

signals:
    // Emmited when the word is changed
    void wordChanged ();

private slots:
    // must be activated to synchronize hex display with bit display
    // (internal slot)
    void updateHexDisplay ();

public slots:
    // must be activated when the word is externally changed
    void updateDisplay ();
};

#endif
