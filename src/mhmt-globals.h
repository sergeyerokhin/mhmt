#ifndef MHMT_GLOBALS_H

#define MHMT_GLOBALS_H


#include <stdio.h>
#include "mhmt-types.h"

//#define DBG


// packer/depacker type constants, used in struct global
#define PK_MLZ  1
#define PK_HRM  2
#define PK_HST  3



// definition of a global configuration/working structure:
// holding parsed commandline options, file sizes/descriptors,
// allocated memory arrays etc.
struct globals
{
	ULONG packtype; // packer/depacker type: one of PK_MLZ, PK_HRM, PK_HST

	ULONG greedy; // zero if no greedy mode coding (optimal coding), non-zero if greedy mode

	ULONG mode; // zero if packing, non-zero if depacking

	ULONG zxheader; // zero if no zx header needed, non-zero if zx header is needed.
	                // zx header, among other, contain some bytes from the end of the unpacked file

	ULONG wordbit; // zero if bitstream is spread in 8bit (one-byte) chunks across bytestream,
	               // non-zero if chunks are 16bit (two-byte).

	ULONG bigend; // zero if 16bit bitstream chunks are little-endian, non-zero if big-endian:
	              // for 16 consecutive bits abcdefghijklmnop, big-endian will be
	              // consecutive bytes [abcdefgh],[ijklmnop], little-endian [ijklmnop],[abcdefgh]

	ULONG fullbits; // nonzero if 'full bits', i.e. new bitword in depacker fetched after last bit consumed
	                // zero if 'empty bits', new bitword fetched when bit is needed but no ones left

	ULONG maxwin; // maximum window (or lookback), positive number, which is one of:
	             // 256,512,1024,2048,4096,4352,8192,16384,32768 or 65536

	char * fname_in;
	char * fname_out;

	FILE * file_in;
	FILE * file_out;

	UBYTE * indata;
	ULONG inlen;
};


extern struct globals wrk;



void init_globals(void);
void free_globals(void);



#endif

