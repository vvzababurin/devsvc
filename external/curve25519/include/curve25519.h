
#ifndef __CURVE25519_H__
#define __CURVE25519_H__

#ifdef DLL_EXPORT
#define DLL_EXPORT_LIB extern __declspec(dllexport)
#else
#define DLL_EXPORT_LIB __declspec(dllimport)
#endif

extern "C"
{
	DLL_EXPORT_LIB unsigned int PRNG();

	DLL_EXPORT_LIB void* curve25519_malloc(size_t);
	DLL_EXPORT_LIB void* curve25519_realloc(void*, size_t);
	DLL_EXPORT_LIB void curve25519_free(void*);

	DLL_EXPORT_LIB void curve25519_privkeygen(unsigned char* curve25519_privkey_out /* 32 bytes */);
	DLL_EXPORT_LIB void curve25519_keygen(unsigned char* curve25519_pubkey_out, /* 32 bytes */
		const unsigned char* curve25519_privkey_in); /* 32 bytes */
/* returns 0 on success */
	DLL_EXPORT_LIB int curve25519_sign(unsigned char* signature_out, /* 64 bytes */
		const unsigned char* curve25519_privkey, /* 32 bytes */
		const unsigned char* msg, const unsigned long msg_len,
		const unsigned char* random); /* 64 bytes */
/* returns 0 on success */
	DLL_EXPORT_LIB int curve25519_verify(const unsigned char* signature, /* 64 bytes */
		const unsigned char* curve25519_pubkey, /* 32 bytes */
		const unsigned char* msg, const unsigned long msg_len);

	DLL_EXPORT_LIB void curve25519_privkeygenbase64(char** privkey_out_base64);
	DLL_EXPORT_LIB void curve25519_keygenbase64(char** pubkey_out_base64, const char* privkey_in_base64);
	DLL_EXPORT_LIB int curve25519_signbase64(char** signature_out_base64, /* 64 bytes */
		const char* privkey_base64, /* 32 bytes */
		const unsigned char* msg, const unsigned long msg_len,
		const unsigned char* random); /* 64 bytes */
	DLL_EXPORT_LIB int curve25519_verifybase64(const char* signature_base64, /* 64 bytes */
		const char* pubkey_base64, /* 32 bytes */
		const unsigned char* msg, const unsigned long msg_len);

}

#endif  /* __CURVE25519_H__ */
