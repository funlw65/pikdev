#include <QtGui>

#include "CHighlighter.h"


CHighlighter::CHighlighter (QTextDocument *parent)
    : HighlighterBase (parent)
{
    // default colors
    QColor keywordColor (0x80, 0x80, 0x0); // olive
    QColor cppColor (0x11, 0x88, 0x11); // green
    QColor typeColor (0xA8, 0x51, 0x51); // Bordeaux red
    QColor stringColor (0xBF, 0x3, 0x3); // bright red
    QColor charConstColor (0xFF, 0x30, 0x0C); // orange
    QColor numericConstColor (0, 0, 0xFF); // blue
    QColor extensionColor (0, 0, 0); // black
    QColor sfrColor (0xA5, 0x4B, 0xA5); // violine
    QColor commentColor (0x3F, 0x7F, 0xFF); // light blue
    QColor functionColor (0x0, 0x0, 0x0); // black (italic)

    setLanguage ("C", QStringList ("c") << "h" << "C" << "H" << "cpp" << "CPP" << "hh" << "HH");
    setTabLength (3);

    // this installation can be done from the outside via the factory
    // this allows you to use another tool
    // setIndenter ("astyle --style = ansi --pad = oper --indent = spaces = 3 %1 2>&1");

    // function calls/definitions
    addRule ("function", "\\b[A-Za-z_][A-Za-z_0-9]*[ ]*(?=\\()", functionColor, "i");

    // language keywords
    QStringList keywordPatterns;
    keywordPatterns << "\\bbreak\\b"
		    << "\\bcase\\b"
		    << "\\bcontinue\\b"
		    << "\\bdefault\\b"
		    << "\\bdo\\b"
		    << "\\belse\\b"
		    << "\\benum\\b"
		    << "\\bextern\\b"
		    << "\\bfor[ ]*(?=\\()"
		    << "\\bforeach[ ]*(?=\\()"
		    << "\\bgoto\\b"
		    << "\\bif[ ]*(?=\\()"
		    << "\\binline\\b"
		    << "\\breturn\\b"
		    << "\\bsizeof\\b"
		    << "\\bstruct\\b"
		    << "\\bswitch\\b"
		    << "\\btypedef\\b"
		    << "\\bunion\\b"
		    << "\\bwhile[ ]*(?=\\()";

    addRules ("keyword", keywordPatterns, keywordColor, "");

    QStringList typePatterns;
    typePatterns << "\\bauto\\b"
		 << "\\bchar\\b"
		 << "\\bconst\\b"
		 << "\\bdouble\\b"
		 << "\\bfloat\\b"
		 << "\\bint\\b"
		 << "\\blong\\b"
		 << "\\bregister\\b"
		 << "\\bshort\\b"
		 << "\\bsigned\\b"
		 << "\\bstatic\\b"
		 << "\\bunsigned\\b"
		 << "\\bvoid\\b"
		 << "\\bvolatile\\b"
		 << "\\bint8_t\\b"
		 << "\\bint16_t\\b"
		 << "\\bint32_t\\b"
		 << "\\bint64_t\\b"
		 << "\\buint8_t\\b"
		 << "\\buint16_t\\b"
		 << "\\buint32_t\\b"
		 << "\\buint64_t\\b"
		 << "\\bsize_t\\b"
		 << "\\bwchar_t\\b";

    addRules ("type", typePatterns, typeColor);

    QStringList sfrPatterns;
    sfrPatterns << "\\bTOSU\\b"
		<< "\\bTOSH\\b"
		<< "\\bTOSL\\b"
		<< "\\bSTKPTR\\b"
		<< "\\bPCLATU\\b"
		<< "\\bPCLATH\\b"
		<< "\\bPCL\\b"
		<< "\\bTBLPTRU\\b"
		<< "\\bTBLPTRH\\b"
		<< "\\bTBLPTRL\\b"
		<< "\\bTABLAT\\b"
		<< "\\bPRODH\\b"
		<< "\\bPRODL\\b"
		<< "\\bINTCON\\b"
		<< "\\bINTCON2\\b"
		<< "\\bINTCON3\\b"
		<< "\\bINDF0\\b"
		<< "\\bPOSTINC0\\b"
		<< "\\bPOSTDEC0\\b"
		<< "\\bPREINC0\\b"
		<< "\\bPLUSW0\\b"
		<< "\\bFSR0H\\b"
		<< "\\bFSR0L\\b"
		<< "\\bWREG\\b"
		<< "\\bINDF1\\b"
		<< "\\bPOSTINC1\\b"
		<< "\\bPOSTDEC1\\b"
		<< "\\bPREINC1\\b"
		<< "\\bPLUSW1\\b"
		<< "\\bFSR1H\\b"
		<< "\\bFSR1L\\b"
		<< "\\bBSR\\b"
		<< "\\bINDF2\\b"
		<< "\\bPOSTINC2\\b"
		<< "\\bPOSTDEC2\\b"
		<< "\\bPREINC2\\b"
		<< "\\bPLUSW2\\b"
		<< "\\bFSR2H\\b"
		<< "\\bFSR2L\\b"
		<< "\\bSTATUS\\b"
		<< "\\bTMR0H\\b"
		<< "\\bTMR0L\\b"
		<< "\\bT0CON\\b"
		<< "\\bOSCCON\\b"
		<< "\\bLVDCON\\b"
		<< "\\bWDTCON\\b"
		<< "\\bRCON\\b"
		<< "\\bTMR1H\\b"
		<< "\\bTMR1L\\b"
		<< "\\bT1CON\\b"
		<< "\\bTMR2\\b"
		<< "\\bPR2\\b"
		<< "\\bT2CON\\b"
		<< "\\bSSPBUF\\b"
		<< "\\bSSPADD\\b"
		<< "\\bSSPSTAT\\b"
		<< "\\bSSPCON1\\b"
		<< "\\bSSPCON2\\b"
		<< "\\bADRESH\\b"
		<< "\\bADRESL\\b"
		<< "\\bADCON0\\b"
		<< "\\bADCON1\\b"
		<< "\\bADCON2\\b"
		<< "\\bCCPR1H\\b"
		<< "\\bCCPR1L\\b"
		<< "\\bCCP1CON\\b"
		<< "\\bCCPR2H\\b"
		<< "\\bCCPR2L\\b"
		<< "\\bCCP2CON\\b"
		<< "\\bPWM1CON\\b"
		<< "\\bECCPAS\\b"
		<< "\\bCVRCON\\b"
		<< "\\bCMCON\\b"
		<< "\\bTMR3H\\b"
		<< "\\bTMR3L\\b"
		<< "\\bT3CON\\b"
		<< "\\bSPBRG\\b"
		<< "\\bRCREG\\b"
		<< "\\bTXREG\\b"
		<< "\\bTXSTA\\b"
		<< "\\bRCSTA\\b"
		<< "\\bEEADR\\b"
		<< "\\bEEDATA\\b"
		<< "\\bEECON2\\b"
		<< "\\bEECON1\\b"
		<< "\\bIPR2\\b"
		<< "\\bPIR2\\b"
		<< "\\bPIE2\\b"
		<< "\\bIPR1\\b"
		<< "\\bPIR1\\b"
		<< "\\bPIE1\\b"
		<< "\\bOSCTUNE\\b"
		<< "\\bTRISE\\b"
		<< "\\bTRISD\\b"
		<< "\\bTRISC\\b"
		<< "\\bTRISB\\b"
		<< "\\bTRISA\\b"
		<< "\\bLATE\\b"
		<< "\\bLATD\\b"
		<< "\\bLATC\\b"
		<< "\\bLATB\\b"
		<< "\\bLATA\\b"
		<< "\\bPORTE\\b"
		<< "\\bPORTD\\b"
		<< "\\bPORTC\\b"
		<< "\\bPORTB\\b"
		<< "\\bPORTA\\b"
		<< "\\bEEADRH\\b";

    addRules ("sfr", sfrPatterns, sfrColor, "");

    // various numeric constants
    QStringList numericPatterns;
    numericPatterns << "\\b\\d+[LlUu]{0,3}\\b" // decimal integer
		    << "\\b0(x|X)[\\d9A-Fa-f]+[LlUu]{0,3}\\b" // hex integer number
		    << "\\b((\\d*\\.\\d+)|(\\d+\\.\\d*))((E|e)(\\+|-)?\\d+)?\\b"; // FP number
    addRules ("number", numericPatterns, numericConstColor);

    // extension : "placed" variables suffix
    // TODO : Check if the address must be hex
    addRule ("placed_var", "\\b[A-Za-z_]+[\\dA-Za-z_]*@0(?:x|X)[\\dA-Fa-f]+",
	     extensionColor, "b");

    // extension : variable slicing
    addRule ("slicing", "\\b[A-Za-z_]+[\\dA-Za-z_]*\\.[0-7](?:\\:[1-8])?\\b",
	     extensionColor, "b");

    // extension : binary constants
    addRule ("binary_const", "\\b0b[01_]+\\b", extensionColor, "b");

    // literals
    // escaped quotes like \" are managed
    addRule ("string", "\"(\\\"|[^\"])*\"", stringColor);

    addRule ("char_const", "'(\\'|[^'])*'", charConstColor);

    // single line comment
    addRule ("comment", "//[^\n]*", commentColor);

    // preprocessor directives
    QStringList cppPatterns;
    cppPatterns << "\\#include[ ]*\"[^\"]+\"" << "\\#include[ ]*<[^>]+>"
		<< "\\#if[ ]+[^\\n]*$" << "\\#ifdef[ ]+[^\\n]*$"
		<< "\\#ifndef[ ]+[^\\n]*$" << "\\#\\#" << "\\#define[ ]+[^\\n]*$"
		<< "\\#else" << "\\#elif[ ]+[^\\n]*$" << "\\#endif"
		<< "\\#undef[ ]+[^\\n]*$" << "\\#pragma[ ]+[^\\n]*$";

    addRules ("preprocessor", cppPatterns, cppColor);

    // prepare data that will be used by  highlightBlock ()
    commentStartExpression = QRegExp ("/\\*");
    commentEndExpression = QRegExp ("\\*/");

    setCommentPattern ("/*", "*/");
}

void CHighlighter::highlightBlock (const QString& text)
{
    // pick up single line format for multi-line comments
    const QTextCharFormat *commentfmt = 0;

    foreach (const HLRule& rule, HLRules_)
      {
        if (rule.name () == "comment")
	  commentfmt = &rule.format_;

        foreach (const QRegExp& expression, rule.patterns_)
	  {
            int index = expression.indexIn(text);
            while (index >= 0)
	      {
                int length = expression.matchedLength ();
                setFormat (index, length, rule.format_);
                index = expression.indexIn (text, index + length);
	      }
	  }
      }

    setCurrentBlockState (0);

    int startIndex = 0;
    if (previousBlockState () != 1)
      startIndex = commentStartExpression.indexIn (text);

    while (startIndex >= 0)
      {
        int endIndex = commentEndExpression.indexIn (text, startIndex);
        int commentLength;
        if (endIndex == -1)
	  {
            setCurrentBlockState (1);
            commentLength = text.length () - startIndex;
	  }
        else
	  commentLength = endIndex - startIndex + commentEndExpression.matchedLength ();
        if (commentfmt)
	  setFormat (startIndex, commentLength, *commentfmt);
        startIndex = commentStartExpression.indexIn (text, startIndex + commentLength);
      }
}

