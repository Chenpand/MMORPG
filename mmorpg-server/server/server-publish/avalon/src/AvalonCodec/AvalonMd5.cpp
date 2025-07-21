#include "AvalonMd5.h"

namespace Avalon
{
	typedef UInt8* BYTE_POINTER;

	typedef struct {
		UInt32 state[4];             // state (abcd) 
		UInt32 count[2];             // number of bits, modulo 2^64 (lsb first) 
		unsigned char buffer[64];   // input buffer
	}Md5_ctx;

	// constants for md5transform routine.
	#define s11 7
	#define s12 12
	#define s13 17
	#define s14 22
	#define s21 5
	#define s22 9
	#define s23 14
	#define s24 20
	#define s31 4
	#define s32 11
	#define s33 16
	#define s34 23
	#define s41 6
	#define s42 10
	#define s43 15
	#define s44 21

	// f, g, h and i are basic md5 functions.
	#define f(x, y, z) (((x) & (y)) | ((~x) & (z)))
	#define g(x, y, z) (((x) & (z)) | ((y) & (~z)))
	#define h(x, y, z) ((x) ^ (y) ^ (z))
	#define i(x, y, z) ((y) ^ ((x) | (~z)))

	// rotate_left rotates x left n bits.
	#define rotate_left(x, n) (((x) << (n)) | ((x) >> (32-(n))))

	// ff, gg, hh, and ii transformations for rounds 1, 2, 3, and 4.
	//rotation is separate from addition to prevent recomputation.
	#define ff(a, b, c, d, x, s, ac) { \
			(a) += f ((b), (c), (d)) + (x) + (UInt32)(ac); \
			(a) = rotate_left ((a), (s)); \
			(a) += (b); \
			}
	#define gg(a, b, c, d, x, s, ac) { \
			(a) += g ((b), (c), (d)) + (x) + (UInt32)(ac); \
			(a) = rotate_left ((a), (s)); \
			(a) += (b); \
	}
	#define hh(a, b, c, d, x, s, ac) { \
			(a) += h ((b), (c), (d)) + (x) + (UInt32)(ac); \
			(a) = rotate_left ((a), (s)); \
			(a) += (b); \
	}
	#define ii(a, b, c, d, x, s, ac) { \
			(a) += i ((b), (c), (d)) + (x) + (UInt32)(ac); \
			(a) = rotate_left ((a), (s)); \
			(a) += (b); \
	}

	unsigned char md5_padding[64] = {
		0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
	};

	// md5 initialization. begins an md5 operation, writing a new context.
	void md5_init (Md5_ctx *context)
	{
		context->count[0] = context->count[1] = 0;
		// load magic initialization constants.
		context->state[0] = 0x67452301;
		context->state[1] = 0xefcdab89;
		context->state[2] = 0x98badcfe;
		context->state[3] = 0x10325476;
	}

	void md5_transform (UInt32 state[4], unsigned char block[64]);
	// md5 block update operation. continues an md5 message-digest
	//operation, processing another message block, and updating the
	//context.
	void md5_update (Md5_ctx *context, unsigned char *input, unsigned int inputlen)
	{
		unsigned int i, index, partlen;

		// compute number of bytes mod 64
		index = (unsigned int)((context->count[0] >> 3) & 0x3f);

		// update number of bits
		if ((context->count[0] += ((UInt32)inputlen << 3)) < ((UInt32)inputlen << 3))
			context->count[1]++;
		context->count[1] += ((UInt32)inputlen >> 29);

		partlen = 64 - index;

		// transform as many times as possible.
		if (inputlen >= partlen) {
			memcpy((BYTE_POINTER)&context->buffer[index], (BYTE_POINTER)input, partlen);
			md5_transform (context->state, context->buffer);
			for (i = partlen; i + 63 < inputlen; i += 64)
				md5_transform (context->state, &input[i]);

			index = 0;
		}
		else
			i = 0;

		// buffer remaining input
		memcpy((BYTE_POINTER)&context->buffer[index], (BYTE_POINTER)&input[i],inputlen-i);
	}

	void md5_doencode (unsigned char *output, UInt32 *input, unsigned int len);
	void md5_dodecode (UInt32 *output, unsigned char *input, unsigned int len);

	// md5 finalization. ends an md5 message-digest operation, writing the
	//the message digest and zeroizing the context.
	void md5_final (unsigned char digest[16], Md5_ctx *context)
	{
		unsigned char bits[8];
		unsigned int index, padlen;

		// save number of bits 
		md5_doencode (bits, context->count, 8);

		// pad out to 56 mod 64.
		index = (unsigned int)((context->count[0] >> 3) & 0x3f);
		padlen = (index < 56) ? (56 - index) : (120 - index);
		md5_update (context, md5_padding, padlen);

		// append length (before padding)
		md5_update (context, bits, 8);
		// store state in digest
		md5_doencode (digest, context->state, 16);

		// zeroize sensitive information.
		memset ((BYTE_POINTER)context, 0, sizeof (*context));
	}

	/////////////////////////////////////////////
	// md5 basic transformation. transforms state based on block.
	void md5_transform (UInt32 state[4], unsigned char block[64])
	{
		UInt32 a = state[0], b = state[1], c = state[2], d = state[3], x[16];

		md5_dodecode (x, block, 64);

		// round 1
		ff (a, b, c, d, x[ 0], s11, 0xd76aa478); // 1 
		ff (d, a, b, c, x[ 1], s12, 0xe8c7b756); // 2 
		ff (c, d, a, b, x[ 2], s13, 0x242070db); // 3 
		ff (b, c, d, a, x[ 3], s14, 0xc1bdceee); // 4 
  		ff (a, b, c, d, x[ 4], s11, 0xf57c0faf); // 5 
      	ff (d, a, b, c, x[ 5], s12, 0x4787c62a); // 6 
   		ff (c, d, a, b, x[ 6], s13, 0xa8304613); // 7 
       	ff (b, c, d, a, x[ 7], s14, 0xfd469501); // 8 
    	ff (a, b, c, d, x[ 8], s11, 0x698098d8); // 9 
		ff (d, a, b, c, x[ 9], s12, 0x8b44f7af); // 10 
		ff (c, d, a, b, x[10], s13, 0xffff5bb1); // 11 
		ff (b, c, d, a, x[11], s14, 0x895cd7be); // 12 
    	ff (a, b, c, d, x[12], s11, 0x6b901122); // 13 
		ff (d, a, b, c, x[13], s12, 0xfd987193); // 14 
		ff (c, d, a, b, x[14], s13, 0xa679438e); // 15 
		ff (b, c, d, a, x[15], s14, 0x49b40821); // 16 
		// round 2 
		gg (a, b, c, d, x[ 1], s21, 0xf61e2562); // 17 
		gg (d, a, b, c, x[ 6], s22, 0xc040b340); // 18 
		gg (c, d, a, b, x[11], s23, 0x265e5a51); // 19 
		gg (b, c, d, a, x[ 0], s24, 0xe9b6c7aa); // 20 
		gg (a, b, c, d, x[ 5], s21, 0xd62f105d); // 21 
		gg (d, a, b, c, x[10], s22,  0x2441453); // 22 
		gg (c, d, a, b, x[15], s23, 0xd8a1e681); // 23 
		gg (b, c, d, a, x[ 4], s24, 0xe7d3fbc8); // 24 
		gg (a, b, c, d, x[ 9], s21, 0x21e1cde6); // 25 
		gg (d, a, b, c, x[14], s22, 0xc33707d6); // 26 
		gg (c, d, a, b, x[ 3], s23, 0xf4d50d87); // 27 
		gg (b, c, d, a, x[ 8], s24, 0x455a14ed); // 28 
		gg (a, b, c, d, x[13], s21, 0xa9e3e905); // 29 
		gg (d, a, b, c, x[ 2], s22, 0xfcefa3f8); // 30 
		gg (c, d, a, b, x[ 7], s23, 0x676f02d9); // 31 
		gg (b, c, d, a, x[12], s24, 0x8d2a4c8a); // 32 
		// round 3 
		hh (a, b, c, d, x[ 5], s31, 0xfffa3942); // 33 
		hh (d, a, b, c, x[ 8], s32, 0x8771f681); // 34 
		hh (c, d, a, b, x[11], s33, 0x6d9d6122); // 35 
		hh (b, c, d, a, x[14], s34, 0xfde5380c); // 36 
		hh (a, b, c, d, x[ 1], s31, 0xa4beea44); // 37 
		hh (d, a, b, c, x[ 4], s32, 0x4bdecfa9); // 38 
		hh (c, d, a, b, x[ 7], s33, 0xf6bb4b60); // 39 
		hh (b, c, d, a, x[10], s34, 0xbebfbc70); // 40 
		hh (a, b, c, d, x[13], s31, 0x289b7ec6); // 41 
		hh (d, a, b, c, x[ 0], s32, 0xeaa127fa); // 42 
		hh (c, d, a, b, x[ 3], s33, 0xd4ef3085); // 43 
		hh (b, c, d, a, x[ 6], s34,  0x4881d05); // 44 
		hh (a, b, c, d, x[ 9], s31, 0xd9d4d039); // 45 
		hh (d, a, b, c, x[12], s32, 0xe6db99e5); // 46 
		hh (c, d, a, b, x[15], s33, 0x1fa27cf8); // 47 
		hh (b, c, d, a, x[ 2], s34, 0xc4ac5665); // 48 

		// round 4 
		ii (a, b, c, d, x[ 0], s41, 0xf4292244); // 49 
		ii (d, a, b, c, x[ 7], s42, 0x432aff97); // 50 
		ii (c, d, a, b, x[14], s43, 0xab9423a7); // 51 
		ii (b, c, d, a, x[ 5], s44, 0xfc93a039); // 52 
		ii (a, b, c, d, x[12], s41, 0x655b59c3); // 53 
		ii (d, a, b, c, x[ 3], s42, 0x8f0ccc92); // 54 
		ii (c, d, a, b, x[10], s43, 0xffeff47d); // 55 
		ii (b, c, d, a, x[ 1], s44, 0x85845dd1); // 56 
		ii (a, b, c, d, x[ 8], s41, 0x6fa87e4f); // 57 
		ii (d, a, b, c, x[15], s42, 0xfe2ce6e0); // 58 
		ii (c, d, a, b, x[ 6], s43, 0xa3014314); // 59 
		ii (b, c, d, a, x[13], s44, 0x4e0811a1); // 60 
		ii (a, b, c, d, x[ 4], s41, 0xf7537e82); // 61 
			ii (d, a, b, c, x[11], s42, 0xbd3af235); // 62 
		ii (c, d, a, b, x[ 2], s43, 0x2ad7d2bb); // 63 
		ii (b, c, d, a, x[ 9], s44, 0xeb86d391); // 64 
		
		state[0] += a;
		state[1] += b;
		state[2] += c;
       	state[3] += d;
		
		// zeroize sensitive information.    
		memset ((BYTE_POINTER)x, 0, sizeof (x));
	}

	// encodes input (UInt32) into output (unsigned char). assumes len is
	//a multiple of 4.
	void md5_doencode (unsigned char *output, UInt32 *input, unsigned int len)
	{
		unsigned int i, j;

		for (i = 0, j = 0; j < len; i++, j += 4) {
			output[j] = (unsigned char)(input[i] & 0xff);
			output[j+1] = (unsigned char)((input[i] >> 8) & 0xff);
			output[j+2] = (unsigned char)((input[i] >> 16) & 0xff);
			output[j+3] = (unsigned char)((input[i] >> 24) & 0xff);
		}
	}

	// decodes input (unsigned char) into output (UInt32). assumes len is
	//a multiple of 4.
	void md5_dodecode (UInt32 *output, unsigned char *input, unsigned int len)
	{
		unsigned int i, j;

		for (i = 0, j = 0; j < len; i++, j += 4)
		output[i] = ((UInt32)input[j]) | (((UInt32)input[j+1]) << 8) | 
			(((UInt32)input[j+2]) << 16) | (((UInt32)input[j+3]) << 24);
	}

	void md5_encode(char digest[16],  void *input, unsigned int inputlen)
	{
		Md5_ctx context;

		md5_init (&context);
		md5_update (&context, (unsigned char *)input, inputlen);
		md5_final ((unsigned char*)digest, &context);
	}


	inline char to_hexchar(int val)
	{
		if(val < 10) return char('0' + val) ;
		else return char ('a' + val - 10);
	}

	void md5_tohex(char src[16], char dest[32])
	{
		for(int i = 0; i < 16; ++i)
		{
			unsigned c = *(unsigned char*)(src+i);
			dest[i*2] = to_hexchar(c >> 4);
			dest[i*2 + 1] = to_hexchar(c & 0x0F);
		}
	}

	void md5_encode32(char dest[32], void* data, unsigned int len)
	{
		char md5[16];
		md5_encode(md5, data, len);
		md5_tohex(md5, dest);
	}
}
