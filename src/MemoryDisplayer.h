#include <map>
#include <utility>
using namespace std;

#include <QGroupBox>
#include <QLabel>
#include <QList>

#include "FragBuffer.hh"
#include "HexValueEditor.h"


class MemoryDisplayer : public QGroupBox
{
    Q_OBJECT

private:
    FragBuffer& memory;
    unsigned int begin_display, // idx of first word to be displayed
    begin, // idx of first word of the buffer
    size, // number of words
    lines, // number of lines of the display window
    cols; // number of columns
    bool byte_addr; // true if the widget must display byte addresses
    HexValueEditor *firstEditor;
    QList<QLabel *> *adresses;
    std::map<int, pair<int, QString>> specialLocs;

public:
    MemoryDisplayer (QWidget *parent,
		     FragBuffer& mem,
		     unsigned int index, unsigned int siz,
		     unsigned int lin, unsigned int col,
		     const QString& title, unsigned int mask,
		     bool byteAddr, const char *sploc_descriptor = "");
    void setBase (unsigned int newbase);
    void decodeSPDescriptor (const char *descriptor);

signals:
    void BufferModified ();

public slots:
    void set1stLine (int origin);
    void updateDisplay ();
};
