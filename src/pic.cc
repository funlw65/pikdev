/* --------------------------------------------------------------------- *
   Linux pic programming software
   Copyright 1994-1998 Brian C. Lane
   Rewritten by Alain Gibaud, 2000 
   
   The software has been inspirated by Brian C. Lane's C code   
   but has been re-written from scratch by Alain Gibaud in C++
   
   --------------------------------------------------------------------- */

#include <cassert>
#include <cctype>
#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <getopt.h>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <sys/time.h>
using namespace std;

#include "PicFactory.hh"


/* ----------------------------------------------------------------------- 
   pic class implementation by A Gibaud,
   This code has been written from Brian C. Lane work
   but is 99% rewritten
   ----------------------------------------------------------------------- */
pic::pic ()
{
    debugmode = 0;
    hard_ = 0;
    memaddr1 = memaddr2
      = idaddr1 = idaddr2
      = eeaddr1 = eeaddr2
      = cfaddr1 = cfaddr2
      = calibration1 = calibration2 = -1;
    wr_buf_size = 1;
    config_desc = "";
    specific_name = "";

    clk_delay = 0;
    programming_option = -1;
    setProgressListener (0);
}

pic::~pic ()
{
}

void pic::setPort (hardware *port)
{
    hard_ = port;
}
void pic::debugon (int mask)
{
    debugmode |= mask;
}

void pic::debugoff (int mask)
{
    debugmode &= ~mask;
}

/* -----------------------------------------------------------------------
   Initalize all the memory to its blank state (0xFFFF)
   ----------------------------------------------------------------------- */
void pic::fill_mem ()
{
    if (memaddr1 != -1)
      mem.setBlock (memaddr1, memaddr2);
    if (idaddr1 != -1)
      mem.setBlock (idaddr1, idaddr2);
    if (eeaddr1 != -1)
      mem.setBlock (eeaddr1, eeaddr2);
    if (cfaddr1 != -1)
      mem.setBlock (cfaddr1, cfaddr2);
}

/* -----------------------------------------------------------------------
   Load PIC data from a Intel HEX formatted file (8M or 16)
   with format autodetection
   ----------------------------------------------------------------------- */
bool pic::load_hex (const char *fname)
{
    int stat;
    ifstream fp;

    fp.open (fname);

    if (! fp.is_open ())
      {
        cerr << "Can't open " << fname << endl;
        return false;
      }

    fill_mem ();
    stat = readihex (fp);

    fp.close ();

    if (stat == 1)
      {
        cerr << "CRC error while reading " << fname << endl;
        return false;
      }
    else if (stat == 2)
      {
        cerr << "format error while reading " << fname << endl;
        return false;
      }

    return true;
}

/* ------------------------------------------------------------------------
   File Manipulation Routines for Linux PIC programmer
   ------------------------------------------------------------------------ */

/* -------------------------------------------------------------------------
   Read a Intel HEX file of either INHX16 or INHX8M format type,
   detecting the format automatically by the wordcount and length
   of the line

   Returns:
   0 == OK, 1 == CRC mismatch, 2 == corrupted file

   (Tested in 8 and 16 bits modes)
   ------------------------------------------------------------------------ */
int pic::readihex (ifstream& fp)
{
    char buf[1024];
    char *p;
    unsigned int count, addr, cksum, type, x, data, line, kend;
    unsigned int bytecount, datah, datal;
    unsigned int addrH = 0;  // upper 16 bits of 32 bits address (inhx32 format)
    bool is_inhx32 = false;

    line = 0;
    fill_mem (); // fill all usefull words with INVALID (i.e. 0xFFFFFFFF)

    // all the configuration words must be programmed
    // the following turn any INVALID config word to a valid one
    // initialized with All 1
    for (int k = cfaddr1; k <= cfaddr2; ++k)
      mem[k] &= wmask ();

    // Read each line of the file (getline() remove trailing '\n')
    while (fp.getline (buf, 1023))
      {
        line++;

        p = buf;
        // If it isn't a valid intel hex line, then move to the next line
        if (*p != ':')
	  continue;

        /* because we need the exact line length for format detection,
           strip out CR/LF (and Any non-hex digit !)
        */
        for (kend = strlen (buf) - 1; ! isxdigit (buf[kend]); --kend)
	  ;
        if (buf[kend] == ':')
	  continue; // ':' is alone
        buf[kend + 1] = 0;
        bytecount = (kend - 9) / 2; // number of data bytes of this record

        /* Get the byte count, the address and the type for this line.
           Because the checksum is computed from these elements,
           i assume they are mandatory, thus I can read them together */

        if (sscanf (++p, "%02X%04X%02X", &count, &addr, &type) != 3)
	  return 2;
        // Point to the first data byte
        p += 8;

        // Start calculating the checksum
        cksum = count + (addr >> 8) + (addr & 0xFF) + type;

        // End of file field ?, exit now
        if (type == 0x01)
	  {
            if (sscanf (p, "%02X", &data) != 1)
	      return 2; // bad EOF record
            else if (((cksum + data) & 0xFF) != 0)
	      return 1; // bad CRC
            else
	      return 0; // OK, EOF
	  }
        // example : linear extended record for adr 0x21xxxx
        // (blanks inserted for clarify)
        // :02 0000 04 0021 D9
        else if (type == 0x04)
	  {
            if (sscanf (p, "%04X%02X", &addrH, &data) != 2)
	      return 2;  // bad address record

            cksum += (addrH & 0xFF);
            cksum += (addrH >> 8);

            is_inhx32 = true;

            if (((cksum + data) & 0xFF) != 0) // CAUTION probably a bug fixed here (parenthesis)
	      return 1; // bad CRC
            else
	      continue; // goto next record
	  }

        /* Figure out if its INHX16 or INHX8M
           if count is a 16 bits words count => INHX16
           if count is a byte count => INHX8M or INHX32 */
        if (bytecount == count)
	  {
            /* Processing a INHX8M line */
            /* Modified to be able to read fuses from hexfile created by C18 toolchain
               changed by Tobias Schoene 9 April 2005,
               modified by A.G, because low and hi bytes was swapped
               in Tobias's code, 8 may 2005 */
            unsigned int addrbase = ((addrH << 16) | addr);
            for (x = 0; x < count; x++)
	      {
                if (sscanf (p, "%02X", &datal) != 1)
		  return 2;

                p += 2;

                unsigned int addrx = addrbase + x, addrx2 = addrx >> 1;
                if (addrx & 1)
		  {
                    // clear out hi bits (patch from Chris Teague chris.teague@gmail.com)
                    mem[addrx2] &= 0xFFU;
                    mem[addrx2] |= (datal << 8);   // Odd addr => Hi byte
		  }
                else
		  {
                    // clear out low bits (patch from Chris Teague chris.teague@gmail.com)
                    mem[addrx2] &= ~0xFFU;
                    mem[addrx2] |= datal; // Low byte
		  }

                cksum += datal;
	      }
	  }
        else if (bytecount == count * 2)
	  {
            if (is_inhx32)
	      return 2; // inconsistant format
            // Processing a INHX16 line
            for (x = 0; x < count; x++)
	      {
                if (sscanf (p, "%02X%02X", &datah, &datal) != 2)
		  return 2;
                p += 4;

                mem[addr + x] = datal | (datah << 8);
                cksum += datah;
                cksum += datal;
	      }
	  }
        else
	  return 2; // Brrrr !! Strange format.

        // Process the checksum
        if (sscanf (p, "%02X", &data) != 1)
	  return 2; // fmt error

        if (((data + cksum) & 0xFF) != 0)
	  return 1; // checksum error
      }

    return 0;
}

/* -------------------------------------------------------------------------
   Write one line of Intel-hex file
   Original code source from Timo Rossi,
   modified by Alain Gibaud to support large blocks write
   ------------------------------------------------------------------------ */
void pic::write_hex_block (ostream& fp,
			   int reclen, // length (in words)
			   // pointer to 1st data word (incremented by function)
			   FragBuffer::iterator& data,
			   ihexfmt format) // IHX8M or IHX16
{
    int check = 0;
    char buf[20];
    unsigned int loc;

    // A recursive algorithm is nicer but I don't want to stress the stack
    while (reclen > HEXBLKSIZE)
      {
        write_hex_block (fp, HEXBLKSIZE, data, format);
        reclen -= HEXBLKSIZE;
      }

    if (reclen <= 0)
      return; // Oops, block has just a HEXBLKSIZE * n size

    // write out line start ...
    if (format == IHX8M || format == IHX32)
      {
        loc = (*data).first * 2;
        sprintf (buf, ":%02X%04X00", 2 * reclen, loc & 0xFFFF);
	fp << buf;
        check += ((loc) & 0xff) + (((loc) >> 8) & 0xff) + 2 * reclen;
      }
    else if (format == IHX16)
      {
        loc = (*data).first;
        sprintf (buf, ":%02X%04X00", reclen, loc & 0xFFFF);
	fp << buf;
        check += (loc & 0xff) + ((loc >> 8) & 0xff) + reclen;
      }
    // else Hmmm .. something wrong

    for (; reclen > 0; ++data, --reclen)
      {
        unsigned int word = (*data).second;
        if (format == IHX8M || format == IHX32)
	  {
            sprintf (buf, "%02X%02X", (int) (word & 0xff), (int) ((word >> 8) & 0xff));
            fp << buf;
	  }
        else if (format == IHX16)
	  {
            sprintf (buf, "%02X%02X", (int) ((word >> 8) & 0xff), (int) (word & 0xff));
            fp << buf;
	  }
        // else Hmmm ..

        check += (word & 0xff) + ((word >> 8) & 0xff);
      }

    // write checksum, assumes 2-complement
    sprintf (buf, "%02X\n", (-check) & 0xff);
    fp << buf;
}

/* -------------------------------------------------------------------------
   Detects the next block to output
   A block is a set of consecutive addresse words not
   containing 0xFFFFFFFF
   
   Returns: 
   true if a block has been detected
   'it' is updated to point to the first address of the block
   '*len' contains the size of the block
   ------------------------------------------------------------------------ */
bool pic::fetch_next_block (FragBuffer::iterator& it, int *len)
{
    unsigned int startadr, curadr;
    // for (i = *start; (i < MAXPICSIZE) && (Mem[i] == INVALID); ++i);
    // skip non-used words

    // Search block start
    while ((it != mem.end ()) && (*it).second == 0xFFFFFFFFU)
      ++it;

    // if (i >= MAXPICSIZE) return false;
    if (it == mem.end ())
      return false;
    
    // search block end - a block may not cross a segment boundary
    FragBuffer::iterator itt (it);
    for (curadr = startadr = (*itt).first, ++itt; itt != mem.end (); ++itt)
      {
        if ((*itt).first != curadr + 1)
	  break; // non contiguous addresses
        if ((*itt).second == 0xFFFFFFFFU)
	  break; // unused word found
        if ((((*itt).first) & 0xFFFF0000U) != (curadr & 0xFFFF0000U))
	  break; // cross segment boundary

        curadr = (*itt).first;
      }
    *len = curadr - startadr + 1;

    return *len != 0;
}

/* -------------------------------------------------------------------------
   Write a Intel INHX16 or INHX8M or INHX32 formatted file

   Returns:
   0 == OK, 1 = file creation error
   ------------------------------------------------------------------------ */
int pic::write_hex (const char *name, ihexfmt format)
{
    int len;

    ofstream f (name);
    if (! f.is_open ())
      return 1;
    char buf[50];
    unsigned int oldseg = 0xFFFFFFFF, seg;

    FragBuffer::iterator block = mem.begin ();
    while (fetch_next_block (block, &len))
      {
        // block found, write it
        seg = (*block).first >> 15; // 2 * seg address
        if (format == IHX32 && seg != oldseg)
	  {
            unsigned int check = 0x02 + 0x04 + (seg >> 8) + (seg & 0xFF);
            sprintf (buf, ":02000004%04X%02X\n", seg, -check & 0xFF);
	    f << buf;
            oldseg = seg;
	  }
        write_hex_block (f, len,block, format);
      }
    f << ":00000001FF\n";
    f.close ();

    return 0;
}

int pic::wmask ()
{
   // compute mask from core
   return (1 << core) - 1;
}

/* ------------------------------------------------------------------------
   Programming Routines for Linux PIC programmer
   ------------------------------------------------------------------------ */
void pic::send_cmd (unsigned int d)
{
    send_bits (d, 6);
}

void pic::send_bits (unsigned int d, int nbb)
{
    int x;

    // Keep the write state...
    hard_->rwwrite ();

    for (x = nbb; x; --x)
      {
        hard_->clkhi (); // Clock high

        if (d & 0x01)
	  hard_->outhi (); // Output to 1
        else
	  hard_->outlo (); // Output to 0

        waitus (1 + clk_delay);
        hard_->clklo (); // Clock low
        waitus (1 + clk_delay);
        d >>= 1;
      }
    hard_->outhi ();

    // Normal (safe) state...
    hard_->rwread ();
}

void pic::send_data (unsigned int d)
{
    int x;

    // Keep the write state...
    hard_->rwwrite();

    d &= wmask (); // mask unused msb
    d <<= 1; // insert start bit

    for (x = 0; x < 16; x++)
      {
        hard_->clkhi ();
        if (d & 0x01)
	  hard_->outhi (); // Data bit is one
        else
	  hard_->outlo (); // Data bit is zero
        waitus (1 + clk_delay); // needed for slow programmers/fast PCs
        hard_->clklo ();
        waitus (1 + clk_delay);
        d >>= 1;
      }
    hard_->outhi (); // Added for ITU-1 support

    // Normal (safe) state...
    hard_->rwread ();
}

/* -----------------------------------------------------------------------
   Read 14 bits of data from the PIC
   clock idles low, change data. 1 start bit, 1 stop bit, data valid on
   falling edge.

   Get a word of data from device
   May be reimplemented
   (the default implementation is correct for (most ?) pic12 and pic16
   devices)
   ----------------------------------------------------------------------- */
unsigned int pic::get_word ()
{
    unsigned int x, ind;

    // Normal (safe) state...
    hard_->rwread ();

    ind = 0;

    hard_->outhi ();
    for (x = 16; x; --x)
      {
        hard_->clkhi ();
        waitus (1 + clk_delay);
        if (hard_->inbit ())
	  ind |= 0x8000; // Bit is 1
        else
	  ind &= 0x7FFF; // Bit is 0
        ind >>= 1;
        hard_->clklo ();
        waitus (1 + clk_delay);
      }
    return ind & wmask ();
}

/* -----------------------------------------------------------------------
   Read a byte of data from the device
   May be reimplemented
   The default implementation just call get_word () and is correct
   for pic12 and pic16 devices
   ----------------------------------------------------------------------- */
unsigned int pic::get_byte ()
{
    return get_word () & 0xFF;
}

/* ------------------------------------------------------------------------
   Delay for a number of uS -- Works better than usleep for me!
   ++Gib: maybe a better implementation might use nanosleep() 
   (but nanosleep() is linux-specific and the present implementation
   seems to work fine ... )
   ------------------------------------------------------------------------ */
void pic::waitus (unsigned int us)
{
    struct timeval tv, wt;

    gettimeofday (&tv, NULL);

    wt.tv_usec = (tv.tv_usec + us) % 1000000;
    wt.tv_sec = tv.tv_sec + ((tv.tv_usec + us) / 1000000);

    // loop until time is up
    for (;;)
      {
        gettimeofday (&tv, NULL);
        if (tv.tv_sec > wt.tv_sec)
	  return;
        if (tv.tv_sec == wt.tv_sec)
	  {
	    if (tv.tv_usec > wt.tv_usec)
	      return;
	  }
      }
}

/* ------------------------------------------------------------------------
   General command sequence interpreter
   The commands to be executed are located in "cmd"
   the "word" parameter is unusued by many commands, but may be
   used as input (by S and s and V and v) or as output (by R and r)
 
   Returns:
   number of  errors during verification process
 
   Here are the coding conventions for sending commands to the pic

   electric signals level commands:
   --------------------------------
   UPPER case means signal High - lower case means Low
   Example: "cPCcp" means "clock-low, , power ON, clock high,
   clock low, power OFF".
  
   c/C = clock
   d/D = data
   p/P = power (vdd)
   b/B = burn (vpp)

   higher level commands:
   --------------------------------
   wnnnnn = wait nnnnn microseconds (nnnnn is decimal)
   S  = send data word
   R = read data word
   s = send byte  (not impemented)
   r = read byte
   F = fake word read (read from pic, but don't send back via *word,
       keep the value for subsequent Verify)
   f = dummy byte read (read from pic, but don't send back via *word,
       keep the value for subsequent verify)
   knn = send 6 bits command nn to PIC (nn is decimal)
   v = verify byte data (compare previously read byte  against *word & 0xFF)
   V = verify word data (compare previously read word  against *word)
   ; = NOP (can be used as separator - usefull in debug mode)
   ? = return previously read word
   
   WARNING ! No blanks allowed

   COMMANDS SPECIFIC TO 18Fxxx SUPPORT:
   --------------------------------
   Knn = send 4 bits command nn to PIC (nn is decimal)
   xhh = send a 8 bits constant (hh must be a 2 digits hex constant)
   Xhhhh =  send a 16 bits constant (hhhh must be a 4 digits hex constant)
   , = a shorthand for w1
   ------------------------------------------------------------------------ */
int pic::pulseEngine (const char *cmd, unsigned int *word)
{
    static unsigned int rword = INVALID;
    unsigned int n;
    int errors, cmdcod;
    char buf[100];

    for (errors = 0; *cmd; ++cmd)
      {
        cmdcod = *cmd;

        switch (cmdcod)
	  {
	    default:
	      printf ("\npic::pulseEngine : invalid command [ %c ]\n", cmdcod);
	      exit (0);
	      break;
	    case ';': // NOP
	      if (debugmode & 1)
		cout << endl;
	      break;
	    case 'c':
	      if (debugmode & 1)
		cout << "CLOCK L" << endl;
	      else
		hard_->clklo ();
	      break;
	    case 'C':
	      if (debugmode & 1)
		cout << "CLOCK H" << endl;
	      else
		hard_->clkhi ();
	      break;
	    case 'd':
	      if (debugmode & 1)
		cout << "DATA L" << endl;
	      else
		hard_->outlo ();
	      break;
	    case 'D':
	      if (debugmode & 1)
		cout << "DATA H" << endl;
	      else
		hard_->outhi ();
	      break;
	    case 'p':
	      if (debugmode & 1)
		cout << "VDD Off" << endl;
	      else
		{
		  hard_->vddoff ();
		  waitus (10000);
		}
	      break;
	    case 'P':
	      if (debugmode & 1)
		cout << "VDD On" << endl;
	      else
		{
		  hard_->vddon ();
		  // wait 10ms for ALL devices
		  // because some programmers (or target board)
		  // have capacitors to charge
		  waitus (10000);
		}
	      break;
	    case 'b':
	      if (debugmode & 1)
		cout << "VPP Off" << endl;
	      else
		hard_->vppoff ();
	      break;
	    case 'B':
	      if (debugmode & 1)
		cout << "VPP On" << endl;
	      else
		{
		  hard_->vppon ();
		  waitus (100000);
		}
	      break;
	    case 'w':
		for (n = 0, ++cmd; *cmd && isdigit ((int) *cmd); ++cmd)
		  n = (n * 10) + *cmd - '0';
		--cmd;
		if (debugmode & 1)
		  cout << "WAIT " << n << " micro-sec." << endl;
		else
		  hard_->waitus (n);
		break;
	    case 'S':
	      if (debugmode & 1)
		{
		  sprintf (buf, "SEND 0x%04X", *word);
		  cout << buf << endl;
		}
	      else
		send_data (*word);
	      break;
	    case 'R':
	      if (debugmode & 1)
		cout << "Read Word" << endl;
	      else
		*word = get_word ();
	      break;
	    case 'r':
	      if (debugmode & 1)
		cout << "Read Byte" << endl;
	      else
		*word = get_byte ();
	      break;
	    case 'F':
	      if (debugmode & 1)
		cout << "Fake read Word" << endl;
	      else
		rword = get_word ();
	      break;
	    case 'f':
	      if (debugmode & 1)
		cout << "Fake read Byte" << endl;
	      else
		rword = get_byte ();
	      break;
	    case 'k':
	      for (n = 0, ++cmd; *cmd && isdigit ((int) *cmd); ++cmd)
		n = (n * 10) + *cmd - '0';
	      --cmd;
	      if (debugmode & 1)
		cout << "Send cmd " << n << endl;
	      else
		send_cmd (n);
	      break;
	    case 'K':
	      assert ("pulseEngine : K command removed, please use k instead" == 0);
	      break;
	    case 'V':
	      if (debugmode & 1)
		cout << "Compare Word " << endl;
	      else
		{
		  if (rword != (*word & wmask ()))
		    ++errors;
		}
		break;
	    case 'v':
		if (debugmode & 1)
		  cout << "Compare Byte " << endl;
		else
		  {
		    if (rword != (*word & 0xFF))
		      ++errors;
		  }
		break;
	    case 'x':
	      sscanf (++cmd, "%02X", &n);
	      ++cmd;

	      if (debugmode & 1)
		{
		  sprintf (buf, "Send 1 byte constant : 0x%02X", n);
		  cout << buf << endl;
		}
	      else
		send_bits (n, 8);
	      break;
	    case 'X':
	      sscanf (++cmd, "%04X", &n);
	      cmd += 3;

	      if (debugmode & 1)
		{
		  sprintf (buf, "Send 2 byte constant : 0x%04X", n);
		  cout << buf << endl;
		}
	      else
		send_bits (n, 16);
	      break;
	    case ',':
	      if (debugmode & 1)
		cout << "Wait 1 micro-sec" << endl;
	      else
		waitus (1);
	      break;
	    case '?':
	      if (debugmode & 1)
		cout << "Fetch fake cache : " << hex << rword << dec << endl;
	      else
		*word = rword & wmask ();
	      break;
	  }
      }
    return errors;
}

void pic::set_memory (unsigned int adr, unsigned int val)
{
    mem[adr] = val;
}

/* -------------------------------------------------------------------------
   Displays contents of the mem buffer
   invalid (i.e.: 0xFFFF words) are not displayed 
   and acts as blocks separators
   
   Returns:
   the number of detected blocks
   ------------------------------------------------------------------------ */
int pic::display_mem (ostream &out)
{
    int len, nblocks;

    FragBuffer::iterator block = mem.begin ();
    string adrtype = byteAdresses () ? " (bytes adresses) " : " (words adresses) ";

    for (nblocks = 0; fetch_next_block (block, &len); ++nblocks)
      {
        // block found, display it
        out << "======= Block #" << hex << nblocks << adrtype << endl;
        display_block (out, len, block);
      }
    return nblocks;
}

/* -------------------------------------------------------------------------
   Display a block of memory buffer on "out" stream
   iterator block is updated by the function
   ------------------------------------------------------------------------ */
#define ASCIIBLKSIZE 8 // number of values per line

void pic::display_block (ostream &out,
			 int reclen, // length (in words)
			 FragBuffer::iterator& block) // iterator to block
{
    char buf[20];

    while (reclen > ASCIIBLKSIZE)
      {
        display_block (out, ASCIIBLKSIZE, block);
        reclen -= ASCIIBLKSIZE;
      }

    if (reclen <= 0)
      return; // Oops, block has just a ASCIIBLKSIZE * n size

    // adress of first word of the line
    // form is not ANSI conformant, so the following line is not portable
    // out.form("%04X :", loc );
    // uppercase is not always implemented, so the following line is not portable
    // out << hex << setw(4) << setfill('0') << uppercase << loc << " :" ;
    // so I use an old-fashoned sprintf

    unsigned int adr = (*block).first;
    if (byteAdresses ())
      adr *= 2;
    sprintf (buf, "%08X :", adr);
    out << buf;
    for (; reclen > 0; ++block, --reclen)
      {
	sprintf (buf, " %04X", (*block).second);
	out << buf;
      }
    out << endl;
}

// Return message corresponding to the error code "code"
const char *pic::error_msg (int code)
{
    static char m[500];

    *m = 0;
    if (code == 0)
      strcpy (m, "No error");
    else
      {
        if (code & 1)
	  {
            if (*m)
	      strcat (m, "+");
            strcat (m, "memory");
	  }
        if (code & 2)
	  {
            if (*m)
	      strcat (m, "+");
	    strcat (m, "EEPROM");
	  }
        if (code & 4)
	  {
            if (*m)
	      strcat (m, "+");
            strcat (m, "ID");
	  }
        if (code & 8)
	  {
            if (*m)
	      strcat (m, "+");
            strcat (m, "fuses");
	  }
        if (code & 16)
	  {
            if (*m)
	      strcat (m, "+");
	    strcat (m, "non applicable op.");
	  }        
        if (code & (1 | 2 | 4 | 8 | 16))
	  strcat (m," error");        
        if (code & 32)
	  {
            if (*m)
	      strcat (m, "+");
	    strcat (m, "information");
	  }
        if (code & 64)
	  {
            if (*m)
	      strcat (m, "+");
            strcat (m, "calibration");
	  }
      }

    return m;
}

void pic::getBuffers (FragBuffer **membuf,
		      int& mem1, int& mem2,
		      int& ee1, int& ee2,
		      int& id1, int& id2,
		      int& cf1, int& cf2,
		      int& cal1, int& cal2)
{
    *membuf = &mem;
    mem1 = memaddr1; mem2 = memaddr2;
    ee1 = eeaddr1; ee2 = eeaddr2;
    id1 = idaddr1; id2 = idaddr2;
    cf1 = cfaddr1; cf2 = cfaddr2;
    cal1 =  calibration1;
    cal2 =  calibration2;
}

const char *pic::loadHexFile (const char *filename)
{
    ifstream ii (filename);
    if (! ii.is_open ())
      return "Can't open file ";

    // 0 == OK, 1 == CRC mismatch, 2 == corrupted file
    switch (readihex (ii))
      {
        case 0:
	  return 0;
        case 1:
	  return "CRC mismatch ";
        case 2:
	  return "Corrupted file ";
      }
    return "Unknown error code in loadHexFile ";
}

// Support format name compatibility with gnupic
const char *pic::writeHexFile (const char *fileName, const char *format)
{
    ihexfmt fmt;
    string f (format);

    if (f == "inhx8m")
      fmt = IHX8M;
    else if (f == "inhx16")
      fmt = IHX16;
    else if (f == "inhx32")
      fmt = IHX32;
    else
      return "Unsupported HEX-file format ";

    if (write_hex (fileName, fmt) == 1)
      return "Can't create file ";

    return 0;
}

// Computes the word size (in bits) from the word mask
int pic::wwidth ()
{
    return core; // new generic framework
}

// Non virtual interfaces to program/verify routines
// Output stream is forced to cout
int pic::program_pic ()
{
    return program_pic (cout);
}

int pic::verify_pic ()
{
    return verify_pic (cout);
}

// Must be used by program/verify algorithms to output diags
void pic::warning (ostream& out, const char *what,
		   int where, int chipvalue, int memvalue)
{
    out << "Warning: [" << what << "] (addr = 0x" << hex << where;
    if (chipvalue != -1)
      out << ", device = 0x" << (chipvalue & wmask ());
    if (memvalue != -1)
      out << ", buffer = 0x" << (memvalue & wmask ());

    out << ")" << endl << dec << flush;
}

/* -------------------------------------------------------------------------
   Deduces the device family from the pic names
   
   Returns:
   12, 16, 17 or 18 or 0 if picname is not recognized
   ------------------------------------------------------------------------ */
int pic::picFamily ()
{
    const char *n = names ();
    while (*n == ' ')
      ++n;
    int f = 0;
    sscanf (n, "p%d", &f);
    return f;
}

bool pic::byteAdresses () const
{
    return core == 16;
}

string pic::deviceID2Name (unsigned int id)
{
    return pikDB::pikdb ()->getName (id & ~0x1F);
}

unsigned int pic::deviceID2Revision (unsigned int id)
{
    return id & 0x1F;
}

/* -------------------------------------------------------------------------
   Device autodetection
   
   Returns:
   The raw type ID of the device (including revision number)
   ------------------------------------------------------------------------ */
unsigned int pic::get_deviceID ()
{
    return 0; // no autodetection by default
}

/* -------------------------------------------------------------------------
   Device autodetection support

   Returns:
   A blanks separated list of pic ID. this list MUST correspond item by item 
   to the names list returned by names(). 
   Each ID of the list is the device ID word in which the revision number is
   masked.
   It must be specified in hex.
   If none of the devices returned by names() is autodetectable, IDs() can
   return an empty string. If some devices are not identifiable,their  ID must
   be 0.

   The default implementation is to return an empty string, so old non
   autodetectable devices do not need this method to be implemented.
   ------------------------------------------------------------------------ */
const char *pic::IDs ()
{
    return "";
}

/* -------------------------------------------------------------------------
   Special locations descriptor
   
   This method returns a list of special memory locations (in device's memory
   space)
   The main purpose of this method is to allow the programmer widget to
   highlight some special words.
   Be careful: as special word needs color changes and tooptips
   allocations/deallocation, using too many special words can slow down the
   programming widget.

   General syntax:
   ---------------
   adresses_range = color{help_text} ...

   * adresses_range:
   [low_addr:hi_addr] | unique_addr
   (unique_addr is a shorthand for [unique_addr:unique_addr], all addresses
   are hexadecimal values)

   * color:
   #RRGGBB
   (A HTML-like RGB value)
   color field can be ommited. In this case, the previous color is used.
   If no color is provided, a default color is used.

   * help_text:
   Arbitrary long text, without } character. \n are allowed.
   {help_text} can be ommited. In this case, the location does not have any
   attached text.

   The method can return several adress specifications, separated by blanks

   Example:
   return "[0-3]=#FF0000{first four words} 3FF=#AAAAAA{OSCCAL word}";

   Returns:
   A blank separated list of locations descriptor.
   Must return an empty string (i.e.:"") if not used.
   Never return a NULL pointer !
   ------------------------------------------------------------------------ */
const char *pic::SPLocs ()
{
    return "";
}

const char *pic::progOptions ()
{
    return "";
}

int pic::setProgOptionNumber (int opt)
{
    int k = programming_option;
    programming_option = opt;
    return k;
}

/* -------------------------------------------------------------------------
    Read the currently plugged device, an translate
    its ID to name .
    The default implementation of read_device_name() is correct
    for all devices supported by pikdev programming engine,
    so this function just needs to be reimplemented when and
    external  programming software (picp, pk2, etc..) is used
    

    Returns:
    "" : id unavailable
    "?" : id is available, but device is not supported by the
          current programming engine
    else, the chip name with lowercase chars (ie: p16f628)

    Note:
    This is an attempt to provide an uniform programming interface
    for both internal (i.e.:pikdev) and external programming software.
   ------------------------------------------------------------------------ */
string pic::read_device_name ()
{
    unsigned int id = get_deviceID ();

    return deviceID2Name (id);
}

/* -------------------------------------------------------------------------
   Returns the currently plugged device revision number
   as a string. 
   When the device is not autodetectable, must return
   an empty string ("")
   ------------------------------------------------------------------------ */
string pic::read_device_revision ()
{
    unsigned int id = get_deviceID ();

    if (id == 0)
      return "";
    else
      {
        ostringstream buf;
        buf << deviceID2Revision (id);
        return buf.str ();
      }
}

/* -------------------------------------------------------------------------
   Gets device characteristics from pikDB
   
   Returns:
   true for known devices.
   ------------------------------------------------------------------------ */
bool pic::instanciate (const char *devname)
{
    specific_name = "";
    
    pikDB *db = pikDB::pikdb ();
    deviceRecord *dev = db->getData (devname);
    
    if (! dev)
      return false;

    memaddr1 = dev->memaddr1;
    memaddr2 = dev->memaddr2;
    idaddr1 = dev->idaddr1;
    idaddr2 = dev->idaddr2;
    eeaddr1 = dev->eeaddr1;
    eeaddr2 = dev->eeaddr2;
    cfaddr1 = dev->cfaddr1;
    cfaddr2 = dev->cfaddr2;
    calibration1 = dev->calibration1;
    calibration2 = dev->calibration2;
    wr_buf_size = dev->wr_buf_size;
    config_desc = dev->configDesc;
    core = dev->core;
    specific_name = dev->gpName;

    // as generic pic can be used for different
    // real devices, clear device memory
    mem.clear ();

    // hook for handling very special cases
    postInstanciate (devname);
    return true;
}


// This config_bits routine should not be overloaded
const char *pic::config_bits ()
{
    return config_desc;
}

const char *pic::name ()
{
    return specific_name;
}

void pic::postInstanciate (__attribute__ ((unused)) const char *devicename)
{
}

unsigned int& pic::operator[] (unsigned int index)
{
    return mem[index];
}

// true if the device supports autodetection
bool pic::isDetectable ()
{
    return pikDB::pikdb ()->getID (specific_name) != -1U;
}

// Returns specific ID of this device
// or 0 if device is not autodetectable or instanciation has failed.
unsigned int pic::specificID ()
{
    if (specific_name[0] == 0)
      return 0;
    else
      return pikDB::pikdb ()->getID (specific_name);
}

const char *pic::specificName ()
{
    return specific_name;
}

// Connect a progress handler for displaying current progress status
// of device programming.
void pic::setProgressListener (progressListener *pl)
{
    progress_status.connect (pl);
}

/* -------------------------------------------------------------------------
   Init progress status from various memory sizes

   Returns the total step to be performed. 

   Note: Might be wrong in some special cases, just reload
   process_status with the correct value
   ------------------------------------------------------------------------ */
int pic::initProgress ()
{
    int steps = (memaddr2 - memaddr1 + 1);
    if (eeaddr1 != -1)
      steps += (eeaddr2 - eeaddr1 + 1);
    if (cfaddr1 != -1)
      steps += (cfaddr2 - cfaddr1 + 1);
    if (idaddr1 != -1)
      steps += (idaddr2 - idaddr1 + 1);

    progress_status = steps;
    return steps;
}

// Default implementation for names
// no device supported
const char *pic::names ()
{
    return "";
}

int pic::deviceCore ()
{
    return core;
}
