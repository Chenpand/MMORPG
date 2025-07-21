#include "AvalonBase64.h"

namespace Avalon
{
	int base64_encode(char* dst, int dstLen, const void* src_, int srcLen)
	{
		if(src_	== 0 || dst == 0) return -1;

	    const unsigned char *src = static_cast<const unsigned char *>(src_);

		static const char b64[] =
			"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";


		int encLen = 0;

		// encode blocks of 3 bytes into 4 base64 characters
		for ( ; srcLen >= 3; srcLen -= 3, src += 3 )
		{
			encLen += 4;
			if ( dst )
			{
				if ( encLen > dstLen )
					return -1;

				*dst++ = b64[src[0] >> 2];
				*dst++ = b64[((src[0] & 0x03) << 4) | ((src[1] & 0xf0) >> 4)];
				*dst++ = b64[((src[1] & 0x0f) << 2) | ((src[2] & 0xc0) >> 6)];
				*dst++ = b64[src[2] & 0x3f];
			}
		}

		// finish with the remaining characters
		if ( srcLen )
		{
			encLen += 4;
			if ( dst )
			{
				if ( encLen > dstLen )
					return -1;

				// we have definitely one and maybe two bytes remaining
				unsigned char next = srcLen == 2 ? src[1] : 0;
				*dst++ = b64[src[0] >> 2];
				*dst++ = b64[((src[0] & 0x03) << 4) | ((next & 0xf0) >> 4)];
				*dst++ = srcLen == 2 ? b64[((next & 0x0f) << 2)] : '=';
				*dst++ = '=';
			}
		}
		*dst++ = 0; 

		return encLen;
	}

	int base64_decode(char* dst_, int dstLen, const char* src, int srcLen)
	{
		if(src == 0 || dst_ == 0) return -1;

	    unsigned char *dst = (unsigned char *)dst_;

		int decLen = 0;

		// this table contains the values, in base 64, of all valid characters and
		// special values WSP or INV for white space and invalid characters
		// respectively as well as a special PAD value for '='
		enum
		{
			WSP = 200,
			INV,
			PAD
		};

		static const unsigned char decode[256] =
		{
			WSP,INV,INV,INV,INV,INV,INV,INV,INV,WSP,WSP,INV,WSP,WSP,INV,INV,
			INV,INV,INV,INV,INV,INV,INV,INV,INV,INV,INV,INV,INV,INV,INV,INV,
			WSP,INV,INV,INV,INV,INV,INV,INV,INV,INV,INV,076,INV,INV,INV,077,
			064,065,066,067,070,071,072,073,074,075,INV,INV,INV,PAD,INV,INV,
			INV,000,001,002,003,004,005,006,007,010,011,012,013,014,015,016,
			017,020,021,022,023,024,025,026,027,030,031,INV,INV,INV,INV,INV,
			INV,032,033,034,035,036,037,040,041,042,043,044,045,046,047,050,
			051,052,053,054,055,056,057,060,061,062,063,INV,INV,INV,INV,INV,
			INV,INV,INV,INV,INV,INV,INV,INV,INV,INV,INV,INV,INV,INV,INV,INV,
			INV,INV,INV,INV,INV,INV,INV,INV,INV,INV,INV,INV,INV,INV,INV,INV,
			INV,INV,INV,INV,INV,INV,INV,INV,INV,INV,INV,INV,INV,INV,INV,INV,
			INV,INV,INV,INV,INV,INV,INV,INV,INV,INV,INV,INV,INV,INV,INV,INV,
			INV,INV,INV,INV,INV,INV,INV,INV,INV,INV,INV,INV,INV,INV,INV,INV,
			INV,INV,INV,INV,INV,INV,INV,INV,INV,INV,INV,INV,INV,INV,INV,INV,
			INV,INV,INV,INV,INV,INV,INV,INV,INV,INV,INV,INV,INV,INV,INV,INV,
			INV,INV,INV,INV,INV,INV,INV,INV,INV,INV,INV,INV,INV,INV,INV,INV,
		};

		// we decode input by groups of 4 characters but things are complicated by
		// the fact that there can be whitespace and other junk in it too so keep
		// record of where exactly we're inside the current quartet in this var
		int n = 0;
		unsigned char in[4];                    // current quartet
		bool end = false;                       // set when we find padding
		int padLen = 0;                      // length lost to padding
		const char *p;
		for ( p = src; srcLen; p++, srcLen-- )
		{
			const unsigned char c = decode[static_cast<unsigned char>(*p)];
			switch ( c )
			{
				case WSP:
				case INV:
					// force the loop to stop and an error to be returned
					n = -1;
					srcLen = 1;
					break;

				case PAD:
					// set the flag telling us that we're past the end now
					end = true;

					// there can be either a single '=' at the end of a quartet or
					// "==" in positions 2 and 3
					if ( n == 3 )
					{
						padLen = 1;
						in[n++] = '\0';
					}
					else if ( (n == 2) && (--srcLen && *++p == '=') )
					{
						padLen = 2;
						in[n++] = '\0';
						in[n++] = '\0';
					}
					else // invalid padding
					{
						// force the loop terminate with an error
						n = -1;
						srcLen = 1;
					}
					break;

				default:
					if ( end )
					{
						// nothing is allowed after the end so provoke error return
						n = -1;
						srcLen = 1;
						break;
					}

					in[n++] = c;
			}

			if ( n == 4 )
			{
				// got entire block, decode
				decLen += 3 - padLen;
				if ( dst )
				{
					if ( decLen > dstLen )
						return -1;

					// undo the bit shifting done during encoding
					*dst++ = in[0] << 2 | in[1] >> 4;

					// be careful to not overwrite the output buffer with NUL pad
					// bytes
					if ( padLen != 2 )
					{
						*dst++ = in[1] << 4 | in[2] >> 2;
						if ( !padLen )
							*dst++ = in[2] << 6 | in[3];
					}
				}

				n = 0;
			}
		}

		if ( n )
		{
			return -1;
		}

		return decLen;
	}
};

