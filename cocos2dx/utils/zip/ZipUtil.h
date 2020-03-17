#pragma once

#include "ccMacros.h"


NS_CC_BEGIN

struct CCZHeader {
	unsigned char   sig[4];             // signature. Should be 'CCZ!' 4 bytes
	unsigned short  compression_type;   // should 0
	unsigned short  version;            // should be 2 (although version type==1 is also supported)
	unsigned int    reserved;           // Reserved for users.
	unsigned int    len;                // size of the uncompressed file
};

enum {
	CCZ_COMPRESSION_ZLIB,               // zlib format.
	CCZ_COMPRESSION_BZIP2,              // bzip2 format (not supported yet)
	CCZ_COMPRESSION_GZIP,               // gzip format (not supported yet)
	CCZ_COMPRESSION_NONE,               // plain (not supported yet)
};


class CC_DLL ZipUtils
{
public:
	// 解压
	// Inflates either zlib or gzip deflated memory
	// The inflated memory is expected to be freed by the caller
	// It will allocate 256k for the destination buffer. If it is not enough it will multiply the previous buffer size per 2, 
	// until there is enough memory
	static int  ccInflateMemory(unsigned char* in, unsigned int inLength, unsigned char** out);

	// 额外多返回了解压后的空间大小
	static int  ccInflateMemoryWithHint(unsigned char* in, unsigned int inLength, unsigned char** out, unsigned int outLenghtHint);

	static int  ccInflateGZipFile(const char* filename, unsigned char** out);

	static int  ccInflateCCZFile(const char* filename, unsigned char** out);


    /** Sets the pvr.ccz encryption key parts separately for added
        * security.
        *
        * Example: If the key used to encrypt the pvr.ccz file is
        * 0xaaaaaaaabbbbbbbbccccccccdddddddd you will call this function 4
        * different times, preferably from 4 different source files, as follows
        *
        * ZipUtils::ccSetPvrEncryptionKeyPart(0, 0xaaaaaaaa);
        * ZipUtils::ccSetPvrEncryptionKeyPart(1, 0xbbbbbbbb);
        * ZipUtils::ccSetPvrEncryptionKeyPart(2, 0xcccccccc);
        * ZipUtils::ccSetPvrEncryptionKeyPart(3, 0xdddddddd);
        *
        * Splitting the key into 4 parts and calling the function
        * from 4 different source files increases the difficulty to
        * reverse engineer the encryption key. Be aware that encrpytion
        * is *never* 100% secure and the key code can be cracked by
        * knowledgable persons.
        *
        * IMPORTANT: Be sure to call ccSetPvrEncryptionKey or
        * ccSetPvrEncryptionKeyPart with all of the key parts *before* loading
        * the spritesheet or decryption will fail and the spritesheet
        * will fail to load.
        *
        * @param index part of the key [0..3]
        * @param value value of the key part
        */
    static void ccSetPvrEncryptionKeyPart(int index, unsigned int value);

    /** Sets the pvr.ccz encryption key.
        *
        * Example: If the key used to encrypt the pvr.ccz file is
        * 0xaaaaaaaabbbbbbbbccccccccdddddddd you will call this function with
        * the key split into 4 parts as follows
        *
        * ZipUtils::ccSetPvrEncryptionKey(0xaaaaaaaa, 0xbbbbbbbb, 0xcccccccc, 0xdddddddd);
        *
        * Note that using this function makes it easier to reverse engineer and
        * discover the complete key because the key parts are present in one
        * function call.
        *
        * IMPORTANT: Be sure to call ccSetPvrEncryptionKey or
        * ccSetPvrEncryptionKeyPart with all of the key parts *before* loading
        * the spritesheet or decryption will fail and the spritesheet
        * will fail to load.
        *
        * @param keyPart1 the key value part 1.
        * @param keyPart2 the key value part 2.
        * @param keyPart3 the key value part 3.
        * @param keyPart4 the key value part 4.
        */
    static void ccSetPvrEncryptionKey(unsigned int keyPart1, unsigned int keyPart2, unsigned int keyPart3, unsigned int keyPart4);


private:
	static int ccInflateMemoryWithHint(unsigned char* in, unsigned int inLength, unsigned char** out, unsigned int* outLength,
		                unsigned int outLenghtHint);
    static inline void ccDecodeEncodedPvr(unsigned int* data, int len);
    static inline unsigned int ccChecksumPvr(const unsigned int* data, int len);



private:
	static unsigned int s_uEncryptedPvrKeyParts[4];
	static unsigned int s_uEncryptionKey[1024];
	static bool         s_bEncryptionKeyIsValid;
};




NS_CC_END
