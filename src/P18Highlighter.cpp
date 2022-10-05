#include <iostream>
using namespace std;

#include <QColor>
#include <QRegExp>
#include <QString>
#include <QStringList>
#include <QtGui>

#include "codeeditor/P18Highlighter.h"


P18Highlighter::P18Highlighter (QTextDocument *parent)
    : HighlighterBase (parent)
{
    QColor directiveColor (0x80, 0x80, 0x0), // olive
           preprocessorColor (0x11, 0x88, 0x11), // green
           gpasmMacroColor (0x00, 0x20, 0xF), // navy blue
           cpikMacroColor (0x00, 0x20, 0xFF), // navy blue
           sfrColor (0xA5, 0x4B, 0xA5), // violine
           instructionColor (0, 0x60, 0xFF), // blue
           stringColor (0xBF, 0x3, 0x3), // bright red
           charConstColor (0xFF, 0x30, 0x0C), // orange
           numberColor (0, 0, 0xFF), // blue
           commentColor (0x60, 0x60, 0x60), //  grey
           moduleMarkColor (0xFF, 0x20, 0xFF),
           labelColor (0, 0, 0); // black

    setLanguage ("pic18", QStringList ("asm") << "ASM" <<"src" << "SRC"
		 << "slb" << "SLB" << "pic18" << "PIC18");
    setTabLength (5);

    // labels begin at column 1
    HLRules += makeRule ("^[a-zA-Z_\\$][a-zA-Z_\\$0-9]*", labelColor, "b");

    // assembler directives
    QStringList directivePatterns;
    directivePatterns << "\\b__BADRAM\\b"
		      << "\\b__CONFIG\\b"
		      << "\\b__IDLOCS\\b"
		      << "\\b__MAXRAM\\b"
		      << "\\bbankisel\\b"
		      << "\\bbanksel\\b"
		      << "\\bcblock\\b"
		      << "\\bcode\\b"
		      << "\\bcblock\\b"
		      << "\\bconstant\\b"
		      << "\\bda\\b"
		      << "\\bdata\\b"
		      << "\\bdb\\b"
		      << "\\bde\\b"
		      << "\\bdt\\b"
		      << "\\bdw\\b"
		      << "\\bendm\\b"
		      << "\\bendc\\b"
		      << "\\bendw\\b"
		      << "\\bequ\\b"
		      << "\\berror\\b"
		      << "\\berrorlevel\\b"
		      << "\\bextern\\b"
		      << "\\bexitm\\b"
		      << "\\bexpand\\b"
		      << "\\bfill\\b"
		      << "\\bglobal\\b"
		      << "\\bidata\\b"
		      << "\\blist\\b"
		      << "\\blocal\\b"
		      << "\\bmacro\\b"
		      << "\\bmessg\\b"
		      << "\\bnoexpand\\b"
		      << "\\bnolist\\b"
		      << "\\borg\\b"
		      << "\\bpage\\b"
		      << "\\bprocessor\\b"
		      << "\\bpagesel\\b"
		      << "\\bradix\\b"
		      << "\\bres\\b"
		      << "\\bset\\b"
		      << "\\bspace\\b"
		      << "\\bsubtitle\\b"
		      << "\\btitle\\b"
		      << "\\budata\\b"
		      << "\\budata_acs\\b"
		      << "\\budata_ovr\\b"
		      << "\\budata_shr\\b"
		      << "\\bvariable\\b"
		      << "\\bend\\b"
		      << "\\bBANKISEL\\b"
		      << "\\bBANKSEL\\b"
		      << "\\bCBLOCK\\b"
		      << "\\bCODE\\b"
		      << "\\bCBLOCK\\b"
		      << "\\bCONSTANT\\b"
		      << "\\bDA\\b"
		      << "\\bDATA\\b"
		      << "\\bDB\\b"
		      << "\\bDE\\b"
		      << "\\bDT\\b"
		      << "\\bDW\\b"
		      << "\\bENDM\\b"
		      << "\\bENDC\\b"
		      << "\\bENDW\\b"
		      << "\\bEQU\\b"
		      << "\\bERROR\\b"
		      << "\\bERRORLEVEL\\b"
		      << "\\bEXTERN\\b"
		      << "\\bEXITM\\b"
		      << "\\bEXPAND\\b"
		      << "\\bFILL\\b"
		      << "\\bGLOBAL\\b"
		      << "\\bIDATA\\b"
		      << "\\bLIST\\b"
		      << "\\bLOCAL\\b"
		      << "\\bMACRO\\b"
		      << "\\bMESSG\\b"
		      << "\\bNOEXPAND\\b"
		      << "\\bNOLIST\\b"
		      << "\\bORG\\b"
		      << "\\bPAGE\\b"
		      << "\\bPROCESSOR\\b"
		      << "\\bPAGESEL\\b"
		      << "\\bRADIX\\b"
		      << "\\bRES\\b"
		      << "\\bSET\\b"
		      << "\\bSPACE\\b"
		      << "\\bSUBTITLE\\b"
		      << "\\bTITLE\\b"
		      << "\\bUDATA\\b"
		      << "\\bUDATA_ACS\\b"
		      << "\\bUDATA_OVR\\b"
		      << "\\bUDATA_SHR\\b"
		      << "\\bVARIABLE\\b"
		      << "\\bEND\\b"
                      // Hi level extensions
		      << "\\.def\\b"
		      << "\\.DEF\\b"
		      << "\\.dim\\b"
		      << "\\.DIM\\b"
		      << "\\.direct\\b"
		      << "\\.DIRECT\\b"
		      << "\\.eof\\b"
		      << "\\.EOF\\b"
		      << "\\.file\\b"
		      << "\\.FILE\\b"
		      << "\\.ident\\b"
		      << "\\.IDENT\\b"
		      << "\\.line\\b"
		      << "\\.LINE\\b"
		      << "\\.type\\b"
		      << "\\.TYPE\\b";

    HLRules += makeRules (directivePatterns, directiveColor, "");

    QStringList gpasmMacroPatterns;
    gpasmMacroPatterns << "\\baddcf\\b"
		       << "\\bb\\b"
		       << "\\bbc\\b"
		       << "\\bbz\\b"
		       << "\\bbnc\\b"
		       << "\\bbnz\\b"
		       << "\\bclrc\\b"
		       << "\\bclrz\\b"
		       << "\\bsetc\\b"
		       << "\\bsetz\\b"
		       << "\\bmovfw\\b"
		       << "\\bnegf\\b"
		       << "\\bskpc\\b"
		       << "\\bskpz\\b"
		       << "\\bskpnc\\b"
		       << "\\bskpnz\\b"
		       << "\\bsubcf\\b"
		       << "\\btstf\\b";

    HLRules += makeRules (gpasmMacroPatterns, gpasmMacroColor, "b");

    // TODO, add short and long versions
    QStringList cpikMacroPatterns;
    cpikMacroPatterns << "\\bIBZ\\b"
		      << "\\bIBNZ\\b"
		      << "\\bIBRA\\b"
		      << "\\bICALL\\b";

    HLRules += makeRules (cpikMacroPatterns, cpikMacroColor, "b");

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

    HLRules += makeRules (sfrPatterns, sfrColor, "");

    QStringList instructionPatterns;
    instructionPatterns << "\\bW\\b" << " \\bF\\b"
			<< "\\bUPPER\\b"
			<< "\\bHIGH\\b"
			<< "\\LOW\\b"
			<< "\\bADDLW\\b"
			<< "\\bADDWF\\b"
			<< "\\bADDWFC\\b"
			<< "\\bANDLW\\b"
			<< "\\bANDWF\\b"
			<< "\\bBCF\\b"
			<< "\\bBC\\b"
			<< "\\bBN\\b"
			<< "\\bBNC\\b"
			<< "\\bBNN\\b"
			<< "\\bBNOV\\b"
			<< "\\bBNZ\\b"
			<< "\\bBZ\\b"
			<< "\\bBOV\\b"
			<< "\\bBRA\\b"
			<< "\\bBSF\\b"
			<< "\\bBTFSC\\b"
			<< "\\bBTG\\b"
			<< "\\bBTFSS\\b"
			<< "\\bCALL\\b"
			<< "\\bCLRF\\b"
			<< "\\bCLRW\\b"
			<< "\\bCLRWDT\\b"
			<< "\\bCOMF\\b"
			<< "\\bCPFSEQ\\b"
			<< "\\bCPFSGT\\b"
			<< "\\bCPFSLT\\b"
			<< "\\bDAW\\b"
			<< "\\bDECF\\b"
			<< "\\bDECFSZ\\b"
			<< "\\bDCFSNZ\\b"
			<< "\\bGOTO\\b"
			<< "\\bINCF\\b"
			<< "\\bINCFSZ\\b"
			<< "\\bINFSNZ\\b"
			<< "\\bIORLW\\b"
			<< "\\bIORWF\\b"
			<< "\\bLFSR\\b"
			<< "\\bMOVF\\b"
			<< "\\bMOVFF\\b"
			<< "\\bMOVLB\\b"
			<< "\\bMOVLW\\b"
			<< "\\bMOVWF\\b"
			<< "\\bMULLW\\b"
			<< "\\bMULWF\\b"
			<< "\\bNEGF\\b"
			<< "\\bNOP\\b"
			<< "\\bOPTION\\b"
			<< "\\bPOP\\b"
			<< "\\bPUSH\\b"
			<< "\\bRCALL\\b"
			<< "\\bRESET\\b"
			<< "\\bRETFIE\\b"
			<< "\\bRETLW\\b"
			<< "\\bRLCF\\b"
			<< "\\bRLNCF\\b"
			<< "\\bRRCF\\b"
			<< "\\bRRNCF\\b"
			<< "\\bRETURN\\b"
			<< "\\bRLF\\b"
			<< "\\bRRF\\b"
			<< "\\bSETF\\b"
			<< "\\bSLEEP\\b"
			<< "\\bSUBLW\\b"
			<< "\\bSUBFWB\\b"
			<< "\\bSUBWF\\b"
			<< "\\bSUBWFB\\b"
			<< "\\bSWAPF\\b"
			<< "\\bTSTFSZ\\b"
			<< "\\bTRIS\\b"
			<< "\\bTBLRD\\b"
			<< "\\bTBLWT\\b"
			<< "\\bXORLW\\b"
			<< "\\bXORWF\\b"
			<< "\\baddlw\\b"
			<< "\\baddwf\\b"
			<< "\\baddwfc\\b"
			<< "\\bandlw\\b"
			<< "\\bandwf\\b"
			<< "\\bbcf\\b"
			<< "\\bbc\\b"
			<< "\\bbn\\b"
			<< "\\bbnc\\b"
			<< "\\bbnn\\b"
			<< "\\bbnov\\b"
			<< "\\bbnz\\b"
			<< "\\bbz\\b"
			<< "\\bbov\\b"
			<< "\\bbra\\b"
			<< "\\bbsf\\b"
			<< "\\bbtfsc\\b"
			<< "\\bbtg\\b"
			<< "\\bbtfss\\b"
			<< "\\bcall\\b"
			<< "\\bclrf\\b"
			<< "\\bclrw\\b"
			<< "\\bclrwdt\\b"
			<< "\\bcomf\\b"
			<< "\\bcpfseq\\b"
			<< "\\bcpfsgt\\b"
			<< "\\bcpfslt\\b"
			<< "\\bdaw\\b"
			<< "\\bdecf\\b"
			<< "\\bdecfsz\\b"
			<< "\\bdcfsnz\\b"
			<< "\\bgoto\\b"
			<< "\\bincf\\b"
			<< "\\bincfsz\\b"
			<< "\\binfsnz\\b"
			<< "\\biorlw\\b"
			<< "\\biorwf\\b"
			<< "\\blfsr\\b"
			<< "\\bmovf\\b"
			<< "\\bmovff\\b"
			<< "\\bmovlb\\b"
			<< "\\bmovlw\\b"
			<< "\\bmovwf\\b"
			<< "\\bmullw\\b"
			<< "\\bmulwf\\b"
			<< "\\bnegf\\b"
			<< "\\bnop\\b"
			<< "\\boption\\b"
			<< "\\bpop\\b"
			<< "\\bpush\\b"
			<< "\\brcall\\b"
			<< "\\breset\\b"
			<< "\\bretfie\\b"
			<< "\\bretlw\\b"
			<< "\\brlcf\\b"
			<< "\\brlncf\\b"
			<< "\\brrcf\\b"
			<< "\\brrncf\\b"
			<< "\\breturn\\b"
			<< "\\brlf\\b"
			<< "\\brrf\\b"
			<< "\\bsetf\\b"
			<< "\\bsleep\\b"
			<< "\\bsublw\\b"
			<< "\\bsubfwb\\b"
			<< "\\bsubwf\\b"
			<< "\\bsubwfb\\b"
			<< "\\bswapf\\b"
			<< "\\btstfsz\\b"
			<< "\\btris\\b"
			<< "\\btblrd\\b"
			<< "\\btblwt\\b"
			<< "\\bxorlw\\b"
			<< "\\bxorwf\\b";

    HLRules += makeRules (instructionPatterns, instructionColor, "b");

    // literals
    // escaped quotes like \" are managed
    HLRules += makeRule ("\"(\\\"|[^\"])*\"", stringColor);
    HLRules += makeRule ("'(\\'|[^'])'", charConstColor);

    // various numeric constants
    QStringList numberPatterns;
    numberPatterns << "\\b\\d+[LlUu]{0,3}\\b" // decimal integer
		   << "\\b0(x|X)[\\d9A-Fa-f]+[LlUu]{0,3}\\b" // hex integer number
		   << "\\b((\\d*\\.\\d+)|(\\d+\\.\\d*))((E|e)(\\+|-)?\\d+)?\\b" // FP number
		   << "\\bB'[01]+'" // binary
		   << "\\bO'[0-7]+'" // octal
		   << "\\bD'[0-9]+'" // decimal
		   << "\\bH'[0-9A-Fa-f]+'"; // hex

    HLRules += makeRules (numberPatterns, numberColor);

    // TODO : adjust + finely the priority pb between '0' and D'0'

    // preprocessor directives
    QStringList preprocessorPatterns;
    preprocessorPatterns << "\\#define[ ]+[^\\n]*$"
			 << "\\#undefine[ ]+[^\\n]*$"
			 << "\\#v\\b"
			 << "\\b(?:include|if|while|ifdef|ifndef)[ ]+[^\\n]*$"
			 << "\\b(?:else|endif|endw)\\b"
			 << "\\b[{}]\\b";

    HLRules += makeRules (preprocessorPatterns, preprocessorColor);

    // single line comment
    HLRules += makeRule (";[^\n]*", commentColor);

    // Module marks (in .slb files)
    HLRules += makeRule ("^;<(?:\\+|=|-?)[^>]*>", moduleMarkColor, "bi");

    // prepare data that will be used by highlightBlock ()
    // TODO : to delete ??
#if 0
    multiLineCommentFormat.setForeground (commentColor);
    commentStartExpression = QRegExp ("/\\*");
    commentEndExpression = QRegExp ("\\*/");
#endif

    setCommentPattern ("; ");
}

void P18Highlighter::highlightBlock (const QString& text)
{
    foreach (const HLRule& rule, HLRules)
      {
        QRegExp expression (rule.pattern);
        int index = expression.indexIn (text);
        while (index >= 0)
	  {
            int length = expression.matchedLength ();
            setFormat (index, length, rule.format);
            index = expression.indexIn (text, index + length);
	  }
      }

#if 0
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
	  {
	    commentLength = (endIndex - startIndex
			     + commentEndExpression.matchedLength ());
	  }
        setFormat (startIndex, commentLength, multiLineCommentFormat);
        startIndex = commentStartExpression.indexIn (text, startIndex + commentLength);
      }
#endif

}
