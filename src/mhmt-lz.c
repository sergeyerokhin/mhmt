#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mhmt-types.h"

#include "mhmt-lz.h"
#include "mhmt-tb.h"
#include "mhmt-globals.h"

// MegaLZ search function
//
void make_lz_codes_megalz(ULONG position, ULONG actual_len, UBYTE * hash, struct lzcode * codes)
{
	ULONG codepos;
	ULONG codelen,i;
	ULONG was_match;
	UBYTE curr_byte,next_byte;
	struct tb_chain * curr_tb;
	UWORD index;
	ULONG max_lookback;

	// copy-byte code is always present
	codes[0].length = 1;
	codes[0].disp   = 0;

	// start more filling of codes[] from that position
	codepos = 1;


	curr_byte=wrk.indata[position];

	// check for one-byter (-1..-8)
	i = (position>8) ? position-8 : 0;
	do
	{
		if( wrk.indata[i] == curr_byte )
		{
			codes[codepos].length = 1;
			codes[codepos].disp   = -(LONG)(position-i);
			codepos++;
			break;
		}
	} while( (++i)<position );



	max_lookback = (wrk.maxwin<4352) ? wrk.maxwin : 4352;

	// check for two-byter (-1..-256), no check for wrk.maxwin since it can't be less than 256
	//
	curr_tb = NULL;
	//
	if( position<(actual_len-1) ) // don't try two-byter if we are at the byte before last one
	{
		next_byte = wrk.indata[position+1];
		index=(curr_byte<<8) + next_byte;
		curr_tb = tb_entry[index];

		// there is two-byters!
		if( curr_tb )
		{
                        if( ((position-curr_tb->pos)<=256) && ((position-curr_tb->pos)<=max_lookback) ) // 2byters are no longer than 256 bytes, as well as no lookbacks (wrk.maxwin)
                                                             // lesser than 256
			{
				codes[codepos].length = 2;
				codes[codepos].disp   = -(LONG)(position - curr_tb->pos);
				codepos++;
			}
		}
	}

	// at last, check for lengths=3..255 up to 4352 or wrk.maxwin
	//
	//
	if(  curr_tb  &&  ( (position-curr_tb->pos)<=max_lookback )  &&  ( position<(actual_len-2) )  ) // if we can proceed at all
	{
		was_match = 1; // there was match at codelen-1

		for( codelen=3; ( codelen<=255 )&&( position<(actual_len-codelen+1) ); /*nothing*/ )
		{
			if( was_match ) // for codelen-1
			{
				// codelen-1 bytes are matched, compare one more byte
				if( wrk.indata[position+codelen-1] == wrk.indata[curr_tb->pos+codelen-1] )
				{
					// add code to the table
					codes[codepos].length = codelen;
					codes[codepos].disp   = -(LONG)(position - curr_tb->pos);
					codepos++;

					codelen++; // next time do comparision of greater size
				}
				else // last bytes do not match
				{

MATCH_FAIL_MEGALZ: // entrance for failed matches here: used 3-fold so we set "goto" here

					// go for older twobyter
					curr_tb = curr_tb->next;

					// no more twobyters or they are too far - stop search at all
					if( !curr_tb ) break;
					if( (position - curr_tb->pos)>max_lookback ) break;

					// mark there was no matches
					was_match = 0;
				}
			}
			else // there were no matches for previous codelen
			{
				// next twobyter is already taken, but no comparision is done for codelen bytes
				// first we check if we need to do such comparision at all by seeing to the hashes of the ends of strings
				if( hash[position+codelen-1] == hash[curr_tb->pos+codelen-1] )
				{	// hashes match, so try matching complete string
					if( !memcmp( &wrk.indata[position], &wrk.indata[curr_tb->pos], codelen ) )
					{
						was_match = 1;
						codes[codepos].length = codelen;
						codes[codepos].disp   = -(LONG)(position - curr_tb->pos);
						codepos++;

						codelen++;
					}
					else
						// no match of whole string
						goto MATCH_FAIL_MEGALZ;
				}
				else
					// no match of hashes
					goto MATCH_FAIL_MEGALZ;
			}
		}
	}


	// here we assume to have found all possible matches. check for codes[] table overflow:
	// there could be matches for length 1..255, and there is copy-byte, total 256 entries
	if( codepos>256 ) // this should not happen!
	{
		printf("mhmt-lz.c:make_lz_codes_megalz() encountered too many entries in codes[] table. Fatal error.\n");
		exit(1);
	}

	// mark end-of-records in codes[]
	codes[codepos].length = 0;
	codes[codepos].disp   = 0;
}







// Hrum search function
//
void make_lz_codes_hrum(ULONG position, ULONG actual_len, UBYTE * hash, struct lzcode * codes)
{
	ULONG codepos;
	ULONG codelen,i;
	ULONG was_match;
	UBYTE curr_byte,next_byte;
	struct tb_chain * curr_tb;
	UWORD index;
	ULONG max_lookback;

	// copy-byte code is always present
	codes[0].length = 1;
	codes[0].disp   = 0;

	// start more filling of codes[] from that position
	codepos = 1;

	curr_byte=wrk.indata[position];

	// check for one-byter (-1..-8)
	i = (position>8) ? position-8 : 0;
	do
	{
		if( wrk.indata[i] == curr_byte )
		{
			codes[codepos].length = 1;
			codes[codepos].disp   = -(LONG)(position-i);
			codepos++;
			break;
		}
	} while( (++i)<position );


	max_lookback = (wrk.maxwin<4096) ? wrk.maxwin : 4096;


	// check for two-byter (-1..-256)
	//
	curr_tb = NULL;
	//
	if( position<(actual_len-1) ) // don't try two-byter if we are at the byte before last one
	{
		next_byte = wrk.indata[position+1];
		index=(curr_byte<<8) + next_byte;
		curr_tb = tb_entry[index];

		// there is two-byters!
		if( curr_tb )
		{
			if( ((position-curr_tb->pos)<=256) && ((position-curr_tb->pos)<=max_lookback) ) // 2byters are no longer than 256 bytes, as well as no lookbacks (wrk.maxwin)
			                                                                                // lesser than 256
			{
				codes[codepos].length = 2;
				codes[codepos].disp   = -(LONG)(position - curr_tb->pos);
				codepos++;
			}
		}
	}


	// at last, check for lengths=3..255 up to 4096 or wrk.maxwin
	if(  curr_tb  &&  ( (position-curr_tb->pos)<=max_lookback )  &&  ( position<(actual_len-2) )  ) // if we can proceed at all
	{
		was_match = 1; // there was match at codelen-1

		for( codelen=3; ( codelen<=255 )&&( position<(actual_len-codelen+1) ); /*nothing*/ )
		{
			if( was_match ) // for codelen-1
			{
				// codelen-1 bytes are matched, compare one more byte
				if( wrk.indata[position+codelen-1] == wrk.indata[curr_tb->pos+codelen-1] )
				{
					// add code to the table
					codes[codepos].length = codelen;
					codes[codepos].disp   = -(LONG)(position - curr_tb->pos);
					codepos++;

					codelen++; // next time do comparision of greater size
				}
				else // last bytes do not match
				{

MATCH_FAIL_HRUM: // entrance for failed matches here: used 3-fold so we set "goto" here

					// go for older twobyter
					curr_tb = curr_tb->next;

					// no more twobyters or they are too far - stop search at all
					if( !curr_tb ) break;
					if( (position - curr_tb->pos)>max_lookback ) break;

					// mark there was no matches
					was_match = 0;
				}
			}
			else // there were no matches for previous codelen
			{
				// next twobyter is already taken, but no comparision is done for codelen bytes
				// first we check if we need to do such comparision at all by seeing to the hashes of the ends of strings
				if( hash[position+codelen-1] == hash[curr_tb->pos+codelen-1] )
				{	// hashes match, so try matching complete string
					if( !memcmp( &wrk.indata[position], &wrk.indata[curr_tb->pos], codelen ) )
					{
						was_match = 1;
						codes[codepos].length = codelen;
						codes[codepos].disp   = -(LONG)(position - curr_tb->pos);
						codepos++;

						codelen++;
					}
					else
						// no match of whole string
						goto MATCH_FAIL_HRUM;
				}
				else
					// no match of hashes
					goto MATCH_FAIL_HRUM;
			}
		}
	}


	// here we assume to have found all possible matches. check for codes[] table overflow:
	// there could be matches for length 1..255, and there is copy-byte, total 256 entries
	if( codepos>256 ) // this should not happen!
	{
		printf("mhmt-lz.c:make_lz_codes_hrum() encountered too many entries in codes[] table. Fatal error.\n");
		exit(1);
	}

	// mark end-of-records in codes[]
	codes[codepos].length = 0;
	codes[codepos].disp   = 0;

}




























// Hrust search function
//
void make_lz_codes_hrust(ULONG position, ULONG actual_len, UBYTE * hash, struct lzcode * codes)
{
	ULONG codepos;
	ULONG codelen,i;
	ULONG was_match;
	UBYTE curr_byte,next_byte;
	struct tb_chain * curr_tb;
	UWORD index;
	ULONG max_lookback;

	// copy-byte code is always present
	codes[0].length = 1;
	codes[0].disp   = 0;

	// start more filling of codes[] from that position
	codepos = 1;

	// add 12,14,16,...,40,42 bytes copies, if possible
	for(codelen=12;codelen<=42;codelen+=2)
	{
		if( position <= (actual_len-codelen) )
		{
			codes[codepos].length = codelen;
			codes[codepos].disp   = 0;
			codepos++;
		}
		else
			break;
	}

	// check for one-byter (-1..-8)
	//
	curr_byte=wrk.indata[position];
	//
	i = (position>8) ? position-8 : 0;
	do
	{
		if( wrk.indata[i] == curr_byte )
		{
			codes[codepos].length = 1;
			codes[codepos].disp   = -(LONG)(position-i);
			codepos++;
			break;
		}
	} while( (++i)<position );




	// check 3-byte insertion code (-1..-79)
	if( position < (actual_len-2) )
	{
		i = (position>79) ? position-79 : 0;
		do
		{
			if( (wrk.indata[i]==curr_byte) && (wrk.indata[i+2]==wrk.indata[position+2]) )
			{
				codes[codepos].length = (-3);
				codes[codepos].disp   = -(LONG)(position-i);
				codepos++;
				break;
			}
		} while( (++i)<position );
	}


	max_lookback = (wrk.maxwin<65536) ? wrk.maxwin : 65536;

	// check for two-byter (-1..!-768!)
	//
	curr_tb = NULL;
	//
	if( position<(actual_len-1) ) // don't try two-byter if we are at the byte before last one
	{
		next_byte = wrk.indata[position+1];
		index=(curr_byte<<8) + next_byte;
		curr_tb = tb_entry[index];

		// there are two-byters!
		if( curr_tb )
		{
			if( ((position-curr_tb->pos)<=768) && ((position-curr_tb->pos)<=max_lookback) )
			{
				codes[codepos].length = 2;
				codes[codepos].disp   = -(LONG)(position - curr_tb->pos);
				codepos++;
			}
		}
	}


	// at last, check for lengths=3..3839 up to lookback 65536 or wrk.maxwin
	if(  curr_tb  &&  ( (position-curr_tb->pos)<=max_lookback )  &&  ( position<(actual_len-2) )  ) // if we can proceed at all
	{
		was_match = 1; // there was match at codelen-1

		for( codelen=3; ( codelen<=3839 )&&( position<(actual_len-codelen+1) ); /*nothing*/ )
		{
			if( was_match ) // for codelen-1
			{
				// codelen-1 bytes are matched, compare one more byte
				if( wrk.indata[position+codelen-1] == wrk.indata[curr_tb->pos+codelen-1] )
				{
					// add code to the table
					codes[codepos].length = codelen;
					codes[codepos].disp   = -(LONG)(position - curr_tb->pos);
					codepos++;

					codelen++; // next time do comparision of greater size
				}
				else // last bytes do not match
				{

MATCH_FAIL_HRUST: // entrance for failed matches here: used 3-fold so we set "goto" here

					// go for older twobyter
					curr_tb = curr_tb->next;

					// no more twobyters or they are too far - stop search at all
					if( !curr_tb ) break;
					if( (position - curr_tb->pos)>max_lookback ) break;

					// mark there was no matches
					was_match = 0;
				}
			}
			else // there were no matches for previous codelen
			{
				// next twobyter is already taken, but no comparision is done for codelen bytes
				// first we check if we need to do such comparision at all by seeing to the hashes of the ends of strings
				if( hash[position+codelen-1] == hash[curr_tb->pos+codelen-1] )
				{	// hashes match, so try matching complete string
					if( !memcmp( &wrk.indata[position], &wrk.indata[curr_tb->pos], codelen ) )
					{
						was_match = 1;
						codes[codepos].length = codelen;
						codes[codepos].disp   = -(LONG)(position - curr_tb->pos);
						codepos++;

						codelen++;
					}
					else
						// no match of whole string
						goto MATCH_FAIL_HRUST;
				}
				else
					// no match of hashes
					goto MATCH_FAIL_HRUST;
			}
		}
	}

	// here we assume to have found all possible matches. check for codes[] table overflow:
	// there could be matches for length 1..3839, and there is copy-1-byte, 16 copymanybyters, 1 insertion match, total 3857 entries
	if( codepos>3857 ) // this should not happen!
	{
		printf("mhmt-lz.c:make_lz_codes_hrust() encountered too many entries in codes[] table. Fatal error.\n");
		exit(1);
	}

	// mark end-of-records in codes[]
	codes[codepos].length = 0;
	codes[codepos].disp   = 0;
}
























// returns price in bits or zero if error
//
ULONG get_lz_price_megalz(ULONG position, struct lzcode * lzcode)
{
	ULONG varbits,varlen;
	LONG length,disp;

	length = lzcode->length;
	disp   = lzcode->disp;

	if( length==1 )
	{
		if( disp==0 )
			return 9;
		else if( (-8)<=disp && disp<=(-1) )
			return 6;
		else
			goto INVALID_CODE_MEGALZ;
	}
	else if( length==2 )
	{
		if( (-256)<=disp && disp<=(-1) )
			return 11;
		else
			goto INVALID_CODE_MEGALZ;
	}
	else if( length==3 )
	{
		if( (-256)<=disp && disp<=(-1) )
			return 12;
		else if( (-4352)<=disp && disp<(-256) )
			return 16;
		else
			goto INVALID_CODE_MEGALZ;
	}
	else if( 4<=length && length<=255 )
	{
		varlen = 0;
		varbits = (length-2)>>1;
		while( varbits )
		{
			varbits >>= 1;
			varlen+=2;
		}

		if( (-256)<=disp && disp<=(-1) )
			varlen += 9;
		else if( (-4352)<=disp && disp<(-256) )
			varlen += 13;
		else
			goto INVALID_CODE_MEGALZ;

		return varlen+3;
	}
	else
	{
INVALID_CODE_MEGALZ:
		printf("mhmt-lz.c:get_lz_price_megalz(): Found invalid code length=%d, displacement=%d\n",length, disp);
		return 0;
	}
}


ULONG get_lz_price_hrum(ULONG position, struct lzcode * lzcode)
{
	ULONG varlen;
	LONG length,disp;

	length = lzcode->length;
	disp   = lzcode->disp;

	if( length==1 )
	{
		if( disp==0 )
			return 9;
		else if( (-8)<=disp && disp<=(-1) )
			return 6;
		else
			goto INVALID_CODE_HRUM;
	}
	else if( length==2 )
	{
		if( (-256)<=disp && disp<=(-1) )
			return 11;
		else
			goto INVALID_CODE_HRUM;
	}
	else if( length==3 )
	{
		if( (-256)<=disp && disp<=(-1) )
			return 11;
		else if( (-4096)<=disp && disp<(-256) )
			return 26;
		else
			goto INVALID_CODE_HRUM;
	}
	else if (4<=length && length<=15)
	{
		varlen = 5;
		if( length>=6 ) varlen += 2;
		if( length>=9 ) varlen += 2;
		if( length>=12) varlen += 2;

		if( (-256)<=disp && disp<=(-1) )
			varlen += 9;
		else if( (-4096)<=disp && disp<(-256) )
			varlen += 13;
		else
			goto INVALID_CODE_HRUM;

		return varlen;
	}
	else if( 15<length && length<=255 )
	{
		if( (-256)<=disp && disp<=(-1) )
			return 22;
		else if( (-4096)<=disp && disp<(-256) )
			return 26;
		else
			goto INVALID_CODE_HRUM;
	}
	else
	{
INVALID_CODE_HRUM:
		printf("mhmt-lz.c:get_lz_price_hrum(): Found invalid code length=%d, displacement=%d\n",length, disp);
		return 0;
	}
}











ULONG get_lz_price_hrust(ULONG position, struct lzcode * lzcode)
{
	ULONG varbits,varlen;
	LONG length,disp,tmp;

	length = lzcode->length;
	disp   = lzcode->disp;


	if( disp==0 )
	{
		if( length==1 )
		{
			return 9; // copy-1-byte
		}
		else if( (12<=length) && (length<=42) && ( !(length&1) ) )
		{
			return 11 + 8*length;
		}
		else
			goto INVALID_CODE_HRUST;
	}
	else if( length==(-3) ) // insertion match!
	{
		if( (-16)<=disp && disp<=(-1) )
		{
			return 10+8;
		}
		else if( (-79)<=disp && disp<(-16) )
		{
			return 5+8+8;
		}
		else
			goto INVALID_CODE_HRUST;
	}
	else if( length==1 )
	{
		if( (-8)<=disp && disp<=(-1) )
			return 6;
		else
			goto INVALID_CODE_HRUST;
	}
	else if( length==2 )
	{
		if( (-32)<=disp && disp<=(-1) )
		{
			return 10;
		}
		else if( (-768)<=disp && disp<(-32) )
		{
			return 13;
		}
		else
			goto INVALID_CODE_HRUST;
	}
	else if (3<=length && length<=3839 && (-65536)<=disp && disp<=(-1) )
	{
		// first, calc influence of length
		if( length<=15 ) // 3..15
		{
			varlen = 3 + ( (length/3)<<1 );

			if( length==3 )  varlen = 3;

			if( length==15 ) varlen = 11;
		}
		else if( length<=127 ) // 16..127
		{
			varlen = 14;
		}
		else // 128..3839
		{
			varlen = 14+8;
		}


		// add displacement length
		if( (-32)<=disp ) // ffe0..ffff
		{
			varlen += 7;
		}
		else if( (-512)<=disp ) // fe00..ffdf
		{
			varlen += 10;
		}
		else // 0000(-65536)..fdff: -513:-1024, -1025:-2048, -2049:-4096, ... ,-32769:-65536
		{    // bits:                   12           13           14               18

			varlen += 12;

			if( position>32768 )
			{
				varlen += 6; // 8bits
			}
			else
			{
				tmp = 1024;

				while( position>tmp )
				{
					varlen++;

					tmp <<= 1;
				}
			}
		}

		return varlen;
	}
	else
	{
INVALID_CODE_HRUST:
		printf("mhmt-lz.c:get_lz_price_hrust(): Found invalid code length=%d, displacement=%d\n",length, disp);
		return 0;
	}
}

