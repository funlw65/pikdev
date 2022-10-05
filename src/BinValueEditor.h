/* This bit editor allows to edit and display one bit in memory using
   a labeled checkbox */

#ifndef BINVALUEEDITOR_H
#define BINVALUEEDITOR_H

#include <QCheckBox>
#include <QString>
#include <QWidget>


class BinValueEditor : public QWidget
{
    Q_OBJECT
  
private:
    QString label;
    unsigned int *word;
    unsigned int bit;
    QCheckBox *cb;

public:
    /* ----------------------------------------------------------------------- 
      Bit editor widget

      Mainly a labelled checkbox connected to a bit.
       
      Parameters:
      wo: points to the word containing edited bit
      bi: the bit number inside *wo word
      tx: label of the bit
      editable: true if the bit can be edited
                 (if false, checkbox is non clickable)
      ----------------------------------------------------------------------- */  
    BinValueEditor (unsigned int *wo, unsigned int bi, const QString& tx,
		    QWidget *parent, bool editable = true);

signals:
    // emmited when a bit-widget is clicked
    void bitChanged ();

public slots:
    // update bit-widget when word is externally changed
    void updateDisplay ();

private slots:
    // update word when bit-widget is clicked
    void updateWord ();
};

#endif
