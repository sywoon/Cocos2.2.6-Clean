#include "ZipUtil.h"
#include "zlib.h"
#include "FileUtils.h"



NS_CC_BEGIN

#define BUFFER_INC_FACTOR (2)


unsigned int ZipUtils::s_uEncryptedPvrKeyParts[4] = { 0,0,0,0 };
unsigned int ZipUtils::s_uEncryptionKey[1024];
bool ZipUtils::s_bEncryptionKeyIsValid = false;


int ZipUtils::ccInflateMemory(unsigned char* in, unsigned int inLength, unsigned char** out)
{
	// 256k for hint
	return ccInflateMemoryWithHint(in, inLength, out, 256 * 1024);
}


int ZipUtils::ccInflateMemoryWithHint(unsigned char* in, unsigned int inLength, unsigned char** out, unsigned int outLengthHint)
{
	unsigned int outLength = 0;
	int err = ccInflateMemoryWithHint(in, inLength, out, &outLength, outLengthHint);

	if (err != Z_OK || *out == NULL) 
	{
		if (err == Z_MEM_ERROR)
		{
			CCLOG("cocos2d: ZipUtils: Out of memory while decompressing map data!");
		}
		else if (err == Z_VERSION_ERROR)
		{
			CCLOG("cocos2d: ZipUtils: Incompatible zlib version!");
		}
		else if (err == Z_DATA_ERROR)
		{
			CCLOG("cocos2d: ZipUtils: Incorrect zlib compressed data!");
		}
		else
		{
			CCLOG("cocos2d: ZipUtils: Unknown error while decompressing map data!");
		}

		delete[] * out;
		*out = NULL;
		outLength = 0;
	}

	return outLength;
}

int ZipUtils::ccInflateMemoryWithHint(unsigned char* in, unsigned int inLength, unsigned char** out, unsigned int* outLength, unsigned int outLenghtHint)
{
	/* ret value */
	int err = Z_OK;

	int bufferSize = outLenghtHint;
	*out = new unsigned char[bufferSize];

	z_stream d_stream; /* decompression stream */
	d_stream.zalloc = (alloc_func)0;
	d_stream.zfree = (free_func)0;
	d_stream.opaque = (voidpf)0;

	d_stream.next_in = in;
	d_stream.avail_in = inLength;
	d_stream.next_out = *out;
	d_stream.avail_out = bufferSize;

	/* window size to hold 256k */
	if ((err = inflateInit2(&d_stream, 15 + 32)) != Z_OK)
		return err;

	for (;;)
	{
		err = inflate(&d_stream, Z_NO_FLUSH);

		if (err == Z_STREAM_END)
		{
			break;
		}

		switch (err)
		{
		case Z_NEED_DICT:
			err = Z_DATA_ERROR;
		case Z_DATA_ERROR:
		case Z_MEM_ERROR:
			inflateEnd(&d_stream);
			return err;
		}

		// not enough memory ?
		if (err != Z_STREAM_END)
		{
			*out = (unsigned char*)realloc(*out, bufferSize * BUFFER_INC_FACTOR);

			/* not enough memory, ouch */
			if (!*out)
			{
				CCLOG("cocos2d: ZipUtils: realloc failed");
				inflateEnd(&d_stream);
				return Z_MEM_ERROR;
			}

			d_stream.next_out = *out + bufferSize;
			d_stream.avail_out = bufferSize;
			bufferSize *= BUFFER_INC_FACTOR;
		}
	}

	*outLength = bufferSize - d_stream.avail_out;
	err = inflateEnd(&d_stream);
	return err;
}


int ZipUtils::ccInflateGZipFile(const char* path, unsigned char** out)
{
	int len;
	unsigned int offset = 0;

	CCAssert(out, "");
	CCAssert(&*out, "");

	gzFile inFile = gzopen(path, "rb");
	if (inFile == NULL) {
		CCLOG("cocos2d: ZipUtils: error open gzip file: %s", path);
		return -1;
	}

	/* 512k initial decompress buffer */
	unsigned int bufferSize = 512 * 1024;
	unsigned int totalBufferSize = bufferSize;

	*out = (unsigned char*)malloc(bufferSize);
	if (!out)
	{
		CCLOG("cocos2d: ZipUtils: out of memory");
		return -1;
	}

	for (;;) {
		len = gzread(inFile, *out + offset, bufferSize);
		if (len < 0)
		{
			CCLOG("cocos2d: ZipUtils: error in gzread");
			free(*out);
			*out = NULL;
			return -1;
		}
		if (len == 0)
		{
			break;
		}

		offset += len;

		// finish reading the file
		if ((unsigned int)len < bufferSize)
		{
			break;
		}

		bufferSize *= BUFFER_INC_FACTOR;
		totalBufferSize += bufferSize;
		unsigned char* tmp = (unsigned char*)realloc(*out, totalBufferSize);

		if (!tmp)
		{
			CCLOG("cocos2d: ZipUtils: out of memory");
			free(*out);
			*out = NULL;
			return -1;
		}

		*out = tmp;
	}

	if (gzclose(inFile) != Z_OK)
	{
		CCLOG("cocos2d: ZipUtils: gzclose failed");
	}

	return offset;
}


int ZipUtils::ccInflateCCZFile(const char* path, unsigned char** out)
{
	CCAssert(out, "");
	CCAssert(&*out, "");

	// load file into memory
	unsigned char* compressed = NULL;

	unsigned long fileLen = 0;
	compressed = FileUtils::sharedFileUtils()->getFileData(path, "rb", &fileLen);

	if (NULL == compressed || 0 == fileLen)
	{
		CCLOG("cocos2d: Error loading CCZ compressed file");
		return -1;
	}

	struct CCZHeader* header = (struct CCZHeader*) compressed;

	// verify header
	if (header->sig[0] == 'C' && header->sig[1] == 'C' && header->sig[2] == 'Z' && header->sig[3] == '!')
	{
		// verify header version
		unsigned int version = CC_SWAP_INT16_BIG_TO_HOST(header->version);
		if (version > 2)
		{
			CCLOG("cocos2d: Unsupported CCZ header format");
			delete[] compressed;
			return -1;
		}

		// verify compression format
		if (CC_SWAP_INT16_BIG_TO_HOST(header->compression_type) != CCZ_COMPRESSION_ZLIB)
		{
			CCLOG("cocos2d: CCZ Unsupported compression method");
			delete[] compressed;
			return -1;
		}
	}
	else if (header->sig[0] == 'C' && header->sig[1] == 'C' && header->sig[2] == 'Z' && header->sig[3] == 'p')
	{
		// encrypted ccz file
		header = (struct CCZHeader*) compressed;

		// verify header version
		unsigned int version = CC_SWAP_INT16_BIG_TO_HOST(header->version);
		if (version > 0)
		{
			CCLOG("cocos2d: Unsupported CCZ header format");
			delete[] compressed;
			return -1;
		}

		// verify compression format
		if (CC_SWAP_INT16_BIG_TO_HOST(header->compression_type) != CCZ_COMPRESSION_ZLIB)
		{
			CCLOG("cocos2d: CCZ Unsupported compression method");
			delete[] compressed;
			return -1;
		}

		// decrypt
		unsigned int* ints = (unsigned int*)(compressed + 12);
		int enclen = (fileLen - 12) / 4;

		ccDecodeEncodedPvr(ints, enclen);

#if COCOS2D_DEBUG > 0
		// verify checksum in debug mode
		unsigned int calculated = ccChecksumPvr(ints, enclen);
		unsigned int required = CC_SWAP_INT32_BIG_TO_HOST(header->reserved);

		if (calculated != required)
		{
			CCLOG("cocos2d: Can't decrypt image file. Is the decryption key valid?");
			delete[] compressed;
			return -1;
		}
#endif
	}
	else
	{
		CCLOG("cocos2d: Invalid CCZ file");
		delete[] compressed;
		return -1;
	}

	unsigned int len = CC_SWAP_INT32_BIG_TO_HOST(header->len);

	*out = (unsigned char*)malloc(len);
	if (!*out)
	{
		CCLOG("cocos2d: CCZ: Failed to allocate memory for texture");
		delete[] compressed;
		return -1;
	}

	unsigned long destlen = len;
	unsigned long source = (unsigned long)compressed + sizeof(*header);
	int ret = uncompress(*out, &destlen, (Bytef*)source, fileLen - sizeof(*header));

	delete[] compressed;

	if (ret != Z_OK)
	{
		CCLOG("cocos2d: CCZ: Failed to uncompress data");
		free(*out);
		*out = NULL;
		return -1;
	}

	return len;
}


inline void ZipUtils::ccDecodeEncodedPvr(unsigned int* data, int len)
{
	const int enclen = 1024;
	const int securelen = 512;
	const int distance = 64;

	// check if key was set
	// make sure to call caw_setkey_part() for all 4 key parts
	CCAssert(s_uEncryptedPvrKeyParts[0] != 0, "Cocos2D: CCZ file is encrypted but key part 0 is not set. Did you call ZipUtils::ccSetPvrEncryptionKeyPart(...)?");
	CCAssert(s_uEncryptedPvrKeyParts[1] != 0, "Cocos2D: CCZ file is encrypted but key part 1 is not set. Did you call ZipUtils::ccSetPvrEncryptionKeyPart(...)?");
	CCAssert(s_uEncryptedPvrKeyParts[2] != 0, "Cocos2D: CCZ file is encrypted but key part 2 is not set. Did you call ZipUtils::ccSetPvrEncryptionKeyPart(...)?");
	CCAssert(s_uEncryptedPvrKeyParts[3] != 0, "Cocos2D: CCZ file is encrypted but key part 3 is not set. Did you call ZipUtils::ccSetPvrEncryptionKeyPart(...)?");

	// create long key
	if (!s_bEncryptionKeyIsValid)
	{
		unsigned int y, p, e;
		unsigned int rounds = 6;
		unsigned int sum = 0;
		unsigned int z = s_uEncryptionKey[enclen - 1];

		do
		{
#define DELTA 0x9e3779b9
#define MX (((z>>5^y<<2) + (y>>3^z<<4)) ^ ((sum^y) + (s_uEncryptedPvrKeyParts[(p&3)^e] ^ z)))

			sum += DELTA;
			e = (sum >> 2) & 3;

			for (p = 0; p < enclen - 1; p++)
			{
				y = s_uEncryptionKey[p + 1];
				z = s_uEncryptionKey[p] += MX;
			}

			y = s_uEncryptionKey[0];
			z = s_uEncryptionKey[enclen - 1] += MX;

		} while (--rounds);

		s_bEncryptionKeyIsValid = true;
	}

	int b = 0;
	int i = 0;

	// encrypt first part completely
	for (; i < len && i < securelen; i++)
	{
		data[i] ^= s_uEncryptionKey[b++];

		if (b >= enclen)
		{
			b = 0;
		}
	}

	// encrypt second section partially
	for (; i < len; i += distance)
	{
		data[i] ^= s_uEncryptionKey[b++];

		if (b >= enclen)
		{
			b = 0;
		}
	}
}


inline unsigned int ZipUtils::ccChecksumPvr(const unsigned int* data, int len)
{
	unsigned int cs = 0;
	const int cslen = 128;

	len = (len < cslen) ? len : cslen;

	for (int i = 0; i < len; i++)
	{
		cs = cs ^ data[i];
	}

	return cs;
}


void ZipUtils::ccSetPvrEncryptionKeyPart(int index, unsigned int value)
{
	CCAssert(index >= 0, "Cocos2d: key part index cannot be less than 0");
	CCAssert(index <= 3, "Cocos2d: key part index cannot be greater than 3");

	if (s_uEncryptedPvrKeyParts[index] != value)
	{
		s_uEncryptedPvrKeyParts[index] = value;
		s_bEncryptionKeyIsValid = false;
	}
}

void ZipUtils::ccSetPvrEncryptionKey(unsigned int keyPart1, unsigned int keyPart2, unsigned int keyPart3, unsigned int keyPart4)
{
	ccSetPvrEncryptionKeyPart(0, keyPart1);
	ccSetPvrEncryptionKeyPart(1, keyPart2);
	ccSetPvrEncryptionKeyPart(2, keyPart3);
	ccSetPvrEncryptionKeyPart(3, keyPart4);
}




NS_CC_END