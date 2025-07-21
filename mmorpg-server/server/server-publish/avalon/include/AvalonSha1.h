#ifndef _AVALON_SHA1_H_
#define _AVALON_SHA1_H_

#include "AvalonDefine.h"
#include <vector>
#include <string>

namespace Avalon
{
	class SHA1Engine
	{
	public:
		typedef std::vector<unsigned char> Digest;

		enum
		{
			BLOCK_SIZE  = 64,
			DIGEST_SIZE = 20
		};

	public:
		SHA1Engine();
		~SHA1Engine();

		void update(const void* data, unsigned length){ updateImpl(data, length); }
		void update(char data){ updateImpl(&data, 1); }
		void update(const std::string& data){ updateImpl(data.data(), (unsigned) data.size()); }
		
		unsigned digestLength() const;
		void reset();
		const Digest& digest();

		static std::string digestToHex(const Digest& bytes);

	protected:
		void updateImpl(const void* data, unsigned length);

	private:
		void transform();
		static void byteReverse(UInt32* buffer, int byteCount);

		typedef UInt8 BYTE;

		struct Context
		{
			UInt32 digest[5]; // Message digest
			UInt32 countLo;   // 64-bit bit count
			UInt32 countHi;
			UInt32 data[16];  // SHA data buffer
			UInt32 slop;      // # of bytes saved in data[]
		};

		Context _context;
		Digest _digest;

		SHA1Engine(const SHA1Engine&);
		SHA1Engine& operator = (const SHA1Engine&);
	};
}

#endif
