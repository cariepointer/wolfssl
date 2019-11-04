/* api.c API unit tests
 *
 * Copyright (C) 2006-2019 wolfSSL Inc.
 *
 * This file is part of wolfSSL.
 *
 * wolfSSL is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * wolfSSL is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1335, USA
 */


/*----------------------------------------------------------------------------*
 | Includes
 *----------------------------------------------------------------------------*/

#ifdef HAVE_CONFIG_H
    #include <config.h>
#endif

#include <wolfssl/wolfcrypt/settings.h>

#ifndef FOURK_BUF
    #define FOURK_BUF 4096
#endif
#ifndef TWOK_BUF
    #define TWOK_BUF 2048
#endif
#ifndef ONEK_BUF
    #define ONEK_BUF 1024
#endif
#if defined(WOLFSSL_STATIC_MEMORY)
    #include <wolfssl/wolfcrypt/memory.h>
#endif /* WOLFSSL_STATIC_MEMORY */
#ifndef HEAP_HINT
    #define HEAP_HINT NULL
#endif /* WOLFSSL_STAIC_MEMORY */
#ifdef WOLFSSL_ASNC_CRYPT
    #include <wolfssl/wolfcrypt/async.h>
#endif
#ifdef HAVE_ECC
    #include <wolfssl/wolfcrypt/ecc.h>   /* wc_ecc_fp_free */
    #ifndef ECC_ASN963_MAX_BUF_SZ
        #define ECC_ASN963_MAX_BUF_SZ 133
    #endif
    #ifndef ECC_PRIV_KEY_BUF
        #define ECC_PRIV_KEY_BUF 66  /* For non user defined curves. */
    #endif
    #ifdef HAVE_ALL_CURVES
        /* ecc key sizes: 14, 16, 20, 24, 28, 30, 32, 40, 48, 64*/
        #ifndef KEY14
            #define KEY14 14
        #endif
        #if !defined(KEY16)
            #define KEY16 16
        #endif
        #if !defined(KEY20)
            #define KEY20 20
        #endif
        #if !defined(KEY24)
            #define KEY24 24
        #endif
        #if !defined(KEY28)
            #define KEY28 28
        #endif
        #if !defined(KEY30)
            #define KEY30 30
        #endif
        #if !defined(KEY32)
            #define KEY32 32
        #endif
        #if !defined(KEY40)
            #define KEY40 40
        #endif
        #if !defined(KEY48)
            #define KEY48 48
        #endif
        #if !defined(KEY64)
            #define KEY64 64
        #endif
    #else
        /* ecc key sizes: 14, 16, 20, 24, 28, 30, 32, 40, 48, 64*/
        #ifndef KEY14
            #define KEY14 32
        #endif
        #if !defined(KEY16)
            #define KEY16 32
        #endif
        #if !defined(KEY20)
            #define KEY20 32
        #endif
        #if !defined(KEY24)
            #define KEY24 32
        #endif
        #if !defined(KEY28)
            #define KEY28 32
        #endif
        #if !defined(KEY30)
            #define KEY30 32
        #endif
        #if !defined(KEY32)
            #define KEY32 32
        #endif
        #if !defined(KEY40)
            #define KEY40 32
        #endif
        #if !defined(KEY48)
            #define KEY48 32
        #endif
        #if !defined(KEY64)
            #define KEY64 32
        #endif
    #endif
    #if !defined(HAVE_COMP_KEY)
        #if !defined(NOCOMP)
            #define NOCOMP 0
        #endif
    #else
        #if !defined(COMP)
            #define COMP 1
        #endif
    #endif
    #if !defined(DER_SZ)
        #define DER_SZ (keySz * 2 + 1)
    #endif
#endif
#ifndef NO_ASN
    #include <wolfssl/wolfcrypt/asn_public.h>
#endif
#include <wolfssl/error-ssl.h>

#include <stdlib.h>
#include <wolfssl/ssl.h>  /* compatibility layer */
#include <wolfssl/test.h>
#include <tests/unit.h>
#include "examples/server/server.h"
     /* for testing compatibility layer callbacks */

#ifndef NO_MD5
    #include <wolfssl/wolfcrypt/md5.h>
#endif
#ifndef NO_SHA
    #include <wolfssl/wolfcrypt/sha.h>
#endif
#ifndef NO_SHA256
    #include <wolfssl/wolfcrypt/sha256.h>
#endif
#ifdef WOLFSSL_SHA512
    #include <wolfssl/wolfcrypt/sha512.h>
#endif
#ifdef WOLFSSL_SHA384
    #include <wolfssl/wolfcrypt/sha512.h>
#endif

#ifdef WOLFSSL_SHA3
    #include <wolfssl/wolfcrypt/sha3.h>
    #ifndef HEAP_HINT
        #define HEAP_HINT   NULL
    #endif
#endif

#ifndef NO_AES
    #include <wolfssl/wolfcrypt/aes.h>
    #ifdef HAVE_AES_DECRYPT
        #include <wolfssl/wolfcrypt/wc_encrypt.h>
    #endif
#endif
#ifdef WOLFSSL_RIPEMD
    #include <wolfssl/wolfcrypt/ripemd.h>
#endif
#ifdef HAVE_IDEA
    #include <wolfssl/wolfcrypt/idea.h>
#endif
#ifndef NO_DES3
    #include <wolfssl/wolfcrypt/des3.h>
    #include <wolfssl/wolfcrypt/wc_encrypt.h>
#endif

#ifndef NO_HMAC
    #include <wolfssl/wolfcrypt/hmac.h>
#endif

#ifdef HAVE_CHACHA
    #include <wolfssl/wolfcrypt/chacha.h>
#endif

#ifdef HAVE_POLY1305
    #include <wolfssl/wolfcrypt/poly1305.h>
#endif

#if defined(HAVE_CHACHA) && defined(HAVE_POLY1305)
    #include <wolfssl/wolfcrypt/chacha20_poly1305.h>
#endif

#ifdef HAVE_CAMELLIA
    #include <wolfssl/wolfcrypt/camellia.h>
#endif

#ifndef NO_RABBIT
    #include <wolfssl/wolfcrypt/rabbit.h>
#endif

#ifndef NO_RC4
    #include <wolfssl/wolfcrypt/arc4.h>
#endif

#ifdef HAVE_BLAKE2
    #include <wolfssl/wolfcrypt/blake2.h>
#endif

#ifndef NO_RSA
    #include <wolfssl/wolfcrypt/rsa.h>
    #include <wolfssl/wolfcrypt/hash.h>

    #define FOURK_BUF 4096
    #define GEN_BUF  294

    #ifndef USER_CRYPTO_ERROR
        #define USER_CRYPTO_ERROR -101 /* error returned by IPP lib. */
    #endif
#endif

#ifndef NO_SIG_WRAPPER
    #include <wolfssl/wolfcrypt/signature.h>
#endif


#ifdef HAVE_AESCCM
    #include <wolfssl/wolfcrypt/aes.h>
#endif

#ifdef HAVE_HC128
    #include <wolfssl/wolfcrypt/hc128.h>
#endif

#ifdef HAVE_PKCS7
    #include <wolfssl/wolfcrypt/pkcs7.h>
    #include <wolfssl/wolfcrypt/asn.h>
#endif

#ifdef WOLFSSL_SMALL_CERT_VERIFY
    #include <wolfssl/wolfcrypt/asn.h>
#endif

#if defined(WOLFSSL_SHA3) || defined(HAVE_PKCS7) || !defined(NO_RSA)
    static int  devId = INVALID_DEVID;
#endif
#ifndef NO_DSA
    #include <wolfssl/wolfcrypt/dsa.h>
    #ifndef ONEK_BUF
        #define ONEK_BUF 1024
    #endif
    #ifndef TWOK_BUF
        #define TWOK_BUF 2048
    #endif
    #ifndef FOURK_BUF
        #define FOURK_BUF 4096
    #endif
    #ifndef DSA_SIG_SIZE
        #define DSA_SIG_SIZE 40
    #endif
    #ifndef MAX_DSA_PARAM_SIZE
        #define MAX_DSA_PARAM_SIZE 256
    #endif
#endif

#ifdef WOLFSSL_CMAC
    #include <wolfssl/wolfcrypt/cmac.h>
#endif

#ifdef HAVE_ED25519
    #include <wolfssl/wolfcrypt/ed25519.h>
#endif

#ifdef HAVE_CURVE25519
    #include <wolfssl/wolfcrypt/curve25519.h>
#endif
#ifdef HAVE_PKCS12
    #include <wolfssl/wolfcrypt/pkcs12.h>
#endif

#if (defined(OPENSSL_EXTRA) || defined(OPENSSL_EXTRA_X509_SMALL) || defined(OPENSSL_ALL))
    #include <wolfssl/openssl/ssl.h>
    #ifndef NO_ASN
        /* for ASN_COMMON_NAME DN_tags enum */
        #include <wolfssl/wolfcrypt/asn.h>
    #endif
    #ifdef HAVE_OCSP
        #include <wolfssl/openssl/ocsp.h>
    #endif
#endif
#ifdef OPENSSL_EXTRA
    #include <wolfssl/openssl/x509v3.h>
    #include <wolfssl/openssl/asn1.h>
    #include <wolfssl/openssl/crypto.h>
    #include <wolfssl/openssl/pkcs12.h>
    #include <wolfssl/openssl/evp.h>
    #include <wolfssl/openssl/dh.h>
    #include <wolfssl/openssl/bn.h>
    #include <wolfssl/openssl/buffer.h>
    #include <wolfssl/openssl/pem.h>
    #include <wolfssl/openssl/ec.h>
    #include <wolfssl/openssl/engine.h>
    #include <wolfssl/openssl/crypto.h>
    #include <wolfssl/openssl/hmac.h>
    #include <wolfssl/openssl/objects.h>
#ifndef NO_AES
    #include <wolfssl/openssl/aes.h>
#endif
#ifndef NO_DES3
    #include <wolfssl/openssl/des.h>
#endif
#ifdef HAVE_ECC
    #include <wolfssl/openssl/ecdsa.h>
#endif
#ifdef HAVE_PKCS7
    #include <wolfssl/openssl/pkcs7.h>
#endif
#endif /* OPENSSL_EXTRA */

#if defined(OPENSSL_EXTRA) && defined(WOLFCRYPT_HAVE_SRP) \
    && !defined(NO_SHA256) && !defined(RC_NO_RNG)
        #include <wolfssl/wolfcrypt/srp.h>
#endif

#if defined(SESSION_CERTS) && defined(TEST_PEER_CERT_CHAIN)
#include "wolfssl/internal.h" /* for testing SSL_get_peer_cert_chain */
#endif

/* force enable test buffers */
#ifndef USE_CERT_BUFFERS_2048
    #define USE_CERT_BUFFERS_2048
#endif
#ifndef USE_CERT_BUFFERS_256
    #define USE_CERT_BUFFERS_256
#endif
#include <wolfssl/certs_test.h>

typedef struct testVector {
    const char* input;
    const char* output;
    size_t inLen;
    size_t outLen;

} testVector;

#if defined(HAVE_PKCS7)
    typedef struct {
        const byte* content;
        word32      contentSz;
        int         contentOID;
        int         encryptOID;
        int         keyWrapOID;
        int         keyAgreeOID;
        byte*       cert;
        size_t      certSz;
        byte*       privateKey;
        word32      privateKeySz;
    } pkcs7EnvelopedVector;

    #ifndef NO_PKCS7_ENCRYPTED_DATA
        typedef struct {
            const byte*     content;
            word32          contentSz;
            int             contentOID;
            int             encryptOID;
            byte*           encryptionKey;
            word32          encryptionKeySz;
        } pkcs7EncryptedVector;
    #endif
#endif /* HAVE_PKCS7 */


/*----------------------------------------------------------------------------*
 | Constants
 *----------------------------------------------------------------------------*/

#define TEST_SUCCESS    (1)
#define TEST_FAIL       (0)

#define testingFmt "   %s:"
#define resultFmt  " %s\n"
static const char* passed = "passed";
static const char* failed = "failed";

#if !defined(NO_FILESYSTEM) && !defined(NO_CERTS) && \
    (!defined(NO_WOLFSSL_SERVER) || !defined(NO_WOLFSSL_CLIENT))
    static const char* bogusFile  =
    #ifdef _WIN32
        "NUL"
    #else
        "/dev/null"
    #endif
    ;
#endif /* !NO_FILESYSTEM && !NO_CERTS && (!NO_WOLFSSL_SERVER || !NO_WOLFSSL_CLIENT) */

enum {
    TESTING_RSA = 1,
    TESTING_ECC = 2
};


/*----------------------------------------------------------------------------*
 | Setup
 *----------------------------------------------------------------------------*/

static int test_wolfSSL_Init(void)
{
    int result;

    printf(testingFmt, "wolfSSL_Init()");
    result = wolfSSL_Init();
    printf(resultFmt, result == WOLFSSL_SUCCESS ? passed : failed);

    return result;
}


static int test_wolfSSL_Cleanup(void)
{
    int result;

    printf(testingFmt, "wolfSSL_Cleanup()");
    result = wolfSSL_Cleanup();
    printf(resultFmt, result == WOLFSSL_SUCCESS ? passed : failed);

    return result;
}


/*  Initialize the wolfCrypt state.
 *  POST: 0 success.
 */
static int test_wolfCrypt_Init(void)
{
    int result;

    printf(testingFmt, "wolfCrypt_Init()");
    result = wolfCrypt_Init();
    printf(resultFmt, result == 0 ? passed : failed);

    return result;

} /* END test_wolfCrypt_Init */

/*----------------------------------------------------------------------------*
 | Platform dependent function test
 *----------------------------------------------------------------------------*/
 static int test_fileAccess()
{
#if defined(WOLFSSL_TEST_PLATFORMDEPEND) && !defined(NO_FILESYSTEM)
    const char *fname[] = {
    svrCertFile, svrKeyFile, caCertFile,
    eccCertFile, eccKeyFile, eccRsaCertFile,
    cliCertFile, cliCertDerFile, cliKeyFile,
    ntruCertFile, ntruKeyFile, dhParamFile,
    cliEccKeyFile, cliEccCertFile, caEccCertFile, edCertFile, edKeyFile,
    cliEdCertFile, cliEdKeyFile, caEdCertFile,
    NULL
    };
     const char derfile[] = "./certs/server-cert.der";
    XFILE f;
    size_t sz;
    byte *buff;
    int i;

    printf(testingFmt, "test_fileAccess()");

    AssertTrue(XFOPEN("badfilename", "rb") == XBADFILE);
     for(i=0; fname[i] != NULL ; i++){
        AssertTrue((f = XFOPEN(fname[i], "rb")) != XBADFILE);
        XFCLOSE(f);
    }

    AssertTrue((f = XFOPEN(derfile, "rb")) != XBADFILE);
    AssertTrue(XFSEEK(f, 0, XSEEK_END) == 0);
    sz = (size_t) XFTELL(f);
    XREWIND(f);
    AssertTrue(sz == sizeof_server_cert_der_2048);
    AssertTrue((buff = (byte*)XMALLOC(sz, NULL, DYNAMIC_TYPE_FILE)) != NULL) ;
    AssertTrue(XFREAD(buff, 1, sz, f) == sz);
    XMEMCMP(server_cert_der_2048, buff, sz);
     printf(resultFmt, passed);
#endif
    return WOLFSSL_SUCCESS;
}

/*----------------------------------------------------------------------------*
 | Method Allocators
 *----------------------------------------------------------------------------*/

static void test_wolfSSL_Method_Allocators(void)
{
    #define TEST_METHOD_ALLOCATOR(allocator, condition) \
        do {                                            \
            WOLFSSL_METHOD *method;                      \
            condition(method = allocator());            \
            XFREE(method, 0, DYNAMIC_TYPE_METHOD);      \
        } while(0)

    #define TEST_VALID_METHOD_ALLOCATOR(a) \
            TEST_METHOD_ALLOCATOR(a, AssertNotNull)

    #define TEST_INVALID_METHOD_ALLOCATOR(a) \
            TEST_METHOD_ALLOCATOR(a, AssertNull)

#ifndef NO_OLD_TLS
    #ifdef WOLFSSL_ALLOW_SSLV3
        #ifndef NO_WOLFSSL_SERVER
        TEST_VALID_METHOD_ALLOCATOR(wolfSSLv3_server_method);
        #endif
        #ifndef NO_WOLFSSL_CLIENT
        TEST_VALID_METHOD_ALLOCATOR(wolfSSLv3_client_method);
        #endif
    #endif
    #ifdef WOLFLSL_ALLOW_TLSV10
        #ifndef NO_WOLFSSL_SERVER
        TEST_VALID_METHOD_ALLOCATOR(wolfTLSv1_server_method);
        #endif
        #ifndef NO_WOLFSSL_CLIENT
        TEST_VALID_METHOD_ALLOCATOR(wolfTLSv1_client_method);
        #endif
    #endif
    #ifndef NO_WOLFSSL_SERVER
        TEST_VALID_METHOD_ALLOCATOR(wolfTLSv1_1_server_method);
    #endif
    #ifndef NO_WOLFSSL_CLIENT
        TEST_VALID_METHOD_ALLOCATOR(wolfTLSv1_1_client_method);
    #endif
#endif /* !NO_OLD_TLS */

#ifndef WOLFSSL_NO_TLS12
    #ifndef NO_WOLFSSL_SERVER
        TEST_VALID_METHOD_ALLOCATOR(wolfTLSv1_2_server_method);
    #endif
    #ifndef NO_WOLFSSL_CLIENT
        TEST_VALID_METHOD_ALLOCATOR(wolfTLSv1_2_client_method);
    #endif
#endif /* !WOLFSSL_NO_TLS12 */

#ifdef WOLFSSL_TLS13
    #ifndef NO_WOLFSSL_SERVER
        TEST_VALID_METHOD_ALLOCATOR(wolfTLSv1_3_server_method);
    #endif
    #ifndef NO_WOLFSSL_CLIENT
        TEST_VALID_METHOD_ALLOCATOR(wolfTLSv1_3_client_method);
    #endif
#endif /* WOLFSSL_TLS13 */

#ifndef NO_WOLFSSL_SERVER
    TEST_VALID_METHOD_ALLOCATOR(wolfSSLv23_server_method);
#endif
#ifndef NO_WOLFSSL_CLIENT
    TEST_VALID_METHOD_ALLOCATOR(wolfSSLv23_client_method);
#endif

#ifdef WOLFSSL_DTLS
    #ifndef NO_OLD_TLS
        #ifndef NO_WOLFSSL_SERVER
        TEST_VALID_METHOD_ALLOCATOR(wolfDTLSv1_server_method);
        #endif
        #ifndef NO_WOLFSSL_CLIENT
        TEST_VALID_METHOD_ALLOCATOR(wolfDTLSv1_client_method);
        #endif
    #endif
    #ifndef WOLFSSL_NO_TLS12
        #ifndef NO_WOLFSSL_SERVER
        TEST_VALID_METHOD_ALLOCATOR(wolfDTLSv1_2_server_method);
        #endif
        #ifndef NO_WOLFSSL_CLIENT
        TEST_VALID_METHOD_ALLOCATOR(wolfDTLSv1_2_client_method);
        #endif
    #endif
#endif /* WOLFSSL_DTLS */

#if !defined(NO_OLD_TLS) && defined(OPENSSL_EXTRA)
    /* Stubs */
    #ifndef NO_WOLFSSL_SERVER
        TEST_INVALID_METHOD_ALLOCATOR(wolfSSLv2_server_method);
    #endif
    #ifndef NO_WOLFSSL_CLIENT
        TEST_INVALID_METHOD_ALLOCATOR(wolfSSLv2_client_method);
    #endif
#endif

    /* Test Either Method (client or server) */
#if defined(OPENSSL_EXTRA) || defined(WOLFSSL_EITHER_SIDE)
    TEST_VALID_METHOD_ALLOCATOR(wolfSSLv23_method);
    #ifndef NO_OLD_TLS
        #ifdef WOLFSSL_ALLOW_TLSV10
            TEST_VALID_METHOD_ALLOCATOR(wolfTLSv1_method);
        #endif
        TEST_VALID_METHOD_ALLOCATOR(wolfTLSv1_1_method);
    #endif /* !NO_OLD_TLS */
    #ifndef WOLFSSL_NO_TLS12
        TEST_VALID_METHOD_ALLOCATOR(wolfTLSv1_2_method);
    #endif /* !WOLFSSL_NO_TLS12 */
    #ifdef WOLFSSL_TLS13
        TEST_VALID_METHOD_ALLOCATOR(wolfTLSv1_3_method);
    #endif /* WOLFSSL_TLS13 */
    #ifdef WOLFSSL_DTLS
        TEST_VALID_METHOD_ALLOCATOR(wolfDTLS_method);
        #ifndef NO_OLD_TLS
            TEST_VALID_METHOD_ALLOCATOR(wolfDTLSv1_method);
        #endif /* !NO_OLD_TLS */
        #ifndef WOLFSSL_NO_TLS12
            TEST_VALID_METHOD_ALLOCATOR(wolfDTLSv1_2_method);
        #endif /* !WOLFSSL_NO_TLS12 */
    #endif /* WOLFSSL_DTLS */
#endif /* OPENSSL_EXTRA || WOLFSSL_EITHER_SIDE */
}


/*----------------------------------------------------------------------------*
 | Context
 *----------------------------------------------------------------------------*/
#ifndef NO_WOLFSSL_SERVER
static void test_wolfSSL_CTX_new(WOLFSSL_METHOD *method)
{
    WOLFSSL_CTX *ctx;

    AssertNull(ctx = wolfSSL_CTX_new(NULL));

    AssertNotNull(method);
    AssertNotNull(ctx = wolfSSL_CTX_new(method));
    wolfSSL_CTX_free(ctx);
}
#endif

#if (!defined(NO_WOLFSSL_CLIENT) || !defined(NO_WOLFSSL_SERVER)) && \
(!defined(NO_RSA) || defined(HAVE_ECC))
static void test_for_double_Free(void)
{
    WOLFSSL_CTX* ctx;
    WOLFSSL*     ssl;
    int skipTest = 0;
    const char* testCertFile;
    const char* testKeyFile;
    char optionsCiphers[] = "RC4-SHA:RC4-MD5:DES-CBC3-SHA:AES128-SHA:AES256-SHA"
":NULL-SHA:NULL-SHA256:DHE-RSA-AES128-SHA:DHE-RSA-AES256-SHA:DHE-PSK-AES256-GCM"
"-SHA384:DHE-PSK-AES128-GCM-SHA256:PSK-AES256-GCM-SHA384:PSK-AES128-GCM-SHA256:"
"DHE-PSK-AES256-CBC-SHA384:DHE-PSK-AES128-CBC-SHA256:PSK-AES256-CBC-SHA384:PSK-"
"AES128-CBC-SHA256:PSK-AES128-CBC-SHA:PSK-AES256-CBC-SHA:DHE-PSK-AES128-CCM:DHE"
"-PSK-AES256-CCM:PSK-AES128-CCM:PSK-AES256-CCM:PSK-AES128-CCM-8:PSK-AES256-CCM-"
"8:DHE-PSK-NULL-SHA384:DHE-PSK-NULL-SHA256:PSK-NULL-SHA384:PSK-NULL-SHA256:PSK-"
"NULL-SHA:HC128-MD5:HC128-SHA:RABBIT-SHA:AES128-CCM-8:AES256-CCM-8:ECDHE-ECDSA-"
"AES128-CCM:ECDHE-ECDSA-AES128-CCM-8:ECDHE-ECDSA-AES256-CCM-8:ECDHE-RSA-AES128-"
"SHA:ECDHE-RSA-AES256-SHA:ECDHE-ECDSA-AES128-SHA:ECDHE-ECDSA-AES256-SHA:ECDHE-R"
"SA-RC4-SHA:ECDHE-RSA-DES-CBC3-SHA:ECDHE-ECDSA-RC4-SHA:ECDHE-ECDSA-DES-CBC3-SHA"
":AES128-SHA256:AES256-SHA256:DHE-RSA-AES128-SHA256:DHE-RSA-AES256-SHA256:ECDH-"
"RSA-AES128-SHA:ECDH-RSA-AES256-SHA:ECDH-ECDSA-AES128-SHA:ECDH-ECDSA-AES256-SHA"
":ECDH-RSA-RC4-SHA:ECDH-RSA-DES-CBC3-SHA:ECDH-ECDSA-RC4-SHA:ECDH-ECDSA-DES-CBC3"
"-SHA:AES128-GCM-SHA256:AES256-GCM-SHA384:DHE-RSA-AES128-GCM-SHA256:DHE-RSA-AES"
"256-GCM-SHA384:ECDHE-RSA-AES128-GCM-SHA256:ECDHE-RSA-AES256-GCM-SHA384:ECDHE-E"
"CDSA-AES128-GCM-SHA256:ECDHE-ECDSA-AES256-GCM-SHA384:ECDH-RSA-AES128-GCM-SHA25"
"6:ECDH-RSA-AES256-GCM-SHA384:ECDH-ECDSA-AES128-GCM-SHA256:ECDH-ECDSA-AES256-GC"
"M-SHA384:CAMELLIA128-SHA:DHE-RSA-CAMELLIA128-SHA:CAMELLIA256-SHA:DHE-RSA-CAMEL"
"LIA256-SHA:CAMELLIA128-SHA256:DHE-RSA-CAMELLIA128-SHA256:CAMELLIA256-SHA256:DH"
"E-RSA-CAMELLIA256-SHA256:ECDHE-RSA-AES128-SHA256:ECDHE-ECDSA-AES128-SHA256:ECD"
"H-RSA-AES128-SHA256:ECDH-ECDSA-AES128-SHA256:ECDHE-RSA-AES256-SHA384:ECDHE-ECD"
"SA-AES256-SHA384:ECDH-RSA-AES256-SHA384:ECDH-ECDSA-AES256-SHA384:ECDHE-RSA-CHA"
"CHA20-POLY1305:ECDHE-ECDSA-CHACHA20-POLY1305:DHE-RSA-CHACHA20-POLY1305:ECDHE-R"
"SA-CHACHA20-POLY1305-OLD:ECDHE-ECDSA-CHACHA20-POLY1305-OLD:DHE-RSA-CHACHA20-PO"
"LY1305-OLD:IDEA-CBC-SHA:ECDHE-ECDSA-NULL-SHA:ECDHE-PSK-NULL-SHA256:ECDHE-PSK-A"
"ES128-CBC-SHA256:PSK-CHACHA20-POLY1305:ECDHE-PSK-CHACHA20-POLY1305:DHE-PSK-CHA"
"CHA20-POLY1305:EDH-RSA-DES-CBC3-SHA:TLS13-AES128-GCM-SHA256:TLS13-AES256-GCM-S"
"HA384:TLS13-CHACHA20-POLY1305-SHA256:TLS13-AES128-CCM-SHA256:TLS13-AES128-CCM-"
"8-SHA256:TLS13-SHA256-SHA256:TLS13-SHA384-SHA384";
#ifndef NO_RSA
	testCertFile = svrCertFile;
	testKeyFile = svrKeyFile;
#elif defined(HAVE_ECC)
	testCertFile = eccCertFile;
	testKeyFile = eccKeyFile;
#else
    skipTest = 1;
#endif

    if (skipTest != 1) {
#ifndef NO_WOLFSSL_SERVER
        AssertNotNull(ctx = wolfSSL_CTX_new(wolfSSLv23_server_method()));
#else
        AssertNotNull(ctx = wolfSSL_CTX_new(wolfSSLv23_client_method()));
#endif
        AssertTrue(wolfSSL_CTX_use_certificate_file(ctx, testCertFile, WOLFSSL_FILETYPE_PEM));
        AssertTrue(wolfSSL_CTX_use_PrivateKey_file(ctx, testKeyFile, WOLFSSL_FILETYPE_PEM));

        AssertNotNull(ssl = wolfSSL_new(ctx));

        /* First test freeing SSL, then CTX */
        wolfSSL_free(ssl);
        wolfSSL_CTX_free(ctx);

#ifndef NO_WOLFSSL_CLIENT
        AssertNotNull(ctx = wolfSSL_CTX_new(wolfSSLv23_client_method()));
#else
        AssertNotNull(ctx = wolfSSL_CTX_new(wolfSSLv23_server_method()));
#endif
        AssertTrue(wolfSSL_CTX_use_certificate_file(ctx, testCertFile, WOLFSSL_FILETYPE_PEM));
        AssertTrue(wolfSSL_CTX_use_PrivateKey_file(ctx, testKeyFile, WOLFSSL_FILETYPE_PEM));

        AssertNotNull(ssl = wolfSSL_new(ctx));

        /* Next test freeing CTX then SSL */
        wolfSSL_CTX_free(ctx);
        wolfSSL_free(ssl);

#ifndef NO_WOLFSSL_SERVER
        AssertNotNull(ctx = wolfSSL_CTX_new(wolfSSLv23_server_method()));
#else
        AssertNotNull(ctx = wolfSSL_CTX_new(wolfSSLv23_client_method()));
#endif
        /* Test setting ciphers at ctx level */
        AssertTrue(wolfSSL_CTX_use_certificate_file(ctx, testCertFile, WOLFSSL_FILETYPE_PEM));
        AssertTrue(wolfSSL_CTX_use_PrivateKey_file(ctx, testKeyFile, WOLFSSL_FILETYPE_PEM));
        AssertTrue(wolfSSL_CTX_set_cipher_list(ctx, optionsCiphers));
        AssertNotNull(ssl = wolfSSL_new(ctx));
        wolfSSL_CTX_free(ctx);
        wolfSSL_free(ssl);

#ifndef NO_WOLFSSL_CLIENT
        AssertNotNull(ctx = wolfSSL_CTX_new(wolfSSLv23_client_method()));
#else
        AssertNotNull(ctx = wolfSSL_CTX_new(wolfSSLv23_server_method()));
#endif
        AssertTrue(wolfSSL_CTX_use_certificate_file(ctx, testCertFile, WOLFSSL_FILETYPE_PEM));
        AssertTrue(wolfSSL_CTX_use_PrivateKey_file(ctx, testKeyFile, WOLFSSL_FILETYPE_PEM));
        AssertNotNull(ssl = wolfSSL_new(ctx));
        /* test setting ciphers at SSL level */
        AssertTrue(wolfSSL_set_cipher_list(ssl, optionsCiphers));
        wolfSSL_CTX_free(ctx);
        wolfSSL_free(ssl);
    }
}
#endif


static void test_wolfSSL_CTX_use_certificate_file(void)
{
#if !defined(NO_FILESYSTEM) && !defined(NO_CERTS) && !defined(NO_WOLFSSL_SERVER)
    WOLFSSL_CTX *ctx;

    AssertNotNull(ctx = wolfSSL_CTX_new(wolfSSLv23_server_method()));

    /* invalid context */
    AssertFalse(wolfSSL_CTX_use_certificate_file(NULL, svrCertFile,
                                                             WOLFSSL_FILETYPE_PEM));
    /* invalid cert file */
    AssertFalse(wolfSSL_CTX_use_certificate_file(ctx, bogusFile,
                                                             WOLFSSL_FILETYPE_PEM));
    /* invalid cert type */
    AssertFalse(wolfSSL_CTX_use_certificate_file(ctx, svrCertFile, 9999));

#ifdef NO_RSA
    /* rsa needed */
    AssertFalse(wolfSSL_CTX_use_certificate_file(ctx, svrCertFile,WOLFSSL_FILETYPE_PEM));
#else
    /* success */
    AssertTrue(wolfSSL_CTX_use_certificate_file(ctx, svrCertFile, WOLFSSL_FILETYPE_PEM));
#endif

    wolfSSL_CTX_free(ctx);
#endif
}

#if (defined(OPENSSL_ALL) || defined(WOLFSSL_ASIO)) && !defined(NO_RSA)
static int test_wolfSSL_CTX_use_certificate_ASN1(void)
{
#if !defined(NO_CERTS) && !defined(NO_WOLFSSL_SERVER) && !defined(NO_ASN)
    WOLFSSL_CTX*            ctx;
    int                     ret;

    printf(testingFmt, "wolfSSL_CTX_use_certificate_ASN1()");
    AssertNotNull(ctx = wolfSSL_CTX_new(wolfSSLv23_server_method()));

    ret = SSL_CTX_use_certificate_ASN1(ctx, sizeof_server_cert_der_2048,
                server_cert_der_2048);

    printf(resultFmt, ret == WOLFSSL_SUCCESS ? passed : failed);
    wolfSSL_CTX_free(ctx);

    return ret;
#else
    return WOLFSSL_SUCCESS;
#endif
}
#endif /* (OPENSSL_ALL || WOLFSSL_ASIO) && !NO_RSA */

/*  Test function for wolfSSL_CTX_use_certificate_buffer. Load cert into
 *  context using buffer.
 *  PRE: NO_CERTS not defined; USE_CERT_BUFFERS_2048 defined; compile with
 *  --enable-testcert flag.
 */
static int test_wolfSSL_CTX_use_certificate_buffer(void)
{
    #if !defined(NO_CERTS) && defined(USE_CERT_BUFFERS_2048) && \
            !defined(NO_RSA) && !defined(NO_WOLFSSL_SERVER)
        WOLFSSL_CTX*            ctx;
        int                     ret;

        printf(testingFmt, "wolfSSL_CTX_use_certificate_buffer()");
        AssertNotNull(ctx = wolfSSL_CTX_new(wolfSSLv23_server_method()));

        ret = wolfSSL_CTX_use_certificate_buffer(ctx, server_cert_der_2048,
                    sizeof_server_cert_der_2048, WOLFSSL_FILETYPE_ASN1);

        printf(resultFmt, ret == WOLFSSL_SUCCESS ? passed : failed);
        wolfSSL_CTX_free(ctx);

        return ret;
    #else
        return WOLFSSL_SUCCESS;
    #endif

} /*END test_wolfSSL_CTX_use_certificate_buffer*/

static void test_wolfSSL_CTX_use_PrivateKey_file(void)
{
#if !defined(NO_FILESYSTEM) && !defined(NO_CERTS) && !defined(NO_WOLFSSL_SERVER)
    WOLFSSL_CTX *ctx;

    AssertNotNull(ctx = wolfSSL_CTX_new(wolfSSLv23_server_method()));

    /* invalid context */
    AssertFalse(wolfSSL_CTX_use_PrivateKey_file(NULL, svrKeyFile,
                                                             WOLFSSL_FILETYPE_PEM));
    /* invalid key file */
    AssertFalse(wolfSSL_CTX_use_PrivateKey_file(ctx, bogusFile,
                                                             WOLFSSL_FILETYPE_PEM));
    /* invalid key type */
    AssertFalse(wolfSSL_CTX_use_PrivateKey_file(ctx, svrKeyFile, 9999));

    /* success */
#ifdef NO_RSA
    /* rsa needed */
    AssertFalse(wolfSSL_CTX_use_PrivateKey_file(ctx, svrKeyFile, WOLFSSL_FILETYPE_PEM));
#else
    /* success */
    AssertTrue(wolfSSL_CTX_use_PrivateKey_file(ctx, svrKeyFile, WOLFSSL_FILETYPE_PEM));
#endif

    wolfSSL_CTX_free(ctx);
#endif
}


/* test both file and buffer versions along with unloading trusted peer certs */
static void test_wolfSSL_CTX_trust_peer_cert(void)
{
#if !defined(NO_CERTS) && defined(WOLFSSL_TRUST_PEER_CERT) && \
    !defined(NO_WOLFSSL_CLIENT)
    WOLFSSL_CTX *ctx;

    AssertNotNull(ctx = wolfSSL_CTX_new(wolfSSLv23_client_method()));

#if !defined(NO_FILESYSTEM)
    /* invalid file */
    assert(wolfSSL_CTX_trust_peer_cert(ctx, NULL,
                                              WOLFSSL_FILETYPE_PEM) != WOLFSSL_SUCCESS);
    assert(wolfSSL_CTX_trust_peer_cert(ctx, bogusFile,
                                              WOLFSSL_FILETYPE_PEM) != WOLFSSL_SUCCESS);
    assert(wolfSSL_CTX_trust_peer_cert(ctx, cliCertFile,
                                             WOLFSSL_FILETYPE_ASN1) != WOLFSSL_SUCCESS);

    /* success */
    assert(wolfSSL_CTX_trust_peer_cert(ctx, cliCertFile, WOLFSSL_FILETYPE_PEM)
                                                                == WOLFSSL_SUCCESS);

    /* unload cert */
    assert(wolfSSL_CTX_Unload_trust_peers(NULL) != WOLFSSL_SUCCESS);
    assert(wolfSSL_CTX_Unload_trust_peers(ctx) == WOLFSSL_SUCCESS);
#endif

    /* Test of loading certs from buffers */

    /* invalid buffer */
    assert(wolfSSL_CTX_trust_peer_buffer(ctx, NULL, -1,
                                             WOLFSSL_FILETYPE_ASN1) != WOLFSSL_SUCCESS);

    /* success */
#ifdef USE_CERT_BUFFERS_1024
    assert(wolfSSL_CTX_trust_peer_buffer(ctx, client_cert_der_1024,
                sizeof_client_cert_der_1024, WOLFSSL_FILETYPE_ASN1) == WOLFSSL_SUCCESS);
#endif
#ifdef USE_CERT_BUFFERS_2048
    assert(wolfSSL_CTX_trust_peer_buffer(ctx, client_cert_der_2048,
                sizeof_client_cert_der_2048, WOLFSSL_FILETYPE_ASN1) == WOLFSSL_SUCCESS);
#endif

    /* unload cert */
    assert(wolfSSL_CTX_Unload_trust_peers(NULL) != WOLFSSL_SUCCESS);
    assert(wolfSSL_CTX_Unload_trust_peers(ctx) == WOLFSSL_SUCCESS);

    wolfSSL_CTX_free(ctx);
#endif
}


static void test_wolfSSL_CTX_load_verify_locations(void)
{
#if !defined(NO_FILESYSTEM) && !defined(NO_CERTS) && !defined(NO_WOLFSSL_CLIENT)
    WOLFSSL_CTX *ctx;
#ifndef NO_RSA
    WOLFSSL_CERT_MANAGER* cm;
#ifdef PERSIST_CERT_CACHE
    int cacheSz;
#endif
#endif
#if !defined(NO_WOLFSSL_DIR) && !defined(WOLFSSL_TIRTOS)
    const char* load_certs_path = "./certs/external";
    const char* load_no_certs_path = "./examples";
    const char* load_expired_path = "./certs/test/expired";
#endif

    AssertNotNull(ctx = wolfSSL_CTX_new(wolfSSLv23_client_method()));

    /* invalid arguments */
    AssertIntEQ(wolfSSL_CTX_load_verify_locations(NULL, caCertFile, NULL), WOLFSSL_FAILURE);
    AssertIntEQ(wolfSSL_CTX_load_verify_locations(ctx, NULL, NULL), WOLFSSL_FAILURE);

    /* invalid ca file */
    AssertIntEQ(wolfSSL_CTX_load_verify_locations(ctx, bogusFile, NULL), WOLFSSL_BAD_FILE);


#if !defined(NO_WOLFSSL_DIR) && !defined(WOLFSSL_TIRTOS)
    /* invalid path */
    AssertIntEQ(wolfSSL_CTX_load_verify_locations(ctx, NULL, bogusFile), BAD_PATH_ERROR);
#endif

    /* load ca cert */
#ifdef NO_RSA
    AssertIntEQ(wolfSSL_CTX_load_verify_locations(ctx, caCertFile, NULL), ASN_UNKNOWN_OID_E);
#else /* Skip the following test without RSA certs. */
    AssertIntEQ(wolfSSL_CTX_load_verify_locations(ctx, caCertFile, NULL), WOLFSSL_SUCCESS);

#ifdef PERSIST_CERT_CACHE
    /* Get cert cache size */
    cacheSz = wolfSSL_CTX_get_cert_cache_memsize(ctx);
#endif
    /* Test unloading CA's */
    AssertIntEQ(wolfSSL_CTX_UnloadCAs(ctx), WOLFSSL_SUCCESS);

#ifdef PERSIST_CERT_CACHE
    /* Verify no certs (result is less than cacheSz) */
    AssertIntGT(cacheSz, wolfSSL_CTX_get_cert_cache_memsize(ctx));
#endif

    /* load ca cert again */
    AssertIntEQ(wolfSSL_CTX_load_verify_locations(ctx, caCertFile, NULL), WOLFSSL_SUCCESS);

    /* Test getting CERT_MANAGER */
    AssertNotNull(cm = wolfSSL_CTX_GetCertManager(ctx));

    /* Test unloading CA's using CM */
    AssertIntEQ(wolfSSL_CertManagerUnloadCAs(cm), WOLFSSL_SUCCESS);

#ifdef PERSIST_CERT_CACHE
    /* Verify no certs (result is less than cacheSz) */
    AssertIntGT(cacheSz, wolfSSL_CTX_get_cert_cache_memsize(ctx));
#endif
#endif

#if !defined(NO_WOLFSSL_DIR) && !defined(WOLFSSL_TIRTOS)
    /* Test loading CA certificates using a path */
    #ifdef NO_RSA
    /* failure here okay since certs in external directory are RSA */
    AssertIntNE(wolfSSL_CTX_load_verify_locations_ex(ctx, NULL, load_certs_path,
        WOLFSSL_LOAD_FLAG_PEM_CA_ONLY), WOLFSSL_SUCCESS);
    #else
    AssertIntEQ(wolfSSL_CTX_load_verify_locations_ex(ctx, NULL, load_certs_path,
        WOLFSSL_LOAD_FLAG_PEM_CA_ONLY), WOLFSSL_SUCCESS);
    #endif

    /* Test loading path with no files */
    AssertIntEQ(wolfSSL_CTX_load_verify_locations_ex(ctx, NULL, load_no_certs_path,
        WOLFSSL_LOAD_FLAG_PEM_CA_ONLY), WOLFSSL_FAILURE);

    /* Test loading expired CA certificates */
    #ifdef NO_RSA
    AssertIntNE(wolfSSL_CTX_load_verify_locations_ex(ctx, NULL, load_expired_path,
        WOLFSSL_LOAD_FLAG_DATE_ERR_OKAY | WOLFSSL_LOAD_FLAG_PEM_CA_ONLY),
        WOLFSSL_SUCCESS);
    #else
    AssertIntEQ(wolfSSL_CTX_load_verify_locations_ex(ctx, NULL, load_expired_path,
        WOLFSSL_LOAD_FLAG_DATE_ERR_OKAY | WOLFSSL_LOAD_FLAG_PEM_CA_ONLY),
        WOLFSSL_SUCCESS);
    #endif

    /* Test loading CA certificates and ignoring all errors */
    #ifdef NO_RSA
    AssertIntEQ(wolfSSL_CTX_load_verify_locations_ex(ctx, NULL, load_certs_path,
        WOLFSSL_LOAD_FLAG_IGNORE_ERR), WOLFSSL_FAILURE);
    #else
    AssertIntEQ(wolfSSL_CTX_load_verify_locations_ex(ctx, NULL, load_certs_path,
        WOLFSSL_LOAD_FLAG_IGNORE_ERR), WOLFSSL_SUCCESS);
    #endif
#endif

    wolfSSL_CTX_free(ctx);
#endif
}

#if !defined(NO_FILESYSTEM) && !defined(NO_CERTS)
static int test_cm_load_ca_buffer(const byte* cert_buf, size_t cert_sz, int file_type)
{
    int ret;
    WOLFSSL_CERT_MANAGER* cm = NULL;

    cm = wolfSSL_CertManagerNew();
    if (cm == NULL) {
        printf("test_cm_load_ca failed\n");
        return -1;
    }

    ret = wolfSSL_CertManagerLoadCABuffer(cm, cert_buf, cert_sz, file_type);

    wolfSSL_CertManagerFree(cm);

    return ret;
}

static int test_cm_load_ca_file(const char* ca_cert_file)
{
    int ret = 0;
    byte* cert_buf = NULL;
    size_t cert_sz = 0;
#if defined(WOLFSSL_PEM_TO_DER)
    DerBuffer* pDer = NULL;
#endif

    ret = load_file(ca_cert_file, &cert_buf, &cert_sz);
    if (ret == 0) {
        /* normal test */
        ret = test_cm_load_ca_buffer(cert_buf, cert_sz, WOLFSSL_FILETYPE_PEM);

        if (ret == 0) {
            /* test including null terminator in length */
            ret = test_cm_load_ca_buffer(cert_buf, cert_sz+1, WOLFSSL_FILETYPE_PEM);
        }

    #if defined(WOLFSSL_PEM_TO_DER)
        if (ret == 0) {
            /* test loading DER */
            ret = wc_PemToDer(cert_buf, cert_sz, CA_TYPE, &pDer, NULL, NULL, NULL);
            if (ret == 0 && pDer != NULL) {
                ret = test_cm_load_ca_buffer(pDer->buffer, pDer->length,
                    WOLFSSL_FILETYPE_ASN1);

                wc_FreeDer(&pDer);
            }
        }
    #endif

        free(cert_buf);
    }
    return ret;
}
#endif /* !NO_FILESYSTEM && !NO_CERTS */

static int test_wolfSSL_CertManagerLoadCABuffer(void)
{
    int ret = 0;

#if !defined(NO_FILESYSTEM) && !defined(NO_CERTS)
    const char* ca_cert = "./certs/ca-cert.pem";
    const char* ca_expired_cert = "./certs/test/expired/expired-ca.pem";

    ret = test_cm_load_ca_file(ca_cert);
    #ifdef NO_RSA
    AssertIntEQ(ret, ASN_UNKNOWN_OID_E);
    #else
    AssertIntEQ(ret, WOLFSSL_SUCCESS);
    #endif

    ret = test_cm_load_ca_file(ca_expired_cert);
    #ifdef NO_RSA
    AssertIntEQ(ret, ASN_UNKNOWN_OID_E);
    #else
    AssertIntEQ(ret, ASN_AFTER_DATE_E);
    #endif
#endif

    return ret;
}

static void test_wolfSSL_CertManagerGetCerts(void)
{
#if defined(OPENSSL_ALL) && !defined(NO_CERTS) && \
    !defined(NO_FILESYSTEM) && !defined(NO_RSA) && \
     defined(WOLFSSL_SIGNER_DER_CERT)

    WOLFSSL_CERT_MANAGER* cm = NULL;
    WOLFSSL_STACK* sk = NULL;
    X509* x509 = NULL;
    X509* cert1 = NULL;
    FILE* file1 = NULL;
#ifdef DEBUG_WOLFSSL_VERBOSE
    WOLFSSL_BIO* bio = NULL;
#endif
    int i = 0;

    printf(testingFmt, "wolfSSL_CertManagerGetCerts()");
    AssertNotNull(file1=fopen("./certs/ca-cert.pem", "rb"));

    AssertNotNull(cert1 = wolfSSL_PEM_read_X509(file1, NULL, NULL, NULL));
    fclose(file1);

    AssertNotNull(cm = wolfSSL_CertManagerNew_ex(NULL));
    AssertNull(sk = wolfSSL_CertManagerGetCerts(cm));
    AssertIntEQ(WOLFSSL_SUCCESS, wolfSSL_CertManagerLoadCA(cm,
                "./certs/ca-cert.pem", NULL));

    AssertNotNull(sk = wolfSSL_CertManagerGetCerts(cm));

    for (i = 0; i < sk_X509_num(sk); i++) {
        x509 = sk_X509_value(sk, i);
        AssertIntEQ(0, wolfSSL_X509_cmp(x509, cert1));

#ifdef DEBUG_WOLFSSL_VERBOSE
        bio = BIO_new(wolfSSL_BIO_s_file());
        if (bio != NULL) {
            BIO_set_fp(bio, stdout, BIO_NOCLOSE);
            X509_print(bio, x509);
            BIO_free(bio);
        }
#endif /* DEBUG_WOLFSSL_VERBOSE */
    }
    wolfSSL_X509_free(cert1);
    sk_X509_free(sk);
    wolfSSL_CertManagerFree(cm);
    printf(resultFmt, passed);
#endif /* defined(OPENSSL_ALL) && !defined(NO_CERTS) && \
          !defined(NO_FILESYSTEM) && !defined(NO_RSA) && \
          defined(WOLFSSL_SIGNER_DER_CERT) */
}
static void test_wolfSSL_CertManagerCRL(void)
{
#if !defined(NO_FILESYSTEM) && !defined(NO_CERTS) && defined(HAVE_CRL) && \
    !defined(NO_RSA)

    const char* ca_cert = "./certs/ca-cert.pem";
    const char* crl1     = "./certs/crl/crl.pem";
    const char* crl2     = "./certs/crl/crl2.pem";

    WOLFSSL_CERT_MANAGER* cm = NULL;

    AssertNotNull(cm = wolfSSL_CertManagerNew());
    AssertIntEQ(WOLFSSL_SUCCESS,
        wolfSSL_CertManagerLoadCA(cm, ca_cert, NULL));
    AssertIntEQ(WOLFSSL_SUCCESS,
        wolfSSL_CertManagerLoadCRL(cm, crl1, WOLFSSL_FILETYPE_PEM, 0));
    AssertIntEQ(WOLFSSL_SUCCESS,
        wolfSSL_CertManagerLoadCRL(cm, crl2, WOLFSSL_FILETYPE_PEM, 0));
    wolfSSL_CertManagerFreeCRL(cm);

    AssertIntEQ(WOLFSSL_SUCCESS,
        wolfSSL_CertManagerLoadCRL(cm, crl1, WOLFSSL_FILETYPE_PEM, 0));
    AssertIntEQ(WOLFSSL_SUCCESS,
        wolfSSL_CertManagerLoadCA(cm, ca_cert, NULL));
    wolfSSL_CertManagerFree(cm);

#endif
}

static void test_wolfSSL_CTX_load_verify_locations_ex(void)
{
#if !defined(NO_FILESYSTEM) && !defined(NO_CERTS) && !defined(NO_RSA) && \
    !defined(NO_WOLFSSL_CLIENT)
    WOLFSSL_CTX* ctx;
    const char* ca_cert = "./certs/ca-cert.pem";
    const char* ca_expired_cert = "./certs/test/expired/expired-ca.pem";

    AssertNotNull(ctx = wolfSSL_CTX_new(wolfSSLv23_client_method()));

    /* test good CA */
    AssertTrue(WOLFSSL_SUCCESS ==
        wolfSSL_CTX_load_verify_locations_ex(ctx, ca_cert, NULL,
            WOLFSSL_LOAD_FLAG_NONE));

    /* test expired CA */
    AssertTrue(WOLFSSL_SUCCESS !=
        wolfSSL_CTX_load_verify_locations_ex(ctx, ca_expired_cert, NULL,
            WOLFSSL_LOAD_FLAG_NONE));
    AssertTrue(WOLFSSL_SUCCESS ==
        wolfSSL_CTX_load_verify_locations_ex(ctx, ca_expired_cert, NULL,
            WOLFSSL_LOAD_FLAG_DATE_ERR_OKAY));

    wolfSSL_CTX_free(ctx);

#endif
}

static void test_wolfSSL_CTX_load_verify_buffer_ex(void)
{
#if !defined(NO_FILESYSTEM) && !defined(NO_CERTS) && !defined(NO_RSA) && \
    defined(USE_CERT_BUFFERS_2048)
    WOLFSSL_CTX* ctx;
    const char* ca_expired_cert_file = "./certs/test/expired/expired-ca.der";
    byte ca_expired_cert[TWOK_BUF];
    word32 sizeof_ca_expired_cert;
    XFILE fp;

#ifndef NO_WOLFSSL_CLIENT
    ctx = wolfSSL_CTX_new(wolfSSLv23_client_method());
#else
    ctx = wolfSSL_CTX_new(wolfSSLv23_server_method());
#endif
    AssertNotNull(ctx);

    /* test good CA */
    AssertTrue(WOLFSSL_SUCCESS ==
        wolfSSL_CTX_load_verify_buffer_ex(ctx, ca_cert_der_2048,
            sizeof_ca_cert_der_2048, WOLFSSL_FILETYPE_ASN1, 0,
            WOLFSSL_LOAD_FLAG_NONE));

    /* load expired CA */
    XMEMSET(ca_expired_cert, 0, sizeof(ca_expired_cert));
    fp = XFOPEN(ca_expired_cert_file, "rb");
    AssertTrue(fp != XBADFILE);
    sizeof_ca_expired_cert = (word32)XFREAD(ca_expired_cert, 1,
        sizeof(ca_expired_cert), fp);
    XFCLOSE(fp);

    /* test expired CA failure */
    AssertTrue(WOLFSSL_SUCCESS !=
        wolfSSL_CTX_load_verify_buffer_ex(ctx, ca_expired_cert,
            sizeof_ca_expired_cert, WOLFSSL_FILETYPE_ASN1, 0,
            WOLFSSL_LOAD_FLAG_NONE));

    /* test expired CA success */
    AssertTrue(WOLFSSL_SUCCESS ==
        wolfSSL_CTX_load_verify_buffer_ex(ctx, ca_expired_cert,
            sizeof_ca_expired_cert, WOLFSSL_FILETYPE_ASN1, 0,
            WOLFSSL_LOAD_FLAG_DATE_ERR_OKAY));

    wolfSSL_CTX_free(ctx);

#endif
}

static void test_wolfSSL_CTX_load_verify_chain_buffer_format(void)
{
#if !defined(NO_CERTS) && !defined(NO_RSA) && defined(OPENSSL_EXTRA) && \
    defined(WOLFSSL_CERT_GEN) && defined(USE_CERT_BUFFERS_2048)
    WOLFSSL_CTX* ctx;

  #ifndef NO_WOLFSSL_CLIENT
    AssertNotNull(ctx = wolfSSL_CTX_new(wolfSSLv23_client_method()));
  #else
    AssertNotNull(ctx = wolfSSL_CTX_new(wolfSSLv23_server_method()));
  #endif

    AssertTrue(WOLFSSL_SUCCESS ==
               wolfSSL_CTX_load_verify_chain_buffer_format(ctx, ca_cert_chain_der,
                                                           sizeof_ca_cert_chain_der,
                                                           WOLFSSL_FILETYPE_ASN1));

    wolfSSL_CTX_free(ctx);
#endif
}

static int test_wolfSSL_CTX_use_certificate_chain_file_format(void)
{
    int ret = 0;
#if !defined(NO_FILESYSTEM) && !defined(NO_CERTS) && !defined(NO_RSA)
    const char* server_chain_der = "./certs/server-cert-chain.der";
    WOLFSSL_CTX* ctx;

  #ifndef NO_WOLFSSL_CLIENT
    AssertNotNull(ctx = wolfSSL_CTX_new(wolfSSLv23_client_method()));
  #else
    AssertNotNull(ctx = wolfSSL_CTX_new(wolfSSLv23_server_method()));
  #endif

    AssertIntEQ(wolfSSL_CTX_use_certificate_chain_file_format(ctx,
        server_chain_der, WOLFSSL_FILETYPE_ASN1), WOLFSSL_SUCCESS);

    wolfSSL_CTX_free(ctx);
#endif
    return ret;
}

static void test_wolfSSL_CTX_SetTmpDH_file(void)
{
#if !defined(NO_FILESYSTEM) && !defined(NO_CERTS) && !defined(NO_DH)
    WOLFSSL_CTX *ctx;

  #ifndef NO_WOLFSSL_CLIENT
    AssertNotNull(ctx = wolfSSL_CTX_new(wolfSSLv23_client_method()));
  #else
    AssertNotNull(ctx = wolfSSL_CTX_new(wolfSSLv23_server_method()));
  #endif

    /* invalid context */
    AssertIntNE(WOLFSSL_SUCCESS, wolfSSL_CTX_SetTmpDH_file(NULL,
                dhParamFile, WOLFSSL_FILETYPE_PEM));

    /* invalid dhParamFile file */
    AssertIntNE(WOLFSSL_SUCCESS, wolfSSL_CTX_SetTmpDH_file(ctx,
                NULL, WOLFSSL_FILETYPE_PEM));
    AssertIntNE(WOLFSSL_SUCCESS, wolfSSL_CTX_SetTmpDH_file(ctx,
                bogusFile, WOLFSSL_FILETYPE_PEM));

    /* success */
    AssertIntEQ(WOLFSSL_SUCCESS, wolfSSL_CTX_SetTmpDH_file(ctx, dhParamFile,
                WOLFSSL_FILETYPE_PEM));

    wolfSSL_CTX_free(ctx);
#endif
}

static void test_wolfSSL_CTX_SetTmpDH_buffer(void)
{
#if !defined(NO_CERTS) && !defined(NO_DH)
    WOLFSSL_CTX *ctx;

  #ifndef NO_WOLFSSL_CLIENT
    AssertNotNull(ctx = wolfSSL_CTX_new(wolfSSLv23_client_method()));
  #else
    AssertNotNull(ctx = wolfSSL_CTX_new(wolfSSLv23_server_method()));
  #endif

    /* invalid context */
    AssertIntNE(WOLFSSL_SUCCESS, wolfSSL_CTX_SetTmpDH_buffer(NULL, dh_key_der_2048,
                sizeof_dh_key_der_2048, WOLFSSL_FILETYPE_ASN1));

    /* invalid dhParamFile file */
    AssertIntNE(WOLFSSL_SUCCESS, wolfSSL_CTX_SetTmpDH_buffer(NULL, NULL,
                0, WOLFSSL_FILETYPE_ASN1));
    AssertIntNE(WOLFSSL_SUCCESS, wolfSSL_CTX_SetTmpDH_buffer(ctx, dsa_key_der_2048,
                sizeof_dsa_key_der_2048, WOLFSSL_FILETYPE_ASN1));

    /* success */
    AssertIntEQ(WOLFSSL_SUCCESS, wolfSSL_CTX_SetTmpDH_buffer(ctx, dh_key_der_2048,
                sizeof_dh_key_der_2048, WOLFSSL_FILETYPE_ASN1));

    wolfSSL_CTX_free(ctx);
#endif
}

static void test_wolfSSL_CTX_SetMinMaxDhKey_Sz(void)
{
#if !defined(NO_CERTS) && !defined(NO_DH)
    WOLFSSL_CTX *ctx;

  #ifndef NO_WOLFSSL_CLIENT
    AssertNotNull(ctx = wolfSSL_CTX_new(wolfSSLv23_client_method()));
  #else
    AssertNotNull(ctx = wolfSSL_CTX_new(wolfSSLv23_server_method()));
  #endif

    AssertIntEQ(WOLFSSL_SUCCESS, wolfSSL_CTX_SetMinDhKey_Sz(ctx, 3072));

    AssertIntEQ(DH_KEY_SIZE_E, wolfSSL_CTX_SetTmpDH_buffer(ctx, dh_key_der_2048,
                sizeof_dh_key_der_2048, WOLFSSL_FILETYPE_ASN1));

    AssertIntEQ(WOLFSSL_SUCCESS, wolfSSL_CTX_SetMinDhKey_Sz(ctx, 2048));

    AssertIntEQ(WOLFSSL_SUCCESS, wolfSSL_CTX_SetTmpDH_buffer(ctx, dh_key_der_2048,
                sizeof_dh_key_der_2048, WOLFSSL_FILETYPE_ASN1));

    AssertIntEQ(WOLFSSL_SUCCESS, wolfSSL_CTX_SetMaxDhKey_Sz(ctx, 1024));

    AssertIntEQ(DH_KEY_SIZE_E, wolfSSL_CTX_SetTmpDH_buffer(ctx, dh_key_der_2048,
                sizeof_dh_key_der_2048, WOLFSSL_FILETYPE_ASN1));

    AssertIntEQ(WOLFSSL_SUCCESS, wolfSSL_CTX_SetMaxDhKey_Sz(ctx, 2048));

    AssertIntEQ(WOLFSSL_SUCCESS, wolfSSL_CTX_SetTmpDH_buffer(ctx, dh_key_der_2048,
                sizeof_dh_key_der_2048, WOLFSSL_FILETYPE_ASN1));

    wolfSSL_CTX_free(ctx);
#endif
}

static void test_wolfSSL_CTX_der_load_verify_locations(void)
{
#ifdef WOLFSSL_DER_LOAD
    WOLFSSL_CTX* ctx = NULL;
    const char* derCert = "./certs/server-cert.der";
    const char* nullPath = NULL;
    const char* invalidPath = "./certs/this-cert-does-not-exist.der";
    const char* emptyPath = "";

    /* der load Case 1 ctx NULL */
    AssertIntEQ(wolfSSL_CTX_der_load_verify_locations(ctx, derCert,
                WOLFSSL_FILETYPE_ASN1), WOLFSSL_FAILURE);

  #ifndef NO_WOLFSSL_CLIENT
    AssertNotNull(ctx = wolfSSL_CTX_new(wolfSSLv23_client_method()));
  #else
    AssertNotNull(ctx = wolfSSL_CTX_new(wolfSSLv23_server_method()));
  #endif

    /* Case 2 filePath NULL */
    AssertIntEQ(wolfSSL_CTX_der_load_verify_locations(ctx, nullPath,
                WOLFSSL_FILETYPE_ASN1), WOLFSSL_FAILURE);
    /* Case 3 invalid format */
    AssertIntEQ(wolfSSL_CTX_der_load_verify_locations(ctx, derCert,
                WOLFSSL_FILETYPE_PEM), WOLFSSL_FAILURE);
    /* Case 4 filePath not valid */
    AssertIntEQ(wolfSSL_CTX_der_load_verify_locations(ctx, invalidPath,
                WOLFSSL_FILETYPE_ASN1), WOLFSSL_FAILURE);
    /* Case 5 filePath empty */
    AssertIntEQ(wolfSSL_CTX_der_load_verify_locations(ctx, emptyPath,
                WOLFSSL_FILETYPE_ASN1), WOLFSSL_FAILURE);
#ifndef NO_RSA
    /* Case 6 success case */
    AssertIntEQ(wolfSSL_CTX_der_load_verify_locations(ctx, derCert,
                WOLFSSL_FILETYPE_ASN1), WOLFSSL_SUCCESS);
#endif

    wolfSSL_CTX_free(ctx);
#endif
}

static void test_wolfSSL_CTX_enable_disable(void)
{
#ifndef NO_CERTS
    WOLFSSL_CTX* ctx = NULL;

  #ifdef HAVE_CRL
    AssertIntEQ(wolfSSL_CTX_DisableCRL(ctx), BAD_FUNC_ARG);
    AssertIntEQ(wolfSSL_CTX_EnableCRL(ctx, 0), BAD_FUNC_ARG);
  #endif

  #ifdef HAVE_OCSP
    AssertIntEQ(wolfSSL_CTX_DisableOCSP(ctx), BAD_FUNC_ARG);
    AssertIntEQ(wolfSSL_CTX_EnableOCSP(ctx, 0), BAD_FUNC_ARG);
  #endif

  #if defined(HAVE_CERTIFICATE_STATUS_REQUEST) || \
      defined(HAVE_CERTIFICATE_STATUS_REQUEST_V2)
    AssertIntEQ(wolfSSL_CTX_DisableOCSPStapling(ctx), BAD_FUNC_ARG);
    AssertIntEQ(wolfSSL_CTX_EnableOCSPStapling(ctx), BAD_FUNC_ARG);
  #endif

  #ifndef NO_WOLFSSL_CLIENT

    #ifdef HAVE_EXTENDED_MASTER
    AssertIntEQ(wolfSSL_CTX_DisableExtendedMasterSecret(ctx), BAD_FUNC_ARG);
    #endif

    AssertNotNull(ctx = wolfSSL_CTX_new(wolfSSLv23_client_method()));

    #ifdef HAVE_EXTENDED_MASTER
    AssertIntEQ(wolfSSL_CTX_DisableExtendedMasterSecret(ctx), WOLFSSL_SUCCESS);
    #endif

  #elif !defined(NO_WOLFSSL_SERVER)
    AssertNotNull(ctx = wolfSSL_CTX_new(wolfSSLv23_server_method()));
  #else
    return;
  #endif

  #ifdef HAVE_CRL
    AssertIntEQ(wolfSSL_CTX_DisableCRL(ctx), WOLFSSL_SUCCESS);
    AssertIntEQ(wolfSSL_CTX_EnableCRL(ctx, 0), WOLFSSL_SUCCESS);
  #endif

  #ifdef HAVE_OCSP
    AssertIntEQ(wolfSSL_CTX_DisableOCSP(ctx), WOLFSSL_SUCCESS);
    AssertIntEQ(wolfSSL_CTX_EnableOCSP(ctx, WOLFSSL_OCSP_URL_OVERRIDE),
                WOLFSSL_SUCCESS);
    AssertIntEQ(wolfSSL_CTX_EnableOCSP(ctx, WOLFSSL_OCSP_NO_NONCE),
                WOLFSSL_SUCCESS);
    AssertIntEQ(wolfSSL_CTX_EnableOCSP(ctx, WOLFSSL_OCSP_CHECKALL),
                WOLFSSL_SUCCESS);
  #endif

  #if defined(HAVE_CERTIFICATE_STATUS_REQUEST) || \
      defined(HAVE_CERTIFICATE_STATUS_REQUEST_V2)
    AssertIntEQ(wolfSSL_CTX_DisableOCSPStapling(ctx), WOLFSSL_SUCCESS);
    AssertIntEQ(wolfSSL_CTX_EnableOCSPStapling(ctx), WOLFSSL_SUCCESS);
  #endif
    wolfSSL_CTX_free(ctx);
#endif /* NO_CERTS */
}
/*----------------------------------------------------------------------------*
 | SSL
 *----------------------------------------------------------------------------*/

static void test_server_wolfSSL_new(void)
{
#if !defined(NO_FILESYSTEM) && !defined(NO_CERTS) && !defined(NO_RSA) && \
        !defined(NO_WOLFSSL_SERVER)
    WOLFSSL_CTX *ctx;
    WOLFSSL_CTX *ctx_nocert;
    WOLFSSL *ssl;

    AssertNotNull(ctx_nocert = wolfSSL_CTX_new(wolfSSLv23_server_method()));
    AssertNotNull(ctx        = wolfSSL_CTX_new(wolfSSLv23_server_method()));

    AssertTrue(wolfSSL_CTX_use_certificate_file(ctx, svrCertFile, WOLFSSL_FILETYPE_PEM));
    AssertTrue(wolfSSL_CTX_use_PrivateKey_file(ctx, svrKeyFile, WOLFSSL_FILETYPE_PEM));

    /* invalid context */
    AssertNull(ssl = wolfSSL_new(NULL));
#ifndef WOLFSSL_SESSION_EXPORT
    AssertNull(ssl = wolfSSL_new(ctx_nocert));
#endif

    /* success */
    AssertNotNull(ssl = wolfSSL_new(ctx));

    wolfSSL_free(ssl);
    wolfSSL_CTX_free(ctx);
    wolfSSL_CTX_free(ctx_nocert);
#endif
}


static void test_client_wolfSSL_new(void)
{
#if !defined(NO_FILESYSTEM) && !defined(NO_CERTS) && !defined(NO_RSA) && \
        !defined(NO_WOLFSSL_CLIENT)
    WOLFSSL_CTX *ctx;
    WOLFSSL_CTX *ctx_nocert;
    WOLFSSL *ssl;

    AssertNotNull(ctx_nocert = wolfSSL_CTX_new(wolfSSLv23_client_method()));
    AssertNotNull(ctx        = wolfSSL_CTX_new(wolfSSLv23_client_method()));

    AssertTrue(wolfSSL_CTX_load_verify_locations(ctx, caCertFile, 0));

    /* invalid context */
    AssertNull(ssl = wolfSSL_new(NULL));

    /* success */
    AssertNotNull(ssl = wolfSSL_new(ctx_nocert));
    wolfSSL_free(ssl);

    /* success */
    AssertNotNull(ssl = wolfSSL_new(ctx));
    wolfSSL_free(ssl);

    wolfSSL_CTX_free(ctx);
    wolfSSL_CTX_free(ctx_nocert);
#endif
}

static void test_wolfSSL_SetTmpDH_file(void)
{
#if !defined(NO_FILESYSTEM) && !defined(NO_CERTS) && !defined(NO_DH) && \
        !defined(NO_WOLFSSL_SERVER)
    WOLFSSL_CTX *ctx;
    WOLFSSL *ssl;

    AssertNotNull(ctx = wolfSSL_CTX_new(wolfSSLv23_server_method()));
#ifndef NO_RSA
    AssertTrue(wolfSSL_CTX_use_certificate_file(ctx, svrCertFile,
                WOLFSSL_FILETYPE_PEM));
    AssertTrue(wolfSSL_CTX_use_PrivateKey_file(ctx, svrKeyFile,
                WOLFSSL_FILETYPE_PEM));
#elif defined(HAVE_ECC)
    AssertTrue(wolfSSL_CTX_use_certificate_file(ctx, eccCertFile,
                WOLFSSL_FILETYPE_PEM));
    AssertTrue(wolfSSL_CTX_use_PrivateKey_file(ctx, eccKeyFile,
                WOLFSSL_FILETYPE_PEM));
#elif defined(HAVE_ED25519)
    AssertTrue(wolfSSL_CTX_use_certificate_file(ctx, edCertFile,
                WOLFSSL_FILETYPE_PEM));
    AssertTrue(wolfSSL_CTX_use_PrivateKey_file(ctx, edKeyFile,
                WOLFSSL_FILETYPE_PEM));
#endif
    AssertNotNull(ssl = wolfSSL_new(ctx));

    /* invalid ssl */
    AssertIntNE(WOLFSSL_SUCCESS, wolfSSL_SetTmpDH_file(NULL,
                dhParamFile, WOLFSSL_FILETYPE_PEM));

    /* invalid dhParamFile file */
    AssertIntNE(WOLFSSL_SUCCESS, wolfSSL_SetTmpDH_file(ssl,
                NULL, WOLFSSL_FILETYPE_PEM));
    AssertIntNE(WOLFSSL_SUCCESS, wolfSSL_SetTmpDH_file(ssl,
                bogusFile, WOLFSSL_FILETYPE_PEM));

    /* success */
    AssertIntEQ(WOLFSSL_SUCCESS, wolfSSL_SetTmpDH_file(ssl, dhParamFile,
                WOLFSSL_FILETYPE_PEM));

    wolfSSL_free(ssl);
    wolfSSL_CTX_free(ctx);
#endif
}

static void test_wolfSSL_SetTmpDH_buffer(void)
{
#if !defined(NO_CERTS) && !defined(NO_DH) && !defined(NO_WOLFSSL_SERVER)
    WOLFSSL_CTX *ctx;
    WOLFSSL *ssl;

    AssertNotNull(ctx = wolfSSL_CTX_new(wolfSSLv23_server_method()));
    AssertTrue(wolfSSL_CTX_use_certificate_buffer(ctx, server_cert_der_2048,
                sizeof_server_cert_der_2048, WOLFSSL_FILETYPE_ASN1));
    AssertTrue(wolfSSL_CTX_use_PrivateKey_buffer(ctx, server_key_der_2048,
                sizeof_server_key_der_2048, WOLFSSL_FILETYPE_ASN1));
    AssertNotNull(ssl = wolfSSL_new(ctx));

    /* invalid ssl */
    AssertIntNE(WOLFSSL_SUCCESS, wolfSSL_SetTmpDH_buffer(NULL, dh_key_der_2048,
                sizeof_dh_key_der_2048, WOLFSSL_FILETYPE_ASN1));

    /* invalid dhParamFile file */
    AssertIntNE(WOLFSSL_SUCCESS, wolfSSL_SetTmpDH_buffer(NULL, NULL,
                0, WOLFSSL_FILETYPE_ASN1));
    AssertIntNE(WOLFSSL_SUCCESS, wolfSSL_SetTmpDH_buffer(ssl, dsa_key_der_2048,
                sizeof_dsa_key_der_2048, WOLFSSL_FILETYPE_ASN1));

    /* success */
    AssertIntEQ(WOLFSSL_SUCCESS, wolfSSL_SetTmpDH_buffer(ssl, dh_key_der_2048,
                sizeof_dh_key_der_2048, WOLFSSL_FILETYPE_ASN1));

    wolfSSL_free(ssl);
    wolfSSL_CTX_free(ctx);
#endif
}

static void test_wolfSSL_SetMinMaxDhKey_Sz(void)
{
#if !defined(NO_CERTS) && !defined(NO_DH) && !defined(NO_WOLFSSL_SERVER)
    WOLFSSL_CTX *ctx, *ctx2;
    WOLFSSL *ssl, *ssl2;

    AssertNotNull(ctx = wolfSSL_CTX_new(wolfSSLv23_server_method()));
    AssertTrue(wolfSSL_CTX_use_certificate_buffer(ctx, server_cert_der_2048,
                sizeof_server_cert_der_2048, WOLFSSL_FILETYPE_ASN1));
    AssertTrue(wolfSSL_CTX_use_PrivateKey_buffer(ctx, server_key_der_2048,
                sizeof_server_key_der_2048, WOLFSSL_FILETYPE_ASN1));
    AssertIntEQ(WOLFSSL_SUCCESS, wolfSSL_CTX_SetMinDhKey_Sz(ctx, 3072));
    AssertNotNull(ssl = wolfSSL_new(ctx));
    AssertNotNull(ctx2 = wolfSSL_CTX_new(wolfSSLv23_server_method()));
    AssertTrue(wolfSSL_CTX_use_certificate_buffer(ctx2, server_cert_der_2048,
                sizeof_server_cert_der_2048, WOLFSSL_FILETYPE_ASN1));
    AssertTrue(wolfSSL_CTX_use_PrivateKey_buffer(ctx2, server_key_der_2048,
                sizeof_server_key_der_2048, WOLFSSL_FILETYPE_ASN1));
    AssertIntEQ(WOLFSSL_SUCCESS, wolfSSL_CTX_SetMaxDhKey_Sz(ctx, 1024));
    AssertNotNull(ssl2 = wolfSSL_new(ctx2));

    AssertIntEQ(DH_KEY_SIZE_E, wolfSSL_SetTmpDH_buffer(ssl, dh_key_der_2048,
                sizeof_dh_key_der_2048, WOLFSSL_FILETYPE_ASN1));

    AssertIntEQ(WOLFSSL_SUCCESS, wolfSSL_SetMinDhKey_Sz(ssl, 2048));
    AssertIntEQ(WOLFSSL_SUCCESS, wolfSSL_SetTmpDH_buffer(ssl, dh_key_der_2048,
                sizeof_dh_key_der_2048, WOLFSSL_FILETYPE_ASN1));

    AssertIntEQ(WOLFSSL_SUCCESS, wolfSSL_SetMinDhKey_Sz(ssl, 3072));
    AssertIntEQ(DH_KEY_SIZE_E, wolfSSL_SetTmpDH_buffer(ssl, dh_key_der_2048,
                sizeof_dh_key_der_2048, WOLFSSL_FILETYPE_ASN1));

    AssertIntEQ(WOLFSSL_SUCCESS, wolfSSL_SetTmpDH_buffer(ssl2, dh_key_der_2048,
                sizeof_dh_key_der_2048, WOLFSSL_FILETYPE_ASN1));

    AssertIntEQ(WOLFSSL_SUCCESS, wolfSSL_SetMaxDhKey_Sz(ssl2, 2048));
    AssertIntEQ(WOLFSSL_SUCCESS, wolfSSL_SetTmpDH_buffer(ssl2, dh_key_der_2048,
                sizeof_dh_key_der_2048, WOLFSSL_FILETYPE_ASN1));

    AssertIntEQ(WOLFSSL_SUCCESS, wolfSSL_SetMaxDhKey_Sz(ssl2, 1024));
    AssertIntEQ(DH_KEY_SIZE_E, wolfSSL_SetTmpDH_buffer(ssl, dh_key_der_2048,
                sizeof_dh_key_der_2048, WOLFSSL_FILETYPE_ASN1));

    wolfSSL_free(ssl2);
    wolfSSL_CTX_free(ctx2);
    wolfSSL_free(ssl);
    wolfSSL_CTX_free(ctx);
#endif
}


/* Test function for wolfSSL_SetMinVersion. Sets the minimum downgrade version
 * allowed.
 * POST: return 1 on success.
 */
static int test_wolfSSL_SetMinVersion(void)
{
    int                 failFlag = WOLFSSL_SUCCESS;
#ifndef NO_WOLFSSL_CLIENT
    WOLFSSL_CTX*        ctx;
    WOLFSSL*            ssl;
    int                 itr;

    #ifndef NO_OLD_TLS
        const int versions[]  =  {
                            #ifdef WOLFSSL_ALLOW_TLSV10
                                   WOLFSSL_TLSV1,
                            #endif
                                   WOLFSSL_TLSV1_1,
                                   WOLFSSL_TLSV1_2};
    #elif !defined(WOLFSSL_NO_TLS12)
        const int versions[]  =  { WOLFSSL_TLSV1_2 };
    #else
        const int versions[]  =  { WOLFSSL_TLSV1_3 };
    #endif

    ctx = wolfSSL_CTX_new(wolfSSLv23_client_method());
    ssl = wolfSSL_new(ctx);

    printf(testingFmt, "wolfSSL_SetMinVersion()");

    for (itr = 0; itr < (int)(sizeof(versions)/sizeof(int)); itr++){
       if(wolfSSL_SetMinVersion(ssl, *(versions + itr)) != WOLFSSL_SUCCESS){
            failFlag = WOLFSSL_FAILURE;
        }
    }

    printf(resultFmt, failFlag == WOLFSSL_SUCCESS ? passed : failed);

    wolfSSL_free(ssl);
    wolfSSL_CTX_free(ctx);
#endif
    return failFlag;

} /* END test_wolfSSL_SetMinVersion */


/*----------------------------------------------------------------------------*
 | EC
 *----------------------------------------------------------------------------*/

/* Test function for EC_POINT_new, EC_POINT_mul, EC_POINT_free,
    EC_GROUP_new_by_curve_name, EC_GROUP_order_bits
 */

# if defined(OPENSSL_EXTRA)
static void test_wolfSSL_EC(void)
{
#ifdef HAVE_ECC
    BN_CTX *ctx;
    EC_GROUP *group;
    EC_POINT *Gxy, *new_point;
    BIGNUM *k = NULL, *Gx = NULL, *Gy = NULL, *Gz = NULL;
    BIGNUM *X, *Y;
    char* hexStr;
    int group_bits;

    const char* kTest = "F4F8338AFCC562C5C3F3E1E46A7EFECD17AF381913FF7A96314EA47055EA0FD0";
    /* NISTP256R1 Gx/Gy */
    const char* kGx   = "6B17D1F2E12C4247F8BCE6E563A440F277037D812DEB33A0F4A13945D898C296";
    const char* kGy   = "4FE342E2FE1A7F9B8EE7EB4A7C0F9E162BCE33576B315ECECBB6406837BF51F5";

#ifndef HAVE_SELFTEST
    const char* uncompG   = "046B17D1F2E12C4247F8BCE6E563A440F277037D812DEB33A0F4A13945D898C2964FE342E2FE1A7F9B8EE7EB4A7C0F9E162BCE33576B315ECECBB6406837BF51F5";
    const char* compG   = "036B17D1F2E12C4247F8BCE6E563A440F277037D812DEB33A0F4A13945D898C296";
#endif

    AssertNotNull(ctx = BN_CTX_new());
    AssertNotNull(group = EC_GROUP_new_by_curve_name(NID_X9_62_prime256v1));
    AssertIntEQ((group_bits = EC_GROUP_order_bits(group)), 256);
    AssertNotNull(Gxy = EC_POINT_new(group));
    AssertNotNull(new_point = EC_POINT_new(group));
    AssertNotNull(X = BN_new());
    AssertNotNull(Y = BN_new());

    /* load test values */
    AssertIntEQ(BN_hex2bn(&k,  kTest), WOLFSSL_SUCCESS);
    AssertIntEQ(BN_hex2bn(&Gx, kGx),   WOLFSSL_SUCCESS);
    AssertIntEQ(BN_hex2bn(&Gy, kGy),   WOLFSSL_SUCCESS);
    AssertIntEQ(BN_hex2bn(&Gz, "1"),   WOLFSSL_SUCCESS);

    /* populate coordinates for input point */
    Gxy->X = Gx;
    Gxy->Y = Gy;
    Gxy->Z = Gz;

    /* perform point multiplication */
    AssertIntEQ(EC_POINT_mul(group, new_point, NULL, Gxy, k, ctx), WOLFSSL_SUCCESS);

    /* check if point X coordinate is zero */
    AssertIntEQ(BN_is_zero(new_point->X), WOLFSSL_FAILURE);

    /* extract the coordinates from point */
    AssertIntEQ(EC_POINT_get_affine_coordinates_GFp(group, new_point, X, Y, ctx), WOLFSSL_SUCCESS);

    /* check if point X coordinate is zero */
    AssertIntEQ(BN_is_zero(X), WOLFSSL_FAILURE);

    /* check bn2hex */
    hexStr = BN_bn2hex(k);
    AssertStrEQ(hexStr, kTest);
#ifndef NO_FILESYSTEM
    BN_print_fp(stdout, k);
    printf("\n");
#endif
    XFREE(hexStr, NULL, DYNAMIC_TYPE_ECC);

    hexStr = BN_bn2hex(Gx);
    AssertStrEQ(hexStr, kGx);
#ifndef NO_FILESYSTEM
    BN_print_fp(stdout, Gx);
    printf("\n");
#endif
    XFREE(hexStr, NULL, DYNAMIC_TYPE_ECC);

    hexStr = BN_bn2hex(Gy);
    AssertStrEQ(hexStr, kGy);
#ifndef NO_FILESYSTEM
    BN_print_fp(stdout, Gy);
    printf("\n");
#endif
    XFREE(hexStr, NULL, DYNAMIC_TYPE_ECC);

#ifndef HAVE_SELFTEST
    hexStr = EC_POINT_point2hex(group, Gxy, POINT_CONVERSION_UNCOMPRESSED, ctx);
    AssertStrEQ(hexStr, uncompG);
    XFREE(hexStr, NULL, DYNAMIC_TYPE_ECC);

    hexStr = EC_POINT_point2hex(group, Gxy, POINT_CONVERSION_COMPRESSED, ctx);
    AssertStrEQ(hexStr, compG);
    XFREE(hexStr, NULL, DYNAMIC_TYPE_ECC);
#endif

    /* cleanup */
    BN_free(X);
    BN_free(Y);
    BN_free(k);
    EC_POINT_free(new_point);
    EC_POINT_free(Gxy);
    EC_GROUP_free(group);
    BN_CTX_free(ctx);
#endif /* HAVE_ECC */
}
#endif

static void test_wolfSSL_PEM_read_bio_ECPKParameters(void)
{
#if defined(HAVE_ECC) && !defined(NO_FILESYSTEM) && defined(OPENSSL_EXTRA)
    EC_GROUP *group;
    BIO* bio;

    AssertNotNull(bio = BIO_new(BIO_s_file()));
    AssertIntEQ(BIO_read_filename(bio, eccKeyFile), WOLFSSL_SUCCESS);
    AssertNotNull(group = PEM_read_bio_ECPKParameters(bio, NULL, NULL, NULL));
    AssertIntEQ(EC_GROUP_get_curve_name(group), NID_X9_62_prime256v1);
    EC_GROUP_free(group);
    BIO_free(bio);
#endif /* HAVE_ECC */
}

# if defined(OPENSSL_EXTRA)
static void test_wolfSSL_ECDSA_SIG(void)
{
#ifdef HAVE_ECC
    WOLFSSL_ECDSA_SIG* sig = NULL;
    WOLFSSL_ECDSA_SIG* sig2 = NULL;
    const unsigned char* cp;
    unsigned char* p;
    unsigned char outSig[8];
    unsigned char sigData[8] =
                             { 0x30, 0x06, 0x02, 0x01, 0x01, 0x02, 0x01, 0x01 };

    AssertNull(wolfSSL_d2i_ECDSA_SIG(NULL, NULL, sizeof(sigData)));
    cp = sigData;
    AssertNotNull((sig = wolfSSL_d2i_ECDSA_SIG(NULL, &cp, sizeof(sigData))));
    AssertIntEQ((cp == sigData + 8), 1);
    cp = sigData;
    AssertNull(wolfSSL_d2i_ECDSA_SIG(&sig, NULL, sizeof(sigData)));
    AssertNotNull((sig2 = wolfSSL_d2i_ECDSA_SIG(&sig, &cp, sizeof(sigData))));
    AssertIntEQ((sig == sig2), 1);
    cp = outSig;

    p = outSig;
    AssertIntEQ(wolfSSL_i2d_ECDSA_SIG(NULL, &p), 0);
    AssertIntEQ(wolfSSL_i2d_ECDSA_SIG(NULL, NULL), 0);
    AssertIntEQ(wolfSSL_i2d_ECDSA_SIG(sig, NULL), 8);
    AssertIntEQ(wolfSSL_i2d_ECDSA_SIG(sig, &p), sizeof(sigData));
    AssertIntEQ((p == outSig + 8), 1);
    AssertIntEQ(XMEMCMP(sigData, outSig, 8), 0);

    wolfSSL_ECDSA_SIG_free(sig);
#endif /* HAVE_ECC */
}
#endif /* OPENSSL_EXTRA */

#include <wolfssl/openssl/pem.h>
/*----------------------------------------------------------------------------*
 | EVP
 *----------------------------------------------------------------------------*/

/* Test function for wolfSSL_EVP_get_cipherbynid.
 */

#ifdef OPENSSL_EXTRA
static void test_wolfSSL_EVP_get_cipherbynid(void)
{
#ifndef NO_AES
    const WOLFSSL_EVP_CIPHER* c;

    c = wolfSSL_EVP_get_cipherbynid(419);
    #if defined(HAVE_AES_CBC) && defined(WOLFSSL_AES_128)
        AssertNotNull(c);
        AssertNotNull(strcmp("EVP_AES_128_CBC", c));
    #else
        AssertNull(c);
    #endif

    c = wolfSSL_EVP_get_cipherbynid(423);
    #if defined(HAVE_AES_CBC) && defined(WOLFSSL_AES_192)
        AssertNotNull(c);
        AssertNotNull(strcmp("EVP_AES_192_CBC", c));
    #else
        AssertNull(c);
    #endif

    c = wolfSSL_EVP_get_cipherbynid(427);
    #if defined(HAVE_AES_CBC) && defined(WOLFSSL_AES_256)
        AssertNotNull(c);
        AssertNotNull(strcmp("EVP_AES_256_CBC", c));
    #else
        AssertNull(c);
    #endif

    c = wolfSSL_EVP_get_cipherbynid(904);
    #if defined(WOLFSSL_AES_COUNTER) && defined(WOLFSSL_AES_128)
        AssertNotNull(c);
        AssertNotNull(strcmp("EVP_AES_128_CTR", c));
    #else
        AssertNull(c);
    #endif

    c = wolfSSL_EVP_get_cipherbynid(905);
    #if defined(WOLFSSL_AES_COUNTER) && defined(WOLFSSL_AES_192)
        AssertNotNull(c);
        AssertNotNull(strcmp("EVP_AES_192_CTR", c));
    #else
        AssertNull(c);
    #endif

    c = wolfSSL_EVP_get_cipherbynid(906);
    #if defined(WOLFSSL_AES_COUNTER) && defined(WOLFSSL_AES_256)
        AssertNotNull(c);
        AssertNotNull(strcmp("EVP_AES_256_CTR", c));
    #else
        AssertNull(c);
    #endif

    c = wolfSSL_EVP_get_cipherbynid(418);
    #if defined(HAVE_AES_ECB) && defined(WOLFSSL_AES_128)
        AssertNotNull(c);
        AssertNotNull(strcmp("EVP_AES_128_ECB", c));
    #else
        AssertNull(c);
    #endif

    c = wolfSSL_EVP_get_cipherbynid(422);
    #if defined(HAVE_AES_ECB) && defined(WOLFSSL_AES_192)
        AssertNotNull(c);
        AssertNotNull(strcmp("EVP_AES_192_ECB", c));
    #else
        AssertNull(c);
    #endif

    c = wolfSSL_EVP_get_cipherbynid(426);
    #if defined(HAVE_AES_ECB) && defined(WOLFSSL_AES_256)
        AssertNotNull(c);
        AssertNotNull(strcmp("EVP_AES_256_ECB", c));
    #else
        AssertNull(c);
    #endif
#endif /* !NO_AES */

#ifndef NO_DES3
    AssertNotNull(strcmp("EVP_DES_CBC", wolfSSL_EVP_get_cipherbynid(31)));
#ifdef WOLFSSL_DES_ECB
    AssertNotNull(strcmp("EVP_DES_ECB", wolfSSL_EVP_get_cipherbynid(29)));
#endif
    AssertNotNull(strcmp("EVP_DES_EDE3_CBC", wolfSSL_EVP_get_cipherbynid(44)));
#ifdef WOLFSSL_DES_ECB
    AssertNotNull(strcmp("EVP_DES_EDE3_ECB", wolfSSL_EVP_get_cipherbynid(33)));
#endif
#endif /* !NO_DES3 */

#ifdef HAVE_IDEA
    AssertNotNull(strcmp("EVP_IDEA_CBC", wolfSSL_EVP_get_cipherbynid(34)));
#endif

  /* test for nid is out of range */
  AssertNull(wolfSSL_EVP_get_cipherbynid(1));

}

static void test_wolfSSL_EVP_CIPHER_CTX()
{
#if !defined(NO_AES) && defined(HAVE_AES_CBC) && defined(WOLFSSL_AES_128)
    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    const EVP_CIPHER *init = EVP_aes_128_cbc();
    const EVP_CIPHER *test = NULL;
    byte key[AES_BLOCK_SIZE] = {0};
    byte iv[AES_BLOCK_SIZE] = {0};

    AssertNotNull(ctx);
    wolfSSL_EVP_CIPHER_CTX_init(ctx);
    AssertIntEQ(EVP_CipherInit(ctx, init, key, iv, 1), WOLFSSL_SUCCESS);
    test = EVP_CIPHER_CTX_cipher(ctx);
    AssertTrue(init == test);
    AssertIntEQ(EVP_CIPHER_nid(test), NID_aes_128_cbc);

    EVP_CIPHER_CTX_free(ctx);
#endif /* !NO_AES && HAVE_AES_CBC && WOLFSSL_AES_128 */
}
#endif /* OPENSSL_EXTRA */

#if defined(OPENSSL_EXTRA)
/* Test function for various EVP_cipher methods.
 */
static void test_wolfSSL_EVP_CIPHER_CTX(void)
{
#if !defined(NO_AES) && defined(HAVE_AES_CBC)
    EVP_CIPHER_CTX evpCipherContext;
    EVP_CIPHER_CTX *ctx = &evpCipherContext;
    const EVP_CIPHER* type = NULL;
    int keylen = 0;

    printf(testingFmt, "test_wolfSSL_EVP_CIPHER_CTX");

    /* EVP_AES_128_CBC used as an example */
    AssertNotNull(type = EVP_aes_128_cbc());
    /* below test indirectly tests EVP_CIPHER_CTX_init */
    AssertIntEQ(EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_INIT, 0, NULL), WOLFSSL_SUCCESS);
    AssertNotNull(ctx);
    AssertIntEQ(EVP_CipherInit(NULL, NULL, NULL, NULL, 0), WOLFSSL_FAILURE);
    AssertIntEQ(EVP_CipherInit(ctx, type, NULL, NULL, 0), WOLFSSL_SUCCESS);
    AssertIntEQ((keylen = EVP_CIPHER_CTX_key_length(ctx)), 16);
    AssertIntEQ(EVP_CIPHER_CTX_set_key_length(NULL, 0), WOLFSSL_FAILURE);
    /* below test indirectly tests EVP_CIPHER_CTX_set_key_length */
    AssertIntEQ(EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_SET_KEY_LENGTH, keylen,
                                    NULL), WOLFSSL_SUCCESS);

    EVP_CIPHER_CTX_cleanup(ctx);
#endif

    printf(resultFmt, passed);
}
#endif

/*----------------------------------------------------------------------------*
 | IO
 *----------------------------------------------------------------------------*/
#if !defined(NO_FILESYSTEM) && !defined(NO_CERTS) && \
    !defined(NO_RSA)        && !defined(SINGLE_THREADED) && \
    !defined(NO_WOLFSSL_SERVER) && !defined(NO_WOLFSSL_CLIENT)
#define HAVE_IO_TESTS_DEPENDENCIES
#endif

/* helper functions */
#ifdef HAVE_IO_TESTS_DEPENDENCIES

#ifdef WOLFSSL_SESSION_EXPORT
/* set up function for sending session information */
static int test_export(WOLFSSL* inSsl, byte* buf, word32 sz, void* userCtx)
{
    WOLFSSL_CTX* ctx;
    WOLFSSL*     ssl;

    AssertNotNull(inSsl);
    AssertNotNull(buf);
    AssertIntNE(0, sz);

    /* Set ctx to DTLS 1.2 */
    ctx = wolfSSL_CTX_new(wolfDTLSv1_2_server_method());
    AssertNotNull(ctx);

    ssl = wolfSSL_new(ctx);
    AssertNotNull(ssl);

    AssertIntGE(wolfSSL_dtls_import(ssl, buf, sz), 0);

    wolfSSL_free(ssl);
    wolfSSL_CTX_free(ctx);
    (void)userCtx;
    return WOLFSSL_SUCCESS;
}

/* returns negative value on fail and positive (including 0) on success */
static int nonblocking_accept_read(void* args, WOLFSSL* ssl, SOCKET_T* sockfd)
{
    int ret, err, loop_count, count, timeout = 10;
    char msg[] = "I hear you fa shizzle!";
    char input[1024];

    loop_count = ((func_args*)args)->argc;
    do {
#ifdef WOLFSSL_ASYNC_CRYPT
        if (err == WC_PENDING_E) {
            ret = wolfSSL_AsyncPoll(ssl, WOLF_POLL_FLAG_CHECK_HW);
            if (ret < 0) { break; } else if (ret == 0) { continue; }
        }
#endif

        err = 0; /* Reset error */

        ret = wolfSSL_accept(ssl);
        if (ret != WOLFSSL_SUCCESS) {
            err = wolfSSL_get_error(ssl, 0);
            if (err == WOLFSSL_ERROR_WANT_READ ||
                    err == WOLFSSL_ERROR_WANT_WRITE) {
                int select_ret;

                err = WC_PENDING_E;
                select_ret = tcp_select(*sockfd, timeout);
                if (select_ret == TEST_TIMEOUT) {
                    return WOLFSSL_FATAL_ERROR;
                }
            }
        }
    } while (ret != WOLFSSL_SUCCESS && err == WC_PENDING_E);

    if (ret != WOLFSSL_SUCCESS) {
        char buff[WOLFSSL_MAX_ERROR_SZ];
        printf("error = %d, %s\n", err, wolfSSL_ERR_error_string(err, buff));
        return ret;
    }

    for (count = 0; count < loop_count; count++) {
        int select_ret;

        select_ret = tcp_select(*sockfd, timeout);
        if (select_ret == TEST_TIMEOUT) {
            ret = WOLFSSL_FATAL_ERROR;
            break;
        }

        do {
            ret = wolfSSL_read(ssl, input, sizeof(input)-1);
            if (ret > 0) {
                input[ret] = '\0';
                printf("Client message: %s\n", input);
            }
        } while (err == WOLFSSL_ERROR_WANT_READ && ret != WOLFSSL_SUCCESS);

        do {
            if ((ret = wolfSSL_write(ssl, msg, sizeof(msg))) != sizeof(msg)) {
                return WOLFSSL_FATAL_ERROR;
            }
            err = wolfSSL_get_error(ssl, ret);
        } while (err == WOLFSSL_ERROR_WANT_READ && ret != WOLFSSL_SUCCESS);
    }
    return ret;
}
#endif /* WOLFSSL_SESSION_EXPORT */

static THREAD_RETURN WOLFSSL_THREAD test_server_nofail(void* args)
{
    SOCKET_T sockfd = 0;
    SOCKET_T clientfd = 0;
    word16 port;

    callback_functions* cbf = NULL;
    WOLFSSL_CTX* ctx = 0;
    WOLFSSL* ssl = 0;

    char msg[] = "I hear you fa shizzle!";
    char input[1024];
    int idx;
    int ret, err = 0;
    int sharedCtx = 0;

#ifdef WOLFSSL_TIRTOS
    fdOpenSession(Task_self());
#endif

    ((func_args*)args)->return_code = TEST_FAIL;
    cbf = ((func_args*)args)->callbacks;

#if defined(OPENSSL_EXTRA) || defined(WOLFSSL_EITHER_SIDE)
    if (cbf != NULL && cbf->ctx) {
        ctx = cbf->ctx;
        sharedCtx = 1;
    }
    else
#endif
    {
        WOLFSSL_METHOD* method = NULL;
        if (cbf != NULL && cbf->method != NULL) {
            method = cbf->method();
        }
        else {
            method = wolfSSLv23_server_method();
        }
        ctx = wolfSSL_CTX_new(method);
    }

#if defined(HAVE_SESSION_TICKET) && defined(HAVE_CHACHA) && \
                                    defined(HAVE_POLY1305)
    TicketInit();
    wolfSSL_CTX_set_TicketEncCb(ctx, myTicketEncCb);
#endif

#if defined(USE_WINDOWS_API)
    port = ((func_args*)args)->signal->port;
#elif defined(NO_MAIN_DRIVER) && !defined(WOLFSSL_SNIFFER) && \
     !defined(WOLFSSL_MDK_SHELL) && !defined(WOLFSSL_TIRTOS)
    /* Let tcp_listen assign port */
    port = 0;
#else
    /* Use default port */
    port = wolfSSLPort;
#endif

    /* do it here to detect failure */
    tcp_accept(&sockfd, &clientfd, (func_args*)args, port, 0, 0, 0, 0, 1);
    CloseSocket(sockfd);

    wolfSSL_CTX_set_verify(ctx,
                  WOLFSSL_VERIFY_PEER | WOLFSSL_VERIFY_FAIL_IF_NO_PEER_CERT, 0);

#ifdef WOLFSSL_ENCRYPTED_KEYS
    wolfSSL_CTX_set_default_passwd_cb(ctx, PasswordCallBack);
#endif

    if (wolfSSL_CTX_load_verify_locations(ctx, cliCertFile, 0)
                                                           != WOLFSSL_SUCCESS) {
        /*err_sys("can't load ca file, Please run from wolfSSL home dir");*/
        goto done;
    }
    if (!sharedCtx && wolfSSL_CTX_use_certificate_file(ctx, svrCertFile,
                                     WOLFSSL_FILETYPE_PEM) != WOLFSSL_SUCCESS) {
        /*err_sys("can't load server cert chain file, "
                "Please run from wolfSSL home dir");*/
        goto done;
    }
    if (!sharedCtx && wolfSSL_CTX_use_PrivateKey_file(ctx, svrKeyFile,
                                     WOLFSSL_FILETYPE_PEM) != WOLFSSL_SUCCESS) {
        /*err_sys("can't load server key file, "
                "Please run from wolfSSL home dir");*/
        goto done;
    }

    /* call ctx setup callback */
    if (cbf != NULL && cbf->ctx_ready != NULL) {
        cbf->ctx_ready(ctx);
    }

    ssl = wolfSSL_new(ctx);
    if (ssl == NULL) {
        goto done;
    }

#ifdef WOLFSSL_SESSION_EXPORT
    /* only add in more complex nonblocking case with session export tests */
    if (args && ((func_args*)args)->argc > 0) {
        /* set as nonblock and time out for waiting on read/write */
        tcp_set_nonblocking(&clientfd);
        wolfSSL_dtls_set_using_nonblock(ssl, 1);
    }
#endif

    if (sharedCtx && wolfSSL_use_certificate_file(ssl, svrCertFile,
                                     WOLFSSL_FILETYPE_PEM) != WOLFSSL_SUCCESS) {
        /*err_sys("can't load server cert chain file, "
                "Please run from wolfSSL home dir");*/
        goto done;
    }
    if (sharedCtx && wolfSSL_use_PrivateKey_file(ssl, svrKeyFile,
                                     WOLFSSL_FILETYPE_PEM) != WOLFSSL_SUCCESS) {
        /*err_sys("can't load server key file, "
                "Please run from wolfSSL home dir");*/
        goto done;
    }

    if (wolfSSL_set_fd(ssl, clientfd) != WOLFSSL_SUCCESS) {
        /*err_sys("SSL_set_fd failed");*/
        goto done;
    }

#if !defined(NO_FILESYSTEM) && !defined(NO_DH)
    wolfSSL_SetTmpDH_file(ssl, dhParamFile, WOLFSSL_FILETYPE_PEM);
#elif !defined(NO_DH)
    SetDH(ssl);  /* will repick suites with DHE, higher priority than PSK */
#endif

    /* call ssl setup callback */
    if (cbf != NULL && cbf->ssl_ready != NULL) {
        cbf->ssl_ready(ssl);
    }

#ifdef WOLFSSL_SESSION_EXPORT
    /* only add in more complex nonblocking case with session export tests */
    if (args && ((func_args*)args)->argc > 0) {
        ret = nonblocking_accept_read(args, ssl, &clientfd);
        if (ret >= 0) {
            ((func_args*)args)->return_code = TEST_SUCCESS;
        }
    #ifdef WOLFSSL_TIRTOS
        Task_yield();
    #endif
        goto done;
    }
#endif

    do {
#ifdef WOLFSSL_ASYNC_CRYPT
        if (err == WC_PENDING_E) {
            ret = wolfSSL_AsyncPoll(ssl, WOLF_POLL_FLAG_CHECK_HW);
            if (ret < 0) { break; } else if (ret == 0) { continue; }
        }
#endif

        err = 0; /* Reset error */
        ret = wolfSSL_accept(ssl);
        if (ret != WOLFSSL_SUCCESS) {
            err = wolfSSL_get_error(ssl, 0);
        }
    } while (ret != WOLFSSL_SUCCESS && err == WC_PENDING_E);

    if (ret != WOLFSSL_SUCCESS) {
        char buff[WOLFSSL_MAX_ERROR_SZ];
        printf("error = %d, %s\n", err, wolfSSL_ERR_error_string(err, buff));
        /*err_sys("SSL_accept failed");*/
        goto done;
    }

    idx = wolfSSL_read(ssl, input, sizeof(input)-1);
    if (idx > 0) {
        input[idx] = '\0';
        printf("Client message: %s\n", input);
    }

    if (wolfSSL_write(ssl, msg, sizeof(msg)) != sizeof(msg)) {
        /*err_sys("SSL_write failed");*/
#ifdef WOLFSSL_TIRTOS
        return;
#else
        return 0;
#endif
    }

#ifdef WOLFSSL_TIRTOS
    Task_yield();
#endif

    ((func_args*)args)->return_code = TEST_SUCCESS;

done:
    wolfSSL_shutdown(ssl);
    wolfSSL_free(ssl);
    if (!sharedCtx)
        wolfSSL_CTX_free(ctx);

    CloseSocket(clientfd);

#ifdef WOLFSSL_TIRTOS
    fdCloseSession(Task_self());
#endif

#if defined(NO_MAIN_DRIVER) && defined(HAVE_ECC) && defined(FP_ECC) \
                            && defined(HAVE_THREAD_LS)
    wc_ecc_fp_free();  /* free per thread cache */
#endif

#if defined(HAVE_SESSION_TICKET) && defined(HAVE_CHACHA) && \
                                    defined(HAVE_POLY1305)
    TicketCleanup();
#endif

#ifndef WOLFSSL_TIRTOS
    return 0;
#endif
}

#if defined(OPENSSL_EXTRA) && !defined(NO_SESSION_CACHE) && !defined(WOLFSSL_TLS13)
static THREAD_RETURN WOLFSSL_THREAD test_server_loop(void* args)
{
    SOCKET_T sockfd = 0;
    SOCKET_T clientfd = 0;
    word16 port;

    callback_functions* cbf = NULL;
    WOLFSSL_CTX* ctx = 0;
    WOLFSSL* ssl = 0;

    char msg[] = "I hear you fa shizzle!";
    char input[1024];
    int idx;
    int ret, err = 0;
    int sharedCtx = 0;
    int loop_count = ((func_args*)args)->argc;
    int count = 0;

#ifdef WOLFSSL_TIRTOS
    fdOpenSession(Task_self());
#endif

    ((func_args*)args)->return_code = TEST_FAIL;
    cbf = ((func_args*)args)->callbacks;

#if defined(OPENSSL_EXTRA) || defined(WOLFSSL_EITHER_SIDE)
    if (cbf != NULL && cbf->ctx) {
        ctx = cbf->ctx;
        sharedCtx = 1;
    }
    else
#endif
    {
        WOLFSSL_METHOD* method = NULL;
        if (cbf != NULL && cbf->method != NULL) {
            method = cbf->method();
        }
        else {
            method = wolfSSLv23_server_method();
        }
        ctx = wolfSSL_CTX_new(method);
    }

#if defined(USE_WINDOWS_API)
    port = ((func_args*)args)->signal->port;
#elif defined(NO_MAIN_DRIVER) && !defined(WOLFSSL_SNIFFER) && \
     !defined(WOLFSSL_MDK_SHELL) && !defined(WOLFSSL_TIRTOS)
    /* Let tcp_listen assign port */
    port = 0;
#else
    /* Use default port */
    port = wolfSSLPort;
#endif

    wolfSSL_CTX_set_verify(ctx,
                  WOLFSSL_VERIFY_PEER | WOLFSSL_VERIFY_FAIL_IF_NO_PEER_CERT, 0);

#ifdef WOLFSSL_ENCRYPTED_KEYS
    wolfSSL_CTX_set_default_passwd_cb(ctx, PasswordCallBack);
#endif

    if (wolfSSL_CTX_load_verify_locations(ctx, cliCertFile, 0)
                                                           != WOLFSSL_SUCCESS) {
        /*err_sys("can't load ca file, Please run from wolfSSL home dir");*/
        goto done;
    }
    if (!sharedCtx && wolfSSL_CTX_use_certificate_file(ctx, svrCertFile,
                                     WOLFSSL_FILETYPE_PEM) != WOLFSSL_SUCCESS) {
        /*err_sys("can't load server cert chain file, "
                "Please run from wolfSSL home dir");*/
        goto done;
    }
    if (!sharedCtx && wolfSSL_CTX_use_PrivateKey_file(ctx, svrKeyFile,
                                     WOLFSSL_FILETYPE_PEM) != WOLFSSL_SUCCESS) {
        /*err_sys("can't load server key file, "
                "Please run from wolfSSL home dir");*/
        goto done;
    }
    /* call ctx setup callback */
    if (cbf != NULL && cbf->ctx_ready != NULL) {
        cbf->ctx_ready(ctx);
    }

    while(count != loop_count) {
        ssl = wolfSSL_new(ctx);
        if (ssl == NULL) {
            goto done;
        }
        if (sharedCtx && wolfSSL_use_certificate_file(ssl, svrCertFile,
                                        WOLFSSL_FILETYPE_PEM) != WOLFSSL_SUCCESS) {
            /*err_sys("can't load server cert chain file, "
                    "Please run from wolfSSL home dir");*/
            goto done;
        }
        if (sharedCtx && wolfSSL_use_PrivateKey_file(ssl, svrKeyFile,
                                        WOLFSSL_FILETYPE_PEM) != WOLFSSL_SUCCESS) {
            /*err_sys("can't load server key file, "
                    "Please run from wolfSSL home dir");*/
            goto done;
        }

#if !defined(NO_FILESYSTEM) && !defined(NO_DH)
        wolfSSL_SetTmpDH_file(ssl, dhParamFile, WOLFSSL_FILETYPE_PEM);
#elif !defined(NO_DH)
        SetDH(ssl);  /* will repick suites with DHE, higher priority than PSK */
#endif
        /* call ssl setup callback */
        if (cbf != NULL && cbf->ssl_ready != NULL) {
            cbf->ssl_ready(ssl);
        }
        /* do it here to detect failure */
        tcp_accept(&sockfd, &clientfd, (func_args*)args, port, 0, 0, 0, 0, 1);
        CloseSocket(sockfd);
        if (wolfSSL_set_fd(ssl, clientfd) != WOLFSSL_SUCCESS) {
            /*err_sys("SSL_set_fd failed");*/
            goto done;
        }

        do {
    #ifdef WOLFSSL_ASYNC_CRYPT
            if (err == WC_PENDING_E) {
                ret = wolfSSL_AsyncPoll(ssl, WOLF_POLL_FLAG_CHECK_HW);
                if (ret < 0) { break; } else if (ret == 0) { continue; }
            }
    #endif
            err = 0; /* Reset error */
            ret = wolfSSL_accept(ssl);
            if (ret != WOLFSSL_SUCCESS) {
                err = wolfSSL_get_error(ssl, 0);
            }
        } while (ret != WOLFSSL_SUCCESS && err == WC_PENDING_E);

        if (ret != WOLFSSL_SUCCESS) {
            char buff[WOLFSSL_MAX_ERROR_SZ];
            printf("error = %d, %s\n", err, wolfSSL_ERR_error_string(err, buff));
            /*err_sys("SSL_accept failed");*/
            goto done;
        }

        idx = wolfSSL_read(ssl, input, sizeof(input)-1);
        if (idx > 0) {
            input[idx] = '\0';
            printf("Client message: %s\n", input);
        }

        if (wolfSSL_write(ssl, msg, sizeof(msg)) != sizeof(msg)) {
            /*err_sys("SSL_write failed");*/
    #ifdef WOLFSSL_TIRTOS
            return;
    #else
            return 0;
    #endif
        }
        /* free ssl for this connection */
        wolfSSL_shutdown(ssl);
        wolfSSL_free(ssl); ssl = NULL;
        CloseSocket(clientfd);

        count++;
    }
#ifdef WOLFSSL_TIRTOS
    Task_yield();
#endif

    ((func_args*)args)->return_code = TEST_SUCCESS;

done:
    if(ssl != NULL) {
        wolfSSL_shutdown(ssl);
        wolfSSL_free(ssl);
    }
    if (!sharedCtx)
        wolfSSL_CTX_free(ctx);

    CloseSocket(clientfd);

#ifdef WOLFSSL_TIRTOS
    fdCloseSession(Task_self());
#endif

#if defined(NO_MAIN_DRIVER) && defined(HAVE_ECC) && defined(FP_ECC) \
                            && defined(HAVE_THREAD_LS)
    wc_ecc_fp_free();  /* free per thread cache */
#endif

#ifndef WOLFSSL_TIRTOS
    return 0;
#endif
}
#endif /* defined(OPENSSL_EXTRA) && !defined(NO_SESSION_CACHE) && !defined(WOLFSSL_TLS13) */

typedef int (*cbType)(WOLFSSL_CTX *ctx, WOLFSSL *ssl);

static void test_client_nofail(void* args, void *cb)
{
    SOCKET_T sockfd = 0;
    callback_functions* cbf = NULL;

    WOLFSSL_CTX*     ctx     = 0;
    WOLFSSL*         ssl     = 0;
    WOLFSSL_CIPHER*  cipher;

    char msg[64] = "hello wolfssl!";
    char reply[1024];
    int  input;
    int  msgSz = (int)XSTRLEN(msg);
    int  ret, err = 0;
    int  cipherSuite;
    int  sharedCtx = 0;
    const char* cipherName1, *cipherName2;

#ifdef WOLFSSL_TIRTOS
    fdOpenSession(Task_self());
#endif

    ((func_args*)args)->return_code = TEST_FAIL;
    cbf = ((func_args*)args)->callbacks;

#if defined(OPENSSL_EXTRA) || defined(WOLFSSL_EITHER_SIDE)
    if (cbf != NULL && cbf->ctx) {
        ctx = cbf->ctx;
        sharedCtx = 1;
    }
    else
#endif
    {
        WOLFSSL_METHOD* method  = NULL;
        if (cbf != NULL && cbf->method != NULL) {
            method = cbf->method();
        }
        else {
            method = wolfSSLv23_client_method();
        }
        ctx = wolfSSL_CTX_new(method);
    }

#ifdef WOLFSSL_ENCRYPTED_KEYS
    wolfSSL_CTX_set_default_passwd_cb(ctx, PasswordCallBack);
#endif

    /* Do connect here so server detects failures */
    tcp_connect(&sockfd, wolfSSLIP, ((func_args*)args)->signal->port,
                0, 0, NULL);

    if (wolfSSL_CTX_load_verify_locations(ctx, caCertFile, 0) != WOLFSSL_SUCCESS)
    {
        /* err_sys("can't load ca file, Please run from wolfSSL home dir");*/
        goto done;
    }
    if (!sharedCtx && wolfSSL_CTX_use_certificate_file(ctx, cliCertFile,
                                     WOLFSSL_FILETYPE_PEM) != WOLFSSL_SUCCESS) {
        /*err_sys("can't load client cert file, "
                "Please run from wolfSSL home dir");*/
        goto done;
    }
    if (!sharedCtx && wolfSSL_CTX_use_PrivateKey_file(ctx, cliKeyFile,
                                     WOLFSSL_FILETYPE_PEM) != WOLFSSL_SUCCESS) {
        /*err_sys("can't load client key file, "
                "Please run from wolfSSL home dir");*/
        goto done;
    }

    /* call ctx setup callback */
    if (cbf != NULL && cbf->ctx_ready != NULL) {
        cbf->ctx_ready(ctx);
    }

    ssl = wolfSSL_new(ctx);
    if (ssl == NULL) {
        goto done;
    }

    if (sharedCtx && wolfSSL_use_certificate_file(ssl, cliCertFile,
                                     WOLFSSL_FILETYPE_PEM) != WOLFSSL_SUCCESS) {
        /*err_sys("can't load client cert file, "
                "Please run from wolfSSL home dir");*/
        goto done;
    }
    if (sharedCtx && wolfSSL_use_PrivateKey_file(ssl, cliKeyFile,
                                     WOLFSSL_FILETYPE_PEM) != WOLFSSL_SUCCESS) {
        /*err_sys("can't load client key file, "
                "Please run from wolfSSL home dir");*/
        goto done;
    }

    if (wolfSSL_set_fd(ssl, sockfd) != WOLFSSL_SUCCESS) {
        /*err_sys("SSL_set_fd failed");*/
        goto done;
    }

    /* call ssl setup callback */
    if (cbf != NULL && cbf->ssl_ready != NULL) {
        cbf->ssl_ready(ssl);
    }

    do {
#ifdef WOLFSSL_ASYNC_CRYPT
        if (err == WC_PENDING_E) {
            ret = wolfSSL_AsyncPoll(ssl, WOLF_POLL_FLAG_CHECK_HW);
            if (ret < 0) { break; } else if (ret == 0) { continue; }
        }
#endif

        err = 0; /* Reset error */
        ret = wolfSSL_connect(ssl);
        if (ret != WOLFSSL_SUCCESS) {
            err = wolfSSL_get_error(ssl, 0);
        }
    } while (ret != WOLFSSL_SUCCESS && err == WC_PENDING_E);

    if (ret != WOLFSSL_SUCCESS) {
        char buff[WOLFSSL_MAX_ERROR_SZ];
        printf("error = %d, %s\n", err, wolfSSL_ERR_error_string(err, buff));
        /*err_sys("SSL_connect failed");*/
        goto done;
    }

    /* test the various get cipher methods */
    /* Internal cipher suite names */
    cipherSuite = wolfSSL_get_current_cipher_suite(ssl);
    cipherName1 = wolfSSL_get_cipher_name(ssl);
    cipherName2 = wolfSSL_get_cipher_name_from_suite(
        (cipherSuite >> 8), cipherSuite & 0xFF);
    AssertStrEQ(cipherName1, cipherName2);

    /* IANA Cipher Suites Names */
    /* Unless WOLFSSL_CIPHER_INTERNALNAME or NO_ERROR_STRINGS,
        then it's the internal cipher suite name */
    cipher = wolfSSL_get_current_cipher(ssl);
    cipherName1 = wolfSSL_CIPHER_get_name(cipher);
    cipherName2 = wolfSSL_get_cipher(ssl);
    AssertStrEQ(cipherName1, cipherName2);
#if !defined(WOLFSSL_CIPHER_INTERNALNAME) && !defined(NO_ERROR_STRINGS) && \
    !defined(WOLFSSL_QT)
    cipherName1 = wolfSSL_get_cipher_name_iana_from_suite(
            (cipherSuite >> 8), cipherSuite & 0xFF);
    AssertStrEQ(cipherName1, cipherName2);
#endif

    if (cb != NULL)
        ((cbType)cb)(ctx, ssl);

    if (wolfSSL_write(ssl, msg, msgSz) != msgSz) {
        /*err_sys("SSL_write failed");*/
        goto done;
    }

    input = wolfSSL_read(ssl, reply, sizeof(reply)-1);
    if (input > 0) {
        reply[input] = '\0';
        printf("Server response: %s\n", reply);
    }

    ((func_args*)args)->return_code = TEST_SUCCESS;

done:
    wolfSSL_free(ssl);
    if (!sharedCtx)
        wolfSSL_CTX_free(ctx);

    CloseSocket(sockfd);

#ifdef WOLFSSL_TIRTOS
    fdCloseSession(Task_self());
#endif

#if defined(NO_MAIN_DRIVER) && defined(HAVE_ECC) && defined(FP_ECC) \
                            && defined(HAVE_THREAD_LS)
    wc_ecc_fp_free();  /* free per thread cache */
#endif

    return;
}

#if defined(OPENSSL_EXTRA) && !defined(NO_SESSION_CACHE) && !defined(WOLFSSL_TLS13)
static void test_client_reuse_WOLFSSLobj(void* args, void *cb, void* server_args)
{
    SOCKET_T sockfd = 0;
    callback_functions* cbf = NULL;

    WOLFSSL_CTX*     ctx     = 0;
    WOLFSSL*         ssl     = 0;
    WOLFSSL_SESSION* session = NULL;

    char msg[64] = "hello wolfssl!";
    char reply[1024];
    int  input;
    int  msgSz = (int)XSTRLEN(msg);
    int  ret, err = 0;
    int  sharedCtx = 0;

#ifdef WOLFSSL_TIRTOS
    fdOpenSession(Task_self());
#endif

    ((func_args*)args)->return_code = TEST_FAIL;
    cbf = ((func_args*)args)->callbacks;

#if defined(OPENSSL_EXTRA) || defined(WOLFSSL_EITHER_SIDE)
    if (cbf != NULL && cbf->ctx) {
        ctx = cbf->ctx;
        sharedCtx = 1;
    }
    else
#endif
    {
        WOLFSSL_METHOD* method  = NULL;
        if (cbf != NULL && cbf->method != NULL) {
            method = cbf->method();
        }
        else {
            method = wolfSSLv23_client_method();
        }
        ctx = wolfSSL_CTX_new(method);
    }

#ifdef WOLFSSL_ENCRYPTED_KEYS
    wolfSSL_CTX_set_default_passwd_cb(ctx, PasswordCallBack);
#endif

    /* Do connect here so server detects failures */
    tcp_connect(&sockfd, wolfSSLIP, ((func_args*)args)->signal->port,
                0, 0, NULL);

    if (wolfSSL_CTX_load_verify_locations(ctx, caCertFile, 0) != WOLFSSL_SUCCESS)
    {
        /* err_sys("can't load ca file, Please run from wolfSSL home dir");*/
        goto done;
    }
    if (!sharedCtx && wolfSSL_CTX_use_certificate_file(ctx, cliCertFile,
                                     WOLFSSL_FILETYPE_PEM) != WOLFSSL_SUCCESS) {
        /*err_sys("can't load client cert file, "
                "Please run from wolfSSL home dir");*/
        goto done;
    }
    if (!sharedCtx && wolfSSL_CTX_use_PrivateKey_file(ctx, cliKeyFile,
                                     WOLFSSL_FILETYPE_PEM) != WOLFSSL_SUCCESS) {
        /*err_sys("can't load client key file, "
                "Please run from wolfSSL home dir");*/
        goto done;
    }

    /* call ctx setup callback */
    if (cbf != NULL && cbf->ctx_ready != NULL) {
        cbf->ctx_ready(ctx);
    }

    ssl = wolfSSL_new(ctx);
    if (ssl == NULL) {
        goto done;
    }
    /* keep handshakre resources for re-using WOLFSSL obj */
    wolfSSL_KeepArrays(ssl);
    if(wolfSSL_KeepHandshakeResources(ssl)) {
        /* err_sys("SSL_KeepHandshakeResources failed"); */
        goto done;
    }
    if (sharedCtx && wolfSSL_use_certificate_file(ssl, cliCertFile,
                                     WOLFSSL_FILETYPE_PEM) != WOLFSSL_SUCCESS) {
        /*err_sys("can't load client cert file, "
                "Please run from wolfSSL home dir");*/
        goto done;
    }
    if (sharedCtx && wolfSSL_use_PrivateKey_file(ssl, cliKeyFile,
                                     WOLFSSL_FILETYPE_PEM) != WOLFSSL_SUCCESS) {
        /*err_sys("can't load client key file, "
                "Please run from wolfSSL home dir");*/
        goto done;
    }

    if (wolfSSL_set_fd(ssl, sockfd) != WOLFSSL_SUCCESS) {
        /*err_sys("SSL_set_fd failed");*/
        goto done;
    }

    /* call ssl setup callback */
    if (cbf != NULL && cbf->ssl_ready != NULL) {
        cbf->ssl_ready(ssl);
    }

    do {
#ifdef WOLFSSL_ASYNC_CRYPT
        if (err == WC_PENDING_E) {
            ret = wolfSSL_AsyncPoll(ssl, WOLF_POLL_FLAG_CHECK_HW);
            if (ret < 0) { break; } else if (ret == 0) { continue; }
        }
#endif

        err = 0; /* Reset error */
        ret = wolfSSL_connect(ssl);
        if (ret != WOLFSSL_SUCCESS) {
            err = wolfSSL_get_error(ssl, 0);
        }
    } while (ret != WOLFSSL_SUCCESS && err == WC_PENDING_E);

    if (ret != WOLFSSL_SUCCESS) {
        char buff[WOLFSSL_MAX_ERROR_SZ];
        printf("error = %d, %s\n", err, wolfSSL_ERR_error_string(err, buff));
        /*err_sys("SSL_connect failed");*/
        goto done;
    }
    /* Build first session */
    if (cb != NULL)
        ((cbType)cb)(ctx, ssl);

    if (wolfSSL_write(ssl, msg, msgSz) != msgSz) {
        /*err_sys("SSL_write failed");*/
        goto done;
    }

    input = wolfSSL_read(ssl, reply, sizeof(reply)-1);
    if (input > 0) {
        reply[input] = '\0';
        printf("Server response: %s\n", reply);
    }

    /* Session Resumption by re-using WOLFSSL object */
    wolfSSL_set_quiet_shutdown(ssl, 1);
    if (wolfSSL_shutdown(ssl) != WOLFSSL_SUCCESS) {
        /* err_sys ("SSL shutdown failed"); */
        goto done;
    }
    session = wolfSSL_get_session(ssl);
    if (wolfSSL_clear(ssl) != WOLFSSL_SUCCESS) {
        /* err_sys ("SSL_clear failed"); */
        goto done;
    }
    wolfSSL_set_session(ssl, session);
    /* close socket onece */
    CloseSocket(sockfd);
    sockfd = 0;
    /* wait until server ready */
    wait_tcp_ready((func_args*)server_args);
    printf("session resumption\n");
    /* Do re-connect  */
    tcp_connect(&sockfd, wolfSSLIP, ((func_args*)args)->signal->port,
                0, 0, NULL);
    if (wolfSSL_set_fd(ssl, sockfd) != WOLFSSL_SUCCESS) {
        /*err_sys("SSL_set_fd failed");*/
        goto done;
    }

do {
#ifdef WOLFSSL_ASYNC_CRYPT
        if (err == WC_PENDING_E) {
            ret = wolfSSL_AsyncPoll(ssl, WOLF_POLL_FLAG_CHECK_HW);
            if (ret < 0) { break; } else if (ret == 0) { continue; }
        }
#endif

        err = 0; /* Reset error */
        ret = wolfSSL_connect(ssl);
        if (ret != WOLFSSL_SUCCESS) {
            err = wolfSSL_get_error(ssl, 0);
        }
    } while (ret != WOLFSSL_SUCCESS && err == WC_PENDING_E);

    if (ret != WOLFSSL_SUCCESS) {
        char buff[WOLFSSL_MAX_ERROR_SZ];
        printf("error = %d, %s\n", err, wolfSSL_ERR_error_string(err, buff));
        /*err_sys("SSL_connect failed");*/
        goto done;
    }
    /* Build first session */
    if (cb != NULL)
        ((cbType)cb)(ctx, ssl);

    if (wolfSSL_write(ssl, msg, msgSz) != msgSz) {
        /*err_sys("SSL_write failed");*/
        goto done;
    }

    input = wolfSSL_read(ssl, reply, sizeof(reply)-1);
    if (input > 0) {
        reply[input] = '\0';
        printf("Server response: %s\n", reply);
    }

    ((func_args*)args)->return_code = TEST_SUCCESS;

done:
    wolfSSL_free(ssl);
    if (!sharedCtx)
        wolfSSL_CTX_free(ctx);

    CloseSocket(sockfd);

#ifdef WOLFSSL_TIRTOS
    fdCloseSession(Task_self());
#endif

    return;
}
#endif /* defined(OPENSSL_EXTRA) && !defined(NO_SESSION_CACHE) && !defined(WOLFSSL_TLS13) */

/* SNI / ALPN / session export helper functions */
#if defined(HAVE_SNI) || defined(HAVE_ALPN) || defined(WOLFSSL_SESSION_EXPORT)

static THREAD_RETURN WOLFSSL_THREAD run_wolfssl_server(void* args)
{
    callback_functions* callbacks = ((func_args*)args)->callbacks;

    WOLFSSL_CTX* ctx = wolfSSL_CTX_new(callbacks->method());
    WOLFSSL*     ssl = NULL;
    SOCKET_T    sfd = 0;
    SOCKET_T    cfd = 0;
    word16      port;

    char msg[] = "I hear you fa shizzle!";
    int  len   = (int) XSTRLEN(msg);
    char input[1024];
    int  idx;
    int  ret, err = 0;

#ifdef WOLFSSL_TIRTOS
    fdOpenSession(Task_self());
#endif
    ((func_args*)args)->return_code = TEST_FAIL;

#if defined(USE_WINDOWS_API)
    port = ((func_args*)args)->signal->port;
#elif defined(NO_MAIN_DRIVER) && !defined(WOLFSSL_SNIFFER) && \
     !defined(WOLFSSL_MDK_SHELL) && !defined(WOLFSSL_TIRTOS)
    /* Let tcp_listen assign port */
    port = 0;
#else
    /* Use default port */
    port = wolfSSLPort;
#endif

    wolfSSL_CTX_set_verify(ctx,
                  WOLFSSL_VERIFY_PEER | WOLFSSL_VERIFY_FAIL_IF_NO_PEER_CERT, 0);

#ifdef WOLFSSL_ENCRYPTED_KEYS
    wolfSSL_CTX_set_default_passwd_cb(ctx, PasswordCallBack);
#endif
#ifdef WOLFSSL_SESSION_EXPORT
    AssertIntEQ(WOLFSSL_SUCCESS, wolfSSL_CTX_dtls_set_export(ctx, test_export));
#endif


    AssertIntEQ(WOLFSSL_SUCCESS,
        wolfSSL_CTX_load_verify_locations(ctx, cliCertFile, 0));

    AssertIntEQ(WOLFSSL_SUCCESS,
        wolfSSL_CTX_use_certificate_file(ctx, svrCertFile,
            WOLFSSL_FILETYPE_PEM));

    AssertIntEQ(WOLFSSL_SUCCESS,
        wolfSSL_CTX_use_PrivateKey_file(ctx, svrKeyFile, WOLFSSL_FILETYPE_PEM));

    if (callbacks->ctx_ready)
        callbacks->ctx_ready(ctx);

    ssl = wolfSSL_new(ctx);
    if (wolfSSL_dtls(ssl)) {
        SOCKADDR_IN_T cliAddr;
        socklen_t     cliLen;

        cliLen = sizeof(cliAddr);
        tcp_accept(&sfd, &cfd, (func_args*)args, port, 0, 1, 0, 0, 0);
        idx = (int)recvfrom(sfd, input, sizeof(input), MSG_PEEK,
                (struct sockaddr*)&cliAddr, &cliLen);
        AssertIntGT(idx, 0);
        wolfSSL_dtls_set_peer(ssl, &cliAddr, cliLen);
    }
    else {
        tcp_accept(&sfd, &cfd, (func_args*)args, port, 0, 0, 0, 0, 1);
        CloseSocket(sfd);
    }

    AssertIntEQ(WOLFSSL_SUCCESS, wolfSSL_set_fd(ssl, cfd));

#ifdef NO_PSK
    #if !defined(NO_FILESYSTEM) && !defined(NO_DH)
        wolfSSL_SetTmpDH_file(ssl, dhParamFile, WOLFSSL_FILETYPE_PEM);
    #elif !defined(NO_DH)
        SetDH(ssl);  /* will repick suites with DHE, higher priority than PSK */
    #endif
#endif

    if (callbacks->ssl_ready)
        callbacks->ssl_ready(ssl);

    do {
#ifdef WOLFSSL_ASYNC_CRYPT
        if (err == WC_PENDING_E) {
            ret = wolfSSL_AsyncPoll(ssl, WOLF_POLL_FLAG_CHECK_HW);
            if (ret < 0) { break; } else if (ret == 0) { continue; }
        }
#endif

        err = 0; /* Reset error */
        ret = wolfSSL_accept(ssl);
        if (ret != WOLFSSL_SUCCESS) {
            err = wolfSSL_get_error(ssl, 0);
        }
    } while (ret != WOLFSSL_SUCCESS && err == WC_PENDING_E);

    if (ret != WOLFSSL_SUCCESS) {
        char buff[WOLFSSL_MAX_ERROR_SZ];
        printf("error = %d, %s\n", err, wolfSSL_ERR_error_string(err, buff));
        /*err_sys("SSL_accept failed");*/
    }
    else {
        if (0 < (idx = wolfSSL_read(ssl, input, sizeof(input)-1))) {
            input[idx] = 0;
            printf("Client message: %s\n", input);
        }

        AssertIntEQ(len, wolfSSL_write(ssl, msg, len));
#if defined(WOLFSSL_SESSION_EXPORT) && !defined(HAVE_IO_POOL)
        if (wolfSSL_dtls(ssl)) {
            byte*  import;
            word32 sz;

            wolfSSL_dtls_export(ssl, NULL, &sz);
            import = (byte*)XMALLOC(sz, NULL, DYNAMIC_TYPE_TMP_BUFFER);
            AssertNotNull(import);
            idx = wolfSSL_dtls_export(ssl, import, &sz);
            AssertIntGE(idx, 0);
            AssertIntGE(wolfSSL_dtls_import(ssl, import, idx), 0);
            XFREE(import, NULL, DYNAMIC_TYPE_TMP_BUFFER);
        }
#endif
#ifdef WOLFSSL_TIRTOS
        Task_yield();
#endif
        ((func_args*)args)->return_code = TEST_SUCCESS;
    }

    if (callbacks->on_result)
        callbacks->on_result(ssl);

    wolfSSL_shutdown(ssl);
    wolfSSL_free(ssl);
    wolfSSL_CTX_free(ctx);
    CloseSocket(cfd);


#ifdef WOLFSSL_TIRTOS
    fdCloseSession(Task_self());
#endif

#if defined(NO_MAIN_DRIVER) && defined(HAVE_ECC) && defined(FP_ECC) \
                            && defined(HAVE_THREAD_LS)
    wc_ecc_fp_free();  /* free per thread cache */
#endif

#ifndef WOLFSSL_TIRTOS
    return 0;
#endif
}

static void run_wolfssl_client(void* args)
{
    callback_functions* callbacks = ((func_args*)args)->callbacks;

    WOLFSSL_CTX* ctx = wolfSSL_CTX_new(callbacks->method());
    WOLFSSL*     ssl = NULL;
    SOCKET_T    sfd = 0;

    char msg[] = "hello wolfssl server!";
    int  len   = (int) XSTRLEN(msg);
    char input[1024];
    int  idx;
    int  ret, err = 0;

#ifdef WOLFSSL_TIRTOS
    fdOpenSession(Task_self());
#endif

    ((func_args*)args)->return_code = TEST_FAIL;

#ifdef WOLFSSL_ENCRYPTED_KEYS
    wolfSSL_CTX_set_default_passwd_cb(ctx, PasswordCallBack);
#endif

    AssertIntEQ(WOLFSSL_SUCCESS, wolfSSL_CTX_load_verify_locations(ctx, caCertFile, 0));

    AssertIntEQ(WOLFSSL_SUCCESS,
               wolfSSL_CTX_use_certificate_file(ctx, cliCertFile, WOLFSSL_FILETYPE_PEM));

    AssertIntEQ(WOLFSSL_SUCCESS,
                 wolfSSL_CTX_use_PrivateKey_file(ctx, cliKeyFile, WOLFSSL_FILETYPE_PEM));

    if (callbacks->ctx_ready)
        callbacks->ctx_ready(ctx);

    ssl = wolfSSL_new(ctx);
    if (wolfSSL_dtls(ssl)) {
        tcp_connect(&sfd, wolfSSLIP, ((func_args*)args)->signal->port,
                    1, 0, ssl);
    }
    else {
        tcp_connect(&sfd, wolfSSLIP, ((func_args*)args)->signal->port,
                    0, 0, ssl);
    }
    AssertIntEQ(WOLFSSL_SUCCESS, wolfSSL_set_fd(ssl, sfd));

    if (callbacks->ssl_ready)
        callbacks->ssl_ready(ssl);

    do {
#ifdef WOLFSSL_ASYNC_CRYPT
        if (err == WC_PENDING_E) {
            ret = wolfSSL_AsyncPoll(ssl, WOLF_POLL_FLAG_CHECK_HW);
            if (ret < 0) { break; } else if (ret == 0) { continue; }
        }
#endif

        err = 0; /* Reset error */
        ret = wolfSSL_connect(ssl);
        if (ret != WOLFSSL_SUCCESS) {
            err = wolfSSL_get_error(ssl, 0);
        }
    } while (ret != WOLFSSL_SUCCESS && err == WC_PENDING_E);

    if (ret != WOLFSSL_SUCCESS) {
        char buff[WOLFSSL_MAX_ERROR_SZ];
        printf("error = %d, %s\n", err, wolfSSL_ERR_error_string(err, buff));
        /*err_sys("SSL_connect failed");*/
    }
    else {
        AssertIntEQ(len, wolfSSL_write(ssl, msg, len));

        if (0 < (idx = wolfSSL_read(ssl, input, sizeof(input)-1))) {
            input[idx] = 0;
            printf("Server response: %s\n", input);
        }
        ((func_args*)args)->return_code = TEST_SUCCESS;
    }

    if (callbacks->on_result)
        callbacks->on_result(ssl);

    wolfSSL_free(ssl);
    wolfSSL_CTX_free(ctx);
    CloseSocket(sfd);

#ifdef WOLFSSL_TIRTOS
    fdCloseSession(Task_self());
#endif
}

#endif /* defined(HAVE_SNI) || defined(HAVE_ALPN) ||
          defined(WOLFSSL_SESSION_EXPORT) */

static void test_wolfSSL_read_write(void)
{
    /* The unit testing for read and write shall happen simutaneously, since
     * one can't do anything with one without the other. (Except for a failure
     * test case.) This function will call all the others that will set up,
     * execute, and report their test findings.
     *
     * Set up the success case first. This function will become the template
     * for the other tests. This should eventually be renamed
     *
     * The success case isn't interesting, how can this fail?
     * - Do not give the client context a CA certificate. The connect should
     *   fail. Do not need server for this?
     * - Using NULL for the ssl object on server. Do not need client for this.
     * - Using NULL for the ssl object on client. Do not need server for this.
     * - Good ssl objects for client and server. Client write() without server
     *   read().
     * - Good ssl objects for client and server. Server write() without client
     *   read().
     * - Forgetting the password callback?
    */
    tcp_ready ready;
    func_args client_args;
    func_args server_args;
    THREAD_TYPE serverThread;

    XMEMSET(&client_args, 0, sizeof(func_args));
    XMEMSET(&server_args, 0, sizeof(func_args));
#ifdef WOLFSSL_TIRTOS
    fdOpenSession(Task_self());
#endif

    StartTCP();
    InitTcpReady(&ready);

#if defined(USE_WINDOWS_API)
    /* use RNG to get random port if using windows */
    ready.port = GetRandomPort();
#endif

    server_args.signal = &ready;
    client_args.signal = &ready;

    start_thread(test_server_nofail, &server_args, &serverThread);
    wait_tcp_ready(&server_args);
    test_client_nofail(&client_args, NULL);
    join_thread(serverThread);

    AssertTrue(client_args.return_code);
    AssertTrue(server_args.return_code);

    FreeTcpReady(&ready);

#ifdef WOLFSSL_TIRTOS
    fdOpenSession(Task_self());
#endif
}

#if defined(OPENSSL_EXTRA) && !defined(NO_SESSION_CACHE) && !defined(WOLFSSL_TLS13)
static void test_wolfSSL_reuse_WOLFSSLobj(void)
{
    /* The unit test for session resumption by re-using WOLFSSL object.
     * WOLFSSL object is not cleared after first session. It re-use the obeject
     * for second connection.
    */
    tcp_ready ready;
    func_args client_args;
    func_args server_args;
    THREAD_TYPE serverThread;

    XMEMSET(&client_args, 0, sizeof(func_args));
    XMEMSET(&server_args, 0, sizeof(func_args));
#ifdef WOLFSSL_TIRTOS
    fdOpenSession(Task_self());
#endif

    StartTCP();
    InitTcpReady(&ready);

#if defined(USE_WINDOWS_API)
    /* use RNG to get random port if using windows */
    ready.port = GetRandomPort();
#endif

    server_args.signal = &ready;
    client_args.signal = &ready;
    /* the var is used for loop number */
    server_args.argc = 2;

    start_thread(test_server_loop, &server_args, &serverThread);
    wait_tcp_ready(&server_args);
    test_client_reuse_WOLFSSLobj(&client_args, NULL, &server_args);
    join_thread(serverThread);

    AssertTrue(client_args.return_code);
    AssertTrue(server_args.return_code);

    FreeTcpReady(&ready);

#ifdef WOLFSSL_TIRTOS
    fdOpenSession(Task_self());
#endif
}
#endif /* defined(OPENSSL_EXTRA) && !defined(NO_SESSION_CACHE) && !defined(WOLFSSL_TLS13) */

#if defined(WOLFSSL_DTLS) && defined(WOLFSSL_SESSION_EXPORT)
/* canned export of a session using older version 3 */
static unsigned char version_3[] = {
    0xA5, 0xA3, 0x01, 0x87, 0x00, 0x3b, 0x00, 0x01,
    0x00, 0x00, 0x00, 0x80, 0x0C, 0x00, 0x00, 0x00,
    0x00, 0x80, 0x00, 0x1C, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
    0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0xC0, 0x30, 0x05,
    0x09, 0x0A, 0x01, 0x01, 0x00, 0x0D, 0x05, 0xFE,
    0xFD, 0x01, 0x25, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x01, 0x00, 0x01, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x06, 0x00, 0x05, 0x00, 0x06, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x06, 0x00, 0x01, 0x00, 0x07, 0x00, 0x00, 0x00,
    0x30, 0x00, 0x00, 0x00, 0x10, 0x01, 0x01, 0x00,
    0x02, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x3F, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x30, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x05, 0x12,
    0xCF, 0x22, 0xA1, 0x9F, 0x1C, 0x39, 0x1D, 0x31,
    0x11, 0x12, 0x1D, 0x11, 0x18, 0x0D, 0x0B, 0xF3,
    0xE1, 0x4D, 0xDC, 0xB1, 0xF1, 0x39, 0x98, 0x91,
    0x6C, 0x48, 0xE5, 0xED, 0x11, 0x12, 0xA0, 0x00,
    0xF2, 0x25, 0x4C, 0x09, 0x26, 0xD1, 0x74, 0xDF,
    0x23, 0x40, 0x15, 0x6A, 0x42, 0x2A, 0x26, 0xA5,
    0xAC, 0x56, 0xD5, 0x4A, 0x20, 0xB7, 0xE9, 0xEF,
    0xEB, 0xAF, 0xA8, 0x1E, 0x23, 0x7C, 0x04, 0xAA,
    0xA1, 0x6D, 0x92, 0x79, 0x7B, 0xFA, 0x80, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x0C,
    0x79, 0x7B, 0xFA, 0x80, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0xAA, 0xA1, 0x6D, 0x92,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x10, 0x00, 0x20, 0x00, 0x04, 0x00, 0x10,
    0x00, 0x10, 0x08, 0x02, 0x05, 0x08, 0x01, 0x30,
    0x28, 0x00, 0x00, 0x0F, 0x00, 0x02, 0x00, 0x09,
    0x31, 0x32, 0x37, 0x2E, 0x30, 0x2E, 0x30, 0x2E,
    0x31, 0xED, 0x4F
};
#endif /* defined(WOLFSSL_DTLS) && defined(WOLFSSL_SESSION_EXPORT) */

static void test_wolfSSL_dtls_export(void)
{
#if defined(WOLFSSL_DTLS) && defined(WOLFSSL_SESSION_EXPORT)
    tcp_ready ready;
    func_args client_args;
    func_args server_args;
    THREAD_TYPE serverThread;
    callback_functions server_cbf;
    callback_functions client_cbf;
#ifdef WOLFSSL_TIRTOS
    fdOpenSession(Task_self());
#endif

    InitTcpReady(&ready);

#if defined(USE_WINDOWS_API)
    /* use RNG to get random port if using windows */
    ready.port = GetRandomPort();
#endif

    /* set using dtls */
    XMEMSET(&client_args, 0, sizeof(func_args));
    XMEMSET(&server_args, 0, sizeof(func_args));
    XMEMSET(&server_cbf, 0, sizeof(callback_functions));
    XMEMSET(&client_cbf, 0, sizeof(callback_functions));
    server_cbf.method = wolfDTLSv1_2_server_method;
    client_cbf.method = wolfDTLSv1_2_client_method;
    server_args.callbacks = &server_cbf;
    client_args.callbacks = &client_cbf;

    server_args.signal = &ready;
    client_args.signal = &ready;

    start_thread(run_wolfssl_server, &server_args, &serverThread);
    wait_tcp_ready(&server_args);
    run_wolfssl_client(&client_args);
    join_thread(serverThread);

    AssertTrue(client_args.return_code);
    AssertTrue(server_args.return_code);

    FreeTcpReady(&ready);

#ifdef WOLFSSL_TIRTOS
    fdOpenSession(Task_self());
#endif

    {
        SOCKET_T sockfd = 0;
        WOLFSSL_CTX* ctx;
        WOLFSSL*     ssl;
        char msg[64] = "hello wolfssl!";
        char reply[1024];
        int  msgSz = (int)XSTRLEN(msg);
        byte *session, *window;
        unsigned int sessionSz, windowSz;
        struct sockaddr_in peerAddr;
        int i;


        /* Set ctx to DTLS 1.2 */
        AssertNotNull(ctx = wolfSSL_CTX_new(wolfDTLSv1_2_server_method()));
        AssertNotNull(ssl = wolfSSL_new(ctx));

        /* test importing version 3 */
        AssertIntGE(wolfSSL_dtls_import(ssl, version_3, sizeof(version_3)), 0);

        /* test importing bad length and bad version */
        version_3[2] += 1;
        AssertIntLT(wolfSSL_dtls_import(ssl, version_3, sizeof(version_3)), 0);
        version_3[2] -= 1; version_3[1] = 0XA0;
        AssertIntLT(wolfSSL_dtls_import(ssl, version_3, sizeof(version_3)), 0);
        wolfSSL_free(ssl);
        wolfSSL_CTX_free(ctx);


    /* check storing client state after connection and storing window only */
#ifdef WOLFSSL_TIRTOS
    fdOpenSession(Task_self());
#endif

    InitTcpReady(&ready);

#if defined(USE_WINDOWS_API)
    /* use RNG to get random port if using windows */
    ready.port = GetRandomPort();
#endif

    /* set using dtls */
    XMEMSET(&server_args, 0, sizeof(func_args));
    XMEMSET(&server_cbf, 0, sizeof(callback_functions));
    server_cbf.method = wolfDTLSv1_2_server_method;
    server_args.callbacks = &server_cbf;
    server_args.argc = 3; /* set loop_count to 3 */


    server_args.signal = &ready;
    start_thread(test_server_nofail, &server_args, &serverThread);
    wait_tcp_ready(&server_args);

    /* create and connect with client */
    AssertNotNull(ctx = wolfSSL_CTX_new(wolfDTLSv1_2_client_method()));
    AssertIntEQ(WOLFSSL_SUCCESS,
            wolfSSL_CTX_load_verify_locations(ctx, caCertFile, 0));
    AssertIntEQ(WOLFSSL_SUCCESS,
          wolfSSL_CTX_use_certificate_file(ctx, cliCertFile, SSL_FILETYPE_PEM));
    AssertIntEQ(WOLFSSL_SUCCESS,
            wolfSSL_CTX_use_PrivateKey_file(ctx, cliKeyFile, SSL_FILETYPE_PEM));
    tcp_connect(&sockfd, wolfSSLIP, server_args.signal->port, 0, 0, NULL);
    AssertNotNull(ssl = wolfSSL_new(ctx));
    AssertIntEQ(wolfSSL_set_fd(ssl, sockfd), WOLFSSL_SUCCESS);

    /* store server information connected too */
    XMEMSET(&peerAddr, 0, sizeof(peerAddr));
    peerAddr.sin_family = AF_INET;
    peerAddr.sin_port = XHTONS(server_args.signal->port);
    wolfSSL_dtls_set_peer(ssl, &peerAddr, sizeof(peerAddr));

    AssertIntEQ(wolfSSL_connect(ssl), WOLFSSL_SUCCESS);
    AssertIntEQ(wolfSSL_dtls_export(ssl, NULL, &sessionSz), 0);
    session = (byte*)XMALLOC(sessionSz, HEAP_HINT, DYNAMIC_TYPE_TMP_BUFFER);
    AssertIntGT(wolfSSL_dtls_export(ssl, session, &sessionSz), 0);
    AssertIntEQ(wolfSSL_write(ssl, msg, msgSz), msgSz);
    AssertIntGT(wolfSSL_read(ssl, reply, sizeof(reply)), 0);
    AssertIntEQ(wolfSSL_dtls_export_state_only(ssl, NULL, &windowSz), 0);
    window = (byte*)XMALLOC(windowSz, HEAP_HINT, DYNAMIC_TYPE_TMP_BUFFER);
    AssertIntGT(wolfSSL_dtls_export_state_only(ssl, window, &windowSz), 0);
    wolfSSL_free(ssl);

    for (i = 1; i < server_args.argc; i++) {
        /* restore state */
        AssertNotNull(ssl = wolfSSL_new(ctx));
        AssertIntGT(wolfSSL_dtls_import(ssl, session, sessionSz), 0);
        AssertIntGT(wolfSSL_dtls_import(ssl, window, windowSz), 0);
        AssertIntEQ(wolfSSL_set_fd(ssl, sockfd), WOLFSSL_SUCCESS);
        AssertIntEQ(wolfSSL_write(ssl, msg, msgSz), msgSz);
        AssertIntGE(wolfSSL_read(ssl, reply, sizeof(reply)), 0);
        AssertIntGT(wolfSSL_dtls_export_state_only(ssl, window, &windowSz), 0);
        wolfSSL_free(ssl);
    }
    XFREE(session, HEAP_HINT, DYNAMIC_TYPE_TMP_BUFFER);
    XFREE(window, HEAP_HINT, DYNAMIC_TYPE_TMP_BUFFER);
    wolfSSL_CTX_free(ctx);

    printf("done and waiting for server\n");
    join_thread(serverThread);
    AssertIntEQ(server_args.return_code, TEST_SUCCESS);

    FreeTcpReady(&ready);

#ifdef WOLFSSL_TIRTOS
    fdOpenSession(Task_self());
#endif
    }


    printf(testingFmt, "wolfSSL_dtls_export()");
    printf(resultFmt, passed);
#endif
}

/*----------------------------------------------------------------------------*
 | TLS extensions tests
 *----------------------------------------------------------------------------*/

#if defined(HAVE_SNI) || defined(HAVE_ALPN)
/* connection test runner */
static void test_wolfSSL_client_server(callback_functions* client_callbacks,
                                       callback_functions* server_callbacks)
{
    tcp_ready ready;
    func_args client_args;
    func_args server_args;
    THREAD_TYPE serverThread;

    XMEMSET(&client_args, 0, sizeof(func_args));
    XMEMSET(&server_args, 0, sizeof(func_args));

    StartTCP();

    client_args.callbacks = client_callbacks;
    server_args.callbacks = server_callbacks;

#ifdef WOLFSSL_TIRTOS
    fdOpenSession(Task_self());
#endif

    /* RUN Server side */
    InitTcpReady(&ready);

#if defined(USE_WINDOWS_API)
    /* use RNG to get random port if using windows */
    ready.port = GetRandomPort();
#endif

    server_args.signal = &ready;
    client_args.signal = &ready;
    start_thread(run_wolfssl_server, &server_args, &serverThread);
    wait_tcp_ready(&server_args);

    /* RUN Client side */
    run_wolfssl_client(&client_args);
    join_thread(serverThread);

    FreeTcpReady(&ready);
#ifdef WOLFSSL_TIRTOS
    fdCloseSession(Task_self());
#endif
}
#endif /* defined(HAVE_SNI) || defined(HAVE_ALPN) */

#ifdef HAVE_SNI
static void test_wolfSSL_UseSNI_params(void)
{
    WOLFSSL_CTX *ctx = wolfSSL_CTX_new(wolfSSLv23_client_method());
    WOLFSSL     *ssl = wolfSSL_new(ctx);

    AssertNotNull(ctx);
    AssertNotNull(ssl);

    /* invalid [ctx|ssl] */
    AssertIntNE(WOLFSSL_SUCCESS, wolfSSL_CTX_UseSNI(NULL, 0, "ctx", 3));
    AssertIntNE(WOLFSSL_SUCCESS, wolfSSL_UseSNI(    NULL, 0, "ssl", 3));
    /* invalid type */
    AssertIntNE(WOLFSSL_SUCCESS, wolfSSL_CTX_UseSNI(ctx, -1, "ctx", 3));
    AssertIntNE(WOLFSSL_SUCCESS, wolfSSL_UseSNI(    ssl, -1, "ssl", 3));
    /* invalid data */
    AssertIntNE(WOLFSSL_SUCCESS, wolfSSL_CTX_UseSNI(ctx,  0, NULL,  3));
    AssertIntNE(WOLFSSL_SUCCESS, wolfSSL_UseSNI(    ssl,  0, NULL,  3));
    /* success case */
    AssertIntEQ(WOLFSSL_SUCCESS, wolfSSL_CTX_UseSNI(ctx,  0, "ctx", 3));
    AssertIntEQ(WOLFSSL_SUCCESS, wolfSSL_UseSNI(    ssl,  0, "ssl", 3));

    wolfSSL_free(ssl);
    wolfSSL_CTX_free(ctx);
}

/* BEGIN of connection tests callbacks */
static void use_SNI_at_ctx(WOLFSSL_CTX* ctx)
{
    AssertIntEQ(WOLFSSL_SUCCESS,
        wolfSSL_CTX_UseSNI(ctx, WOLFSSL_SNI_HOST_NAME, "www.wolfssl.com", 15));
}

static void use_SNI_at_ssl(WOLFSSL* ssl)
{
    AssertIntEQ(WOLFSSL_SUCCESS,
             wolfSSL_UseSNI(ssl, WOLFSSL_SNI_HOST_NAME, "www.wolfssl.com", 15));
}

static void different_SNI_at_ssl(WOLFSSL* ssl)
{
    AssertIntEQ(WOLFSSL_SUCCESS,
             wolfSSL_UseSNI(ssl, WOLFSSL_SNI_HOST_NAME, "ww2.wolfssl.com", 15));
}

static void use_SNI_WITH_CONTINUE_at_ssl(WOLFSSL* ssl)
{
    use_SNI_at_ssl(ssl);
    wolfSSL_SNI_SetOptions(ssl, WOLFSSL_SNI_HOST_NAME,
                                              WOLFSSL_SNI_CONTINUE_ON_MISMATCH);
}

static void use_SNI_WITH_FAKE_ANSWER_at_ssl(WOLFSSL* ssl)
{
    use_SNI_at_ssl(ssl);
    wolfSSL_SNI_SetOptions(ssl, WOLFSSL_SNI_HOST_NAME,
                                                WOLFSSL_SNI_ANSWER_ON_MISMATCH);
}

static void use_MANDATORY_SNI_at_ctx(WOLFSSL_CTX* ctx)
{
    use_SNI_at_ctx(ctx);
    wolfSSL_CTX_SNI_SetOptions(ctx, WOLFSSL_SNI_HOST_NAME,
                                                  WOLFSSL_SNI_ABORT_ON_ABSENCE);
}

static void use_MANDATORY_SNI_at_ssl(WOLFSSL* ssl)
{
    use_SNI_at_ssl(ssl);
    wolfSSL_SNI_SetOptions(ssl, WOLFSSL_SNI_HOST_NAME,
                                                  WOLFSSL_SNI_ABORT_ON_ABSENCE);
}

static void use_PSEUDO_MANDATORY_SNI_at_ctx(WOLFSSL_CTX* ctx)
{
    use_SNI_at_ctx(ctx);
    wolfSSL_CTX_SNI_SetOptions(ctx, WOLFSSL_SNI_HOST_NAME,
                 WOLFSSL_SNI_ANSWER_ON_MISMATCH | WOLFSSL_SNI_ABORT_ON_ABSENCE);
}

static void verify_UNKNOWN_SNI_on_server(WOLFSSL* ssl)
{
    AssertIntEQ(UNKNOWN_SNI_HOST_NAME_E, wolfSSL_get_error(ssl, 0));
}

static void verify_SNI_ABSENT_on_server(WOLFSSL* ssl)
{
    AssertIntEQ(SNI_ABSENT_ERROR, wolfSSL_get_error(ssl, 0));
}

static void verify_SNI_no_matching(WOLFSSL* ssl)
{
    byte type = WOLFSSL_SNI_HOST_NAME;
    char* request = (char*) &type; /* to be overwritten */

    AssertIntEQ(WOLFSSL_SNI_NO_MATCH, wolfSSL_SNI_Status(ssl, type));
    AssertNotNull(request);
    AssertIntEQ(0, wolfSSL_SNI_GetRequest(ssl, type, (void**) &request));
    AssertNull(request);
}

static void verify_SNI_real_matching(WOLFSSL* ssl)
{
    byte type = WOLFSSL_SNI_HOST_NAME;
    char* request = NULL;

    AssertIntEQ(WOLFSSL_SNI_REAL_MATCH, wolfSSL_SNI_Status(ssl, type));
    AssertIntEQ(15, wolfSSL_SNI_GetRequest(ssl, type, (void**) &request));
    AssertNotNull(request);
    AssertStrEQ("www.wolfssl.com", request);
}

static void verify_SNI_fake_matching(WOLFSSL* ssl)
{
    byte type = WOLFSSL_SNI_HOST_NAME;
    char* request = NULL;

    AssertIntEQ(WOLFSSL_SNI_FAKE_MATCH, wolfSSL_SNI_Status(ssl, type));
    AssertIntEQ(15, wolfSSL_SNI_GetRequest(ssl, type, (void**) &request));
    AssertNotNull(request);
    AssertStrEQ("ww2.wolfssl.com", request);
}

static void verify_FATAL_ERROR_on_client(WOLFSSL* ssl)
{
    AssertIntEQ(FATAL_ERROR, wolfSSL_get_error(ssl, 0));
}
/* END of connection tests callbacks */

static void test_wolfSSL_UseSNI_connection(void)
{
    unsigned long i;
    callback_functions callbacks[] = {
        /* success case at ctx */
        {0, use_SNI_at_ctx, 0, 0, 0},
        {0, use_SNI_at_ctx, 0, verify_SNI_real_matching, 0},

        /* success case at ssl */
        {0, 0, use_SNI_at_ssl, verify_SNI_real_matching, 0},
        {0, 0, use_SNI_at_ssl, verify_SNI_real_matching, 0},

        /* default mismatch behavior */
        {0, 0, different_SNI_at_ssl, verify_FATAL_ERROR_on_client, 0},
        {0, 0, use_SNI_at_ssl,       verify_UNKNOWN_SNI_on_server, 0},

        /* continue on mismatch */
        {0, 0, different_SNI_at_ssl,         0, 0},
        {0, 0, use_SNI_WITH_CONTINUE_at_ssl, verify_SNI_no_matching, 0},

        /* fake answer on mismatch */
        {0, 0, different_SNI_at_ssl,            0, 0},
        {0, 0, use_SNI_WITH_FAKE_ANSWER_at_ssl, verify_SNI_fake_matching, 0},

        /* sni abort - success */
        {0, use_SNI_at_ctx,           0, 0, 0},
        {0, use_MANDATORY_SNI_at_ctx, 0, verify_SNI_real_matching, 0},

        /* sni abort - abort when absent (ctx) */
        {0, 0,                        0, verify_FATAL_ERROR_on_client, 0},
        {0, use_MANDATORY_SNI_at_ctx, 0, verify_SNI_ABSENT_on_server, 0},

        /* sni abort - abort when absent (ssl) */
        {0, 0, 0,                        verify_FATAL_ERROR_on_client, 0},
        {0, 0, use_MANDATORY_SNI_at_ssl, verify_SNI_ABSENT_on_server, 0},

        /* sni abort - success when overwritten */
        {0, 0, 0, 0, 0},
        {0, use_MANDATORY_SNI_at_ctx, use_SNI_at_ssl, verify_SNI_no_matching, 0},

        /* sni abort - success when allowing mismatches */
        {0, 0, different_SNI_at_ssl, 0, 0},
        {0, use_PSEUDO_MANDATORY_SNI_at_ctx, 0, verify_SNI_fake_matching, 0},
    };

    for (i = 0; i < sizeof(callbacks) / sizeof(callback_functions); i += 2) {
        callbacks[i    ].method = wolfSSLv23_client_method;
        callbacks[i + 1].method = wolfSSLv23_server_method;
        test_wolfSSL_client_server(&callbacks[i], &callbacks[i + 1]);
    }
}

static void test_wolfSSL_SNI_GetFromBuffer(void)
{
    byte buffer[] = { /* www.paypal.com */
        0x00, 0x00, 0x00, 0x00, 0xff, 0x01, 0x00, 0x00, 0x60, 0x03, 0x03, 0x5c,
        0xc4, 0xb3, 0x8c, 0x87, 0xef, 0xa4, 0x09, 0xe0, 0x02, 0xab, 0x86, 0xca,
        0x76, 0xf0, 0x9e, 0x01, 0x65, 0xf6, 0xa6, 0x06, 0x13, 0x1d, 0x0f, 0xa5,
        0x79, 0xb0, 0xd4, 0x77, 0x22, 0xeb, 0x1a, 0x00, 0x00, 0x16, 0x00, 0x6b,
        0x00, 0x67, 0x00, 0x39, 0x00, 0x33, 0x00, 0x3d, 0x00, 0x3c, 0x00, 0x35,
        0x00, 0x2f, 0x00, 0x05, 0x00, 0x04, 0x00, 0x0a, 0x01, 0x00, 0x00, 0x21,
        0x00, 0x00, 0x00, 0x13, 0x00, 0x11, 0x00, 0x00, 0x0e, 0x77, 0x77, 0x77,
        0x2e, 0x70, 0x61, 0x79, 0x70, 0x61, 0x6c, 0x2e, 0x63, 0x6f, 0x6d, 0x00,
        0x0d, 0x00, 0x06, 0x00, 0x04, 0x04, 0x01, 0x02, 0x01
    };

    byte buffer2[] = { /* api.textmate.org */
        0x16, 0x03, 0x01, 0x00, 0xc6, 0x01, 0x00, 0x00, 0xc2, 0x03, 0x03, 0x52,
        0x8b, 0x7b, 0xca, 0x69, 0xec, 0x97, 0xd5, 0x08, 0x03, 0x50, 0xfe, 0x3b,
        0x99, 0xc3, 0x20, 0xce, 0xa5, 0xf6, 0x99, 0xa5, 0x71, 0xf9, 0x57, 0x7f,
        0x04, 0x38, 0xf6, 0x11, 0x0b, 0xb8, 0xd3, 0x00, 0x00, 0x5e, 0x00, 0xff,
        0xc0, 0x24, 0xc0, 0x23, 0xc0, 0x0a, 0xc0, 0x09, 0xc0, 0x07, 0xc0, 0x08,
        0xc0, 0x28, 0xc0, 0x27, 0xc0, 0x14, 0xc0, 0x13, 0xc0, 0x11, 0xc0, 0x12,
        0xc0, 0x26, 0xc0, 0x25, 0xc0, 0x2a, 0xc0, 0x29, 0xc0, 0x05, 0xc0, 0x04,
        0xc0, 0x02, 0xc0, 0x03, 0xc0, 0x0f, 0xc0, 0x0e, 0xc0, 0x0c, 0xc0, 0x0d,
        0x00, 0x3d, 0x00, 0x3c, 0x00, 0x2f, 0x00, 0x05, 0x00, 0x04, 0x00, 0x35,
        0x00, 0x0a, 0x00, 0x67, 0x00, 0x6b, 0x00, 0x33, 0x00, 0x39, 0x00, 0x16,
        0x00, 0xaf, 0x00, 0xae, 0x00, 0x8d, 0x00, 0x8c, 0x00, 0x8a, 0x00, 0x8b,
        0x00, 0xb1, 0x00, 0xb0, 0x00, 0x2c, 0x00, 0x3b, 0x01, 0x00, 0x00, 0x3b,
        0x00, 0x00, 0x00, 0x15, 0x00, 0x13, 0x00, 0x00, 0x10, 0x61, 0x70, 0x69,
        0x2e, 0x74, 0x65, 0x78, 0x74, 0x6d, 0x61, 0x74, 0x65, 0x2e, 0x6f, 0x72,
        0x67, 0x00, 0x0a, 0x00, 0x08, 0x00, 0x06, 0x00, 0x17, 0x00, 0x18, 0x00,
        0x19, 0x00, 0x0b, 0x00, 0x02, 0x01, 0x00, 0x00, 0x0d, 0x00, 0x0c, 0x00,
        0x0a, 0x05, 0x01, 0x04, 0x01, 0x02, 0x01, 0x04, 0x03, 0x02, 0x03
    };

    byte buffer3[] = { /* no sni extension */
        0x16, 0x03, 0x03, 0x00, 0x4d, 0x01, 0x00, 0x00, 0x49, 0x03, 0x03, 0xea,
        0xa1, 0x9f, 0x60, 0xdd, 0x52, 0x12, 0x13, 0xbd, 0x84, 0x34, 0xd5, 0x1c,
        0x38, 0x25, 0xa8, 0x97, 0xd2, 0xd5, 0xc6, 0x45, 0xaf, 0x1b, 0x08, 0xe4,
        0x1e, 0xbb, 0xdf, 0x9d, 0x39, 0xf0, 0x65, 0x00, 0x00, 0x16, 0x00, 0x6b,
        0x00, 0x67, 0x00, 0x39, 0x00, 0x33, 0x00, 0x3d, 0x00, 0x3c, 0x00, 0x35,
        0x00, 0x2f, 0x00, 0x05, 0x00, 0x04, 0x00, 0x0a, 0x01, 0x00, 0x00, 0x0a,
        0x00, 0x0d, 0x00, 0x06, 0x00, 0x04, 0x04, 0x01, 0x02, 0x01
    };

    byte buffer4[] = { /* last extension has zero size */
        0x16, 0x03, 0x01, 0x00, 0xba, 0x01, 0x00, 0x00,
        0xb6, 0x03, 0x03, 0x83, 0xa3, 0xe6, 0xdc, 0x16, 0xa1, 0x43, 0xe9, 0x45,
        0x15, 0xbd, 0x64, 0xa9, 0xb6, 0x07, 0xb4, 0x50, 0xc6, 0xdd, 0xff, 0xc2,
        0xd3, 0x0d, 0x4f, 0x36, 0xb4, 0x41, 0x51, 0x61, 0xc1, 0xa5, 0x9e, 0x00,
        0x00, 0x28, 0xcc, 0x14, 0xcc, 0x13, 0xc0, 0x2b, 0xc0, 0x2f, 0x00, 0x9e,
        0xc0, 0x0a, 0xc0, 0x09, 0xc0, 0x13, 0xc0, 0x14, 0xc0, 0x07, 0xc0, 0x11,
        0x00, 0x33, 0x00, 0x32, 0x00, 0x39, 0x00, 0x9c, 0x00, 0x2f, 0x00, 0x35,
        0x00, 0x0a, 0x00, 0x05, 0x00, 0x04, 0x01, 0x00, 0x00, 0x65, 0xff, 0x01,
        0x00, 0x01, 0x00, 0x00, 0x0a, 0x00, 0x08, 0x00, 0x06, 0x00, 0x17, 0x00,
        0x18, 0x00, 0x19, 0x00, 0x0b, 0x00, 0x02, 0x01, 0x00, 0x00, 0x23, 0x00,
        0x00, 0x33, 0x74, 0x00, 0x00, 0x00, 0x10, 0x00, 0x1b, 0x00, 0x19, 0x06,
        0x73, 0x70, 0x64, 0x79, 0x2f, 0x33, 0x08, 0x73, 0x70, 0x64, 0x79, 0x2f,
        0x33, 0x2e, 0x31, 0x08, 0x68, 0x74, 0x74, 0x70, 0x2f, 0x31, 0x2e, 0x31,
        0x75, 0x50, 0x00, 0x00, 0x00, 0x05, 0x00, 0x05, 0x01, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x0d, 0x00, 0x12, 0x00, 0x10, 0x04, 0x01, 0x05, 0x01, 0x02,
        0x01, 0x04, 0x03, 0x05, 0x03, 0x02, 0x03, 0x04, 0x02, 0x02, 0x02, 0x00,
        0x12, 0x00, 0x00
    };

    byte buffer5[] = { /* SSL v2.0 client hello */
        0x00, 0x2b, 0x01, 0x03, 0x01, 0x00, 0x09, 0x00, 0x00,
        /* dummy bytes bellow, just to pass size check */
        0xb6, 0x03, 0x03, 0x83, 0xa3, 0xe6, 0xdc, 0x16, 0xa1, 0x43, 0xe9, 0x45,
        0x15, 0xbd, 0x64, 0xa9, 0xb6, 0x07, 0xb4, 0x50, 0xc6, 0xdd, 0xff, 0xc2,
        0xd3, 0x0d, 0x4f, 0x36, 0xb4, 0x41, 0x51, 0x61, 0xc1, 0xa5, 0x9e, 0x00,
    };

    byte result[32] = {0};
    word32 length   = 32;

    AssertIntEQ(0, wolfSSL_SNI_GetFromBuffer(buffer4, sizeof(buffer4),
                                                           0, result, &length));

    AssertIntEQ(0, wolfSSL_SNI_GetFromBuffer(buffer3, sizeof(buffer3),
                                                           0, result, &length));

    AssertIntEQ(0, wolfSSL_SNI_GetFromBuffer(buffer2, sizeof(buffer2),
                                                           1, result, &length));

    AssertIntEQ(BUFFER_ERROR, wolfSSL_SNI_GetFromBuffer(buffer, sizeof(buffer),
                                                           0, result, &length));
    buffer[0] = 0x16;

    AssertIntEQ(BUFFER_ERROR, wolfSSL_SNI_GetFromBuffer(buffer, sizeof(buffer),
                                                           0, result, &length));
    buffer[1] = 0x03;

    AssertIntEQ(SNI_UNSUPPORTED, wolfSSL_SNI_GetFromBuffer(buffer,
                                           sizeof(buffer), 0, result, &length));
    buffer[2] = 0x03;

    AssertIntEQ(INCOMPLETE_DATA, wolfSSL_SNI_GetFromBuffer(buffer,
                                           sizeof(buffer), 0, result, &length));
    buffer[4] = 0x64;

    AssertIntEQ(WOLFSSL_SUCCESS, wolfSSL_SNI_GetFromBuffer(buffer, sizeof(buffer),
                                                           0, result, &length));
    result[length] = 0;
    AssertStrEQ("www.paypal.com", (const char*) result);

    length = 32;

    AssertIntEQ(WOLFSSL_SUCCESS, wolfSSL_SNI_GetFromBuffer(buffer2, sizeof(buffer2),
                                                           0, result, &length));
    result[length] = 0;
    AssertStrEQ("api.textmate.org", (const char*) result);

    /* SSL v2.0 tests */
    AssertIntEQ(SNI_UNSUPPORTED, wolfSSL_SNI_GetFromBuffer(buffer5,
                                          sizeof(buffer5), 0, result, &length));

    buffer5[2] = 0x02;
    AssertIntEQ(BUFFER_ERROR, wolfSSL_SNI_GetFromBuffer(buffer5,
                                          sizeof(buffer5), 0, result, &length));

    buffer5[2] = 0x01; buffer5[6] = 0x08;
    AssertIntEQ(BUFFER_ERROR, wolfSSL_SNI_GetFromBuffer(buffer5,
                                          sizeof(buffer5), 0, result, &length));

    buffer5[6] = 0x09; buffer5[8] = 0x01;
    AssertIntEQ(BUFFER_ERROR, wolfSSL_SNI_GetFromBuffer(buffer5,
                                          sizeof(buffer5), 0, result, &length));
}

#endif /* HAVE_SNI */

static void test_wolfSSL_UseSNI(void)
{
#ifdef HAVE_SNI
    test_wolfSSL_UseSNI_params();
    test_wolfSSL_UseSNI_connection();

    test_wolfSSL_SNI_GetFromBuffer();
#endif
}

#endif /* HAVE_IO_TESTS_DEPENDENCIES */

static void test_wolfSSL_UseTrustedCA(void)
{
#if defined(HAVE_TRUSTED_CA) && !defined(NO_CERTS) && !defined(NO_FILESYSTEM)
    WOLFSSL_CTX *ctx;
    WOLFSSL     *ssl;
    byte        id[20];

#ifndef NO_WOLFSSL_SERVER
    AssertNotNull((ctx = wolfSSL_CTX_new(wolfSSLv23_server_method())));
    AssertTrue(wolfSSL_CTX_use_certificate_file(ctx, svrCertFile, SSL_FILETYPE_PEM));
    AssertTrue(SSL_CTX_use_PrivateKey_file(ctx, svrKeyFile, SSL_FILETYPE_PEM));
#else
    AssertNotNull((ctx = wolfSSL_CTX_new(wolfSSLv23_client_method())));
#endif
    AssertNotNull((ssl = wolfSSL_new(ctx)));
    XMEMSET(id, 0, sizeof(id));

    /* error cases */
    AssertIntNE(WOLFSSL_SUCCESS, wolfSSL_UseTrustedCA(NULL, 0, NULL, 0));
    AssertIntNE(WOLFSSL_SUCCESS, wolfSSL_UseTrustedCA(ssl,
                WOLFSSL_TRUSTED_CA_CERT_SHA1+1, NULL, 0));
    AssertIntNE(WOLFSSL_SUCCESS, wolfSSL_UseTrustedCA(ssl,
                WOLFSSL_TRUSTED_CA_CERT_SHA1, NULL, 0));
    AssertIntNE(WOLFSSL_SUCCESS, wolfSSL_UseTrustedCA(ssl,
                WOLFSSL_TRUSTED_CA_CERT_SHA1, id, 5));
#ifdef NO_SHA
    AssertIntNE(WOLFSSL_SUCCESS, wolfSSL_UseTrustedCA(ssl,
                WOLFSSL_TRUSTED_CA_KEY_SHA1, id, sizeof(id)));
#endif
    AssertIntNE(WOLFSSL_SUCCESS, wolfSSL_UseTrustedCA(ssl,
                WOLFSSL_TRUSTED_CA_X509_NAME, id, 0));

    /* success cases */
    AssertIntEQ(WOLFSSL_SUCCESS, wolfSSL_UseTrustedCA(ssl,
                WOLFSSL_TRUSTED_CA_PRE_AGREED, NULL, 0));
#ifndef NO_SHA
    AssertIntEQ(WOLFSSL_SUCCESS, wolfSSL_UseTrustedCA(ssl,
                WOLFSSL_TRUSTED_CA_KEY_SHA1, id, sizeof(id)));
#endif
    AssertIntEQ(WOLFSSL_SUCCESS, wolfSSL_UseTrustedCA(ssl,
                WOLFSSL_TRUSTED_CA_X509_NAME, id, 5));

    wolfSSL_free(ssl);
    wolfSSL_CTX_free(ctx);
#endif /* HAVE_TRUSTED_CA */
}

static void test_wolfSSL_UseMaxFragment(void)
{
#if defined(HAVE_MAX_FRAGMENT) && !defined(NO_CERTS) && !defined(NO_FILESYSTEM)
  #ifndef NO_WOLFSSL_SERVER
    WOLFSSL_CTX* ctx = wolfSSL_CTX_new(wolfSSLv23_server_method());
    AssertTrue(wolfSSL_CTX_use_certificate_file(ctx, svrCertFile, SSL_FILETYPE_PEM));
    AssertTrue(SSL_CTX_use_PrivateKey_file(ctx, svrKeyFile, SSL_FILETYPE_PEM));
  #else
    WOLFSSL_CTX* ctx = wolfSSL_CTX_new(wolfSSLv23_client_method());
  #endif
    WOLFSSL     *ssl = wolfSSL_new(ctx);

    AssertNotNull(ctx);
    AssertNotNull(ssl);

    /* error cases */
    AssertIntNE(WOLFSSL_SUCCESS, wolfSSL_CTX_UseMaxFragment(NULL, WOLFSSL_MFL_2_9));
    AssertIntNE(WOLFSSL_SUCCESS, wolfSSL_UseMaxFragment(    NULL, WOLFSSL_MFL_2_9));
    AssertIntNE(WOLFSSL_SUCCESS, wolfSSL_CTX_UseMaxFragment(ctx, WOLFSSL_MFL_MIN-1));
    AssertIntNE(WOLFSSL_SUCCESS, wolfSSL_CTX_UseMaxFragment(ctx, WOLFSSL_MFL_MAX+1));
    AssertIntNE(WOLFSSL_SUCCESS, wolfSSL_UseMaxFragment(ssl, WOLFSSL_MFL_MIN-1));
    AssertIntNE(WOLFSSL_SUCCESS, wolfSSL_UseMaxFragment(ssl, WOLFSSL_MFL_MAX+1));

    /* success case */
    AssertIntEQ(WOLFSSL_SUCCESS, wolfSSL_CTX_UseMaxFragment(ctx,  WOLFSSL_MFL_2_8));
    AssertIntEQ(WOLFSSL_SUCCESS, wolfSSL_CTX_UseMaxFragment(ctx,  WOLFSSL_MFL_2_9));
    AssertIntEQ(WOLFSSL_SUCCESS, wolfSSL_CTX_UseMaxFragment(ctx,  WOLFSSL_MFL_2_10));
    AssertIntEQ(WOLFSSL_SUCCESS, wolfSSL_CTX_UseMaxFragment(ctx,  WOLFSSL_MFL_2_11));
    AssertIntEQ(WOLFSSL_SUCCESS, wolfSSL_CTX_UseMaxFragment(ctx,  WOLFSSL_MFL_2_12));
    AssertIntEQ(WOLFSSL_SUCCESS, wolfSSL_CTX_UseMaxFragment(ctx,  WOLFSSL_MFL_2_13));
    AssertIntEQ(WOLFSSL_SUCCESS, wolfSSL_UseMaxFragment(    ssl,  WOLFSSL_MFL_2_8));
    AssertIntEQ(WOLFSSL_SUCCESS, wolfSSL_UseMaxFragment(    ssl,  WOLFSSL_MFL_2_9));
    AssertIntEQ(WOLFSSL_SUCCESS, wolfSSL_UseMaxFragment(    ssl,  WOLFSSL_MFL_2_10));
    AssertIntEQ(WOLFSSL_SUCCESS, wolfSSL_UseMaxFragment(    ssl,  WOLFSSL_MFL_2_11));
    AssertIntEQ(WOLFSSL_SUCCESS, wolfSSL_UseMaxFragment(    ssl,  WOLFSSL_MFL_2_12));
    AssertIntEQ(WOLFSSL_SUCCESS, wolfSSL_UseMaxFragment(    ssl,  WOLFSSL_MFL_2_13));

    wolfSSL_free(ssl);
    wolfSSL_CTX_free(ctx);
#endif
}

static void test_wolfSSL_UseTruncatedHMAC(void)
{
#if defined(HAVE_TRUNCATED_HMAC) && !defined(NO_CERTS) && !defined(NO_FILESYSTEM)
  #ifndef NO_WOLFSSL_SERVER
    WOLFSSL_CTX* ctx = wolfSSL_CTX_new(wolfSSLv23_server_method());
    AssertTrue(wolfSSL_CTX_use_certificate_file(ctx, svrCertFile, SSL_FILETYPE_PEM));
    AssertTrue(SSL_CTX_use_PrivateKey_file(ctx, svrKeyFile, SSL_FILETYPE_PEM));
  #else
    WOLFSSL_CTX* ctx = wolfSSL_CTX_new(wolfSSLv23_client_method());
  #endif
    WOLFSSL     *ssl = wolfSSL_new(ctx);

    AssertNotNull(ctx);
    AssertNotNull(ssl);

    /* error cases */
    AssertIntNE(WOLFSSL_SUCCESS, wolfSSL_CTX_UseTruncatedHMAC(NULL));
    AssertIntNE(WOLFSSL_SUCCESS, wolfSSL_UseTruncatedHMAC(NULL));

    /* success case */
    AssertIntEQ(WOLFSSL_SUCCESS, wolfSSL_CTX_UseTruncatedHMAC(ctx));
    AssertIntEQ(WOLFSSL_SUCCESS, wolfSSL_UseTruncatedHMAC(ssl));

    wolfSSL_free(ssl);
    wolfSSL_CTX_free(ctx);
#endif
}

static void test_wolfSSL_UseSupportedCurve(void)
{
#if defined(HAVE_SUPPORTED_CURVES) && !defined(NO_WOLFSSL_CLIENT) && !defined(NO_TLS)
    WOLFSSL_CTX* ctx = wolfSSL_CTX_new(wolfSSLv23_client_method());
    WOLFSSL     *ssl = wolfSSL_new(ctx);

    AssertNotNull(ctx);
    AssertNotNull(ssl);

    /* error cases */
    AssertIntNE(WOLFSSL_SUCCESS,
                      wolfSSL_CTX_UseSupportedCurve(NULL, WOLFSSL_ECC_SECP256R1));
    AssertIntNE(WOLFSSL_SUCCESS, wolfSSL_CTX_UseSupportedCurve(ctx,  0));

    AssertIntNE(WOLFSSL_SUCCESS,
                          wolfSSL_UseSupportedCurve(NULL, WOLFSSL_ECC_SECP256R1));
    AssertIntNE(WOLFSSL_SUCCESS, wolfSSL_UseSupportedCurve(ssl,  0));

    /* success case */
    AssertIntEQ(WOLFSSL_SUCCESS,
                       wolfSSL_CTX_UseSupportedCurve(ctx, WOLFSSL_ECC_SECP256R1));
    AssertIntEQ(WOLFSSL_SUCCESS,
                           wolfSSL_UseSupportedCurve(ssl, WOLFSSL_ECC_SECP256R1));

    wolfSSL_free(ssl);
    wolfSSL_CTX_free(ctx);
#endif
}

#if defined(HAVE_ALPN) && !defined(NO_WOLFSSL_SERVER)

static void verify_ALPN_FATAL_ERROR_on_client(WOLFSSL* ssl)
{
    AssertIntEQ(UNKNOWN_ALPN_PROTOCOL_NAME_E, wolfSSL_get_error(ssl, 0));
}

static void use_ALPN_all(WOLFSSL* ssl)
{
    /* http/1.1,spdy/1,spdy/2,spdy/3 */
    char alpn_list[] = {0x68, 0x74, 0x74, 0x70, 0x2f, 0x31, 0x2e, 0x31, 0x2c,
                        0x73, 0x70, 0x64, 0x79, 0x2f, 0x31, 0x2c,
                        0x73, 0x70, 0x64, 0x79, 0x2f, 0x32, 0x2c,
                        0x73, 0x70, 0x64, 0x79, 0x2f, 0x33};
    AssertIntEQ(WOLFSSL_SUCCESS, wolfSSL_UseALPN(ssl, alpn_list, sizeof(alpn_list),
                                             WOLFSSL_ALPN_FAILED_ON_MISMATCH));
}

static void use_ALPN_all_continue(WOLFSSL* ssl)
{
    /* http/1.1,spdy/1,spdy/2,spdy/3 */
    char alpn_list[] = {0x68, 0x74, 0x74, 0x70, 0x2f, 0x31, 0x2e, 0x31, 0x2c,
        0x73, 0x70, 0x64, 0x79, 0x2f, 0x31, 0x2c,
        0x73, 0x70, 0x64, 0x79, 0x2f, 0x32, 0x2c,
        0x73, 0x70, 0x64, 0x79, 0x2f, 0x33};
    AssertIntEQ(WOLFSSL_SUCCESS, wolfSSL_UseALPN(ssl, alpn_list, sizeof(alpn_list),
                                             WOLFSSL_ALPN_CONTINUE_ON_MISMATCH));
}

static void use_ALPN_one(WOLFSSL* ssl)
{
    /* spdy/2 */
    char proto[] = {0x73, 0x70, 0x64, 0x79, 0x2f, 0x32};

    AssertIntEQ(WOLFSSL_SUCCESS, wolfSSL_UseALPN(ssl, proto, sizeof(proto),
                                             WOLFSSL_ALPN_FAILED_ON_MISMATCH));
}

static void use_ALPN_unknown(WOLFSSL* ssl)
{
    /* http/2.0 */
    char proto[] = {0x68, 0x74, 0x74, 0x70, 0x2f, 0x32, 0x2e, 0x30};

    AssertIntEQ(WOLFSSL_SUCCESS, wolfSSL_UseALPN(ssl, proto, sizeof(proto),
                                             WOLFSSL_ALPN_FAILED_ON_MISMATCH));
}

static void use_ALPN_unknown_continue(WOLFSSL* ssl)
{
    /* http/2.0 */
    char proto[] = {0x68, 0x74, 0x74, 0x70, 0x2f, 0x32, 0x2e, 0x30};

    AssertIntEQ(WOLFSSL_SUCCESS, wolfSSL_UseALPN(ssl, proto, sizeof(proto),
                                             WOLFSSL_ALPN_CONTINUE_ON_MISMATCH));
}

static void verify_ALPN_not_matching_spdy3(WOLFSSL* ssl)
{
    /* spdy/3 */
    char nego_proto[] = {0x73, 0x70, 0x64, 0x79, 0x2f, 0x33};

    char *proto = NULL;
    word16 protoSz = 0;

    AssertIntEQ(WOLFSSL_SUCCESS, wolfSSL_ALPN_GetProtocol(ssl, &proto, &protoSz));

    /* check value */
    AssertIntNE(1, sizeof(nego_proto) == protoSz);
    if (proto) {
        AssertIntNE(0, XMEMCMP(nego_proto, proto, sizeof(nego_proto)));
    }
}

static void verify_ALPN_not_matching_continue(WOLFSSL* ssl)
{
    char *proto = NULL;
    word16 protoSz = 0;

    AssertIntEQ(WOLFSSL_ALPN_NOT_FOUND,
                wolfSSL_ALPN_GetProtocol(ssl, &proto, &protoSz));

    /* check value */
    AssertIntEQ(1, (0 == protoSz));
    AssertIntEQ(1, (NULL == proto));
}

static void verify_ALPN_matching_http1(WOLFSSL* ssl)
{
    /* http/1.1 */
    char nego_proto[] = {0x68, 0x74, 0x74, 0x70, 0x2f, 0x31, 0x2e, 0x31};
    char *proto;
    word16 protoSz = 0;

    AssertIntEQ(WOLFSSL_SUCCESS, wolfSSL_ALPN_GetProtocol(ssl, &proto, &protoSz));

    /* check value */
    AssertIntEQ(1, sizeof(nego_proto) == protoSz);
    AssertIntEQ(0, XMEMCMP(nego_proto, proto, protoSz));
}

static void verify_ALPN_matching_spdy2(WOLFSSL* ssl)
{
    /* spdy/2 */
    char nego_proto[] = {0x73, 0x70, 0x64, 0x79, 0x2f, 0x32};
    char *proto;
    word16 protoSz = 0;

    AssertIntEQ(WOLFSSL_SUCCESS, wolfSSL_ALPN_GetProtocol(ssl, &proto, &protoSz));

    /* check value */
    AssertIntEQ(1, sizeof(nego_proto) == protoSz);
    AssertIntEQ(0, XMEMCMP(nego_proto, proto, protoSz));
}

static void verify_ALPN_client_list(WOLFSSL* ssl)
{
    /* http/1.1,spdy/1,spdy/2,spdy/3 */
    char alpn_list[] = {0x68, 0x74, 0x74, 0x70, 0x2f, 0x31, 0x2e, 0x31, 0x2c,
                        0x73, 0x70, 0x64, 0x79, 0x2f, 0x31, 0x2c,
                        0x73, 0x70, 0x64, 0x79, 0x2f, 0x32, 0x2c,
                        0x73, 0x70, 0x64, 0x79, 0x2f, 0x33};
    char    *clist = NULL;
    word16  clistSz = 0;

    AssertIntEQ(WOLFSSL_SUCCESS, wolfSSL_ALPN_GetPeerProtocol(ssl, &clist,
                                                          &clistSz));

    /* check value */
    AssertIntEQ(1, sizeof(alpn_list) == clistSz);
    AssertIntEQ(0, XMEMCMP(alpn_list, clist, clistSz));

    AssertIntEQ(WOLFSSL_SUCCESS, wolfSSL_ALPN_FreePeerProtocol(ssl, &clist));
}

static void test_wolfSSL_UseALPN_connection(void)
{
    unsigned long i;
    callback_functions callbacks[] = {
        /* success case same list */
        {0, 0, use_ALPN_all, 0, 0},
        {0, 0, use_ALPN_all, verify_ALPN_matching_http1, 0},

        /* success case only one for server */
        {0, 0, use_ALPN_all, 0, 0},
        {0, 0, use_ALPN_one, verify_ALPN_matching_spdy2, 0},

        /* success case only one for client */
        {0, 0, use_ALPN_one, 0, 0},
        {0, 0, use_ALPN_all, verify_ALPN_matching_spdy2, 0},

        /* success case none for client */
        {0, 0, 0, 0, 0},
        {0, 0, use_ALPN_all, 0, 0},

        /* success case mismatch behavior but option 'continue' set */
        {0, 0, use_ALPN_all_continue, verify_ALPN_not_matching_continue, 0},
        {0, 0, use_ALPN_unknown_continue, 0, 0},

        /* success case read protocol send by client */
        {0, 0, use_ALPN_all, 0, 0},
        {0, 0, use_ALPN_one, verify_ALPN_client_list, 0},

        /* mismatch behavior with same list
         * the first and only this one must be taken */
        {0, 0, use_ALPN_all, 0, 0},
        {0, 0, use_ALPN_all, verify_ALPN_not_matching_spdy3, 0},

        /* default mismatch behavior */
        {0, 0, use_ALPN_all, 0, 0},
        {0, 0, use_ALPN_unknown, verify_ALPN_FATAL_ERROR_on_client, 0},
    };

    for (i = 0; i < sizeof(callbacks) / sizeof(callback_functions); i += 2) {
        callbacks[i    ].method = wolfSSLv23_client_method;
        callbacks[i + 1].method = wolfSSLv23_server_method;
        test_wolfSSL_client_server(&callbacks[i], &callbacks[i + 1]);
    }
}

static void test_wolfSSL_UseALPN_params(void)
{
#ifndef NO_WOLFSSL_CLIENT
    /* "http/1.1" */
    char http1[] = {0x68, 0x74, 0x74, 0x70, 0x2f, 0x31, 0x2e, 0x31};
    /* "spdy/1" */
    char spdy1[] = {0x73, 0x70, 0x64, 0x79, 0x2f, 0x31};
    /* "spdy/2" */
    char spdy2[] = {0x73, 0x70, 0x64, 0x79, 0x2f, 0x32};
    /* "spdy/3" */
    char spdy3[] = {0x73, 0x70, 0x64, 0x79, 0x2f, 0x33};
    char buff[256];
    word32 idx;

    WOLFSSL_CTX *ctx = wolfSSL_CTX_new(wolfSSLv23_client_method());
    WOLFSSL     *ssl = wolfSSL_new(ctx);

    AssertNotNull(ctx);
    AssertNotNull(ssl);

    /* error cases */
    AssertIntNE(WOLFSSL_SUCCESS,
                wolfSSL_UseALPN(NULL, http1, sizeof(http1),
                                WOLFSSL_ALPN_FAILED_ON_MISMATCH));
    AssertIntNE(WOLFSSL_SUCCESS, wolfSSL_UseALPN(ssl, NULL, 0,
                                             WOLFSSL_ALPN_FAILED_ON_MISMATCH));

    /* success case */
    /* http1 only */
    AssertIntEQ(WOLFSSL_SUCCESS,
                wolfSSL_UseALPN(ssl, http1, sizeof(http1),
                                WOLFSSL_ALPN_FAILED_ON_MISMATCH));

    /* http1, spdy1 */
    XMEMCPY(buff, http1, sizeof(http1));
    idx = sizeof(http1);
    buff[idx++] = ',';
    XMEMCPY(buff+idx, spdy1, sizeof(spdy1));
    idx += sizeof(spdy1);
    AssertIntEQ(WOLFSSL_SUCCESS, wolfSSL_UseALPN(ssl, buff, idx,
                                             WOLFSSL_ALPN_FAILED_ON_MISMATCH));

    /* http1, spdy2, spdy1 */
    XMEMCPY(buff, http1, sizeof(http1));
    idx = sizeof(http1);
    buff[idx++] = ',';
    XMEMCPY(buff+idx, spdy2, sizeof(spdy2));
    idx += sizeof(spdy2);
    buff[idx++] = ',';
    XMEMCPY(buff+idx, spdy1, sizeof(spdy1));
    idx += sizeof(spdy1);
    AssertIntEQ(WOLFSSL_SUCCESS, wolfSSL_UseALPN(ssl, buff, idx,
                                             WOLFSSL_ALPN_FAILED_ON_MISMATCH));

    /* spdy3, http1, spdy2, spdy1 */
    XMEMCPY(buff, spdy3, sizeof(spdy3));
    idx = sizeof(spdy3);
    buff[idx++] = ',';
    XMEMCPY(buff+idx, http1, sizeof(http1));
    idx += sizeof(http1);
    buff[idx++] = ',';
    XMEMCPY(buff+idx, spdy2, sizeof(spdy2));
    idx += sizeof(spdy2);
    buff[idx++] = ',';
    XMEMCPY(buff+idx, spdy1, sizeof(spdy1));
    idx += sizeof(spdy1);
    AssertIntEQ(WOLFSSL_SUCCESS, wolfSSL_UseALPN(ssl, buff, idx,
                                             WOLFSSL_ALPN_CONTINUE_ON_MISMATCH));

    wolfSSL_free(ssl);
    wolfSSL_CTX_free(ctx);
#endif
}
#endif /* HAVE_ALPN  */

static void test_wolfSSL_UseALPN(void)
{
#if defined(HAVE_ALPN) && !defined(NO_WOLFSSL_SERVER)
    test_wolfSSL_UseALPN_connection();
    test_wolfSSL_UseALPN_params();
#endif
}

static void test_wolfSSL_DisableExtendedMasterSecret(void)
{
#if defined(HAVE_EXTENDED_MASTER) && !defined(NO_WOLFSSL_CLIENT)
    WOLFSSL_CTX *ctx = wolfSSL_CTX_new(wolfSSLv23_client_method());
    WOLFSSL     *ssl = wolfSSL_new(ctx);

    AssertNotNull(ctx);
    AssertNotNull(ssl);

    /* error cases */
    AssertIntNE(WOLFSSL_SUCCESS, wolfSSL_CTX_DisableExtendedMasterSecret(NULL));
    AssertIntNE(WOLFSSL_SUCCESS, wolfSSL_DisableExtendedMasterSecret(NULL));

    /* success cases */
    AssertIntEQ(WOLFSSL_SUCCESS, wolfSSL_CTX_DisableExtendedMasterSecret(ctx));
    AssertIntEQ(WOLFSSL_SUCCESS, wolfSSL_DisableExtendedMasterSecret(ssl));

    wolfSSL_free(ssl);
    wolfSSL_CTX_free(ctx);
#endif
}

static void test_wolfSSL_wolfSSL_UseSecureRenegotiation(void)
{
#if defined(HAVE_SECURE_RENEGOTIATION) && !defined(NO_WOLFSSL_CLIENT)
    WOLFSSL_CTX *ctx = wolfSSL_CTX_new(wolfSSLv23_client_method());
    WOLFSSL     *ssl = wolfSSL_new(ctx);

    AssertNotNull(ctx);
    AssertNotNull(ssl);

    /* error cases */
    AssertIntNE(WOLFSSL_SUCCESS, wolfSSL_CTX_UseSecureRenegotiation(NULL));
    AssertIntNE(WOLFSSL_SUCCESS, wolfSSL_UseSecureRenegotiation(NULL));

    /* success cases */
    AssertIntEQ(WOLFSSL_SUCCESS, wolfSSL_CTX_UseSecureRenegotiation(ctx));
    AssertIntEQ(WOLFSSL_SUCCESS, wolfSSL_UseSecureRenegotiation(ssl));

    wolfSSL_free(ssl);
    wolfSSL_CTX_free(ctx);
#endif
}


/*----------------------------------------------------------------------------*
 | X509 Tests
 *----------------------------------------------------------------------------*/
static void test_wolfSSL_X509_NAME_get_entry(void)
{
#if !defined(NO_CERTS) && !defined(NO_RSA)
#if defined(OPENSSL_ALL) || \
        (defined(OPENSSL_EXTRA) && \
            (defined(KEEP_PEER_CERT) || defined(SESSION_CERTS)))
    printf(testingFmt, "wolfSSL_X509_NAME_get_entry()");

    {
        /* use openssl like name to test mapping */
        X509_NAME_ENTRY* ne = NULL;
        X509_NAME* name = NULL;
        char* subCN = NULL;
        X509* x509;
        ASN1_STRING* asn;
        int idx;
        ASN1_OBJECT *object = NULL;
#if defined(WOLFSSL_APACHE_HTTPD) || defined(OPENSSL_ALL) || defined(WOLFSSL_NGINX)
        BIO* bio;
#endif

    #ifndef NO_FILESYSTEM
        x509 = wolfSSL_X509_load_certificate_file(cliCertFile, WOLFSSL_FILETYPE_PEM);
        AssertNotNull(x509);
        name = X509_get_subject_name(x509);
        idx = X509_NAME_get_index_by_NID(name, NID_commonName, -1);
        AssertIntGE(idx, 0);
        ne = X509_NAME_get_entry(name, idx);
        AssertNotNull(ne);
        asn = X509_NAME_ENTRY_get_data(ne);
        AssertNotNull(asn);
        subCN = (char*)ASN1_STRING_data(asn);
        AssertNotNull(subCN);
        wolfSSL_FreeX509(x509);
    #endif

        x509 = wolfSSL_X509_load_certificate_file(cliCertFile, WOLFSSL_FILETYPE_PEM);
        AssertNotNull(x509);
        name = X509_get_subject_name(x509);
        idx = X509_NAME_get_index_by_NID(name, NID_commonName, -1);
        AssertIntGE(idx, 0);

#if defined(WOLFSSL_APACHE_HTTPD) || defined(OPENSSL_ALL) || defined(WOLFSSL_NGINX)
        AssertNotNull(bio = BIO_new(BIO_s_mem()));
        AssertIntEQ(X509_NAME_print_ex(bio, name, 4,
                        (XN_FLAG_RFC2253 & ~XN_FLAG_DN_REV)), WOLFSSL_SUCCESS);
        BIO_free(bio);
#endif

        ne = X509_NAME_get_entry(name, idx);
        AssertNotNull(ne);
        AssertNotNull(object = X509_NAME_ENTRY_get_object(ne));
        wolfSSL_FreeX509(x509);
    }

    printf(resultFmt, passed);
#endif /* OPENSSL_ALL || (OPENSSL_EXTRA && (KEEP_PEER_CERT || SESSION_CERTS) */
#endif /* !NO_CERTS && !NO_RSA */
}

/* Testing functions dealing with PKCS12 parsing out X509 certs */
static void test_wolfSSL_PKCS12(void)
{
    /* .p12 file is encrypted with DES3 */
#ifndef HAVE_FIPS /* Password used in cert "wolfSSL test" is only 12-bytes
                   * (96-bit) FIPS mode requires Minimum of 14-byte (112-bit)
                   * Password Key
                   */
#if defined(OPENSSL_EXTRA) && !defined(NO_DES3) && !defined(NO_FILESYSTEM) && \
    !defined(NO_ASN) && !defined(NO_PWDBASED) && !defined(NO_RSA) && \
    !defined(NO_SHA)
    byte buffer[5300];
    char file[] = "./certs/test-servercert.p12";
    char order[] = "./certs/ecc-rsa-server.p12";
    char pass[] = "a password";
    WOLFSSL_X509_NAME* subject;
    XFILE f;
    int  bytes, ret;
    WOLFSSL_BIO      *bio;
    WOLFSSL_EVP_PKEY *pkey;
    WC_PKCS12        *pkcs12;
    WC_PKCS12        *pkcs12_2;
    WOLFSSL_X509     *cert;
    WOLFSSL_X509     *x509;
    WOLFSSL_X509     *tmp;
    WOLFSSL_CTX      *ctx;
    WOLF_STACK_OF(WOLFSSL_X509) *ca;
#if defined(OPENSSL_ALL) || defined(WOLFSSL_ASIO) || defined(WOLFSSL_HAPROXY) \
    || defined(WOLFSSL_NGINX)
    WOLFSSL          *ssl;
    WOLF_STACK_OF(WOLFSSL_X509) *tmp_ca = NULL;
#endif

    printf(testingFmt, "wolfSSL_PKCS12()");

    f = XFOPEN(file, "rb");
    AssertTrue((f != XBADFILE));
    bytes = (int)XFREAD(buffer, 1, sizeof(buffer), f);
    XFCLOSE(f);

    bio = BIO_new_mem_buf((void*)buffer, bytes);
    AssertNotNull(bio);

    pkcs12 = d2i_PKCS12_bio(bio, NULL);
    AssertNotNull(pkcs12);
    PKCS12_free(pkcs12);

    d2i_PKCS12_bio(bio, &pkcs12);
    AssertNotNull(pkcs12);

    /* check verify MAC fail case */
    ret = PKCS12_parse(pkcs12, "bad", &pkey, &cert, NULL);
    AssertIntEQ(ret, 0);
    AssertNull(pkey);
    AssertNull(cert);

    /* check parse with no extra certs kept */
    ret = PKCS12_parse(pkcs12, "wolfSSL test", &pkey, &cert, NULL);
    AssertIntEQ(ret, 1);
    AssertNotNull(pkey);
    AssertNotNull(cert);

    wolfSSL_EVP_PKEY_free(pkey);
    wolfSSL_X509_free(cert);

    /* check parse with extra certs kept */
    ret = PKCS12_parse(pkcs12, "wolfSSL test", &pkey, &cert, &ca);
    AssertIntEQ(ret, 1);
    AssertNotNull(pkey);
    AssertNotNull(cert);
    AssertNotNull(ca);

    /* Check that SSL_CTX_set0_chain correctly sets the certChain buffer */
#ifndef NO_WOLFSSL_CLIENT
    AssertNotNull(ctx = wolfSSL_CTX_new(wolfSSLv23_client_method()));
#else
    AssertNotNull(ctx = wolfSSL_CTX_new(wolfSSLv23_server_method()));
#endif
#if defined(OPENSSL_ALL) || defined(WOLFSSL_ASIO) || defined(WOLFSSL_HAPROXY) \
    || defined(WOLFSSL_NGINX)
    /* Copy stack structure */
    AssertNotNull(tmp_ca = sk_X509_dup(ca));
    AssertIntEQ(SSL_CTX_set0_chain(ctx, tmp_ca), 1);
    /* CTX now owns the tmp_ca stack structure */
    tmp_ca = NULL;
    AssertIntEQ(wolfSSL_CTX_get_extra_chain_certs(ctx, &tmp_ca), 1);
    AssertNotNull(tmp_ca);
    AssertIntEQ(sk_X509_num(tmp_ca), sk_X509_num(ca));
    /* Check that the main cert is also set */
    AssertNotNull(ssl = SSL_new(ctx));
    AssertNotNull(SSL_get_certificate(ssl));
    SSL_free(ssl);
    SSL_CTX_free(ctx);
#endif

    /* should be 2 other certs on stack */
    tmp = sk_X509_pop(ca);
    AssertNotNull(tmp);
    X509_free(tmp);
    tmp = sk_X509_pop(ca);
    AssertNotNull(tmp);
    X509_free(tmp);
    AssertNull(sk_X509_pop(ca));

    EVP_PKEY_free(pkey);
    X509_free(cert);
    sk_X509_pop_free(ca, X509_free);

    /* check PKCS12_create */
    AssertNull(PKCS12_create(pass, NULL, NULL, NULL, NULL, -1, -1, -1, -1,0));
    AssertIntEQ(PKCS12_parse(pkcs12, "wolfSSL test", &pkey, &cert, &ca),
            SSL_SUCCESS);
    AssertNotNull((pkcs12_2 = PKCS12_create(pass, NULL, pkey, cert, ca,
                    -1, -1, 100, -1, 0)));
    EVP_PKEY_free(pkey);
    X509_free(cert);
    sk_X509_free(ca);

    AssertIntEQ(PKCS12_parse(pkcs12_2, "a password", &pkey, &cert, &ca),
            SSL_SUCCESS);
    PKCS12_free(pkcs12_2);
    AssertNotNull((pkcs12_2 = PKCS12_create(pass, NULL, pkey, cert, ca,
             NID_pbe_WithSHA1And3_Key_TripleDES_CBC,
             NID_pbe_WithSHA1And3_Key_TripleDES_CBC,
             2000, 1, 0)));
    EVP_PKEY_free(pkey);
    X509_free(cert);
    sk_X509_free(ca);

    AssertIntEQ(PKCS12_parse(pkcs12_2, "a password", &pkey, &cert, &ca),
            SSL_SUCCESS);

    /* should be 2 other certs on stack */
    tmp = sk_X509_pop(ca);
    AssertNotNull(tmp);
    X509_free(tmp);
    tmp = sk_X509_pop(ca);
    AssertNotNull(tmp);
    X509_free(tmp);
    AssertNull(sk_X509_pop(ca));


#ifndef NO_RC4
    PKCS12_free(pkcs12_2);
    AssertNotNull((pkcs12_2 = PKCS12_create(pass, NULL, pkey, cert, NULL,
             NID_pbe_WithSHA1And128BitRC4,
             NID_pbe_WithSHA1And128BitRC4,
             2000, 1, 0)));
    EVP_PKEY_free(pkey);
    X509_free(cert);
    sk_X509_free(ca);

    AssertIntEQ(PKCS12_parse(pkcs12_2, "a password", &pkey, &cert, &ca),
            SSL_SUCCESS);

#endif /* NO_RC4 */

    EVP_PKEY_free(pkey);
    X509_free(cert);
    BIO_free(bio);
    PKCS12_free(pkcs12);
    PKCS12_free(pkcs12_2);
    sk_X509_free(ca);

#ifdef HAVE_ECC
    /* test order of parsing */
    f = XFOPEN(order, "rb");
    AssertTrue(f != XBADFILE);
    bytes = (int)XFREAD(buffer, 1, sizeof(buffer), f);
    XFCLOSE(f);

    AssertNotNull(bio = BIO_new_mem_buf((void*)buffer, bytes));
    AssertNotNull(pkcs12 = d2i_PKCS12_bio(bio, NULL));
    AssertIntEQ((ret = PKCS12_parse(pkcs12, "", &pkey, &cert, &ca)),
            WOLFSSL_SUCCESS);
    AssertNotNull(pkey);
    AssertNotNull(cert);
    AssertNotNull(ca);

    /* compare subject lines of certificates */
    AssertNotNull(subject = wolfSSL_X509_get_subject_name(cert));
    AssertNotNull(x509 = wolfSSL_X509_load_certificate_file(eccRsaCertFile,
                SSL_FILETYPE_PEM));
    AssertIntEQ(wolfSSL_X509_NAME_cmp((const WOLFSSL_X509_NAME*)subject,
            (const WOLFSSL_X509_NAME*)wolfSSL_X509_get_subject_name(x509)), 0);
    X509_free(x509);

    /* test expected fail case */
    AssertNotNull(x509 = wolfSSL_X509_load_certificate_file(eccCertFile,
                SSL_FILETYPE_PEM));
    AssertIntNE(wolfSSL_X509_NAME_cmp((const WOLFSSL_X509_NAME*)subject,
            (const WOLFSSL_X509_NAME*)wolfSSL_X509_get_subject_name(x509)), 0);
    X509_free(x509);
    X509_free(cert);

    /* get subject line from ca stack */
    AssertNotNull(cert = sk_X509_pop(ca));
    AssertNotNull(subject = wolfSSL_X509_get_subject_name(cert));

    /* compare subject from certificate in ca to expected */
    AssertNotNull(x509 = wolfSSL_X509_load_certificate_file(eccCertFile,
                SSL_FILETYPE_PEM));
    AssertIntEQ(wolfSSL_X509_NAME_cmp((const WOLFSSL_X509_NAME*)subject,
            (const WOLFSSL_X509_NAME*)wolfSSL_X509_get_subject_name(x509)), 0);

    EVP_PKEY_free(pkey);
    X509_free(x509);
    X509_free(cert);
    BIO_free(bio);
    PKCS12_free(pkcs12);
    sk_X509_free(ca); /* TEST d2i_PKCS12_fp */

    /* test order of parsing */
    f = XFOPEN(file, "rb");
    AssertTrue(f != XBADFILE);
    AssertNotNull(pkcs12 = d2i_PKCS12_fp(f, NULL));
    XFCLOSE(f);

    /* check verify MAC fail case */
    ret = PKCS12_parse(pkcs12, "bad", &pkey, &cert, NULL);
    AssertIntEQ(ret, 0);
    AssertNull(pkey);
    AssertNull(cert);

    /* check parse with no extra certs kept */
    ret = PKCS12_parse(pkcs12, "wolfSSL test", &pkey, &cert, NULL);
    AssertIntEQ(ret, 1);
    AssertNotNull(pkey);
    AssertNotNull(cert);

    wolfSSL_EVP_PKEY_free(pkey);
    wolfSSL_X509_free(cert);

    /* check parse with extra certs kept */
    ret = PKCS12_parse(pkcs12, "wolfSSL test", &pkey, &cert, &ca);
    AssertIntEQ(ret, 1);
    AssertNotNull(pkey);
    AssertNotNull(cert);
    AssertNotNull(ca);

    wolfSSL_EVP_PKEY_free(pkey);
    wolfSSL_X509_free(cert);
    sk_X509_free(ca);

    PKCS12_free(pkcs12);
#endif /* HAVE_ECC */

    /* Test i2d_PKCS12_bio */
    f = XFOPEN(file, "rb");
    AssertTrue((f != XBADFILE));
    AssertNotNull(pkcs12 = d2i_PKCS12_fp(f, NULL));
    XFCLOSE(f);

    bio = BIO_new(BIO_s_mem());
    AssertNotNull(bio);

    ret = i2d_PKCS12_bio(bio, pkcs12);
    AssertIntEQ(ret, 1);

    ret = i2d_PKCS12_bio(NULL, pkcs12);
    AssertIntEQ(ret, 0);

    ret = i2d_PKCS12_bio(bio, NULL);
    AssertIntEQ(ret, 0);

    PKCS12_free(pkcs12);
    BIO_free(bio);

    (void)x509;
    (void)subject;
    (void)order;

    printf(resultFmt, passed);
#endif /* OPENSSL_EXTRA */
#endif /* HAVE_FIPS */
}


#if !defined(NO_FILESYSTEM) && !defined(NO_ASN) && defined(HAVE_PKCS8) && \
    defined(WOLFSSL_ENCRYPTED_KEYS) && !defined(NO_DES3) && !defined(NO_PWDBASED) && \
    (!defined(NO_RSA) || defined(HAVE_ECC)) && !defined(NO_MD5)
    #define TEST_PKCS8_ENC
#endif

#if !defined(NO_FILESYSTEM) && !defined(NO_ASN) && defined(HAVE_PKCS8) \
    && defined(HAVE_ECC) && defined(WOLFSSL_ENCRYPTED_KEYS)
static WC_INLINE int FailTestCallBack(char* passwd, int sz, int rw, void* userdata)
{
    (void)passwd;
    (void)sz;
    (void)rw;
    (void)userdata;
    Fail(("Password callback should not be called by default"),
            ("Password callback was called without attempting "
             "to first decipher private key without password."));
    return 0;
}
#endif

static void test_wolfSSL_no_password_cb(void)
{
#if !defined(NO_FILESYSTEM) && !defined(NO_ASN) && defined(HAVE_PKCS8) \
    && defined(HAVE_ECC) && defined(WOLFSSL_ENCRYPTED_KEYS)
    WOLFSSL_CTX* ctx;
    byte buffer[FOURK_BUF];
    const char eccPkcs8PrivKeyDerFile[] = "./certs/ecc-privkeyPkcs8.der";
    const char eccPkcs8PrivKeyPemFile[] = "./certs/ecc-privkeyPkcs8.pem";
    XFILE f;
    int bytes;

    printf(testingFmt, "test_wolfSSL_no_password_cb()");

#ifndef NO_WOLFSSL_CLIENT
    AssertNotNull(ctx = wolfSSL_CTX_new(wolfTLS_client_method()));
#else
    AssertNotNull(ctx = wolfSSL_CTX_new(wolfTLS_server_method()));
#endif
    wolfSSL_CTX_set_default_passwd_cb(ctx, FailTestCallBack);

    AssertTrue((f = XFOPEN(eccPkcs8PrivKeyDerFile, "rb")) != XBADFILE);
    bytes = (int)XFREAD(buffer, 1, sizeof(buffer), f);
    XFCLOSE(f);
    AssertIntLE(bytes, sizeof(buffer));
    AssertIntEQ(wolfSSL_CTX_use_PrivateKey_buffer(ctx, buffer, bytes,
                WOLFSSL_FILETYPE_ASN1), WOLFSSL_SUCCESS);

    AssertTrue((f = XFOPEN(eccPkcs8PrivKeyPemFile, "rb")) != XBADFILE);
    bytes = (int)XFREAD(buffer, 1, sizeof(buffer), f);
    XFCLOSE(f);
    AssertIntLE(bytes, sizeof(buffer));
    AssertIntEQ(wolfSSL_CTX_use_PrivateKey_buffer(ctx, buffer, bytes,
                WOLFSSL_FILETYPE_PEM), WOLFSSL_SUCCESS);

    wolfSSL_CTX_free(ctx);

    printf(resultFmt, passed);
#endif
}

#ifdef TEST_PKCS8_ENC
/* for PKCS8 test case */
static WC_INLINE int PKCS8TestCallBack(char* passwd, int sz, int rw, void* userdata)
{
    int flag = 0;

    (void)rw;
    if (userdata != NULL) {
        flag = *((int*)userdata); /* user set data */
    }

    switch (flag) {
        case 1: /* flag set for specific WOLFSSL_CTX structure, note userdata
                 * can be anything the user wishes to be passed to the callback
                 * associated with the WOLFSSL_CTX */
            XSTRNCPY(passwd, "yassl123", sz);
            return 8;

        default:
            return BAD_FUNC_ARG;
    }
}
#endif /* TEST_PKCS8_ENC */

/* Testing functions dealing with PKCS8 */
static void test_wolfSSL_PKCS8(void)
{
#if !defined(NO_FILESYSTEM) && !defined(NO_ASN) && defined(HAVE_PKCS8)
    byte buffer[FOURK_BUF];
    byte der[FOURK_BUF];
    #ifndef NO_RSA
        const char serverKeyPkcs8PemFile[] = "./certs/server-keyPkcs8.pem";
        const char serverKeyPkcs8DerFile[] = "./certs/server-keyPkcs8.der";
    #endif
    const char eccPkcs8PrivKeyPemFile[] = "./certs/ecc-privkeyPkcs8.pem";
    #ifdef HAVE_ECC
        const char eccPkcs8PrivKeyDerFile[] = "./certs/ecc-privkeyPkcs8.der";
    #endif
    XFILE f;
    int bytes;
    WOLFSSL_CTX* ctx;
#ifdef HAVE_ECC
    int ret;
    ecc_key key;
    word32 x = 0;
#endif
#ifdef TEST_PKCS8_ENC
    #if !defined(NO_RSA) && !defined(NO_SHA)
        const char serverKeyPkcs8EncPemFile[] = "./certs/server-keyPkcs8Enc.pem";
        const char serverKeyPkcs8EncDerFile[] = "./certs/server-keyPkcs8Enc.der";
    #endif
    #if defined(HAVE_ECC) && !defined(NO_SHA)
        const char eccPkcs8EncPrivKeyPemFile[] = "./certs/ecc-keyPkcs8Enc.pem";
        const char eccPkcs8EncPrivKeyDerFile[] = "./certs/ecc-keyPkcs8Enc.der";
    #endif
    int flag;
#endif

    printf(testingFmt, "wolfSSL_PKCS8()");

#ifndef NO_WOLFSSL_CLIENT
    #ifndef WOLFSSL_NO_TLS12
        AssertNotNull(ctx = wolfSSL_CTX_new(wolfTLSv1_2_client_method()));
    #else
        AssertNotNull(ctx = wolfSSL_CTX_new(wolfTLSv1_3_client_method()));
    #endif
#else
    #ifndef WOLFSSL_NO_TLS12
        AssertNotNull(ctx = wolfSSL_CTX_new(wolfTLSv1_2_server_method()));
    #else
        AssertNotNull(ctx = wolfSSL_CTX_new(wolfTLSv1_3_server_method()));
    #endif
#endif

#ifdef TEST_PKCS8_ENC
    wolfSSL_CTX_set_default_passwd_cb(ctx, PKCS8TestCallBack);
    wolfSSL_CTX_set_default_passwd_cb_userdata(ctx, (void*)&flag);
    flag = 1; /* used by password callback as return code */

    #if !defined(NO_RSA) && !defined(NO_SHA)
    /* test loading PEM PKCS8 encrypted file */
    f = XFOPEN(serverKeyPkcs8EncPemFile, "rb");
    AssertTrue((f != XBADFILE));
    bytes = (int)XFREAD(buffer, 1, sizeof(buffer), f);
    XFCLOSE(f);
    AssertIntEQ(wolfSSL_CTX_use_PrivateKey_buffer(ctx, buffer, bytes,
                WOLFSSL_FILETYPE_PEM), WOLFSSL_SUCCESS);

    /* this next case should fail because of password callback return code */
    flag = 0; /* used by password callback as return code */
    AssertIntNE(wolfSSL_CTX_use_PrivateKey_buffer(ctx, buffer, bytes,
                WOLFSSL_FILETYPE_PEM), WOLFSSL_SUCCESS);

    /* decrypt PKCS8 PEM to key in DER format with not using WOLFSSL_CTX */
    AssertIntGT(wc_KeyPemToDer(buffer, bytes, der, (word32)sizeof(der),
        "yassl123"), 0);

    /* test that error value is returned with a bad password */
    AssertIntLT(wc_KeyPemToDer(buffer, bytes, der, (word32)sizeof(der),
        "bad"), 0);

    /* test loading PEM PKCS8 encrypted file */
    f = XFOPEN(serverKeyPkcs8EncDerFile, "rb");
    AssertTrue((f != XBADFILE));
    bytes = (int)XFREAD(buffer, 1, sizeof(buffer), f);
    XFCLOSE(f);
    flag = 1; /* used by password callback as return code */
    AssertIntEQ(wolfSSL_CTX_use_PrivateKey_buffer(ctx, buffer, bytes,
                WOLFSSL_FILETYPE_ASN1), WOLFSSL_SUCCESS);

    /* this next case should fail because of password callback return code */
    flag = 0; /* used by password callback as return code */
    AssertIntNE(wolfSSL_CTX_use_PrivateKey_buffer(ctx, buffer, bytes,
                WOLFSSL_FILETYPE_ASN1), WOLFSSL_SUCCESS);
    #endif /* !NO_RSA */

    #if defined(HAVE_ECC) && !defined(NO_SHA)
    /* test loading PEM PKCS8 encrypted ECC Key file */
    f = XFOPEN(eccPkcs8EncPrivKeyPemFile, "rb");
    AssertTrue((f != XBADFILE));
    bytes = (int)XFREAD(buffer, 1, sizeof(buffer), f);
    XFCLOSE(f);
    flag = 1; /* used by password callback as return code */
    AssertIntEQ(wolfSSL_CTX_use_PrivateKey_buffer(ctx, buffer, bytes,
                WOLFSSL_FILETYPE_PEM), WOLFSSL_SUCCESS);

    /* this next case should fail because of password callback return code */
    flag = 0; /* used by password callback as return code */
    AssertIntNE(wolfSSL_CTX_use_PrivateKey_buffer(ctx, buffer, bytes,
                WOLFSSL_FILETYPE_PEM), WOLFSSL_SUCCESS);

    /* decrypt PKCS8 PEM to key in DER format with not using WOLFSSL_CTX */
    AssertIntGT(wc_KeyPemToDer(buffer, bytes, der, (word32)sizeof(der),
        "yassl123"), 0);

    /* test that error value is returned with a bad password */
    AssertIntLT(wc_KeyPemToDer(buffer, bytes, der, (word32)sizeof(der),
        "bad"), 0);

    /* test loading DER PKCS8 encrypted ECC Key file */
    f = XFOPEN(eccPkcs8EncPrivKeyDerFile, "rb");
    AssertTrue((f != XBADFILE));
    bytes = (int)XFREAD(buffer, 1, sizeof(buffer), f);
    XFCLOSE(f);
    flag = 1; /* used by password callback as return code */
    AssertIntEQ(wolfSSL_CTX_use_PrivateKey_buffer(ctx, buffer, bytes,
                WOLFSSL_FILETYPE_ASN1), WOLFSSL_SUCCESS);

    /* this next case should fail because of password callback return code */
    flag = 0; /* used by password callback as return code */
    AssertIntNE(wolfSSL_CTX_use_PrivateKey_buffer(ctx, buffer, bytes,
                WOLFSSL_FILETYPE_ASN1), WOLFSSL_SUCCESS);

    /* leave flag as "okay" */
    flag = 1;
    #endif /* HAVE_ECC */
#endif /* TEST_PKCS8_ENC */


#ifndef NO_RSA
    /* test loading ASN.1 (DER) PKCS8 private key file (not encrypted) */
    f = XFOPEN(serverKeyPkcs8DerFile, "rb");
    AssertTrue((f != XBADFILE));
    bytes = (int)XFREAD(buffer, 1, sizeof(buffer), f);
    XFCLOSE(f);
    AssertIntEQ(wolfSSL_CTX_use_PrivateKey_buffer(ctx, buffer, bytes,
                WOLFSSL_FILETYPE_ASN1), WOLFSSL_SUCCESS);

    /* test loading PEM PKCS8 private key file (not encrypted) */
    f = XFOPEN(serverKeyPkcs8PemFile, "rb");
    AssertTrue((f != XBADFILE));
    bytes = (int)XFREAD(buffer, 1, sizeof(buffer), f);
    XFCLOSE(f);
    AssertIntEQ(wolfSSL_CTX_use_PrivateKey_buffer(ctx, buffer, bytes,
                WOLFSSL_FILETYPE_PEM), WOLFSSL_SUCCESS);
#endif /* !NO_RSA */

    /* Test PKCS8 PEM ECC key no crypt */
    f = XFOPEN(eccPkcs8PrivKeyPemFile, "rb");
    AssertTrue((f != XBADFILE));
    bytes = (int)XFREAD(buffer, 1, sizeof(buffer), f);
    XFCLOSE(f);
#ifdef HAVE_ECC
    /* Test PKCS8 PEM ECC key no crypt */
    AssertIntEQ(wolfSSL_CTX_use_PrivateKey_buffer(ctx, buffer, bytes,
                WOLFSSL_FILETYPE_PEM), WOLFSSL_SUCCESS);

    /* decrypt PKCS8 PEM to key in DER format */
    AssertIntGT((bytes = wc_KeyPemToDer(buffer, bytes, der,
        (word32)sizeof(der), NULL)), 0);
    ret = wc_ecc_init(&key);
    if (ret == 0) {
        ret = wc_EccPrivateKeyDecode(der, &x, &key, bytes);
        wc_ecc_free(&key);
    }
    AssertIntEQ(ret, 0);

    /* Test PKCS8 DER ECC key no crypt */
    f = XFOPEN(eccPkcs8PrivKeyDerFile, "rb");
    AssertTrue((f != XBADFILE));
    bytes = (int)XFREAD(buffer, 1, sizeof(buffer), f);
    XFCLOSE(f);

    /* Test using a PKCS8 ECC PEM */
    AssertIntEQ(wolfSSL_CTX_use_PrivateKey_buffer(ctx, buffer, bytes,
                WOLFSSL_FILETYPE_ASN1), WOLFSSL_SUCCESS);
#else
    AssertIntEQ((bytes = wc_KeyPemToDer(buffer, bytes, der,
        (word32)sizeof(der), NULL)), ASN_NO_PEM_HEADER);
#endif /* HAVE_ECC */

    wolfSSL_CTX_free(ctx);

    printf(resultFmt, passed);
#endif /* !NO_FILESYSTEM && !NO_ASN && HAVE_PKCS8 */
}

/* Testing functions dealing with PKCS5 */
static void test_wolfSSL_PKCS5(void)
{
#if defined(OPENSSL_EXTRA) && !defined(NO_SHA) && !defined(NO_PWDBASED)
#ifdef HAVE_FIPS /* Password minimum length is 14 (112-bit) in FIPS MODE */
    const char* passwd = "myfipsPa$$W0rd";
#else
    const char *passwd = "pass1234";
#endif
    const unsigned char *salt = (unsigned char *)"salt1234";
    unsigned char *out = (unsigned char *)XMALLOC(WC_SHA_DIGEST_SIZE, NULL,
                                                  DYNAMIC_TYPE_TMP_BUFFER);
    int ret = 0;

    AssertNotNull(out);
    ret = PKCS5_PBKDF2_HMAC_SHA1(passwd,(int)XSTRLEN(passwd), salt,
                                 (int)XSTRLEN((const char *) salt), 10,
                                 WC_SHA_DIGEST_SIZE,out);
    AssertIntEQ(ret, SSL_SUCCESS);

#ifdef WOLFSSL_SHA512
    ret = PKCS5_PBKDF2_HMAC(passwd,(int)XSTRLEN(passwd), salt,
                             (int)XSTRLEN((const char *) salt), 10,
                             wolfSSL_EVP_sha512(), WC_SHA_DIGEST_SIZE, out);
    AssertIntEQ(ret, SSL_SUCCESS);
#endif

    XFREE(out, NULL, DYNAMIC_TYPE_TMP_BUFFER);
#endif /* defined(OPENSSL_EXTRA) && !defined(NO_SHA) */
}

/* test parsing URI from certificate */
static void test_wolfSSL_URI(void)
{
#if !defined(NO_CERTS) && !defined(NO_RSA) && !defined(NO_FILESYSTEM) \
    && (defined(KEEP_PEER_CERT) || defined(SESSION_CERTS) || \
    defined(OPENSSL_EXTRA))
    WOLFSSL_X509* x509;
    const char uri[] = "./certs/client-uri-cert.pem";
    const char badUri[] = "./certs/client-relative-uri.pem";

    printf(testingFmt, "wolfSSL URI parse");

    x509 = wolfSSL_X509_load_certificate_file(uri, WOLFSSL_FILETYPE_PEM);
    AssertNotNull(x509);

    wolfSSL_FreeX509(x509);

    x509 = wolfSSL_X509_load_certificate_file(badUri, WOLFSSL_FILETYPE_PEM);
#ifndef IGNORE_NAME_CONSTRAINTS
    AssertNull(x509);
#else
    AssertNotNull(x509);
#endif

    printf(resultFmt, passed);
#endif
}


static void test_wolfSSL_TBS(void)
{
#if !defined(NO_CERTS) && !defined(NO_RSA) && !defined(NO_FILESYSTEM) \
    && defined(OPENSSL_EXTRA)
    WOLFSSL_X509* x509;
    const unsigned char* tbs;
    int tbsSz;

    printf(testingFmt, "wolfSSL TBS");

    AssertNotNull(x509 =
          wolfSSL_X509_load_certificate_file(caCertFile, WOLFSSL_FILETYPE_PEM));

    AssertNull(tbs = wolfSSL_X509_get_tbs(NULL, &tbsSz));
    AssertNull(tbs = wolfSSL_X509_get_tbs(x509, NULL));
    AssertNotNull(tbs = wolfSSL_X509_get_tbs(x509, &tbsSz));
    AssertIntEQ(tbsSz, 918);

    wolfSSL_FreeX509(x509);

    printf(resultFmt, passed);
#endif
}

static void test_wolfSSL_X509_verify(void)
{
#if !defined(NO_CERTS) && !defined(NO_RSA) && !defined(NO_FILESYSTEM) \
    && defined(OPENSSL_EXTRA)
    WOLFSSL_X509* ca;
    WOLFSSL_X509* server;
    WOLFSSL_EVP_PKEY* pkey;
    unsigned char buf[2048];
    unsigned char* pt;
    int bufSz;

    printf(testingFmt, "wolfSSL X509 verify");

    AssertNotNull(ca =
          wolfSSL_X509_load_certificate_file(caCertFile, WOLFSSL_FILETYPE_PEM));

    AssertIntNE(wolfSSL_X509_get_pubkey_buffer(NULL, buf, &bufSz),
            WOLFSSL_SUCCESS);
    AssertIntEQ(wolfSSL_X509_get_pubkey_buffer(ca, NULL, &bufSz),
            WOLFSSL_SUCCESS);
    AssertIntEQ(bufSz, 294);

    bufSz = 2048;
    AssertIntEQ(wolfSSL_X509_get_pubkey_buffer(ca, buf, &bufSz),
            WOLFSSL_SUCCESS);
    AssertIntEQ(wolfSSL_X509_get_pubkey_type(NULL), WOLFSSL_FAILURE);
    AssertIntEQ(wolfSSL_X509_get_pubkey_type(ca), RSAk);


    AssertNotNull(server =
          wolfSSL_X509_load_certificate_file(svrCertFile, WOLFSSL_FILETYPE_PEM));

    /* success case */
    pt = buf;
    AssertNotNull(pkey = wolfSSL_d2i_PUBKEY(NULL, &pt, bufSz));
    AssertIntEQ(wolfSSL_X509_verify(server, pkey), WOLFSSL_SUCCESS);
    wolfSSL_EVP_PKEY_free(pkey);

    /* fail case */
    bufSz = 2048;
    AssertIntEQ(wolfSSL_X509_get_pubkey_buffer(server, buf, &bufSz),
            WOLFSSL_SUCCESS);
    pt = buf;
    AssertNotNull(pkey = wolfSSL_d2i_PUBKEY(NULL, &pt, bufSz));
    AssertIntEQ(wolfSSL_X509_verify(server, pkey), WOLFSSL_FAILURE);

    AssertIntEQ(wolfSSL_X509_verify(NULL, pkey), WOLFSSL_FATAL_ERROR);
    AssertIntEQ(wolfSSL_X509_verify(server, NULL), WOLFSSL_FATAL_ERROR);
    wolfSSL_EVP_PKEY_free(pkey);

    wolfSSL_FreeX509(ca);
    wolfSSL_FreeX509(server);

    printf(resultFmt, passed);
#endif
}
/* Testing function  wolfSSL_CTX_SetMinVersion; sets the minimum downgrade
 * version allowed.
 * POST: 1 on success.
 */
static int test_wolfSSL_CTX_SetMinVersion(void)
{
    int                     failFlag = WOLFSSL_SUCCESS;
#ifndef NO_WOLFSSL_CLIENT
    WOLFSSL_CTX*            ctx;
    int                     itr;

    #ifndef NO_OLD_TLS
        const int versions[]  = {
                            #ifdef WOLFSSL_ALLOW_TLSV10
                                  WOLFSSL_TLSV1,
                            #endif
                                  WOLFSSL_TLSV1_1,
                                  WOLFSSL_TLSV1_2 };
    #elif !defined(WOLFSSL_NO_TLS12)
        const int versions[]  = { WOLFSSL_TLSV1_2 };
    #elif defined(WOLFSSL_TLS13)
        const int versions[]  = { WOLFSSL_TLSV1_3 };
    #else
        const int versions[0];
    #endif

    failFlag = WOLFSSL_SUCCESS;

    ctx = wolfSSL_CTX_new(wolfSSLv23_client_method());

    printf(testingFmt, "wolfSSL_CTX_SetMinVersion()");

    for (itr = 0; itr < (int)(sizeof(versions)/sizeof(int)); itr++){
        if(wolfSSL_CTX_SetMinVersion(ctx, *(versions + itr)) != WOLFSSL_SUCCESS){
            failFlag = WOLFSSL_FAILURE;
        }
    }

    printf(resultFmt, failFlag == WOLFSSL_SUCCESS ? passed : failed);

    wolfSSL_CTX_free(ctx);
#endif
    return failFlag;

} /* END test_wolfSSL_CTX_SetMinVersion */


/*----------------------------------------------------------------------------*
 | OCSP Stapling
 *----------------------------------------------------------------------------*/


/* Testing wolfSSL_UseOCSPStapling function. OCSP stapling eliminates the need
 * need to contact the CA, lowering the cost of cert revocation checking.
 * PRE: HAVE_OCSP and HAVE_CERTIFICATE_STATUS_REQUEST
 * POST: 1 returned for success.
 */
static int test_wolfSSL_UseOCSPStapling(void)
{
    #if defined(HAVE_CERTIFICATE_STATUS_REQUEST) && defined(HAVE_OCSP) && \
            !defined(NO_WOLFSSL_CLIENT)
        int             ret;
        WOLFSSL_CTX*    ctx;
        WOLFSSL*        ssl;

#ifndef NO_WOLFSSL_CLIENT
    #ifndef WOLFSSL_NO_TLS12
        ctx = wolfSSL_CTX_new(wolfTLSv1_2_client_method());
    #else
        ctx = wolfSSL_CTX_new(wolfTLSv1_3_client_method());
    #endif
#else
    #ifndef WOLFSSL_NO_TLS12
        ctx = wolfSSL_CTX_new(wolfTLSv1_2_server_method());
    #else
        ctx = wolfSSL_CTX_new(wolfTLSv1_3_server_method());
    #endif
#endif
        ssl = wolfSSL_new(ctx);
        printf(testingFmt, "wolfSSL_UseOCSPStapling()");

        ret = wolfSSL_UseOCSPStapling(ssl, WOLFSSL_CSR2_OCSP,
                                    WOLFSSL_CSR2_OCSP_USE_NONCE);

        printf(resultFmt, ret == WOLFSSL_SUCCESS ? passed : failed);


        wolfSSL_free(ssl);
        wolfSSL_CTX_free(ctx);

        return ret;
    #else
        return WOLFSSL_SUCCESS;
    #endif

} /*END test_wolfSSL_UseOCSPStapling */


/* Testing OCSP stapling version 2, wolfSSL_UseOCSPStaplingV2 funciton. OCSP
 * stapling eliminates the need ot contact the CA and lowers cert revocation
 * check.
 * PRE: HAVE_CERTIFICATE_STATUS_REQUEST_V2 and HAVE_OCSP defined.
 */
static int test_wolfSSL_UseOCSPStaplingV2 (void)
{
    #if defined(HAVE_CERTIFICATE_STATUS_REQUEST_V2) && defined(HAVE_OCSP) && \
            !defined(NO_WOLFSSL_CLIENT)
        int                 ret;
        WOLFSSL_CTX*        ctx;
        WOLFSSL*            ssl;

#ifndef NO_WOLFSSL_CLIENT
    #ifndef WOLFSSL_NO_TLS12
        ctx = wolfSSL_CTX_new(wolfTLSv1_2_client_method());
    #else
        ctx = wolfSSL_CTX_new(wolfTLSv1_3_client_method());
    #endif
#else
    #ifndef WOLFSSL_NO_TLS12
        ctx = wolfSSL_CTX_new(wolfTLSv1_2_server_method());
    #else
        ctx = wolfSSL_CTX_new(wolfTLSv1_3_server_method());
    #endif
#endif
        ssl = wolfSSL_new(ctx);
        printf(testingFmt, "wolfSSL_UseOCSPStaplingV2()");

        ret = wolfSSL_UseOCSPStaplingV2(ssl, WOLFSSL_CSR2_OCSP,
                                        WOLFSSL_CSR2_OCSP_USE_NONCE );

        printf(resultFmt, ret == WOLFSSL_SUCCESS ? passed : failed);

        wolfSSL_free(ssl);
        wolfSSL_CTX_free(ctx);

        return ret;
    #else
        return WOLFSSL_SUCCESS;
    #endif

} /*END test_wolfSSL_UseOCSPStaplingV2*/

/*----------------------------------------------------------------------------*
 | Multicast Tests
 *----------------------------------------------------------------------------*/
static void test_wolfSSL_mcast(void)
{
#if defined(WOLFSSL_DTLS) && defined(WOLFSSL_MULTICAST)
    WOLFSSL_CTX* ctx;
    WOLFSSL* ssl;
    int result;
    byte preMasterSecret[512];
    byte clientRandom[32];
    byte serverRandom[32];
    byte suite[2] = {0, 0xfe};  /* WDM_WITH_NULL_SHA256 */
    byte buf[256];
    word16 newId;

    ctx = wolfSSL_CTX_new(wolfDTLSv1_2_client_method());
    AssertNotNull(ctx);

    result = wolfSSL_CTX_mcast_set_member_id(ctx, 0);
    AssertIntEQ(result, WOLFSSL_SUCCESS);

    ssl = wolfSSL_new(ctx);
    AssertNotNull(ssl);

    XMEMSET(preMasterSecret, 0x23, sizeof(preMasterSecret));
    XMEMSET(clientRandom, 0xA5, sizeof(clientRandom));
    XMEMSET(serverRandom, 0x5A, sizeof(serverRandom));
    result = wolfSSL_set_secret(ssl, 23,
                preMasterSecret, sizeof(preMasterSecret),
                clientRandom, serverRandom, suite);
    AssertIntEQ(result, WOLFSSL_SUCCESS);

    result = wolfSSL_mcast_read(ssl, &newId, buf, sizeof(buf));
    AssertIntLE(result, 0);
    AssertIntLE(newId, 100);

    wolfSSL_free(ssl);
    wolfSSL_CTX_free(ctx);
#endif /* WOLFSSL_DTLS && WOLFSSL_MULTICAST */
}


/*----------------------------------------------------------------------------*
 |  Wolfcrypt
 *----------------------------------------------------------------------------*/

/*
 * Unit test for the wc_InitBlake2b()
 */
static int test_wc_InitBlake2b (void)
{
    int ret = 0;
#ifdef HAVE_BLAKE2

    Blake2b blake2;

    printf(testingFmt, "wc_InitBlake2B()");

    /* Test good arg. */
    ret = wc_InitBlake2b(&blake2, 64);
    if (ret != 0) {
        ret = WOLFSSL_FATAL_ERROR;
    }

    /* Test bad arg. */
    if (!ret) {
        ret = wc_InitBlake2b(NULL, 64);
        if (ret == 0) {
            ret = WOLFSSL_FATAL_ERROR;
        } else {
            ret = 0;
        }
    }

    if (!ret) {
        ret = wc_InitBlake2b(NULL, 128);
        if (ret == 0) {
            ret = WOLFSSL_FATAL_ERROR;
        } else {
            ret = 0;
        }
    }

    if (!ret) {
        ret = wc_InitBlake2b(&blake2, 128);
        if (ret == 0) {
            ret = WOLFSSL_FATAL_ERROR;
        } else {
            ret = 0;
        }
    }

    if (!ret) {
        ret = wc_InitBlake2b(NULL, 0);
        if (ret == 0) {
            ret = WOLFSSL_FATAL_ERROR;
        } else {
            ret = 0;
        }
    }

    if (!ret) {
        ret = wc_InitBlake2b(&blake2, 0);
        if (ret == 0) {
            ret = WOLFSSL_FATAL_ERROR;
        } else {
            ret = 0;
        }
    }

    printf(resultFmt, ret == 0 ? passed : failed);

#endif
    return ret;
}     /*END test_wc_InitBlake2b*/


/*
 * Unit test for the wc_InitMd5()
 */
static int test_wc_InitMd5 (void)
{
    int flag = 0;
#ifndef NO_MD5

    wc_Md5 md5;
    int ret;

    printf(testingFmt, "wc_InitMd5()");

    /* Test good arg. */
    ret = wc_InitMd5(&md5);
    if (ret != 0) {
        flag = WOLFSSL_FATAL_ERROR;
    }

    /* Test bad arg. */
    if (!flag) {
        ret = wc_InitMd5(NULL);
        if (ret != BAD_FUNC_ARG) {
            flag = WOLFSSL_FATAL_ERROR;
        }
    }

    wc_Md5Free(&md5);

    printf(resultFmt, flag == 0 ? passed : failed);

#endif
    return flag;
}     /* END test_wc_InitMd5 */


/*
 * Testing wc_UpdateMd5()
 */
static int test_wc_Md5Update (void)
{

    int flag = 0;
#ifndef NO_MD5
    wc_Md5 md5;
    byte hash[WC_MD5_DIGEST_SIZE];
    testVector a, b, c;
    int ret;

    ret = wc_InitMd5(&md5);
    if (ret != 0) {
        flag = ret;
    }

    printf(testingFmt, "wc_Md5Update()");

    /* Input */
    if (!flag) {
        a.input = "a";
        a.inLen = XSTRLEN(a.input);

        ret = wc_Md5Update(&md5, (byte*)a.input, (word32)a.inLen);
        if (ret != 0) {
            flag = ret;
        }
    }

    if (!flag) {
        ret = wc_Md5Final(&md5, hash);
        if (ret != 0) {
            flag = ret;
        }
    }

    /* Update input. */
    if (!flag) {
        a.input = "abc";
        a.output = "\x90\x01\x50\x98\x3c\xd2\x4f\xb0\xd6\x96\x3f\x7d\x28\xe1\x7f"
                    "\x72";
        a.inLen = XSTRLEN(a.input);
        a.outLen = XSTRLEN(a.output);

        ret = wc_Md5Update(&md5, (byte*) a.input, (word32) a.inLen);
        if (ret != 0) {
            flag = ret;
        }
    }

    if (!flag) {
        ret = wc_Md5Final(&md5, hash);
        if (ret != 0) {
            flag = ret;
        }
    }

    if (!flag) {
        if (XMEMCMP(hash, a.output, WC_MD5_DIGEST_SIZE) != 0) {
            flag = WOLFSSL_FATAL_ERROR;
        }
    }

    /*Pass in bad values. */
    if (!flag) {
        b.input = NULL;
        b.inLen = 0;

        ret = wc_Md5Update(&md5, (byte*)b.input, (word32)b.inLen);
        if (ret != 0) {
            flag = ret;
        }
    }

    if (!flag) {
        c.input = NULL;
        c.inLen = WC_MD5_DIGEST_SIZE;

        ret = wc_Md5Update(&md5, (byte*)c.input, (word32)c.inLen);
        if (ret != BAD_FUNC_ARG) {
            flag = WOLFSSL_FATAL_ERROR;
        }
    }

    if (!flag) {
        ret = wc_Md5Update(NULL, (byte*)a.input, (word32)a.inLen);
        if (ret != BAD_FUNC_ARG) {
            flag = WOLFSSL_FATAL_ERROR;
        }
    }

    wc_Md5Free(&md5);

    printf(resultFmt, flag == 0 ? passed : failed);

#endif
    return flag;
} /* END test_wc_Md5Update()  */


/*
 *  Unit test on wc_Md5Final() in wolfcrypt/src/md5.c
 */
static int test_wc_Md5Final (void)
{

    int flag = 0;
#ifndef NO_MD5
    /* Instantiate */
    wc_Md5 md5;
    byte* hash_test[3];
    byte hash1[WC_MD5_DIGEST_SIZE];
    byte hash2[2*WC_MD5_DIGEST_SIZE];
    byte hash3[5*WC_MD5_DIGEST_SIZE];
    int times, i, ret;

    /* Initialize */
    ret = wc_InitMd5(&md5);
    if (ret != 0)  {
        flag = ret;
    }

    if (!flag) {
        hash_test[0] = hash1;
        hash_test[1] = hash2;
        hash_test[2] = hash3;
    }

    times = sizeof(hash_test)/sizeof(byte*);

    /* Test good args. */
    printf(testingFmt, "wc_Md5Final()");

    for (i = 0; i < times; i++) {
        if (!flag) {
            ret = wc_Md5Final(&md5, hash_test[i]);
            if (ret != 0) {
                flag = WOLFSSL_FATAL_ERROR;
            }
        }
    }

    /* Test bad args. */
    if (!flag) {
        ret = wc_Md5Final(NULL, NULL);
        if (ret != BAD_FUNC_ARG) {
            flag = WOLFSSL_FATAL_ERROR;
        }
    }

    if (!flag) {
        ret = wc_Md5Final(NULL, hash1);
        if (ret != BAD_FUNC_ARG) {
            flag = WOLFSSL_FATAL_ERROR;
        }
    }

    if (!flag) {
        ret = wc_Md5Final(&md5, NULL);
        if (ret != BAD_FUNC_ARG) {
            flag = WOLFSSL_FATAL_ERROR;
        }
    }

    wc_Md5Free(&md5);

    printf(resultFmt, flag == 0 ? passed : failed);

#endif
    return flag;
}


/*
 * Unit test for the wc_InitSha()
 */
static int test_wc_InitSha(void)
{
    int flag = 0;
#ifndef NO_SHA
    wc_Sha sha;
    int ret;

    printf(testingFmt, "wc_InitSha()");

    /* Test good arg. */
    ret = wc_InitSha(&sha);
    if (ret != 0) {
        flag = WOLFSSL_FATAL_ERROR;
    }

    /* Test bad arg. */
    if (!flag) {
        ret = wc_InitSha(NULL);
        if (ret != BAD_FUNC_ARG) {
            flag = WOLFSSL_FATAL_ERROR;
        }
    }

    wc_ShaFree(&sha);

    printf(resultFmt, flag == 0 ? passed : failed);

#endif
    return flag;

} /* END test_wc_InitSha */

/*
 *  Tesing wc_ShaUpdate()
 */
static int test_wc_ShaUpdate (void)
{

    int flag = 0;
#ifndef NO_SHA
    wc_Sha sha;
    byte hash[WC_SHA_DIGEST_SIZE];
    testVector a, b, c;
    int ret;

    ret = wc_InitSha(&sha);
    if (ret != 0) {
        flag = ret;
    }

    printf(testingFmt, "wc_ShaUpdate()");

    /* Input. */
    if (!flag) {
        a.input = "a";
        a.inLen = XSTRLEN(a.input);

        ret = wc_ShaUpdate(&sha, (byte*)a.input, (word32)a.inLen);
        if (ret != 0) {
            flag = ret;
        }
    }

    if (!flag) {
        ret = wc_ShaFinal(&sha, hash);
        if (ret != 0) {
            flag = ret;
        }
    }

    /* Update input. */
    if (!flag) {
        a.input = "abc";
        a.output = "\xA9\x99\x3E\x36\x47\x06\x81\x6A\xBA\x3E\x25\x71\x78\x50\xC2"
                    "\x6C\x9C\xD0\xD8\x9D";
        a.inLen = XSTRLEN(a.input);
        a.outLen = XSTRLEN(a.output);

        ret = wc_ShaUpdate(&sha, (byte*)a.input, (word32)a.inLen);
        if (ret != 0) {
            flag = ret;
        }
    }

    if (!flag) {
        ret = wc_ShaFinal(&sha, hash);
        if (ret !=0) {
            flag = ret;
        }
    }

    if (!flag) {
        if (XMEMCMP(hash, a.output, WC_SHA_DIGEST_SIZE) != 0) {
            flag = WOLFSSL_FATAL_ERROR;
        }
    }

    /* Try passing in bad values. */
    if (!flag) {
        b.input = NULL;
        b.inLen = 0;

        ret = wc_ShaUpdate(&sha, (byte*)b.input, (word32)b.inLen);
        if (ret != 0) {
            flag = ret;
        }
    }

    if (!flag) {
        c.input = NULL;
        c.inLen = WC_SHA_DIGEST_SIZE;

        ret = wc_ShaUpdate(&sha, (byte*)c.input, (word32)c.inLen);
        if (ret != BAD_FUNC_ARG) {
            flag = WOLFSSL_FATAL_ERROR;
        }
    }

    if (!flag) {
        ret = wc_ShaUpdate(NULL, (byte*)a.input, (word32)a.inLen);
        if (ret != BAD_FUNC_ARG) {
            flag = WOLFSSL_FATAL_ERROR;
        }
    }

    wc_ShaFree(&sha);
    /* If not returned then the unit test passed test vectors. */
    printf(resultFmt, flag == 0 ? passed : failed);

#endif
    return flag;

} /* END test_wc_ShaUpdate() */


/*
 * Unit test on wc_ShaFinal
 */
static int test_wc_ShaFinal (void)
{
    int flag = 0;
#ifndef NO_SHA
    wc_Sha sha;
    byte* hash_test[3];
    byte hash1[WC_SHA_DIGEST_SIZE];
    byte hash2[2*WC_SHA_DIGEST_SIZE];
    byte hash3[5*WC_SHA_DIGEST_SIZE];
    int times, i, ret;

    /*Initialize*/
    ret = wc_InitSha(&sha);
    if (ret) {
        flag =  ret;
    }

    if (!flag) {
        hash_test[0] = hash1;
        hash_test[1] = hash2;
        hash_test[2] = hash3;
    }

    times = sizeof(hash_test)/sizeof(byte*);

    /* Good test args. */
    printf(testingFmt, "wc_ShaFinal()");

    for (i = 0; i < times; i++) {
        if (!flag) {
            ret = wc_ShaFinal(&sha, hash_test[i]);
            if (ret != 0) {
                flag = WOLFSSL_FATAL_ERROR;
            }
        }
    }

    /* Test bad args. */
    if (!flag) {
        ret = wc_ShaFinal(NULL, NULL);
        if (ret != BAD_FUNC_ARG) {
            flag = WOLFSSL_FATAL_ERROR;
        }
    }

    if (!flag) {
        ret = wc_ShaFinal(NULL, hash1);
        if (ret != BAD_FUNC_ARG) {
            flag = WOLFSSL_FATAL_ERROR;
        }
    }

    if (!flag) {
        ret = wc_ShaFinal(&sha, NULL);
        if (ret != BAD_FUNC_ARG) {
            flag = WOLFSSL_FATAL_ERROR;
        }
    }

    wc_ShaFree(&sha);

    printf(resultFmt, flag == 0 ? passed : failed);

#endif
    return flag;
} /* END test_wc_ShaFinal */


/*
 * Unit test for wc_InitSha256()
 */
static int test_wc_InitSha256 (void)
{
    int flag = 0;
#ifndef NO_SHA256
    wc_Sha256 sha256;
    int ret;

    printf(testingFmt, "wc_InitSha256()");

    /* Test good arg. */
    ret = wc_InitSha256(&sha256);
    if (ret != 0) {
        flag = WOLFSSL_FATAL_ERROR;
    }

    /* Test bad arg. */
    if (!flag) {
        ret = wc_InitSha256(NULL);
        if (ret != BAD_FUNC_ARG) {
            flag = WOLFSSL_FATAL_ERROR;
        }
    }

    wc_Sha256Free(&sha256);

    printf(resultFmt, flag == 0 ? passed : failed);

#endif
    return flag;
} /* END test_wc_InitSha256 */


/*
 * Unit test for wc_Sha256Update()
 */
static int test_wc_Sha256Update (void)
{
    int flag = 0;
#ifndef NO_SHA256
    wc_Sha256 sha256;
    byte hash[WC_SHA256_DIGEST_SIZE];
    testVector a, b, c;
    int ret;

    ret = wc_InitSha256(&sha256);
    if (ret != 0) {
        flag = ret;
    }

    printf(testingFmt, "wc_Sha256Update()");

    /*  Input. */
    if (!flag) {
        a.input = "a";
        a.inLen = XSTRLEN(a.input);

        ret = wc_Sha256Update(&sha256, (byte*)a.input, (word32)a.inLen);
        if (ret != 0) {
            flag = ret;
        }
    }

    if (!flag) {
        ret = wc_Sha256Final(&sha256, hash);
        if (ret != 0) {
            flag = ret;
        }
    }

    /* Update input. */
    if (!flag) {
        a.input = "abc";
        a.output = "\xBA\x78\x16\xBF\x8F\x01\xCF\xEA\x41\x41\x40\xDE\x5D\xAE\x22"
                    "\x23\xB0\x03\x61\xA3\x96\x17\x7A\x9C\xB4\x10\xFF\x61\xF2\x00"
                    "\x15\xAD";
        a.inLen = XSTRLEN(a.input);
        a.outLen = XSTRLEN(a.output);

        ret = wc_Sha256Update(&sha256, (byte*)a.input, (word32)a.inLen);
        if (ret != 0) {
            flag = ret;
        }
    }

    if (!flag) {
        ret = wc_Sha256Final(&sha256, hash);
        if (ret != 0) {
            flag = ret;
        }
    }

    if (!flag) {
        if (XMEMCMP(hash, a.output, WC_SHA256_DIGEST_SIZE) != 0) {
            flag = WOLFSSL_FATAL_ERROR;
        }
    }

    /* Try passing in bad values */
    if (!flag) {
        b.input = NULL;
        b.inLen = 0;

        ret = wc_Sha256Update(&sha256, (byte*)b.input, (word32)b.inLen);
        if (ret != 0) {
            flag = ret;
        }
    }

    if (!flag) {
        c.input = NULL;
        c.inLen = WC_SHA256_DIGEST_SIZE;

        ret = wc_Sha256Update(&sha256, (byte*)c.input, (word32)c.inLen);
        if (ret != BAD_FUNC_ARG) {
            flag = WOLFSSL_FATAL_ERROR;
        }
    }

    if (!flag) {
        ret = wc_Sha256Update(NULL, (byte*)a.input, (word32)a.inLen);
        if (ret != BAD_FUNC_ARG) {
            flag = WOLFSSL_FATAL_ERROR;
        }
    }

    wc_Sha256Free(&sha256);
    /* If not returned then the unit test passed. */
    printf(resultFmt, flag == 0 ? passed : failed);

#endif
    return flag;

} /* END test_wc_Sha256Update */


/*
 * Unit test function for wc_Sha256Final()
 */
static int test_wc_Sha256Final (void)
{
    int flag = 0;
#ifndef NO_SHA256
    wc_Sha256 sha256;
    byte* hash_test[3];
    byte hash1[WC_SHA256_DIGEST_SIZE];
    byte hash2[2*WC_SHA256_DIGEST_SIZE];
    byte hash3[5*WC_SHA256_DIGEST_SIZE];
    int times, i, ret;

    /* Initialize */
    ret = wc_InitSha256(&sha256);
    if (ret != 0) {
        flag =  ret;
    }

    if (!flag) {
        hash_test[0] = hash1;
        hash_test[1] = hash2;
        hash_test[2] = hash3;
    }

    times = sizeof(hash_test) / sizeof(byte*);

    /* Good test args. */
    printf(testingFmt, "wc_Sha256Final()");

    for (i = 0; i < times; i++) {
        if (!flag) {
            ret = wc_Sha256Final(&sha256, hash_test[i]);
            if (ret != 0) {
                flag = WOLFSSL_FATAL_ERROR;
            }
        }
    }

    /* Test bad args. */
    if (!flag ) {
        ret = wc_Sha256Final(NULL, NULL);
        if (ret != BAD_FUNC_ARG) {
            flag = WOLFSSL_FATAL_ERROR;
        }
    }

    if (!flag) {
        ret = wc_Sha256Final(NULL, hash1);
        if (ret != BAD_FUNC_ARG) {
            flag = WOLFSSL_FATAL_ERROR;
        }
    }

    if (!flag) {
        ret = wc_Sha256Final(&sha256, NULL);
        if (ret != BAD_FUNC_ARG) {
            flag = WOLFSSL_FATAL_ERROR;
        }
    }

    wc_Sha256Free(&sha256);

    printf(resultFmt, flag == 0 ? passed : failed);

#endif
    return flag;

} /* END test_wc_Sha256Final */



/*
 * Testing wc_InitSha512()
 */
static int test_wc_InitSha512 (void)
{
    int flag = 0;
#ifdef WOLFSSL_SHA512
    wc_Sha512 sha512;
    int ret;

    printf(testingFmt, "wc_InitSha512()");

    /* Test good arg. */
    ret = wc_InitSha512(&sha512);
    if (ret != 0) {
        flag  = WOLFSSL_FATAL_ERROR;
    }

    /* Test bad arg. */
    if (!flag) {
        ret = wc_InitSha512(NULL);
        if (ret != BAD_FUNC_ARG) {
            flag = WOLFSSL_FATAL_ERROR;
        }
    }

    wc_Sha512Free(&sha512);

    printf(resultFmt, flag == 0 ? passed : failed);

#endif
    return flag;

} /* END test_wc_InitSha512 */


/*
 *  wc_Sha512Update() test.
 */
static int test_wc_Sha512Update (void)
{

    int flag = 0;
#ifdef WOLFSSL_SHA512
    wc_Sha512 sha512;
    byte hash[WC_SHA512_DIGEST_SIZE];
    testVector a, b, c;
    int ret;

    ret = wc_InitSha512(&sha512);
    if (ret != 0) {
        flag = ret;
    }

    printf(testingFmt, "wc_Sha512Update()");

    /* Input. */
    if (!flag) {
        a.input = "a";
        a.inLen = XSTRLEN(a.input);

        ret = wc_Sha512Update(&sha512, (byte*)a.input, (word32)a.inLen);
        if (ret != 0) {
            flag = ret;
        }

        ret = wc_Sha512Final(&sha512, hash);
        if (ret != 0) {
            flag = ret;
        }
    }

    /* Update input. */
    if (!flag) {
        a.input = "abc";
        a.output = "\xdd\xaf\x35\xa1\x93\x61\x7a\xba\xcc\x41\x73\x49\xae\x20\x41"
                    "\x31\x12\xe6\xfa\x4e\x89\xa9\x7e\xa2\x0a\x9e\xee\xe6\x4b"
                    "\x55\xd3\x9a\x21\x92\x99\x2a\x27\x4f\xc1\xa8\x36\xba\x3c"
                    "\x23\xa3\xfe\xeb\xbd\x45\x4d\x44\x23\x64\x3c\xe8\x0e\x2a"
                    "\x9a\xc9\x4f\xa5\x4c\xa4\x9f";
        a.inLen = XSTRLEN(a.input);
        a.outLen = XSTRLEN(a.output);

        ret = wc_Sha512Update(&sha512, (byte*) a.input, (word32) a.inLen);
        if (ret != 0) {
            flag = ret;
        }
    }

    if (!flag) {
        ret = wc_Sha512Final(&sha512, hash);
        if (ret != 0) {
            flag = ret;
        }
    }

    if (!flag) {
        if (XMEMCMP(hash, a.output, WC_SHA512_DIGEST_SIZE) != 0) {
            flag = WOLFSSL_FATAL_ERROR;
        }
    }

    /* Try passing in bad values */
    if (!flag) {
        b.input = NULL;
        b.inLen = 0;

        ret = wc_Sha512Update(&sha512, (byte*)b.input, (word32)b.inLen);
        if (ret != 0) {
            flag = ret;
        }
    }

    if (!flag) {
        c.input = NULL;
        c.inLen = WC_SHA512_DIGEST_SIZE;

        ret = wc_Sha512Update(&sha512, (byte*)c.input, (word32)c.inLen);
        if (ret != BAD_FUNC_ARG) {
            flag = WOLFSSL_FATAL_ERROR;
        }
    }

    if (!flag) {
        ret = wc_Sha512Update(NULL, (byte*)a.input, (word32)a.inLen);
        if (ret != BAD_FUNC_ARG) {
            flag = WOLFSSL_FATAL_ERROR;
        }
    }

    wc_Sha512Free(&sha512);

    /* If not returned then the unit test passed test vectors. */
    printf(resultFmt, flag == 0 ? passed : failed);

#endif
    return flag;

} /* END test_wc_Sha512Update  */


/*
 * Unit test function for wc_Sha512Final()
 */
static int test_wc_Sha512Final (void)
{
    int flag = 0;
#ifdef WOLFSSL_SHA512
    wc_Sha512 sha512;
    byte* hash_test[3];
    byte hash1[WC_SHA512_DIGEST_SIZE];
    byte hash2[2*WC_SHA512_DIGEST_SIZE];
    byte hash3[5*WC_SHA512_DIGEST_SIZE];
    int times, i, ret;

    /* Initialize  */
    ret = wc_InitSha512(&sha512);
    if (ret != 0) {
        flag =  ret;
    }

    if (!flag) {
        hash_test[0] = hash1;
        hash_test[1] = hash2;
        hash_test[2] = hash3;
    }

    times = sizeof(hash_test) / sizeof(byte *);

    /* Good test args. */
    printf(testingFmt, "wc_Sha512Final()");

    for (i = 0; i < times; i++) {
        if (!flag) {
            ret = wc_Sha512Final(&sha512, hash_test[i]);
            if (ret != 0) {
                flag = WOLFSSL_FATAL_ERROR;
            }
        }
    }
    /* Test bad args. */
    if (!flag) {
        ret = wc_Sha512Final(NULL, NULL);
        if (ret != BAD_FUNC_ARG) {
            flag = WOLFSSL_FATAL_ERROR;
        }

    if (!flag) {}
        ret = wc_Sha512Final(NULL, hash1);
        if (ret != BAD_FUNC_ARG) {
            flag = WOLFSSL_FATAL_ERROR;
        }
    }

    if (!flag) {
        ret = wc_Sha512Final(&sha512, NULL);
        if (ret != BAD_FUNC_ARG) {
            flag = WOLFSSL_FATAL_ERROR;
        }
    }

    wc_Sha512Free(&sha512);

    printf(resultFmt, flag == 0 ? passed : failed);

#endif
    return flag;
} /* END test_wc_Sha512Final */




/*
 * Testing wc_InitSha384()
 */
static int test_wc_InitSha384 (void)
{
    int flag = 0;
#ifdef WOLFSSL_SHA384
    wc_Sha384 sha384;
    int ret;

    printf(testingFmt, "wc_InitSha384()");

    /* Test good arg. */
    ret = wc_InitSha384(&sha384);
    if (ret != 0) {
        flag = WOLFSSL_FATAL_ERROR;
    }

    /* Test bad arg. */
    if (!flag) {
        ret = wc_InitSha384(NULL);
        if (ret != BAD_FUNC_ARG) {
            flag = WOLFSSL_FATAL_ERROR;
        }
    }

    wc_Sha384Free(&sha384);
    printf(resultFmt, flag == 0 ? passed : failed);

#endif
    return flag;
} /* END test_wc_InitSha384 */


/*
 * test wc_Sha384Update()
 */
static int test_wc_Sha384Update (void)
{

    int flag = 0;
#ifdef WOLFSSL_SHA384
    wc_Sha384 sha384;
    byte hash[WC_SHA384_DIGEST_SIZE];
    testVector a, b, c;
    int ret;

    ret = wc_InitSha384(&sha384);
    if (ret != 0) {
        flag = ret;
    }

    printf(testingFmt, "wc_Sha384Update()");

    /* Input */
    if (!flag) {
        a.input = "a";
        a.inLen = XSTRLEN(a.input);

        ret = wc_Sha384Update(&sha384, (byte*)a.input, (word32)a.inLen);
        if (ret != 0) {
            flag = ret;
        }
    }

    if (!flag) {
        ret = wc_Sha384Final(&sha384, hash);
        if (ret != 0) {
            flag = ret;
        }
    }

    /* Update input. */
    if (!flag) {
        a.input = "abc";
        a.output = "\xcb\x00\x75\x3f\x45\xa3\x5e\x8b\xb5\xa0\x3d\x69\x9a\xc6\x50"
                   "\x07\x27\x2c\x32\xab\x0e\xde\xd1\x63\x1a\x8b\x60\x5a\x43\xff"
                   "\x5b\xed\x80\x86\x07\x2b\xa1\xe7\xcc\x23\x58\xba\xec\xa1\x34"
                   "\xc8\x25\xa7";
        a.inLen = XSTRLEN(a.input);
        a.outLen = XSTRLEN(a.output);

        ret = wc_Sha384Update(&sha384, (byte*)a.input, (word32)a.inLen);
        if (ret != 0) {
            flag = ret;
        }
    }

    if (!flag) {
        ret = wc_Sha384Final(&sha384, hash);
        if (ret != 0) {
            flag = ret;
        }
    }

    if (!flag) {
        if (XMEMCMP(hash, a.output, WC_SHA384_DIGEST_SIZE) != 0) {
            flag = WOLFSSL_FATAL_ERROR;
        }
    }

    /* Pass in bad values. */
    if (!flag) {
        b.input = NULL;
        b.inLen = 0;

        ret = wc_Sha384Update(&sha384, (byte*)b.input, (word32)b.inLen);
        if (ret != 0) {
            flag = ret;
        }
    }

    if (!flag) {
        c.input = NULL;
        c.inLen = WC_SHA384_DIGEST_SIZE;

        ret = wc_Sha384Update(&sha384, (byte*)c.input, (word32)c.inLen);
        if (ret != BAD_FUNC_ARG) {
            flag = WOLFSSL_FATAL_ERROR;
        }
    }

    if (!flag) {
        ret = wc_Sha384Update(NULL, (byte*)a.input, (word32)a.inLen);
        if (ret != BAD_FUNC_ARG) {
            flag = WOLFSSL_FATAL_ERROR;
        }
    }

    wc_Sha384Free(&sha384);

    /* If not returned then the unit test passed test vectors. */
    printf(resultFmt, flag == 0 ? passed : failed);

#endif
    return flag;
} /* END test_wc_Sha384Update */

/*
 * Unit test function for wc_Sha384Final();
 */
static int test_wc_Sha384Final (void)
{
    int flag = 0;
#ifdef WOLFSSL_SHA384
    wc_Sha384 sha384;
    byte* hash_test[3];
    byte hash1[WC_SHA384_DIGEST_SIZE];
    byte hash2[2*WC_SHA384_DIGEST_SIZE];
    byte hash3[5*WC_SHA384_DIGEST_SIZE];
    int times, i, ret;

    /* Initialize */
    ret = wc_InitSha384(&sha384);
    if (ret) {
        flag = ret;
    }

    if (!flag) {
        hash_test[0] = hash1;
        hash_test[1] = hash2;
        hash_test[2] = hash3;
    }

    times = sizeof(hash_test) / sizeof(byte*);

    /* Good test args. */
    printf(testingFmt, "wc_Sha384Final()");

    for (i = 0; i < times; i++) {
        if (!flag) {
            ret = wc_Sha384Final(&sha384, hash_test[i]);
            if (ret != 0) {
                flag = WOLFSSL_FATAL_ERROR;
            }
        }
    }

    /* Test bad args. */
    if (!flag) {
        ret = wc_Sha384Final(NULL, NULL);
        if (ret != BAD_FUNC_ARG) {
            flag = WOLFSSL_FATAL_ERROR;
        }
    }

    if (!flag) {
        ret = wc_Sha384Final(NULL, hash1);
        if (ret != BAD_FUNC_ARG) {
            flag = WOLFSSL_FATAL_ERROR;
        }
    }

    if (!flag) {
        ret = wc_Sha384Final(&sha384, NULL);
        if (ret != BAD_FUNC_ARG) {
            flag = WOLFSSL_FATAL_ERROR;
        }
    }

    wc_Sha384Free(&sha384);

    printf(resultFmt, flag == 0 ? passed : failed);

#endif
    return flag;

} /* END test_wc_Sha384Final */



/*
 * Testing wc_InitSha224();
 */
static int test_wc_InitSha224 (void)
{
    int flag = 0;
#ifdef WOLFSSL_SHA224
    wc_Sha224 sha224;
    int ret;

    printf(testingFmt, "wc_InitSha224()");

    /* Test good arg. */
    ret = wc_InitSha224(&sha224);
    if (ret != 0) {
        flag = WOLFSSL_FATAL_ERROR;
    }

    /* Test bad arg. */
    if (!flag) {
        ret = wc_InitSha224(NULL);
        if (ret != BAD_FUNC_ARG) {
            flag = WOLFSSL_FATAL_ERROR;
        }
    }

    wc_Sha224Free(&sha224);
    printf(resultFmt, flag == 0 ? passed : failed);

#endif
    return flag;
} /* END test_wc_InitSha224 */



/*
 * Unit test on wc_Sha224Update
 */
static int test_wc_Sha224Update (void)
{
    int flag = 0;
#ifdef WOLFSSL_SHA224
    wc_Sha224 sha224;
    byte hash[WC_SHA224_DIGEST_SIZE];
    testVector a, b, c;
    int ret;

    ret = wc_InitSha224(&sha224);
    if (ret != 0) {
        flag = ret;
    }

    printf(testingFmt, "wc_Sha224Update()");

    /* Input. */
    if (!flag) {
        a.input = "a";
        a.inLen = XSTRLEN(a.input);

        ret = wc_Sha224Update(&sha224, (byte*)a.input, (word32)a.inLen);
        if (ret != 0) {
            flag = ret;
        }
    }

    if (!flag) {
        ret = wc_Sha224Final(&sha224, hash);
        if (ret != 0) {
            flag = ret;
        }
    }

    /* Update input. */
    if (!flag) {
        a.input = "abc";
        a.output = "\x23\x09\x7d\x22\x34\x05\xd8\x22\x86\x42\xa4\x77\xbd\xa2"
                    "\x55\xb3\x2a\xad\xbc\xe4\xbd\xa0\xb3\xf7\xe3\x6c\x9d\xa7";
        a.inLen = XSTRLEN(a.input);
        a.outLen = XSTRLEN(a.output);

        ret = wc_Sha224Update(&sha224, (byte*)a.input, (word32)a.inLen);
        if (ret != 0) {
            flag = ret;
        }
    }

    if (!flag) {
        ret = wc_Sha224Final(&sha224, hash);
        if (ret != 0) {
            flag = ret;
        }
    }

    if (!flag) {
        if (XMEMCMP(hash, a.output, WC_SHA224_DIGEST_SIZE) != 0) {
            flag = WOLFSSL_FATAL_ERROR;
        }
    }

    /* Pass  in bad values. */
    if (!flag) {
        b.input = NULL;
        b.inLen = 0;

        ret = wc_Sha224Update(&sha224, (byte*)b.input, (word32)b.inLen);
        if (ret != 0) {
            flag = ret;
        }
    }

    if (!flag) {
        c.input = NULL;
        c.inLen = WC_SHA224_DIGEST_SIZE;

        ret = wc_Sha224Update(&sha224, (byte*)c.input, (word32)c.inLen);
        if (ret != BAD_FUNC_ARG) {
            flag = WOLFSSL_FATAL_ERROR;
        }
    }

    if (!flag) {
        ret = wc_Sha224Update(NULL, (byte*)a.input, (word32)a.inLen);
        if (ret != BAD_FUNC_ARG) {
            flag = WOLFSSL_FATAL_ERROR;
        }
    }

    wc_Sha224Free(&sha224);

    /* If not returned then the unit test passed test vectors. */
    printf(resultFmt, flag == 0 ? passed : failed);

#endif
    return flag;

} /* END test_wc_Sha224Update */



/*
 * Unit test for wc_Sha224Final();
 */
static int test_wc_Sha224Final (void)
{
    int flag = 0;
#ifdef WOLFSSL_SHA224
    wc_Sha224 sha224;
    byte* hash_test[3];
    byte hash1[WC_SHA224_DIGEST_SIZE];
    byte hash2[2*WC_SHA224_DIGEST_SIZE];
    byte hash3[5*WC_SHA224_DIGEST_SIZE];
    int times, i, ret;

    /* Initialize */
    ret = wc_InitSha224(&sha224);
    if (ret) {
        flag = ret;
    }

    if (!flag) {
        hash_test[0] = hash1;
        hash_test[1] = hash2;
        hash_test[2] = hash3;
    }

    times = sizeof(hash_test) / sizeof(byte*);

    /* Good test args. */
    printf(testingFmt, "wc_sha224Final()");
    /* Testing oversized buffers. */
    for (i = 0; i < times; i++) {
        if (!flag) {
            ret = wc_Sha224Final(&sha224, hash_test[i]);
            if (ret != 0) {
                flag = WOLFSSL_FATAL_ERROR;
            }
        }
    }

    /* Test bad args. */
    if (!flag) {
        ret = wc_Sha224Final(NULL, NULL);
        if (ret != BAD_FUNC_ARG) {
            flag = WOLFSSL_FATAL_ERROR;
        }
    }

    if (!flag) {
        ret = wc_Sha224Final(NULL, hash1);
        if (ret != BAD_FUNC_ARG) {
            flag = WOLFSSL_FATAL_ERROR;
        }
    }

    if (!flag) {
        ret = wc_Sha224Final(&sha224, NULL);
        if (ret != BAD_FUNC_ARG) {
            flag = WOLFSSL_FATAL_ERROR;
        }
    }

    wc_Sha224Free(&sha224);

    printf(resultFmt, flag == 0 ? passed : failed);

#endif
    return flag;
} /* END test_wc_Sha224Final */




/*
 * Testing wc_InitRipeMd()
 */
static int test_wc_InitRipeMd (void)
{
    int flag = 0;
#ifdef WOLFSSL_RIPEMD
    RipeMd ripemd;
    int ret;

    printf(testingFmt, "wc_InitRipeMd()");

    /* Test good arg. */
    ret = wc_InitRipeMd(&ripemd);
    if (ret != 0) {
        flag = WOLFSSL_FATAL_ERROR;
    }

    /* Test bad arg. */
    if (!flag) {
        ret = wc_InitRipeMd(NULL);
        if (ret != BAD_FUNC_ARG) {
            flag = WOLFSSL_FATAL_ERROR;
        }
    }

    printf(resultFmt, flag == 0 ? passed : failed);

#endif
    return flag;

} /* END test_wc_InitRipeMd */


/*
 * Testing wc_RipeMdUpdate()
 */
static int test_wc_RipeMdUpdate (void)
{

    int flag = 0;
#ifdef WOLFSSL_RIPEMD
    RipeMd ripemd;
    byte hash[RIPEMD_DIGEST_SIZE];
    testVector a, b, c;
    int ret;

    ret = wc_InitRipeMd(&ripemd);
    if (ret != 0) {
        flag = ret;
    }

    printf(testingFmt, "wc_RipeMdUpdate()");

    /* Input */
    if (!flag) {
        a.input = "a";
        a.inLen = XSTRLEN(a.input);

        ret = wc_RipeMdUpdate(&ripemd, (byte*)a.input, (word32)a.inLen);
        if (ret != 0) {
            flag = ret;
        }
    }

    if (!flag) {
        ret = wc_RipeMdFinal(&ripemd, hash);
        if (ret != 0) {
            flag = ret;
        }
    }

    /* Update input. */
    if (!flag) {
        a.input = "abc";
        a.output = "\x8e\xb2\x08\xf7\xe0\x5d\x98\x7a\x9b\x04\x4a\x8e\x98\xc6"
                   "\xb0\x87\xf1\x5a\x0b\xfc";
        a.inLen = XSTRLEN(a.input);
        a.outLen = XSTRLEN(a.output);

        ret = wc_RipeMdUpdate(&ripemd, (byte*)a.input, (word32)a.inLen);
        if (ret != 0) {
            flag = ret;
        }
    }

    if (!flag) {
        ret = wc_RipeMdFinal(&ripemd, hash);
        if (ret != 0) {
            flag = ret;
        }
    }

    if (!flag) {
        if (XMEMCMP(hash, a.output, RIPEMD_DIGEST_SIZE) != 0) {
            flag = WOLFSSL_FATAL_ERROR;
        }
    }

    /* Pass in bad values. */
    if (!flag) {
        b.input = NULL;
        b.inLen = 0;

        ret = wc_RipeMdUpdate(&ripemd, (byte*)b.input, (word32)b.inLen);
        if (ret != 0) {
            flag = ret;
        }
    }

    if (!flag) {
        c.input = NULL;
        c.inLen = RIPEMD_DIGEST_SIZE;

        ret = wc_RipeMdUpdate(&ripemd, (byte*)c.input, (word32)c.inLen);
        if (ret != BAD_FUNC_ARG) {
            flag = WOLFSSL_FATAL_ERROR;
        }
    }

    if (!flag) {
        ret = wc_RipeMdUpdate(NULL, (byte*)a.input, (word32)a.inLen);
        if (ret != BAD_FUNC_ARG) {
            flag = WOLFSSL_FATAL_ERROR;
        }
    }

    printf(resultFmt, flag == 0 ? passed : failed);

#endif
    return flag;

} /* END test_wc_RipeMdUdpate */



/*
 * Unit test function for wc_RipeMdFinal()
 */
static int test_wc_RipeMdFinal (void)
{
    int flag = 0;
#ifdef WOLFSSL_RIPEMD
    RipeMd ripemd;
    byte* hash_test[3];
    byte hash1[RIPEMD_DIGEST_SIZE];
    byte hash2[2*RIPEMD_DIGEST_SIZE];
    byte hash3[5*RIPEMD_DIGEST_SIZE];
    int times, i, ret;

    /* Initialize */
    ret = wc_InitRipeMd(&ripemd);
    if (ret != 0) {
        flag = ret;
    }

    if (!flag) {
        hash_test[0] = hash1;
        hash_test[1] = hash2;
        hash_test[2] = hash3;
    }

    times = sizeof(hash_test) / sizeof(byte*);

    /* Good test args. */
    printf(testingFmt, "wc_RipeMdFinal()");
    /* Testing oversized buffers. */
    for (i = 0; i < times; i++) {
        if (!flag) {
            ret = wc_RipeMdFinal(&ripemd, hash_test[i]);
            if (ret != 0) {
                flag = WOLFSSL_FATAL_ERROR;
            }
        }
    }

    /* Test bad args. */
    if (!flag) {
        ret = wc_RipeMdFinal(NULL, NULL);
        if (ret != BAD_FUNC_ARG) {
            flag = WOLFSSL_FATAL_ERROR;
        }
    }

    if (!flag) {
        ret = wc_RipeMdFinal(NULL, hash1);
        if (ret != BAD_FUNC_ARG) {
            flag = WOLFSSL_FATAL_ERROR;
        }
    }

    if (!flag) {
        ret = wc_RipeMdFinal(&ripemd, NULL);
        if (ret != BAD_FUNC_ARG) {
            flag = WOLFSSL_FATAL_ERROR;
        }
    }

    printf(resultFmt, flag == 0 ? passed : failed);

#endif
    return flag;
} /* END test_wc_RipeMdFinal */





/*
 * Testing wc_InitSha3_224, wc_InitSha3_256, wc_InitSha3_384, and
 * wc_InitSha3_512
 */
static int test_wc_InitSha3 (void)
{
    int             ret = 0;
#if defined(WOLFSSL_SHA3)
    wc_Sha3            sha3;

    #if !defined(WOLFSSL_NOSHA3_224)
        printf(testingFmt, "wc_InitSha3_224()");

        ret = wc_InitSha3_224(&sha3, HEAP_HINT, devId);

        /* Test bad args. */
        if (ret == 0) {
            ret = wc_InitSha3_224(NULL, HEAP_HINT, devId);
            if (ret == BAD_FUNC_ARG) {
                ret = 0;
            } else if (ret == 0) {
                ret = WOLFSSL_FATAL_ERROR;
            }
        }
        wc_Sha3_224_Free(&sha3);
        printf(resultFmt, ret == 0 ? passed : failed);
    #endif /* NOSHA3_224 */
    #if !defined(WOLFSSL_NOSHA3_256)
        if (ret == 0) {
            printf(testingFmt, "wc_InitSha3_256()");

            ret = wc_InitSha3_256(&sha3, HEAP_HINT, devId);
            /* Test bad args. */
            if (ret == 0) {
                ret = wc_InitSha3_256(NULL, HEAP_HINT, devId);
                if (ret == BAD_FUNC_ARG) {
                    ret = 0;
                } else if (ret == 0) {
                    ret = WOLFSSL_FATAL_ERROR;
                }
            }
            wc_Sha3_256_Free(&sha3);
            printf(resultFmt, ret == 0 ? passed : failed);
        } /* END sha3_256 */
    #endif /* NOSHA3_256 */
    #if !defined(WOLFSSL_NOSHA3_384)
        if (ret == 0) {
            printf(testingFmt, "wc_InitSha3_384()");

            ret = wc_InitSha3_384(&sha3, HEAP_HINT, devId);
            /* Test bad args. */
            if (ret == 0) {
                ret = wc_InitSha3_384(NULL, HEAP_HINT, devId);
                if (ret == BAD_FUNC_ARG) {
                    ret = 0;
                } else if (ret == 0) {
                    ret = WOLFSSL_FATAL_ERROR;
                }
            }
            wc_Sha3_384_Free(&sha3);
            printf(resultFmt, ret == 0 ? passed : failed);
        } /* END sha3_384 */
    #endif /* NOSHA3_384 */
    #if !defined(WOLFSSL_NOSHA3_512)
        if (ret == 0) {
            printf(testingFmt, "wc_InitSha3_512()");

            ret = wc_InitSha3_512(&sha3, HEAP_HINT, devId);
            /* Test bad args. */
            if (ret == 0) {
                ret = wc_InitSha3_512(NULL, HEAP_HINT, devId);
                if (ret == BAD_FUNC_ARG) {
                    ret = 0;
                } else if (ret == 0) {
                    ret = WOLFSSL_FATAL_ERROR;
                }
            }
            wc_Sha3_512_Free(&sha3);
            printf(resultFmt, ret == 0 ? passed : failed);
        } /* END sha3_512 */
    #endif /* NOSHA3_512 */
#endif
    return ret;

} /* END test_wc_InitSha3 */


/*
 * Testing wc_Sha3_Update()
 */
static int testing_wc_Sha3_Update (void)
{
    int         ret = 0;

#if defined(WOLFSSL_SHA3) && !defined(WOLFSSL_XILINX_CRYPT) && \
   !defined(WOLFSSL_AFALG_XILINX)
    wc_Sha3        sha3;
    byte        msg[] = "Everybody's working for the weekend.";
    byte        msg2[] = "Everybody gets Friday off.";
    byte        msgCmp[] = "\x45\x76\x65\x72\x79\x62\x6f\x64\x79\x27\x73\x20"
                        "\x77\x6f\x72\x6b\x69\x6e\x67\x20\x66\x6f\x72\x20\x74"
                        "\x68\x65\x20\x77\x65\x65\x6b\x65\x6e\x64\x2e\x45\x76"
                        "\x65\x72\x79\x62\x6f\x64\x79\x20\x67\x65\x74\x73\x20"
                        "\x46\x72\x69\x64\x61\x79\x20\x6f\x66\x66\x2e";
    word32      msglen = sizeof(msg) - 1;
    word32      msg2len = sizeof(msg2);
    word32      msgCmplen = sizeof(msgCmp);

    #if !defined(WOLFSSL_NOSHA3_224)
        printf(testingFmt, "wc_Sha3_224_Update()");

        ret = wc_InitSha3_224(&sha3, HEAP_HINT, devId);
        if (ret != 0) {
            return ret;
        }

        ret = wc_Sha3_224_Update(&sha3, msg, msglen);
        if (XMEMCMP(msg, sha3.t, msglen) || sha3.i != msglen) {
            ret = WOLFSSL_FATAL_ERROR;
        }
        if (ret == 0) {
            ret = wc_Sha3_224_Update(&sha3, msg2, msg2len);
            if (ret == 0 && XMEMCMP(sha3.t, msgCmp, msgCmplen) != 0) {
                ret = WOLFSSL_FATAL_ERROR;
            }
        }
        /* Pass bad args. */
        if (ret == 0) {
            ret = wc_Sha3_224_Update(NULL, msg2, msg2len);
            if (ret == BAD_FUNC_ARG) {
                ret = wc_Sha3_224_Update(&sha3, NULL, 5);
            }
            if (ret == BAD_FUNC_ARG) {
                wc_Sha3_224_Free(&sha3);
                if (wc_InitSha3_224(&sha3, HEAP_HINT, devId)) {
                    return ret;
                }
                ret = wc_Sha3_224_Update(&sha3, NULL, 0);
                if (ret == 0) {
                    ret = wc_Sha3_224_Update(&sha3, msg2, msg2len);
                }
                if (ret == 0 && XMEMCMP(msg2, sha3.t, msg2len) != 0) {
                    ret = WOLFSSL_FATAL_ERROR;
                }
            }
        }
        wc_Sha3_224_Free(&sha3);

        printf(resultFmt, ret == 0 ? passed : failed);
    #endif /* SHA3_224 */

    #if !defined(WOLFSSL_NOSHA3_256)
        if (ret == 0) {
            printf(testingFmt, "wc_Sha3_256_Update()");

            ret = wc_InitSha3_256(&sha3, HEAP_HINT, devId);
            if (ret != 0) {
                return ret;
            }
            ret = wc_Sha3_256_Update(&sha3, msg, msglen);
            if (XMEMCMP(msg, sha3.t, msglen) || sha3.i != msglen) {
                ret = WOLFSSL_FATAL_ERROR;
            }
            if (ret == 0) {
                ret = wc_Sha3_256_Update(&sha3, msg2, msg2len);
                if (XMEMCMP(sha3.t, msgCmp, msgCmplen) != 0) {
                    ret = WOLFSSL_FATAL_ERROR;
                }
            }
            /* Pass bad args. */
            if (ret == 0) {
                ret = wc_Sha3_256_Update(NULL, msg2, msg2len);
                if (ret == BAD_FUNC_ARG) {
                    ret = wc_Sha3_256_Update(&sha3, NULL, 5);
                }
                if (ret == BAD_FUNC_ARG) {
                    wc_Sha3_256_Free(&sha3);
                    if (wc_InitSha3_256(&sha3, HEAP_HINT, devId)) {
                        return ret;
                    }
                    ret = wc_Sha3_256_Update(&sha3, NULL, 0);
                    if (ret == 0) {
                        ret = wc_Sha3_256_Update(&sha3, msg2, msg2len);
                    }
                    if (ret == 0 && XMEMCMP(msg2, sha3.t, msg2len) != 0) {
                        ret = WOLFSSL_FATAL_ERROR;
                    }
                }
            }
            wc_Sha3_256_Free(&sha3);

            printf(resultFmt, ret == 0 ? passed : failed);
        }
    #endif /* SHA3_256 */

    #if !defined(WOLFSSL_NOSHA3_384)
        if (ret == 0) {
            printf(testingFmt, "wc_Sha3_384_Update()");

            ret = wc_InitSha3_384(&sha3, HEAP_HINT, devId);
            if (ret != 0) {
                return ret;
            }
            ret = wc_Sha3_384_Update(&sha3, msg, msglen);
            if (XMEMCMP(msg, sha3.t, msglen) || sha3.i != msglen) {
                ret = WOLFSSL_FATAL_ERROR;
            }
            if (ret == 0) {
                ret = wc_Sha3_384_Update(&sha3, msg2, msg2len);
                if (XMEMCMP(sha3.t, msgCmp, msgCmplen) != 0) {
                    ret = WOLFSSL_FATAL_ERROR;
                }
            }
            /* Pass bad args. */
            if (ret == 0) {
                ret = wc_Sha3_384_Update(NULL, msg2, msg2len);
                if (ret == BAD_FUNC_ARG) {
                    ret = wc_Sha3_384_Update(&sha3, NULL, 5);
                }
                if (ret == BAD_FUNC_ARG) {
                    wc_Sha3_384_Free(&sha3);
                    if (wc_InitSha3_384(&sha3, HEAP_HINT, devId)) {
                        return ret;
                    }
                    ret = wc_Sha3_384_Update(&sha3, NULL, 0);
                    if (ret == 0) {
                        ret = wc_Sha3_384_Update(&sha3, msg2, msg2len);
                    }
                    if (ret == 0 && XMEMCMP(msg2, sha3.t, msg2len) != 0) {
                        ret = WOLFSSL_FATAL_ERROR;
                    }
                }
            }
            wc_Sha3_384_Free(&sha3);

            printf(resultFmt, ret == 0 ? passed : failed);
        }
    #endif /* SHA3_384 */

    #if !defined(WOLFSSL_NOSHA3_512)
        if (ret == 0) {
            printf(testingFmt, "wc_Sha3_512_Update()");

            ret = wc_InitSha3_512(&sha3, HEAP_HINT, devId);
            if (ret != 0) {
                return ret;
            }
            ret = wc_Sha3_512_Update(&sha3, msg, msglen);
            if (XMEMCMP(msg, sha3.t, msglen) || sha3.i != msglen) {
                ret = WOLFSSL_FATAL_ERROR;
            }
            if (ret == 0) {
                ret = wc_Sha3_512_Update(&sha3, msg2, msg2len);
                if (XMEMCMP(sha3.t, msgCmp, msgCmplen) != 0) {
                    ret = WOLFSSL_FATAL_ERROR;
                }
            }
            /* Pass bad args. */
            if (ret == 0) {
                ret = wc_Sha3_512_Update(NULL, msg2, msg2len);
                if (ret == BAD_FUNC_ARG) {
                    ret = wc_Sha3_512_Update(&sha3, NULL, 5);
                }
                if (ret == BAD_FUNC_ARG) {
                    wc_Sha3_512_Free(&sha3);
                    if (wc_InitSha3_512(&sha3, HEAP_HINT, devId)) {
                        return ret;
                    }
                    ret = wc_Sha3_512_Update(&sha3, NULL, 0);
                    if (ret == 0) {
                        ret = wc_Sha3_512_Update(&sha3, msg2, msg2len);
                    }
                    if (ret == 0 && XMEMCMP(msg2, sha3.t, msg2len) != 0) {
                        ret = WOLFSSL_FATAL_ERROR;
                    }
                }
            }
            wc_Sha3_512_Free(&sha3);
            printf(resultFmt, ret == 0 ? passed : failed);
        }
    #endif /* SHA3_512 */
#endif /* WOLFSSL_SHA3 */
    return ret;

} /* END testing_wc_Sha3_Update */

/*
 *  Testing wc_Sha3_224_Final()
 */
static int test_wc_Sha3_224_Final (void)
{
    int         ret = 0;

#if defined(WOLFSSL_SHA3) && !defined(WOLFSSL_NOSHA3_224)
    wc_Sha3        sha3;
    const char* msg    = "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnom"
                         "nopnopq";
    const char* expOut = "\x8a\x24\x10\x8b\x15\x4a\xda\x21\xc9\xfd\x55"
                         "\x74\x49\x44\x79\xba\x5c\x7e\x7a\xb7\x6e\xf2"
                         "\x64\xea\xd0\xfc\xce\x33";
    byte        hash[WC_SHA3_224_DIGEST_SIZE];
    byte        hashRet[WC_SHA3_224_DIGEST_SIZE];

    /* Init stack variables. */
    XMEMSET(hash, 0, sizeof(hash));

    printf(testingFmt, "wc_Sha3_224_Final()");

    ret = wc_InitSha3_224(&sha3, HEAP_HINT, devId);
    if (ret != 0) {
        return ret;
    }

    ret= wc_Sha3_224_Update(&sha3, (byte*)msg, (word32)XSTRLEN(msg));
    if (ret == 0) {
        ret = wc_Sha3_224_Final(&sha3, hash);
        if (ret == 0 && XMEMCMP(expOut, hash, WC_SHA3_224_DIGEST_SIZE) != 0) {
            ret = WOLFSSL_FATAL_ERROR;
        }
    }
    /* Test bad args. */
    if (ret == 0) {
        ret = wc_Sha3_224_Final(NULL, hash);
        if (ret == 0) {
            ret = wc_Sha3_224_Final(&sha3, NULL);
        }
        if (ret == BAD_FUNC_ARG) {
            ret = 0;
        } else if (ret == 0) {
            ret = WOLFSSL_FATAL_ERROR;
        }
    }
    wc_Sha3_224_Free(&sha3);
    printf(resultFmt, ret == 0 ? passed : failed);

    if (ret == 0) {
        printf(testingFmt, "wc_Sha3_224_GetHash()");

        ret = wc_InitSha3_224(&sha3, HEAP_HINT, devId);
        if (ret != 0) {
            return ret;
        }

        /* Init stack variables. */
        XMEMSET(hash, 0, sizeof(hash));
        XMEMSET(hashRet, 0, sizeof(hashRet));

        ret= wc_Sha3_224_Update(&sha3, (byte*)msg, (word32)XSTRLEN(msg));
        if (ret == 0) {
            ret = wc_Sha3_224_GetHash(&sha3, hashRet);
        }

        if (ret == 0) {
            ret = wc_Sha3_224_Final(&sha3, hash);
            if (ret == 0 && XMEMCMP(hash, hashRet, WC_SHA3_224_DIGEST_SIZE) != 0) {
                ret = WOLFSSL_FATAL_ERROR;
            }
        }
        if (ret == 0) {
            /* Test bad args. */
            ret = wc_Sha3_224_GetHash(NULL, hashRet);
            if (ret == BAD_FUNC_ARG) {
                ret = wc_Sha3_224_GetHash(&sha3, NULL);
            }
            if (ret == BAD_FUNC_ARG) {
                ret = 0;
            } else if (ret == 0) {
                ret = WOLFSSL_FATAL_ERROR;
            }
        }

        printf(resultFmt, ret == 0 ? passed : failed);
    }

    wc_Sha3_224_Free(&sha3);
#endif
    return ret;
} /* END test_wc_Sha3_224_Final */


/*
 *  Testing wc_Sha3_256_Final()
 */
static int test_wc_Sha3_256_Final (void)
{
    int         ret = 0;

#if defined(WOLFSSL_SHA3) && !defined(WOLFSSL_NOSHA3_256)
    wc_Sha3        sha3;
    const char* msg    = "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnom"
                         "nopnopq";
    const char* expOut = "\x41\xc0\xdb\xa2\xa9\xd6\x24\x08\x49\x10\x03\x76\xa8"
                        "\x23\x5e\x2c\x82\xe1\xb9\x99\x8a\x99\x9e\x21\xdb\x32"
                        "\xdd\x97\x49\x6d\x33\x76";
    byte        hash[WC_SHA3_256_DIGEST_SIZE];
    byte        hashRet[WC_SHA3_256_DIGEST_SIZE];

    /* Init stack variables. */
    XMEMSET(hash, 0, sizeof(hash));

    printf(testingFmt, "wc_Sha3_256_Final()");

    ret = wc_InitSha3_256(&sha3, HEAP_HINT, devId);
    if (ret != 0) {
        return ret;
    }

    ret= wc_Sha3_256_Update(&sha3, (byte*)msg, (word32)XSTRLEN(msg));
    if (ret == 0) {
        ret = wc_Sha3_256_Final(&sha3, hash);
        if (ret == 0 && XMEMCMP(expOut, hash, WC_SHA3_256_DIGEST_SIZE) != 0) {
            ret = WOLFSSL_FATAL_ERROR;
        }
    }
    /* Test bad args. */
    if (ret == 0) {
        ret = wc_Sha3_256_Final(NULL, hash);
        if (ret == 0) {
            ret = wc_Sha3_256_Final(&sha3, NULL);
        }
        if (ret == BAD_FUNC_ARG) {
            ret = 0;
        } else if (ret == 0) {
            ret = WOLFSSL_FATAL_ERROR;
        }
    }
    wc_Sha3_256_Free(&sha3);
    printf(resultFmt, ret == 0 ? passed : failed);

    if (ret == 0) {
        printf(testingFmt, "wc_Sha3_256_GetHash()");

        ret = wc_InitSha3_256(&sha3, HEAP_HINT, devId);
        if (ret != 0) {
            return ret;
        }
        /* Init stack variables. */
        XMEMSET(hash, 0, sizeof(hash));
        XMEMSET(hashRet, 0, sizeof(hashRet));

        ret= wc_Sha3_256_Update(&sha3, (byte*)msg, (word32)XSTRLEN(msg));
        if (ret == 0) {
            ret = wc_Sha3_256_GetHash(&sha3, hashRet);
        }
        if (ret == 0) {
            ret = wc_Sha3_256_Final(&sha3, hash);
            if (ret == 0 && XMEMCMP(hash, hashRet, WC_SHA3_256_DIGEST_SIZE) != 0) {
                ret = WOLFSSL_FATAL_ERROR;
            }
        }
        if (ret == 0) {
            /* Test bad args. */
            ret = wc_Sha3_256_GetHash(NULL, hashRet);
            if (ret == BAD_FUNC_ARG) {
                ret = wc_Sha3_256_GetHash(&sha3, NULL);
            }
            if (ret == BAD_FUNC_ARG) {
                ret = 0;
            } else if (ret == 0) {
                ret = WOLFSSL_FATAL_ERROR;
            }
        }

        printf(resultFmt, ret == 0 ? passed : failed);
    }

    wc_Sha3_256_Free(&sha3);
#endif
    return ret;
} /* END test_wc_Sha3_256_Final */


/*
 *  Testing wc_Sha3_384_Final()
 */
static int test_wc_Sha3_384_Final (void)
{
    int         ret = 0;

#if defined(WOLFSSL_SHA3) && !defined(WOLFSSL_NOSHA3_384)
    wc_Sha3        sha3;
    const char* msg    = "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnom"
                         "nopnopq";
    const char* expOut = "\x99\x1c\x66\x57\x55\xeb\x3a\x4b\x6b\xbd\xfb\x75\xc7"
                         "\x8a\x49\x2e\x8c\x56\xa2\x2c\x5c\x4d\x7e\x42\x9b\xfd"
                         "\xbc\x32\xb9\xd4\xad\x5a\xa0\x4a\x1f\x07\x6e\x62\xfe"
                         "\xa1\x9e\xef\x51\xac\xd0\x65\x7c\x22";
    byte        hash[WC_SHA3_384_DIGEST_SIZE];
    byte        hashRet[WC_SHA3_384_DIGEST_SIZE];

    /* Init stack variables. */
    XMEMSET(hash, 0, sizeof(hash));

    printf(testingFmt, "wc_Sha3_384_Final()");

    ret = wc_InitSha3_384(&sha3, HEAP_HINT, devId);
    if (ret != 0) {
        return ret;
    }

    ret= wc_Sha3_384_Update(&sha3, (byte*)msg, (word32)XSTRLEN(msg));
    if (ret == 0) {
        ret = wc_Sha3_384_Final(&sha3, hash);
        if (ret == 0 && XMEMCMP(expOut, hash, WC_SHA3_384_DIGEST_SIZE) != 0) {
            ret = WOLFSSL_FATAL_ERROR;
        }
    }
    /* Test bad args. */
    if (ret == 0) {
        ret = wc_Sha3_384_Final(NULL, hash);
        if (ret == 0) {
            ret = wc_Sha3_384_Final(&sha3, NULL);
        }
        if (ret == BAD_FUNC_ARG) {
            ret = 0;
        } else if (ret == 0) {
            ret = WOLFSSL_FATAL_ERROR;
        }
    }
    wc_Sha3_384_Free(&sha3);
    printf(resultFmt, ret == 0 ? passed : failed);

    if (ret == 0) {
        printf(testingFmt, "wc_Sha3_384_GetHash()");

        ret = wc_InitSha3_384(&sha3, HEAP_HINT, devId);
        if (ret != 0) {
            return ret;
        }
        /* Init stack variables. */
        XMEMSET(hash, 0, sizeof(hash));
        XMEMSET(hashRet, 0, sizeof(hashRet));

        ret= wc_Sha3_384_Update(&sha3, (byte*)msg, (word32)XSTRLEN(msg));
        if (ret == 0) {
            ret = wc_Sha3_384_GetHash(&sha3, hashRet);
        }
        if (ret == 0) {
            ret = wc_Sha3_384_Final(&sha3, hash);
            if (ret == 0 && XMEMCMP(hash, hashRet, WC_SHA3_384_DIGEST_SIZE) != 0) {
                ret = WOLFSSL_FATAL_ERROR;
            }
        }
        if (ret == 0) {
            /* Test bad args. */
            ret = wc_Sha3_384_GetHash(NULL, hashRet);
            if (ret == BAD_FUNC_ARG) {
                ret = wc_Sha3_384_GetHash(&sha3, NULL);
            }
            if (ret == BAD_FUNC_ARG) {
                ret = 0;
            } else if (ret == 0) {
                ret = WOLFSSL_FATAL_ERROR;
            }
        }

        printf(resultFmt, ret == 0 ? passed : failed);
    }

    wc_Sha3_384_Free(&sha3);
#endif
    return ret;
} /* END test_wc_Sha3_384_Final */



/*
 *  Testing wc_Sha3_512_Final()
 */
static int test_wc_Sha3_512_Final (void)
{
    int         ret = 0;

#if defined(WOLFSSL_SHA3) && !defined(WOLFSSL_NOSHA3_512) && \
   !defined(WOLFSSL_NOSHA3_384)
    wc_Sha3        sha3;
    const char* msg    = "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnom"
                         "nopnopq";
    const char* expOut = "\x04\xa3\x71\xe8\x4e\xcf\xb5\xb8\xb7\x7c\xb4\x86\x10"
                         "\xfc\xa8\x18\x2d\xd4\x57\xce\x6f\x32\x6a\x0f\xd3\xd7"
                         "\xec\x2f\x1e\x91\x63\x6d\xee\x69\x1f\xbe\x0c\x98\x53"
                         "\x02\xba\x1b\x0d\x8d\xc7\x8c\x08\x63\x46\xb5\x33\xb4"
                         "\x9c\x03\x0d\x99\xa2\x7d\xaf\x11\x39\xd6\xe7\x5e";
    byte        hash[WC_SHA3_512_DIGEST_SIZE];
    byte        hashRet[WC_SHA3_512_DIGEST_SIZE];

    /* Init stack variables. */
    XMEMSET(hash, 0, sizeof(hash));

    printf(testingFmt, "wc_Sha3_512_Final()");

    ret = wc_InitSha3_512(&sha3, HEAP_HINT, devId);
    if (ret != 0) {
        return ret;
    }

    ret= wc_Sha3_512_Update(&sha3, (byte*)msg, (word32)XSTRLEN(msg));
    if (ret == 0) {
        ret = wc_Sha3_512_Final(&sha3, hash);
        if (ret == 0 && XMEMCMP(expOut, hash, WC_SHA3_512_DIGEST_SIZE) != 0) {
            ret = WOLFSSL_FATAL_ERROR;
        }
    }
    /* Test bad args. */
    if (ret == 0) {
        ret = wc_Sha3_512_Final(NULL, hash);
        if (ret == 0) {
            ret = wc_Sha3_384_Final(&sha3, NULL);
        }
        if (ret == BAD_FUNC_ARG) {
            ret = 0;
        } else if (ret == 0) {
            ret = WOLFSSL_FATAL_ERROR;
        }
    }
    wc_Sha3_512_Free(&sha3);
    printf(resultFmt, ret == 0 ? passed : failed);

    if (ret == 0) {
        printf(testingFmt, "wc_Sha3_512_GetHash()");

        ret = wc_InitSha3_512(&sha3, HEAP_HINT, devId);
        if (ret != 0) {
            return ret;
        }
        /* Init stack variables. */
        XMEMSET(hash, 0, sizeof(hash));
        XMEMSET(hashRet, 0, sizeof(hashRet));

        ret= wc_Sha3_512_Update(&sha3, (byte*)msg, (word32)XSTRLEN(msg));
        if (ret == 0) {
            ret = wc_Sha3_512_GetHash(&sha3, hashRet);
        }
        if (ret == 0) {
            ret = wc_Sha3_512_Final(&sha3, hash);
            if (ret == 0 && XMEMCMP(hash, hashRet, WC_SHA3_512_DIGEST_SIZE) != 0) {
                ret = WOLFSSL_FATAL_ERROR;
            }
        }
        if (ret == 0) {
            /* Test bad args. */
            ret = wc_Sha3_512_GetHash(NULL, hashRet);
            if (ret == BAD_FUNC_ARG) {
                ret = wc_Sha3_512_GetHash(&sha3, NULL);
            }
            if (ret == BAD_FUNC_ARG) {
                ret = 0;
            } else if (ret == 0) {
                ret = WOLFSSL_FATAL_ERROR;
            }
        }

        printf(resultFmt, ret == 0 ? passed : failed);
    }

    wc_Sha3_512_Free(&sha3);
#endif
    return ret;
} /* END test_wc_Sha3_512_Final */


/*
 *  Testing wc_Sha3_224_Copy()
 */
static int test_wc_Sha3_224_Copy (void)
{
    int         ret = 0;
#if defined(WOLFSSL_SHA3) && !defined(WOLFSSL_NOSHA3_224)
    wc_Sha3        sha3, sha3Cpy;
    const char* msg = "Everyone gets Friday off.";
    word32      msglen = (word32)XSTRLEN(msg);
    byte        hash[WC_SHA3_224_DIGEST_SIZE];
    byte        hashCpy[WC_SHA3_224_DIGEST_SIZE];

    XMEMSET(hash, 0, sizeof(hash));
    XMEMSET(hashCpy, 0, sizeof(hashCpy));

    printf(testingFmt, "wc_Sha3_224_Copy()");

    ret = wc_InitSha3_224(&sha3, HEAP_HINT, devId);
    if (ret != 0) {
        return ret;
    }

    ret = wc_InitSha3_224(&sha3Cpy, HEAP_HINT, devId);
    if (ret != 0) {
    wc_Sha3_224_Free(&sha3);
    return ret;
    }

    ret = wc_Sha3_224_Update(&sha3, (byte*)msg, msglen);

    if (ret == 0) {
        ret = wc_Sha3_224_Copy(&sha3Cpy, &sha3);
        if (ret == 0) {
            ret = wc_Sha3_224_Final(&sha3, hash);
            if (ret == 0) {
                ret = wc_Sha3_224_Final(&sha3Cpy, hashCpy);
            }
        }
        if (ret == 0 && XMEMCMP(hash, hashCpy, sizeof(hash)) != 0) {
            ret = WOLFSSL_FATAL_ERROR;
        }
    }
    /* Test bad args. */
    if (ret == 0) {
        ret = wc_Sha3_224_Copy(NULL, &sha3);
        if (ret == BAD_FUNC_ARG) {
            ret = wc_Sha3_224_Copy(&sha3Cpy, NULL);
        }
        if (ret == BAD_FUNC_ARG) {
            ret = 0;
        } else if (ret == 0) {
            ret = WOLFSSL_FATAL_ERROR;
        }
    }

    printf(resultFmt, ret == 0 ? passed : failed);

#endif
    return ret;

} /* END test_wc_Sha3_224_Copy */



/*
 *  Testing wc_Sha3_256_Copy()
 */
static int test_wc_Sha3_256_Copy (void)
{
    int         ret = 0;
#if defined(WOLFSSL_SHA3) && !defined(WOLFSSL_NOSHA3_256)
    wc_Sha3        sha3, sha3Cpy;
    const char* msg = "Everyone gets Friday off.";
    word32      msglen = (word32)XSTRLEN(msg);
    byte        hash[WC_SHA3_256_DIGEST_SIZE];
    byte        hashCpy[WC_SHA3_256_DIGEST_SIZE];

    XMEMSET(hash, 0, sizeof(hash));
    XMEMSET(hashCpy, 0, sizeof(hashCpy));

    printf(testingFmt, "wc_Sha3_256_Copy()");

    ret = wc_InitSha3_256(&sha3, HEAP_HINT, devId);
    if (ret != 0) {
        return ret;
    }

    ret = wc_InitSha3_256(&sha3Cpy, HEAP_HINT, devId);
    if (ret != 0) {
    wc_Sha3_256_Free(&sha3);
    return ret;
    }

    ret = wc_Sha3_256_Update(&sha3, (byte*)msg, msglen);

    if (ret == 0) {
        ret = wc_Sha3_256_Copy(&sha3Cpy, &sha3);
        if (ret == 0) {
            ret = wc_Sha3_256_Final(&sha3, hash);
            if (ret == 0) {
                ret = wc_Sha3_256_Final(&sha3Cpy, hashCpy);
            }
        }
        if (ret == 0 && XMEMCMP(hash, hashCpy, sizeof(hash)) != 0) {
            ret = WOLFSSL_FATAL_ERROR;
        }
    }
    /* Test bad args. */
    if (ret == 0) {
        ret = wc_Sha3_256_Copy(NULL, &sha3);
        if (ret == BAD_FUNC_ARG) {
            ret = wc_Sha3_256_Copy(&sha3Cpy, NULL);
        }
        if (ret == BAD_FUNC_ARG) {
            ret = 0;
        } else if (ret == 0) {
            ret = WOLFSSL_FATAL_ERROR;
        }
    }

    printf(resultFmt, ret == 0 ? passed : failed);

#endif
    return ret;

} /* END test_wc_Sha3_256_Copy */



/*
 *  Testing wc_Sha3_384_Copy()
 */
static int test_wc_Sha3_384_Copy (void)
{
    int         ret = 0;
#if defined(WOLFSSL_SHA3) && !defined(WOLFSSL_NOSHA3_384)
    wc_Sha3        sha3, sha3Cpy;
    const char* msg = "Everyone gets Friday off.";
    word32      msglen = (word32)XSTRLEN(msg);
    byte        hash[WC_SHA3_384_DIGEST_SIZE];
    byte        hashCpy[WC_SHA3_384_DIGEST_SIZE];

    XMEMSET(hash, 0, sizeof(hash));
    XMEMSET(hashCpy, 0, sizeof(hashCpy));

    printf(testingFmt, "wc_Sha3_384_Copy()");

    ret = wc_InitSha3_384(&sha3, HEAP_HINT, devId);
    if (ret != 0) {
        return ret;
    }

    ret = wc_InitSha3_384(&sha3Cpy, HEAP_HINT, devId);
    if (ret != 0) {
    wc_Sha3_384_Free(&sha3);
    return ret;
    }

    ret = wc_Sha3_384_Update(&sha3, (byte*)msg, msglen);

    if (ret == 0) {
        ret = wc_Sha3_384_Copy(&sha3Cpy, &sha3);
        if (ret == 0) {
            ret = wc_Sha3_384_Final(&sha3, hash);
            if (ret == 0) {
                ret = wc_Sha3_384_Final(&sha3Cpy, hashCpy);
            }
        }
        if (ret == 0 && XMEMCMP(hash, hashCpy, sizeof(hash)) != 0) {
            ret = WOLFSSL_FATAL_ERROR;
        }
    }
    /* Test bad args. */
    if (ret == 0) {
        ret = wc_Sha3_384_Copy(NULL, &sha3);
        if (ret == BAD_FUNC_ARG) {
            ret = wc_Sha3_384_Copy(&sha3Cpy, NULL);
        }
        if (ret == BAD_FUNC_ARG) {
            ret = 0;
        } else if (ret == 0) {
            ret = WOLFSSL_FATAL_ERROR;
        }
    }

    printf(resultFmt, ret == 0 ? passed : failed);

#endif
    return ret;

} /* END test_wc_Sha3_384_Copy */


/*
 *  Testing wc_Sha3_512_Copy()
 */
static int test_wc_Sha3_512_Copy (void)
{
    int         ret = 0;

#if defined(WOLFSSL_SHA3) && !defined(WOLFSSL_NOSHA3_512)
    wc_Sha3        sha3, sha3Cpy;
    const char* msg = "Everyone gets Friday off.";
    word32      msglen = (word32)XSTRLEN(msg);
    byte        hash[WC_SHA3_512_DIGEST_SIZE];
    byte        hashCpy[WC_SHA3_512_DIGEST_SIZE];

    XMEMSET(hash, 0, sizeof(hash));
    XMEMSET(hashCpy, 0, sizeof(hashCpy));


    printf(testingFmt, "wc_Sha3_512_Copy()");

    ret = wc_InitSha3_512(&sha3, HEAP_HINT, devId);
    if (ret != 0) {
        return ret;
    }

    ret = wc_InitSha3_512(&sha3Cpy, HEAP_HINT, devId);
    if (ret != 0) {
    wc_Sha3_512_Free(&sha3);
    return ret;
    }

    ret = wc_Sha3_512_Update(&sha3, (byte*)msg, msglen);

    if (ret == 0) {
        ret = wc_Sha3_512_Copy(&sha3Cpy, &sha3);
        if (ret == 0) {
            ret = wc_Sha3_512_Final(&sha3, hash);
            if (ret == 0) {
                ret = wc_Sha3_512_Final(&sha3Cpy, hashCpy);
            }
        }
        if (ret == 0 && XMEMCMP(hash, hashCpy, sizeof(hash)) != 0) {
            ret = WOLFSSL_FATAL_ERROR;
        }
    }
    /* Test bad args. */
    if (ret == 0) {
        ret = wc_Sha3_512_Copy(NULL, &sha3);
        if (ret == BAD_FUNC_ARG) {
            ret = wc_Sha3_512_Copy(&sha3Cpy, NULL);
        }
        if (ret == BAD_FUNC_ARG) {
            ret = 0;
        } else if (ret == 0) {
            ret = WOLFSSL_FATAL_ERROR;
        }
    }

    printf(resultFmt, ret == 0 ? passed : failed);

#endif
    return ret;

} /* END test_wc_Sha3_512_Copy */




/*
 * unit test for wc_IdeaSetKey()
 */
static int test_wc_IdeaSetKey (void)
{
    int ret = 0;
#ifdef HAVE_IDEA

    Idea        idea;
    const byte  key[] =
    {
        0x37, 0x37, 0x37, 0x37, 0x37, 0x37, 0x37, 0x37,
        0x37, 0x37, 0x37, 0x37, 0x37, 0x37, 0x37, 0x37
    };
    int         flag = 0;

    printf(testingFmt, "wc_IdeaSetKey()");
    /*IV can be NULL, default value is 0*/
    ret = wc_IdeaSetKey(&idea, key, IDEA_KEY_SIZE, NULL, IDEA_ENCRYPTION);
    if (ret == 0) {
        ret = wc_IdeaSetKey(&idea, key, IDEA_KEY_SIZE, NULL, IDEA_DECRYPTION);
    }
    /* Bad args. */
    if (ret == 0) {
        ret = wc_IdeaSetKey(NULL, key, IDEA_KEY_SIZE, NULL, IDEA_ENCRYPTION);
        if (ret != BAD_FUNC_ARG) {
            flag = 1;
        }
        ret = wc_IdeaSetKey(&idea, NULL, IDEA_KEY_SIZE, NULL, IDEA_ENCRYPTION);
        if (ret != BAD_FUNC_ARG) {
            flag = 1;
        }
        ret = wc_IdeaSetKey(&idea, key, IDEA_KEY_SIZE - 1,
                                    NULL, IDEA_ENCRYPTION);
        if (ret != BAD_FUNC_ARG) {
            flag = 1;
        }
        ret = wc_IdeaSetKey(&idea, key, IDEA_KEY_SIZE, NULL, -1);
        if (ret != BAD_FUNC_ARG) {
            flag = 1;
        }
        if (flag == 1) {
            ret = WOLFSSL_FATAL_ERROR;
        } else {
            ret = 0;
        }
    } /* END Test Bad Args. */

    printf(resultFmt, ret == 0 ? passed : failed);

#endif
    return ret;

} /* END test_wc_IdeaSetKey */

/*
 * Unit test for wc_IdeaSetIV()
 */
static int test_wc_IdeaSetIV (void)
{
    int     ret = 0;
#ifdef HAVE_IDEA
    Idea    idea;

    printf(testingFmt, "wc_IdeaSetIV()");

    ret = wc_IdeaSetIV(&idea, NULL);
    /* Test bad args. */
    if (ret == 0) {
        ret = wc_IdeaSetIV(NULL, NULL);
        if (ret == BAD_FUNC_ARG) {
            ret = 0;
        } else {
            ret = WOLFSSL_FATAL_ERROR;
        }
    }

    printf(resultFmt, ret == 0 ? passed : failed);
#endif
    return ret;

} /* END test_wc_IdeaSetIV */

/*
 * Unit test for wc_IdeaCipher()
 */
static int test_wc_IdeaCipher (void)
{
    int     ret = 0;
#ifdef HAVE_IDEA
    Idea        idea;
    const byte  key[] =
    {
        0x2B, 0xD6, 0x45, 0x9F, 0x82, 0xC5, 0xB3, 0x00,
        0x95, 0x2C, 0x49, 0x10, 0x48, 0x81, 0xFF, 0x48
    };
    const byte  plain[] =
    {
        0x37, 0x37, 0x37, 0x37, 0x37, 0x37, 0x37, 0x37
    };
    byte    enc[sizeof(plain)];
    byte    dec[sizeof(enc)];

    printf(testingFmt, "wc_IdeaCipher()");

    ret = wc_IdeaSetKey(&idea, key, IDEA_KEY_SIZE, NULL, IDEA_ENCRYPTION);
    if (ret == 0) {
        ret = wc_IdeaCipher(&idea, enc, plain);
        if (ret != 0) {
            ret = WOLFSSL_FATAL_ERROR;
        }
    }
    if (ret == 0) {
        ret = wc_IdeaSetKey(&idea, key, IDEA_KEY_SIZE, NULL, IDEA_DECRYPTION);
        if (ret == 0) {
            ret = wc_IdeaCipher(&idea, dec, enc);
        }
        if (ret == 0) {
            ret = XMEMCMP(plain, dec, IDEA_BLOCK_SIZE);
        }
        if (ret != 0) {
            ret = WOLFSSL_FATAL_ERROR;
        }
    }
    /* Pass Bad Args. */
    if (ret == 0) {
        ret = wc_IdeaCipher(NULL, enc, dec);
        if (ret == BAD_FUNC_ARG) {
            ret = wc_IdeaCipher(&idea, NULL, dec);
        }
        if (ret == BAD_FUNC_ARG) {
            ret = wc_IdeaCipher(&idea, enc, NULL);
        }
        if (ret == BAD_FUNC_ARG) {
            ret = 0;
        } else {
            ret = WOLFSSL_FATAL_ERROR;
        }
    }

    printf(resultFmt, ret == 0 ? passed : failed);

#endif
    return ret;
} /* END test_wc_IdeaCipher */

/*
 * Unit test for functions wc_IdeaCbcEncrypt and wc_IdeaCbcDecrypt
 */
static int test_wc_IdeaCbcEncyptDecrypt (void)
{
    int         ret = 0;
#ifdef HAVE_IDEA
    Idea        idea;
    const byte  key[] =
    {
        0x37, 0x37, 0x37, 0x37, 0x37, 0x37, 0x37, 0x37,
        0x37, 0x37, 0x37, 0x37, 0x37, 0x37, 0x37, 0x37
    };
    const char* message = "International Data Encryption Algorithm";
    byte        msg_enc[40];
    byte        msg_dec[40];

    printf(testingFmt, "wc_IdeaCbcEncrypt()");

    ret = wc_IdeaSetKey(&idea, key, sizeof(key), NULL, IDEA_ENCRYPTION);
    if (ret == 0) {
        ret = wc_IdeaCbcEncrypt(&idea, msg_enc, (byte *)message,
                                        (word32)XSTRLEN(message) + 1);
    }
    if (ret == 0) {
        ret = wc_IdeaSetKey(&idea, key, sizeof(key), NULL, IDEA_DECRYPTION);
    }
    if (ret == 0) {
        ret = wc_IdeaCbcDecrypt(&idea, msg_dec, msg_enc,
                                            (word32)XSTRLEN(message) + 1);
        if (XMEMCMP(message, msg_dec, (word32)XSTRLEN(message))) {
            ret = WOLFSSL_FATAL_ERROR;
        }
    }

    /* Test bad args. Enc */
    if (ret == 0) {
        ret = wc_IdeaCbcEncrypt(NULL, msg_enc, (byte*)message,
                                    (word32)XSTRLEN(message) + 1);
        if (ret == BAD_FUNC_ARG) {
            ret = wc_IdeaCbcEncrypt(&idea, NULL, (byte*)message,
                                    (word32)XSTRLEN(message) + 1);
        }
        if (ret == BAD_FUNC_ARG) {
            ret = wc_IdeaCbcEncrypt(&idea, msg_enc, NULL,
                                    (word32)XSTRLEN(message) + 1);
        }
        if (ret != BAD_FUNC_ARG) {
            ret = WOLFSSL_FATAL_ERROR;
        } else {
            ret = 0;
        }
    } /* END test bad args ENC  */

    /* Test bad args DEC */
    if (ret == 0) {
        ret = wc_IdeaCbcDecrypt(NULL, msg_dec, msg_enc,
                                    (word32)XSTRLEN(message) + 1);
        if (ret == BAD_FUNC_ARG) {
            ret = wc_IdeaCbcDecrypt(&idea, NULL, msg_enc,
                                    (word32)XSTRLEN(message) + 1);
        }
        if (ret == BAD_FUNC_ARG) {
            ret = wc_IdeaCbcDecrypt(&idea, msg_dec, NULL,
                                    (word32)XSTRLEN(message) + 1);
        }
        if (ret != BAD_FUNC_ARG) {
            ret = WOLFSSL_FATAL_ERROR;
        } else {
            ret = 0;
        }
    }

    printf(resultFmt, ret == 0 ? passed : failed);

#endif
    return ret;

} /* END test_wc_IdeaCbcEncryptDecrypt */


/*
 * Test function for wc_HmacSetKey
 */
static int test_wc_Md5HmacSetKey (void)
{
    int flag = 0;
#if !defined(NO_HMAC) && !defined(NO_MD5)
    Hmac hmac;
    int ret,  times, itr;

    const char* keys[]=
    {
        "\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b",
#ifndef HAVE_FIPS
        "Jefe", /* smaller than minumum FIPS key size */
#endif
        "\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA"
    };

    times = sizeof(keys) / sizeof(char*);
    flag = 0;

    printf(testingFmt, "wc_HmacSetKey() with MD5");

    ret = wc_HmacInit(&hmac, NULL, INVALID_DEVID);
    if (ret != 0)
        return ret;

    for (itr = 0; itr < times; itr++) {
        ret = wc_HmacSetKey(&hmac, WC_MD5, (byte*)keys[itr],
                            (word32)XSTRLEN(keys[itr]));
        if (ret != 0) {
            flag = ret;
        }
    }

    /* Bad args. */
    if (!flag) {
        ret = wc_HmacSetKey(NULL, WC_MD5, (byte*)keys[0],
                                        (word32)XSTRLEN(keys[0]));
        if (ret != BAD_FUNC_ARG) {
            flag = WOLFSSL_FATAL_ERROR;
        }
    }

    if (!flag) {
        ret = wc_HmacSetKey(&hmac, WC_MD5, NULL, (word32)XSTRLEN(keys[0]));
        if (ret != BAD_FUNC_ARG) {
            flag = WOLFSSL_FATAL_ERROR;
        }
    }

    if (!flag) {
        ret = wc_HmacSetKey(&hmac, 20, (byte*)keys[0],
                                        (word32)XSTRLEN(keys[0]));
        if (ret != BAD_FUNC_ARG) {
            flag = WOLFSSL_FATAL_ERROR;
        }
    }

    if (!flag) {
        ret = wc_HmacSetKey(&hmac, WC_MD5, (byte*)keys[0], 0);
#ifdef HAVE_FIPS
        if (ret != HMAC_MIN_KEYLEN_E) {
            flag = WOLFSSL_FATAL_ERROR;
        }
#else
        if (ret != 0) {
            flag = WOLFSSL_FATAL_ERROR;
        }
#endif
    }

    wc_HmacFree(&hmac);

    printf(resultFmt, flag == 0 ? passed : failed);

#endif
    return flag;
} /* END test_wc_Md5HmacSetKey */


/*
 * testing wc_HmacSetKey() on wc_Sha hash.
 */
static int test_wc_ShaHmacSetKey (void)
{

    int flag = 0;
#if !defined(NO_HMAC) && !defined(NO_SHA)
    Hmac hmac;
    int ret, times, itr;

    const char* keys[]=
    {
        "\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b"
                                                                "\x0b\x0b\x0b",
#ifndef HAVE_FIPS
        "Jefe", /* smaller than minumum FIPS key size */
#endif
        "\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA"
                                                                "\xAA\xAA\xAA"
    };

    times = sizeof(keys) / sizeof(char*);
    flag = 0;

    printf(testingFmt, "wc_HmacSetKey() with SHA");

    ret = wc_HmacInit(&hmac, NULL, INVALID_DEVID);
    if (ret != 0)
        return ret;

    for (itr = 0; itr < times; itr++) {
        ret = wc_HmacSetKey(&hmac, WC_SHA, (byte*)keys[itr],
                                        (word32)XSTRLEN(keys[itr]));
        if (ret != 0) {
            flag = ret;
        }
    }

    /* Bad args. */
    if (!flag) {
        ret = wc_HmacSetKey(NULL, WC_SHA, (byte*)keys[0],
                                        (word32)XSTRLEN(keys[0]));
        if (ret != BAD_FUNC_ARG) {
            flag = WOLFSSL_FATAL_ERROR;
        }
    }

    if (!flag) {
        ret = wc_HmacSetKey(&hmac, WC_SHA, NULL, (word32)XSTRLEN(keys[0]));
        if (ret != BAD_FUNC_ARG) {
            flag = WOLFSSL_FATAL_ERROR;
        }
    }

    if (!flag) {
        ret = wc_HmacSetKey(&hmac, 20, (byte*)keys[0],
                                        (word32)XSTRLEN(keys[0]));
        if (ret != BAD_FUNC_ARG) {
            flag = WOLFSSL_FATAL_ERROR;
        }
    }

    if (!flag) {
        ret = wc_HmacSetKey(&hmac, WC_SHA, (byte*)keys[0], 0);
#ifdef HAVE_FIPS
        if (ret != HMAC_MIN_KEYLEN_E) {
            flag = WOLFSSL_FATAL_ERROR;
        }
#else
        if (ret != 0) {
            flag = WOLFSSL_FATAL_ERROR;
        }
#endif
    }

    wc_HmacFree(&hmac);

    printf(resultFmt, flag == 0 ? passed : failed);

#endif
    return flag;
} /* END test_wc_ShaHmacSetKey() */

/*
 * testing wc_HmacSetKey() on Sha224 hash.
 */
static int test_wc_Sha224HmacSetKey (void)
{

    int flag = 0;
#if !defined(NO_HMAC) && defined(WOLFSSL_SHA224)
    Hmac hmac;
    int ret, times, itr;

    const char* keys[]=
    {
        "\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b"
                                                                "\x0b\x0b\x0b",
#ifndef HAVE_FIPS
        "Jefe", /* smaller than minumum FIPS key size */
#endif
        "\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA"
                                                                "\xAA\xAA\xAA"
    };

    times = sizeof(keys) / sizeof(char*);
    flag = 0;

    printf(testingFmt, "wc_HmacSetKey() with SHA 224");

    ret = wc_HmacInit(&hmac, NULL, INVALID_DEVID);
    if (ret != 0)
        return ret;

    for (itr = 0; itr < times; itr++) {
        ret = wc_HmacSetKey(&hmac, WC_SHA224, (byte*)keys[itr],
                                            (word32)XSTRLEN(keys[itr]));
        if (ret != 0) {
            flag = ret;
        }
    }

    /* Bad args. */
    if (!flag) {
        ret = wc_HmacSetKey(NULL, WC_SHA224, (byte*)keys[0],
                                            (word32)XSTRLEN(keys[0]));
        if (ret != BAD_FUNC_ARG) {
            flag = WOLFSSL_FATAL_ERROR;
        }
    }

    if (!flag) {
        ret = wc_HmacSetKey(&hmac, WC_SHA224, NULL, (word32)XSTRLEN(keys[0]));
        if (ret != BAD_FUNC_ARG) {
            flag = WOLFSSL_FATAL_ERROR;
        }
    }

    if (!flag) {
        ret = wc_HmacSetKey(&hmac, 20, (byte*)keys[0],
                                            (word32)XSTRLEN(keys[0]));
        if (ret != BAD_FUNC_ARG) {
            flag = WOLFSSL_FATAL_ERROR;
        }
    }

    if (!flag) {
        ret = wc_HmacSetKey(&hmac, WC_SHA224, (byte*)keys[0], 0);
#ifdef HAVE_FIPS
        if (ret != HMAC_MIN_KEYLEN_E) {
            flag = WOLFSSL_FATAL_ERROR;
        }
#else
        if (ret != 0) {
            flag = WOLFSSL_FATAL_ERROR;
        }
#endif
    }

    wc_HmacFree(&hmac);

    printf(resultFmt, flag == 0 ? passed : failed);

#endif
    return flag;
} /* END test_wc_Sha224HmacSetKey() */

 /*
  * testing wc_HmacSetKey() on Sha256 hash
  */
static int test_wc_Sha256HmacSetKey (void)
{

    int flag = 0;
#if !defined(NO_HMAC) && !defined(NO_SHA256)
    Hmac hmac;
    int ret, times, itr;

    const char* keys[]=
    {
        "\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b"
                                                                "\x0b\x0b\x0b",
#ifndef HAVE_FIPS
        "Jefe", /* smaller than minumum FIPS key size */
#endif
        "\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA"
                                                                "\xAA\xAA\xAA"
    };

    times = sizeof(keys) / sizeof(char*);
    flag = 0;

    printf(testingFmt, "wc_HmacSetKey() with SHA256");

    ret = wc_HmacInit(&hmac, NULL, INVALID_DEVID);
    if (ret != 0)
        return ret;

    for (itr = 0; itr < times; itr++) {
        ret = wc_HmacSetKey(&hmac, WC_SHA256, (byte*)keys[itr],
                                            (word32)XSTRLEN(keys[itr]));
        if (ret != 0) {
            flag = ret;
        }
    }

    /* Bad args. */
    if (!flag) {
        ret = wc_HmacSetKey(NULL, WC_SHA256, (byte*)keys[0],
                                            (word32)XSTRLEN(keys[0]));
        if (ret != BAD_FUNC_ARG) {
            flag = WOLFSSL_FATAL_ERROR;
        }
    }

    if (!flag) {
        ret = wc_HmacSetKey(&hmac, WC_SHA256, NULL, (word32)XSTRLEN(keys[0]));
        if (ret != BAD_FUNC_ARG) {
            flag = WOLFSSL_FATAL_ERROR;
        }
    }

    if (!flag) {
        ret = wc_HmacSetKey(&hmac, 20, (byte*)keys[0],
                                            (word32)XSTRLEN(keys[0]));
        if (ret != BAD_FUNC_ARG) {
            flag = WOLFSSL_FATAL_ERROR;
        }
    }

    if (!flag) {
        ret = wc_HmacSetKey(&hmac, WC_SHA256, (byte*)keys[0], 0);
#ifdef HAVE_FIPS
        if (ret != HMAC_MIN_KEYLEN_E) {
            flag = WOLFSSL_FATAL_ERROR;
        }
#else
        if (ret != 0) {
            flag = WOLFSSL_FATAL_ERROR;
        }
#endif
    }

    wc_HmacFree(&hmac);

    printf(resultFmt, flag == 0 ? passed : failed);

#endif
    return flag;
} /* END test_wc_Sha256HmacSetKey() */


/*
 * testing wc_HmacSetKey on Sha384 hash.
 */
static int test_wc_Sha384HmacSetKey (void)
{
    int flag = 0;
#if !defined(NO_HMAC) && defined(WOLFSSL_SHA384)
    Hmac hmac;
    int ret, times, itr;

    const char* keys[]=
    {
        "\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b"
                                                                "\x0b\x0b\x0b",
#ifndef HAVE_FIPS
        "Jefe", /* smaller than minumum FIPS key size */
#endif
        "\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA"
                                                                "\xAA\xAA\xAA"
    };

    times = sizeof(keys) / sizeof(char*);
    flag = 0;

    printf(testingFmt, "wc_HmacSetKey() with SHA384");

    ret = wc_HmacInit(&hmac, NULL, INVALID_DEVID);
    if (ret != 0)
        return ret;

    for (itr = 0; itr < times; itr++) {
        ret = wc_HmacSetKey(&hmac, WC_SHA384, (byte*)keys[itr],
                                            (word32)XSTRLEN(keys[itr]));
        if (ret != 0) {
            flag = ret;
        }
    }

    /* Bad args. */
    if (!flag) {
        ret = wc_HmacSetKey(NULL, WC_SHA384, (byte*)keys[0],
                                            (word32)XSTRLEN(keys[0]));
        if (ret != BAD_FUNC_ARG) {
            flag = WOLFSSL_FATAL_ERROR;
        }
    }

    if (!flag) {
        ret = wc_HmacSetKey(&hmac, WC_SHA384, NULL, (word32)XSTRLEN(keys[0]));
        if (ret != BAD_FUNC_ARG) {
            flag = WOLFSSL_FATAL_ERROR;
        }
    }

    if (!flag) {
        ret = wc_HmacSetKey(&hmac, 20, (byte*)keys[0],
                                            (word32)XSTRLEN(keys[0]));
        if (ret != BAD_FUNC_ARG) {
            flag = WOLFSSL_FATAL_ERROR;
        }
    }

    if (!flag) {
        ret = wc_HmacSetKey(&hmac, WC_SHA384, (byte*)keys[0], 0);
#ifdef HAVE_FIPS
        if (ret != HMAC_MIN_KEYLEN_E) {
            flag = WOLFSSL_FATAL_ERROR;
        }
#else
        if (ret != 0) {
            flag = WOLFSSL_FATAL_ERROR;
        }
#endif
    }

    wc_HmacFree(&hmac);

    printf(resultFmt, flag == 0 ? passed : failed);

#endif
    return flag;
} /* END test_wc_Sha384HmacSetKey() */


/*
 * testing wc_HmacUpdate on wc_Md5 hash.
 */
static int test_wc_Md5HmacUpdate (void)
{
    int flag = 0;
#if !defined(NO_HMAC) && !defined(NO_MD5)
    Hmac hmac;
    testVector a, b;
    int ret;
#ifdef HAVE_FIPS
    const char* keys =
        "\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b";
#else
    const char* keys = "Jefe";
#endif

    a.input = "what do ya want for nothing?";
    a.inLen  = XSTRLEN(a.input);

    b.input = "Hi There";
    b.inLen = XSTRLEN(b.input);

    flag = 0;

    printf(testingFmt, "wc_HmacUpdate() with MD5");

    ret = wc_HmacInit(&hmac, NULL, INVALID_DEVID);
    if (ret != 0)
        return ret;

    ret = wc_HmacSetKey(&hmac, WC_MD5, (byte*)keys, (word32)XSTRLEN(keys));
    if (ret != 0) {
        flag = ret;
    }

    if (!flag) {
        ret = wc_HmacUpdate(&hmac, (byte*)b.input, (word32)b.inLen);
        if (ret != 0) {
            flag = ret;
        }
    }
    /* Update Hmac. */
    if (!flag) {
        ret = wc_HmacUpdate(&hmac, (byte*)a.input, (word32)a.inLen);
        if (ret != 0) {
            flag = ret;
        }
    }

    /* Test bad args. */
    if (!flag) {
        ret = wc_HmacUpdate(NULL, (byte*)a.input, (word32)a.inLen);
        if (ret != BAD_FUNC_ARG) {
            flag = WOLFSSL_FATAL_ERROR;
        }
    }

    if (!flag) {
        ret = wc_HmacUpdate(&hmac, NULL, (word32)a.inLen);
        if (ret != BAD_FUNC_ARG) {
            flag = WOLFSSL_FATAL_ERROR;
        }
    }

    if (!flag) {
        ret = wc_HmacUpdate(&hmac, (byte*)a.input, 0);
        if (ret != 0) {
            flag = ret;
        }
    }

    wc_HmacFree(&hmac);

    printf(resultFmt, flag == 0 ? passed : failed);

#endif
    return flag;
} /* END test_wc_Md5HmacUpdate */

/*
 * testing wc_HmacUpdate on SHA hash.
 */
static int test_wc_ShaHmacUpdate (void)
{
    int flag = 0;
#if !defined(NO_HMAC) && !defined(NO_SHA)
    Hmac hmac;
    testVector a, b;
    int ret;
#ifdef HAVE_FIPS
    const char* keys =
        "\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b";
#else
    const char* keys = "Jefe";
#endif

    a.input = "what do ya want for nothing?";
    a.inLen  = XSTRLEN(a.input);

    b.input = "Hi There";
    b.inLen = XSTRLEN(b.input);

    flag = 0;

    printf(testingFmt, "wc_HmacUpdate() with SHA");

    ret = wc_HmacInit(&hmac, NULL, INVALID_DEVID);
    if (ret != 0)
        return ret;

    ret = wc_HmacSetKey(&hmac, WC_SHA, (byte*)keys, (word32)XSTRLEN(keys));
    if (ret != 0) {
        flag = ret;
    }

    if (!flag) {
        ret = wc_HmacUpdate(&hmac, (byte*)b.input, (word32)b.inLen);
        if (ret != 0) {
            flag = ret;
        }
    }
    /* Update Hmac. */
    if (!flag) {
        ret = wc_HmacUpdate(&hmac, (byte*)a.input, (word32)a.inLen);
        if (ret != 0) {
            flag = ret;
        }
    }

    /* Test bad args. */
    if (!flag) {
        ret = wc_HmacUpdate(NULL, (byte*)a.input, (word32)a.inLen);
        if (ret != BAD_FUNC_ARG) {
            flag = WOLFSSL_FATAL_ERROR;
        }
    }

    if (!flag) {
        ret = wc_HmacUpdate(&hmac, NULL, (word32)a.inLen);
        if (ret != BAD_FUNC_ARG) {
            flag = WOLFSSL_FATAL_ERROR;
        }
    }

    if (!flag) {
        ret = wc_HmacUpdate(&hmac, (byte*)a.input, 0);
        if (ret != 0) {
            flag = ret;
        }
    }

    wc_HmacFree(&hmac);

    printf(resultFmt, flag == 0 ? passed : failed);

#endif
    return flag;
} /* END test_wc_ShaHmacUpdate */

/*
 * testing wc_HmacUpdate on SHA224 hash.
 */
static int test_wc_Sha224HmacUpdate (void)
{
    int flag = 0;
#if !defined(NO_HMAC) && defined(WOLFSSL_SHA224)
    Hmac hmac;
    testVector a, b;
    int ret;
#ifdef HAVE_FIPS
    const char* keys =
        "\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b";
#else
    const char* keys = "Jefe";
#endif

    a.input = "what do ya want for nothing?";
    a.inLen  = XSTRLEN(a.input);

    b.input = "Hi There";
    b.inLen = XSTRLEN(b.input);

    flag = 0;

    printf(testingFmt, "wc_HmacUpdate() with SHA224");

    ret = wc_HmacInit(&hmac, NULL, INVALID_DEVID);
    if (ret != 0)
        return ret;

    ret = wc_HmacSetKey(&hmac, WC_SHA224, (byte*)keys, (word32)XSTRLEN(keys));
    if (ret != 0) {
        flag = ret;
    }

    if (!flag) {
        ret = wc_HmacUpdate(&hmac, (byte*)b.input, (word32)b.inLen);
        if (ret != 0) {
            flag = ret;
        }
    }
    /* Update Hmac. */
    if (!flag) {
        ret = wc_HmacUpdate(&hmac, (byte*)a.input, (word32)a.inLen);
        if (ret != 0) {
            flag = ret;
        }
    }

    /* Test bad args. */
    if (!flag) {
        ret = wc_HmacUpdate(NULL, (byte*)a.input, (word32)a.inLen);
        if (ret != BAD_FUNC_ARG) {
            flag = WOLFSSL_FATAL_ERROR;
        }
    }

    if (!flag) {
        ret = wc_HmacUpdate(&hmac, NULL, (word32)a.inLen);
        if (ret != BAD_FUNC_ARG) {
            flag = WOLFSSL_FATAL_ERROR;
        }
    }

    if (!flag) {
        ret = wc_HmacUpdate(&hmac, (byte*)a.input, 0);
        if (ret != 0) {
            flag = ret;
        }
    }

    wc_HmacFree(&hmac);

    printf(resultFmt, flag == 0 ? passed : failed);

#endif
    return flag;
} /* END test_wc_Sha224HmacUpdate */

/*
 * testing wc_HmacUpdate on SHA256 hash.
 */
static int test_wc_Sha256HmacUpdate (void)
{
    int flag = 0;
#if !defined(NO_HMAC) && !defined(NO_SHA256)
    Hmac hmac;
    testVector a, b;
    int ret;
#ifdef HAVE_FIPS
    const char* keys =
        "\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b";
#else
    const char* keys = "Jefe";
#endif

    a.input = "what do ya want for nothing?";
    a.inLen  = XSTRLEN(a.input);

    b.input = "Hi There";
    b.inLen = XSTRLEN(b.input);

    flag = 0;

    printf(testingFmt, "wc_HmacUpdate() with WC_SHA256");

    ret = wc_HmacInit(&hmac, NULL, INVALID_DEVID);
    if (ret != 0)
        return ret;

    ret = wc_HmacSetKey(&hmac, WC_SHA256, (byte*)keys, (word32)XSTRLEN(keys));
    if (ret != 0) {
        flag = ret;
    }

    if (!flag) {
        ret = wc_HmacUpdate(&hmac, (byte*)b.input, (word32)b.inLen);
        if (ret != 0) {
            flag = ret;
        }
    }
    /* Update Hmac. */
    if (!flag) {
        ret = wc_HmacUpdate(&hmac, (byte*)a.input, (word32)a.inLen);
        if (ret != 0) {
            flag = ret;
        }
    }

    /* Test bad args. */
    if (!flag) {
        ret = wc_HmacUpdate(NULL, (byte*)a.input, (word32)a.inLen);
        if (ret != BAD_FUNC_ARG) {
            flag = WOLFSSL_FATAL_ERROR;
        }
    }

    if (!flag) {
        ret = wc_HmacUpdate(&hmac, NULL, (word32)a.inLen);
        if (ret != BAD_FUNC_ARG) {
            flag = WOLFSSL_FATAL_ERROR;
        }
    }

    if (!flag) {
        ret = wc_HmacUpdate(&hmac, (byte*)a.input, 0);
        if (ret != 0) {
            flag = ret;
        }
    }

    wc_HmacFree(&hmac);

    printf(resultFmt, flag == 0 ? passed : failed);

#endif
    return flag;
} /* END test_wc_Sha256HmacUpdate */

/*
 * testing wc_HmacUpdate on SHA384  hash.
 */
static int test_wc_Sha384HmacUpdate (void)
{
    int flag = 0;
#if !defined(NO_HMAC) && defined(WOLFSSL_SHA384)
    Hmac hmac;
    testVector a, b;
    int ret;
#ifdef HAVE_FIPS
    const char* keys =
        "\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b";
#else
    const char* keys = "Jefe";
#endif

    a.input = "what do ya want for nothing?";
    a.inLen  = XSTRLEN(a.input);

    b.input = "Hi There";
    b.inLen = XSTRLEN(b.input);

    flag = 0;

    printf(testingFmt, "wc_HmacUpdate() with SHA384");

    ret = wc_HmacInit(&hmac, NULL, INVALID_DEVID);
    if (ret != 0)
        return ret;

    ret = wc_HmacSetKey(&hmac, WC_SHA384, (byte*)keys, (word32)XSTRLEN(keys));
    if (ret != 0) {
        flag = ret;
    }

    if (!flag) {
        ret = wc_HmacUpdate(&hmac, (byte*)b.input, (word32)b.inLen);
        if (ret != 0) {
            flag = ret;
        }
    }
    /* Update Hmac. */
    if (!flag) {
        ret = wc_HmacUpdate(&hmac, (byte*)a.input, (word32)a.inLen);
        if (ret != 0) {
            flag = ret;
        }
    }

    /* Test bad args. */
    if (!flag) {
        ret = wc_HmacUpdate(NULL, (byte*)a.input, (word32)a.inLen);
        if (ret != BAD_FUNC_ARG) {
            flag = WOLFSSL_FATAL_ERROR;
        }
    }

    if (!flag) {
        ret = wc_HmacUpdate(&hmac, NULL, (word32)a.inLen);
        if (ret != BAD_FUNC_ARG) {
            flag = WOLFSSL_FATAL_ERROR;
        }
    }

    if (!flag) {
        ret = wc_HmacUpdate(&hmac, (byte*)a.input, 0);
        if (ret != 0) {
            flag = ret;
        }
    }

    wc_HmacFree(&hmac);

    printf(resultFmt, flag == 0 ? passed : failed);

#endif
    return flag;
} /* END test_wc_Sha384HmacUpdate */

/*
 * Testing wc_HmacFinal() with MD5
 */

static int test_wc_Md5HmacFinal (void)
{
    int flag = 0;
#if !defined(NO_HMAC) && !defined(NO_MD5)
    Hmac hmac;
    byte hash[WC_MD5_DIGEST_SIZE];
    testVector a;
    int ret;
    const char* key;

    key = "\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b";
    a.input = "Hi There";
    a.output = "\x92\x94\x72\x7a\x36\x38\xbb\x1c\x13\xf4\x8e\xf8\x15\x8b\xfc"
               "\x9d";
    a.inLen  = XSTRLEN(a.input);
    a.outLen = XSTRLEN(a.output);

    flag = 0;

    printf(testingFmt, "wc_HmacFinal() with MD5");

    ret = wc_HmacInit(&hmac, NULL, INVALID_DEVID);
    if (ret != 0)
        return ret;

    ret = wc_HmacSetKey(&hmac, WC_MD5, (byte*)key, (word32)XSTRLEN(key));
    if (ret != 0) {
        flag = ret;
    }

    if (!flag) {
        ret = wc_HmacUpdate(&hmac, (byte*)a.input, (word32)a.inLen);
        if (ret != 0) {
            flag = ret;
        }
    }

    if (!flag) {
        ret = wc_HmacFinal(&hmac, hash);
        if (ret != 0) {
            flag = ret;
        }
    }

    if (!flag) {
        if (XMEMCMP(hash, a.output, WC_MD5_DIGEST_SIZE) != 0) {
            flag = WOLFSSL_FATAL_ERROR;
        }
    }

    /* Try bad parameters. */
    if (!flag) {
        ret = wc_HmacFinal(NULL, hash);
        if (ret != BAD_FUNC_ARG) {
            flag = WOLFSSL_FATAL_ERROR;
        }
    }

#ifndef HAVE_FIPS
    if (!flag) {
        ret = wc_HmacFinal(&hmac, NULL);
        if (ret != BAD_FUNC_ARG) {
            flag = WOLFSSL_FATAL_ERROR;
        }
    }
#endif

    wc_HmacFree(&hmac);

    printf(resultFmt, flag == 0 ? passed : failed);

#endif
    return flag;

} /* END test_wc_Md5HmacFinal */

/*
 * Testing wc_HmacFinal() with SHA
 */
static int test_wc_ShaHmacFinal (void)
{
    int flag = 0;
#if !defined(NO_HMAC) && !defined(NO_SHA)
    Hmac hmac;
    byte hash[WC_SHA_DIGEST_SIZE];
    testVector a;
    int ret;
    const char* key;

    key = "\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b"
                                                                "\x0b\x0b\x0b";
    a.input = "Hi There";
    a.output = "\xb6\x17\x31\x86\x55\x05\x72\x64\xe2\x8b\xc0\xb6\xfb\x37\x8c"
               "\x8e\xf1\x46\xbe\x00";
    a.inLen  = XSTRLEN(a.input);
    a.outLen = XSTRLEN(a.output);

    flag = 0;

    printf(testingFmt, "wc_HmacFinal() with SHA");

    ret = wc_HmacInit(&hmac, NULL, INVALID_DEVID);
    if (ret != 0)
        return ret;

    ret = wc_HmacSetKey(&hmac, WC_SHA, (byte*)key, (word32)XSTRLEN(key));
    if (ret != 0) {
        flag = ret;
    }

    if (!flag) {
        ret = wc_HmacUpdate(&hmac, (byte*)a.input, (word32)a.inLen);
        if (ret != 0) {
            flag = ret;
        }
    }

    if (!flag) {
        ret = wc_HmacFinal(&hmac, hash);
        if (ret != 0) {
            flag = ret;
        }
    }

    if (!flag) {
        if (XMEMCMP(hash, a.output, WC_SHA_DIGEST_SIZE) != 0) {
            flag = WOLFSSL_FATAL_ERROR;
        }
    }

    /* Try bad parameters. */
    if (!flag) {
        ret = wc_HmacFinal(NULL, hash);
        if (ret != BAD_FUNC_ARG) {
            flag = WOLFSSL_FATAL_ERROR;
        }
    }

#ifndef HAVE_FIPS
    if (!flag) {
        ret = wc_HmacFinal(&hmac, NULL);
        if (ret != BAD_FUNC_ARG) {
            flag = WOLFSSL_FATAL_ERROR;
        }
    }
#endif

    wc_HmacFree(&hmac);

    printf(resultFmt, flag == 0 ? passed : failed);

#endif
    return flag;

} /* END test_wc_ShaHmacFinal */


/*
 * Testing wc_HmacFinal() with SHA224
 */
static int test_wc_Sha224HmacFinal (void)
{
    int flag = 0;
#if !defined(NO_HMAC) && defined(WOLFSSL_SHA224)
    Hmac hmac;
    byte hash[WC_SHA224_DIGEST_SIZE];
    testVector a;
    int ret;
    const char* key;

    key = "\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b"
                                                                "\x0b\x0b\x0b";
    a.input = "Hi There";
    a.output = "\x89\x6f\xb1\x12\x8a\xbb\xdf\x19\x68\x32\x10\x7c\xd4\x9d\xf3"
               "\x3f\x47\xb4\xb1\x16\x99\x12\xba\x4f\x53\x68\x4b\x22";
    a.inLen  = XSTRLEN(a.input);
    a.outLen = XSTRLEN(a.output);

    flag = 0;

    printf(testingFmt, "wc_HmacFinal() with SHA224");

    ret = wc_HmacInit(&hmac, NULL, INVALID_DEVID);
    if (ret != 0)
        return ret;

    ret = wc_HmacSetKey(&hmac, WC_SHA224, (byte*)key, (word32)XSTRLEN(key));
    if (ret != 0) {
        flag = ret;
    }

    if (!flag) {
        ret = wc_HmacUpdate(&hmac, (byte*)a.input, (word32)a.inLen);
        if (ret != 0) {
            flag = ret;
        }
    }

    if (!flag) {
        ret = wc_HmacFinal(&hmac, hash);
        if (ret != 0) {
            flag = ret;
        }
    }

    if (!flag) {
        if (XMEMCMP(hash, a.output, WC_SHA224_DIGEST_SIZE) != 0) {
            flag = WOLFSSL_FATAL_ERROR;
        }
    }

    /* Try bad parameters. */
    if (!flag) {
        ret = wc_HmacFinal(NULL, hash);
        if (ret != BAD_FUNC_ARG) {
            flag = WOLFSSL_FATAL_ERROR;
        }
    }

#ifndef HAVE_FIPS
    if (!flag) {
        ret = wc_HmacFinal(&hmac, NULL);
        if (ret != BAD_FUNC_ARG) {
            flag = WOLFSSL_FATAL_ERROR;
        }
    }
#endif

    wc_HmacFree(&hmac);

    printf(resultFmt, flag == 0 ? passed : failed);

#endif
    return flag;
} /* END test_wc_Sha224HmacFinal */

/*
 * Testing wc_HmacFinal() with SHA256
 */
static int test_wc_Sha256HmacFinal (void)
{
    int flag = 0;
#if !defined(NO_HMAC) && !defined(NO_SHA256)
    Hmac hmac;
    byte hash[WC_SHA256_DIGEST_SIZE];
    testVector a;
    int ret;
    const char* key;

    key = "\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b"
                                                                "\x0b\x0b\x0b";
    a.input = "Hi There";
    a.output = "\xb0\x34\x4c\x61\xd8\xdb\x38\x53\x5c\xa8\xaf\xce\xaf\x0b\xf1"
               "\x2b\x88\x1d\xc2\x00\xc9\x83\x3d\xa7\x26\xe9\x37\x6c\x2e\x32"
               "\xcf\xf7";
    a.inLen  = XSTRLEN(a.input);
    a.outLen = XSTRLEN(a.output);

    flag = 0;

    printf(testingFmt, "wc_HmacFinal() with WC_SHA256");

    ret = wc_HmacInit(&hmac, NULL, INVALID_DEVID);
    if (ret != 0)
        return ret;

    ret = wc_HmacSetKey(&hmac, WC_SHA256, (byte*)key, (word32)XSTRLEN(key));
    if (ret != 0) {
        flag = ret;
    }

    if (!flag) {
        ret = wc_HmacUpdate(&hmac, (byte*)a.input, (word32)a.inLen);
        if (ret != 0) {
            flag = ret;
        }
    }

    if (!flag) {
        ret = wc_HmacFinal(&hmac, hash);
        if (ret != 0) {
            flag = ret;
        }
    }

    if (!flag) {
        if (XMEMCMP(hash, a.output, WC_SHA256_DIGEST_SIZE) != 0) {
            flag = WOLFSSL_FATAL_ERROR;
        }
    }

    /* Try bad parameters. */
    if (!flag) {
        ret = wc_HmacFinal(NULL, hash);
        if (ret != BAD_FUNC_ARG) {
            flag = WOLFSSL_FATAL_ERROR;
        }
    }

#ifndef HAVE_FIPS
    if (!flag) {
        ret = wc_HmacFinal(&hmac, NULL);
        if (ret != BAD_FUNC_ARG) {
            flag = WOLFSSL_FATAL_ERROR;
        }
    }
#endif

    wc_HmacFree(&hmac);

    printf(resultFmt, flag == 0 ? passed : failed);

#endif
    return flag;
} /* END test_wc_Sha256HmacFinal */

/*
 * Testing wc_HmacFinal() with SHA384
 */
static int test_wc_Sha384HmacFinal (void)
{
    int flag = 0;
#if !defined(NO_HMAC) && defined(WOLFSSL_SHA384)
    Hmac hmac;
    byte hash[WC_SHA384_DIGEST_SIZE];
    testVector a;
    int ret;
    const char* key;

    key = "\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b"
                                                                "\x0b\x0b\x0b";
    a.input = "Hi There";
    a.output = "\xaf\xd0\x39\x44\xd8\x48\x95\x62\x6b\x08\x25\xf4\xab\x46\x90"
               "\x7f\x15\xf9\xda\xdb\xe4\x10\x1e\xc6\x82\xaa\x03\x4c\x7c\xeb"
               "\xc5\x9c\xfa\xea\x9e\xa9\x07\x6e\xde\x7f\x4a\xf1\x52\xe8\xb2"
               "\xfa\x9c\xb6";
    a.inLen  = XSTRLEN(a.input);
    a.outLen = XSTRLEN(a.output);

    flag = 0;

    printf(testingFmt, "wc_HmacFinal() with SHA384");

    ret = wc_HmacInit(&hmac, NULL, INVALID_DEVID);
    if (ret != 0)
        return ret;

    ret = wc_HmacSetKey(&hmac, WC_SHA384, (byte*)key, (word32)XSTRLEN(key));
    if (ret != 0) {
        flag = ret;
    }

    if (!flag) {
        ret = wc_HmacUpdate(&hmac, (byte*)a.input, (word32)a.inLen);
        if (ret != 0) {
            flag = ret;
        }
    }

    if (!flag) {
        ret = wc_HmacFinal(&hmac, hash);
        if (ret != 0) {
            flag = ret;
        }
    }

    if (!flag) {
        if (XMEMCMP(hash, a.output, WC_SHA384_DIGEST_SIZE) != 0) {
            flag = WOLFSSL_FATAL_ERROR;
        }
    }

    /* Try bad parameters. */
    if (!flag) {
        ret = wc_HmacFinal(NULL, hash);
        if (ret != BAD_FUNC_ARG) {
            flag = WOLFSSL_FATAL_ERROR;
        }
    }
#ifndef HAVE_FIPS
    if (!flag) {
        ret = wc_HmacFinal(&hmac, NULL);
        if (ret != BAD_FUNC_ARG) {
            flag = WOLFSSL_FATAL_ERROR;
        }
    }
#endif

    wc_HmacFree(&hmac);

    printf(resultFmt, flag == 0 ? passed : failed);

#endif
    return flag;
} /* END test_wc_Sha384HmacFinal */



/*
 * Testing wc_InitCmac()
 */
static int test_wc_InitCmac (void)
{
    int         ret = 0;

#if defined(WOLFSSL_CMAC) && !defined(NO_AES)
    Cmac        cmac1, cmac2, cmac3;
    /* AES 128 key. */
    byte        key1[] = "\x01\x02\x03\x04\x05\x06\x07\x08"
                         "\x09\x10\x11\x12\x13\x14\x15\x16";
    /* AES 192 key. */
    byte        key2[] = "\x01\x02\x03\x04\x05\x06\x07\x08"
                         "\x09\x01\x11\x12\x13\x14\x15\x16"
                         "\x01\x02\x03\x04\x05\x06\x07\x08";

    /* AES 256 key. */
    byte        key3[] = "\x01\x02\x03\x04\x05\x06\x07\x08"
                         "\x09\x01\x11\x12\x13\x14\x15\x16"
                         "\x01\x02\x03\x04\x05\x06\x07\x08"
                         "\x09\x01\x11\x12\x13\x14\x15\x16";

    word32      key1Sz = (word32)sizeof(key1) - 1;
    word32      key2Sz = (word32)sizeof(key2) - 1;
    word32      key3Sz = (word32)sizeof(key3) - 1;
    int         type   = WC_CMAC_AES;

    printf(testingFmt, "wc_InitCmac()");

#ifdef WOLFSSL_AES_128
    ret = wc_InitCmac(&cmac1, key1, key1Sz, type, NULL);
#endif
#ifdef WOLFSSL_AES_192
    if (ret == 0)
        ret = wc_InitCmac(&cmac2, key2, key2Sz, type, NULL);
#endif
#ifdef WOLFSSL_AES_256
    if (ret == 0)
        ret = wc_InitCmac(&cmac3, key3, key3Sz, type, NULL);
#endif

    /* Test bad args. */
    if (ret == 0) {
        ret = wc_InitCmac(NULL, key3, key3Sz, type, NULL);
        if (ret == BAD_FUNC_ARG) {
            ret = wc_InitCmac(&cmac3, NULL, key3Sz, type, NULL);
        }
        if (ret == BAD_FUNC_ARG) {
            ret = wc_InitCmac(&cmac3, key3, 0, type, NULL);
        }
        if (ret == BAD_FUNC_ARG) {
            ret = wc_InitCmac(&cmac3, key3, key3Sz, 0, NULL);
        }
        if (ret == BAD_FUNC_ARG) {
            ret = 0;
        } else {
            ret = SSL_FATAL_ERROR;
        }
    }

    (void)key1;
    (void)key1Sz;
    (void)key2;
    (void)key2Sz;
    (void)cmac1;
    (void)cmac2;

    printf(resultFmt, ret == 0 ? passed : failed);

#endif
    return ret;

} /* END test_wc_InitCmac */


/*
 * Testing wc_CmacUpdate()
 */
static int test_wc_CmacUpdate (void)
{
    int         ret = 0;

#if defined(WOLFSSL_CMAC) && !defined(NO_AES) && defined(WOLFSSL_AES_128)
    Cmac        cmac;
    byte        key[] =
    {
        0x64, 0x4c, 0xbf, 0x12, 0x85, 0x9d, 0xf0, 0x55,
        0x7e, 0xa9, 0x1f, 0x08, 0xe0, 0x51, 0xff, 0x27
    };
    byte        in[] = "\xe2\xb4\xb6\xf9\x48\x44\x02\x64"
                       "\x5c\x47\x80\x9e\xd5\xa8\x3a\x17"
                       "\xb3\x78\xcf\x85\x22\x41\x74\xd9"
                       "\xa0\x97\x39\x71\x62\xf1\x8e\x8f"
                       "\xf4";

    word32      inSz  = (word32)sizeof(in) - 1;
    word32      keySz = (word32)sizeof(key);
    int         type  = WC_CMAC_AES;

    ret = wc_InitCmac(&cmac, key, keySz, type, NULL);
    if (ret != 0) {
        return ret;
    }

    printf(testingFmt, "wc_CmacUpdate()");

    ret = wc_CmacUpdate(&cmac, in, inSz);

    /* Test bad args. */
    if (ret == 0) {
        ret = wc_CmacUpdate(NULL, in, inSz);
        if (ret == BAD_FUNC_ARG) {
            ret = wc_CmacUpdate(&cmac, NULL, 30);
        }
        if (ret == BAD_FUNC_ARG) {
            ret = 0;
        } else if (ret == 0) {
            ret = SSL_FATAL_ERROR;
        }
    }

    printf(resultFmt, ret == 0 ? passed : failed);

#endif
    return ret;

} /* END test_wc_CmacUpdate */


/*
 * Testing wc_CmacFinal()
 */
static int test_wc_CmacFinal (void)
{
    int         ret = 0;

#if defined(WOLFSSL_CMAC) && !defined(NO_AES) && defined(WOLFSSL_AES_128)
    Cmac        cmac;
    byte        key[] =
    {
        0x64, 0x4c, 0xbf, 0x12, 0x85, 0x9d, 0xf0, 0x55,
        0x7e, 0xa9, 0x1f, 0x08, 0xe0, 0x51, 0xff, 0x27
    };
    byte        msg[] =
    {
        0xe2, 0xb4, 0xb6, 0xf9, 0x48, 0x44, 0x02, 0x64,
        0x5c, 0x47, 0x80, 0x9e, 0xd5, 0xa8, 0x3a, 0x17,
        0xb3, 0x78, 0xcf, 0x85, 0x22, 0x41, 0x74, 0xd9,
        0xa0, 0x97, 0x39, 0x71, 0x62, 0xf1, 0x8e, 0x8f,
        0xf4
    };
    /* Test vectors from CMACGenAES128.rsp from
     * http://csrc.nist.gov/groups/STM/cavp/block-cipher-modes.html#cmac
     * Per RFC4493 truncation of lsb is possible.
     */
    byte        expMac[] =
    {
        0x4e, 0x6e, 0xc5, 0x6f, 0xf9, 0x5d, 0x0e, 0xae,
        0x1c, 0xf8, 0x3e, 0xfc, 0xf4, 0x4b, 0xeb
    };
    byte        mac[AES_BLOCK_SIZE];
    word32      msgSz    = (word32)sizeof(msg);
    word32      keySz    = (word32)sizeof(key);
    word32      macSz    = sizeof(mac);
    word32      badMacSz = 17;
    int         expMacSz = sizeof(expMac);
    int         type     = WC_CMAC_AES;

    XMEMSET(mac, 0, macSz);

    ret = wc_InitCmac(&cmac, key, keySz, type, NULL);
    if (ret != 0) {
        return ret;
    }
    ret = wc_CmacUpdate(&cmac, msg, msgSz);

    printf(testingFmt, "wc_CmacFinal()");
    if (ret == 0) {
        ret = wc_CmacFinal(&cmac, mac, &macSz);
        if (ret == 0 && XMEMCMP(mac, expMac, expMacSz) != 0) {
            ret = SSL_FATAL_ERROR;
        }
        /* Pass in bad args. */
        if (ret == 0) {
            ret = wc_CmacFinal(NULL, mac, &macSz);
            if (ret == BAD_FUNC_ARG) {
                ret = wc_CmacFinal(&cmac, NULL, &macSz);
            }
            if (ret == BAD_FUNC_ARG) {
                ret = wc_CmacFinal(&cmac, mac, &badMacSz);
                if (ret == BUFFER_E) {
                    ret = 0;
                }
            } else if (ret == 0) {
                ret = SSL_FATAL_ERROR;
            }
        }
    }
    printf(resultFmt, ret == 0 ? passed : failed);

#endif
    return ret;

} /* END test_wc_CmacFinal */


/*
 * Testing wc_AesCmacGenerate() && wc_AesCmacVerify()
 */
static int test_wc_AesCmacGenerate (void)
{
    int         ret = 0;
#if defined(WOLFSSL_CMAC) && !defined(NO_AES) && defined(WOLFSSL_AES_128)
    Cmac        cmac;
    byte        key[] =
    {
        0x26, 0xef, 0x8b, 0x40, 0x34, 0x11, 0x7d, 0x9e,
        0xbe, 0xc0, 0xc7, 0xfc, 0x31, 0x08, 0x54, 0x69
    };
    byte        msg[]    = "\x18\x90\x49\xef\xfd\x7c\xf9\xc8"
                           "\xf3\x59\x65\xbc\xb0\x97\x8f\xd4";
    byte        expMac[] = "\x29\x5f\x2f\x71\xfc\x58\xe6\xf6"
                           "\x3d\x32\x65\x4c\x66\x23\xc5";
    byte        mac[AES_BLOCK_SIZE];
    word32      keySz    = sizeof(key);
    word32      macSz    = sizeof(mac);
    word32      msgSz    = sizeof(msg) - 1;
    word32      expMacSz = sizeof(expMac) - 1;
    int         type     = WC_CMAC_AES;

    XMEMSET(mac, 0, macSz);

    ret = wc_InitCmac(&cmac, key, keySz, type, NULL);
    if (ret != 0) {
        return ret;
    }

    ret = wc_CmacUpdate(&cmac, msg, msgSz);
    if (ret != 0) {
        return ret;
    }

    printf(testingFmt, "wc_AesCmacGenerate()");

    ret = wc_AesCmacGenerate(mac, &macSz, msg, msgSz, key, keySz);
    if (ret == 0 && XMEMCMP(mac, expMac, expMacSz) != 0) {
        ret = SSL_FATAL_ERROR;
    }
    /* Pass in bad args. */
    if (ret == 0) {
        ret = wc_AesCmacGenerate(NULL, &macSz, msg, msgSz, key, keySz);
        if (ret == BAD_FUNC_ARG) {
            ret = wc_AesCmacGenerate(mac, &macSz, msg, msgSz, NULL, keySz);
        }
        if (ret == BAD_FUNC_ARG) {
            ret = wc_AesCmacGenerate(mac, &macSz, msg, msgSz, key, 0);
        }
        if (ret == BAD_FUNC_ARG) {
            ret = wc_AesCmacGenerate(mac, &macSz, NULL, msgSz, key, keySz);
        }
        if (ret == BAD_FUNC_ARG) {
            ret = 0;
        } else if (ret == 0) {
            ret = SSL_FATAL_ERROR;
        }
    }
    printf(resultFmt, ret == 0 ? passed : failed);

    if (ret == 0) {
        printf(testingFmt, "wc_AesCmacVerify()");

        ret = wc_AesCmacVerify(mac, macSz, msg, msgSz, key, keySz);
        /* Test bad args. */
        if (ret == 0) {
            ret = wc_AesCmacVerify(NULL, macSz, msg, msgSz, key, keySz);
            if (ret == BAD_FUNC_ARG) {
                ret = wc_AesCmacVerify(mac, 0, msg, msgSz, key, keySz);
            }
            if (ret == BAD_FUNC_ARG) {
                ret = wc_AesCmacVerify(mac, macSz, msg, msgSz, NULL, keySz);
            }
            if (ret == BAD_FUNC_ARG) {
                ret = wc_AesCmacVerify(mac, macSz, msg, msgSz, key, 0);
            }
            if (ret == BAD_FUNC_ARG) {
                ret = wc_AesCmacVerify(mac, macSz, NULL, msgSz, key, keySz);
            }
            if (ret == BAD_FUNC_ARG) {
                ret = 0;
            } else if (ret == 0) {
                ret = SSL_FATAL_ERROR;
            }
        }

        printf(resultFmt, ret == 0 ? passed : failed);
    }

#endif
    return ret;

} /* END test_wc_AesCmacGenerate */




/*
 * unit test for wc_Des3_SetIV()
 */
static int test_wc_Des3_SetIV (void)
{
    int  ret = 0;
#ifndef NO_DES3
    Des3 des;
    const byte key[] =
    {
        0x01,0x23,0x45,0x67,0x89,0xab,0xcd,0xef,
        0xfe,0xde,0xba,0x98,0x76,0x54,0x32,0x10,
        0x89,0xab,0xcd,0xef,0x01,0x23,0x45,0x67
    };

    const byte iv[] =
    {
        0x12,0x34,0x56,0x78,0x90,0xab,0xcd,0xef,
        0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,
        0x11,0x21,0x31,0x41,0x51,0x61,0x71,0x81
    };

    printf(testingFmt, "wc_Des3_SetIV()");

    ret = wc_Des3Init(&des, NULL, INVALID_DEVID);
    if (ret != 0)
        return ret;

    /* DES_ENCRYPTION or DES_DECRYPTION */
    ret = wc_Des3_SetKey(&des, key, iv, DES_ENCRYPTION);

    if (ret == 0) {
        if (XMEMCMP(iv, des.reg, DES_BLOCK_SIZE) != 0) {
            ret = WOLFSSL_FATAL_ERROR;
        }
    }

#ifndef HAVE_FIPS /* no sanity checks with FIPS wrapper */
    /* Test explicitly wc_Des3_SetIV()  */
    if (ret == 0) {
        ret = wc_Des3_SetIV(NULL, iv);
        if (ret == BAD_FUNC_ARG) {
            ret = wc_Des3_SetIV(&des, NULL);
        } else if (ret == 0) {
            ret = WOLFSSL_FATAL_ERROR;
        }
    }
#endif

    wc_Des3Free(&des);

    printf(resultFmt, ret == 0 ? passed : failed);

#endif
    return ret;

} /* END test_wc_Des3_SetIV */

/*
 * unit test for wc_Des3_SetKey()
 */
static int test_wc_Des3_SetKey (void)
{
    int  ret = 0;
#ifndef NO_DES3
    Des3 des;
    const byte key[] =
    {
        0x01,0x23,0x45,0x67,0x89,0xab,0xcd,0xef,
        0xfe,0xde,0xba,0x98,0x76,0x54,0x32,0x10,
        0x89,0xab,0xcd,0xef,0x01,0x23,0x45,0x67
    };

    const byte iv[] =
    {
        0x12,0x34,0x56,0x78,0x90,0xab,0xcd,0xef,
        0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,
        0x11,0x21,0x31,0x41,0x51,0x61,0x71,0x81
    };

    printf(testingFmt, "wc_Des3_SetKey()");

    ret = wc_Des3Init(&des, NULL, INVALID_DEVID);
    if (ret != 0)
        return ret;

    /* DES_ENCRYPTION or DES_DECRYPTION */
    ret = wc_Des3_SetKey(&des, key, iv, DES_ENCRYPTION);
    if (ret == 0) {
        if (XMEMCMP(iv, des.reg, DES_BLOCK_SIZE) != 0) {
            ret = WOLFSSL_FATAL_ERROR;
        }
    }

    /* Test bad args. */
    if (ret == 0) {
        ret = wc_Des3_SetKey(NULL, key, iv, DES_ENCRYPTION);
        if (ret == BAD_FUNC_ARG) {
            ret = wc_Des3_SetKey(&des, NULL, iv, DES_ENCRYPTION);
        }
        if (ret == BAD_FUNC_ARG) {
            ret = wc_Des3_SetKey(&des, key, iv, -1);
        }
        if (ret == BAD_FUNC_ARG) {
            /* Default case. Should return 0. */
            ret = wc_Des3_SetKey(&des, key, NULL, DES_ENCRYPTION);
        }
    } /* END if ret != 0 */

    wc_Des3Free(&des);

    printf(resultFmt, ret == 0 ? passed : failed);

#endif
    return ret;

} /* END test_wc_Des3_SetKey */


/*
 * Test function for wc_Des3_CbcEncrypt and wc_Des3_CbcDecrypt
 */
static int test_wc_Des3_CbcEncryptDecrypt (void)
{
    int ret = 0;
#ifndef NO_DES3
    Des3 des;
    byte cipher[24];
    byte plain[24];

    const byte key[] =
    {
        0x01,0x23,0x45,0x67,0x89,0xab,0xcd,0xef,
        0xfe,0xde,0xba,0x98,0x76,0x54,0x32,0x10,
        0x89,0xab,0xcd,0xef,0x01,0x23,0x45,0x67
    };

    const byte iv[] =
    {
        0x12,0x34,0x56,0x78,0x90,0xab,0xcd,0xef,
        0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,
        0x11,0x21,0x31,0x41,0x51,0x61,0x71,0x81
    };

    const byte vector[] = { /* "Now is the time for all " w/o trailing 0 */
        0x4e,0x6f,0x77,0x20,0x69,0x73,0x20,0x74,
        0x68,0x65,0x20,0x74,0x69,0x6d,0x65,0x20,
        0x66,0x6f,0x72,0x20,0x61,0x6c,0x6c,0x20
    };

    printf(testingFmt, "wc_Des3_CbcEncrypt()");

    ret = wc_Des3Init(&des, NULL, INVALID_DEVID);
    if (ret != 0)
        return ret;

    ret = wc_Des3_SetKey(&des, key, iv, DES_ENCRYPTION);

    if (ret == 0) {
        ret = wc_Des3_CbcEncrypt(&des, cipher, vector, 24);

        if (ret == 0) {
            ret = wc_Des3_SetKey(&des, key, iv, DES_DECRYPTION);
        }
        if (ret == 0) {
            ret = wc_Des3_CbcDecrypt(&des, plain, cipher, 24);
        }
    }

    if (ret == 0) {
        if (XMEMCMP(plain, vector, 24) != 0) {
            ret = WOLFSSL_FATAL_ERROR;
        }
    }

    /* Pass in bad args. */
    if (ret == 0) {
        ret = wc_Des3_CbcEncrypt(NULL, cipher, vector, 24);
        if (ret == BAD_FUNC_ARG) {
            ret = wc_Des3_CbcEncrypt(&des, NULL, vector, 24);
        }
        if (ret == BAD_FUNC_ARG) {
            ret = wc_Des3_CbcEncrypt(&des, cipher, NULL, sizeof(vector));
        }
        if (ret != BAD_FUNC_ARG) {
            ret = WOLFSSL_FATAL_ERROR;
        } else {
            ret = 0;
        }
    }

    if (ret == 0) {
        ret = wc_Des3_CbcDecrypt(NULL, plain, cipher, 24);
        if (ret == BAD_FUNC_ARG) {
            ret = wc_Des3_CbcDecrypt(&des, NULL, cipher, 24);
        }
        if (ret == BAD_FUNC_ARG) {
            ret = wc_Des3_CbcDecrypt(&des, plain, NULL, 24);
        }
        if (ret != BAD_FUNC_ARG) {
            ret = WOLFSSL_FATAL_ERROR;
        } else {
            ret = 0;
        }
    }

    wc_Des3Free(&des);

    printf(resultFmt, ret == 0 ? passed : failed);

#endif
    return ret;

} /* END wc_Des3_CbcEncrypt */

/*
 *  Unit test for wc_Des3_CbcEncryptWithKey and wc_Des3_CbcDecryptWithKey
 */
static int test_wc_Des3_CbcEncryptDecryptWithKey (void)
{
    int ret = 0;
#ifndef NO_DES3

    word32 vectorSz, cipherSz;
    byte cipher[24];
    byte plain[24];

    byte vector[] = /* Now is the time for all w/o trailing 0 */
    {
        0x4e,0x6f,0x77,0x20,0x69,0x73,0x20,0x74,
        0x68,0x65,0x20,0x74,0x69,0x6d,0x65,0x20,
        0x66,0x6f,0x72,0x20,0x61,0x6c,0x6c,0x20
    };

    byte key[] =
    {
        0x01,0x23,0x45,0x67,0x89,0xab,0xcd,0xef,
        0xfe,0xde,0xba,0x98,0x76,0x54,0x32,0x10,
        0x89,0xab,0xcd,0xef,0x01,0x23,0x45,0x67
    };

    byte iv[] =
    {
        0x12,0x34,0x56,0x78,0x90,0xab,0xcd,0xef,
        0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,
        0x11,0x21,0x31,0x41,0x51,0x61,0x71,0x81
    };


    vectorSz = sizeof(byte) * 24;
    cipherSz = sizeof(byte) * 24;

    printf(testingFmt, "wc_Des3_CbcEncryptWithKey()");

    ret = wc_Des3_CbcEncryptWithKey(cipher, vector, vectorSz, key, iv);
    if (ret == 0) {
        ret = wc_Des3_CbcDecryptWithKey(plain, cipher, cipherSz, key, iv);
        if (ret == 0) {
            if (XMEMCMP(plain, vector, 24) !=  0) {
                ret = WOLFSSL_FATAL_ERROR;
            }
        }
    }

    /* pass in bad args. */
    if (ret == 0) {
        ret = wc_Des3_CbcEncryptWithKey(NULL, vector, vectorSz, key, iv);
        if (ret == BAD_FUNC_ARG) {
            ret = wc_Des3_CbcEncryptWithKey(cipher, NULL, vectorSz, key, iv);
        }
        if (ret == BAD_FUNC_ARG) {
            ret = wc_Des3_CbcEncryptWithKey(cipher, vector, vectorSz, NULL, iv);
        }
        if (ret == BAD_FUNC_ARG) {
            ret = wc_Des3_CbcEncryptWithKey(cipher, vector, vectorSz,
                                                                    key, NULL);
        } else {
            /* Return code catch. */
            ret = WOLFSSL_FAILURE;
        }
    }

    if (ret == 0) {
        ret = wc_Des3_CbcDecryptWithKey(NULL, cipher, cipherSz, key, iv);
        if (ret == BAD_FUNC_ARG) {
            ret = wc_Des3_CbcDecryptWithKey(plain, NULL, cipherSz, key, iv);
        }
        if (ret == BAD_FUNC_ARG) {
            ret = wc_Des3_CbcDecryptWithKey(plain, cipher, cipherSz, NULL, iv);
        }
        if (ret == BAD_FUNC_ARG) {
            ret = wc_Des3_CbcDecryptWithKey(plain, cipher, cipherSz, key, NULL);
        } else {
            ret = WOLFSSL_FAILURE;
        }
    }

    printf(resultFmt, ret == 0 ? passed : failed);

#endif
    return ret;
} /* END test_wc_Des3_CbcEncryptDecryptWithKey */


/*
 * Testing wc_Chacha_SetKey() and wc_Chacha_SetIV()
 */
static int test_wc_Chacha_SetKey (void)
{
    int         ret = 0;
#ifdef HAVE_CHACHA
    ChaCha      ctx;
    const byte  key[] =
    {
         0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
         0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
         0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
         0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01
    };
    byte        cipher[128];

    printf(testingFmt, "wc_Chacha_SetKey()");

    ret = wc_Chacha_SetKey(&ctx, key, (word32)(sizeof(key)/sizeof(byte)));
    /* Test bad args. */
    if (ret == 0) {
        ret = wc_Chacha_SetKey(NULL, key, (word32)(sizeof(key)/sizeof(byte)));
        if (ret == BAD_FUNC_ARG) {
            ret = wc_Chacha_SetKey(&ctx, key, 18);
        }
        if (ret == BAD_FUNC_ARG) {
            ret = 0;
        } else {
            ret = WOLFSSL_FATAL_ERROR;
        }
    }
    printf(resultFmt, ret == 0 ? passed : failed);
    if (ret != 0) {
        return ret;
    }

    printf(testingFmt, "wc_Chacha_SetIV");
    ret = wc_Chacha_SetIV(&ctx, cipher, 0);
    if (ret == 0) {
    /* Test bad args. */
        ret = wc_Chacha_SetIV(NULL, cipher, 0);
        if (ret == BAD_FUNC_ARG) {
            ret = 0;
        } else {
            ret = WOLFSSL_FAILURE;
        }
    }
    printf(resultFmt, ret == 0 ? passed : failed);

#endif
    return ret;
} /* END test_wc_Chacha_SetKey */

/*
 * unit test for wc_Poly1305SetKey()
 */
static int test_wc_Poly1305SetKey(void)
{
    int ret = 0;

#ifdef HAVE_POLY1305
    Poly1305      ctx;
    const byte  key[] =
    {
         0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
         0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
         0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
         0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01
    };

    printf(testingFmt, "wc_Poly1305_SetKey()");

    ret = wc_Poly1305SetKey(&ctx, key, (word32)(sizeof(key)/sizeof(byte)));
    /* Test bad args. */
    if (ret == 0) {
        ret = wc_Poly1305SetKey(NULL, key, (word32)(sizeof(key)/sizeof(byte)));
        if(ret == BAD_FUNC_ARG) {
            ret = wc_Poly1305SetKey(&ctx, NULL, (word32)(sizeof(key)/sizeof(byte)));
        }
        if (ret == BAD_FUNC_ARG) {
            ret = wc_Poly1305SetKey(&ctx, key, 18);
        }
        if (ret == BAD_FUNC_ARG) {
            ret = 0;
        } else {
            ret = WOLFSSL_FATAL_ERROR;
        }
    }

    printf(resultFmt, ret == 0 ? passed : failed);

#endif
    return ret;
} /* END test_wc_Poly1305_SetKey() */

/*
 * Testing wc_Chacha_Process()
 */
static int test_wc_Chacha_Process (void)
{
    int         ret = 0;
#ifdef HAVE_CHACHA
    ChaCha      enc, dec;
    byte        cipher[128];
    byte        plain[128];
    const byte  key[] =
    {
         0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
         0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
         0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
         0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01
    };
    const char* input = "Everybody gets Friday off.";
    word32      keySz = sizeof(key)/sizeof(byte);
    unsigned long int inlen = XSTRLEN(input);

    /*Initialize stack varialbes.*/
    XMEMSET(cipher, 0, 128);
    XMEMSET(plain, 0, 128);

    printf(testingFmt, "wc_Chacha_Process()");

    ret = wc_Chacha_SetKey(&enc, key, keySz);
    if (ret == 0) {
        ret = wc_Chacha_SetKey(&dec, key, keySz);
        if (ret == 0) {
            ret = wc_Chacha_SetIV(&enc, cipher, 0);
        }
        if (ret == 0) {
            ret = wc_Chacha_SetIV(&dec, cipher, 0);
        }
    }
    if (ret == 0) {
        ret = wc_Chacha_Process(&enc, cipher, (byte*)input, (word32)inlen);
        if (ret == 0) {
            ret = wc_Chacha_Process(&dec, plain, cipher, (word32)inlen);
            if (ret == 0) {
                ret = XMEMCMP(input, plain, (int)inlen);
            }
        }
    }
    /* Test bad args. */
    if (ret == 0) {
        ret = wc_Chacha_Process(NULL, cipher, (byte*)input, (word32)inlen);
        if (ret == BAD_FUNC_ARG) {
            ret = 0;
        } else {
            ret = WOLFSSL_FATAL_ERROR;
        }
    }

    printf(resultFmt, ret == 0 ? passed : failed);

#endif
    return ret;
} /* END test_wc_Chacha_Process */

/*
 * Testing wc_ChaCha20Poly1305_Encrypt() and wc_ChaCha20Poly1305_Decrypt()
 */
static int test_wc_ChaCha20Poly1305_aead (void)
{
    int   ret = 0;
#if defined(HAVE_CHACHA) && defined(HAVE_POLY1305)
    const byte  key[] = {
        0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
        0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f,
        0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97,
        0x98, 0x99, 0x9a, 0x9b, 0x9c, 0x9d, 0x9e, 0x9f
    };

    const byte  plaintext[] = {
        0x4c, 0x61, 0x64, 0x69, 0x65, 0x73, 0x20, 0x61,
        0x6e, 0x64, 0x20, 0x47, 0x65, 0x6e, 0x74, 0x6c,
        0x65, 0x6d, 0x65, 0x6e, 0x20, 0x6f, 0x66, 0x20,
        0x74, 0x68, 0x65, 0x20, 0x63, 0x6c, 0x61, 0x73,
        0x73, 0x20, 0x6f, 0x66, 0x20, 0x27, 0x39, 0x39,
        0x3a, 0x20, 0x49, 0x66, 0x20, 0x49, 0x20, 0x63,
        0x6f, 0x75, 0x6c, 0x64, 0x20, 0x6f, 0x66, 0x66,
        0x65, 0x72, 0x20, 0x79, 0x6f, 0x75, 0x20, 0x6f,
        0x6e, 0x6c, 0x79, 0x20, 0x6f, 0x6e, 0x65, 0x20,
        0x74, 0x69, 0x70, 0x20, 0x66, 0x6f, 0x72, 0x20,
        0x74, 0x68, 0x65, 0x20, 0x66, 0x75, 0x74, 0x75,
        0x72, 0x65, 0x2c, 0x20, 0x73, 0x75, 0x6e, 0x73,
        0x63, 0x72, 0x65, 0x65, 0x6e, 0x20, 0x77, 0x6f,
        0x75, 0x6c, 0x64, 0x20, 0x62, 0x65, 0x20, 0x69,
        0x74, 0x2e
    };

    const byte  iv[] = {
        0x07, 0x00, 0x00, 0x00, 0x40, 0x41, 0x42, 0x43,
        0x44, 0x45, 0x46, 0x47
    };

    const byte  aad[] = { /* additional data */
        0x50, 0x51, 0x52, 0x53, 0xc0, 0xc1, 0xc2, 0xc3,
        0xc4, 0xc5, 0xc6, 0xc7
    };
    const byte  cipher[] = { /* expected output from operation */
        0xd3, 0x1a, 0x8d, 0x34, 0x64, 0x8e, 0x60, 0xdb,
        0x7b, 0x86, 0xaf, 0xbc, 0x53, 0xef, 0x7e, 0xc2,
        0xa4, 0xad, 0xed, 0x51, 0x29, 0x6e, 0x08, 0xfe,
        0xa9, 0xe2, 0xb5, 0xa7, 0x36, 0xee, 0x62, 0xd6,
        0x3d, 0xbe, 0xa4, 0x5e, 0x8c, 0xa9, 0x67, 0x12,
        0x82, 0xfa, 0xfb, 0x69, 0xda, 0x92, 0x72, 0x8b,
        0x1a, 0x71, 0xde, 0x0a, 0x9e, 0x06, 0x0b, 0x29,
        0x05, 0xd6, 0xa5, 0xb6, 0x7e, 0xcd, 0x3b, 0x36,
        0x92, 0xdd, 0xbd, 0x7f, 0x2d, 0x77, 0x8b, 0x8c,
        0x98, 0x03, 0xae, 0xe3, 0x28, 0x09, 0x1b, 0x58,
        0xfa, 0xb3, 0x24, 0xe4, 0xfa, 0xd6, 0x75, 0x94,
        0x55, 0x85, 0x80, 0x8b, 0x48, 0x31, 0xd7, 0xbc,
        0x3f, 0xf4, 0xde, 0xf0, 0x8e, 0x4b, 0x7a, 0x9d,
        0xe5, 0x76, 0xd2, 0x65, 0x86, 0xce, 0xc6, 0x4b,
        0x61, 0x16
    };
    const byte  authTag[] = { /* expected output from operation */
        0x1a, 0xe1, 0x0b, 0x59, 0x4f, 0x09, 0xe2, 0x6a,
        0x7e, 0x90, 0x2e, 0xcb, 0xd0, 0x60, 0x06, 0x91
    };
    byte        generatedCiphertext[272];
    byte        generatedPlaintext[272];
    byte        generatedAuthTag[CHACHA20_POLY1305_AEAD_AUTHTAG_SIZE];

    /* Initialize stack variables. */
    XMEMSET(generatedCiphertext, 0, 272);
    XMEMSET(generatedPlaintext, 0, 272);

    /* Test Encrypt */
    printf(testingFmt, "wc_ChaCha20Poly1305_Encrypt()");

    ret = wc_ChaCha20Poly1305_Encrypt(key, iv, aad, sizeof(aad), plaintext,
                sizeof(plaintext), generatedCiphertext, generatedAuthTag);
    if (ret == 0) {
        ret = XMEMCMP(generatedCiphertext, cipher, sizeof(cipher)/sizeof(byte));
    }
    /* Test bad args. */
    if (ret == 0) {
        ret = wc_ChaCha20Poly1305_Encrypt(NULL, iv, aad, sizeof(aad), plaintext,
                    sizeof(plaintext), generatedCiphertext, generatedAuthTag);
        if (ret == BAD_FUNC_ARG) {
            ret = wc_ChaCha20Poly1305_Encrypt(key, NULL, aad, sizeof(aad),
                                        plaintext, sizeof(plaintext),
                                        generatedCiphertext, generatedAuthTag);
        }
        if (ret == BAD_FUNC_ARG) {
            ret = wc_ChaCha20Poly1305_Encrypt(key, iv, aad, sizeof(aad), NULL,
                    sizeof(plaintext), generatedCiphertext, generatedAuthTag);
        }
        if (ret == BAD_FUNC_ARG) {
            ret = wc_ChaCha20Poly1305_Encrypt(key, iv, aad, sizeof(aad),
                    plaintext, 0, generatedCiphertext, generatedAuthTag);
        }
        if (ret == BAD_FUNC_ARG) {
            ret = wc_ChaCha20Poly1305_Encrypt(key, iv, aad, sizeof(aad),
                    plaintext, sizeof(plaintext), NULL, generatedAuthTag);
        }
        if (ret == BAD_FUNC_ARG) {
            ret = wc_ChaCha20Poly1305_Encrypt(key, iv, aad, sizeof(aad),
                    plaintext, sizeof(plaintext), generatedCiphertext, NULL);
        }
        if (ret == BAD_FUNC_ARG) {
            ret = 0;
        } else {
            ret = WOLFSSL_FATAL_ERROR;
        }
    }
    printf(resultFmt, ret == 0 ? passed : failed);
    if (ret != 0) {
        return ret;
    }

    printf(testingFmt, "wc_ChaCha20Poly1305_Decrypt()");
    ret = wc_ChaCha20Poly1305_Decrypt(key, iv, aad, sizeof(aad), cipher,
                            sizeof(cipher), authTag, generatedPlaintext);
    if (ret == 0) {
        ret = XMEMCMP(generatedPlaintext, plaintext,
                                        sizeof(plaintext)/sizeof(byte));
    }
    /* Test bad args. */
    if (ret == 0) {
        ret = wc_ChaCha20Poly1305_Decrypt(NULL, iv, aad, sizeof(aad), cipher,
                                sizeof(cipher), authTag, generatedPlaintext);
        if (ret == BAD_FUNC_ARG) {
            ret = wc_ChaCha20Poly1305_Decrypt(key, NULL, aad, sizeof(aad),
                        cipher, sizeof(cipher), authTag, generatedPlaintext);
        }
        if (ret == BAD_FUNC_ARG) {
            ret = wc_ChaCha20Poly1305_Decrypt(key, iv, aad, sizeof(aad), NULL,
                                sizeof(cipher), authTag, generatedPlaintext);
        }
        if (ret == BAD_FUNC_ARG) {
            ret = wc_ChaCha20Poly1305_Decrypt(key, iv, aad, sizeof(aad), cipher,
                                    sizeof(cipher), NULL, generatedPlaintext);
        }
        if (ret == BAD_FUNC_ARG) {
            ret = wc_ChaCha20Poly1305_Decrypt(key, iv, aad, sizeof(aad), cipher,
                                                sizeof(cipher), authTag, NULL);
        }
        if (ret == BAD_FUNC_ARG) {
            ret = wc_ChaCha20Poly1305_Decrypt(key, iv, aad, sizeof(aad), cipher,
                                                0, authTag, generatedPlaintext);
        }
        if (ret == BAD_FUNC_ARG) {
            ret = 0;
        } else {
            ret = WOLFSSL_FATAL_ERROR;
        }
    }

    printf(resultFmt, ret == 0 ? passed : failed);

#endif
    return ret;

} /* END test-wc_ChaCha20Poly1305_EncryptDecrypt */


/*
 * Testing function for wc_AesSetIV
 */
static int test_wc_AesSetIV (void)
{
    int     ret = 0;
#if !defined(NO_AES) && defined(WOLFSSL_AES_128)
    Aes     aes;
    byte    key16[] =
    {
        0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
        0x38, 0x39, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66
    };
    byte    iv1[]    = "1234567890abcdef";
    byte    iv2[]    = "0987654321fedcba";

    printf(testingFmt, "wc_AesSetIV()");

    ret = wc_AesInit(&aes, NULL, INVALID_DEVID);
    if (ret != 0)
        return ret;

    ret = wc_AesSetKey(&aes, key16, (word32) sizeof(key16) / sizeof(byte),
                                                     iv1, AES_ENCRYPTION);
    if(ret == 0) {
        ret = wc_AesSetIV(&aes, iv2);
    }
    /* Test bad args. */
    if(ret == 0) {
        ret = wc_AesSetIV(NULL, iv1);
        if(ret == BAD_FUNC_ARG) {
            /* NULL iv should return 0. */
            ret = wc_AesSetIV(&aes, NULL);
        } else {
            ret = WOLFSSL_FATAL_ERROR;
        }
    }

    wc_AesFree(&aes);

    printf(resultFmt, ret == 0 ? passed : failed);

#endif
    return ret;
} /* test_wc_AesSetIV */


/*
 * Testing function for wc_AesSetKey().
 */
static int test_wc_AesSetKey (void)
{
    int     ret = 0;
#ifndef NO_AES
    Aes     aes;
    byte    key16[] =
    {
        0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
        0x38, 0x39, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66
    };
#ifdef WOLFSSL_AES_192
    byte    key24[] =
    {
        0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
        0x38, 0x39, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66,
        0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37
    };
#endif
#ifdef WOLFSSL_AES_256
    byte    key32[] =
    {
        0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
        0x38, 0x39, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66,
        0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
        0x38, 0x39, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66
    };
#endif
    byte    badKey16[] =
    {
        0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
        0x38, 0x39, 0x61, 0x62, 0x63, 0x64, 0x65
    };
    byte    iv[]    = "1234567890abcdef";

    printf(testingFmt, "wc_AesSetKey()");

    ret = wc_AesInit(&aes, NULL, INVALID_DEVID);
    if (ret != 0)
        return ret;

#ifdef WOLFSSL_AES_128
    ret = wc_AesSetKey(&aes, key16, (word32) sizeof(key16) / sizeof(byte),
                                                        iv, AES_ENCRYPTION);
#endif
#ifdef WOLFSSL_AES_192
    if (ret == 0) {
        ret = wc_AesSetKey (&aes, key24, (word32) sizeof(key24) / sizeof(byte),
                                                           iv, AES_ENCRYPTION);
    }
#endif
#ifdef WOLFSSL_AES_256
    if (ret == 0) {
        ret = wc_AesSetKey (&aes, key32, (word32) sizeof(key32) / sizeof(byte),
                                                           iv, AES_ENCRYPTION);
    }
#endif

    /* Pass in bad args. */
    if (ret == 0) {
        ret = wc_AesSetKey (NULL, key16, (word32) sizeof(key16) / sizeof(byte),
                                                           iv, AES_ENCRYPTION);
            if (ret == BAD_FUNC_ARG) {
                ret = wc_AesSetKey(&aes, badKey16,
                                    (word32) sizeof(badKey16) / sizeof(byte),
                                                         iv, AES_ENCRYPTION);
        }
        if (ret == BAD_FUNC_ARG) {
            ret = 0;
        } else {
            ret = WOLFSSL_FATAL_ERROR;
        }
    }

    wc_AesFree(&aes);

    printf(resultFmt, ret == 0 ? passed : failed);

#endif
    return ret;
} /* END test_wc_AesSetKey */



/*
 * test function for wc_AesCbcEncrypt(), wc_AesCbcDecrypt(),
 * and wc_AesCbcDecryptWithKey()
 */
static int test_wc_AesCbcEncryptDecrypt (void)
{
    int     ret = 0;
#if !defined(NO_AES) && defined(HAVE_AES_CBC) && defined(HAVE_AES_DECRYPT)&& \
    defined(WOLFSSL_AES_256)
    Aes     aes;
    byte    key32[] =
    {
        0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
        0x38, 0x39, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66,
        0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
        0x38, 0x39, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66
    };
    byte    vector[] = /* Now is the time for all w/o trailing 0 */
    {
        0x4e,0x6f,0x77,0x20,0x69,0x73,0x20,0x74,
        0x68,0x65,0x20,0x74,0x69,0x6d,0x65,0x20,
        0x66,0x6f,0x72,0x20,0x61,0x6c,0x6c,0x20
    };
    byte    iv[]    = "1234567890abcdef";
    byte    enc[sizeof(vector)];
    byte    dec[sizeof(vector)];
    int     cbcE    =   WOLFSSL_FATAL_ERROR;
    int     cbcD    =   WOLFSSL_FATAL_ERROR;
    int     cbcDWK  =   WOLFSSL_FATAL_ERROR;
    byte    dec2[sizeof(vector)];

    /* Init stack variables. */
    XMEMSET(enc, 0, sizeof(enc));
    XMEMSET(dec, 0, sizeof(vector));
    XMEMSET(dec2, 0, sizeof(vector));

    ret = wc_AesInit(&aes, NULL, INVALID_DEVID);
    if (ret != 0)
        return ret;

    ret = wc_AesSetKey(&aes, key32, AES_BLOCK_SIZE * 2, iv, AES_ENCRYPTION);
    if (ret == 0) {
        ret = wc_AesCbcEncrypt(&aes, enc, vector, sizeof(vector));
        if (ret == 0) {
            /* Re init for decrypt and set flag. */
            cbcE = 0;
            wc_AesFree(&aes);
            ret = wc_AesSetKey(&aes, key32, AES_BLOCK_SIZE * 2,
                                                    iv, AES_DECRYPTION);
        }
        if (ret == 0) {
            ret = wc_AesCbcDecrypt(&aes, dec, enc, AES_BLOCK_SIZE);
            if (ret != 0 || XMEMCMP(vector, dec, AES_BLOCK_SIZE) != 0) {
                ret = WOLFSSL_FATAL_ERROR;
            } else {
                /* Set flag. */
                cbcD = 0;
            }
        }
    }
    /* If encrypt succeeds but cbc decrypt fails, we can still test. */
    if (ret == 0 || (ret != 0 && cbcE == 0)) {
        ret = wc_AesCbcDecryptWithKey(dec2, enc, AES_BLOCK_SIZE,
                                     key32, sizeof(key32)/sizeof(byte), iv);
        if (ret == 0 || XMEMCMP(vector, dec2, AES_BLOCK_SIZE) == 0) {
            cbcDWK = 0;
        }
    }

    printf(testingFmt, "wc_AesCbcEncrypt()");
    /* Pass in bad args */
    if (cbcE == 0) {
        cbcE = wc_AesCbcEncrypt(NULL, enc, vector, sizeof(vector));
        if (cbcE == BAD_FUNC_ARG) {
            cbcE = wc_AesCbcEncrypt(&aes, NULL, vector, sizeof(vector));
        }
        if (cbcE == BAD_FUNC_ARG) {
            cbcE = wc_AesCbcEncrypt(&aes, enc, NULL, sizeof(vector));
        }
        if (cbcE == BAD_FUNC_ARG) {
            cbcE = 0;
        } else {
            cbcE = WOLFSSL_FATAL_ERROR;
        }
    }
    printf(resultFmt, cbcE == 0 ? passed : failed);
    if (cbcE != 0) {
        wc_AesFree(&aes);
        return cbcE;
    }

    printf(testingFmt, "wc_AesCbcDecrypt()");
    if (cbcD == 0) {
        cbcD = wc_AesCbcDecrypt(NULL, dec, enc, AES_BLOCK_SIZE);
        if (cbcD == BAD_FUNC_ARG) {
            cbcD = wc_AesCbcDecrypt(&aes, NULL, enc, AES_BLOCK_SIZE);
        }
        if (cbcD == BAD_FUNC_ARG) {
            cbcD = wc_AesCbcDecrypt(&aes, dec, NULL, AES_BLOCK_SIZE);
        }
        if (cbcD == BAD_FUNC_ARG) {
            cbcD = wc_AesCbcDecrypt(&aes, dec, enc, AES_BLOCK_SIZE * 2 - 1);
        }
        if (cbcD == BAD_FUNC_ARG) {
            cbcD = 0;
        } else {
            cbcD = WOLFSSL_FATAL_ERROR;
        }
    }
    printf(resultFmt, cbcD == 0 ? passed : failed);
    if (cbcD != 0) {
        wc_AesFree(&aes);
        return cbcD;
    }

    printf(testingFmt, "wc_AesCbcDecryptWithKey()");
    if (cbcDWK == 0) {
        cbcDWK = wc_AesCbcDecryptWithKey(NULL, enc, AES_BLOCK_SIZE,
                                     key32, sizeof(key32)/sizeof(byte), iv);
        if (cbcDWK == BAD_FUNC_ARG) {
            cbcDWK = wc_AesCbcDecryptWithKey(dec2, NULL, AES_BLOCK_SIZE,
                                     key32, sizeof(key32)/sizeof(byte), iv);
        }
        if (cbcDWK == BAD_FUNC_ARG) {
            cbcDWK = wc_AesCbcDecryptWithKey(dec2, enc, AES_BLOCK_SIZE,
                                     NULL, sizeof(key32)/sizeof(byte), iv);
        }
        if (cbcDWK == BAD_FUNC_ARG) {
            cbcDWK = wc_AesCbcDecryptWithKey(dec2, enc, AES_BLOCK_SIZE,
                                     key32, sizeof(key32)/sizeof(byte), NULL);
        }
        if (cbcDWK == BAD_FUNC_ARG) {
            cbcDWK = 0;
        } else {
            cbcDWK = WOLFSSL_FATAL_ERROR;
        }
    }

    wc_AesFree(&aes);

    printf(resultFmt, cbcDWK == 0 ? passed : failed);

    if (cbcDWK != 0) {
        return cbcDWK;
    }
#endif
    return ret;
} /* END test_wc_AesCbcEncryptDecrypt */

/*
 * Testing wc_AesCtrEncrypt and wc_AesCtrDecrypt
 */
static int test_wc_AesCtrEncryptDecrypt (void)
{
    int     ret = 0;
#if !defined(NO_AES) && defined(WOLFSSL_AES_COUNTER) && defined(WOLFSSL_AES_256)
    Aes     aesEnc, aesDec;
    byte    key32[] =
    {
        0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
        0x38, 0x39, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66,
        0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
        0x38, 0x39, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66
    };
    byte    vector[] = /* Now is the time for all w/o trailing 0 */
    {
        0x4e,0x6f,0x77,0x20,0x69,0x73,0x20,0x74,
        0x68,0x65,0x20,0x74,0x69,0x6d,0x65,0x20,
        0x66,0x6f,0x72,0x20,0x61,0x6c,0x6c,0x20
    };
    byte    iv[]    = "1234567890abcdef";
    byte    enc[AES_BLOCK_SIZE * 2];
    byte    dec[AES_BLOCK_SIZE * 2];

    /* Init stack variables. */
    XMEMSET(enc, 0, AES_BLOCK_SIZE * 2);
    XMEMSET(dec, 0, AES_BLOCK_SIZE * 2);

    printf(testingFmt, "wc_AesCtrEncrypt()");

    ret = wc_AesInit(&aesEnc, NULL, INVALID_DEVID);
    if (ret != 0)
        return ret;
    ret = wc_AesInit(&aesDec, NULL, INVALID_DEVID);
    if (ret != 0) {
        wc_AesFree(&aesEnc);
        return ret;
    }

    ret = wc_AesSetKey(&aesEnc, key32, AES_BLOCK_SIZE * 2,
                                                    iv, AES_ENCRYPTION);
    if (ret == 0) {
        ret = wc_AesCtrEncrypt(&aesEnc, enc, vector,
                                            sizeof(vector)/sizeof(byte));
        if (ret == 0) {
            /* Decrypt with wc_AesCtrEncrypt() */
            ret = wc_AesSetKey(&aesDec, key32, AES_BLOCK_SIZE * 2,
                                                    iv, AES_ENCRYPTION);
        }
        if (ret == 0) {
            ret = wc_AesCtrEncrypt(&aesDec, dec, enc, sizeof(enc)/sizeof(byte));
            if (ret != 0 || XMEMCMP(vector, dec, sizeof(vector))) {
                ret = WOLFSSL_FATAL_ERROR;
            }
        }
    }

    /* Test bad args. */
    if (ret == 0) {
        ret = wc_AesCtrEncrypt(NULL, dec, enc, sizeof(enc)/sizeof(byte));
        if (ret == BAD_FUNC_ARG) {
            ret = wc_AesCtrEncrypt(&aesDec, NULL, enc, sizeof(enc)/sizeof(byte));
        }
        if (ret == BAD_FUNC_ARG) {
            ret = wc_AesCtrEncrypt(&aesDec, dec, NULL, sizeof(enc)/sizeof(byte));
        }
        if (ret == BAD_FUNC_ARG) {
            ret = 0;
        } else {
            ret = WOLFSSL_FATAL_ERROR;
        }
    }

    wc_AesFree(&aesEnc);
    wc_AesFree(&aesDec);

    printf(resultFmt, ret == 0 ? passed : failed);

#endif
    return ret;

} /* END test_wc_AesCtrEncryptDecrypt */

/*
 * test function for wc_AesGcmSetKey()
 */
static int test_wc_AesGcmSetKey (void)
{
    int     ret = 0;
#if  !defined(NO_AES) && defined(HAVE_AESGCM)

    Aes     aes;
#ifdef WOLFSSL_AES_128
    byte    key16[] =
    {
        0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
        0x38, 0x39, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66
    };
#endif
#ifdef WOLFSSL_AES_192
    byte    key24[] =
    {
        0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
        0x38, 0x39, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66,
        0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37
    };
#endif
#ifdef WOLFSSL_AES_256
    byte    key32[] =
    {
        0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
        0x38, 0x39, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66,
        0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
        0x38, 0x39, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66
    };
#endif
    byte    badKey16[] =
    {
        0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
        0x38, 0x39, 0x61, 0x62, 0x63, 0x64, 0x65
    };
    byte    badKey24[] =
    {
        0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
        0x38, 0x39, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66,
        0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36
    };
    byte   badKey32[] =
    {
        0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x37, 0x37,
        0x38, 0x39, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66,
        0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
        0x38, 0x39, 0x61, 0x62, 0x63, 0x64, 0x65
    };

    printf(testingFmt, "wc_AesGcmSetKey()");

    ret = wc_AesInit(&aes, NULL, INVALID_DEVID);
    if (ret != 0)
        return ret;

#ifdef WOLFSSL_AES_128
    ret = wc_AesGcmSetKey(&aes, key16, sizeof(key16)/sizeof(byte));
#endif
#ifdef WOLFSSL_AES_192
    if (ret == 0) {
        ret = wc_AesGcmSetKey(&aes, key24, sizeof(key24)/sizeof(byte));
    }
#endif
#ifdef WOLFSSL_AES_256
    if (ret == 0) {
        ret = wc_AesGcmSetKey(&aes, key32, sizeof(key32)/sizeof(byte));
    }
#endif

    /* Pass in bad args. */
    if (ret == 0) {
        ret = wc_AesGcmSetKey(&aes, badKey16, sizeof(badKey16)/sizeof(byte));
        if (ret == BAD_FUNC_ARG) {
            ret = wc_AesGcmSetKey(&aes, badKey24, sizeof(badKey24)/sizeof(byte));
        }
        if (ret == BAD_FUNC_ARG) {
            ret = wc_AesGcmSetKey(&aes, badKey32, sizeof(badKey32)/sizeof(byte));
        }
        if (ret == BAD_FUNC_ARG) {
            ret = 0;
        } else {
            ret = WOLFSSL_FATAL_ERROR;
        }
    }

    wc_AesFree(&aes);

    printf(resultFmt, ret == 0 ? passed : failed);

#endif
    return ret;
} /* END test_wc_AesGcmSetKey */

/*
 * test function for wc_AesGcmEncrypt and wc_AesGcmDecrypt
 */
static int test_wc_AesGcmEncryptDecrypt (void)
{
    int     ret = 0;
    /* WOLFSSL_AFALG requires 12 byte IV */
#if !defined(NO_AES) && defined(HAVE_AESGCM) && defined(WOLFSSL_AES_256) && \
    !defined(WOLFSSL_AFALG) && !defined(WOLFSSL_DEVCRYPTO_AES)

    Aes     aes;
    byte    key32[] =
    {
        0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
        0x38, 0x39, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66,
        0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
        0x38, 0x39, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66
    };
    byte    vector[] = /* Now is the time for all w/o trailing 0 */
    {
        0x4e,0x6f,0x77,0x20,0x69,0x73,0x20,0x74,
        0x68,0x65,0x20,0x74,0x69,0x6d,0x65,0x20,
        0x66,0x6f,0x72,0x20,0x61,0x6c,0x6c,0x20
    };
    const byte a[] =
    {
        0xfe, 0xed, 0xfa, 0xce, 0xde, 0xad, 0xbe, 0xef,
        0xfe, 0xed, 0xfa, 0xce, 0xde, 0xad, 0xbe, 0xef,
        0xab, 0xad, 0xda, 0xd2
    };
    byte    iv[]   = "1234567890a";
    byte    longIV[]  = "1234567890abcdefghij";
    byte    enc[sizeof(vector)];
    byte    resultT[AES_BLOCK_SIZE];
    byte    dec[sizeof(vector)];
    int     gcmD     =   WOLFSSL_FATAL_ERROR;
    int     gcmE     =   WOLFSSL_FATAL_ERROR;

    /* Init stack variables. */
    XMEMSET(enc, 0, sizeof(vector));
    XMEMSET(dec, 0, sizeof(vector));
    XMEMSET(resultT, 0, AES_BLOCK_SIZE);

    ret = wc_AesInit(&aes, NULL, INVALID_DEVID);
    if (ret != 0)
        return ret;

    ret = wc_AesGcmSetKey(&aes, key32, sizeof(key32)/sizeof(byte));
    if (ret == 0) {
        gcmE = wc_AesGcmEncrypt(&aes, enc, vector, sizeof(vector),
                                        iv, sizeof(iv)/sizeof(byte), resultT,
                                        sizeof(resultT), a, sizeof(a));
    }
    if (gcmE == 0) { /* If encrypt fails, no decrypt. */
        gcmD = wc_AesGcmDecrypt(&aes, dec, enc, sizeof(vector),
                                        iv, sizeof(iv)/sizeof(byte), resultT,
                                        sizeof(resultT), a, sizeof(a));
        if(gcmD == 0 && (XMEMCMP(vector, dec, sizeof(vector)) !=  0)) {
            gcmD = WOLFSSL_FATAL_ERROR;
        }
    }
    printf(testingFmt, "wc_AesGcmEncrypt()");
    /*Test bad args for wc_AesGcmEncrypt and wc_AesGcmDecrypt */
    if (gcmE == 0) {
        gcmE = wc_AesGcmEncrypt(NULL, enc, vector, sizeof(vector),
                        iv, sizeof(iv)/sizeof(byte), resultT, sizeof(resultT),
                        a, sizeof(a));
        if (gcmE == BAD_FUNC_ARG) {
            gcmE = wc_AesGcmEncrypt(&aes, enc, vector,
                    sizeof(vector), iv, sizeof(iv)/sizeof(byte),
                    resultT, sizeof(resultT) + 1, a, sizeof(a));
        }
        if (gcmE == BAD_FUNC_ARG) {
            gcmE = wc_AesGcmEncrypt(&aes, enc, vector,
                    sizeof(vector), iv, sizeof(iv)/sizeof(byte),
                    resultT, sizeof(resultT) - 5, a, sizeof(a));
        }

        if (gcmE == BAD_FUNC_ARG) {
            gcmE = 0;
        } else {
            gcmE = WOLFSSL_FATAL_ERROR;
        }
    }

    /* This case is now considered good. Long IVs are now allowed.
     * Except for the original FIPS release, it still has an upper
     * bound on the IV length. */
#if !defined(HAVE_FIPS) || \
    (defined(HAVE_FIPS_VERSION) && (HAVE_FIPS_VERSION >= 2))
    if (gcmE == 0) {
        gcmE = wc_AesGcmEncrypt(&aes, enc, vector, sizeof(vector), longIV,
                        sizeof(longIV)/sizeof(byte), resultT, sizeof(resultT),
                        a, sizeof(a));
    }
#else
    (void)longIV;
#endif /* Old FIPS */
    /* END wc_AesGcmEncrypt */

    printf(resultFmt, gcmE == 0 ? passed : failed);
    if (gcmE != 0) {
        wc_AesFree(&aes);
        return gcmE;
    }

    #ifdef HAVE_AES_DECRYPT
        printf(testingFmt, "wc_AesGcmDecrypt()");

        if (gcmD == 0) {
            gcmD = wc_AesGcmDecrypt(NULL, dec, enc, sizeof(enc)/sizeof(byte),
                                   iv, sizeof(iv)/sizeof(byte), resultT,
                                   sizeof(resultT), a, sizeof(a));
            if (gcmD == BAD_FUNC_ARG) {
                gcmD = wc_AesGcmDecrypt(&aes, NULL, enc, sizeof(enc)/sizeof(byte),
                                   iv, sizeof(iv)/sizeof(byte), resultT,
                                   sizeof(resultT), a, sizeof(a));
            }
            if (gcmD == BAD_FUNC_ARG) {
                gcmD = wc_AesGcmDecrypt(&aes, dec, NULL, sizeof(enc)/sizeof(byte),
                                   iv, sizeof(iv)/sizeof(byte), resultT,
                                   sizeof(resultT), a, sizeof(a));
            }
            if (gcmD == BAD_FUNC_ARG) {
                gcmD = wc_AesGcmDecrypt(&aes, dec, enc, sizeof(enc)/sizeof(byte),
                                   NULL, sizeof(iv)/sizeof(byte), resultT,
                                   sizeof(resultT), a, sizeof(a));
            }
            if (gcmD == BAD_FUNC_ARG) {
                gcmD = wc_AesGcmDecrypt(&aes, dec, enc, sizeof(enc)/sizeof(byte),
                                   iv, sizeof(iv)/sizeof(byte), NULL,
                                   sizeof(resultT), a, sizeof(a));
            }
            if (gcmD == BAD_FUNC_ARG) {
                gcmD = wc_AesGcmDecrypt(&aes, dec, enc, sizeof(enc)/sizeof(byte),
                                   iv, sizeof(iv)/sizeof(byte), resultT,
                                   sizeof(resultT) + 1, a, sizeof(a));
            }
            if (gcmD == BAD_FUNC_ARG) {
                gcmD = 0;
            } else {
                gcmD = WOLFSSL_FATAL_ERROR;
            }
        } /* END wc_AesGcmDecrypt */

        printf(resultFmt, gcmD == 0 ? passed : failed);
    #endif /* HAVE_AES_DECRYPT */

    wc_AesFree(&aes);
#endif

    return ret;

} /* END test_wc_AesGcmEncryptDecrypt */

/*
 * unit test for wc_GmacSetKey()
 */
static int test_wc_GmacSetKey (void)
{
    int     ret = 0;
#if !defined(NO_AES) && defined(HAVE_AESGCM)
    Gmac    gmac;
    byte    key16[] =
    {
        0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
        0x38, 0x39, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66
    };
#ifdef WOLFSSL_AES_192
    byte    key24[] =
    {
        0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
        0x38, 0x39, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66,
        0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37
    };
#endif
#ifdef WOLFSSL_AES_256
    byte    key32[] =
    {
        0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
        0x38, 0x39, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66,
        0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
        0x38, 0x39, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66
    };
#endif
    byte    badKey16[] =
    {
        0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
        0x38, 0x39, 0x61, 0x62, 0x63, 0x64, 0x66
    };
    byte    badKey24[] =
    {
        0x30, 0x31, 0x32, 0x33, 0x34, 0x36, 0x37,
        0x38, 0x39, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66,
        0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37
    };
    byte    badKey32[] =
    {
        0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
        0x38, 0x39, 0x61, 0x62, 0x64, 0x65, 0x66,
        0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
        0x38, 0x39, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66
    };

    printf(testingFmt, "wc_GmacSetKey()");

    ret = wc_AesInit(&gmac.aes, NULL, INVALID_DEVID);
    if (ret != 0)
        return ret;

#ifdef WOLFSSL_AES_128
    ret = wc_GmacSetKey(&gmac, key16, sizeof(key16)/sizeof(byte));
#endif
#ifdef WOLFSSL_AES_192
    if (ret == 0) {
        ret = wc_GmacSetKey(&gmac, key24, sizeof(key24)/sizeof(byte));
    }
#endif
#ifdef WOLFSSL_AES_256
    if (ret == 0) {
        ret = wc_GmacSetKey(&gmac, key32, sizeof(key32)/sizeof(byte));
    }
#endif

    /* Pass in bad args. */
    if (ret == 0) {
        ret = wc_GmacSetKey(NULL, key16, sizeof(key16)/sizeof(byte));
        if (ret == BAD_FUNC_ARG) {
            ret = wc_GmacSetKey(&gmac, NULL, sizeof(key16)/sizeof(byte));
        }
        if (ret == BAD_FUNC_ARG) {
            ret = wc_GmacSetKey(&gmac, badKey16, sizeof(badKey16)/sizeof(byte));
        }
        if (ret == BAD_FUNC_ARG) {
            ret = wc_GmacSetKey(&gmac, badKey24, sizeof(badKey24)/sizeof(byte));
        }
        if (ret == BAD_FUNC_ARG) {
            ret = wc_GmacSetKey(&gmac, badKey32, sizeof(badKey32)/sizeof(byte));
        }
        if (ret == BAD_FUNC_ARG) {
            ret = 0;
        } else {
            ret = WOLFSSL_FATAL_ERROR;
        }
    }

    wc_AesFree(&gmac.aes);

    printf(resultFmt, ret == 0 ? passed : failed);

#endif
    return ret;

} /* END test_wc_GmacSetKey */

/*
 * unit test for wc_GmacUpdate
 */
static int test_wc_GmacUpdate (void)
{
    int     ret = 0;
#if !defined(NO_AES) && defined(HAVE_AESGCM)
    Gmac    gmac;
#ifdef WOLFSSL_AES_128
    const byte key16[] =
    {
        0x89, 0xc9, 0x49, 0xe9, 0xc8, 0x04, 0xaf, 0x01,
        0x4d, 0x56, 0x04, 0xb3, 0x94, 0x59, 0xf2, 0xc8
    };
#endif
#ifdef WOLFSSL_AES_192
    byte    key24[] =
    {
        0x41, 0xc5, 0xda, 0x86, 0x67, 0xef, 0x72, 0x52,
        0x20, 0xff, 0xe3, 0x9a, 0xe0, 0xac, 0x59, 0x0a,
        0xc9, 0xfc, 0xa7, 0x29, 0xab, 0x60, 0xad, 0xa0
    };
#endif
#ifdef WOLFSSL_AES_256
   byte    key32[] =
    {
        0x78, 0xdc, 0x4e, 0x0a, 0xaf, 0x52, 0xd9, 0x35,
        0xc3, 0xc0, 0x1e, 0xea, 0x57, 0x42, 0x8f, 0x00,
        0xca, 0x1f, 0xd4, 0x75, 0xf5, 0xda, 0x86, 0xa4,
        0x9c, 0x8d, 0xd7, 0x3d, 0x68, 0xc8, 0xe2, 0x23
    };
#endif
#ifdef WOLFSSL_AES_128
    const byte authIn[] =
    {
        0x82, 0xad, 0xcd, 0x63, 0x8d, 0x3f, 0xa9, 0xd9,
        0xf3, 0xe8, 0x41, 0x00, 0xd6, 0x1e, 0x07, 0x77
    };
#endif
#ifdef WOLFSSL_AES_192
    const byte authIn2[] =
    {
       0x8b, 0x5c, 0x12, 0x4b, 0xef, 0x6e, 0x2f, 0x0f,
       0xe4, 0xd8, 0xc9, 0x5c, 0xd5, 0xfa, 0x4c, 0xf1
    };
#endif
    const byte authIn3[] =
    {
        0xb9, 0x6b, 0xaa, 0x8c, 0x1c, 0x75, 0xa6, 0x71,
        0xbf, 0xb2, 0xd0, 0x8d, 0x06, 0xbe, 0x5f, 0x36
    };
#ifdef WOLFSSL_AES_128
    const byte tag1[] = /* Known. */
    {
        0x88, 0xdb, 0x9d, 0x62, 0x17, 0x2e, 0xd0, 0x43,
        0xaa, 0x10, 0xf1, 0x6d, 0x22, 0x7d, 0xc4, 0x1b
    };
#endif
#ifdef WOLFSSL_AES_192
    const byte tag2[] = /* Known */
    {
        0x20, 0x4b, 0xdb, 0x1b, 0xd6, 0x21, 0x54, 0xbf,
        0x08, 0x92, 0x2a, 0xaa, 0x54, 0xee, 0xd7, 0x05
    };
#endif
    const byte tag3[] = /* Known */
    {
        0x3e, 0x5d, 0x48, 0x6a, 0xa2, 0xe3, 0x0b, 0x22,
        0xe0, 0x40, 0xb8, 0x57, 0x23, 0xa0, 0x6e, 0x76
    };
#ifdef WOLFSSL_AES_128
    const byte iv[] =
    {
        0xd1, 0xb1, 0x04, 0xc8, 0x15, 0xbf, 0x1e, 0x94,
        0xe2, 0x8c, 0x8f, 0x16
    };
#endif
#ifdef WOLFSSL_AES_192
    const byte iv2[] =
    {
        0x05, 0xad, 0x13, 0xa5, 0xe2, 0xc2, 0xab, 0x66,
        0x7e, 0x1a, 0x6f, 0xbc
    };
#endif
    const byte iv3[] =
    {
        0xd7, 0x9c, 0xf2, 0x2d, 0x50, 0x4c, 0xc7, 0x93,
        0xc3, 0xfb, 0x6c, 0x8a
    };
    byte    tagOut[16];
    byte    tagOut2[24];
    byte    tagOut3[32];

    /* Init stack varaibles. */
    XMEMSET(tagOut, 0, sizeof(tagOut));
    XMEMSET(tagOut2, 0, sizeof(tagOut2));
    XMEMSET(tagOut3, 0, sizeof(tagOut3));

    printf(testingFmt, "wc_GmacUpdate()");

    ret = wc_AesInit(&gmac.aes, NULL, INVALID_DEVID);
    if (ret != 0)
        return ret;

#ifdef WOLFSSL_AES_128
    ret = wc_GmacSetKey(&gmac, key16, sizeof(key16));
    if (ret == 0) {
        ret = wc_GmacUpdate(&gmac, iv, sizeof(iv), authIn, sizeof(authIn),
                                                    tagOut, sizeof(tag1));
        if (ret == 0) {
            ret = XMEMCMP(tag1, tagOut, sizeof(tag1));
        }
    }
#endif

#ifdef WOLFSSL_AES_192
    if (ret == 0) {
        XMEMSET(&gmac, 0, sizeof(Gmac));
        ret = wc_GmacSetKey(&gmac, key24, sizeof(key24)/sizeof(byte));
    }
    if (ret == 0) {
        ret = wc_GmacUpdate(&gmac, iv2, sizeof(iv2), authIn2,
                            sizeof(authIn2), tagOut2, sizeof(tag2));
    }
    if (ret == 0) {
        ret = XMEMCMP(tagOut2, tag2, sizeof(tag2));
    }
#endif

#ifdef WOLFSSL_AES_256
    if (ret == 0) {
        XMEMSET(&gmac, 0, sizeof(Gmac));
        ret = wc_GmacSetKey(&gmac, key32, sizeof(key32)/sizeof(byte));
    }
    if (ret == 0) {
        ret = wc_GmacUpdate(&gmac, iv3, sizeof(iv3), authIn3,
                            sizeof(authIn3), tagOut3, sizeof(tag3));
    }
    if (ret == 0) {
        ret = XMEMCMP(tag3, tagOut3, sizeof(tag3));
    }
#endif

    /*Pass bad args. */
    if (ret == 0) {
        ret = wc_GmacUpdate(NULL, iv3, sizeof(iv3), authIn3,
                                sizeof(authIn3), tagOut3, sizeof(tag3));
        if (ret == BAD_FUNC_ARG) {
            ret = wc_GmacUpdate(&gmac, iv3, sizeof(iv3), authIn3,
                                sizeof(authIn3), tagOut3, sizeof(tag3) - 5);
        }
        if (ret == BAD_FUNC_ARG) {
            ret = wc_GmacUpdate(&gmac, iv3, sizeof(iv3), authIn3,
                                sizeof(authIn3), tagOut3, sizeof(tag3) + 1);
        }
        if (ret == BAD_FUNC_ARG) {
            ret = 0;
        } else {
            ret = WOLFSSL_FATAL_ERROR;
        }
    }

    wc_AesFree(&gmac.aes);

    printf(resultFmt, ret == 0 ? passed : failed);

#endif
    return ret;

} /* END test_wc_GmacUpdate */


/*
 * testing wc_CamelliaSetKey
 */
static int test_wc_CamelliaSetKey (void)
{
    int ret = 0;
#ifdef HAVE_CAMELLIA
    Camellia camellia;
    /*128-bit key*/
    static const byte key16[] =
    {
        0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,
        0xfe, 0xdc, 0xba, 0x98, 0x76, 0x54, 0x32, 0x10
    };
    /* 192-bit key */
    static const byte key24[] =
    {
        0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,
        0xfe, 0xdc, 0xba, 0x98, 0x76, 0x54, 0x32, 0x10,
        0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77
    };
    /* 256-bit key */
    static const byte key32[] =
    {
        0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,
        0xfe, 0xdc, 0xba, 0x98, 0x76, 0x54, 0x32, 0x10,
        0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
        0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff
    };
    static const byte iv[] =
    {
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
        0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F
    };

    printf(testingFmt, "wc_CamelliaSetKey()");

    ret = wc_CamelliaSetKey(&camellia, key16, (word32)sizeof(key16), iv);
    if (ret == 0) {
        ret = wc_CamelliaSetKey(&camellia, key16,
                                        (word32)sizeof(key16), NULL);
        if (ret == 0) {
            ret = wc_CamelliaSetKey(&camellia, key24,
                                        (word32)sizeof(key24), iv);
        }
        if (ret == 0) {
            ret = wc_CamelliaSetKey(&camellia, key24,
                                        (word32)sizeof(key24), NULL);
        }
        if (ret == 0) {
            ret = wc_CamelliaSetKey(&camellia, key32,
                                        (word32)sizeof(key32), iv);
        }
        if (ret == 0) {
            ret = wc_CamelliaSetKey(&camellia, key32,
                                        (word32)sizeof(key32), NULL);
        }
    }
    /* Bad args. */
    if (ret == 0) {
        ret = wc_CamelliaSetKey(NULL, key32, (word32)sizeof(key32), iv);
        if (ret != BAD_FUNC_ARG) {
            ret = WOLFSSL_FATAL_ERROR;
        } else {
            ret = 0;
        }
    } /* END bad args. */


#endif
    return ret;

} /* END test_wc_CammeliaSetKey */

/*
 * Testing wc_CamelliaSetIV()
 */
static int test_wc_CamelliaSetIV (void)
{
    int ret = 0;
#ifdef HAVE_CAMELLIA
    Camellia    camellia;
    static const byte iv[] =
    {
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
        0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F
    };

    printf(testingFmt, "wc_CamelliaSetIV()");

    ret = wc_CamelliaSetIV(&camellia, iv);
    if (ret == 0) {
        ret = wc_CamelliaSetIV(&camellia, NULL);
    }
    /* Bad args. */
    if (ret == 0) {
        ret = wc_CamelliaSetIV(NULL, NULL);
        if (ret != BAD_FUNC_ARG) {
            ret = WOLFSSL_FATAL_ERROR;
        } else {
            ret = 0;
        }
    }

    printf(resultFmt, ret == 0 ? passed : failed);

#endif
    return ret;
} /*END test_wc_CamelliaSetIV*/

/*
 * Test wc_CamelliaEncryptDirect and wc_CamelliaDecryptDirect
 */
static int test_wc_CamelliaEncryptDecryptDirect (void)
{
    int     ret = 0;
#ifdef HAVE_CAMELLIA
    Camellia camellia;
    static const byte key24[] =
    {
        0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,
        0xfe, 0xdc, 0xba, 0x98, 0x76, 0x54, 0x32, 0x10,
        0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77
    };
    static const byte iv[] =
    {
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
        0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F
    };
    static const byte plainT[] =
    {
        0x6B, 0xC1, 0xBE, 0xE2, 0x2E, 0x40, 0x9F, 0x96,
        0xE9, 0x3D, 0x7E, 0x11, 0x73, 0x93, 0x17, 0x2A
    };
    byte    enc[sizeof(plainT)];
    byte    dec[sizeof(enc)];
    int     camE = WOLFSSL_FATAL_ERROR;
    int     camD = WOLFSSL_FATAL_ERROR;

    /*Init stack variables.*/
    XMEMSET(enc, 0, 16);
    XMEMSET(enc, 0, 16);

    ret = wc_CamelliaSetKey(&camellia, key24, (word32)sizeof(key24), iv);
    if (ret == 0) {
        ret = wc_CamelliaEncryptDirect(&camellia, enc, plainT);
        if (ret == 0) {
            ret = wc_CamelliaDecryptDirect(&camellia, dec, enc);
            if (XMEMCMP(plainT, dec, CAMELLIA_BLOCK_SIZE)) {
                ret = WOLFSSL_FATAL_ERROR;
            }
        }
    }
    printf(testingFmt, "wc_CamelliaEncryptDirect()");
    /* Pass bad args. */
    if (ret == 0) {
        camE = wc_CamelliaEncryptDirect(NULL, enc, plainT);
        if (camE == BAD_FUNC_ARG) {
            camE = wc_CamelliaEncryptDirect(&camellia, NULL, plainT);
        }
        if (camE == BAD_FUNC_ARG) {
            camE = wc_CamelliaEncryptDirect(&camellia, enc, NULL);
        }
        if (camE == BAD_FUNC_ARG) {
            camE = 0;
        } else {
            camE = WOLFSSL_FATAL_ERROR;
        }
    }

    printf(resultFmt, camE == 0 ? passed : failed);
    if (camE != 0) {
        return camE;
    }

    printf(testingFmt, "wc_CamelliaDecryptDirect()");

    if (ret == 0) {
        camD = wc_CamelliaDecryptDirect(NULL, dec, enc);
        if (camD == BAD_FUNC_ARG) {
            camD = wc_CamelliaDecryptDirect(&camellia, NULL, enc);
        }
        if (camD == BAD_FUNC_ARG) {
            camD = wc_CamelliaDecryptDirect(&camellia, dec, NULL);
        }
        if (camD == BAD_FUNC_ARG) {
            camD = 0;
        } else {
            camD = WOLFSSL_FATAL_ERROR;
        }
    }

    printf(resultFmt, camD == 0 ? passed : failed);
    if (camD != 0) {
        return camD;
    }

#endif
    return ret;

} /* END test-wc_CamelliaEncryptDecryptDirect */

/*
 * Testing wc_CamelliaCbcEncrypt and wc_CamelliaCbcDecrypt
 */
static int test_wc_CamelliaCbcEncryptDecrypt (void)
{
    int     ret = 0;
#ifdef HAVE_CAMELLIA
    Camellia camellia;
    static const byte key24[] =
    {
        0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,
        0xfe, 0xdc, 0xba, 0x98, 0x76, 0x54, 0x32, 0x10,
        0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77
    };
    static const byte plainT[] =
    {
        0x6B, 0xC1, 0xBE, 0xE2, 0x2E, 0x40, 0x9F, 0x96,
        0xE9, 0x3D, 0x7E, 0x11, 0x73, 0x93, 0x17, 0x2A
    };
    byte    enc[CAMELLIA_BLOCK_SIZE];
    byte    dec[CAMELLIA_BLOCK_SIZE];
    int     camCbcE = WOLFSSL_FATAL_ERROR;
    int     camCbcD = WOLFSSL_FATAL_ERROR;

    /* Init stack variables. */
    XMEMSET(enc, 0, CAMELLIA_BLOCK_SIZE);
    XMEMSET(enc, 0, CAMELLIA_BLOCK_SIZE);

    ret = wc_CamelliaSetKey(&camellia, key24, (word32)sizeof(key24), NULL);
    if (ret == 0) {
        ret = wc_CamelliaCbcEncrypt(&camellia, enc, plainT, CAMELLIA_BLOCK_SIZE);
        if (ret != 0) {
            ret = WOLFSSL_FATAL_ERROR;
        }
    }
    if (ret == 0) {
        ret = wc_CamelliaSetKey(&camellia, key24, (word32)sizeof(key24), NULL);
        if (ret == 0) {
            ret = wc_CamelliaCbcDecrypt(&camellia, dec, enc, CAMELLIA_BLOCK_SIZE);
            if (XMEMCMP(plainT, dec, CAMELLIA_BLOCK_SIZE)) {
                ret = WOLFSSL_FATAL_ERROR;
            }
        }
    }

    printf(testingFmt, "wc_CamelliaCbcEncrypt");
    /* Pass in bad args. */
    if (ret == 0) {
        camCbcE = wc_CamelliaCbcEncrypt(NULL, enc, plainT, CAMELLIA_BLOCK_SIZE);
        if (camCbcE == BAD_FUNC_ARG) {
            camCbcE = wc_CamelliaCbcEncrypt(&camellia, NULL, plainT,
                                                    CAMELLIA_BLOCK_SIZE);
        }
        if (camCbcE == BAD_FUNC_ARG) {
            camCbcE = wc_CamelliaCbcEncrypt(&camellia, enc, NULL,
                                                    CAMELLIA_BLOCK_SIZE);
        }
        if (camCbcE == BAD_FUNC_ARG) {
            camCbcE = 0;
        } else {
            camCbcE = WOLFSSL_FATAL_ERROR;
        }
    }

    printf(resultFmt, camCbcE == 0 ? passed : failed);
    if (camCbcE != 0) {
        return camCbcE;
    }

    printf(testingFmt, "wc_CamelliaCbcDecrypt()");

    if (ret == 0) {
        camCbcD = wc_CamelliaCbcDecrypt(NULL, dec, enc, CAMELLIA_BLOCK_SIZE);
        if (camCbcD == BAD_FUNC_ARG) {
            camCbcD = wc_CamelliaCbcDecrypt(&camellia, NULL, enc,
                                                    CAMELLIA_BLOCK_SIZE);
        }
        if (camCbcD == BAD_FUNC_ARG) {
            camCbcD = wc_CamelliaCbcDecrypt(&camellia, dec, NULL,
                                                    CAMELLIA_BLOCK_SIZE);
        }
        if (camCbcD == BAD_FUNC_ARG) {
            camCbcD = 0;
        } else {
            camCbcD = WOLFSSL_FATAL_ERROR;
        }
    } /* END bad args. */

    printf(resultFmt, camCbcD == 0 ? passed : failed);
    if (camCbcD != 0) {
        return camCbcD;
    }

#endif
    return ret;

} /* END test_wc_CamelliaCbcEncryptDecrypt */

/*
 * Testing wc_RabbitSetKey()
 */
static int test_wc_RabbitSetKey (void)
{
    int     ret = 0;
#ifndef NO_RABBIT
    Rabbit  rabbit;
    const char* key =  "\xAC\xC3\x51\xDC\xF1\x62\xFC\x3B"
                        "\xFE\x36\x3D\x2E\x29\x13\x28\x91";
    const char* iv =   "\x59\x7E\x26\xC1\x75\xF5\x73\xC3";

    printf(testingFmt, "wc_RabbitSetKey()");

    ret = wc_RabbitSetKey(&rabbit, (byte*)key, (byte*)iv);

    /* Test bad args. */
    if (ret == 0) {
        ret = wc_RabbitSetKey(NULL, (byte*)key, (byte*)iv);
        if (ret == BAD_FUNC_ARG) {
            ret = wc_RabbitSetKey(&rabbit, NULL, (byte*)iv);
        }
        if (ret == BAD_FUNC_ARG) {
            ret = wc_RabbitSetKey(&rabbit, (byte*)key, NULL);
        }
    }

    printf(resultFmt, ret == 0 ? passed : failed);

#endif
    return ret;

} /* END test_wc_RabbitSetKey */

/*
 * Test wc_RabbitProcess()
 */
static int test_wc_RabbitProcess (void)
{
    int     ret = 0;
#ifndef NO_RABBIT
    Rabbit  enc, dec;
    byte    cipher[25];
    byte    plain[25];
    const char* key     =  "\xAC\xC3\x51\xDC\xF1\x62\xFC\x3B"
                            "\xFE\x36\x3D\x2E\x29\x13\x28\x91";
    const char* iv      =   "\x59\x7E\x26\xC1\x75\xF5\x73\xC3";
    const char* input   =   "Everyone gets Friday off.";
    unsigned long int inlen = XSTRLEN(input);

    /* Initialize stack variables. */
    XMEMSET(cipher, 0, sizeof(cipher));
    XMEMSET(plain, 0, sizeof(plain));

    printf(testingFmt, "wc_RabbitProcess()");

    ret = wc_RabbitSetKey(&enc, (byte*)key, (byte*)iv);
    if (ret == 0) {
        ret = wc_RabbitSetKey(&dec, (byte*)key, (byte*)iv);
    }
    if (ret == 0) {
       ret = wc_RabbitProcess(&enc, cipher, (byte*)input, (word32)inlen);
    }
    if (ret == 0) {
        ret = wc_RabbitProcess(&dec, plain, cipher, (word32)inlen);
        if (ret != 0 || XMEMCMP(input, plain, inlen)) {
            ret = WOLFSSL_FATAL_ERROR;
        } else {
            ret = 0;
        }
    }
    /* Test bad args. */
    if (ret == 0) {
        ret = wc_RabbitProcess(NULL, plain, cipher, (word32)inlen);
        if (ret == BAD_FUNC_ARG) {
            ret = wc_RabbitProcess(&dec, NULL, cipher, (word32)inlen);
        }
        if (ret == BAD_FUNC_ARG) {
            ret = wc_RabbitProcess(&dec, plain, NULL, (word32)inlen);
        }
        if (ret == BAD_FUNC_ARG) {
            ret = 0;
        } else {
            ret = WOLFSSL_FATAL_ERROR;
        }
    }

    printf(resultFmt, ret == 0 ? passed : failed);

#endif
    return ret;

} /* END test_wc_RabbitProcess */





/*
 * Testing wc_Arc4SetKey()
 */
static int test_wc_Arc4SetKey (void)
{
    int ret = 0;
#ifndef NO_RC4
    Arc4 arc;
    const char* key = "\x01\x23\x45\x67\x89\xab\xcd\xef";
    int keyLen = 8;

    printf(testingFmt, "wc_Arch4SetKey()");

    ret = wc_Arc4SetKey(&arc, (byte*)key, keyLen);
    /* Test bad args. */
    if (ret == 0) {
        ret = wc_Arc4SetKey(NULL, (byte*)key, keyLen);
        if (ret == BAD_FUNC_ARG) {
            ret = wc_Arc4SetKey(&arc, NULL, keyLen);
        }
        if (ret == BAD_FUNC_ARG) {
            /* Exits normally if keyLen is incorrect. */
            ret = wc_Arc4SetKey(&arc, (byte*)key, 0);
        } else {
            ret = WOLFSSL_FATAL_ERROR;
        }
    } /* END test bad args. */

    printf(resultFmt, ret == 0 ? passed : failed);

#endif
    return ret;

} /* END test_wc_Arc4SetKey */

/*
 * Testing wc_Arc4Process for ENC/DEC.
 */
static int test_wc_Arc4Process (void)
{
    int ret = 0;
#ifndef NO_RC4
    Arc4 enc, dec;
    const char* key = "\x01\x23\x45\x67\x89\xab\xcd\xef";
    int keyLen = 8;
    const char* input = "\x01\x23\x45\x67\x89\xab\xcd\xef";
    byte cipher[8];
    byte plain[8];

    /* Init stack variables */
    XMEMSET(cipher, 0, sizeof(cipher));
    XMEMSET(plain, 0, sizeof(plain));

    /* Use for async. */
    ret = wc_Arc4Init(&enc, NULL, INVALID_DEVID);
    if (ret == 0) {
        ret = wc_Arc4Init(&dec, NULL, INVALID_DEVID);
    }

    printf(testingFmt, "wc_Arc4Process()");

    if (ret == 0) {
        ret = wc_Arc4SetKey(&enc, (byte*)key, keyLen);
    }
    if (ret == 0) {
        ret = wc_Arc4SetKey(&dec, (byte*)key, keyLen);
    }
    if (ret == 0) {
        ret = wc_Arc4Process(&enc, cipher, (byte*)input, keyLen);
    }
    if (ret == 0) {
        ret = wc_Arc4Process(&dec, plain, cipher, keyLen);
        if (ret != 0 || XMEMCMP(plain, input, keyLen)) {
            ret = WOLFSSL_FATAL_ERROR;
        } else {
            ret = 0;
        }
    }

    /* Bad args. */
    if (ret == 0) {
        ret = wc_Arc4Process(NULL, plain, cipher, keyLen);
        if (ret == BAD_FUNC_ARG) {
            ret = wc_Arc4Process(&dec, NULL, cipher, keyLen);
        }
        if (ret == BAD_FUNC_ARG) {
            ret = wc_Arc4Process(&dec, plain, NULL, keyLen);
        }
        if (ret == BAD_FUNC_ARG) {
            ret = 0;
        } else {
            ret = WOLFSSL_FATAL_ERROR;
        }
    }

    printf(resultFmt, ret == 0 ? passed : failed);

    wc_Arc4Free(&enc);
    wc_Arc4Free(&dec);

#endif
    return ret;

}/* END test_wc_Arc4Process */


/*
 * Testing wc_Init RsaKey()
 */
static int test_wc_InitRsaKey (void)
{
    int     ret = 0;
#ifndef NO_RSA
    RsaKey  key;

    printf(testingFmt, "wc_InitRsaKey()");

    ret = wc_InitRsaKey(&key, NULL);

    /* Test bad args. */
    if (ret == 0) {
        ret = wc_InitRsaKey(NULL, NULL);
        #ifndef HAVE_USER_RSA
            if (ret == BAD_FUNC_ARG) {
                ret = 0;
            } else {
        #else
            if (ret == USER_CRYPTO_ERROR) {
                ret = 0;
            } else {
        #endif
            ret = WOLFSSL_FATAL_ERROR;
        }
    } /* end if */

    if (wc_FreeRsaKey(&key) || ret != 0) {
        ret = WOLFSSL_FATAL_ERROR;
    }

    printf(resultFmt, ret == 0 ? passed : failed);

#endif
    return ret;
} /* END test_wc_InitRsaKey */


/*
 * Testing wc_RsaPrivateKeyDecode()
 */
static int test_wc_RsaPrivateKeyDecode (void)
{
    int     ret = 0;
#if !defined(NO_RSA) && (defined(USE_CERT_BUFFERS_1024)\
        || defined(USE_CERT_BUFFERS_2048)) && !defined(HAVE_FIPS)
    RsaKey  key;
    byte*   tmp;
    word32  idx = 0;
    int     bytes = 0;

    printf(testingFmt, "wc_RsaPrivateKeyDecode()");

    tmp = (byte*)XMALLOC(FOURK_BUF, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    if (tmp == NULL) {
        ret = WOLFSSL_FATAL_ERROR;
    }
    if (ret == 0) {
        ret = wc_InitRsaKey(&key, NULL);
    }
    if (ret == 0) {
        #ifdef USE_CERT_BUFFERS_1024
            XMEMCPY(tmp, client_key_der_1024, sizeof_client_key_der_1024);
            bytes = sizeof_client_key_der_1024;
        #else
            XMEMCPY(tmp, client_key_der_2048, sizeof_client_key_der_2048);
            bytes = sizeof_client_key_der_2048;
        #endif /* Use cert buffers. */

        ret = wc_RsaPrivateKeyDecode(tmp, &idx, &key, (word32)bytes);
    }
    #ifndef HAVE_USER_RSA
        /* Test bad args. */
        if (ret == 0) {
            ret = wc_RsaPrivateKeyDecode(NULL, &idx, &key, (word32)bytes);
            if (ret == ASN_PARSE_E) {
                ret = wc_RsaPrivateKeyDecode(tmp, NULL, &key, (word32)bytes);
            }
            if (ret == BAD_FUNC_ARG) {
                ret = wc_RsaPrivateKeyDecode(tmp, &idx, NULL, (word32)bytes);
            }
            if (ret == ASN_PARSE_E) {
                ret = 0;
            } else {
                ret = WOLFSSL_FATAL_ERROR;
            }
        }
    #else
        /* Test bad args. User RSA. */
        if (ret == 0) {
            ret = wc_RsaPrivateKeyDecode(NULL, &idx, &key, (word32)bytes);
            if (ret == USER_CRYPTO_ERROR) {
                ret = wc_RsaPrivateKeyDecode(tmp, NULL, &key, (word32)bytes);
            }
            if (ret == USER_CRYPTO_ERROR) {
                ret = wc_RsaPrivateKeyDecode(tmp, &idx, NULL, (word32)bytes);
            }
            if (ret == USER_CRYPTO_ERROR) {
                ret = 0;
            } else {
                ret = WOLFSSL_FATAL_ERROR;
            }
        }
    #endif

    if (tmp != NULL) {
        XFREE(tmp, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    }
    if (wc_FreeRsaKey(&key) || ret != 0) {
        ret = WOLFSSL_FATAL_ERROR;
    }

    printf(resultFmt, ret == 0 ? passed : failed);

#endif
    return ret;

} /* END test_wc_RsaPrivateKeyDecode */

/*
 * Testing wc_RsaPublicKeyDecode()
 */
static int test_wc_RsaPublicKeyDecode (void)
{
    int     ret = 0;
#if !defined(NO_RSA) && (defined(USE_CERT_BUFFERS_1024)\
        || defined(USE_CERT_BUFFERS_2048)) && !defined(HAVE_FIPS)
    RsaKey  keyPub;
    byte*   tmp;
    word32  idx = 0;
    int     bytes = 0;
    word32  keySz = 0;
    word32  tstKeySz = 0;

    tmp = (byte*)XMALLOC(GEN_BUF, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    if (tmp == NULL) {
        ret = WOLFSSL_FATAL_ERROR;
    }
    if (ret == 0) {
        ret = wc_InitRsaKey(&keyPub, NULL);
    }
    if (ret == 0) {
        #ifdef USE_CERT_BUFFERS_1024
            XMEMCPY(tmp, client_keypub_der_1024, sizeof_client_keypub_der_1024);
            bytes = sizeof_client_keypub_der_1024;
            keySz = 1024;
        #else
            XMEMCPY(tmp, client_keypub_der_2048, sizeof_client_keypub_der_2048);
            bytes = sizeof_client_keypub_der_2048;
            keySz = 2048;
        #endif

        printf(testingFmt, "wc_RsaPublicKeyDecode()");

        ret = wc_RsaPublicKeyDecode(tmp, &idx, &keyPub, (word32)bytes);
    }
    #ifndef HAVE_USER_RSA
        /* Pass in bad args. */
        if (ret == 0) {
            ret = wc_RsaPublicKeyDecode(NULL, &idx, &keyPub, (word32)bytes);
            if (ret == BAD_FUNC_ARG) {
                ret = wc_RsaPublicKeyDecode(tmp, NULL, &keyPub, (word32)bytes);
            }
            if (ret == BAD_FUNC_ARG) {
                ret = wc_RsaPublicKeyDecode(tmp, &idx, NULL, (word32)bytes);
            }
            if (ret == BAD_FUNC_ARG) {
                ret = 0;
            } else {
                ret = WOLFSSL_FATAL_ERROR;
            }
        }
    #else
        /* Pass in bad args. */
        if (ret == 0) {
            ret = wc_RsaPublicKeyDecode(NULL, &idx, &keyPub, (word32)bytes);
            if (ret == USER_CRYPTO_ERROR) {
                ret = wc_RsaPublicKeyDecode(tmp, NULL, &keyPub, (word32)bytes);
            }
            if (ret == USER_CRYPTO_ERROR) {
                ret = wc_RsaPublicKeyDecode(tmp, &idx, NULL, (word32)bytes);
            }
            if (ret == USER_CRYPTO_ERROR) {
                ret = 0;
            } else {
                ret = WOLFSSL_FATAL_ERROR;
            }
        }
    #endif

    if (wc_FreeRsaKey(&keyPub) || ret != 0) {
        ret = WOLFSSL_FATAL_ERROR;
    }

    if (ret == 0) {
        /* Test for getting modulus key size */
        idx = 0;
        ret = wc_RsaPublicKeyDecode_ex(tmp, &idx, (word32)bytes, NULL,
            &tstKeySz, NULL, NULL);
        ret = (ret == 0 && tstKeySz == keySz/8) ? 0 : WOLFSSL_FATAL_ERROR;
    }

    if (tmp != NULL) {
        XFREE(tmp, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    }

    printf(resultFmt, ret == 0 ? passed : failed);


#endif
    return ret;

}  /* END test_wc_RsaPublicKeyDecode */

/*
 * Testing wc_RsaPublicKeyDecodeRaw()
 */
static int test_wc_RsaPublicKeyDecodeRaw (void)
{
    int         ret = 0;
#if !defined(NO_RSA)
    RsaKey      key;
    const byte  n = 0x23;
    const byte  e = 0x03;
    int         nSz = sizeof(n);
    int         eSz = sizeof(e);

    printf(testingFmt, "wc_RsaPublicKeyDecodeRaw()");

    ret = wc_InitRsaKey(&key, NULL);
    if (ret == 0) {
        ret = wc_RsaPublicKeyDecodeRaw(&n, nSz, &e, eSz, &key);
    }
#ifndef HAVE_USER_RSA
    /* Pass in bad args. */
    if (ret == 0) {
        ret = wc_RsaPublicKeyDecodeRaw(NULL, nSz, &e, eSz, &key);
        if (ret == BAD_FUNC_ARG) {
            ret = wc_RsaPublicKeyDecodeRaw(&n, nSz, NULL, eSz, &key);
        }
        if (ret == BAD_FUNC_ARG) {
            ret = wc_RsaPublicKeyDecodeRaw(&n, nSz, &e, eSz, NULL);
        }
        if (ret == BAD_FUNC_ARG) {
            ret = 0;
        } else {
            ret = WOLFSSL_FATAL_ERROR;
        }
    }
#else
    /* Pass in bad args. User RSA. */
    if (ret == 0) {
        ret = wc_RsaPublicKeyDecodeRaw(NULL, nSz, &e, eSz, &key);
        if (ret == USER_CRYPTO_ERROR) {
            ret = wc_RsaPublicKeyDecodeRaw(&n, nSz, NULL, eSz, &key);
        }
        if (ret == USER_CRYPTO_ERROR) {
            ret = wc_RsaPublicKeyDecodeRaw(&n, nSz, &e, eSz, NULL);
        }
        if (ret == USER_CRYPTO_ERROR) {
            ret = 0;
        } else {
            ret = WOLFSSL_FATAL_ERROR;
        }
    }
#endif

    if (wc_FreeRsaKey(&key) || ret != 0) {
        ret = WOLFSSL_FATAL_ERROR;
    }

    printf(resultFmt, ret == 0 ? passed : failed);

#endif
    return ret;

} /* END test_wc_RsaPublicKeyDecodeRaw */


#if (!defined(NO_RSA) || !defined(HAVE_FAST_RSA)) && defined(WOLFSSL_KEY_GEN)
    /* In FIPS builds, wc_MakeRsaKey() will return an error if it cannot find
     * a probable prime in 5*(modLen/2) attempts. In non-FIPS builds, it keeps
     * trying until it gets a probable prime. */
    #ifdef HAVE_FIPS
        static int MakeRsaKeyRetry(RsaKey* key, int size, long e, WC_RNG* rng)
        {
            int ret;

            for (;;) {
                ret = wc_MakeRsaKey(key, size, e, rng);
                if (ret != PRIME_GEN_E) break;
                printf("MakeRsaKey couldn't find prime; trying again.\n");
            }

            return ret;
        }
        #define MAKE_RSA_KEY(a, b, c, d) MakeRsaKeyRetry(a, b, c, d)
    #else
        #define MAKE_RSA_KEY(a, b, c, d) wc_MakeRsaKey(a, b, c, d)
    #endif
#endif


/*
 * Testing wc_MakeRsaKey()
 */
static int test_wc_MakeRsaKey (void)
{
    int     ret = 0;
#if !defined(NO_RSA) && defined(WOLFSSL_KEY_GEN)

    RsaKey  genKey;
    WC_RNG  rng;
    #ifndef WOLFSSL_SP_MATH
    int     bits = 1024;
    #else
    int     bits = 2048;
    #endif

    printf(testingFmt, "wc_MakeRsaKey()");

    ret = wc_InitRsaKey(&genKey, NULL);
    if (ret == 0) {
        ret = wc_InitRng(&rng);
        if (ret == 0) {
            ret = MAKE_RSA_KEY(&genKey, bits, WC_RSA_EXPONENT, &rng);
            if (ret == 0 && wc_FreeRsaKey(&genKey) != 0) {
                ret = WOLFSSL_FATAL_ERROR;
            }
        }
    }
    #ifndef HAVE_USER_RSA
        /* Test bad args. */
        if (ret == 0) {
            ret = MAKE_RSA_KEY(NULL, bits, WC_RSA_EXPONENT, &rng);
            if (ret == BAD_FUNC_ARG) {
                ret = MAKE_RSA_KEY(&genKey, bits, WC_RSA_EXPONENT, NULL);
            }
            if (ret == BAD_FUNC_ARG) {
                /* e < 3 */
                ret = MAKE_RSA_KEY(&genKey, bits, 2, &rng);
            }
            if (ret == BAD_FUNC_ARG) {
                /* e & 1 == 0 */
                ret = MAKE_RSA_KEY(&genKey, bits, 6, &rng);
            }
            if (ret == BAD_FUNC_ARG) {
                ret = 0;
            } else {
                ret = WOLFSSL_FATAL_ERROR;
            }
        }
    #else
        /* Test bad args. */
        if (ret == 0) {
            ret = MAKE_RSA_KEY(NULL, bits, WC_RSA_EXPONENT, &rng);
            if (ret == USER_CRYPTO_ERROR) {
                ret = MAKE_RSA_KEY(&genKey, bits, WC_RSA_EXPONENT, NULL);
            }
            if (ret == USER_CRYPTO_ERROR) {
                /* e < 3 */
                ret = MAKE_RSA_KEY(&genKey, bits, 2, &rng);
            }
            if (ret == USER_CRYPTO_ERROR) {
                /* e & 1 == 0 */
                ret = MAKE_RSA_KEY(&genKey, bits, 6, &rng);
            }
            if (ret == USER_CRYPTO_ERROR) {
                ret = 0;
            } else {
                ret = WOLFSSL_FATAL_ERROR;
            }
        }
    #endif

    if (wc_FreeRng(&rng) || ret != 0) {
        ret = WOLFSSL_FATAL_ERROR;
    }

    printf(resultFmt, ret == 0 ? passed : failed);

#endif
    return ret;

} /* END test_wc_MakeRsaKey */

/*
 * Test the bounds checking on the cipher text versus the key modulus.
 * 1. Make a new RSA key.
 * 2. Set c to 1.
 * 3. Decrypt c into k. (error)
 * 4. Copy the key modulus to c and sub 1 from the copy.
 * 5. Decrypt c into k. (error)
 * Valid bounds test cases are covered by all the other RSA tests.
 */
static int test_RsaDecryptBoundsCheck(void)
{
    int ret = 0;
#if !defined(NO_RSA) && defined(WC_RSA_NO_PADDING) && \
    (defined(USE_CERT_BUFFERS_1024) || defined(USE_CERT_BUFFERS_2048)) && \
    defined(WOLFSSL_PUBLIC_MP) && !defined(NO_RSA_BOUNDS_CHECK)
    RsaKey key;
    byte flatC[256];
    word32 flatCSz;
    byte out[256];
    word32 outSz = sizeof(out);
    WC_RNG rng;

    printf(testingFmt, "RSA decrypt bounds check");

    ret = wc_InitRng(&rng);

    if (ret == 0)
        ret = wc_InitRsaKey(&key, NULL);

    if (ret == 0) {
        const byte* derKey;
        word32 derKeySz;
        word32 idx = 0;

        #ifdef USE_CERT_BUFFERS_1024
            derKey = server_key_der_1024;
            derKeySz = (word32)sizeof_server_key_der_1024;
            flatCSz = 128;
        #else
            derKey = server_key_der_2048;
            derKeySz = (word32)sizeof_server_key_der_2048;
            flatCSz = 256;
        #endif

        ret = wc_RsaPrivateKeyDecode(derKey, &idx, &key, derKeySz);
    }

    if (ret == 0) {
        XMEMSET(flatC, 0, flatCSz);
        flatC[flatCSz-1] = 1;

        ret = wc_RsaDirect(flatC, flatCSz, out, &outSz, &key,
                           RSA_PRIVATE_DECRYPT, &rng);
    }
    if (ret == RSA_OUT_OF_RANGE_E) {
        mp_int c;
        mp_init_copy(&c, &key.n);
        mp_sub_d(&c, 1, &c);
        mp_to_unsigned_bin(&c, flatC);
        ret = wc_RsaDirect(flatC, sizeof(flatC), out, &outSz, &key,
                           RSA_PRIVATE_DECRYPT, NULL);
        mp_clear(&c);
    }
    if (ret == RSA_OUT_OF_RANGE_E)
        ret = 0;

    if (wc_FreeRsaKey(&key) || wc_FreeRng(&rng) || ret != 0)
        ret = WOLFSSL_FATAL_ERROR;

    printf(resultFmt, ret == 0 ? passed : failed);


#endif
    return ret;

} /* END test_wc_RsaDecryptBoundsCheck */

/*
 * Testing wc_SetKeyUsage()
 */
static int test_wc_SetKeyUsage (void)
{
    int     ret = 0;
#if !defined(NO_RSA) && defined(WOLFSSL_CERT_EXT) && defined(WOLFSSL_CERT_GEN) && !defined(HAVE_FIPS)
    Cert    myCert;

    ret = wc_InitCert(&myCert);

    printf(testingFmt, "wc_SetKeyUsage()");
    if (ret == 0) {
        ret = wc_SetKeyUsage(&myCert, "keyEncipherment,keyAgreement");
        if (ret == 0) {
            ret = wc_SetKeyUsage(&myCert, "digitalSignature,nonRepudiation");
        }
        if (ret == 0) {
            ret = wc_SetKeyUsage(&myCert, "contentCommitment,encipherOnly");
        }
        if (ret == 0) {
            ret = wc_SetKeyUsage(&myCert, "decipherOnly");
        }
        if (ret == 0) {
            ret = wc_SetKeyUsage(&myCert, "cRLSign,keyCertSign");
        }
    }
    /* Test bad args. */
    if (ret == 0) {
        ret = wc_SetKeyUsage(NULL, "decipherOnly");
        if (ret == BAD_FUNC_ARG) {
            ret = wc_SetKeyUsage(&myCert, NULL);
        }
        if (ret == BAD_FUNC_ARG) {
            ret = wc_SetKeyUsage(&myCert, "");
        }
        if (ret == KEYUSAGE_E) {
            ret = wc_SetKeyUsage(&myCert, ",");
        }
        if (ret == KEYUSAGE_E) {
            ret = wc_SetKeyUsage(&myCert, "digitalSignature, cRLSign");
        }
        if (ret == KEYUSAGE_E) {
            ret = 0;
        } else {
            ret = WOLFSSL_FATAL_ERROR;
        }
    }

    printf(resultFmt, ret == 0 ? passed : failed);

#endif
    return ret;

} /* END  test_wc_SetKeyUsage */

/*
 * Testing wc_RsaKeyToDer()
 */
static int test_wc_RsaKeyToDer (void)
{
    int     ret = 0;
#if !defined(NO_RSA) && defined(WOLFSSL_KEY_GEN)
    RsaKey  genKey;
    WC_RNG  rng;
    byte*   der;
    #ifndef WOLFSSL_SP_MATH
    int     bits = 1024;
    word32  derSz = 611;
    /* (2 x 128) + 2 (possible leading 00) + (5 x 64) + 5 (possible leading 00)
       + 3 (e) + 8 (ASN tag) + 10 (ASN length) + 4 seqSz + 3 version */
    #else
    int     bits = 2048;
    word32  derSz = 1196;
    /* (2 x 256) + 2 (possible leading 00) + (5 x 128) + 5 (possible leading 00)
       + 3 (e) + 8 (ASN tag) + 17 (ASN length) + 4 seqSz + 3 version */
    #endif

    der = (byte*)XMALLOC(derSz, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    if (der == NULL) {
        ret = WOLFSSL_FATAL_ERROR;
    }
    /* Init structures. */
    if (ret == 0) {
        ret = wc_InitRsaKey(&genKey, NULL);
    }
        if (ret == 0) {
        ret = wc_InitRng(&rng);
    }
    /* Make key. */
    if (ret == 0) {
        ret = MAKE_RSA_KEY(&genKey, bits, WC_RSA_EXPONENT, &rng);
        if (ret != 0) {
            ret = WOLFSSL_FATAL_ERROR;
        }
    }

    printf(testingFmt, "wc_RsaKeyToDer()");

    if (ret == 0) {
        ret = wc_RsaKeyToDer(&genKey, der, derSz);
        if (ret > 0) {
            ret = 0;
        } else {
            ret = WOLFSSL_FATAL_ERROR;
        }
    }
    #ifndef HAVE_USER_RSA
        /* Pass bad args. */
        if (ret == 0) {
            ret = wc_RsaKeyToDer(NULL, der, FOURK_BUF);
            if (ret == BAD_FUNC_ARG) {
                ret = wc_RsaKeyToDer(&genKey, NULL, FOURK_BUF);
            }
            if (ret == BAD_FUNC_ARG) {
                /* Try Public Key. */
                genKey.type = 0;
                ret = wc_RsaKeyToDer(&genKey, der, FOURK_BUF);
            }
            if (ret == BAD_FUNC_ARG) {
                ret = 0;
            } else {
                ret = WOLFSSL_FATAL_ERROR;
            }
        }
    #else
        /* Pass bad args. */
        if (ret == 0) {
            ret = wc_RsaKeyToDer(NULL, der, FOURK_BUF);
            if (ret == USER_CRYPTO_ERROR) {
                ret = wc_RsaKeyToDer(&genKey, NULL, FOURK_BUF);
            }
            if (ret == USER_CRYPTO_ERROR) {
                /* Try Public Key. */
                genKey.type = 0;
                ret = wc_RsaKeyToDer(&genKey, der, FOURK_BUF);
            }
            if (ret == USER_CRYPTO_ERROR) {
                ret = 0;
            } else {
                ret = WOLFSSL_FATAL_ERROR;
            }
        }
    #endif

    if (der != NULL) {
        XFREE(der, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    }
    if (wc_FreeRsaKey(&genKey) || ret != 0) {
        ret = WOLFSSL_FATAL_ERROR;
    }
    if (wc_FreeRng(&rng) || ret != 0) {
        ret = WOLFSSL_FATAL_ERROR;
    }

    printf(resultFmt, ret == 0 ? passed : failed);

#endif
    return ret;
} /* END test_wc_RsaKeyToDer */

/*
 *  Testing wc_RsaKeyToPublicDer()
 */
static int test_wc_RsaKeyToPublicDer (void)
{
    int         ret = 0;
#if !defined(NO_RSA) && !defined(HAVE_FAST_RSA) && defined(WOLFSSL_KEY_GEN) &&\
     (defined(OPENSSL_EXTRA) || defined(OPENSSL_EXTRA_X509_SMALL))
    RsaKey      key;
    WC_RNG      rng;
    byte*       der;
    #ifndef WOLFSSL_SP_MATH
    int         bits = 1024;
    word32      derLen = 162;
    #else
    int         bits = 2048;
    word32      derLen = 290;
    #endif

    der = (byte*)XMALLOC(derLen, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    if (der == NULL) {
        ret = WOLFSSL_FATAL_ERROR;
    }
    if (ret == 0) {
        ret = wc_InitRsaKey(&key, NULL);
    }
    if (ret == 0) {
        ret = wc_InitRng(&rng);
    }
    if (ret == 0) {
        ret = MAKE_RSA_KEY(&key, bits, WC_RSA_EXPONENT, &rng);
    }

    printf(testingFmt, "wc_RsaKeyToPublicDer()");

    if (ret == 0) {
        ret = wc_RsaKeyToPublicDer(&key, der, derLen);
        if (ret >= 0) {
            ret = 0;
        } else {
            ret = WOLFSSL_FATAL_ERROR;
        }
    }

    #ifndef HAVE_USER_RSA
        /* Pass in bad args. */
        if (ret == 0) {
            ret = wc_RsaKeyToPublicDer(NULL, der, derLen);
            if (ret == BAD_FUNC_ARG) {
                ret = wc_RsaKeyToPublicDer(&key, NULL, derLen);
            }
            if (ret == BAD_FUNC_ARG) {
                ret = wc_RsaKeyToPublicDer(&key, der, -1);
            }
            if (ret == BAD_FUNC_ARG) {
                ret = 0;
            } else {
                ret = WOLFSSL_FATAL_ERROR;
            }
        }
    #else
        /* Pass in bad args. */
        if (ret == 0) {
            ret = wc_RsaKeyToPublicDer(NULL, der, derLen);
            if (ret == USER_CRYPTO_ERROR) {
                ret = wc_RsaKeyToPublicDer(&key, NULL, derLen);
            }
            if (ret == USER_CRYPTO_ERROR) {
                ret = wc_RsaKeyToPublicDer(&key, der, -1);
            }
            if (ret == USER_CRYPTO_ERROR) {
                ret = 0;
            } else {
                ret = WOLFSSL_FATAL_ERROR;
            }
        }
    #endif

    if (der != NULL) {
        XFREE(der, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    }
    if (wc_FreeRsaKey(&key) || ret != 0) {
        ret = WOLFSSL_FATAL_ERROR;
    }
    if (wc_FreeRng(&rng) || ret != 0) {
        ret = WOLFSSL_FATAL_ERROR;
    }

    printf(resultFmt, ret == 0 ? passed : failed);

#endif
    return ret;

} /* END test_wc_RsaKeyToPublicDer */

/*
 *  Testing wc_RsaPublicEncrypt() and wc_RsaPrivateDecrypt()
 */
static int test_wc_RsaPublicEncryptDecrypt (void)
{
    int     ret = 0;
#if !defined(NO_RSA) && defined(WOLFSSL_KEY_GEN)
    RsaKey  key;
    WC_RNG  rng;
    const char* inStr = "Everyone gets Friday off.";
    word32  plainLen = 25;
    word32  inLen = (word32)XSTRLEN(inStr);
    #ifndef WOLFSSL_SP_MATH
    int         bits = 1024;
    word32  cipherLen = 128;
    #else
    int         bits = 2048;
    word32  cipherLen = 256;
    #endif

    DECLARE_VAR_INIT(in, byte, inLen, inStr, NULL);
    DECLARE_VAR(plain, byte, plainLen, NULL);
    DECLARE_VAR(cipher, byte, cipherLen, NULL);

    ret = wc_InitRsaKey(&key, NULL);
    if (ret == 0) {
        ret = wc_InitRng(&rng);
    }
    if (ret == 0) {
        ret = MAKE_RSA_KEY(&key, bits, WC_RSA_EXPONENT, &rng);
    }
    /* Encrypt. */
    printf(testingFmt, "wc_RsaPublicEncrypt()");

    if (ret == 0) {
        ret = wc_RsaPublicEncrypt(in, inLen, cipher, cipherLen, &key, &rng);
        if (ret >= 0) {
            cipherLen = ret;
            ret = 0;
        } else {
            ret = WOLFSSL_FATAL_ERROR;
        }
    }

    /* Pass bad args. */
   /* Tests PsaPublicEncryptEx() which, is tested by another fn. No need dup.*/
    printf(resultFmt, ret == 0 ? passed : failed);
    if (ret != 0) {
        return ret;
    }

    /* Decrypt */
    printf(testingFmt, "wc_RsaPrivateDecrypt()");
    #if defined(WC_RSA_BLINDING) && !defined(HAVE_FIPS)
        /* Bind rng */
        if (ret == 0) {
            ret = wc_RsaSetRNG(&key, &rng);
        }
    #endif
    if (ret == 0) {
        ret = wc_RsaPrivateDecrypt(cipher, cipherLen, plain, plainLen, &key);
    }
    if (ret >= 0) {
        ret = XMEMCMP(plain, inStr, plainLen);
    }

    /* Pass in bad args. */
   /* Tests RsaPrivateDecryptEx() which, is tested by another fn. No need dup.*/

    FREE_VAR(in, NULL);
    FREE_VAR(plain, NULL);
    FREE_VAR(cipher, NULL);
    if (wc_FreeRsaKey(&key) || ret != 0) {
        ret = WOLFSSL_FATAL_ERROR;
    }
    if (wc_FreeRng(&rng) || ret != 0) {
        ret = WOLFSSL_FATAL_ERROR;
    }

    printf(resultFmt, ret == 0 ? passed : failed);

#endif
    return ret;

} /* END test_wc_RsaPublicEncryptDecrypt */

/*
 * Testing wc_RsaPrivateDecrypt_ex() and wc_RsaPrivateDecryptInline_ex()
 */
static int test_wc_RsaPublicEncryptDecrypt_ex (void)
{
    int     ret = 0;
#if !defined(NO_RSA) && defined(WOLFSSL_KEY_GEN) && !defined(HAVE_FIPS)\
        && !defined(WC_NO_RSA_OAEP) && !defined(HAVE_USER_RSA)\
        && !defined(NO_SHA)
    RsaKey  key;
    WC_RNG  rng;
    const char* inStr = "Everyone gets Friday off.";
    word32  inLen = (word32)XSTRLEN(inStr);
    const word32 plainSz = 25;
    byte*   res = NULL;
    int     idx = 0;
    #ifndef WOLFSSL_SP_MATH
    int          bits = 1024;
    const word32 cipherSz = 128;
    #else
    int          bits = 2048;
    const word32 cipherSz = 256;
    #endif

    DECLARE_VAR_INIT(in, byte, inLen, inStr, NULL);
    DECLARE_VAR(plain, byte, plainSz, NULL);
    DECLARE_VAR(cipher, byte, cipherSz, NULL);

    /* Initialize stack structures. */
    XMEMSET(&rng, 0, sizeof(rng));
    XMEMSET(&key, 0, sizeof(key));

    ret = wc_InitRsaKey_ex(&key, NULL, INVALID_DEVID);
    if (ret == 0) {
        ret = wc_InitRng(&rng);
    }
    if (ret == 0) {
        ret = MAKE_RSA_KEY(&key, bits, WC_RSA_EXPONENT, &rng);
    }
    /* Encrypt */
    printf(testingFmt, "wc_RsaPublicEncrypt_ex()");
    if (ret == 0) {
        ret = wc_RsaPublicEncrypt_ex(in, inLen, cipher, cipherSz, &key, &rng,
                WC_RSA_OAEP_PAD, WC_HASH_TYPE_SHA, WC_MGF1SHA1, NULL, 0);
        if (ret >= 0) {
            idx = ret;
            ret = 0;
        } else {
            ret = WOLFSSL_FATAL_ERROR;
        }
    }

    /*Pass bad args.*/
   /* Tests RsaPublicEncryptEx again. No need duplicate. */
    printf(resultFmt, ret == 0 ? passed : failed);
    if (ret != 0) {
        return ret;
    }

    /* Decrypt */
    printf(testingFmt, "wc_RsaPrivateDecrypt_ex()");
    #if defined(WC_RSA_BLINDING) && !defined(HAVE_FIPS)
        if (ret == 0) {
            ret = wc_RsaSetRNG(&key, &rng);
        }
    #endif
    if (ret == 0) {
        ret = wc_RsaPrivateDecrypt_ex(cipher, (word32)idx,
                plain, plainSz, &key, WC_RSA_OAEP_PAD, WC_HASH_TYPE_SHA,
                WC_MGF1SHA1, NULL, 0);
    }
   if (ret >= 0) {
        if (!XMEMCMP(plain, inStr, plainSz)) {
            ret = 0;
        } else {
            ret = WOLFSSL_FATAL_ERROR;
        }
    }

    /*Pass bad args.*/
   /* Tests RsaPrivateDecryptEx() again. No need duplicate. */
    printf(resultFmt, ret == 0 ? passed : failed);
    if (ret != 0) {
        return ret;
    }

    printf(testingFmt, "wc_RsaPrivateDecryptInline_ex()");
    if (ret == 0) {
        ret = wc_RsaPrivateDecryptInline_ex(cipher, (word32)idx,
                &res, &key, WC_RSA_OAEP_PAD, WC_HASH_TYPE_SHA,
                WC_MGF1SHA1, NULL, 0);

        if (ret >= 0) {
            if (!XMEMCMP(inStr, res, plainSz)) {
                ret = 0;
            } else {
                ret = WOLFSSL_FATAL_ERROR;
            }
        }
    }

    FREE_VAR(in, NULL);
    FREE_VAR(plain, NULL);
    FREE_VAR(cipher, NULL);
    if (wc_FreeRsaKey(&key) || ret != 0) {
        ret = WOLFSSL_FATAL_ERROR;
    }
    if (wc_FreeRng(&rng) || ret != 0) {
        ret = WOLFSSL_FATAL_ERROR;
    }

    printf(resultFmt, ret == 0 ? passed : failed);

#endif
    return ret;

} /* END test_wc_RsaPublicEncryptDecrypt_ex */

/*
 * Tesing wc_RsaSSL_Sign() and wc_RsaSSL_Verify()
 */
static int test_wc_RsaSSL_SignVerify (void)
{
    int     ret = 0;
#if !defined(NO_RSA) && defined(WOLFSSL_KEY_GEN)
    RsaKey  key;
    WC_RNG  rng;
    const char* inStr = "Everyone gets Friday off.";
    const word32 plainSz = 25;
    word32  inLen = (word32)XSTRLEN(inStr);
    word32  idx = 0;
    #ifndef WOLFSSL_SP_MATH
    int          bits = 1024;
    const word32 outSz = 128;
    #else
    int          bits = 2048;
    const word32 outSz = 256;
    #endif

    DECLARE_VAR_INIT(in, byte, inLen, inStr, NULL);
    DECLARE_VAR(out, byte, outSz, NULL);
    DECLARE_VAR(plain, byte, plainSz, NULL);

    ret = wc_InitRsaKey(&key, NULL);

    if (ret == 0) {
        ret = wc_InitRng(&rng);
    }

    if (ret == 0) {
        ret = MAKE_RSA_KEY(&key, bits, WC_RSA_EXPONENT, &rng);
    }
    /* Sign. */
    printf(testingFmt, "wc_RsaSSL_Sign()");

    if (ret == 0) {
        ret = wc_RsaSSL_Sign(in, inLen, out, outSz, &key, &rng);
        if (ret == (int)outSz) {
            idx = ret;
            ret = 0;
        } else {
            ret = WOLFSSL_FATAL_ERROR;
        }
    }
#ifndef HAVE_USER_RSA
    /* Test bad args. */
    if (ret == 0) {
        ret = wc_RsaSSL_Sign(NULL, inLen, out, outSz, &key, &rng);
        if (ret == BAD_FUNC_ARG) {
            ret = wc_RsaSSL_Sign(in, 0, out, outSz, &key, &rng);
        }
        if (ret == BAD_FUNC_ARG) {
            ret = wc_RsaSSL_Sign(in, inLen, NULL, outSz, &key, &rng);
        }
        if (ret == BAD_FUNC_ARG) {
            ret = wc_RsaSSL_Sign(in, inLen, out, outSz, NULL, &rng);
        }
        if (ret == BAD_FUNC_ARG) {
            ret = 0;
        } else {
            ret = WOLFSSL_FATAL_ERROR;
        }
    }
#else
    /* Test bad args. */
    if (ret == 0) {
        ret = wc_RsaSSL_Sign(NULL, inLen, out, outSz, &key, &rng);
        if (ret == USER_CRYPTO_ERROR) {
            ret = wc_RsaSSL_Sign(in, 0, out, outSz, &key, &rng);
        }
        if (ret == USER_CRYPTO_ERROR) {
            ret = wc_RsaSSL_Sign(in, inLen, NULL, outSz, &key, &rng);
        }
        if (ret == USER_CRYPTO_ERROR) {
           ret = wc_RsaSSL_Sign(in, inLen, out, outSz, NULL, &rng);
        }
        if (ret == USER_CRYPTO_ERROR) {
            ret = 0;
        } else {
            ret = WOLFSSL_FATAL_ERROR;
        }
    }
#endif
    printf(resultFmt, ret == 0 ? passed : failed);
    if (ret != 0) {
        return ret;
    }

    /* Verify. */
    printf(testingFmt, "wc_RsaSSL_Verify()");

    if (ret == 0) {
        ret = wc_RsaSSL_Verify(out, idx, plain, plainSz, &key);
        if (ret == (int)inLen) {
            ret = 0;
        } else {
            ret = WOLFSSL_FATAL_ERROR;
        }
    }
    #ifndef HAVE_USER_RSA
        /* Pass bad args. */
         if (ret == 0) {
                ret = wc_RsaSSL_Verify(NULL, idx, plain, plainSz, &key);
            if (ret == BAD_FUNC_ARG) {
                ret = wc_RsaSSL_Verify(out, 0, plain, plainSz, &key);
            }
            if (ret == BAD_FUNC_ARG) {
                ret = wc_RsaSSL_Verify(out, idx, NULL, plainSz, &key);
            }
            if (ret == BAD_FUNC_ARG) {
               ret = wc_RsaSSL_Verify(out, idx, plain, plainSz, NULL);
            }
            if (ret == BAD_FUNC_ARG) {
                ret = 0;
            } else {
                ret = WOLFSSL_FATAL_ERROR;
            }
        }
    #else
        /* Pass bad args. */
         if (ret == 0) {
                ret = wc_RsaSSL_Verify(NULL, idx, plain, plainSz, &key);
            if (ret == USER_CRYPTO_ERROR) {
                ret = wc_RsaSSL_Verify(out, 0, plain, plainSz, &key);
            }
            if (ret == USER_CRYPTO_ERROR) {
                ret = wc_RsaSSL_Verify(out, idx, NULL, plainSz, &key);
            }
            if (ret == USER_CRYPTO_ERROR) {
               ret = wc_RsaSSL_Verify(out, idx, plain, plainSz, NULL);
            }
            if (ret == USER_CRYPTO_ERROR) {
                ret = 0;
            } else {
                ret = WOLFSSL_FATAL_ERROR;
            }
        }
    #endif

    FREE_VAR(in, NULL);
    FREE_VAR(out, NULL);
    FREE_VAR(plain, NULL);
    if (wc_FreeRsaKey(&key) || ret != 0) {
        ret = WOLFSSL_FATAL_ERROR;
    }
    if (wc_FreeRng(&rng) || ret != 0) {
        ret = WOLFSSL_FATAL_ERROR;
    }

    printf(resultFmt, ret == 0 ? passed : failed);

#endif
    return ret;

} /* END test_wc_RsaSSL_SignVerify */

/*
 * Testing wc_RsaEncryptSize()
 */
static int test_wc_RsaEncryptSize (void)
{
    int     ret = 0;
#if !defined(NO_RSA) && defined(WOLFSSL_KEY_GEN)
    RsaKey  key;
    WC_RNG  rng;

    ret = wc_InitRsaKey(&key, NULL);

    if (ret == 0) {
        ret = wc_InitRng(&rng);
    }

    printf(testingFmt, "wc_RsaEncryptSize()");
#ifndef WOLFSSL_SP_MATH
    if (ret == 0) {
        ret = MAKE_RSA_KEY(&key, 1024, WC_RSA_EXPONENT, &rng);
        if (ret == 0) {
            ret = wc_RsaEncryptSize(&key);
        }
        if (ret == 128) {
            ret = 0;
        } else {
            ret = WOLFSSL_FATAL_ERROR;
        }
    }
    if (wc_FreeRsaKey(&key) || ret != 0) {
        ret = WOLFSSL_FATAL_ERROR;
    } else {
        ret = 0;
    }
#endif

    if (ret == 0) {
        ret = MAKE_RSA_KEY(&key, 2048, WC_RSA_EXPONENT, &rng);
        if (ret == 0) {
            ret = wc_RsaEncryptSize(&key);
        }
        if (ret == 256) {
            ret = 0;
        } else {
            ret = WOLFSSL_FATAL_ERROR;
        }
    }

    /* Pass in bad arg. */
    if (ret == 0) {
        ret = wc_RsaEncryptSize(NULL);
        #ifndef HAVE_USER_RSA
            if (ret == BAD_FUNC_ARG) {
                ret = 0;
            } else {
                ret = WOLFSSL_FATAL_ERROR;
            }
        #endif
    }

    if (wc_FreeRsaKey(&key) || ret != 0) {
        ret = WOLFSSL_FATAL_ERROR;
    }
    if (wc_FreeRng(&rng) || ret != 0) {
        ret = WOLFSSL_FATAL_ERROR;
    }

    printf(resultFmt, ret == 0 ? passed : failed);

#endif
    return ret;

} /* END test_wc_RsaEncryptSize*/

/*
 * Testing wc_RsaFlattenPublicKey()
 */
static int test_wc_RsaFlattenPublicKey (void)
{
    int     ret = 0;
#if !defined(NO_RSA) && defined(WOLFSSL_KEY_GEN)
    RsaKey  key;
    WC_RNG  rng;
    byte    e[256];
    byte    n[256];
    word32  eSz = sizeof(e);
    word32  nSz = sizeof(n);
    #ifndef WOLFSSL_SP_MATH
    int         bits = 1024;
    #else
    int         bits = 2048;
    #endif

    ret = wc_InitRsaKey(&key, NULL);
    if (ret == 0) {
        ret = wc_InitRng(&rng);
    }

    if (ret == 0) {
        ret = MAKE_RSA_KEY(&key, bits, WC_RSA_EXPONENT, &rng);
        if (ret >= 0) {
            ret = 0;
        } else {
            ret = WOLFSSL_FATAL_ERROR;
        }
    }

    printf(testingFmt, "wc_RsaFlattenPublicKey()");

    if (ret == 0) {
        ret = wc_RsaFlattenPublicKey(&key, e, &eSz, n, &nSz);
    }
    #ifndef HAVE_USER_RSA
        /* Pass bad args. */
        if (ret == 0) {
            ret = wc_RsaFlattenPublicKey(NULL, e, &eSz, n, &nSz);
            if (ret == BAD_FUNC_ARG) {
                ret = wc_RsaFlattenPublicKey(&key, NULL, &eSz, n, &nSz);
            }
            if (ret == BAD_FUNC_ARG) {
                ret = wc_RsaFlattenPublicKey(&key, e, NULL, n, &nSz);
            }
            if (ret == BAD_FUNC_ARG) {
                ret = wc_RsaFlattenPublicKey(&key, e, &eSz, NULL, &nSz);
            }
            if (ret == BAD_FUNC_ARG) {
                ret = wc_RsaFlattenPublicKey(&key, e, &eSz, n, NULL);
            }
            if (ret == BAD_FUNC_ARG) {
                ret = 0;
            } else {
                ret = WOLFSSL_FATAL_ERROR;
            }
        }
    #else
        /* Pass bad args. */
        if (ret == 0) {
            ret = wc_RsaFlattenPublicKey(NULL, e, &eSz, n, &nSz);
            if (ret == USER_CRYPTO_ERROR) {
                ret = wc_RsaFlattenPublicKey(&key, NULL, &eSz, n, &nSz);
            }
            if (ret == USER_CRYPTO_ERROR) {
                ret = wc_RsaFlattenPublicKey(&key, e, NULL, n, &nSz);
            }
            if (ret == USER_CRYPTO_ERROR) {
                ret = wc_RsaFlattenPublicKey(&key, e, &eSz, NULL, &nSz);
            }
            if (ret == USER_CRYPTO_ERROR) {
                ret = wc_RsaFlattenPublicKey(&key, e, &eSz, n, NULL);
            }
            if (ret == USER_CRYPTO_ERROR) {
                ret = 0;
            } else {
                ret = WOLFSSL_FATAL_ERROR;
            }
        }
    #endif
    if (wc_FreeRsaKey(&key) || ret != 0) {
        ret = WOLFSSL_FATAL_ERROR;
    }
    if (wc_FreeRng(&rng) || ret != 0) {
        ret = WOLFSSL_FATAL_ERROR;
    }

    printf(resultFmt, ret == 0 ? passed : failed);

#endif
    return ret;

} /* END test_wc_RsaFlattenPublicKey */



/*
 * unit test for wc_AesCcmSetKey
 */
static int test_wc_AesCcmSetKey (void)
{
    int ret = 0;
#ifdef HAVE_AESCCM
    Aes aes;
    const byte  key16[] =
    {
        0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7,
        0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf
    };
    const byte  key24[] =
    {
        0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
        0x38, 0x39, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66,
        0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37
    };
    const byte  key32[] =
    {
        0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
        0x38, 0x39, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66,
        0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
        0x38, 0x39, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66
    };

    printf(testingFmt, "wc_AesCcmSetKey()");

    ret = wc_AesInit(&aes, NULL, INVALID_DEVID);
    if (ret != 0)
        return ret;

#ifdef WOLFSSL_AES_128
    ret = wc_AesCcmSetKey(&aes, key16, sizeof(key16));
#endif
#ifdef WOLFSSL_AES_192
    if (ret == 0) {
        ret = wc_AesCcmSetKey(&aes, key24, sizeof(key24));
    }
#endif
#ifdef WOLFSSL_AES_256
    if (ret == 0) {
        ret = wc_AesCcmSetKey(&aes, key32, sizeof(key32));
    }
#endif

    /* Test bad args. */
    if (ret == 0) {
        ret = wc_AesCcmSetKey(&aes, key16, sizeof(key16) - 1);
        if (ret == BAD_FUNC_ARG) {
            ret = wc_AesCcmSetKey(&aes, key24, sizeof(key24) - 1);
        }
        if (ret == BAD_FUNC_ARG) {
            ret = wc_AesCcmSetKey(&aes, key32, sizeof(key32) - 1);
        }
        if (ret != BAD_FUNC_ARG) {
            ret = WOLFSSL_FATAL_ERROR;
        } else {
            ret = 0;
        }
    }

    wc_AesFree(&aes);

    printf(resultFmt, ret == 0 ? passed : failed);

#endif
    return ret;

} /* END test_wc_AesCcmSetKey */

/*
 * Unit test function for wc_AesCcmEncrypt and wc_AesCcmDecrypt
 */
static int test_wc_AesCcmEncryptDecrypt (void)
{
    int ret = 0;
#if defined(HAVE_AESCCM) && defined(WOLFSSL_AES_128)
    Aes aes;
    const byte  key16[] =
    {
        0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7,
        0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf
    };
    /* plaintext */
    const byte plainT[] =
    {
        0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
        0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
        0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e
    };
    /* nonce */
    const byte iv[] =
    {
        0x00, 0x00, 0x00, 0x03, 0x02, 0x01, 0x00, 0xa0,
        0xa1, 0xa2, 0xa3, 0xa4, 0xa5
    };
    const byte c[] =  /* cipher text. */
    {
        0x58, 0x8c, 0x97, 0x9a, 0x61, 0xc6, 0x63, 0xd2,
        0xf0, 0x66, 0xd0, 0xc2, 0xc0, 0xf9, 0x89, 0x80,
        0x6d, 0x5f, 0x6b, 0x61, 0xda, 0xc3, 0x84
    };
    const byte t[] =  /* Auth tag */
    {
        0x17, 0xe8, 0xd1, 0x2c, 0xfd, 0xf9, 0x26, 0xe0
    };
    const byte authIn[] =
    {
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07
    };
    byte cipherOut[sizeof(plainT)];
    byte authTag[sizeof(t)];
    int ccmE = WOLFSSL_FATAL_ERROR;
    #ifdef HAVE_AES_DECRYPT
        int ccmD = WOLFSSL_FATAL_ERROR;
        byte plainOut[sizeof(cipherOut)];
    #endif

    ret = wc_AesInit(&aes, NULL, INVALID_DEVID);
    if (ret != 0)
        return ret;

    ret = wc_AesCcmSetKey(&aes, key16, sizeof(key16));
    if (ret == 0) {
        ccmE = wc_AesCcmEncrypt(&aes, cipherOut, plainT, sizeof(cipherOut),
                                    iv, sizeof(iv), authTag, sizeof(authTag),
                                    authIn , sizeof(authIn));
        if ((XMEMCMP(cipherOut, c, sizeof(c)) && ccmE == 0) ||
                XMEMCMP(t, authTag, sizeof(t))) {
            ccmE = WOLFSSL_FATAL_ERROR;
            ret = WOLFSSL_FATAL_ERROR;
        }
        #ifdef HAVE_AES_DECRYPT
            if (ret == 0) {
                ccmD = wc_AesCcmDecrypt(&aes, plainOut, cipherOut,
                                        sizeof(plainOut), iv, sizeof(iv),
                                        authTag, sizeof(authTag),
                                        authIn, sizeof(authIn));
                if (XMEMCMP(plainOut, plainT, sizeof(plainT)) && ccmD == 0) {
                    ccmD = WOLFSSL_FATAL_ERROR;
                }
            }
        #endif
    }

    printf(testingFmt, "wc_AesCcmEncrypt()");

    /* Pass in bad args. Encrypt*/
    if (ret == 0 && ccmE == 0) {
        ccmE = wc_AesCcmEncrypt(NULL, cipherOut, plainT, sizeof(cipherOut),
                                    iv, sizeof(iv), authTag, sizeof(authTag),
                                    authIn , sizeof(authIn));
        if (ccmE == BAD_FUNC_ARG) {
            ccmE = wc_AesCcmEncrypt(&aes, NULL, plainT, sizeof(cipherOut),
                                    iv, sizeof(iv), authTag, sizeof(authTag),
                                    authIn , sizeof(authIn));
        }
        if (ccmE == BAD_FUNC_ARG) {
            ccmE = wc_AesCcmEncrypt(&aes, cipherOut, NULL, sizeof(cipherOut),
                                    iv, sizeof(iv), authTag, sizeof(authTag),
                                    authIn , sizeof(authIn));
        }
        if (ccmE == BAD_FUNC_ARG) {
            ccmE = wc_AesCcmEncrypt(&aes, cipherOut, plainT, sizeof(cipherOut),
                                    NULL, sizeof(iv), authTag, sizeof(authTag),
                                    authIn , sizeof(authIn));
        }
        if (ccmE == BAD_FUNC_ARG) {
            ccmE = wc_AesCcmEncrypt(&aes, cipherOut, plainT, sizeof(cipherOut),
                                    iv, sizeof(iv), NULL, sizeof(authTag),
                                    authIn , sizeof(authIn));
        }
        if (ccmE == BAD_FUNC_ARG) {
            ccmE = wc_AesCcmEncrypt(&aes, cipherOut, plainT, sizeof(cipherOut),
                                    iv, sizeof(iv) + 1, authTag, sizeof(authTag),
                                    authIn , sizeof(authIn));
        }
        if (ccmE == BAD_FUNC_ARG) {
            ccmE = wc_AesCcmEncrypt(&aes, cipherOut, plainT, sizeof(cipherOut),
                                    iv, sizeof(iv) - 7, authTag, sizeof(authTag),
                                    authIn , sizeof(authIn));
        }

        if (ccmE != BAD_FUNC_ARG) {
            ccmE = WOLFSSL_FATAL_ERROR;
        } else {
            ccmE = 0;
        }
    } /* End Encrypt */

    printf(resultFmt, ccmE == 0 ? passed : failed);
    if (ccmE != 0) {
        wc_AesFree(&aes);
        return ccmE;
    }
    #ifdef HAVE_AES_DECRYPT
        printf(testingFmt, "wc_AesCcmDecrypt()");

        /* Pass in bad args. Decrypt*/
        if (ret == 0 && ccmD == 0) {
            ccmD = wc_AesCcmDecrypt(NULL, plainOut, cipherOut, sizeof(plainOut),
                                        iv, sizeof(iv), authTag, sizeof(authTag),
                                        authIn, sizeof(authIn));
            if (ccmD == BAD_FUNC_ARG) {
                ccmD = wc_AesCcmDecrypt(&aes, NULL, cipherOut, sizeof(plainOut),
                                        iv, sizeof(iv), authTag, sizeof(authTag),
                                        authIn, sizeof(authIn));
            }
            if (ccmD == BAD_FUNC_ARG) {
                ccmD = wc_AesCcmDecrypt(&aes, plainOut, NULL, sizeof(plainOut),
                                        iv, sizeof(iv), authTag, sizeof(authTag),
                                        authIn, sizeof(authIn));
            }
            if (ccmD == BAD_FUNC_ARG) {
                ccmD = wc_AesCcmDecrypt(&aes, plainOut, cipherOut,
                                        sizeof(plainOut), NULL, sizeof(iv),
                                        authTag, sizeof(authTag),
                                        authIn, sizeof(authIn));
            }
            if (ccmD == BAD_FUNC_ARG) {
                ccmD = wc_AesCcmDecrypt(&aes, plainOut, cipherOut,
                                        sizeof(plainOut), iv, sizeof(iv), NULL,
                                        sizeof(authTag), authIn, sizeof(authIn));
            }
            if (ccmD == BAD_FUNC_ARG) {
                ccmD = wc_AesCcmDecrypt(&aes, plainOut, cipherOut,
                                        sizeof(plainOut), iv, sizeof(iv) + 1,
                                        authTag, sizeof(authTag),
                                        authIn, sizeof(authIn));
            }
            if (ccmD == BAD_FUNC_ARG) {
                ccmD = wc_AesCcmDecrypt(&aes, plainOut, cipherOut,
                                        sizeof(plainOut), iv, sizeof(iv) - 7,
                                        authTag, sizeof(authTag),
                                        authIn, sizeof(authIn));
            }
            if (ccmD != BAD_FUNC_ARG) {
                ccmD = WOLFSSL_FATAL_ERROR;
            } else {
                ccmD = 0;
            }
        } /* END Decrypt */

        printf(resultFmt, ccmD == 0 ? passed : failed);
        if (ccmD != 0) {
            return ccmD;
        }
    #endif

    wc_AesFree(&aes);

#endif  /* HAVE_AESCCM */

    return ret;

} /* END test_wc_AesCcmEncryptDecrypt */



/*
 * Test wc_Hc128_SetKey()
 */
static int test_wc_Hc128_SetKey (void)
{
    int ret = 0;
#ifdef HAVE_HC128
    HC128 ctx;
    const char* key = "\x80\x00\x00\x00\x00\x00\x00\x00"
                      "\x00\x00\x00\x00\x00\x00\x00\x00";
    const char* iv =  "\x0D\x74\xDB\x42\xA9\x10\x77\xDE"
                      "\x45\xAC\x13\x7A\xE1\x48\xAF\x16";

    printf(testingFmt, "wc_Hc128_SetKey()");
        ret = wc_Hc128_SetKey(&ctx, (byte*)key, (byte*)iv);
        /* Test bad args. */
        if (ret == 0) {
            ret = wc_Hc128_SetKey(NULL, (byte*)key, (byte*)iv);
            if (ret == BAD_FUNC_ARG) {
                ret = wc_Hc128_SetKey(&ctx, NULL, (byte*)iv);
            }
            if (ret == BAD_FUNC_ARG) {
                ret = wc_Hc128_SetKey(&ctx, (byte*)key, NULL);
            }
        }

    printf(resultFmt, ret == 0 ? passed : failed);


#endif
    return ret;

} /* END test_wc_Hc128_SetKey */

/*
 * Testing wc_Hc128_Process()
 */
static int test_wc_Hc128_Process (void)
{
    int ret = 0;
#ifdef HAVE_HC128
    HC128 enc;
    HC128 dec;
    const char* key =  "\x0F\x62\xB5\x08\x5B\xAE\x01\x54"
                       "\xA7\xFA\x4D\xA0\xF3\x46\x99\xEC";
    const char* input = "Encrypt Hc128, and then Decrypt.";
    size_t inlen = XSTRLEN(input) + 1; /* Add null terminator */
    byte cipher[inlen];
    byte plain[inlen];

    printf(testingFmt, "wc_Hc128_Process()");
    ret = wc_Hc128_SetKey(&enc, (byte*)key, NULL);
    if (ret == 0) {
        ret = wc_Hc128_SetKey(&dec, (byte*)key, NULL);
    }
    if (ret == 0) {
        ret = wc_Hc128_Process(&enc, cipher, (byte*)input, (word32)inlen);
        if (ret == 0) {
            ret = wc_Hc128_Process(&dec, plain, cipher, (word32)inlen);
        }
    }

    /* Bad args. */
    if (ret == 0) {
        ret = wc_Hc128_Process(NULL, plain, cipher, (word32)inlen);
        if (ret == BAD_FUNC_ARG) {
            ret = wc_Hc128_Process(&dec, NULL, cipher, (word32)inlen);
        }
        if (ret == BAD_FUNC_ARG) {
            ret = wc_Hc128_Process(&dec, plain, NULL, (word32)inlen);
        }
        if (ret == BAD_FUNC_ARG) {
            ret = 0;
        } else {
            ret = WOLFSSL_FATAL_ERROR;
        }
    }

    printf(resultFmt, ret == 0 ? passed : failed);

   #endif
    return ret;

} /* END test_wc_Hc128_Process */


/*
 * Testing wc_InitDsaKey()
 */
static int test_wc_InitDsaKey (void)
{
    int     ret = 0;

#ifndef NO_DSA
    DsaKey  key;

    printf(testingFmt, "wc_InitDsaKey()");

    ret = wc_InitDsaKey(&key);

    /* Pass in bad args. */
    if (ret == 0) {
        ret = wc_InitDsaKey(NULL);
        if (ret == BAD_FUNC_ARG) {
            ret = 0;
        } else {
            ret = WOLFSSL_FATAL_ERROR;
        }
    }

    printf(resultFmt, ret == 0 ? passed : failed);

    wc_FreeDsaKey(&key);

#endif
    return ret;

} /* END test_wc_InitDsaKey */

/*
 * Testing wc_DsaSign() and wc_DsaVerify()
 */
static int test_wc_DsaSignVerify (void)
{
    int     ret = 0;
#if !defined(NO_DSA)
    DsaKey  key;
    WC_RNG  rng;
    wc_Sha  sha;
    byte    signature[DSA_SIG_SIZE];
    byte    hash[WC_SHA_DIGEST_SIZE];
    word32  idx = 0;
    word32  bytes;
    int      answer;
#ifdef USE_CERT_BUFFERS_1024
    byte    tmp[ONEK_BUF];
    XMEMSET(tmp, 0, sizeof(tmp));
    XMEMCPY(tmp, dsa_key_der_1024, sizeof_dsa_key_der_1024);
    bytes = sizeof_dsa_key_der_1024;
#elif defined(USE_CERT_BUFFERS_2048)
    byte    tmp[TWOK_BUF];
    XMEMSET(tmp, 0, sizeof(tmp));
    XMEMCPY(tmp, dsa_key_der_2048, sizeof_dsa_key_der_2048);
    bytes = sizeof_dsa_key_der_2048;
#else
    byte    tmp[TWOK_BUF];
    XMEMSET(tmp, 0, sizeof(tmp));
    XFILE fp = XFOPEN("./certs/dsa2048.der", "rb");
    if (fp == XBADFILE) {
        return WOLFSSL_BAD_FILE;
    }
    bytes = (word32) XFREAD(tmp, 1, sizeof(tmp), fp);
    XFCLOSE(fp);
#endif /* END USE_CERT_BUFFERS_1024 */

    ret = wc_InitSha(&sha);
    if (ret == 0) {
        ret = wc_ShaUpdate(&sha, tmp, bytes);
        if (ret == 0) {
            ret = wc_ShaFinal(&sha, hash);
        }
        if (ret == 0) {
            ret = wc_InitDsaKey(&key);
        }
        if (ret == 0) {
            ret = wc_DsaPrivateKeyDecode(tmp, &idx, &key, bytes);
        }
        if (ret == 0) {
            ret = wc_InitRng(&rng);
        }
    }

    printf(testingFmt, "wc_DsaSign()");
    /* Sign. */
    if (ret == 0) {
        ret = wc_DsaSign(hash, signature, &key, &rng);
    }

    /* Test bad args. */
    if (ret == 0) {
        ret = wc_DsaSign(NULL, signature, &key, &rng);
        if (ret == BAD_FUNC_ARG) {
            ret = wc_DsaSign(hash, NULL, &key, &rng);
        }
        if (ret == BAD_FUNC_ARG) {
            ret = wc_DsaSign(hash, signature, NULL, &rng);
        }
        if (ret == BAD_FUNC_ARG) {
            ret = wc_DsaSign(hash, signature, &key, NULL);
        }
        if (ret == BAD_FUNC_ARG) {
            ret = 0;
        } else {
            ret = WOLFSSL_FATAL_ERROR;
        }
    }

    printf(resultFmt, ret == 0 ? passed : failed);

    if (ret != 0) {
        return ret;
    }

    /* Verify. */
    printf(testingFmt, "wc_DsaVerify()");

    ret = wc_DsaVerify(hash, signature, &key, &answer);
    if (ret != 0 || answer != 1) {
        ret = WOLFSSL_FATAL_ERROR;
    } else {
        ret = 0;
    }

    /* Pass in bad args. */
    if (ret == 0) {
        ret = wc_DsaVerify(NULL, signature, &key, &answer);
        if (ret == BAD_FUNC_ARG) {
            ret = wc_DsaVerify(hash, NULL, &key, &answer);
        }
        if (ret == BAD_FUNC_ARG) {
            ret = wc_DsaVerify(hash, signature, NULL, &answer);
        }
        if (ret == BAD_FUNC_ARG) {
            ret = wc_DsaVerify(hash, signature, &key, NULL);
        }
        if (ret == BAD_FUNC_ARG) {
            ret = 0;
        } else {
            ret = WOLFSSL_FATAL_ERROR;
        }
    }

    if (wc_FreeRng(&rng) && ret == 0) {
        ret = WOLFSSL_FATAL_ERROR;
    }

    printf(resultFmt, ret == 0 ? passed : failed);

    wc_FreeDsaKey(&key);
    wc_ShaFree(&sha);

#endif
    return ret;

} /* END test_wc_DsaSign */

/*
 * Testing wc_DsaPrivateKeyDecode() and wc_DsaPublicKeyDecode()
 */
static int test_wc_DsaPublicPrivateKeyDecode (void)
{
    int     ret = 0;

#if !defined(NO_DSA)
    DsaKey  key;
    word32  bytes;
    word32  idx  = 0;
    int     priv = WOLFSSL_FATAL_ERROR;
    int     pub  = WOLFSSL_FATAL_ERROR;

#ifdef USE_CERT_BUFFERS_1024
    byte    tmp[ONEK_BUF];
    XMEMCPY(tmp, dsa_key_der_1024, sizeof_dsa_key_der_1024);
    bytes = sizeof_dsa_key_der_1024;
#elif defined(USE_CERT_BUFFERS_2048)
    byte    tmp[TWOK_BUF];
    XMEMCPY(tmp, dsa_key_der_2048, sizeof_dsa_key_der_2048);
    bytes = sizeof_dsa_key_der_2048;
#else
    byte    tmp[TWOK_BUF];
    XMEMSET(tmp, 0, sizeof(tmp));
    XFILE fp = XFOPEN("./certs/dsa2048.der", "rb");
    if (fp == XBADFILE)
    {
        return WOLFSSL_BAD_FILE;
    }
    bytes = (word32) XFREAD(tmp, 1, sizeof(tmp), fp);
    XFCLOSE(fp);
#endif /* END USE_CERT_BUFFERS_1024 */

    ret = wc_InitDsaKey(&key);

    printf(testingFmt, "wc_DsaPrivateKeyDecode()");
    if (ret == 0) {
        priv = wc_DsaPrivateKeyDecode(tmp, &idx, &key, bytes);

        /* Test bad args. */
        if (priv == 0) {
            priv = wc_DsaPrivateKeyDecode(NULL, &idx, &key, bytes);
            if (priv == BAD_FUNC_ARG) {
                priv = wc_DsaPrivateKeyDecode(tmp, NULL, &key, bytes);
            }
            if (priv == BAD_FUNC_ARG) {
                priv = wc_DsaPrivateKeyDecode(tmp, &idx, NULL, bytes);
            }
            if (priv == BAD_FUNC_ARG) {
                priv = wc_DsaPrivateKeyDecode(tmp, &idx, &key, bytes);
            }
            if (priv == ASN_PARSE_E) {
                priv = 0;
            } else {
                priv = WOLFSSL_FATAL_ERROR;
            }
        }
    } /* END Private Key  */
    if (ret == 0) {
        wc_FreeDsaKey(&key);
        ret = wc_InitDsaKey(&key);
    }

    printf(resultFmt, priv == 0 ? passed : failed);

    printf(testingFmt, "wc_DsaPublicKeyDecode()");
    if (ret == 0) {
        idx = 0; /* Reset */
        pub = wc_DsaPublicKeyDecode(tmp, &idx, &key, bytes);
        /* Test bad args. */
        if (pub == 0) {
            pub = wc_DsaPublicKeyDecode(NULL, &idx, &key, bytes);
            if (pub == BAD_FUNC_ARG) {
                pub = wc_DsaPublicKeyDecode(tmp, NULL, &key, bytes);
            }
            if (pub == BAD_FUNC_ARG) {
                pub = wc_DsaPublicKeyDecode(tmp, &idx, NULL, bytes);
            }
            if (pub == BAD_FUNC_ARG) {
                pub = wc_DsaPublicKeyDecode(tmp, &idx, &key, bytes);
            }
            if (pub == ASN_PARSE_E) {
                pub = 0;
            } else {
                pub = WOLFSSL_FATAL_ERROR;
            }
        }

    } /* END Public Key */

    printf(resultFmt, pub == 0 ? passed : failed);

    wc_FreeDsaKey(&key);

#endif
    return ret;

} /* END test_wc_DsaPublicPrivateKeyDecode */


/*
 * Testing wc_MakeDsaKey() and wc_MakeDsaParameters()
 */
static int test_wc_MakeDsaKey (void)
{
    int     ret = 0;

#if !defined(NO_DSA) && defined(WOLFSSL_KEY_GEN)
    DsaKey  genKey;
    WC_RNG  rng;

    ret = wc_InitRng(&rng);
    if (ret == 0) {
        ret = wc_InitDsaKey(&genKey);
    }

    printf(testingFmt, "wc_MakeDsaParameters()");
    if (ret == 0) {
        ret = wc_MakeDsaParameters(&rng, ONEK_BUF, &genKey);
    }
    /* Test bad args. */
    if (ret == 0) {
        ret = wc_MakeDsaParameters(NULL, ONEK_BUF, &genKey);
        if (ret == BAD_FUNC_ARG) {
            ret = wc_MakeDsaParameters(&rng, ONEK_BUF, NULL);
        }
        if (ret == BAD_FUNC_ARG) {
            ret = wc_MakeDsaParameters(&rng, ONEK_BUF + 1, &genKey);
        }
        if (ret == BAD_FUNC_ARG) {
            ret = 0;
        } else {
            ret = WOLFSSL_FATAL_ERROR;
        }
    }

    printf(resultFmt, ret == 0 ? passed : failed);

    printf(testingFmt, "wc_MakeDsaKey()");

    if (ret == 0) {
        ret = wc_MakeDsaKey(&rng, &genKey);
    }

    /* Test bad args. */
    if (ret == 0) {
        ret = wc_MakeDsaKey(NULL, &genKey);
        if (ret == BAD_FUNC_ARG) {
            ret = wc_MakeDsaKey(&rng, NULL);
        }
        if (ret == BAD_FUNC_ARG) {
            ret = 0;
        } else {
            ret = WOLFSSL_FATAL_ERROR;
        }
    }

    if (wc_FreeRng(&rng) && ret == 0) {
        ret = WOLFSSL_FAILURE;
    }

    printf(resultFmt, ret == 0 ? passed : failed);

    wc_FreeDsaKey(&genKey);
#endif
    return ret;
} /* END test_wc_MakeDsaKey */

/*
 * Testing wc_DsaKeyToDer()
 */
static int test_wc_DsaKeyToDer (void)
{
    int     ret = 0;

#if !defined(NO_DSA) && defined(WOLFSSL_KEY_GEN)
    DsaKey  genKey;
    WC_RNG  rng;
    word32  bytes;
    word32  idx = 0;

#ifdef USE_CERT_BUFFERS_1024
    byte    tmp[ONEK_BUF];
    byte    der[ONEK_BUF];
    XMEMSET(tmp, 0, sizeof(tmp));
    XMEMSET(der, 0, sizeof(der));
    XMEMCPY(tmp, dsa_key_der_1024, sizeof_dsa_key_der_1024);
    bytes = sizeof_dsa_key_der_1024;
#elif defined(USE_CERT_BUFFERS_2048)
    byte    tmp[TWOK_BUF];
    byte    der[TWOK_BUF];
    XMEMSET(tmp, 0, sizeof(tmp));
    XMEMSET(der, 0, sizeof(der));
    XMEMCPY(tmp, dsa_key_der_2048, sizeof_dsa_key_der_2048);
    bytes = sizeof_dsa_key_der_2048;
#else
    byte    tmp[TWOK_BUF];
    byte    der[TWOK_BUF];
    XMEMSET(tmp, 0, sizeof(tmp));
    XMEMSET(der, 0, sizeof(der));
    XFILE fp = XFOPEN("./certs/dsa2048.der", "rb");
    if (fp == XBADFILE) {
        return WOLFSSL_BAD_FILE;
    }
    bytes = (word32) XFREAD(tmp, 1, sizeof(tmp), fp);
    XFCLOSE(fp);
#endif /* END USE_CERT_BUFFERS_1024 */

    ret = wc_InitRng(&rng);
    if (ret == 0) {
        ret = wc_InitDsaKey(&genKey);
    }
    if (ret == 0) {
        ret = wc_MakeDsaParameters(&rng, sizeof(tmp), &genKey);
        if (ret == 0) {
            wc_FreeDsaKey(&genKey);
            ret = wc_InitDsaKey(&genKey);
        }
    }
    if (ret == 0) {
        ret = wc_DsaPrivateKeyDecode(tmp, &idx, &genKey, bytes);
    }

    printf(testingFmt, "wc_DsaKeyToDer()");

    if (ret == 0) {
        ret = wc_DsaKeyToDer(&genKey, der, bytes);
        if ( ret >= 0 && ( ret = XMEMCMP(der, tmp, bytes) ) == 0 ) {
            ret = 0;
        }
    }

    /* Test bad args. */
    if (ret == 0) {
        ret = wc_DsaKeyToDer(NULL, der, FOURK_BUF);
        if (ret == BAD_FUNC_ARG) {
            ret = wc_DsaKeyToDer(&genKey, NULL, FOURK_BUF);
        }
        if (ret == BAD_FUNC_ARG) {
            ret = 0;
        } else {
            ret = WOLFSSL_FATAL_ERROR;
        }
    }

    if (wc_FreeRng(&rng) && ret == 0) {
        ret = WOLFSSL_FATAL_ERROR;
    }

    printf(resultFmt, ret == 0 ? passed : failed);

    wc_FreeDsaKey(&genKey);

#endif

    return ret;

} /* END test_wc_DsaKeyToDer */

/*
 *  Testing wc_DsaKeyToPublicDer()
 *  (indirectly testing setDsaPublicKey())
 */
static int test_wc_DsaKeyToPublicDer(void)
{
    int         ret = 0;
#ifndef HAVE_SELFTEST
#if !defined(NO_DSA) && defined(WOLFSSL_KEY_GEN)
    DsaKey  genKey;
    WC_RNG  rng;
    byte*   der;

    printf(testingFmt, "wc_DsaKeyToPublicDer()");

    der = (byte*)XMALLOC(ONEK_BUF, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    if (der == NULL) {
        ret = WOLFSSL_FATAL_ERROR;
    }
    if (ret == 0) {
        ret = wc_InitDsaKey(&genKey);
    }
    if (ret == 0) {
        ret = wc_InitRng(&rng);
    }
    if (ret == 0) {
        ret = wc_MakeDsaParameters(&rng, ONEK_BUF, &genKey);
    }
    if (ret == 0) {
        ret = wc_MakeDsaKey(&rng, &genKey);
    }

    if (ret == 0) {
        ret = wc_DsaKeyToPublicDer(&genKey, der, ONEK_BUF);
        if (ret >= 0) {
            ret = 0;
        } else {
            ret = WOLFSSL_FATAL_ERROR;
        }
    }

    /* Test bad args. */
    if (ret == 0) {
        ret = wc_DsaKeyToPublicDer(NULL, der, FOURK_BUF);
        if (ret == BAD_FUNC_ARG) {
            ret = wc_DsaKeyToPublicDer(&genKey, NULL, FOURK_BUF);
        }
        if (ret == BAD_FUNC_ARG) {
            ret = 0;
        } else {
            ret = WOLFSSL_FATAL_ERROR;
        }
    }

    if (wc_FreeRng(&rng) && ret == 0) {
        ret = WOLFSSL_FATAL_ERROR;
    }

    printf(resultFmt, ret == 0 ? passed : failed);

    XFREE(der,NULL,DYNAMIC_TYPE_TMP_BUFFER);
    wc_FreeDsaKey(&genKey);
#endif /* !defined(NO_DSA) && defined(WOLFSSL_KEY_GEN) */
#endif /* HAVE_SELFTEST */
    return ret;

} /* END test_wc_DsaKeyToPublicDer */

/*
 * Testing wc_DsaImportParamsRaw()
 */
static int test_wc_DsaImportParamsRaw (void)
{
    int     ret = 0;

#if !defined(NO_DSA)
    DsaKey  key;

    /* [mod = L=1024, N=160], from CAVP KeyPair */
    const char* p = "d38311e2cd388c3ed698e82fdf88eb92b5a9a483dc88005d"
                    "4b725ef341eabb47cf8a7a8a41e792a156b7ce97206c4f9c"
                    "5ce6fc5ae7912102b6b502e59050b5b21ce263dddb2044b6"
                    "52236f4d42ab4b5d6aa73189cef1ace778d7845a5c1c1c71"
                    "47123188f8dc551054ee162b634d60f097f719076640e209"
                    "80a0093113a8bd73";
    const char* q = "96c5390a8b612c0e422bb2b0ea194a3ec935a281";
    const char* g = "06b7861abbd35cc89e79c52f68d20875389b127361ca66822"
                    "138ce4991d2b862259d6b4548a6495b195aa0e0b6137ca37e"
                    "b23b94074d3c3d300042bdf15762812b6333ef7b07ceba786"
                    "07610fcc9ee68491dbc1e34cd12615474e52b18bc934fb00c"
                    "61d39e7da8902291c4434a4e2224c3f4fd9f93cd6f4f17fc0"
                    "76341a7e7d9";

    /* invalid p and q parameters */
    const char* invalidP = "d38311e2cd388c3ed698e82fdf88eb92b5a9a483dc88005d";
    const char* invalidQ = "96c5390a";

    printf(testingFmt, "wc_DsaImportParamsRaw()");

    ret = wc_InitDsaKey(&key);
    if (ret == 0) {
        ret = wc_DsaImportParamsRaw(&key, p, q, g);
    }

    /* test bad args */
    if (ret == 0) {
        /* null key struct */
        ret = wc_DsaImportParamsRaw(NULL, p, q, g);
        if (ret == BAD_FUNC_ARG) {
            /* null param pointers */
            ret = wc_DsaImportParamsRaw(&key, NULL, NULL, NULL);
        }

        if (ret == BAD_FUNC_ARG) {
            /* illegal p length */
            ret = wc_DsaImportParamsRaw(&key, invalidP, q, g);
        }

        if (ret == BAD_FUNC_ARG) {
            /* illegal q length */
            ret = wc_DsaImportParamsRaw(&key, p, invalidQ, g);
            if (ret == BAD_FUNC_ARG)
                ret = 0;
        }

    }

    printf(resultFmt, ret == 0 ? passed : failed);

    wc_FreeDsaKey(&key);

#endif

    return ret;

} /* END test_wc_DsaImportParamsRaw */

/*
 * Testing wc_DsaImportParamsRawCheck()
 */
static int test_wc_DsaImportParamsRawCheck (void)
{
    int     ret = 0;

#if !defined(NO_DSA) && !defined(HAVE_FIPS) && !defined(HAVE_SELFTEST)
    DsaKey  key;
    int     trusted = 0;
    /* [mod = L=1024, N=160], from CAVP KeyPair */
    const char* p = "d38311e2cd388c3ed698e82fdf88eb92b5a9a483dc88005d"
                    "4b725ef341eabb47cf8a7a8a41e792a156b7ce97206c4f9c"
                    "5ce6fc5ae7912102b6b502e59050b5b21ce263dddb2044b6"
                    "52236f4d42ab4b5d6aa73189cef1ace778d7845a5c1c1c71"
                    "47123188f8dc551054ee162b634d60f097f719076640e209"
                    "80a0093113a8bd73";
    const char* q = "96c5390a8b612c0e422bb2b0ea194a3ec935a281";
    const char* g = "06b7861abbd35cc89e79c52f68d20875389b127361ca66822"
                    "138ce4991d2b862259d6b4548a6495b195aa0e0b6137ca37e"
                    "b23b94074d3c3d300042bdf15762812b6333ef7b07ceba786"
                    "07610fcc9ee68491dbc1e34cd12615474e52b18bc934fb00c"
                    "61d39e7da8902291c4434a4e2224c3f4fd9f93cd6f4f17fc0"
                    "76341a7e7d9";

    /* invalid p and q parameters */
    const char* invalidP = "d38311e2cd388c3ed698e82fdf88eb92b5a9a483dc88005d";
    const char* invalidQ = "96c5390a";

    printf(testingFmt, "wc_DsaImportParamsRawCheck()");

    ret = wc_InitDsaKey(&key);
    if (ret == 0) {
        ret = wc_DsaImportParamsRawCheck(&key, p, q, g, trusted, NULL);
    }

    /* test bad args */
    if (ret == 0) {
        /* null key struct */
        ret = wc_DsaImportParamsRawCheck(NULL, p, q, g, trusted, NULL);
        if (ret == BAD_FUNC_ARG) {
            /* null param pointers */
            ret = wc_DsaImportParamsRawCheck(&key, NULL, NULL, NULL, trusted, NULL);
        }

        if (ret == BAD_FUNC_ARG) {
            /* illegal p length */
            ret = wc_DsaImportParamsRawCheck(&key, invalidP, q, g, trusted, NULL);
        }

        if (ret == BAD_FUNC_ARG) {
            /* illegal q length */
            ret = wc_DsaImportParamsRawCheck(&key, p, invalidQ, g, trusted, NULL);
            if (ret == BAD_FUNC_ARG)
                ret = 0;
        }

    }

    printf(resultFmt, ret == 0 ? passed : failed);

    wc_FreeDsaKey(&key);

#endif

    return ret;

} /* END test_wc_DsaImportParamsRawCheck */

/*
 * Testing wc_DsaExportParamsRaw()
 */
static int test_wc_DsaExportParamsRaw (void)
{
    int     ret = 0;

#if !defined(NO_DSA)
    DsaKey  key;

    /* [mod = L=1024, N=160], from CAVP KeyPair */
    const char* p = "d38311e2cd388c3ed698e82fdf88eb92b5a9a483dc88005d"
                    "4b725ef341eabb47cf8a7a8a41e792a156b7ce97206c4f9c"
                    "5ce6fc5ae7912102b6b502e59050b5b21ce263dddb2044b6"
                    "52236f4d42ab4b5d6aa73189cef1ace778d7845a5c1c1c71"
                    "47123188f8dc551054ee162b634d60f097f719076640e209"
                    "80a0093113a8bd73";
    const char* q = "96c5390a8b612c0e422bb2b0ea194a3ec935a281";
    const char* g = "06b7861abbd35cc89e79c52f68d20875389b127361ca66822"
                    "138ce4991d2b862259d6b4548a6495b195aa0e0b6137ca37e"
                    "b23b94074d3c3d300042bdf15762812b6333ef7b07ceba786"
                    "07610fcc9ee68491dbc1e34cd12615474e52b18bc934fb00c"
                    "61d39e7da8902291c4434a4e2224c3f4fd9f93cd6f4f17fc0"
                    "76341a7e7d9";

    const char* pCompare = "\xd3\x83\x11\xe2\xcd\x38\x8c\x3e\xd6\x98\xe8\x2f"
                           "\xdf\x88\xeb\x92\xb5\xa9\xa4\x83\xdc\x88\x00\x5d"
                           "\x4b\x72\x5e\xf3\x41\xea\xbb\x47\xcf\x8a\x7a\x8a"
                           "\x41\xe7\x92\xa1\x56\xb7\xce\x97\x20\x6c\x4f\x9c"
                           "\x5c\xe6\xfc\x5a\xe7\x91\x21\x02\xb6\xb5\x02\xe5"
                           "\x90\x50\xb5\xb2\x1c\xe2\x63\xdd\xdb\x20\x44\xb6"
                           "\x52\x23\x6f\x4d\x42\xab\x4b\x5d\x6a\xa7\x31\x89"
                           "\xce\xf1\xac\xe7\x78\xd7\x84\x5a\x5c\x1c\x1c\x71"
                           "\x47\x12\x31\x88\xf8\xdc\x55\x10\x54\xee\x16\x2b"
                           "\x63\x4d\x60\xf0\x97\xf7\x19\x07\x66\x40\xe2\x09"
                           "\x80\xa0\x09\x31\x13\xa8\xbd\x73";
    const char* qCompare = "\x96\xc5\x39\x0a\x8b\x61\x2c\x0e\x42\x2b\xb2\xb0"
                           "\xea\x19\x4a\x3e\xc9\x35\xa2\x81";
    const char* gCompare = "\x06\xb7\x86\x1a\xbb\xd3\x5c\xc8\x9e\x79\xc5\x2f"
                           "\x68\xd2\x08\x75\x38\x9b\x12\x73\x61\xca\x66\x82"
                           "\x21\x38\xce\x49\x91\xd2\xb8\x62\x25\x9d\x6b\x45"
                           "\x48\xa6\x49\x5b\x19\x5a\xa0\xe0\xb6\x13\x7c\xa3"
                           "\x7e\xb2\x3b\x94\x07\x4d\x3c\x3d\x30\x00\x42\xbd"
                           "\xf1\x57\x62\x81\x2b\x63\x33\xef\x7b\x07\xce\xba"
                           "\x78\x60\x76\x10\xfc\xc9\xee\x68\x49\x1d\xbc\x1e"
                           "\x34\xcd\x12\x61\x54\x74\xe5\x2b\x18\xbc\x93\x4f"
                           "\xb0\x0c\x61\xd3\x9e\x7d\xa8\x90\x22\x91\xc4\x43"
                           "\x4a\x4e\x22\x24\xc3\xf4\xfd\x9f\x93\xcd\x6f\x4f"
                           "\x17\xfc\x07\x63\x41\xa7\xe7\xd9";

    byte pOut[MAX_DSA_PARAM_SIZE];
    byte qOut[MAX_DSA_PARAM_SIZE];
    byte gOut[MAX_DSA_PARAM_SIZE];
    word32 pOutSz, qOutSz, gOutSz;

    printf(testingFmt, "wc_DsaExportParamsRaw()");

    ret = wc_InitDsaKey(&key);
    if (ret == 0) {
        /* first test using imported raw parameters, for expected */
        ret = wc_DsaImportParamsRaw(&key, p, q, g);
    }

    if (ret == 0) {
        pOutSz = sizeof(pOut);
        qOutSz = sizeof(qOut);
        gOutSz = sizeof(gOut);
        ret = wc_DsaExportParamsRaw(&key, pOut, &pOutSz, qOut, &qOutSz,
                                    gOut, &gOutSz);
    }

    if (ret == 0) {
        /* validate exported parameters are correct */
        if ((XMEMCMP(pOut, pCompare, pOutSz) != 0) ||
            (XMEMCMP(qOut, qCompare, qOutSz) != 0) ||
            (XMEMCMP(gOut, gCompare, gOutSz) != 0) ) {
            ret = -1;
        }
    }

    /* test bad args */
    if (ret == 0) {
        /* null key struct */
        ret = wc_DsaExportParamsRaw(NULL, pOut, &pOutSz, qOut, &qOutSz,
                                    gOut, &gOutSz);

        if (ret == BAD_FUNC_ARG) {
            /* null output pointers */
            ret = wc_DsaExportParamsRaw(&key, NULL, &pOutSz, NULL, &qOutSz,
                                        NULL, &gOutSz);
        }

        if (ret == LENGTH_ONLY_E) {
            /* null output size pointers */
            ret = wc_DsaExportParamsRaw(&key, pOut, NULL, qOut, NULL,
                                        gOut, NULL);
        }

        if (ret == BAD_FUNC_ARG) {
            /* p output buffer size too small */
            pOutSz = 1;
            ret = wc_DsaExportParamsRaw(&key, pOut, &pOutSz, qOut, &qOutSz,
                                        gOut, &gOutSz);
            pOutSz = sizeof(pOut);
        }

        if (ret == BUFFER_E) {
            /* q output buffer size too small */
            qOutSz = 1;
            ret = wc_DsaExportParamsRaw(&key, pOut, &pOutSz, qOut, &qOutSz,
                                        gOut, &gOutSz);
            qOutSz = sizeof(qOut);
        }

        if (ret == BUFFER_E) {
            /* g output buffer size too small */
            gOutSz = 1;
            ret = wc_DsaExportParamsRaw(&key, pOut, &pOutSz, qOut, &qOutSz,
                                        gOut, &gOutSz);
            if (ret == BUFFER_E)
                ret = 0;
        }
    }

    printf(resultFmt, ret == 0 ? passed : failed);

    wc_FreeDsaKey(&key);

#endif

    return ret;

} /* END test_wc_DsaExportParamsRaw */

/*
 * Testing wc_DsaExportKeyRaw()
 */
static int test_wc_DsaExportKeyRaw (void)
{
    int     ret = 0;

#if !defined(NO_DSA) && defined(WOLFSSL_KEY_GEN)
    DsaKey  key;
    WC_RNG  rng;

    byte xOut[MAX_DSA_PARAM_SIZE];
    byte yOut[MAX_DSA_PARAM_SIZE];
    word32 xOutSz, yOutSz;

    printf(testingFmt, "wc_DsaExportKeyRaw()");

    ret = wc_InitRng(&rng);
    if (ret == 0) {
        ret = wc_InitDsaKey(&key);
    }

    if (ret == 0) {
        ret = wc_MakeDsaParameters(&rng, 1024, &key);

        if (ret == 0)  {
            ret = wc_MakeDsaKey(&rng, &key);
        }
    }

    /* try successful export */
    if (ret == 0) {
        xOutSz = sizeof(xOut);
        yOutSz = sizeof(yOut);
        ret = wc_DsaExportKeyRaw(&key, xOut, &xOutSz, yOut, &yOutSz);
    }

    /* test bad args */
    if (ret == 0) {
        /* null key struct */
        ret = wc_DsaExportKeyRaw(NULL, xOut, &xOutSz, yOut, &yOutSz);

        if (ret == BAD_FUNC_ARG) {
            /* null output pointers */
            ret = wc_DsaExportKeyRaw(&key, NULL, &xOutSz, NULL, &yOutSz);
        }

        if (ret == LENGTH_ONLY_E) {
            /* null output size pointers */
            ret = wc_DsaExportKeyRaw(&key, xOut, NULL, yOut, NULL);
        }

        if (ret == BAD_FUNC_ARG) {
            /* x output buffer size too small */
            xOutSz = 1;
            ret = wc_DsaExportKeyRaw(&key, xOut, &xOutSz, yOut, &yOutSz);
            xOutSz = sizeof(xOut);
        }

        if (ret == BUFFER_E) {
            /* y output buffer size too small */
            yOutSz = 1;
            ret = wc_DsaExportKeyRaw(&key, xOut, &xOutSz, yOut, &yOutSz);

            if (ret == BUFFER_E)
                ret = 0;
        }
    }

    printf(resultFmt, ret == 0 ? passed : failed);

    wc_FreeDsaKey(&key);
    wc_FreeRng(&rng);

#endif

    return ret;

} /* END test_wc_DsaExportParamsRaw */


/*
 * Testing wc_ed25519_make_key().
 */
static int test_wc_ed25519_make_key (void)
{
    int ret = 0;

#if defined(HAVE_ED25519)
    ed25519_key     key;
    WC_RNG          rng;

    ret = wc_InitRng(&rng);
    if (ret == 0) {
        ret = wc_ed25519_init(&key);
    }
    printf(testingFmt, "wc_ed25519_make_key()");
    if (ret == 0) {
        ret = wc_ed25519_make_key(&rng, ED25519_KEY_SIZE, &key);
    }
    /* Test bad args. */
    if (ret == 0) {
        ret = wc_ed25519_make_key(NULL, ED25519_KEY_SIZE, &key);
        if (ret == BAD_FUNC_ARG) {
            ret = wc_ed25519_make_key(&rng, ED25519_KEY_SIZE, NULL);
        }
        if (ret == BAD_FUNC_ARG) {
            ret = wc_ed25519_make_key(&rng, ED25519_KEY_SIZE - 1, &key);
        }
        if (ret == BAD_FUNC_ARG) {
            ret = wc_ed25519_make_key(&rng, ED25519_KEY_SIZE + 1, &key);
        }
        if (ret == BAD_FUNC_ARG) {
            ret = 0;
        } else if (ret == 0) {
            ret = SSL_FATAL_ERROR;
        }
    }


    printf(resultFmt, ret == 0 ? passed : failed);

    if (wc_FreeRng(&rng) && ret == 0) {
        ret = SSL_FATAL_ERROR;
    }
    wc_ed25519_free(&key);

#endif
    return ret;

} /* END test_wc_ed25519_make_key */


/*
 * Testing wc_ed25519_init()
 */
static int test_wc_ed25519_init (void)
{
    int             ret = 0;

#if defined(HAVE_ED25519)

    ed25519_key    key;

    printf(testingFmt, "wc_ed25519_init()");

    ret = wc_ed25519_init(&key);

    /* Test bad args. */
    if (ret == 0) {
        ret = wc_ed25519_init(NULL);
        if (ret == BAD_FUNC_ARG) {
            ret = 0;
        } else if (ret == 0) {
            ret = SSL_FATAL_ERROR;
        }
    }

    printf(resultFmt, ret == 0 ? passed : failed);

    wc_ed25519_free(&key);

#endif
    return ret;

} /* END test_wc_ed25519_init */

/*
 * Test wc_ed25519_sign_msg() and wc_ed25519_verify_msg()
 */
static int test_wc_ed25519_sign_msg (void)
{
    int             ret = 0;

#if defined(HAVE_ED25519) && defined(HAVE_ED25519_SIGN)
    WC_RNG          rng;
    ed25519_key     key;
    byte            msg[] = "Everybody gets Friday off.\n";
    byte            sig[ED25519_SIG_SIZE];
    word32          msglen = sizeof(msg);
    word32          siglen = sizeof(sig);
    word32          badSigLen = sizeof(sig) - 1;
    int             verify_ok = 0; /*1 = Verify success.*/

    /* Initialize stack variables. */
    XMEMSET(sig, 0, siglen);

    /* Initialize key. */
    ret = wc_InitRng(&rng);
    if (ret == 0) {
        ret = wc_ed25519_init(&key);
        if (ret == 0) {
            ret = wc_ed25519_make_key(&rng, ED25519_KEY_SIZE, &key);
        }
    }

    printf(testingFmt, "wc_ed25519_sign_msg()");

    if (ret == 0) {
        ret = wc_ed25519_sign_msg(msg, msglen, sig, &siglen, &key);
    }
    /* Test bad args. */
    if (ret == 0 && siglen == ED25519_SIG_SIZE) {
        ret = wc_ed25519_sign_msg(NULL, msglen, sig, &siglen, &key);
        if (ret == BAD_FUNC_ARG) {
            ret = wc_ed25519_sign_msg(msg, msglen, NULL, &siglen, &key);
        }
        if (ret == BAD_FUNC_ARG) {
            ret = wc_ed25519_sign_msg(msg, msglen, sig, NULL, &key);
        }
        if (ret == BAD_FUNC_ARG) {
            ret = wc_ed25519_sign_msg(msg, msglen, sig, &siglen, NULL);
        }
        if (ret == BAD_FUNC_ARG) {
            ret = wc_ed25519_sign_msg(msg, msglen, sig, &badSigLen, &key);
        }
        if (ret == BUFFER_E && badSigLen == ED25519_SIG_SIZE) {
            badSigLen -= 1;
            ret = 0;
        } else if (ret == 0) {
            ret = SSL_FATAL_ERROR;
        }
    } /* END sign */

    printf(resultFmt, ret == 0 ? passed : failed);
    #ifdef HAVE_ED25519_VERIFY
        printf(testingFmt, "wc_ed25519_verify_msg()");

        if (ret == 0) {

            ret = wc_ed25519_verify_msg(sig, siglen, msg, msglen, &verify_ok, &key);
            if (ret == 0  && verify_ok == 1) {
                ret = 0;
            } else if (ret == 0) {
                ret = SSL_FATAL_ERROR;
            }

            /* Test bad args. */
            if (ret == 0) {
                ret = wc_ed25519_verify_msg(NULL, siglen, msg, msglen, &verify_ok,
                                                                        &key);
                if (ret == BAD_FUNC_ARG) {
                    ret = wc_ed25519_verify_msg(sig, siglen, NULL, msglen,
                                                                &verify_ok, &key);
                }
                if (ret == BAD_FUNC_ARG) {
                    ret = wc_ed25519_verify_msg(sig, siglen, msg, msglen,
                                                                  NULL, &key);
                }
                if (ret == BAD_FUNC_ARG) {
                    ret = wc_ed25519_verify_msg(sig, siglen, msg, msglen,
                                                                &verify_ok, NULL);
                }
                if (ret == BAD_FUNC_ARG) {
                    ret = wc_ed25519_verify_msg(sig, badSigLen, msg, msglen,
                                                                &verify_ok, &key);
                }
                if (ret == BAD_FUNC_ARG) {
                    ret = 0;
                } else if (ret == 0) {
                    ret = SSL_FATAL_ERROR;
                }
            }

        } /* END verify. */

        printf(resultFmt, ret == 0 ? passed : failed);
    #endif /* Verify. */

    if (wc_FreeRng(&rng) && ret == 0) {
        ret = SSL_FATAL_ERROR;
    }
    wc_ed25519_free(&key);

#endif
    return ret;

} /* END test_wc_ed25519_sign_msg */

/*
 * Testing wc_ed25519_import_public()
 */
static int test_wc_ed25519_import_public (void)
{
    int             ret = 0;

#if defined(HAVE_ED25519) && defined(HAVE_ED25519_KEY_IMPORT)
    WC_RNG          rng;
    ed25519_key     pubKey;
    const byte      in[] = "Ed25519PublicKeyUnitTest......\n";
    word32          inlen = sizeof(in);


    ret = wc_InitRng(&rng);
    if (ret == 0) {
        ret = wc_ed25519_init(&pubKey);
        if (ret == 0) {
            ret = wc_ed25519_make_key(&rng, ED25519_KEY_SIZE, &pubKey);
        }
    }
    printf(testingFmt, "wc_ed25519_import_public()");

    if (ret == 0) {
        ret = wc_ed25519_import_public(in, inlen, &pubKey);

        if (ret == 0 && XMEMCMP(in, pubKey.p, inlen) == 0) {
            ret = 0;
        } else {
            ret = SSL_FATAL_ERROR;
        }

        /* Test bad args. */
        if (ret == 0) {
            ret = wc_ed25519_import_public(NULL, inlen, &pubKey);
            if (ret == BAD_FUNC_ARG) {
                ret = wc_ed25519_import_public(in, inlen, NULL);
            }
            if (ret == BAD_FUNC_ARG) {
                ret = wc_ed25519_import_public(in, inlen - 1, &pubKey);
            }
            if (ret == BAD_FUNC_ARG) {
                ret = 0;
            } else if (ret == 0) {
                ret = SSL_FATAL_ERROR;
            }
        }
    }

    printf(resultFmt, ret == 0 ? passed : failed);

    if (wc_FreeRng(&rng) && ret == 0) {
        ret = SSL_FATAL_ERROR;
    }
    wc_ed25519_free(&pubKey);

#endif
    return ret;

} /* END wc_ed25519_import_public */

/*
 * Testing wc_ed25519_import_private_key()
 */
static int test_wc_ed25519_import_private_key (void)
{
    int         ret = 0;

#if defined(HAVE_ED25519) && defined(HAVE_ED25519_KEY_IMPORT)
    WC_RNG      rng;
    ed25519_key key;
    const byte  privKey[] = "Ed25519PrivateKeyUnitTest.....\n";
    const byte  pubKey[] = "Ed25519PublicKeyUnitTest......\n";
    word32      privKeySz = sizeof(privKey);
    word32      pubKeySz = sizeof(pubKey);

    ret = wc_InitRng(&rng);
    if (ret != 0) {
        return ret;
    }
    ret = wc_ed25519_init(&key);
    if (ret != 0) {
        wc_FreeRng(&rng);
        return ret;
    }
    ret = wc_ed25519_make_key(&rng, ED25519_KEY_SIZE, &key);

    printf(testingFmt, "wc_ed25519_import_private_key()");

    if (ret == 0) {
        ret = wc_ed25519_import_private_key(privKey, privKeySz, pubKey,
                                                            pubKeySz, &key);
        if (ret == 0 && (XMEMCMP(pubKey, key.p, privKeySz) != 0
                                || XMEMCMP(privKey, key.k, pubKeySz) != 0)) {
            ret = SSL_FATAL_ERROR;
        }
    }

    /* Test bad args. */
    if (ret == 0) {
        ret = wc_ed25519_import_private_key(NULL, privKeySz, pubKey, pubKeySz,
                                                                        &key);
        if (ret == BAD_FUNC_ARG) {
            ret = wc_ed25519_import_private_key(privKey, privKeySz, NULL,
                                                                pubKeySz, &key);
        }
        if (ret == BAD_FUNC_ARG) {
            ret = wc_ed25519_import_private_key(privKey, privKeySz, pubKey,
                                                                pubKeySz, NULL);
        }
        if (ret == BAD_FUNC_ARG) {
            ret = wc_ed25519_import_private_key(privKey, privKeySz - 1, pubKey,
                                                                pubKeySz, &key);
        }
        if (ret == BAD_FUNC_ARG) {
            ret = wc_ed25519_import_private_key(privKey, privKeySz, pubKey,
                                                            pubKeySz - 1, &key);
        }
        if (ret == BAD_FUNC_ARG) {
            ret = 0;
        } else if (ret == 0) {
            ret = SSL_FATAL_ERROR;
        }
    }

    printf(resultFmt, ret == 0 ? passed : failed);

    if (wc_FreeRng(&rng) && ret == 0) {
        ret = SSL_FATAL_ERROR;
    }
    wc_ed25519_free(&key);

#endif
    return ret;

} /* END test_wc_ed25519_import_private_key */

/*
 * Testing wc_ed25519_export_public() and wc_ed25519_export_private_only()
 */
static int test_wc_ed25519_export (void)
{
    int             ret = 0;

#if defined(HAVE_ED25519) && defined(HAVE_ED25519_KEY_EXPORT)
    WC_RNG          rng;
    ed25519_key     key;
    byte            priv[ED25519_PRV_KEY_SIZE];
    byte            pub[ED25519_PUB_KEY_SIZE];
    word32          privSz = sizeof(priv);
    word32          pubSz = sizeof(pub);

    ret = wc_InitRng(&rng);
    if (ret != 0) {
        return ret;
    }

    ret = wc_ed25519_init(&key);
    if (ret != 0) {
        wc_FreeRng(&rng);
        return ret;
    }

    if (ret == 0) {
        ret = wc_ed25519_make_key(&rng, ED25519_KEY_SIZE, &key);
    }

    printf(testingFmt, "wc_ed25519_export_public()");

    if (ret == 0) {
        ret = wc_ed25519_export_public(&key, pub, &pubSz);
        if (ret == 0 && (pubSz != ED25519_KEY_SIZE
                                        || XMEMCMP(key.p, pub, pubSz) != 0)) {
            ret = SSL_FATAL_ERROR;
        }
        if (ret == 0) {
            ret = wc_ed25519_export_public(NULL, pub, &pubSz);
            if (ret == BAD_FUNC_ARG) {
                ret = wc_ed25519_export_public(&key, NULL, &pubSz);
            }
            if (ret == BAD_FUNC_ARG) {
                ret = wc_ed25519_export_public(&key, pub, NULL);
            }
            if (ret == BAD_FUNC_ARG) {
                ret = 0;
            } else if (ret == 0) {
                ret = SSL_FATAL_ERROR;
            }
        }
    }

    printf(resultFmt, ret == 0 ? passed : failed);
    printf(testingFmt, "wc_ed25519_export_private_only()");

    if (ret == 0) {
        ret = wc_ed25519_export_private_only(&key, priv, &privSz);
        if (ret == 0 && (privSz != ED25519_KEY_SIZE
                                        || XMEMCMP(key.k, priv, privSz) != 0)) {
            ret = SSL_FATAL_ERROR;
        }
        if (ret == 0) {
            ret = wc_ed25519_export_private_only(NULL, priv, &privSz);
            if (ret == BAD_FUNC_ARG) {
                ret = wc_ed25519_export_private_only(&key, NULL, &privSz);
            }
            if (ret == BAD_FUNC_ARG) {
                ret = wc_ed25519_export_private_only(&key, priv, NULL);
            }
            if (ret == BAD_FUNC_ARG) {
                ret = 0;
            } else if (ret == 0) {
                ret = SSL_FATAL_ERROR;
            }
        }
    }

    printf(resultFmt, ret == 0 ? passed : failed);

    if (wc_FreeRng(&rng) && ret == 0) {
        ret = SSL_FATAL_ERROR;
    }
    wc_ed25519_free(&key);

#endif
    return ret;

} /* END test_wc_ed25519_export */

/*
 *  Testing wc_ed25519_size()
 */
static int test_wc_ed25519_size (void)
{
    int             ret = 0;
#if defined(HAVE_ED25519)
    WC_RNG          rng;
    ed25519_key     key;

    ret = wc_InitRng(&rng);
    if (ret != 0) {
        return ret;
    }
    ret = wc_ed25519_init(&key);
    if (ret != 0) {
        wc_FreeRng(&rng);
        return ret;
    }

    ret = wc_ed25519_make_key(&rng, ED25519_KEY_SIZE, &key);
    if (ret != 0) {
        wc_FreeRng(&rng);
        wc_ed25519_free(&key);
        return ret;
    }

    printf(testingFmt, "wc_ed25519_size()");
    ret = wc_ed25519_size(&key);
    /* Test bad args. */
    if (ret == ED25519_KEY_SIZE) {
        ret = wc_ed25519_size(NULL);
        if (ret == BAD_FUNC_ARG) {
            ret = 0;
        }
    }
    printf(resultFmt, ret == 0 ? passed : failed);

    if (ret == 0) {
        printf(testingFmt, "wc_ed25519_sig_size()");

        ret = wc_ed25519_sig_size(&key);
        if (ret == ED25519_SIG_SIZE) {
            ret = 0;
        }
        /* Test bad args. */
        if (ret == 0) {
            ret = wc_ed25519_sig_size(NULL);
            if (ret == BAD_FUNC_ARG) {
                ret = 0;
            }
        }

        printf(resultFmt, ret == 0 ? passed : failed);
    } /* END wc_ed25519_sig_size() */

    if (ret == 0) {
        printf(testingFmt, "wc_ed25519_pub_size");
        ret = wc_ed25519_pub_size(&key);
        if (ret == ED25519_PUB_KEY_SIZE) {
            ret = 0;
        }
        if (ret == 0) {
            ret = wc_ed25519_pub_size(NULL);
            if (ret == BAD_FUNC_ARG) {
                ret = 0;
            }
        }
        printf(resultFmt, ret == 0 ? passed : failed);
    } /* END wc_ed25519_pub_size */

    if (ret == 0) {
        printf(testingFmt, "wc_ed25519_priv_size");
        ret = wc_ed25519_priv_size(&key);
        if (ret == ED25519_PRV_KEY_SIZE) {
            ret = 0;
        }
        if (ret == 0) {
            ret = wc_ed25519_priv_size(NULL);
            if (ret == BAD_FUNC_ARG) {
                ret = 0;
            }
        }
        printf(resultFmt, ret == 0 ? passed : failed);
    } /* END wc_ed25519_pub_size */

    if (wc_FreeRng(&rng) && ret == 0) {
        ret = SSL_FATAL_ERROR;
    }
    wc_ed25519_free(&key);

#endif
    return ret;

} /* END test_wc_ed25519_size */

/*
 * Testing wc_ed25519_export_private() and wc_ed25519_export_key()
 */
static int test_wc_ed25519_exportKey (void)
{
    int             ret = 0;
#if defined(HAVE_ED25519) && defined(HAVE_ED25519_KEY_EXPORT)
    WC_RNG          rng;
    ed25519_key     key;
    byte            priv[ED25519_PRV_KEY_SIZE];
    byte            pub[ED25519_PUB_KEY_SIZE];
    byte            privOnly[ED25519_PRV_KEY_SIZE];
    word32          privSz      = sizeof(priv);
    word32          pubSz       = sizeof(pub);
    word32          privOnlySz  = sizeof(privOnly);

    ret = wc_InitRng(&rng);
    if (ret != 0) {
        return ret;
    }
    ret = wc_ed25519_init(&key);
    if (ret != 0) {
        wc_FreeRng(&rng);
        return ret;
    }

    ret = wc_ed25519_make_key(&rng, ED25519_KEY_SIZE, &key);
    if (ret != 0) {
        wc_FreeRng(&rng);
        wc_ed25519_free(&key);
        return ret;
    }

    printf(testingFmt, "wc_ed25519_export_private()");

    ret = wc_ed25519_export_private(&key, privOnly, &privOnlySz);
    if (ret == 0) {
        ret = wc_ed25519_export_private(NULL, privOnly, &privOnlySz);
        if (ret == BAD_FUNC_ARG) {
            ret = wc_ed25519_export_private(&key, NULL, &privOnlySz);
        }
        if (ret == BAD_FUNC_ARG) {
            ret = wc_ed25519_export_private(&key, privOnly, NULL);
        }
        if (ret == BAD_FUNC_ARG) {
            ret = 0;
        } else if (ret == 0) {
            ret = SSL_FATAL_ERROR;
        }
    }

    printf(resultFmt, ret == 0 ? passed : failed);

    if (ret == 0) {
        printf(testingFmt, "wc_ed25519_export_key()");

        ret = wc_ed25519_export_key(&key, priv, &privSz, pub, &pubSz);
        if (ret == 0) {
            ret = wc_ed25519_export_key(NULL, priv, &privSz, pub, &pubSz);
            if (ret == BAD_FUNC_ARG) {
                ret = wc_ed25519_export_key(&key, NULL, &privSz, pub, &pubSz);
            }
            if (ret == BAD_FUNC_ARG) {
                ret = wc_ed25519_export_key(&key, priv, NULL, pub, &pubSz);
            }
            if (ret == BAD_FUNC_ARG) {
                ret = wc_ed25519_export_key(&key, priv, &privSz, NULL, &pubSz);
            }
            if (ret == BAD_FUNC_ARG) {
                ret = wc_ed25519_export_key(&key, priv, &privSz, pub, NULL);
            }
            if (ret == BAD_FUNC_ARG) {
                ret = 0;
            } else if (ret == 0) {
                ret = SSL_FATAL_ERROR;
            }
        }
        printf(resultFmt, ret == 0 ? passed : failed);
    } /* END wc_ed25519_export_key() */

    /* Cross check output. */
    if (ret == 0 && XMEMCMP(priv, privOnly, privSz) != 0) {
        ret = SSL_FATAL_ERROR;
    }

    if (wc_FreeRng(&rng) && ret == 0) {
        ret = SSL_FATAL_ERROR;
    }
    wc_ed25519_free(&key);

#endif
    return ret;

} /* END test_wc_ed25519_exportKey */

/*
 * Testing wc_Ed25519PublicKeyToDer
 */
static int test_wc_Ed25519PublicKeyToDer (void)
{
    int ret = 0;

#if defined(HAVE_ED25519) && (defined(WOLFSSL_CERT_GEN) || \
                              defined(WOLFSSL_KEY_GEN))
    int tmp;
    ed25519_key key;
    byte derBuf[1024];

    printf(testingFmt, "wc_Ed25519PublicKeyToDer()");

    /* Test bad args */
    tmp = wc_Ed25519PublicKeyToDer(NULL, NULL, 0, 0);
    if (tmp != BAD_FUNC_ARG) {
        ret = SSL_FATAL_ERROR;
    }

    if (ret == 0) {
        wc_ed25519_init(&key);
        tmp = wc_Ed25519PublicKeyToDer(&key, derBuf, 0, 0);
        if (tmp != BUFFER_E) {
            ret = SSL_FATAL_ERROR;
        }
        wc_ed25519_free(&key);
    }

    /*  Test good args */
    if (ret == 0) {
        WC_RNG          rng;
        ret = wc_InitRng(&rng);
        if (ret != 0) {
            return ret;
        }
        ret = wc_ed25519_init(&key);
        if (ret != 0) {
            wc_FreeRng(&rng);
            return ret;
        }

        ret = wc_ed25519_make_key(&rng, ED25519_KEY_SIZE, &key);
        if (ret != 0) {
            wc_FreeRng(&rng);
            wc_ed25519_free(&key);
            return ret;
        }

        tmp = wc_Ed25519PublicKeyToDer(&key, derBuf, 1024, 1);
        if (tmp <= 0) {
            ret = SSL_FATAL_ERROR;
        }

        wc_FreeRng(&rng);
        wc_ed25519_free(&key);
    }
    printf(resultFmt, ret == 0 ? passed : failed);
#endif
    return ret;

} /* END testing wc_Ed25519PublicKeyToDer */

/*
 * Testing wc_curve25519_init and wc_curve25519_free.
 */
static int test_wc_curve25519_init (void)
{
    int ret = 0;

#if defined(HAVE_CURVE25519)

    curve25519_key  key;

    printf(testingFmt, "wc_curve25519_init()");
    ret = wc_curve25519_init(&key);

    /* Test bad args for wc_curve25519_init */
    if (ret == 0) {
        ret = wc_curve25519_init(NULL);
        if (ret == BAD_FUNC_ARG) {
            ret = 0;
        } else if (ret == 0) {
            ret = SSL_FATAL_ERROR;
        }
    }

    printf(resultFmt, ret == 0 ? passed : failed);
    /*  Test good args for wc_curve_25519_free */
    wc_curve25519_free(&key);

    wc_curve25519_free(NULL);

#endif
    return ret;

} /* END test_wc_curve25519_init and wc_curve_25519_free*/
/*
 * Testing test_wc_curve25519_size.
 */
static int test_wc_curve25519_size (void)
{
    int ret = 0;

#if defined(HAVE_CURVE25519)

    curve25519_key  key;

    printf(testingFmt, "wc_curve25519_size()");

    ret = wc_curve25519_init(&key);

    /*  Test good args for wc_curve25519_size */
    if (ret == 0) {
        ret = wc_curve25519_size(&key);
    }

    /* Test bad args for wc_curve25519_size */
    if (ret != 0) {
        ret = wc_curve25519_size(NULL);
    }

    printf(resultFmt, ret == 0 ? passed : failed);
    wc_curve25519_free(&key);
#endif
    return ret;

} /* END test_wc_curve25519_size*/
/*
 * Testing wc_ecc_make_key.
 */
static int test_wc_ecc_make_key (void)
{
    int     ret = 0;

#if defined(HAVE_ECC) && !defined(WC_NO_RNG)
    WC_RNG rng;
    ecc_key key;

    printf(testingFmt, "wc_ecc_make_key()");

    ret = wc_InitRng(&rng);
    if (ret != 0)
        return ret;

    ret = wc_ecc_init(&key);
    if (ret == 0) {
        ret = wc_ecc_make_key(&rng, KEY14, &key);

        /* Pass in bad args. */
        if (ret == 0) {
            ret = wc_ecc_make_key(NULL, KEY14, &key);
            if (ret == BAD_FUNC_ARG) {
                ret = wc_ecc_make_key(&rng, KEY14, NULL);
            }
            if (ret == BAD_FUNC_ARG) {
                ret = 0;
            } else if (ret == 0) {
                ret = WOLFSSL_FATAL_ERROR;
            }
        }
        wc_ecc_free(&key);
    }

    if (wc_FreeRng(&rng) != 0 && ret == 0) {
        ret = WOLFSSL_FATAL_ERROR;
    }

#ifdef FP_ECC
    wc_ecc_fp_free();
#endif

    printf(resultFmt, ret == 0 ? passed : failed);
#endif

    return ret;

} /* END test_wc_ecc_make_key */


/*
 * Testing wc_ecc_init()
 */
static int test_wc_ecc_init (void)
{
    int         ret = 0;

#ifdef HAVE_ECC
    ecc_key     key;

    printf(testingFmt, "wc_ecc_init()");

    ret = wc_ecc_init(&key);
    /* Pass in bad args. */
    if (ret == 0) {
        ret = wc_ecc_init(NULL);
        if (ret == BAD_FUNC_ARG) {
            ret = 0;
        } else if (ret == 0) {
            ret = WOLFSSL_FATAL_ERROR;
        }
    }

    printf(resultFmt, ret == 0 ? passed : failed);

    wc_ecc_free(&key);

#endif
    return ret;

} /* END test_wc_ecc_init */

/*
 * Testing wc_ecc_check_key()
 */
static int test_wc_ecc_check_key (void)
{
    int         ret = 0;

#if defined(HAVE_ECC) && !defined(WC_NO_RNG)
    WC_RNG      rng;
    ecc_key     key;

    ret = wc_InitRng(&rng);
    if (ret == 0) {
        ret = wc_ecc_init(&key);
        if (ret == 0) {
            ret = wc_ecc_make_key(&rng, KEY14, &key);
        }
    }

    printf(testingFmt, "wc_ecc_check_key()");

    if (ret == 0) {
        ret = wc_ecc_check_key(&key);
    }

    /* Pass in bad args. */
    if (ret == 0) {
        ret = wc_ecc_check_key(NULL);
        if (ret == BAD_FUNC_ARG) {
            ret = 0;
        } else if (ret == 0) {
            ret = WOLFSSL_FATAL_ERROR;
        }
    }

    printf(resultFmt, ret == 0 ? passed : failed);

    if (wc_FreeRng(&rng) && ret == 0) {
        ret = WOLFSSL_FATAL_ERROR;
    }
    wc_ecc_free(&key);

#ifdef FP_ECC
    wc_ecc_fp_free();
#endif

#endif
    return ret;

} /* END test_wc_ecc_check_key */

/*
 * Testing wc_ecc_size()
 */
static int test_wc_ecc_size (void)
{
    int         ret = 0;

#if defined(HAVE_ECC) && !defined(WC_NO_RNG)
    WC_RNG      rng;
    ecc_key     key;

    ret = wc_InitRng(&rng);
    if (ret == 0) {
        ret = wc_ecc_init(&key);
        if (ret == 0) {
            ret = wc_ecc_make_key(&rng, KEY14, &key);
        }
    }

    printf(testingFmt, "wc_ecc_size()");

    if (ret == 0) {
        ret = wc_ecc_size(&key);
        if (ret == KEY14) {
            ret = 0;
        } else if (ret == 0){
            ret = WOLFSSL_FATAL_ERROR;
        }
    }
    /* Test bad args. */
    if (ret == 0) {
        /* Returns Zero for bad arg. */
        ret = wc_ecc_size(NULL);
    }

    printf(resultFmt, ret == 0 ? passed : failed);

    if (wc_FreeRng(&rng) && ret == 0) {
        ret = WOLFSSL_FATAL_ERROR;
    }
    wc_ecc_free(&key);

#endif
    return ret;
} /* END test_wc_ecc_size */

static void test_wc_ecc_params(void)
{
    /* FIPS/CAVP self-test modules do not have `wc_ecc_get_curve_params`.
        It was added after certifications */
#if defined(HAVE_ECC) && !defined(HAVE_FIPS) && !defined(HAVE_SELFTEST)
    const ecc_set_type* ecc_set;
#if !defined(NO_ECC256) && !defined(NO_ECC_SECP)
    /* Test for SECP256R1 curve */
    int curve_id = ECC_SECP256R1;
    int curve_idx = wc_ecc_get_curve_idx(curve_id);
    AssertIntNE(curve_idx, ECC_CURVE_INVALID);
    ecc_set = wc_ecc_get_curve_params(curve_idx);
    AssertNotNull(ecc_set);
    AssertIntEQ(ecc_set->id, curve_id);
#endif
    /* Test case when SECP256R1 is not enabled */
    /* Test that we get curve params for index 0 */
    ecc_set = wc_ecc_get_curve_params(0);
    AssertNotNull(ecc_set);
#endif /* HAVE_ECC && !HAVE_FIPS && !HAVE_SELFTEST */
}

/*
 * Testing wc_ecc_sign_hash() and wc_ecc_verify_hash()
 */
static int test_wc_ecc_signVerify_hash (void)
{
    int         ret = 0;

#if defined(HAVE_ECC) && defined(HAVE_ECC_SIGN) && !defined(NO_ASN) && !defined(WC_NO_RNG)
    WC_RNG      rng;
    ecc_key     key;
    int         signH = WOLFSSL_FATAL_ERROR;
    #ifdef HAVE_ECC_VERIFY
        int     verifyH = WOLFSSL_FATAL_ERROR;
        int     verify  = 0;
    #endif
    word32      siglen = ECC_BUFSIZE;
    byte        sig[ECC_BUFSIZE];
    byte        digest[] = "Everyone gets Friday off.";
    word32      digestlen = (word32)XSTRLEN((char*)digest);

    /* Init stack var */
    XMEMSET(sig, 0, siglen);

    /* Init structs. */
    ret = wc_InitRng(&rng);
    if (ret == 0) {
        ret = wc_ecc_init(&key);
        if (ret == 0) {
            ret = wc_ecc_make_key(&rng, KEY14, &key);
        }
    }

    printf(testingFmt, "wc_ecc_sign_hash()");

    if (ret == 0) {
        ret = wc_ecc_sign_hash(digest, digestlen, sig, &siglen, &rng, &key);
    }

    /* Checkk bad args. */
    if (ret == 0) {
        signH = wc_ecc_sign_hash(NULL, digestlen, sig, &siglen, &rng, &key);
        if (signH == ECC_BAD_ARG_E) {
            signH = wc_ecc_sign_hash(digest, digestlen, NULL, &siglen,
                                                                &rng, &key);
        }
        if (signH == ECC_BAD_ARG_E) {
            signH = wc_ecc_sign_hash(digest, digestlen, sig, NULL,
                                                                &rng, &key);
        }
        if (signH == ECC_BAD_ARG_E) {
            signH = wc_ecc_sign_hash(digest, digestlen, sig, &siglen,
                                                                NULL, &key);
        }
        if (signH == ECC_BAD_ARG_E) {
            signH = wc_ecc_sign_hash(digest, digestlen, sig, &siglen,
                                                                &rng, NULL);
        }
        if (signH == ECC_BAD_ARG_E) {
            signH = 0;
        } else if (ret == 0) {
            signH = WOLFSSL_FATAL_ERROR;
        }
    }

    printf(resultFmt, signH == 0 ? passed : failed);

    #ifdef HAVE_ECC_VERIFY
        printf(testingFmt, "wc_ecc_verify_hash()");

        ret = wc_ecc_verify_hash(sig, siglen, digest, digestlen, &verify, &key);
        if (verify != 1 && ret == 0) {
            ret = WOLFSSL_FATAL_ERROR;
        }
        /* Test bad args. */
        if (ret == 0) {
            verifyH = wc_ecc_verify_hash(NULL, siglen, digest, digestlen,
                                                            &verify, &key);
            if (verifyH == ECC_BAD_ARG_E) {
                verifyH = wc_ecc_verify_hash(sig, siglen, NULL, digestlen,
                                                            &verify, &key);
            }
            if (verifyH == ECC_BAD_ARG_E) {
                verifyH = wc_ecc_verify_hash(sig, siglen, digest, digestlen,
                                                                NULL, &key);
            }
            if (verifyH == ECC_BAD_ARG_E) {
                verifyH = wc_ecc_verify_hash(sig, siglen, digest, digestlen,
                                                            &verify, NULL);
            }
            if (verifyH == ECC_BAD_ARG_E) {
                verifyH = 0;
            } else if (ret == 0) {
                verifyH = WOLFSSL_FATAL_ERROR;
            }
        }

        printf(resultFmt, verifyH == 0 ? passed : failed);

    #endif /* HAVE_ECC_VERIFY */

        if (wc_FreeRng(&rng) && ret == 0) {
            ret = WOLFSSL_FATAL_ERROR;
        }
        wc_ecc_free(&key);

    #ifdef FP_ECC
        wc_ecc_fp_free();
    #endif

#endif
    return ret;

} /*  END test_wc_ecc_sign_hash */


/*
 * Testing wc_ecc_shared_secret()
 */
static int test_wc_ecc_shared_secret (void)
{
    int         ret = 0;

#if defined(HAVE_ECC) && defined(HAVE_ECC_DHE) && !defined(WC_NO_RNG)
    ecc_key     key, pubKey;
    WC_RNG      rng;
    int         keySz = KEY16;
    byte        out[KEY16];
    word32      outlen = (word32)sizeof(out);

    /* Initialize variables. */
    XMEMSET(out, 0, keySz);

    ret = wc_InitRng(&rng);
    if (ret == 0) {
        ret = wc_ecc_init(&key);
        if (ret == 0) {
            ret = wc_ecc_init(&pubKey);
        }
    }
    if (ret == 0) {
        ret = wc_ecc_make_key(&rng, keySz, &key);
    }
    if (ret == 0) {
        ret = wc_ecc_make_key(&rng, keySz, &pubKey);
    }

    printf(testingFmt, "wc_ecc_shared_secret()");
    if (ret == 0) {
        ret = wc_ecc_shared_secret(&key, &pubKey, out, &outlen);
        /* Test bad args. */
        if (ret == 0) {
            ret = wc_ecc_shared_secret(NULL, &pubKey, out, &outlen);
            if (ret == BAD_FUNC_ARG) {
                ret = wc_ecc_shared_secret(&key, NULL, out, &outlen);
            }
            if (ret == BAD_FUNC_ARG) {
                ret = wc_ecc_shared_secret(&key, &pubKey, NULL, &outlen);
            }
            if (ret == BAD_FUNC_ARG) {
                ret = wc_ecc_shared_secret(&key, &pubKey, out, NULL);
            }
            if (ret == BAD_FUNC_ARG) {
                ret = 0;
            } else if (ret == 0) {
                ret = WOLFSSL_FATAL_ERROR;
            }
        }
    }

    printf(resultFmt, ret == 0 ? passed : failed);

    if (wc_FreeRng(&rng) && ret == 0) {
        ret = WOLFSSL_FATAL_ERROR;
    }
    wc_ecc_free(&key);
    wc_ecc_free(&pubKey);

#ifdef FP_ECC
    wc_ecc_fp_free();
#endif

#endif
    return ret;

} /* END tests_wc_ecc_shared_secret */

/*
 * testint wc_ecc_export_x963()
 */
static int test_wc_ecc_export_x963 (void)
{
    int     ret = 0;

#if defined(HAVE_ECC) && defined(HAVE_ECC_KEY_EXPORT) && !defined(WC_NO_RNG)
    ecc_key key;
    WC_RNG  rng;
    byte    out[ECC_ASN963_MAX_BUF_SZ];
    word32  outlen = sizeof(out);

    /* Initialize variables. */
    XMEMSET(out, 0, outlen);

    ret = wc_InitRng(&rng);
    if (ret == 0) {
        ret = wc_ecc_init(&key);
        if (ret == 0) {
            ret = wc_ecc_make_key(&rng, KEY20, &key);
        }
    }
    printf(testingFmt, "wc_ecc_export_x963()");

    if (ret == 0) {
        ret = wc_ecc_export_x963(&key, out, &outlen);
    }

    /* Test bad args. */
    if (ret == 0) {
        ret = wc_ecc_export_x963(NULL, out, &outlen);
        if (ret == ECC_BAD_ARG_E) {
            ret = wc_ecc_export_x963(&key, NULL, &outlen);
        }
        if (ret == LENGTH_ONLY_E) {
            ret = wc_ecc_export_x963(&key, out, NULL);
        }
        if (ret == ECC_BAD_ARG_E) {
            key.idx = -4;
            ret = wc_ecc_export_x963(&key, out, &outlen);
        }
        if (ret == ECC_BAD_ARG_E) {
            ret = 0;
        } else {
            ret = WOLFSSL_FATAL_ERROR;
        }
    }

    printf(resultFmt, ret == 0 ? passed : failed);

    if (wc_FreeRng(&rng) && ret == 0) {
        ret = WOLFSSL_FATAL_ERROR;
    }
    wc_ecc_free(&key);

#ifdef FP_ECC
    wc_ecc_fp_free();
#endif

#endif
    return ret;

} /* END test_wc_ecc_export_x963 */

/*
 * Testing wc_ecc_export_x963_ex()
 * compile with --enable-compkey will use compression.
 */
static int test_wc_ecc_export_x963_ex (void)
{
    int     ret = 0;

#if defined(HAVE_ECC) && defined(HAVE_ECC_KEY_EXPORT) && !defined(WC_NO_RNG)
    ecc_key key;
    WC_RNG  rng;
    byte    out[ECC_ASN963_MAX_BUF_SZ];
    word32  outlen = sizeof(out);
    #ifdef HAVE_COMP_KEY
        word32  badOutLen = 5;
    #endif

    /* Init stack variables. */
    XMEMSET(out, 0, outlen);

    ret = wc_InitRng(&rng);
    if (ret == 0) {
        ret = wc_ecc_init(&key);
        if (ret == 0) {
            ret = wc_ecc_make_key(&rng, KEY64, &key);
        }
    }

    printf(testingFmt, "wc_ecc_export_x963_ex()");

    #ifdef HAVE_COMP_KEY
        if (ret == 0) {
            ret = wc_ecc_export_x963_ex(&key, out, &outlen, COMP);
        }
    #else
        if (ret == 0) {
            ret = wc_ecc_export_x963_ex(&key, out, &outlen, NOCOMP);
        }
    #endif

    /* Test bad args. */
    #ifdef HAVE_COMP_KEY
    if (ret == 0) {
        ret = wc_ecc_export_x963_ex(NULL, out, &outlen, COMP);
        if (ret == BAD_FUNC_ARG) {
            ret = wc_ecc_export_x963_ex(&key, NULL, &outlen, COMP);
        }
        if (ret == BAD_FUNC_ARG) {
            ret = wc_ecc_export_x963_ex(&key, out, NULL, COMP);
        }
        if (ret == BAD_FUNC_ARG) {
            ret = wc_ecc_export_x963_ex(&key, out, &badOutLen, COMP);
        }
        if (ret == BUFFER_E) {
            key.idx = -4;
            ret = wc_ecc_export_x963_ex(&key, out, &outlen, COMP);
        }
        if (ret == ECC_BAD_ARG_E) {
            ret = 0;
        } else {
            ret = WOLFSSL_FATAL_ERROR;
        }
    }
    #else
        if (ret == 0) {
            ret = wc_ecc_export_x963_ex(NULL, out, &outlen, NOCOMP);
            if (ret == BAD_FUNC_ARG) {
                ret = wc_ecc_export_x963_ex(&key, NULL, &outlen, NOCOMP);
            }
            if (ret == BAD_FUNC_ARG) {
                ret = wc_ecc_export_x963_ex(&key, out, &outlen, 1);
            }
            if (ret == NOT_COMPILED_IN) {
                ret = wc_ecc_export_x963_ex(&key, out, NULL, NOCOMP);
            }
            if (ret == BAD_FUNC_ARG) {
                key.idx = -4;
                ret = wc_ecc_export_x963_ex(&key, out, &outlen, NOCOMP);
            }
            if (ret == ECC_BAD_ARG_E) {
                ret = 0;
            } else if (ret == 0) {
                ret = WOLFSSL_FATAL_ERROR;
            }
        }
    #endif

    printf(resultFmt, ret == 0 ? passed : failed);

    if (wc_FreeRng(&rng) && ret == 0) {
        ret = WOLFSSL_FATAL_ERROR;
    }
    wc_ecc_free(&key);

#ifdef FP_ECC
    wc_ecc_fp_free();
#endif

#endif
    return ret;

} /* END test_wc_ecc_export_x963_ex */

/*
 * testing wc_ecc_import_x963()
 */
static int test_wc_ecc_import_x963 (void)
{
    int     ret = 0;

#if defined(HAVE_ECC) && defined(HAVE_ECC_KEY_IMPORT) && \
    defined(HAVE_ECC_KEY_EXPORT) && !defined(WC_NO_RNG)
    ecc_key pubKey, key;
    WC_RNG  rng;
    byte    x963[ECC_ASN963_MAX_BUF_SZ];
    word32  x963Len = (word32)sizeof(x963);

    /* Init stack variables. */
    XMEMSET(x963, 0, x963Len);
    ret = wc_InitRng(&rng);
    if (ret == 0) {
        ret = wc_ecc_init(&pubKey);
        if (ret == 0) {
            ret = wc_ecc_init(&key);
        }
        if (ret == 0) {
            ret = wc_ecc_make_key(&rng, KEY24, &key);
        }
        if (ret == 0) {
            ret = wc_ecc_export_x963(&key, x963, &x963Len);
        }
    }

    printf(testingFmt, "wc_ecc_import_x963()");
    if (ret == 0) {
        ret = wc_ecc_import_x963(x963, x963Len, &pubKey);
    }

    /* Test bad args. */
    if (ret == 0) {
        ret = wc_ecc_import_x963(NULL, x963Len, &pubKey);
        if (ret == BAD_FUNC_ARG) {
            ret = wc_ecc_import_x963(x963, x963Len, NULL);
        }
        if (ret == BAD_FUNC_ARG) {
            ret = wc_ecc_import_x963(x963, x963Len + 1, &pubKey);
        }
        if (ret == ECC_BAD_ARG_E) {
            ret = 0;
        } else if (ret == 0) {
            ret = WOLFSSL_FATAL_ERROR;
        }
    }

    printf(resultFmt, ret == 0 ? passed : failed);

    if (wc_FreeRng(&rng) && ret == 0) {
        ret = WOLFSSL_FATAL_ERROR;
    }
    wc_ecc_free(&key);
    wc_ecc_free(&pubKey);

#ifdef FP_ECC
    wc_ecc_fp_free();
#endif

#endif
    return ret;

} /* END wc_ecc_import_x963 */

/*
 * testing wc_ecc_import_private_key()
 */
static int ecc_import_private_key (void)
{
    int     ret = 0;

#if defined(HAVE_ECC) && defined(HAVE_ECC_KEY_IMPORT) && \
    defined(HAVE_ECC_KEY_EXPORT) && !defined(WC_NO_RNG)
    ecc_key key, keyImp;
    WC_RNG  rng;
    byte    privKey[ECC_PRIV_KEY_BUF]; /* Raw private key.*/
    byte    x963Key[ECC_ASN963_MAX_BUF_SZ];
    word32  privKeySz = (word32)sizeof(privKey);
    word32  x963KeySz = (word32)sizeof(x963Key);

    /* Init stack variables. */
    XMEMSET(privKey, 0, privKeySz);
    XMEMSET(x963Key, 0, x963KeySz);

    ret = wc_InitRng(&rng);
    if (ret == 0) {
        ret = wc_ecc_init(&key);
        if (ret == 0) {
            ret = wc_ecc_init(&keyImp);
        }
        if (ret == 0) {
            ret = wc_ecc_make_key(&rng, KEY48, &key);
        }
        if (ret == 0) {
            ret = wc_ecc_export_x963(&key, x963Key, &x963KeySz);
        }
        if (ret == 0) {
            ret = wc_ecc_export_private_only(&key, privKey, &privKeySz);
        }
    }

    printf(testingFmt, "wc_ecc_import_private_key()");

    if (ret == 0) {
        ret = wc_ecc_import_private_key(privKey, privKeySz, x963Key,
                                                x963KeySz, &keyImp);
    }
    /* Pass in bad args. */
    if (ret == 0) {
        ret = wc_ecc_import_private_key(privKey, privKeySz, x963Key,
                                                x963KeySz, NULL);
        if (ret == BAD_FUNC_ARG) {
            ret = wc_ecc_import_private_key(NULL, privKeySz, x963Key,
                                                x963KeySz, &keyImp);
        }
        if (ret == BAD_FUNC_ARG) {
            ret = 0;
        } else if (ret == 0) {
            ret = WOLFSSL_FATAL_ERROR;
        }
    }

    printf(resultFmt, ret == 0 ? passed : failed);

    if (wc_FreeRng(&rng) && ret == 0) {
        ret = WOLFSSL_FATAL_ERROR;
    }
    wc_ecc_free(&key);
    wc_ecc_free(&keyImp);

#ifdef FP_ECC
    wc_ecc_fp_free();
#endif

#endif
    return ret;

} /* END wc_ecc_import_private_key */


/*
 * Testing wc_ecc_export_private_only()
 */
static int test_wc_ecc_export_private_only (void)
{
    int     ret = 0;

#if defined(HAVE_ECC) && defined(HAVE_ECC_KEY_EXPORT) && !defined(WC_NO_RNG)
    ecc_key key;
    WC_RNG  rng;
    byte    out[ECC_PRIV_KEY_BUF];
    word32  outlen = sizeof(out);

    /* Init stack variables. */
    XMEMSET(out, 0, outlen);

    ret = wc_InitRng(&rng);
    if (ret == 0) {
        ret = wc_ecc_init(&key);
        if (ret == 0) {
            ret = wc_ecc_make_key(&rng, KEY32, &key);
        }
    }
    printf(testingFmt, "wc_ecc_export_private_only()");

    if (ret == 0) {
        ret = wc_ecc_export_private_only(&key, out, &outlen);
    }
    /* Pass in bad args. */
    if (ret == 0) {
        ret = wc_ecc_export_private_only(NULL, out, &outlen);
        if (ret == BAD_FUNC_ARG) {
            ret = wc_ecc_export_private_only(&key, NULL, &outlen);
        }
        if (ret == BAD_FUNC_ARG) {
            ret = wc_ecc_export_private_only(&key, out, NULL);
        }
        if (ret == BAD_FUNC_ARG) {
            ret = 0;
        } else if (ret == 0) {
            ret = WOLFSSL_FATAL_ERROR;
        }
    }

    printf(resultFmt, ret == 0 ? passed : failed);

    if (wc_FreeRng(&rng) && ret == 0) {
        ret = WOLFSSL_FATAL_ERROR;
    }
    wc_ecc_free(&key);

#ifdef FP_ECC
    wc_ecc_fp_free();
#endif

#endif
    return ret;

} /* END test_wc_ecc_export_private_only */


/*
 * Testing wc_ecc_rs_to_sig()
 */
static int test_wc_ecc_rs_to_sig (void)
{

    int           ret = 0;

#if defined(HAVE_ECC) && !defined(NO_ASN)
    /* first [P-192,SHA-1] vector from FIPS 186-3 NIST vectors */
    const char*   R = "6994d962bdd0d793ffddf855ec5bf2f91a9698b46258a63e";
    const char*   S = "02ba6465a234903744ab02bc8521405b73cf5fc00e1a9f41";
    byte          sig[ECC_MAX_SIG_SIZE];
    word32        siglen = (word32)sizeof(sig);
    /*R and S max size is the order of curve. 2^192.*/
    int           keySz = KEY24;
    byte          r[KEY24];
    byte          s[KEY24];
    word32        rlen = (word32)sizeof(r);
    word32        slen = (word32)sizeof(s);

    /* Init stack variables. */
    XMEMSET(sig, 0, ECC_MAX_SIG_SIZE);
    XMEMSET(r, 0, keySz);
    XMEMSET(s, 0, keySz);

    printf(testingFmt, "wc_ecc_rs_to_sig()");

    ret = wc_ecc_rs_to_sig(R, S, sig, &siglen);
    /* Test bad args. */
    if (ret == 0) {
        ret = wc_ecc_rs_to_sig(NULL, S, sig, &siglen);
        if (ret == ECC_BAD_ARG_E) {
            ret = wc_ecc_rs_to_sig(R, NULL, sig, &siglen);
        }
        if (ret == ECC_BAD_ARG_E) {
            ret = wc_ecc_rs_to_sig(R, S, sig, NULL);
        }
        if (ret == ECC_BAD_ARG_E) {
            ret = wc_ecc_rs_to_sig(R, S, NULL, &siglen);
        }
        if (ret == ECC_BAD_ARG_E) {
            ret = 0;
        } else {
            ret = WOLFSSL_FATAL_ERROR;
        }
    }

    printf(resultFmt, ret == 0 ? passed : failed);
    printf(testingFmt, "wc_ecc_sig_to_rs()");
    if (ret == 0) {
        ret = wc_ecc_sig_to_rs(sig, siglen, r, &rlen, s, &slen);
    }
    /* Test bad args. */
    if (ret == 0) {
        ret = wc_ecc_sig_to_rs(NULL, siglen, r, &rlen, s, &slen);
        if (ret == ECC_BAD_ARG_E) {
            ret = wc_ecc_sig_to_rs(sig, siglen, NULL, &rlen, s, &slen);
        }
        if (ret == ECC_BAD_ARG_E) {
            ret = wc_ecc_sig_to_rs(sig, siglen, r, NULL, s, &slen);
        }
        if (ret == ECC_BAD_ARG_E) {
            ret = wc_ecc_sig_to_rs(sig, siglen, r, &rlen, NULL, &slen);
        }
        if (ret == ECC_BAD_ARG_E) {
            ret = wc_ecc_sig_to_rs(sig, siglen, r, &rlen, s, NULL);
        }
        if (ret == ECC_BAD_ARG_E) {
            ret = 0;
        } else if (ret == 0) {
            ret = WOLFSSL_FATAL_ERROR;
        }
    }

    printf(resultFmt, ret == 0 ? passed : failed);

#endif
    return ret;

} /* END test_wc_ecc_rs_to_sig */

static int test_wc_ecc_import_raw (void)
{
    int         ret = 0;

#ifdef HAVE_ECC
    ecc_key     key;
#ifdef HAVE_ALL_CURVES
    const char* qx = "07008ea40b08dbe76432096e80a2494c94982d2d5bcf98e6";
    const char* qy = "76fab681d00b414ea636ba215de26d98c41bd7f2e4d65477";
    const char* d  = "e14f37b3d1374ff8b03f41b9b3fdd2f0ebccf275d660d7f3";
    const char* curveName = "SECP192R1";
#else
    const char* qx =
              "6c450448386596485678dcf46ccf75e80ff292443cddab1ff216d0c72cd9341";
    const char* qy =
              "9cac72ff8a90e4939e37714bfa07ae4612588535c3fdeab63ceb29b1d80f0d1";
    const char* d  =
             "1e1dd938e15bdd036b0b0e2a6dc62fe7b46dbe042ac42310c6d5db0cda63e807";
    const char* curveName = "SECP256R1";
#endif

    ret = wc_ecc_init(&key);

    printf(testingFmt, "wc_ecc_import_raw()");

    if (ret == 0) {
        ret = wc_ecc_import_raw(&key, qx, qy, d, curveName);
    }
    /* Test bad args. */
    if (ret == 0) {
        ret = wc_ecc_import_raw(NULL, qx, qy, d, curveName);
        if (ret == BAD_FUNC_ARG) {
            ret = wc_ecc_import_raw(&key, NULL, qy, d, curveName);
        }
        if (ret == BAD_FUNC_ARG) {
            ret = wc_ecc_import_raw(&key, qx, NULL, d, curveName);
        }
        if (ret == BAD_FUNC_ARG) {
            ret = wc_ecc_import_raw(&key, qx, qy, d, NULL);
        }
        if (ret == BAD_FUNC_ARG) {
            ret = 0;
        } else if (ret == 0) {
            ret = WOLFSSL_FATAL_ERROR;
        }
    }

    printf(resultFmt, ret == 0 ? passed : failed);

    wc_ecc_free(&key);

#endif

    return ret;

} /* END test_wc_ecc_import_raw */


/*
 * Testing wc_ecc_sig_size()
 */
static int test_wc_ecc_sig_size (void)
{
   int         ret = 0;

#if defined(HAVE_ECC) && !defined(WC_NO_RNG)
    ecc_key     key;
    WC_RNG      rng;
    int         keySz = KEY16;

    ret = wc_InitRng(&rng);
    if (ret == 0) {
        ret = wc_ecc_init(&key);
        if (ret == 0) {
            ret = wc_ecc_make_key(&rng, keySz, &key);
        }
    }

    printf(testingFmt, "wc_ecc_sig_size()");

    if (ret == 0) {
        ret = wc_ecc_sig_size(&key);
        if (ret <= (2 * keySz + SIG_HEADER_SZ + ECC_MAX_PAD_SZ)) {
            ret = 0;
        }
    }

    printf(resultFmt, ret == 0 ? passed : failed);

    if (wc_FreeRng(&rng) && ret == 0) {
        ret = WOLFSSL_FATAL_ERROR;
    }
    wc_ecc_free(&key);

#endif
    return ret;

} /* END test_wc_ecc_sig_size */

/*
 * Testing wc_ecc_ctx_new()
 */
static int test_wc_ecc_ctx_new (void)
{
    int         ret = 0;

#if defined(HAVE_ECC) && defined(HAVE_ECC_ENCRYPT) && !defined(WC_NO_RNG)
    WC_RNG      rng;
    ecEncCtx*   cli = NULL;
    ecEncCtx*   srv = NULL;

    ret = wc_InitRng(&rng);

    printf(testingFmt, "wc_ecc_ctx_new()");
    if (ret == 0) {
        cli = wc_ecc_ctx_new(REQ_RESP_CLIENT, &rng);
        srv = wc_ecc_ctx_new(REQ_RESP_SERVER, &rng);
    }
    if (ret == 0 && (cli == NULL || srv == NULL)) {
        ret = WOLFSSL_FATAL_ERROR;
    }

    wc_ecc_ctx_free(cli);
    wc_ecc_ctx_free(srv);

    /* Test bad args. */
    if (ret == 0) {
        /* wc_ecc_ctx_new_ex() will free if returned NULL. */
        cli = wc_ecc_ctx_new(0, &rng);
        if (cli != NULL) {
            ret = WOLFSSL_FATAL_ERROR;
        }
        cli = wc_ecc_ctx_new(REQ_RESP_CLIENT, NULL);
        if (cli != NULL) {
            ret = WOLFSSL_FATAL_ERROR;
        }
    }

    printf(resultFmt, ret == 0 ? passed : failed);

    if (wc_FreeRng(&rng) && ret == 0) {
        ret = WOLFSSL_FATAL_ERROR;
    }
    wc_ecc_ctx_free(cli);

#endif
    return ret;

} /* END test_wc_ecc_ctx_new */

/*
 * Tesing wc_ecc_reset()
 */
static int test_wc_ecc_ctx_reset (void)
{
    int         ret = 0;

#if defined(HAVE_ECC) && defined(HAVE_ECC_ENCRYPT) && !defined(WC_NO_RNG)
    ecEncCtx*   ctx = NULL;
    WC_RNG      rng;

    ret = wc_InitRng(&rng);
    if (ret == 0) {
        if ( (ctx = wc_ecc_ctx_new(REQ_RESP_CLIENT, &rng)) == NULL ) {
            ret = WOLFSSL_FATAL_ERROR;
        }
    }

    printf(testingFmt, "wc_ecc_ctx_reset()");

    if (ret == 0) {
        ret = wc_ecc_ctx_reset(ctx, &rng);
    }

    /* Pass in bad args. */
    if (ret == 0) {
        ret = wc_ecc_ctx_reset(NULL, &rng);
        if (ret == BAD_FUNC_ARG) {
            ret = wc_ecc_ctx_reset(ctx, NULL);
        }
        if (ret == BAD_FUNC_ARG) {
            ret = 0;
        } else if (ret == 0) {
            ret = WOLFSSL_FATAL_ERROR;
        }
    }

    printf(resultFmt, ret == 0 ? passed : failed);

    if (wc_FreeRng(&rng) && ret == 0) {
        ret = WOLFSSL_FATAL_ERROR;
    }
    wc_ecc_ctx_free(ctx);

#endif
    return ret;

} /* END test_wc_ecc_ctx_reset */

/*
 * Testing wc_ecc_ctx_set_peer_salt() and wc_ecc_ctx_get_own_salt()
 */
static int test_wc_ecc_ctx_set_peer_salt (void)
{
    int         ret = 0;

#if defined(HAVE_ECC) && defined(HAVE_ECC_ENCRYPT) && !defined(WC_NO_RNG)
    WC_RNG          rng;
    ecEncCtx*       cliCtx      = NULL;
    ecEncCtx*       servCtx     = NULL;
    const byte*     cliSalt     = NULL;
    const byte*     servSalt    = NULL;

    ret = wc_InitRng(&rng);
    if (ret == 0) {
        if ( ( (cliCtx = wc_ecc_ctx_new(REQ_RESP_CLIENT, &rng)) == NULL ) ||
           ( (servCtx = wc_ecc_ctx_new(REQ_RESP_SERVER, &rng)) == NULL) ) {
            ret = WOLFSSL_FATAL_ERROR;
        }
    }

    printf(testingFmt, "wc_ecc_ctx_get_own_salt()");

    /* Test bad args. */
    if (ret == 0) {
        cliSalt  = wc_ecc_ctx_get_own_salt(NULL);
        if (cliSalt != NULL) {
            ret = WOLFSSL_FATAL_ERROR;
        }
    }

    if (ret == 0) {
        cliSalt  = wc_ecc_ctx_get_own_salt(cliCtx);
        servSalt = wc_ecc_ctx_get_own_salt(servCtx);
        if (cliSalt == NULL || servSalt == NULL) {
            ret = WOLFSSL_FATAL_ERROR;
        }
    }

    printf(resultFmt, ret == 0 ? passed : failed);
    printf(testingFmt, "wc_ecc_ctx_set_peer_salt()");

    if (ret == 0) {
        ret = wc_ecc_ctx_set_peer_salt(cliCtx, servSalt);
    }
    /* Test bad args. */
    if (ret == 0) {
        ret = wc_ecc_ctx_set_peer_salt(NULL, servSalt);
        if (ret == BAD_FUNC_ARG) {
            ret = wc_ecc_ctx_set_peer_salt(cliCtx, NULL);
        }
        if (ret == BAD_FUNC_ARG) {
            ret = 0;
        } else if (ret == 0) {
            ret = WOLFSSL_FATAL_ERROR;
        }
    }

    printf(resultFmt, ret == 0 ? passed : failed);

    if (wc_FreeRng(&rng) && ret == 0) {
        ret = WOLFSSL_FATAL_ERROR;
    }
    wc_ecc_ctx_free(cliCtx);
    wc_ecc_ctx_free(servCtx);

#endif
    return ret;

} /* END test_wc_ecc_ctx_set_peer_salt */

/*
 * Testing wc_ecc_ctx_set_info()
 */
static int test_wc_ecc_ctx_set_info (void)
{
    int         ret = 0;

#if defined(HAVE_ECC) && defined(HAVE_ECC_ENCRYPT) && !defined(WC_NO_RNG)
    ecEncCtx*   ctx = NULL;
    WC_RNG      rng;
    const char* optInfo = "Optional Test Info.";
    int         optInfoSz = (int)XSTRLEN(optInfo);
    const char* badOptInfo = NULL;

    ret = wc_InitRng(&rng);
    if ( (ctx = wc_ecc_ctx_new(REQ_RESP_CLIENT, &rng)) == NULL || ret != 0 ) {
        ret = WOLFSSL_FATAL_ERROR;
    }

    printf(testingFmt, "wc_ecc_ctx_set_info()");

    if (ret == 0) {
        ret = wc_ecc_ctx_set_info(ctx, (byte*)optInfo, optInfoSz);
    }
    /* Test bad args. */
    if (ret == 0) {
        ret = wc_ecc_ctx_set_info(NULL, (byte*)optInfo, optInfoSz);
        if (ret == BAD_FUNC_ARG) {
            ret = wc_ecc_ctx_set_info(ctx, (byte*)badOptInfo, optInfoSz);
        }
        if (ret == BAD_FUNC_ARG) {
            ret = wc_ecc_ctx_set_info(ctx, (byte*)optInfo, -1);
        }
        if (ret == BAD_FUNC_ARG) {
            ret = 0;
        } else if (ret == 0) {
            ret = WOLFSSL_FATAL_ERROR;
        }
    }

    printf(resultFmt, ret == 0 ? passed : failed);

    if (wc_FreeRng(&rng) && ret == 0) {
        ret = WOLFSSL_FATAL_ERROR;
    }
    wc_ecc_ctx_free(ctx);

#endif
    return ret;

} /* END test_wc_ecc_ctx_set_info */

/*
 * Testing wc_ecc_encrypt() and wc_ecc_decrypt()
 */
static int test_wc_ecc_encryptDecrypt (void)
{
    int         ret = 0;

#if defined(HAVE_ECC) && defined(HAVE_ECC_ENCRYPT) && defined(WOLFSSL_AES_128) \
     && !defined(WC_NO_RNG)
    ecc_key     srvKey, cliKey;
    WC_RNG      rng;
    const char* msg   = "EccBlock Size 16";
    word32      msgSz = (word32)XSTRLEN(msg);
    byte        out[XSTRLEN(msg) + WC_SHA256_DIGEST_SIZE];
    word32      outSz = (word32)sizeof(out);
    byte        plain[XSTRLEN(msg) + 1];
    word32      plainSz = (word32)sizeof(plain);
    int         keySz = KEY20;

    /* Init stack variables. */
    XMEMSET(out, 0, outSz);
    XMEMSET(plain, 0, plainSz);

    ret = wc_InitRng(&rng);
    if (ret == 0) {
        ret = wc_ecc_init(&cliKey);
        if (ret == 0) {
            ret = wc_ecc_make_key(&rng, keySz, &cliKey);
        }
        if (ret == 0) {
            ret = wc_ecc_init(&srvKey);
        }
        if (ret == 0) {
            ret = wc_ecc_make_key(&rng, keySz, &srvKey);
        }
    }

    printf(testingFmt, "wc_ecc_encrypt()");

    if (ret == 0) {
        ret = wc_ecc_encrypt(&cliKey, &srvKey, (byte*)msg, msgSz, out,
                                                            &outSz, NULL);
    }
    if (ret == 0) {
        ret = wc_ecc_encrypt(NULL, &srvKey, (byte*)msg, msgSz, out,
                                                            &outSz, NULL);
        if (ret == BAD_FUNC_ARG) {
            ret = wc_ecc_encrypt(&cliKey, NULL, (byte*)msg, msgSz, out,
                                                            &outSz, NULL);
        }
        if (ret == BAD_FUNC_ARG) {
            ret = wc_ecc_encrypt(&cliKey, &srvKey, NULL, msgSz, out,
                                                            &outSz, NULL);
        }
        if (ret == BAD_FUNC_ARG) {
            ret = wc_ecc_encrypt(&cliKey, &srvKey, (byte*)msg, msgSz, NULL,
                                                            &outSz, NULL);
        }
        if (ret == BAD_FUNC_ARG) {
            ret = wc_ecc_encrypt(&cliKey, &srvKey, (byte*)msg, msgSz, out,
                                                            NULL, NULL);
        }
        if (ret == BAD_FUNC_ARG) {
            ret = 0;
        } else if (ret == 0) {
            ret = WOLFSSL_FATAL_ERROR;
        }
    }

    printf(resultFmt, ret == 0 ? passed : failed);
    printf(testingFmt, "wc_ecc_decrypt()");

    if (ret == 0) {
        ret = wc_ecc_decrypt(&srvKey, &cliKey, out, outSz, plain,
                                                        &plainSz, NULL);
    }
    if (ret == 0) {
        ret = wc_ecc_decrypt(NULL, &cliKey, out, outSz, plain,
                                                        &plainSz, NULL);
        if (ret == BAD_FUNC_ARG) {
            ret = wc_ecc_decrypt(&srvKey, NULL, out, outSz, plain,
                                                        &plainSz, NULL);
        }
        if (ret == BAD_FUNC_ARG) {
            ret = wc_ecc_decrypt(&srvKey, &cliKey, NULL, outSz, plain,
                                                        &plainSz, NULL);
        }
        if (ret == BAD_FUNC_ARG) {
            ret = wc_ecc_decrypt(&srvKey, &cliKey, out, outSz, NULL,
                                                        &plainSz, NULL);
        }
        if (ret == BAD_FUNC_ARG) {
            ret = wc_ecc_decrypt(&srvKey, &cliKey, out, outSz,
                                                        plain, NULL, NULL);
        }
        if (ret == BAD_FUNC_ARG) {
            ret = 0;
        } else if (ret == 0) {
            ret = WOLFSSL_FATAL_ERROR;
        }
    }

    if (XMEMCMP(msg, plain, msgSz) != 0) {
        ret = WOLFSSL_FATAL_ERROR;
    }

    printf(resultFmt, ret == 0 ? passed : failed);

    if (wc_FreeRng(&rng) && ret == 0) {
        ret = WOLFSSL_FATAL_ERROR;
    }
    wc_ecc_free(&cliKey);
    wc_ecc_free(&srvKey);

#endif
    return ret;

} /* END test_wc_ecc_encryptDecrypt */

/*
 * Testing wc_ecc_del_point() and wc_ecc_new_point()
 */
static int test_wc_ecc_del_point (void)
{
    int         ret = 0;

#if defined(HAVE_ECC)
    ecc_point*   pt;

    printf(testingFmt, "wc_ecc_new_point()");

    pt = wc_ecc_new_point();
    if (!pt) {
        ret = WOLFSSL_FATAL_ERROR;
    }

    printf(resultFmt, ret == 0 ? passed : failed);

    wc_ecc_del_point(pt);

#endif
    return ret;

} /* END test_wc_ecc_del_point */

/*
 * Testing wc_ecc_point_is_at_infinity(), wc_ecc_export_point_der(),
 * wc_ecc_import_point_der(), wc_ecc_copy_point(), and wc_ecc_cmp_point()
 */
static int test_wc_ecc_pointFns (void)
{
    int         ret = 0;

#if defined(HAVE_ECC) && defined(HAVE_ECC_KEY_EXPORT) && \
    !defined(WC_NO_RNG) && !defined(WOLFSSL_ATECC508A)
    ecc_key     key;
    WC_RNG      rng;
    ecc_point*  point = NULL;
    ecc_point*  cpypt = NULL;
    int         idx = 0;
    int         keySz = KEY32;
    byte        der[DER_SZ];
    word32      derlenChk = 0;
    word32      derSz = (int)sizeof(der);

    /* Init stack variables. */
    XMEMSET(der, 0, derSz);

    ret = wc_InitRng(&rng);
    if (ret == 0) {
        ret = wc_ecc_init(&key);
        if (ret == 0) {
            ret = wc_ecc_make_key(&rng, keySz, &key);
        }
    }

    if (ret == 0) {
        point = wc_ecc_new_point();
        if (!point) {
            ret = WOLFSSL_FATAL_ERROR;
        }
    }

    if (ret == 0) {
        cpypt = wc_ecc_new_point();
        if (!cpypt) {
            ret = WOLFSSL_FATAL_ERROR;
        }
    }

    /* Export */
    printf(testingFmt, "wc_ecc_export_point_der()");
    if (ret == 0) {
        ret = wc_ecc_export_point_der((idx = key.idx), &key.pubkey,
                                                       NULL, &derlenChk);
        /* Check length value. */
        if (derSz == derlenChk && ret == LENGTH_ONLY_E) {
            ret = wc_ecc_export_point_der((idx = key.idx), &key.pubkey,
                                                           der, &derSz);
        }
    }
    /* Test bad args. */
    if (ret == 0) {
        ret = wc_ecc_export_point_der(-2, &key.pubkey, der, &derSz);
        if (ret == ECC_BAD_ARG_E) {
            ret = wc_ecc_export_point_der((idx = key.idx), NULL, der, &derSz);
        }
        if (ret == ECC_BAD_ARG_E) {
            ret = wc_ecc_export_point_der((idx = key.idx), &key.pubkey,
                                                                der, NULL);
        }
        if (ret == ECC_BAD_ARG_E) {
            ret = 0;
        } else if (ret == 0) {
            ret = WOLFSSL_FATAL_ERROR;
        }
    }

    printf(resultFmt, ret == 0 ? passed : failed);

    /* Import */
    printf(testingFmt, "wc_ecc_import_point_der()");

    if (ret == 0) {
        ret = wc_ecc_import_point_der(der, derSz, idx, point);
        /* Condition double checks wc_ecc_cmp_point().  */
        if (ret == 0 && XMEMCMP(&key.pubkey, point, sizeof(key.pubkey))) {
            ret = wc_ecc_cmp_point(&key.pubkey, point);
        }
    }
    /* Test bad args. */
    if (ret == 0) {
        ret = wc_ecc_import_point_der(NULL, derSz, idx, point);
        if (ret == ECC_BAD_ARG_E) {
            ret = wc_ecc_import_point_der(der, derSz, idx, NULL);
        }
        if (ret == ECC_BAD_ARG_E) {
            ret = wc_ecc_import_point_der(der, derSz, -1, point);
        }
        if (ret == ECC_BAD_ARG_E) {
            ret = wc_ecc_import_point_der(der, derSz + 1, idx, point);
        }
        if (ret == ECC_BAD_ARG_E) {
            ret = 0;
        } else if (ret == 0) {
            ret = WOLFSSL_FATAL_ERROR;
        }
    }
    printf(resultFmt, ret == 0 ? passed : failed);

    /* Copy */
    printf(testingFmt, "wc_ecc_copy_point()");

    if (ret == 0) {
        ret = wc_ecc_copy_point(point, cpypt);
    }
    /* Test bad args. */
    if (ret == 0) {
        ret = wc_ecc_copy_point(NULL, cpypt);
        if (ret == ECC_BAD_ARG_E) {
            ret = wc_ecc_copy_point(point, NULL);
        }
        if (ret == ECC_BAD_ARG_E) {
            ret = 0;
        } else if (ret == 0) {
            ret = WOLFSSL_FATAL_ERROR;
        }
    }

    printf(resultFmt, ret == 0 ? passed : failed);

    printf(testingFmt, "wc_ecc_cmp_point()");
    /* Compare point */
    if (ret == 0) {
        ret = wc_ecc_cmp_point(point, cpypt);
    }
    /* Test bad args. */
    if (ret == 0) {
        ret = wc_ecc_cmp_point(NULL, cpypt);
        if (ret == BAD_FUNC_ARG) {
            ret = wc_ecc_cmp_point(point, NULL);
        }
        if (ret == BAD_FUNC_ARG) {
            ret = 0;
        } else if (ret == 0) {
            ret = WOLFSSL_FATAL_ERROR;
        }
    }
    printf(resultFmt, ret == 0 ? passed : failed);

    printf(testingFmt, "wc_ecc_point_is_at_infinity()");
    /* At infinity if return == 1, otherwise return == 0. */
    if (ret == 0) {
        ret = wc_ecc_point_is_at_infinity(point);
    }
    /* Test bad args. */
    if (ret == 0) {
        ret = wc_ecc_point_is_at_infinity(NULL);
        if (ret == BAD_FUNC_ARG) {
            ret = 0;
        } else if (ret == 0) {
            ret = WOLFSSL_FATAL_ERROR;
        }
    }

    printf(resultFmt, ret == 0 ? passed : failed);

    /* Free */
    wc_ecc_del_point(point);
    wc_ecc_del_point(cpypt);
    wc_ecc_free(&key);
    if (wc_FreeRng(&rng) && ret == 0) {
        ret = WOLFSSL_FATAL_ERROR;
    }

#endif
    return ret;

} /* END test_wc_ecc_pointFns */


/*
 * Testing wc_ecc_sahred_secret_ssh()
 */
static int test_wc_ecc_shared_secret_ssh (void)
{
    int         ret = 0;

#if defined(HAVE_ECC) && defined(HAVE_ECC_DHE) && \
    !defined(WC_NO_RNG) && !defined(WOLFSSL_ATECC508A)
    ecc_key     key, key2;
    WC_RNG      rng;
    int         keySz = KEY32;
    int         key2Sz = KEY24;
    byte        secret[KEY32];
    word32      secretLen = keySz;

    /* Init stack variables. */
    XMEMSET(secret, 0, secretLen);

    /* Make keys */
    ret = wc_InitRng(&rng);
    if (ret == 0) {
        ret = wc_ecc_init(&key);
        if (ret == 0) {
            ret = wc_ecc_make_key(&rng, keySz, &key);
        }
        if (wc_FreeRng(&rng) && ret == 0) {
            ret = WOLFSSL_FATAL_ERROR;
        }
    }
    if (ret == 0) {
        ret = wc_InitRng(&rng);
        if (ret == 0) {
            ret = wc_ecc_init(&key2);
        }
        if (ret == 0) {
            ret = wc_ecc_make_key(&rng, key2Sz, &key2);
        }
    }

    printf(testingFmt, "ecc_shared_secret_ssh()");

    if (ret == 0) {
        ret = wc_ecc_shared_secret_ssh(&key, &key2.pubkey, secret, &secretLen);
    }
    /* Pass in bad args. */
    if (ret == 0) {
        ret = wc_ecc_shared_secret_ssh(NULL, &key2.pubkey, secret, &secretLen);
        if (ret == BAD_FUNC_ARG) {
            ret = wc_ecc_shared_secret_ssh(&key, NULL, secret, &secretLen);
        }
        if (ret == BAD_FUNC_ARG) {
            ret = wc_ecc_shared_secret_ssh(&key, &key2.pubkey, NULL, &secretLen);
        }
        if (ret == BAD_FUNC_ARG) {
            ret = wc_ecc_shared_secret_ssh(&key, &key2.pubkey, secret, NULL);
        }
        if (ret == BAD_FUNC_ARG) {
            key.type = ECC_PUBLICKEY;
            ret = wc_ecc_shared_secret_ssh(&key, &key2.pubkey, secret, &secretLen);
            if (ret == ECC_BAD_ARG_E) {
                ret = 0;
            } else if (ret == 0) {
                ret = WOLFSSL_FATAL_ERROR;
            }
        } else if (ret == 0) {
            ret = WOLFSSL_FATAL_ERROR;
        }
    }

    printf(resultFmt, ret == 0 ? passed : failed);

    if (wc_FreeRng(&rng) && ret == 0) {
        ret = WOLFSSL_FATAL_ERROR;
    }
    wc_ecc_free(&key);
    wc_ecc_free(&key2);

#ifdef FP_ECC
    wc_ecc_fp_free();
#endif

#endif
    return ret;

} /* END test_wc_ecc_shared_secret_ssh */

/*
 * Testing wc_ecc_verify_hash_ex() and wc_ecc_verify_hash_ex()
 */
static int test_wc_ecc_verify_hash_ex (void)
{
    int             ret = 0;

#if defined(HAVE_ECC) && defined(HAVE_ECC_SIGN) && defined(WOLFSSL_PUBLIC_MP) \
     && !defined(WC_NO_RNG) && !defined(WOLFSSL_ATECC508A)
    ecc_key         key;
    WC_RNG          rng;
    mp_int          r;
    mp_int          s;
    unsigned char   hash[] = "Everyone gets Friday off.EccSig";
    unsigned char   iHash[] = "Everyone gets Friday off.......";
    unsigned char   shortHash[] = "Everyone gets Friday off.";
    word32          hashlen = sizeof(hash);
    word32          iHashLen = sizeof(iHash);
    word32          shortHashLen = sizeof(shortHash);
    int             keySz = KEY32;
    int             sig = WOLFSSL_FATAL_ERROR;
    int             ver = WOLFSSL_FATAL_ERROR;
    int             verify_ok = 0;

    /* Initialize r and s. */
    ret = mp_init_multi(&r, &s, NULL, NULL, NULL, NULL);
    if (ret != MP_OKAY) {
        return MP_INIT_E;
    }

    ret = wc_InitRng(&rng);
    if (ret == 0) {
        ret = wc_ecc_init(&key);
        if (ret == 0) {
            ret = wc_ecc_make_key(&rng, keySz, &key);
        }
    }
    if (ret == 0) {
        ret = wc_ecc_sign_hash_ex(hash, hashlen, &rng, &key, &r, &s);
        if (ret == 0) {
            /* verify_ok should be 1. */
            ret = wc_ecc_verify_hash_ex(&r, &s, hash, hashlen, &verify_ok, &key);
            if (verify_ok != 1 && ret == 0) {
                ret = WOLFSSL_FATAL_ERROR;
            }
        }
        if (ret == 0) {
            /* verify_ok should be 0 */
            ret = wc_ecc_verify_hash_ex(&r, &s, iHash, iHashLen,
                                                    &verify_ok, &key);
            if (verify_ok != 0 && ret == 0) {
                ret = WOLFSSL_FATAL_ERROR;
            }
        }
        if (ret == 0) {
            /* verify_ok should be 0. */
            ret = wc_ecc_verify_hash_ex(&r, &s, shortHash, shortHashLen,
                                                            &verify_ok, &key);
            if (verify_ok != 0 && ret == 0) {
                ret = WOLFSSL_FATAL_ERROR;
            }
        }
    }

    printf(testingFmt, "wc_ecc_sign_hash_ex()");
    /* Test bad args. */
    if (ret == 0) {
        if (wc_ecc_sign_hash_ex(NULL, hashlen, &rng, &key, &r, &s)
                                                == ECC_BAD_ARG_E) {
            sig = 0;
        }
        if (sig == 0 && wc_ecc_sign_hash_ex(hash, hashlen, NULL, &key, &r, &s)
                                                            != ECC_BAD_ARG_E) {
            sig = WOLFSSL_FATAL_ERROR;
        }
        if (sig == 0 && wc_ecc_sign_hash_ex(hash, hashlen, &rng, NULL, &r, &s)
                                                            != ECC_BAD_ARG_E) {
            sig = WOLFSSL_FATAL_ERROR;
        }
        if (sig == 0 && wc_ecc_sign_hash_ex(hash, hashlen, &rng, &key, NULL, &s)
                                                            != ECC_BAD_ARG_E) {
            sig = WOLFSSL_FATAL_ERROR;
        }
        if (sig == 0 && wc_ecc_sign_hash_ex(hash, hashlen, &rng, &key, &r, NULL)
                                                            != ECC_BAD_ARG_E) {
            sig = WOLFSSL_FATAL_ERROR;
        }
    }
    printf(resultFmt, sig == 0 ? passed : failed);
    printf(testingFmt, "wc_ecc_verify_hash_ex()");
    /* Test bad args. */
    if (ret == 0) {
        if (wc_ecc_verify_hash_ex(NULL, &s, shortHash, shortHashLen, &verify_ok, &key)
                                                            == ECC_BAD_ARG_E) {
            ver = 0;
        }
        if (ver == 0 && wc_ecc_verify_hash_ex(&r, NULL, shortHash, shortHashLen,
                                                &verify_ok, &key) != ECC_BAD_ARG_E) {
            ver = WOLFSSL_FATAL_ERROR;
        }
        if (ver == 0 && wc_ecc_verify_hash_ex(&r, &s, NULL, shortHashLen, &verify_ok,
                                                       &key) != ECC_BAD_ARG_E) {
            ver = WOLFSSL_FATAL_ERROR;
        }
        if (ver == 0 && wc_ecc_verify_hash_ex(&r, &s, shortHash, shortHashLen,
                                                NULL, &key) != ECC_BAD_ARG_E) {
            ver = WOLFSSL_FATAL_ERROR;
        }
        if (ver == 0 && wc_ecc_verify_hash_ex(&r, &s, shortHash, shortHashLen,
                                                &verify_ok, NULL) != ECC_BAD_ARG_E) {
            ver = WOLFSSL_FATAL_ERROR;
        }
    }
    printf(resultFmt, ver == 0 ? passed : failed);

    wc_ecc_free(&key);
    mp_free(&r);
    mp_free(&s);
    if (wc_FreeRng(&rng)) {
        return WOLFSSL_FATAL_ERROR;
    }
    if (ret == 0 && (sig != 0 || ver != 0)) {
        ret = WOLFSSL_FATAL_ERROR;
    }
#endif
    return ret;

} /* END test_wc_ecc_verify_hash_ex */

/*
 * Testing wc_ecc_mulmod()
 */

static int test_wc_ecc_mulmod (void)
{
    int         ret = 0;

#if defined(HAVE_ECC) && !defined(WC_NO_RNG) && \
    !(defined(WOLFSSL_ATECC508A) || defined(WOLFSSL_VALIDATE_ECC_IMPORT))
    ecc_key     key1, key2, key3;
    WC_RNG      rng;

    ret = wc_InitRng(&rng);
    if (ret == 0) {
        if (ret == 0) {
            ret = wc_ecc_init(&key1);
        }
        if (ret == 0) {
            ret = wc_ecc_init(&key2);
        }
        if (ret == 0) {
            ret = wc_ecc_init(&key3);
        }
        if (ret == 0) {
            ret = wc_ecc_make_key(&rng, KEY32, &key1);
        }
    }
    if (ret == 0) {
        ret = wc_ecc_import_raw_ex(&key2, key1.dp->Gx, key1.dp->Gy, key1.dp->Af,
                                                                 ECC_SECP256R1);
        if (ret == 0) {
            ret = wc_ecc_import_raw_ex(&key3, key1.dp->Gx, key1.dp->Gy,
                                        key1.dp->prime, ECC_SECP256R1);
        }
    }

    printf(testingFmt, "wc_ecc_mulmod()");
    if (ret == 0) {
        ret = wc_ecc_mulmod(&key1.k, &key2.pubkey, &key3.pubkey, &key2.k,
                                                            &key3.k, 1);
    }

    /* Test bad args. */
    if (ret == 0) {
        ret = wc_ecc_mulmod(NULL, &key2.pubkey, &key3.pubkey, &key2.k,
                                                            &key3.k, 1);
        if (ret == ECC_BAD_ARG_E) {
            ret = wc_ecc_mulmod(&key1.k, NULL, &key3.pubkey, &key2.k,
                                                            &key3.k, 1);
        }
        if (ret == ECC_BAD_ARG_E) {
            ret = wc_ecc_mulmod(&key1.k, &key2.pubkey, NULL, &key2.k,
                                                            &key3.k, 1);
        }
        if (ret == ECC_BAD_ARG_E) {
            ret = wc_ecc_mulmod(&key1.k, &key2.pubkey, &key3.pubkey,
                                                            &key2.k, NULL, 1);
        }
        if (ret == ECC_BAD_ARG_E) {
            ret = 0;
        } else if (ret == 0) {
            ret = WOLFSSL_FATAL_ERROR;
        }
    }

    printf(resultFmt, ret == 0 ? passed : failed);

    if (wc_FreeRng(&rng) && ret == 0) {
        ret = WOLFSSL_FATAL_ERROR;
    }
    wc_ecc_free(&key1);
    wc_ecc_free(&key2);
    wc_ecc_free(&key3);

#ifdef FP_ECC
    wc_ecc_fp_free();
#endif

#endif /* HAVE_ECC && !WOLFSSL_ATECC508A */
    return ret;


} /* END test_wc_ecc_mulmod */

/*
 * Testing wc_ecc_is_valid_idx()
 */
static int test_wc_ecc_is_valid_idx (void)
{
    int         ret = 0;

#if defined(HAVE_ECC) && !defined(WC_NO_RNG)
    ecc_key     key;
    WC_RNG      rng;
    int         iVal = -2;
    int         iVal2 = 3000;


    ret = wc_InitRng(&rng);
    if (ret == 0) {
        ret = wc_ecc_init(&key);
        if (ret == 0) {
            ret = wc_ecc_make_key(&rng, 32, &key);
        }
    }

    printf(testingFmt, "wc_ecc_is_valid_idx()");
    if (ret == 0) {
        ret = wc_ecc_is_valid_idx(key.idx);
        if (ret == 1) {
            ret = 0;
        } else {
            ret = WOLFSSL_FATAL_ERROR;
        }
    }
    /* Test bad args. */
    if (ret == 0) {
        ret = wc_ecc_is_valid_idx(iVal); /* should return 0 */
        if (ret == 0) {
            ret = wc_ecc_is_valid_idx(iVal2);
        }
        if (ret != 0) {
            ret = WOLFSSL_FATAL_ERROR;
        }
    }

    printf(resultFmt, ret == 0 ? passed : failed);

    if (wc_FreeRng(&rng) && ret == 0) {
        ret = WOLFSSL_FATAL_ERROR;
    }
    wc_ecc_free(&key);
#ifdef FP_ECC
    wc_ecc_fp_free();
#endif

#endif
    return ret;


} /* END test_wc_ecc_is_valid_idx */


/*
 * Testing wc_PKCS7_New()
 */
static void test_wc_PKCS7_New (void)
{
#if defined(HAVE_PKCS7)
    PKCS7*      pkcs7;
    void*       heap = NULL;

    printf(testingFmt, "wc_PKCS7_New()");

    pkcs7 = wc_PKCS7_New(heap, devId);
    AssertNotNull(pkcs7);

    printf(resultFmt, passed);
    wc_PKCS7_Free(pkcs7);
#endif
} /* END test-wc_PKCS7_New */

/*
 * Testing wc_PKCS7_Init()
 */
static void test_wc_PKCS7_Init (void)
{
#if defined(HAVE_PKCS7)
    PKCS7*      pkcs7;
    void*       heap = NULL;

    printf(testingFmt, "wc_PKCS7_Init()");

    pkcs7 = wc_PKCS7_New(heap, devId);
    AssertNotNull(pkcs7);

    AssertIntEQ(wc_PKCS7_Init(pkcs7, heap, devId), 0);

    /* Pass in bad args. */
    AssertIntEQ(wc_PKCS7_Init(NULL, heap, devId), BAD_FUNC_ARG);

    printf(resultFmt, passed);
    wc_PKCS7_Free(pkcs7);
#endif
} /* END test-wc_PKCS7_Init */


/*
 * Testing wc_PKCS7_InitWithCert()
 */
static void test_wc_PKCS7_InitWithCert (void)
{
#if defined(HAVE_PKCS7)
    PKCS7*       pkcs7;

#ifndef NO_RSA
    #if defined(USE_CERT_BUFFERS_2048)
        unsigned char    cert[sizeof_client_cert_der_2048];
        int              certSz = (int)sizeof(cert);
        XMEMSET(cert, 0, certSz);
        XMEMCPY(cert, client_cert_der_2048, sizeof_client_cert_der_2048);
    #elif defined(USE_CERT_BUFFERS_1024)
        unsigned char    cert[sizeof_client_cert_der_1024];
        int              certSz = (int)sizeof(cert);
        XMEMSET(cert, 0, certSz);
        XMEMCPY(cert, client_cert_der_1024, sizeof_client_cert_der_1024);
    #else
        unsigned char   cert[ONEK_BUF];
        XFILE           fp;
        int             certSz;
        fp = XFOPEN("./certs/1024/client-cert.der", "rb");
        AssertTrue(fp != XBADFILE);

        certSz = (int)XFREAD(cert, 1, sizeof_client_cert_der_1024, fp);
        XFCLOSE(fp);
    #endif
#elif defined(HAVE_ECC)
    #if defined(USE_CERT_BUFFERS_256)
        unsigned char    cert[sizeof_cliecc_cert_der_256];
        int              certSz = (int)sizeof(cert);
        XMEMSET(cert, 0, certSz);
        XMEMCPY(cert, cliecc_cert_der_256, sizeof_cliecc_cert_der_256);
    #else
        unsigned char   cert[ONEK_BUF];
        XFILE           fp;
        int             certSz;
        fp = XFOPEN("./certs/client-ecc-cert.der", "rb");

        AssertTrue(fp != XBADFILE);

        certSz = (int)XFREAD(cert, 1, sizeof_cliecc_cert_der_256, fp);
        XFCLOSE(fp);
    #endif
#else
        #error PKCS7 requires ECC or RSA
#endif
    printf(testingFmt, "wc_PKCS7_InitWithCert()");

    AssertNotNull(pkcs7 = wc_PKCS7_New(HEAP_HINT, devId));
    /* If initialization is not successful, it's free'd in init func. */
    AssertIntEQ(wc_PKCS7_InitWithCert(pkcs7, (byte*)cert, (word32)certSz), 0);

    wc_PKCS7_Free(pkcs7);
    AssertNotNull(pkcs7 = wc_PKCS7_New(HEAP_HINT, devId));

    /* Valid initialization usage. */
    AssertIntEQ(wc_PKCS7_InitWithCert(pkcs7, NULL, 0), 0);

    /* Pass in bad args. No need free for null checks, free at end.*/
    AssertIntEQ(wc_PKCS7_InitWithCert(NULL, (byte*)cert, (word32)certSz),
                                                           BAD_FUNC_ARG);
    AssertIntEQ(wc_PKCS7_InitWithCert(pkcs7, NULL, (word32)certSz),
                                                      BAD_FUNC_ARG);

    printf(resultFmt, passed);

    wc_PKCS7_Free(pkcs7);
#endif
} /* END test_wc_PKCS7_InitWithCert */


/*
 * Testing wc_PKCS7_EncodeData()
 */
static void test_wc_PKCS7_EncodeData (void)
{
#if defined(HAVE_PKCS7)
    PKCS7*      pkcs7;
    byte        output[FOURK_BUF];
    byte        data[] = "My encoded DER cert.";

#ifndef NO_RSA
    #if defined(USE_CERT_BUFFERS_2048)
        unsigned char cert[sizeof_client_cert_der_2048];
        unsigned char key[sizeof_client_key_der_2048];
        int certSz = (int)sizeof(cert);
        int keySz = (int)sizeof(key);
        XMEMSET(cert, 0, certSz);
        XMEMSET(key, 0, keySz);
        XMEMCPY(cert, client_cert_der_2048, certSz);
        XMEMCPY(key, client_key_der_2048, keySz);

    #elif defined(USE_CERT_BUFFERS_1024)
        unsigned char cert[sizeof_client_cert_der_1024];
        unsigned char key[sizeof_client_key_der_1024];
        int certSz = (int)sizeof(cert);
        int keySz = (int)sizeof(key);
        XMEMSET(cert, 0, certSz);
        XMEMSET(key, 0, keySz);
        XMEMCPY(cert, client_cert_der_1024, certSz);
        XMEMCPY(key, client_key_der_1024, keySz);
    #else
        unsigned char   cert[ONEK_BUF];
        unsigned char   key[ONEK_BUF];
        XFILE           fp;
        int             certSz;
        int             keySz;

        fp = XFOPEN("./certs/1024/client-cert.der", "rb");
        AssertTrue(fp != XBADFILE);
        certSz = (int)XFREAD(cert, 1, sizeof_client_cert_der_1024, fp);
        XFCLOSE(fp);

        fp = XFOPEN("./certs/1024/client-key.der", "rb");
        AssertTrue(fp != XBADFILE);
        keySz = (int)XFREAD(key, 1, sizeof_client_key_der_1024, fp);
        XFCLOSE(fp);
    #endif
#elif defined(HAVE_ECC)
    #if defined(USE_CERT_BUFFERS_256)
        unsigned char    cert[sizeof_cliecc_cert_der_256];
        unsigned char    key[sizeof_ecc_clikey_der_256];
        int              certSz = (int)sizeof(cert);
        int              keySz = (int)sizeof(key);
        XMEMSET(cert, 0, certSz);
        XMEMSET(key, 0, keySz);
        XMEMCPY(cert, cliecc_cert_der_256, sizeof_cliecc_cert_der_256);
        XMEMCPY(key, ecc_clikey_der_256, sizeof_ecc_clikey_der_256);
    #else
        unsigned char   cert[ONEK_BUF];
        unsigned char   key[ONEK_BUF];
        XFILE           fp;
        int             certSz, keySz;

        fp = XFOPEN("./certs/client-ecc-cert.der", "rb");
        AssertTrue(fp != XBADFILE);
        certSz = (int)XFREAD(cert, 1, sizeof_cliecc_cert_der_256, fp);
        XFCLOSE(fp);

        fp = XFOPEN("./certs/client-ecc-key.der", "rb");
        AssertTrue(fp != XBADFILE);
        keySz = (int)XFREAD(key, 1, sizeof_ecc_clikey_der_256, fp);
        XFCLOSE(fp);
    #endif
#endif

    XMEMSET(output, 0, sizeof(output));

    AssertNotNull(pkcs7 = wc_PKCS7_New(HEAP_HINT, devId));
    AssertIntEQ(wc_PKCS7_Init(pkcs7, HEAP_HINT, INVALID_DEVID), 0);

    AssertIntEQ(wc_PKCS7_InitWithCert(pkcs7, (byte*)cert, certSz), 0);

    printf(testingFmt, "wc_PKCS7_EncodeData()");

    pkcs7->content = data;
    pkcs7->contentSz = sizeof(data);
    pkcs7->privateKey = key;
    pkcs7->privateKeySz = keySz;
    AssertIntGT(wc_PKCS7_EncodeData(pkcs7, output, (word32)sizeof(output)), 0);

    /* Test bad args. */
    AssertIntEQ(wc_PKCS7_EncodeData(NULL, output, (word32)sizeof(output)),
                                                            BAD_FUNC_ARG);
    AssertIntEQ(wc_PKCS7_EncodeData(pkcs7, NULL, (word32)sizeof(output)),
                                                            BAD_FUNC_ARG);
    AssertIntEQ(wc_PKCS7_EncodeData(pkcs7, output, 5), BUFFER_E);

    printf(resultFmt, passed);

    wc_PKCS7_Free(pkcs7);
#endif
}  /* END test_wc_PKCS7_EncodeData */


/*
 * Testing wc_PKCS7_EncodeSignedData()
 */
static void test_wc_PKCS7_EncodeSignedData(void)
{
#if defined(HAVE_PKCS7)
    PKCS7*      pkcs7;
    WC_RNG      rng;
    byte        output[FOURK_BUF];
    byte        badOut[0];
    word32      outputSz = (word32)sizeof(output);
    word32      badOutSz = (word32)sizeof(badOut);
    byte        data[] = "Test data to encode.";

#ifndef NO_RSA
    #if defined(USE_CERT_BUFFERS_2048)
        byte        key[sizeof_client_key_der_2048];
        byte        cert[sizeof_client_cert_der_2048];
        word32      keySz = (word32)sizeof(key);
        word32      certSz = (word32)sizeof(cert);
        XMEMSET(key, 0, keySz);
        XMEMSET(cert, 0, certSz);
        XMEMCPY(key, client_key_der_2048, keySz);
        XMEMCPY(cert, client_cert_der_2048, certSz);
    #elif defined(USE_CERT_BUFFERS_1024)
        byte        key[sizeof_client_key_der_1024];
        byte        cert[sizeof_client_cert_der_1024];
        word32      keySz = (word32)sizeof(key);
        word32      certSz = (word32)sizeof(cert);
        XMEMSET(key, 0, keySz);
        XMEMSET(cert, 0, certSz);
        XMEMCPY(key, client_key_der_1024, keySz);
        XMEMCPY(cert, client_cert_der_1024, certSz);
    #else
        unsigned char   cert[ONEK_BUF];
        unsigned char   key[ONEK_BUF];
        XFILE           fp;
        int             certSz;
        int             keySz;

        fp = XFOPEN("./certs/1024/client-cert.der", "rb");
        AssertTrue(fp != XBADFILE);
        certSz = (int)XFREAD(cert, 1, sizeof_client_cert_der_1024, fp);
        XFCLOSE(fp);

        fp = XFOPEN("./certs/1024/client-key.der", "rb");
        AssertTrue(fp != XBADFILE);
        keySz = (int)XFREAD(key, 1, sizeof_client_key_der_1024, fp);
        XFCLOSE(fp);
    #endif
#elif defined(HAVE_ECC)
    #if defined(USE_CERT_BUFFERS_256)
        unsigned char    cert[sizeof_cliecc_cert_der_256];
        unsigned char    key[sizeof_ecc_clikey_der_256];
        int              certSz = (int)sizeof(cert);
        int              keySz = (int)sizeof(key);
        XMEMSET(cert, 0, certSz);
        XMEMSET(key, 0, keySz);
        XMEMCPY(cert, cliecc_cert_der_256, sizeof_cliecc_cert_der_256);
        XMEMCPY(key, ecc_clikey_der_256, sizeof_ecc_clikey_der_256);
    #else
        unsigned char   cert[ONEK_BUF];
        unsigned char   key[ONEK_BUF];
        XFILE           fp;
        int             certSz, keySz;

        fp = XOPEN("./certs/client-ecc-cert.der", "rb");
        AssertTrue(fp != XBADFILE);
        certSz = (int)XFREAD(cert, 1, sizeof_cliecc_cert_der_256, fp);
        XFCLOSE(fp);

        fp = XFOPEN("./certs/client-ecc-key.der", "rb");
        AssertTrue(fp != XBADFILE);
        keySz = (int)XFREAD(key, 1, sizeof_ecc_clikey_der_256, fp);
        XFCLOSE(fp);
    #endif
#endif

    XMEMSET(output, 0, outputSz);
    AssertIntEQ(wc_InitRng(&rng), 0);

    AssertNotNull(pkcs7 = wc_PKCS7_New(HEAP_HINT, devId));
    AssertIntEQ(wc_PKCS7_Init(pkcs7, HEAP_HINT, INVALID_DEVID), 0);

    AssertIntEQ(wc_PKCS7_InitWithCert(pkcs7, cert, certSz), 0);

    printf(testingFmt, "wc_PKCS7_EncodeSignedData()");

    pkcs7->content = data;
    pkcs7->contentSz = (word32)sizeof(data);
    pkcs7->privateKey = key;
    pkcs7->privateKeySz = (word32)sizeof(key);
    pkcs7->encryptOID = RSAk;
    pkcs7->hashOID = SHAh;
    pkcs7->rng = &rng;

    AssertIntGT(wc_PKCS7_EncodeSignedData(pkcs7, output, outputSz), 0);

    wc_PKCS7_Free(pkcs7);
    AssertNotNull(pkcs7 = wc_PKCS7_New(HEAP_HINT, devId));
    AssertIntEQ(wc_PKCS7_InitWithCert(pkcs7, NULL, 0), 0);
    AssertIntEQ(wc_PKCS7_VerifySignedData(pkcs7, output, outputSz), 0);

    /* Pass in bad args. */
    AssertIntEQ(wc_PKCS7_EncodeSignedData(NULL, output, outputSz), BAD_FUNC_ARG);
    AssertIntEQ(wc_PKCS7_EncodeSignedData(pkcs7, NULL, outputSz), BAD_FUNC_ARG);
    AssertIntEQ(wc_PKCS7_EncodeSignedData(pkcs7, badOut,
                                badOutSz), BAD_FUNC_ARG);
    pkcs7->hashOID = 0; /* bad hashOID */
    AssertIntEQ(wc_PKCS7_EncodeSignedData(pkcs7, output, outputSz), BAD_FUNC_ARG);

    printf(resultFmt, passed);

    wc_PKCS7_Free(pkcs7);
    wc_FreeRng(&rng);

#endif
} /* END test_wc_PKCS7_EncodeSignedData */

/*
 * Testing wc_PKCS7_EncodeSignedData_ex() and wc_PKCS7_VerifySignedData_ex()
 */
static void test_wc_PKCS7_EncodeSignedData_ex(void)
{
#if defined(HAVE_PKCS7)
    int         ret, i;
    PKCS7*      pkcs7;
    WC_RNG      rng;
    byte        outputHead[FOURK_BUF/2];
    byte        outputFoot[FOURK_BUF/2];
    word32      outputHeadSz = (word32)sizeof(outputHead);
    word32      outputFootSz = (word32)sizeof(outputFoot);
    byte        data[FOURK_BUF];
    wc_HashAlg  hash;
    enum wc_HashType hashType = WC_HASH_TYPE_SHA;
    byte        hashBuf[WC_MAX_DIGEST_SIZE];
    word32      hashSz = wc_HashGetDigestSize(hashType);

#ifndef NO_RSA
    #if defined(USE_CERT_BUFFERS_2048)
        byte        key[sizeof_client_key_der_2048];
        byte        cert[sizeof_client_cert_der_2048];
        word32      keySz = (word32)sizeof(key);
        word32      certSz = (word32)sizeof(cert);
        XMEMSET(key, 0, keySz);
        XMEMSET(cert, 0, certSz);
        XMEMCPY(key, client_key_der_2048, keySz);
        XMEMCPY(cert, client_cert_der_2048, certSz);
    #elif defined(USE_CERT_BUFFERS_1024)
        byte        key[sizeof_client_key_der_1024];
        byte        cert[sizeof_client_cert_der_1024];
        word32      keySz = (word32)sizeof(key);
        word32      certSz = (word32)sizeof(cert);
        XMEMSET(key, 0, keySz);
        XMEMSET(cert, 0, certSz);
        XMEMCPY(key, client_key_der_1024, keySz);
        XMEMCPY(cert, client_cert_der_1024, certSz);
    #else
        unsigned char   cert[ONEK_BUF];
        unsigned char   key[ONEK_BUF];
        XFILE           fp;
        int             certSz;
        int             keySz;

        fp = XFOPEN("./certs/1024/client-cert.der", "rb");
        AssertTrue((fp != XBADFILE));
        certSz = (int)XFREAD(cert, 1, sizeof_client_cert_der_1024, fp);
        XFCLOSE(fp);

        fp = XFOPEN("./certs/1024/client-key.der", "rb");
        AssertTrue(fp != XBADFILE);
        keySz = (int)XFREAD(key, 1, sizeof_client_key_der_1024, fp);
        XFCLOSE(fp);
    #endif
#elif defined(HAVE_ECC)
    #if defined(USE_CERT_BUFFERS_256)
        unsigned char    cert[sizeof_cliecc_cert_der_256];
        unsigned char    key[sizeof_ecc_clikey_der_256];
        int              certSz = (int)sizeof(cert);
        int              keySz = (int)sizeof(key);
        XMEMSET(cert, 0, certSz);
        XMEMSET(key, 0, keySz);
        XMEMCPY(cert, cliecc_cert_der_256, sizeof_cliecc_cert_der_256);
        XMEMCPY(key, ecc_clikey_der_256, sizeof_ecc_clikey_der_256);
    #else
        unsigned char   cert[ONEK_BUF];
        unsigned char   key[ONEK_BUF];
        XFILE           fp;
        int             certSz, keySz;

        fp = XFOPEN("./certs/client-ecc-cert.der", "rb");
        AssertTrue(fp != XBADFILE);
        certSz = (int)XFREAD(cert, 1, sizeof_cliecc_cert_der_256, fp);
        XFCLOSE(fp);

        fp = XFOPEN("./certs/client-ecc-key.der", "rb");
        AssertTrue(fp != XBADFILE);
        keySz = (int)XFREAD(key, 1, sizeof_ecc_clikey_der_256, fp);
        XFCLOSE(fp);
    #endif
#endif

    /* initialize large data with sequence */
    for (i=0; i<(int)sizeof(data); i++)
        data[i] = i & 0xff;

    XMEMSET(outputHead, 0, outputHeadSz);
    XMEMSET(outputFoot, 0, outputFootSz);
    AssertIntEQ(wc_InitRng(&rng), 0);

    AssertNotNull(pkcs7 = wc_PKCS7_New(HEAP_HINT, devId));
    AssertIntEQ(wc_PKCS7_Init(pkcs7, HEAP_HINT, INVALID_DEVID), 0);

    AssertIntEQ(wc_PKCS7_InitWithCert(pkcs7, cert, certSz), 0);

    printf(testingFmt, "wc_PKCS7_EncodeSignedData()");

    pkcs7->content = NULL; /* not used for ex */
    pkcs7->contentSz = (word32)sizeof(data);
    pkcs7->privateKey = key;
    pkcs7->privateKeySz = (word32)sizeof(key);
    pkcs7->encryptOID = RSAk;
    pkcs7->hashOID = SHAh;
    pkcs7->rng = &rng;

    /* calculate hash for content */
    ret = wc_HashInit(&hash, hashType);
    if (ret == 0) {
        ret = wc_HashUpdate(&hash, hashType, data, sizeof(data));
        if (ret == 0) {
            ret = wc_HashFinal(&hash, hashType, hashBuf);
        }
        wc_HashFree(&hash, hashType);
    }
    AssertIntEQ(ret, 0);

    /* Perform PKCS7 sign using hash directly */
    AssertIntEQ(wc_PKCS7_EncodeSignedData_ex(pkcs7, hashBuf, hashSz,
        outputHead, &outputHeadSz, outputFoot, &outputFootSz), 0);
    AssertIntGT(outputHeadSz, 0);
    AssertIntGT(outputFootSz, 0);

    wc_PKCS7_Free(pkcs7);
    AssertNotNull(pkcs7 = wc_PKCS7_New(HEAP_HINT, devId));
    AssertIntEQ(wc_PKCS7_InitWithCert(pkcs7, NULL, 0), 0);

    /* required parameter even on verify when using _ex */
    pkcs7->contentSz = (word32)sizeof(data);
    AssertIntEQ(wc_PKCS7_VerifySignedData_ex(pkcs7, hashBuf, hashSz,
        outputHead, outputHeadSz, outputFoot, outputFootSz), 0);

    wc_PKCS7_Free(pkcs7);

    /* assembly complete PKCS7 sign and use normal verify */
    {
        byte* output = (byte*)XMALLOC(outputHeadSz + sizeof(data) + outputFootSz, HEAP_HINT, DYNAMIC_TYPE_TMP_BUFFER);
        word32 outputSz = 0;
        AssertNotNull(output);
        XMEMCPY(&output[outputSz], outputHead, outputHeadSz);
        outputSz += outputHeadSz;
        XMEMCPY(&output[outputSz], data, sizeof(data));
        outputSz += sizeof(data);
        XMEMCPY(&output[outputSz], outputFoot, outputFootSz);
        outputSz += outputFootSz;

        AssertNotNull(pkcs7 = wc_PKCS7_New(HEAP_HINT, devId));
        AssertIntEQ(wc_PKCS7_InitWithCert(pkcs7, NULL, 0), 0);
        AssertIntEQ(wc_PKCS7_VerifySignedData(pkcs7, output, outputSz), 0);
        XFREE(output, HEAP_HINT, DYNAMIC_TYPE_TMP_BUFFER);
    }

    /* Pass in bad args. */
    AssertIntEQ(wc_PKCS7_EncodeSignedData_ex(NULL, hashBuf, hashSz, outputHead,
        &outputHeadSz, outputFoot, &outputFootSz), BAD_FUNC_ARG);
    AssertIntEQ(wc_PKCS7_EncodeSignedData_ex(pkcs7, NULL, hashSz, outputHead,
        &outputHeadSz, outputFoot, &outputFootSz), BAD_FUNC_ARG);
    AssertIntEQ(wc_PKCS7_EncodeSignedData_ex(pkcs7, hashBuf, 0, outputHead,
        &outputHeadSz, outputFoot, &outputFootSz), BAD_FUNC_ARG);
    AssertIntEQ(wc_PKCS7_EncodeSignedData_ex(pkcs7, hashBuf, hashSz, NULL,
        &outputHeadSz, outputFoot, &outputFootSz), BAD_FUNC_ARG);
    AssertIntEQ(wc_PKCS7_EncodeSignedData_ex(pkcs7, hashBuf, hashSz,
        outputHead, NULL, outputFoot, &outputFootSz), BAD_FUNC_ARG);
    AssertIntEQ(wc_PKCS7_EncodeSignedData_ex(pkcs7, hashBuf, hashSz,
        outputHead, &outputHeadSz, NULL, &outputFootSz), BAD_FUNC_ARG);
    AssertIntEQ(wc_PKCS7_EncodeSignedData_ex(pkcs7, hashBuf, hashSz,
        outputHead, &outputHeadSz, outputFoot, NULL), BAD_FUNC_ARG);
    pkcs7->hashOID = 0; /* bad hashOID */
    AssertIntEQ(wc_PKCS7_EncodeSignedData_ex(pkcs7, hashBuf, hashSz,
        outputHead, &outputHeadSz, outputFoot, &outputFootSz), BAD_FUNC_ARG);

    AssertIntEQ(wc_PKCS7_VerifySignedData_ex(NULL, hashBuf, hashSz, outputHead,
        outputHeadSz, outputFoot, outputFootSz), BAD_FUNC_ARG);

    AssertIntEQ(wc_PKCS7_VerifySignedData_ex(pkcs7, NULL, hashSz, outputHead,
        outputHeadSz, outputFoot, outputFootSz), BAD_FUNC_ARG);
#ifndef NO_PKCS7_STREAM
    AssertIntEQ(wc_PKCS7_VerifySignedData_ex(pkcs7, hashBuf, 0, outputHead,
        outputHeadSz, outputFoot, outputFootSz), WC_PKCS7_WANT_READ_E);
#else
    AssertIntEQ(wc_PKCS7_VerifySignedData_ex(pkcs7, hashBuf, 0, outputHead,
        outputHeadSz, outputFoot, outputFootSz), BUFFER_E);
#endif
    AssertIntEQ(wc_PKCS7_VerifySignedData_ex(pkcs7, hashBuf, hashSz, NULL,
        outputHeadSz, outputFoot, outputFootSz), BAD_FUNC_ARG);
#ifndef NO_PKCS7_STREAM
    /* can pass in 0 buffer length with streaming API */
    AssertIntEQ(wc_PKCS7_VerifySignedData_ex(pkcs7, hashBuf, hashSz,
        outputHead, 0, outputFoot, outputFootSz), WC_PKCS7_WANT_READ_E);
#else
    AssertIntEQ(wc_PKCS7_VerifySignedData_ex(pkcs7, hashBuf, hashSz,
        outputHead, 0, outputFoot, outputFootSz), BAD_FUNC_ARG);
#endif
    AssertIntEQ(wc_PKCS7_VerifySignedData_ex(pkcs7, hashBuf, hashSz,
        outputHead, outputHeadSz, NULL, outputFootSz), BAD_FUNC_ARG);
#ifndef NO_PKCS7_STREAM
    AssertIntEQ(wc_PKCS7_VerifySignedData_ex(pkcs7, hashBuf, hashSz,
        outputHead, outputHeadSz, outputFoot, 0), WC_PKCS7_WANT_READ_E);
#else
    AssertIntEQ(wc_PKCS7_VerifySignedData_ex(pkcs7, hashBuf, hashSz,
        outputHead, outputHeadSz, outputFoot, 0), ASN_PARSE_E);
#endif

    printf(resultFmt, passed);

    wc_PKCS7_Free(pkcs7);
    wc_FreeRng(&rng);

#endif
} /* END test_wc_PKCS7_EncodeSignedData_ex */


#if defined(HAVE_PKCS7)
static int CreatePKCS7SignedData(unsigned char* output, int outputSz,
                                 byte* data, word32 dataSz,
                                 int withAttribs, int detachedSig)
{
    PKCS7*      pkcs7;
    WC_RNG      rng;

    static byte messageTypeOid[] =
               { 0x06, 0x0a, 0x60, 0x86, 0x48, 0x01, 0x86, 0xF8, 0x45, 0x01,
                 0x09, 0x02 };
    static byte messageType[] = { 0x13, 2, '1', '9' };

    PKCS7Attrib attribs[] =
    {
        { messageTypeOid, sizeof(messageTypeOid), messageType,
                                       sizeof(messageType) }
    };

#ifndef NO_RSA
    #if defined(USE_CERT_BUFFERS_2048)
        byte        key[sizeof_client_key_der_2048];
        byte        cert[sizeof_client_cert_der_2048];
        word32      keySz = (word32)sizeof(key);
        word32      certSz = (word32)sizeof(cert);
        XMEMSET(key, 0, keySz);
        XMEMSET(cert, 0, certSz);
        XMEMCPY(key, client_key_der_2048, keySz);
        XMEMCPY(cert, client_cert_der_2048, certSz);
    #elif defined(USE_CERT_BUFFERS_1024)
        byte        key[sizeof_client_key_der_1024];
        byte        cert[sizeof_client_cert_der_1024];
        word32      keySz = (word32)sizeof(key);
        word32      certSz = (word32)sizeof(cert);
        XMEMSET(key, 0, keySz);
        XMEMSET(cert, 0, certSz);
        XMEMCPY(key, client_key_der_1024, keySz);
        XMEMCPY(cert, client_cert_der_1024, certSz);
    #else
        unsigned char   cert[ONEK_BUF];
        unsigned char   key[ONEK_BUF];
        FILE*           fp;
        int             certSz;
        int             keySz;

        fp = fopen("./certs/1024/client-cert.der", "rb");
        AssertNotNull(fp);
        certSz = fread(cert, 1, sizeof_client_cert_der_1024, fp);
        fclose(fp);

        fp = fopen("./certs/1024/client-key.der", "rb");
        AssertNotNull(fp);
        keySz = fread(key, 1, sizeof_client_key_der_1024, fp);
        fclose(fp);
    #endif
#elif defined(HAVE_ECC)
    #if defined(USE_CERT_BUFFERS_256)
        unsigned char    cert[sizeof_cliecc_cert_der_256];
        unsigned char    key[sizeof_ecc_clikey_der_256];
        int              certSz = (int)sizeof(cert);
        int              keySz = (int)sizeof(key);
        XMEMSET(cert, 0, certSz);
        XMEMSET(key, 0, keySz);
        XMEMCPY(cert, cliecc_cert_der_256, sizeof_cliecc_cert_der_256);
        XMEMCPY(key, ecc_clikey_der_256, sizeof_ecc_clikey_der_256);
    #else
        unsigned char   cert[ONEK_BUF];
        unsigned char   key[ONEK_BUF];
        FILE*           fp;
        int             certSz, keySz;

        fp = fopen("./certs/client-ecc-cert.der", "rb");
        AssertNotNull(fp);
        certSz = fread(cert, 1, sizeof_cliecc_cert_der_256, fp);
        fclose(fp);

        fp = fopen("./certs/client-ecc-key.der", "rb");
        AssertNotNull(fp);
        keySz = fread(key, 1, sizeof_ecc_clikey_der_256, fp);
        fclose(fp);
    #endif
#endif

    XMEMSET(output, 0, outputSz);
    AssertIntEQ(wc_InitRng(&rng), 0);

    AssertNotNull(pkcs7 = wc_PKCS7_New(HEAP_HINT, devId));
    AssertIntEQ(wc_PKCS7_Init(pkcs7, HEAP_HINT, INVALID_DEVID), 0);

    AssertIntEQ(wc_PKCS7_InitWithCert(pkcs7, cert, certSz), 0);

    printf(testingFmt, "wc_PKCS7_VerifySignedData()");

    pkcs7->content = data;
    pkcs7->contentSz = dataSz;
    pkcs7->privateKey = key;
    pkcs7->privateKeySz = (word32)sizeof(key);
    pkcs7->encryptOID = RSAk;
    pkcs7->hashOID = SHAh;
    pkcs7->rng = &rng;
    if (withAttribs) {
        /* include a signed attribute */
        pkcs7->signedAttribs   = attribs;
        pkcs7->signedAttribsSz = (sizeof(attribs)/sizeof(PKCS7Attrib));
    }

    if (detachedSig) {
        AssertIntEQ(wc_PKCS7_SetDetached(pkcs7, 1), 0);
    }

    AssertIntGT(wc_PKCS7_EncodeSignedData(pkcs7, output, outputSz), 0);
    wc_PKCS7_Free(pkcs7);
    AssertNotNull(pkcs7 = wc_PKCS7_New(HEAP_HINT, devId));
    AssertIntEQ(wc_PKCS7_InitWithCert(pkcs7, NULL, 0), 0);
    if (detachedSig) {
        pkcs7->content = data;
        pkcs7->contentSz = dataSz;
    }
    AssertIntEQ(wc_PKCS7_VerifySignedData(pkcs7, output, outputSz), 0);

    wc_PKCS7_Free(pkcs7);
    wc_FreeRng(&rng);

    return outputSz;
}
#endif

/*
 * Testing wc_PKCS_VerifySignedData()
 */
static void test_wc_PKCS7_VerifySignedData(void)
{
#if defined(HAVE_PKCS7)
    PKCS7* pkcs7;
    byte   output[FOURK_BUF];
    word32 outputSz = sizeof(output);
    byte   data[] = "Test data to encode.";
    byte   badOut[0];
    word32 badOutSz = (word32)sizeof(badOut);
    byte   badContent[] = "This is different content than was signed";

    AssertIntGT((outputSz = CreatePKCS7SignedData(output, outputSz, data,
                                                  (word32)sizeof(data),
                                                  0, 0)), 0);

    AssertNotNull(pkcs7 = wc_PKCS7_New(HEAP_HINT, devId));
    AssertIntEQ(wc_PKCS7_Init(pkcs7, HEAP_HINT, INVALID_DEVID), 0);
    AssertIntEQ(wc_PKCS7_InitWithCert(pkcs7, NULL, 0), 0);
    AssertIntEQ(wc_PKCS7_VerifySignedData(pkcs7, output, outputSz), 0);

    /* Test bad args. */
    AssertIntEQ(wc_PKCS7_VerifySignedData(NULL, output, outputSz), BAD_FUNC_ARG);
    AssertIntEQ(wc_PKCS7_VerifySignedData(pkcs7, NULL, outputSz), BAD_FUNC_ARG);
#ifndef NO_PKCS7_STREAM
    /* can pass in 0 buffer length with streaming API */
    AssertIntEQ(wc_PKCS7_VerifySignedData(pkcs7, badOut,
                                badOutSz), WC_PKCS7_WANT_READ_E);
#else
    AssertIntEQ(wc_PKCS7_VerifySignedData(pkcs7, badOut,
                                badOutSz), BAD_FUNC_ARG);
#endif

    wc_PKCS7_Free(pkcs7);

    /* Invalid content should error, use detached signature so we can
     * easily change content */
    AssertIntGT((outputSz = CreatePKCS7SignedData(output, outputSz, data,
                                                  (word32)sizeof(data),
                                                  1, 1)), 0);
    AssertNotNull(pkcs7 = wc_PKCS7_New(HEAP_HINT, devId));
    AssertIntEQ(wc_PKCS7_InitWithCert(pkcs7, NULL, 0), 0);
    pkcs7->content = badContent;
    pkcs7->contentSz = sizeof(badContent);
    AssertIntEQ(wc_PKCS7_VerifySignedData(pkcs7, output, outputSz), SIG_VERIFY_E);
    wc_PKCS7_Free(pkcs7);

    /* Test success case with detached signature and valid content */
    AssertNotNull(pkcs7 = wc_PKCS7_New(HEAP_HINT, devId));
    AssertIntEQ(wc_PKCS7_InitWithCert(pkcs7, NULL, 0), 0);
    pkcs7->content = data;
    pkcs7->contentSz = sizeof(data);
    AssertIntEQ(wc_PKCS7_VerifySignedData(pkcs7, output, outputSz), 0);
    wc_PKCS7_Free(pkcs7);

    printf(resultFmt, passed);
#endif
} /* END test_wc_PKCS7_VerifySignedData() */


#if defined(HAVE_PKCS7) && !defined(NO_AES) && !defined(NO_AES_256)
static const byte defKey[] = {
    0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,
    0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,
    0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,
    0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08
};
static byte aesHandle[32]; /* simulated hardware key handle */

/* return 0 on success */
static int myDecryptionFunc(PKCS7* pkcs7, int encryptOID, byte* iv, int ivSz,
        byte* aad, word32 aadSz, byte* authTag, word32 authTagSz,
        byte* in, int inSz, byte* out, void* usrCtx)
{
    int ret;
    Aes aes;

    if (usrCtx == NULL) {
        /* no simulated handle passed in */
        return -1;
    }

    switch (encryptOID) {
        case AES256CBCb:
            if (ivSz  != AES_BLOCK_SIZE)
                return BAD_FUNC_ARG;
            break;

        default:
            WOLFSSL_MSG("Unsupported content cipher type for test");
            return ALGO_ID_E;
    };

    /* simulate using handle to get key */
    ret = wc_AesInit(&aes, HEAP_HINT, INVALID_DEVID);
    if (ret == 0) {
        ret = wc_AesSetKey(&aes, (byte*)usrCtx, 32, iv, AES_DECRYPTION);
        if (ret == 0)
            ret = wc_AesCbcDecrypt(&aes, out, in, inSz);
        wc_AesFree(&aes);
    }

    (void)aad;
    (void)aadSz;
    (void)authTag;
    (void)authTagSz;
    (void)pkcs7;
    return ret;
}


/* returns key size on success */
static int myCEKwrapFunc(PKCS7* pkcs7, byte* cek, word32 cekSz, byte* keyId,
        word32 keyIdSz, byte* orginKey, word32 orginKeySz,
        byte* out, word32 outSz, int keyWrapAlgo, int type, int direction)
{
    int ret = -1;

    if (out == NULL)
        return BAD_FUNC_ARG;

    if (keyId[0] != 0x00) {
        return -1;
    }

    if (type != (int)PKCS7_KEKRI) {
        return -1;
    }

    switch (keyWrapAlgo) {
        case AES256_WRAP:
            /* simulate setting a handle for later decryption but use key
             * as handle in the test case here */
            ret = wc_AesKeyUnWrap(defKey, sizeof(defKey), cek, cekSz,
                                      aesHandle, sizeof(aesHandle), NULL);
            if (ret < 0)
                return ret;

            ret = wc_PKCS7_SetDecodeEncryptedCtx(pkcs7, (void*)aesHandle);
            if (ret < 0)
                return ret;

            /* return key size on success */
            return sizeof(defKey);

        default:
            WOLFSSL_MSG("Unsupported key wrap algorithm in example");
            return BAD_KEYWRAP_ALG_E;
    };

    (void)cekSz;
    (void)cek;
    (void)outSz;
    (void)keyIdSz;
    (void)direction;
    (void)orginKey; /* used with KAKRI */
    (void)orginKeySz;
    return ret;
}
#endif /* HAVE_PKCS7 && !NO_AES && !NO_AES_256 */


/*
 * Testing wc_PKCS7_EncodeEnvelopedData()
 */
static void test_wc_PKCS7_EncodeDecodeEnvelopedData (void)
{
#if defined(HAVE_PKCS7)
    PKCS7*      pkcs7;
    word32      tempWrd32   = 0;
    byte*       tmpBytePtr = NULL;
    const char  input[] = "Test data to encode.";
    int         i;
    int         testSz = 0;
    #if !defined(NO_RSA) && (!defined(NO_AES) || (!defined(NO_SHA) || \
        !defined(NO_SHA256) || defined(WOLFSSL_SHA512)))

        byte*   rsaCert     = NULL;
        byte*   rsaPrivKey  = NULL;
        word32  rsaCertSz;
        word32  rsaPrivKeySz;
        #if !defined(NO_FILESYSTEM) && (!defined(USE_CERT_BUFFERS_1024) && \
                                           !defined(USE_CERT_BUFFERS_2048) )
            static const char* rsaClientCert = "./certs/client-cert.der";
            static const char* rsaClientKey = "./certs/client-key.der";
            rsaCertSz = (word32)sizeof(rsaClientCert);
            rsaPrivKeySz = (word32)sizeof(rsaClientKey);
        #endif
    #endif
    #if defined(HAVE_ECC) && (!defined(NO_AES) || (!defined(NO_SHA) ||\
        !defined(NO_SHA256) || defined(WOLFSSL_SHA512)))

        byte*   eccCert     = NULL;
        byte*   eccPrivKey  = NULL;
        word32  eccCertSz;
        word32  eccPrivKeySz;
        #if !defined(NO_FILESYSTEM) && !defined(USE_CERT_BUFFERS_256)
            static const char* eccClientCert = "./certs/client-ecc-cert.der";
            static const char* eccClientKey = "./certs/ecc-client-key.der";
        #endif
    #endif
    /* Generic buffer size. */
    byte    output[ONEK_BUF];
    byte    decoded[sizeof(input)/sizeof(char)];
    int     decodedSz = 0;
#ifndef NO_FILESYSTEM
    XFILE certFile;
    XFILE keyFile;
#endif

#if !defined(NO_RSA) && (!defined(NO_AES) || (!defined(NO_SHA) ||\
    !defined(NO_SHA256) || defined(WOLFSSL_SHA512)))
    /* RSA certs and keys. */
    #if defined(USE_CERT_BUFFERS_1024)
        /* Allocate buffer space. */
        AssertNotNull(rsaCert =
                (byte*)XMALLOC(ONEK_BUF, HEAP_HINT, DYNAMIC_TYPE_TMP_BUFFER));
        /* Init buffer. */
        rsaCertSz = (word32)sizeof_client_cert_der_1024;
        XMEMCPY(rsaCert, client_cert_der_1024, rsaCertSz);
        AssertNotNull(rsaPrivKey = (byte*)XMALLOC(ONEK_BUF, HEAP_HINT,
                                DYNAMIC_TYPE_TMP_BUFFER));
        rsaPrivKeySz = (word32)sizeof_client_key_der_1024;
        XMEMCPY(rsaPrivKey, client_key_der_1024, rsaPrivKeySz);

    #elif defined(USE_CERT_BUFFERS_2048)
        /* Allocate buffer */
        AssertNotNull(rsaCert =
                (byte*)XMALLOC(TWOK_BUF, HEAP_HINT, DYNAMIC_TYPE_TMP_BUFFER));
        /* Init buffer. */
        rsaCertSz = (word32)sizeof_client_cert_der_2048;
        XMEMCPY(rsaCert, client_cert_der_2048, rsaCertSz);
        AssertNotNull(rsaPrivKey = (byte*)XMALLOC(TWOK_BUF, HEAP_HINT,
                                DYNAMIC_TYPE_TMP_BUFFER));
        rsaPrivKeySz = (word32)sizeof_client_key_der_2048;
        XMEMCPY(rsaPrivKey, client_key_der_2048, rsaPrivKeySz);

    #else
        /* File system. */
        certFile = XFOPEN(rsaClientCert, "rb");
        AssertTrue(certFile != XBADFILE);
        rsaCertSz = (word32)FOURK_BUF;
        AssertNotNull(rsaCert =
                (byte*)XMALLOC(FOURK_BUF, HEAP_HINT, DYNAMIC_TYPE_TMP_BUFFER));
        rsaCertSz = (word32)XFREAD(rsaCert, 1, rsaCertSz, certFile);
        XFCLOSE(certFile);
        keyFile = XFOPEN(rsaClientKey, "rb");
        AssertTrue(keyFile != XBADFILE);
        AssertNotNull(rsaPrivKey = (byte*)XMALLOC(FOURK_BUF, HEAP_HINT,
                                DYNAMIC_TYPE_TMP_BUFFER));
        rsaPrivKeySz = (word32)FOURK_BUF;
        rsaPrivKeySz = (word32)XFREAD(rsaPrivKey, 1, rsaPrivKeySz, keyFile);
        XFCLOSE(keyFile);
    #endif /* USE_CERT_BUFFERS */
#endif /* NO_RSA */

/* ECC */
#if defined(HAVE_ECC) && (!defined(NO_AES) || (!defined(NO_SHA) ||\
    !defined(NO_SHA256) || defined(WOLFSSL_SHA512)))

    #ifdef USE_CERT_BUFFERS_256
        AssertNotNull(eccCert =
                (byte*)XMALLOC(TWOK_BUF, HEAP_HINT, DYNAMIC_TYPE_TMP_BUFFER));
        /* Init buffer. */
        eccCertSz = (word32)sizeof_cliecc_cert_der_256;
        XMEMCPY(eccCert, cliecc_cert_der_256, eccCertSz);
        AssertNotNull(eccPrivKey = (byte*)XMALLOC(TWOK_BUF, HEAP_HINT,
                                DYNAMIC_TYPE_TMP_BUFFER));
        eccPrivKeySz = (word32)sizeof_ecc_clikey_der_256;
        XMEMCPY(eccPrivKey, ecc_clikey_der_256, eccPrivKeySz);
    #else /* File system. */
        certFile = XFOPEN(eccClientCert, "rb");
        AssertTrue(certFile != XBADFILE);
        eccCertSz = (word32)FOURK_BUF;
        AssertNotNull(eccCert =
                (byte*)XMALLOC(FOURK_BUF, HEAP_HINT, DYNAMIC_TYPE_TMP_BUFFER));
        eccCertSz = (word32)XFREAD(eccCert, 1, eccCertSz, certFile);
        XFCLOSE(certFile);
        keyFile = XFOPEN(eccClientKey, "rb");
        AssertTrue(keyFile != XBADFILE);
        eccPrivKeySz = (word32)FOURK_BUF;
        AssertNotNull(eccPrivKey = (byte*)XMALLOC(FOURK_BUF, HEAP_HINT,
                                DYNAMIC_TYPE_TMP_BUFFER));
        eccPrivKeySz = (word32)XFREAD(eccPrivKey, 1, eccPrivKeySz, keyFile);
        XFCLOSE(keyFile);
    #endif /* USE_CERT_BUFFERS_256 */
#endif /* END HAVE_ECC */

    /* Silence. */
    (void)keyFile;
    (void)certFile;

    const pkcs7EnvelopedVector testVectors[] = {
    /* DATA is a global variable defined in the makefile. */
#if !defined(NO_RSA)
    #ifndef NO_DES3
        {(byte*)input, (word32)(sizeof(input)/sizeof(char)), DATA, DES3b, 0, 0,
            rsaCert, rsaCertSz, rsaPrivKey, rsaPrivKeySz},
    #endif /* NO_DES3 */
    #ifndef NO_AES
        #ifndef NO_AES_128
        {(byte*)input, (word32)(sizeof(input)/sizeof(char)), DATA, AES128CBCb,
            0, 0, rsaCert, rsaCertSz, rsaPrivKey, rsaPrivKeySz},
        #endif
        #ifndef NO_AES_192
        {(byte*)input, (word32)(sizeof(input)/sizeof(char)), DATA, AES192CBCb,
            0, 0, rsaCert, rsaCertSz, rsaPrivKey, rsaPrivKeySz},
        #endif
        #ifndef NO_AES_256
        {(byte*)input, (word32)(sizeof(input)/sizeof(char)), DATA, AES256CBCb,
            0, 0, rsaCert, rsaCertSz, rsaPrivKey, rsaPrivKeySz},
        #endif
    #endif /* NO_AES */

#endif /* NO_RSA */
#if defined(HAVE_ECC)
    #ifndef NO_AES
        #if !defined(NO_SHA) && !defined(NO_AES_128)
            {(byte*)input, (word32)(sizeof(input)/sizeof(char)), DATA, AES128CBCb,
                AES128_WRAP, dhSinglePass_stdDH_sha1kdf_scheme, eccCert,
                eccCertSz, eccPrivKey, eccPrivKeySz},
        #endif
        #if !defined(NO_SHA256) && !defined(NO_AES_256)
            {(byte*)input, (word32)(sizeof(input)/sizeof(char)), DATA, AES256CBCb,
                AES256_WRAP, dhSinglePass_stdDH_sha256kdf_scheme, eccCert,
                eccCertSz, eccPrivKey, eccPrivKeySz},
        #endif
        #if defined(WOLFSSL_SHA512) && !defined(NO_AES_256)
            {(byte*)input, (word32)(sizeof(input)/sizeof(char)), DATA, AES256CBCb,
                AES256_WRAP, dhSinglePass_stdDH_sha512kdf_scheme, eccCert,
                eccCertSz, eccPrivKey, eccPrivKeySz},
        #endif
    #endif /* NO_AES */
#endif /* END HAVE_ECC */
    }; /* END pkcs7EnvelopedVector */

    printf(testingFmt, "wc_PKCS7_EncodeEnvelopedData()");

    AssertNotNull(pkcs7 = wc_PKCS7_New(HEAP_HINT, devId));
    AssertIntEQ(wc_PKCS7_Init(pkcs7, HEAP_HINT, devId), 0);

    testSz = (int)sizeof(testVectors)/(int)sizeof(pkcs7EnvelopedVector);
    for (i = 0; i < testSz; i++) {
        AssertIntEQ(wc_PKCS7_InitWithCert(pkcs7, (testVectors + i)->cert,
                                    (word32)(testVectors + i)->certSz), 0);

        pkcs7->content       = (byte*)(testVectors + i)->content;
        pkcs7->contentSz     = (testVectors + i)->contentSz;
        pkcs7->contentOID    = (testVectors + i)->contentOID;
        pkcs7->encryptOID    = (testVectors + i)->encryptOID;
        pkcs7->keyWrapOID    = (testVectors + i)->keyWrapOID;
        pkcs7->keyAgreeOID   = (testVectors + i)->keyAgreeOID;
        pkcs7->privateKey    = (testVectors + i)->privateKey;
        pkcs7->privateKeySz  = (testVectors + i)->privateKeySz;

        AssertIntGE(wc_PKCS7_EncodeEnvelopedData(pkcs7, output,
                            (word32)sizeof(output)), 0);

        decodedSz = wc_PKCS7_DecodeEnvelopedData(pkcs7, output,
                (word32)sizeof(output), decoded, (word32)sizeof(decoded));
        AssertIntGE(decodedSz, 0);
        /* Verify the size of each buffer. */
        AssertIntEQ((word32)sizeof(input)/sizeof(char), decodedSz);
        /* Don't free the last time through the loop. */
        if (i < testSz - 1 ){
            wc_PKCS7_Free(pkcs7);
            AssertNotNull(pkcs7 = wc_PKCS7_New(HEAP_HINT, devId));
        }
    }  /* END test loop. */

    /* Test bad args. */
    AssertIntEQ(wc_PKCS7_EncodeEnvelopedData(NULL, output,
                    (word32)sizeof(output)), BAD_FUNC_ARG);
    AssertIntEQ(wc_PKCS7_EncodeEnvelopedData(pkcs7, NULL,
                    (word32)sizeof(output)), BAD_FUNC_ARG);
    AssertIntEQ(wc_PKCS7_EncodeEnvelopedData(pkcs7, output, 0), BAD_FUNC_ARG);
    printf(resultFmt, passed);

    /* Decode.  */
    printf(testingFmt, "wc_PKCS7_DecodeEnvelopedData()");

    AssertIntEQ(wc_PKCS7_DecodeEnvelopedData(NULL, output,
        (word32)sizeof(output), decoded, (word32)sizeof(decoded)), BAD_FUNC_ARG);
    AssertIntEQ(wc_PKCS7_DecodeEnvelopedData(pkcs7, output,
        (word32)sizeof(output), NULL, (word32)sizeof(decoded)), BAD_FUNC_ARG);
    AssertIntEQ(wc_PKCS7_DecodeEnvelopedData(pkcs7, output,
        (word32)sizeof(output), decoded, 0), BAD_FUNC_ARG);
    AssertIntEQ(wc_PKCS7_DecodeEnvelopedData(pkcs7, NULL,
        (word32)sizeof(output), decoded, (word32)sizeof(decoded)), BAD_FUNC_ARG);
    AssertIntEQ(wc_PKCS7_DecodeEnvelopedData(pkcs7, output, 0, decoded,
        (word32)sizeof(decoded)), BAD_FUNC_ARG);
    /* Should get a return of BAD_FUNC_ARG with structure data. Order matters.*/
#if defined(HAVE_ECC) && !defined(NO_AES)
    /* only a failure for KARI test cases */
    tempWrd32 = pkcs7->singleCertSz;
    pkcs7->singleCertSz = 0;
    AssertIntEQ(wc_PKCS7_DecodeEnvelopedData(pkcs7, output,
        (word32)sizeof(output), decoded, (word32)sizeof(decoded)), BAD_FUNC_ARG);
    pkcs7->singleCertSz = tempWrd32;

    tmpBytePtr = pkcs7->singleCert;
    pkcs7->singleCert = NULL;
    AssertIntEQ(wc_PKCS7_DecodeEnvelopedData(pkcs7, output,
        (word32)sizeof(output), decoded, (word32)sizeof(decoded)), BAD_FUNC_ARG);
    pkcs7->singleCert = tmpBytePtr;
#endif
    tempWrd32 = pkcs7->privateKeySz;
    pkcs7->privateKeySz = 0;
    AssertIntEQ(wc_PKCS7_DecodeEnvelopedData(pkcs7, output,
        (word32)sizeof(output), decoded, (word32)sizeof(decoded)), BAD_FUNC_ARG);
    pkcs7->privateKeySz = tempWrd32;

    tmpBytePtr = pkcs7->privateKey;
    pkcs7->privateKey = NULL;
    AssertIntEQ(wc_PKCS7_DecodeEnvelopedData(pkcs7, output,
        (word32)sizeof(output), decoded, (word32)sizeof(decoded)), BAD_FUNC_ARG);
    pkcs7->privateKey = tmpBytePtr;
    wc_PKCS7_Free(pkcs7);

#if !defined(NO_AES) && !defined(NO_AES_256)
    /* test of decrypt callback with KEKRI enveloped data */
    {
        int envelopedSz;
        const byte keyId[] = { 0x00 };

        AssertNotNull(pkcs7 = wc_PKCS7_New(HEAP_HINT, devId));
        pkcs7->content      = (byte*)input;
        pkcs7->contentSz    = (word32)(sizeof(input)/sizeof(char));
        pkcs7->contentOID   = DATA;
        pkcs7->encryptOID   = AES256CBCb;
        AssertIntGT(wc_PKCS7_AddRecipient_KEKRI(pkcs7, AES256_WRAP,
                    (byte*)defKey, sizeof(defKey), (byte*)keyId,
                    sizeof(keyId), NULL, NULL, 0, NULL, 0, 0), 0);
        AssertIntEQ(wc_PKCS7_SetSignerIdentifierType(pkcs7, CMS_SKID), 0);
        AssertIntGT((envelopedSz = wc_PKCS7_EncodeEnvelopedData(pkcs7, output,
                        (word32)sizeof(output))), 0);
        wc_PKCS7_Free(pkcs7);

        /* decode envelopedData */
        AssertNotNull(pkcs7 = wc_PKCS7_New(HEAP_HINT, devId));
        AssertIntEQ(wc_PKCS7_SetWrapCEKCb(pkcs7, myCEKwrapFunc), 0);
        AssertIntEQ(wc_PKCS7_SetDecodeEncryptedCb(pkcs7, myDecryptionFunc), 0);
        AssertIntGT((decodedSz = wc_PKCS7_DecodeEnvelopedData(pkcs7, output,
                        envelopedSz, decoded, sizeof(decoded))), 0);
        wc_PKCS7_Free(pkcs7);
    }
#endif /* !NO_AES && !NO_AES_256 */


    printf(resultFmt, passed);
#ifndef NO_RSA
    if (rsaCert) {
        XFREE(rsaCert, HEAP_HINT, DYNAMIC_TYPE_TMP_BUFFER);
    }
    if (rsaPrivKey) {
        XFREE(rsaPrivKey, HEAP_HINT, DYNAMIC_TYPE_TMP_BUFFER);
    }
#endif /*NO_RSA */
#ifdef HAVE_ECC
    if (eccCert) {
        XFREE(eccCert, HEAP_HINT, DYNAMIC_TYPE_TMP_BUFFER);
    }
    if (eccPrivKey) {
        XFREE(eccPrivKey, HEAP_HINT, DYNAMIC_TYPE_TMP_BUFFER);
    }
#endif /* HAVE_ECC */

#endif /* HAVE_PKCS7 */
} /* END test_wc_PKCS7_EncodeEnvelopedData() */


/*
 * Testing wc_PKCS7_EncodeEncryptedData()
 */
static void test_wc_PKCS7_EncodeEncryptedData (void)
{
#if defined(HAVE_PKCS7) && !defined(NO_PKCS7_ENCRYPTED_DATA)
    PKCS7*      pkcs7;
    byte*       tmpBytePtr = NULL;
    byte        encrypted[TWOK_BUF];
    byte        decoded[TWOK_BUF];
    word32      tmpWrd32 = 0;
    int         tmpInt = 0;
    int         decodedSz;
    int         encryptedSz;
    int         testSz;
    int         i;

    const byte data[] = { /* Hello World */
        0x48,0x65,0x6c,0x6c,0x6f,0x20,0x57,0x6f,
        0x72,0x6c,0x64
    };

    #ifndef NO_DES3
        byte desKey[] = {
            0x01,0x23,0x45,0x67,0x89,0xab,0xcd,0xef
        };
        byte des3Key[] = {
            0x01,0x23,0x45,0x67,0x89,0xab,0xcd,0xef,
            0xfe,0xde,0xba,0x98,0x76,0x54,0x32,0x10,
            0x89,0xab,0xcd,0xef,0x01,0x23,0x45,0x67
        };
    #endif

    #ifndef NO_AES
        #ifndef NO_AES_128
        byte aes128Key[] = {
            0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,
            0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08
        };
        #endif
        #ifndef NO_AES_192
        byte aes192Key[] = {
            0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,
            0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,
            0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08
        };
        #endif
        #ifndef NO_AES_256
        byte aes256Key[] = {
            0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,
            0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,
            0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,
            0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08
        };
        #endif
    #endif
    const pkcs7EncryptedVector testVectors[] =
    {
    #ifndef NO_DES3
        {data, (word32)sizeof(data), DATA, DES3b, des3Key, sizeof(des3Key)},

        {data, (word32)sizeof(data), DATA, DESb, desKey, sizeof(desKey)},
    #endif /* NO_DES3 */
    #ifndef NO_AES
        #ifndef NO_AES_128
        {data, (word32)sizeof(data), DATA, AES128CBCb, aes128Key,
         sizeof(aes128Key)},
        #endif

        #ifndef NO_AES_192
        {data, (word32)sizeof(data), DATA, AES192CBCb, aes192Key,
         sizeof(aes192Key)},
        #endif

        #ifndef NO_AES_256
        {data, (word32)sizeof(data), DATA, AES256CBCb, aes256Key,
         sizeof(aes256Key)},
        #endif

    #endif /* NO_AES */
    };

    testSz = sizeof(testVectors) / sizeof(pkcs7EncryptedVector);

    for (i = 0; i < testSz; i++) {
        AssertNotNull(pkcs7 = wc_PKCS7_New(HEAP_HINT, devId));
        AssertIntEQ(wc_PKCS7_Init(pkcs7, HEAP_HINT, devId), 0);
        pkcs7->content              = (byte*)testVectors[i].content;
        pkcs7->contentSz            = testVectors[i].contentSz;
        pkcs7->contentOID           = testVectors[i].contentOID;
        pkcs7->encryptOID           = testVectors[i].encryptOID;
        pkcs7->encryptionKey        = testVectors[i].encryptionKey;
        pkcs7->encryptionKeySz      = testVectors[i].encryptionKeySz;
        pkcs7->heap                 = HEAP_HINT;

        /* encode encryptedData */
        encryptedSz = wc_PKCS7_EncodeEncryptedData(pkcs7, encrypted,
                                                   sizeof(encrypted));
        AssertIntGT(encryptedSz, 0);

       /* Decode encryptedData */
        decodedSz = wc_PKCS7_DecodeEncryptedData(pkcs7, encrypted, encryptedSz,
                                                    decoded, sizeof(decoded));

        AssertIntEQ(XMEMCMP(decoded, data, decodedSz), 0);
        /* Keep values for last itr. */
        if (i < testSz - 1) {
            wc_PKCS7_Free(pkcs7);
        }
    }

    printf(testingFmt, "wc_PKCS7_EncodeEncryptedData()");
    AssertIntEQ(wc_PKCS7_EncodeEncryptedData(NULL, encrypted,
                     sizeof(encrypted)),BAD_FUNC_ARG);
    AssertIntEQ(wc_PKCS7_EncodeEncryptedData(pkcs7, NULL,
                     sizeof(encrypted)), BAD_FUNC_ARG);
    AssertIntEQ(wc_PKCS7_EncodeEncryptedData(pkcs7, encrypted,
                     0), BAD_FUNC_ARG);
    /* Testing the struct. */
    tmpBytePtr = pkcs7->content;
    pkcs7->content = NULL;
    AssertIntEQ(wc_PKCS7_EncodeEncryptedData(pkcs7, encrypted,
                             sizeof(encrypted)), BAD_FUNC_ARG);
    pkcs7->content = tmpBytePtr;
    tmpWrd32 = pkcs7->contentSz;
    pkcs7->contentSz = 0;
    AssertIntEQ(wc_PKCS7_EncodeEncryptedData(pkcs7, encrypted,
                             sizeof(encrypted)), BAD_FUNC_ARG);
    pkcs7->contentSz = tmpWrd32;
    tmpInt = pkcs7->encryptOID;
    pkcs7->encryptOID = 0;
    AssertIntEQ(wc_PKCS7_EncodeEncryptedData(pkcs7, encrypted,
                             sizeof(encrypted)), BAD_FUNC_ARG);
    pkcs7->encryptOID = tmpInt;
    tmpBytePtr = pkcs7->encryptionKey;
    pkcs7->encryptionKey = NULL;
    AssertIntEQ(wc_PKCS7_EncodeEncryptedData(pkcs7, encrypted,
                             sizeof(encrypted)), BAD_FUNC_ARG);
    pkcs7->encryptionKey = tmpBytePtr;
    tmpWrd32 = pkcs7->encryptionKeySz;
    pkcs7->encryptionKeySz = 0;
    AssertIntEQ(wc_PKCS7_EncodeEncryptedData(pkcs7, encrypted,
                             sizeof(encrypted)), BAD_FUNC_ARG);
    pkcs7->encryptionKeySz = tmpWrd32;

    printf(resultFmt, passed);

    printf(testingFmt, "wc_PKCS7_EncodeEncryptedData()");

    AssertIntEQ(wc_PKCS7_DecodeEncryptedData(NULL, encrypted, encryptedSz,
                decoded, sizeof(decoded)), BAD_FUNC_ARG);
    AssertIntEQ(wc_PKCS7_DecodeEncryptedData(pkcs7, NULL, encryptedSz,
                decoded, sizeof(decoded)), BAD_FUNC_ARG);
    AssertIntEQ(wc_PKCS7_DecodeEncryptedData(pkcs7, encrypted, 0,
                decoded, sizeof(decoded)), BAD_FUNC_ARG);
    AssertIntEQ(wc_PKCS7_DecodeEncryptedData(pkcs7, encrypted, encryptedSz,
                NULL, sizeof(decoded)), BAD_FUNC_ARG);
    AssertIntEQ(wc_PKCS7_DecodeEncryptedData(pkcs7, encrypted, encryptedSz,
                decoded, 0), BAD_FUNC_ARG);
    /* Test struct fields */

    tmpBytePtr = pkcs7->encryptionKey;
    pkcs7->encryptionKey = NULL;
    AssertIntEQ(wc_PKCS7_DecodeEncryptedData(pkcs7, encrypted, encryptedSz,
                                   decoded, sizeof(decoded)), BAD_FUNC_ARG);
    pkcs7->encryptionKey = tmpBytePtr;
    pkcs7->encryptionKeySz = 0;
    AssertIntEQ(wc_PKCS7_DecodeEncryptedData(pkcs7, encrypted, encryptedSz,
                                   decoded, sizeof(decoded)), BAD_FUNC_ARG);

    printf(resultFmt, passed);
    wc_PKCS7_Free(pkcs7);
#endif
} /* END test_wc_PKCS7_EncodeEncryptedData() */

/*
 * Testing wc_PKCS7_Degenerate()
 */
static void test_wc_PKCS7_Degenerate(void)
{
#if defined(HAVE_PKCS7) && !defined(NO_FILESYSTEM)
    PKCS7* pkcs7;
    char   fName[] = "./certs/test-degenerate.p7b";
    XFILE  f;
    byte   der[4096];
    word32 derSz;
    int    ret;

    printf(testingFmt, "wc_PKCS7_Degenerate()");

    AssertNotNull(f = XFOPEN(fName, "rb"));
    AssertIntGT((ret = (int)fread(der, 1, sizeof(der), f)), 0);
    derSz = (word32)ret;
    XFCLOSE(f);

    /* test degenerate success */
    AssertNotNull(pkcs7 = wc_PKCS7_New(HEAP_HINT, devId));
    AssertIntEQ(wc_PKCS7_Init(pkcs7, HEAP_HINT, INVALID_DEVID), 0);
    AssertIntEQ(wc_PKCS7_InitWithCert(pkcs7, NULL, 0), 0);
#ifndef NO_RSA
    AssertIntEQ(wc_PKCS7_VerifySignedData(pkcs7, der, derSz), 0);
#else
    AssertIntNE(wc_PKCS7_VerifySignedData(pkcs7, der, derSz), 0);
#endif
    wc_PKCS7_Free(pkcs7);

    /* test with turning off degenerate cases */
    AssertNotNull(pkcs7 = wc_PKCS7_New(HEAP_HINT, devId));
    AssertIntEQ(wc_PKCS7_Init(pkcs7, HEAP_HINT, INVALID_DEVID), 0);
    AssertIntEQ(wc_PKCS7_InitWithCert(pkcs7, NULL, 0), 0);
    wc_PKCS7_AllowDegenerate(pkcs7, 0); /* override allowing degenerate case */
    AssertIntEQ(wc_PKCS7_VerifySignedData(pkcs7, der, derSz), PKCS7_NO_SIGNER_E);
    wc_PKCS7_Free(pkcs7);

    printf(resultFmt, passed);
#endif
} /* END test_wc_PKCS7_Degenerate() */

#if defined(HAVE_PKCS7) && !defined(NO_FILESYSTEM) && \
    defined(ASN_BER_TO_DER) && !defined(NO_DES3)
static byte berContent[] = {
    0x30, 0x80, 0x06, 0x09, 0x2A, 0x86, 0x48, 0x86,
    0xF7, 0x0D, 0x01, 0x07, 0x03, 0xA0, 0x80, 0x30,
    0x80, 0x02, 0x01, 0x00, 0x31, 0x82, 0x01, 0x48,
    0x30, 0x82, 0x01, 0x44, 0x02, 0x01, 0x00, 0x30,
    0x81, 0xAC, 0x30, 0x81, 0x9E, 0x31, 0x0B, 0x30,
    0x09, 0x06, 0x03, 0x55, 0x04, 0x06, 0x13, 0x02,
    0x55, 0x53, 0x31, 0x10, 0x30, 0x0E, 0x06, 0x03,
    0x55, 0x04, 0x08, 0x0C, 0x07, 0x4D, 0x6F, 0x6E,
    0x74, 0x61, 0x6E, 0x61, 0x31, 0x10, 0x30, 0x0E,
    0x06, 0x03, 0x55, 0x04, 0x07, 0x0C, 0x07, 0x42,
    0x6F, 0x7A, 0x65, 0x6D, 0x61, 0x6E, 0x31, 0x15,
    0x30, 0x13, 0x06, 0x03, 0x55, 0x04, 0x0A, 0x0C,
    0x0C, 0x77, 0x6F, 0x6C, 0x66, 0x53, 0x53, 0x4C,
    0x5F, 0x31, 0x30, 0x32, 0x34, 0x31, 0x19, 0x30,
    0x17, 0x06, 0x03, 0x55, 0x04, 0x0B, 0x0C, 0x10,
    0x50, 0x72, 0x6F, 0x67, 0x72, 0x61, 0x6D, 0x6D,
    0x69, 0x6E, 0x67, 0x2D, 0x31, 0x30, 0x32, 0x34,
    0x31, 0x18, 0x30, 0x16, 0x06, 0x03, 0x55, 0x04,
    0x03, 0x0C, 0x0F, 0x77, 0x77, 0x77, 0x2E, 0x77,
    0x6F, 0x6C, 0x66, 0x73, 0x73, 0x6C, 0x2E, 0x63,
    0x6F, 0x6D, 0x31, 0x1F, 0x30, 0x1D, 0x06, 0x09,
    0x2A, 0x86, 0x48, 0x86, 0xF7, 0x0D, 0x01, 0x09,
    0x01, 0x16, 0x10, 0x69, 0x6E, 0x66, 0x6F, 0x40,
    0x77, 0x6F, 0x6C, 0x66, 0x73, 0x73, 0x6C, 0x2E,
    0x63, 0x6F, 0x6D, 0x02, 0x09, 0x00, 0xBB, 0xD3,
    0x10, 0x03, 0xE6, 0x9D, 0x28, 0x03, 0x30, 0x0D,
    0x06, 0x09, 0x2A, 0x86, 0x48, 0x86, 0xF7, 0x0D,
    0x01, 0x01, 0x01, 0x05, 0x00, 0x04, 0x81, 0x80,
    0x2F, 0xF9, 0x77, 0x4F, 0x04, 0x5C, 0x16, 0x62,
    0xF0, 0x77, 0x8D, 0x95, 0x4C, 0xB1, 0x44, 0x9A,
    0x8C, 0x3C, 0x8C, 0xE4, 0xD1, 0xC1, 0x14, 0x72,
    0xD0, 0x4A, 0x1A, 0x94, 0x27, 0x0F, 0xAA, 0xE8,
    0xD0, 0xA2, 0xE7, 0xED, 0x4C, 0x7F, 0x0F, 0xC7,
    0x1B, 0xFB, 0x81, 0x0E, 0x76, 0x8F, 0xDD, 0x32,
    0x11, 0x68, 0xA0, 0x13, 0xD2, 0x8D, 0x95, 0xEF,
    0x80, 0x53, 0x81, 0x0E, 0x1F, 0xC8, 0xD6, 0x76,
    0x5C, 0x31, 0xD3, 0x77, 0x33, 0x29, 0xA6, 0x1A,
    0xD3, 0xC6, 0x14, 0x36, 0xCA, 0x8E, 0x7D, 0x72,
    0xA0, 0x29, 0x4C, 0xC7, 0x3A, 0xAF, 0xFE, 0xF7,
    0xFC, 0xD7, 0xE2, 0x8F, 0x6A, 0x20, 0x46, 0x09,
    0x40, 0x22, 0x2D, 0x79, 0x38, 0x11, 0xB1, 0x4A,
    0xE3, 0x48, 0xE8, 0x10, 0x37, 0xA0, 0x22, 0xF7,
    0xB4, 0x79, 0xD1, 0xA9, 0x3D, 0xC2, 0xAB, 0x37,
    0xAE, 0x82, 0x68, 0x1A, 0x16, 0xEF, 0x33, 0x0C,
    0x30, 0x80, 0x06, 0x09, 0x2A, 0x86, 0x48, 0x86,
    0xF7, 0x0D, 0x01, 0x07, 0x01, 0x30, 0x14, 0x06,
    0x08, 0x2A, 0x86, 0x48, 0x86, 0xF7, 0x0D, 0x03,
    0x07, 0x04, 0x08, 0xAD, 0xD0, 0x38, 0x9B, 0x16,
    0x4B, 0x7F, 0x99, 0xA0, 0x80, 0x04, 0x82, 0x03,
    0xE8, 0x6D, 0x48, 0xFB, 0x8A, 0xBD, 0xED, 0x6C,
    0xCD, 0xC6, 0x48, 0xFD, 0xB7, 0xB0, 0x7C, 0x86,
    0x2C, 0x8D, 0xF0, 0x23, 0x12, 0xD8, 0xA3, 0x2A,
    0x21, 0x6F, 0x8B, 0x75, 0xBB, 0x47, 0x7F, 0xC9,
    0xBA, 0xBA, 0xFF, 0x91, 0x09, 0x01, 0x7A, 0x5C,
    0x96, 0x02, 0xB8, 0x8E, 0xF8, 0x67, 0x7E, 0x8F,
    0xF9, 0x51, 0x0E, 0xFF, 0x8E, 0xE2, 0x61, 0xC0,
    0xDF, 0xFA, 0xE2, 0x4C, 0x50, 0x90, 0xAE, 0xA1,
    0x15, 0x38, 0x3D, 0xBE, 0x88, 0xD7, 0x57, 0xC0,
    0x11, 0x44, 0xA2, 0x61, 0x05, 0x49, 0x6A, 0x94,
    0x04, 0x10, 0xD9, 0xC2, 0x2D, 0x15, 0x20, 0x0D,
    0xBD, 0xA2, 0xEF, 0xE4, 0x68, 0xFA, 0x39, 0x75,
    0x7E, 0xD8, 0x64, 0x44, 0xCB, 0xE0, 0x00, 0x6D,
    0x57, 0x4E, 0x8A, 0x17, 0xA9, 0x83, 0x6C, 0x7F,
    0xFE, 0x01, 0xEE, 0xDE, 0x99, 0x3A, 0xB2, 0xFF,
    0xD3, 0x72, 0x78, 0xBA, 0xF1, 0x23, 0x54, 0x48,
    0x02, 0xD8, 0x38, 0xA9, 0x54, 0xE5, 0x4A, 0x81,
    0xB9, 0xC0, 0x67, 0xB2, 0x7D, 0x3C, 0x6F, 0xCE,
    0xA4, 0xDD, 0x34, 0x5F, 0x60, 0xB1, 0xA3, 0x7A,
    0xE4, 0x43, 0xF2, 0x89, 0x64, 0x35, 0x09, 0x32,
    0x51, 0xFB, 0x5C, 0x67, 0x0C, 0x3B, 0xFC, 0x36,
    0x6B, 0x37, 0x43, 0x6C, 0x03, 0xCD, 0x44, 0xC7,
    0x2B, 0x62, 0xD6, 0xD1, 0xF4, 0x07, 0x7B, 0x19,
    0x91, 0xF0, 0xD7, 0xF5, 0x54, 0xBC, 0x0F, 0x42,
    0x6B, 0x69, 0xF7, 0xA3, 0xC8, 0xEE, 0xB9, 0x7A,
    0x9E, 0x3D, 0xDF, 0x53, 0x47, 0xF7, 0x50, 0x67,
    0x00, 0xCF, 0x2B, 0x3B, 0xE9, 0x85, 0xEE, 0xBD,
    0x4C, 0x64, 0x66, 0x0B, 0x77, 0x80, 0x9D, 0xEF,
    0x11, 0x32, 0x77, 0xA8, 0xA4, 0x5F, 0xEE, 0x2D,
    0xE0, 0x43, 0x87, 0x76, 0x87, 0x53, 0x4E, 0xD7,
    0x1A, 0x04, 0x7B, 0xE1, 0xD1, 0xE1, 0xF5, 0x87,
    0x51, 0x13, 0xE0, 0xC2, 0xAA, 0xA3, 0x4B, 0xAA,
    0x9E, 0xB4, 0xA6, 0x1D, 0x4E, 0x28, 0x57, 0x0B,
    0x80, 0x90, 0x81, 0x4E, 0x04, 0xF5, 0x30, 0x8D,
    0x51, 0xCE, 0x57, 0x2F, 0x88, 0xC5, 0x70, 0xC4,
    0x06, 0x8F, 0xDD, 0x37, 0xC1, 0x34, 0x1E, 0x0E,
    0x15, 0x32, 0x23, 0x92, 0xAB, 0x40, 0xEA, 0xF7,
    0x43, 0xE2, 0x1D, 0xE2, 0x4B, 0xC9, 0x91, 0xF4,
    0x63, 0x21, 0x34, 0xDB, 0xE9, 0x86, 0x83, 0x1A,
    0xD2, 0x52, 0xEF, 0x7A, 0xA2, 0xEE, 0xA4, 0x11,
    0x56, 0xD3, 0x6C, 0xF5, 0x6D, 0xE4, 0xA5, 0x2D,
    0x99, 0x02, 0x10, 0xDF, 0x29, 0xC5, 0xE3, 0x0B,
    0xC4, 0xA1, 0xEE, 0x5F, 0x4A, 0x10, 0xEE, 0x85,
    0x73, 0x2A, 0x92, 0x15, 0x2C, 0xC8, 0xF4, 0x8C,
    0xD7, 0x3D, 0xBC, 0xAD, 0x18, 0xE0, 0x59, 0xD3,
    0xEE, 0x75, 0x90, 0x1C, 0xCC, 0x76, 0xC6, 0x64,
    0x17, 0xD2, 0xD0, 0x91, 0xA6, 0xD0, 0xC1, 0x4A,
    0xAA, 0x58, 0x22, 0xEC, 0x45, 0x98, 0xF2, 0xCC,
    0x4C, 0xE4, 0xBF, 0xED, 0xF6, 0x44, 0x72, 0x36,
    0x65, 0x3F, 0xE3, 0xB5, 0x8B, 0x3E, 0x54, 0x9C,
    0x82, 0x86, 0x5E, 0xB0, 0xF2, 0x12, 0xE5, 0x69,
    0xFA, 0x46, 0xA2, 0x54, 0xFC, 0xF5, 0x4B, 0xE0,
    0x24, 0x3B, 0x99, 0x04, 0x1A, 0x7A, 0xF7, 0xD1,
    0xFF, 0x68, 0x97, 0xB2, 0x85, 0x82, 0x95, 0x27,
    0x2B, 0xF4, 0xE7, 0x1A, 0x74, 0x19, 0xEC, 0x8C,
    0x4E, 0xA7, 0x0F, 0xAD, 0x4F, 0x5A, 0x02, 0x80,
    0xC1, 0x6A, 0x9E, 0x54, 0xE4, 0x8E, 0xA3, 0x41,
    0x3F, 0x6F, 0x9C, 0x82, 0x9F, 0x83, 0xB0, 0x44,
    0x01, 0x5F, 0x10, 0x9D, 0xD3, 0xB6, 0x33, 0x5B,
    0xAF, 0xAC, 0x6B, 0x57, 0x2A, 0x01, 0xED, 0x0E,
    0x17, 0xB9, 0x80, 0x76, 0x12, 0x1C, 0x51, 0x56,
    0xDD, 0x6D, 0x94, 0xAB, 0xD2, 0xE5, 0x15, 0x2D,
    0x3C, 0xC5, 0xE8, 0x62, 0x05, 0x8B, 0x40, 0xB1,
    0xC2, 0x83, 0xCA, 0xAC, 0x4B, 0x8B, 0x39, 0xF7,
    0xA0, 0x08, 0x43, 0x5C, 0xF7, 0xE8, 0xED, 0x40,
    0x72, 0x73, 0xE3, 0x6B, 0x18, 0x67, 0xA0, 0xB6,
    0x0F, 0xED, 0x8F, 0x9A, 0xE4, 0x27, 0x62, 0x23,
    0xAA, 0x6D, 0x6C, 0x31, 0xC9, 0x9D, 0x6B, 0xE0,
    0xBF, 0x9D, 0x7D, 0x2E, 0x76, 0x71, 0x06, 0x39,
    0xAC, 0x96, 0x1C, 0xAF, 0x30, 0xF2, 0x62, 0x9C,
    0x84, 0x3F, 0x43, 0x5E, 0x19, 0xA8, 0xE5, 0x3C,
    0x9D, 0x43, 0x3C, 0x43, 0x41, 0xE8, 0x82, 0xE7,
    0x5B, 0xF3, 0xE2, 0x15, 0xE3, 0x52, 0x20, 0xFD,
    0x0D, 0xB2, 0x4D, 0x48, 0xAD, 0x53, 0x7E, 0x0C,
    0xF0, 0xB9, 0xBE, 0xC9, 0x58, 0x4B, 0xC8, 0xA8,
    0xA3, 0x36, 0xF1, 0x2C, 0xD2, 0xE1, 0xC8, 0xC4,
    0x3C, 0x48, 0x70, 0xC2, 0x6D, 0x6C, 0x3D, 0x99,
    0xAC, 0x43, 0x19, 0x69, 0xCA, 0x67, 0x1A, 0xC9,
    0xE1, 0x47, 0xFA, 0x0A, 0xE6, 0x5B, 0x6F, 0x61,
    0xD0, 0x03, 0xE4, 0x03, 0x4B, 0xFD, 0xE2, 0xA5,
    0x8D, 0x83, 0x01, 0x7E, 0xC0, 0x7B, 0x2E, 0x0B,
    0x29, 0xDD, 0xD6, 0xDC, 0x71, 0x46, 0xBD, 0x9A,
    0x40, 0x46, 0x1E, 0x0A, 0xB1, 0x00, 0xE7, 0x71,
    0x29, 0x77, 0xFC, 0x9A, 0x76, 0x8A, 0x5F, 0x66,
    0x9B, 0x63, 0x91, 0x12, 0x78, 0xBF, 0x67, 0xAD,
    0xA1, 0x72, 0x9E, 0xC5, 0x3E, 0xE5, 0xCB, 0xAF,
    0xD6, 0x5A, 0x0D, 0xB6, 0x9B, 0xA3, 0x78, 0xE8,
    0xB0, 0x8F, 0x69, 0xED, 0xC1, 0x73, 0xD5, 0xE5,
    0x1C, 0x18, 0xA0, 0x58, 0x4C, 0x49, 0xBD, 0x91,
    0xCE, 0x15, 0x0D, 0xAA, 0x5A, 0x07, 0xEA, 0x1C,
    0xA7, 0x4B, 0x11, 0x31, 0x80, 0xAF, 0xA1, 0x0A,
    0xED, 0x6C, 0x70, 0xE4, 0xDB, 0x75, 0x86, 0xAE,
    0xBF, 0x4A, 0x05, 0x72, 0xDE, 0x84, 0x8C, 0x7B,
    0x59, 0x81, 0x58, 0xE0, 0xC0, 0x15, 0xB5, 0xF3,
    0xD5, 0x73, 0x78, 0x83, 0x53, 0xDA, 0x92, 0xC1,
    0xE6, 0x71, 0x74, 0xC7, 0x7E, 0xAA, 0x36, 0x06,
    0xF0, 0xDF, 0xBA, 0xFB, 0xEF, 0x54, 0xE8, 0x11,
    0xB2, 0x33, 0xA3, 0x0B, 0x9E, 0x0C, 0x59, 0x75,
    0x13, 0xFA, 0x7F, 0x88, 0xB9, 0x86, 0xBD, 0x1A,
    0xDB, 0x52, 0x12, 0xFB, 0x6D, 0x1A, 0xCB, 0x49,
    0x94, 0x94, 0xC4, 0xA9, 0x99, 0xC0, 0xA4, 0xB6,
    0x60, 0x36, 0x09, 0x94, 0x2A, 0xD5, 0xC4, 0x26,
    0xF4, 0xA3, 0x6A, 0x0E, 0x57, 0x8B, 0x7C, 0xA4,
    0x1D, 0x75, 0xE8, 0x2A, 0xF3, 0xC4, 0x3C, 0x7D,
    0x45, 0x6D, 0xD8, 0x24, 0xD1, 0x3B, 0xF7, 0xCF,
    0xE4, 0x45, 0x2A, 0x55, 0xE5, 0xA9, 0x1F, 0x1C,
    0x8F, 0x55, 0x8D, 0xC1, 0xF7, 0x74, 0xCC, 0x26,
    0xC7, 0xBA, 0x2E, 0x5C, 0xC1, 0x71, 0x0A, 0xAA,
    0xD9, 0x6D, 0x76, 0xA7, 0xF9, 0xD1, 0x18, 0xCB,
    0x5A, 0x52, 0x98, 0xA8, 0x0D, 0x3F, 0x06, 0xFC,
    0x49, 0x11, 0x21, 0x5F, 0x86, 0x19, 0x33, 0x81,
    0xB5, 0x7A, 0xDA, 0xA1, 0x47, 0xBF, 0x7C, 0xD7,
    0x05, 0x96, 0xC7, 0xF5, 0xC1, 0x61, 0xE5, 0x18,
    0xA5, 0x38, 0x68, 0xED, 0xB4, 0x17, 0x62, 0x0D,
    0x01, 0x5E, 0xC3, 0x04, 0xA6, 0xBA, 0xB1, 0x01,
    0x60, 0x5C, 0xC1, 0x3A, 0x34, 0x97, 0xD6, 0xDB,
    0x67, 0x73, 0x4D, 0x33, 0x96, 0x01, 0x67, 0x44,
    0xEA, 0x47, 0x5E, 0x44, 0xB5, 0xE5, 0xD1, 0x6C,
    0x20, 0xA9, 0x6D, 0x4D, 0xBC, 0x02, 0xF0, 0x70,
    0xE4, 0xDD, 0xE9, 0xD5, 0x5C, 0x28, 0x29, 0x0B,
    0xB4, 0x60, 0x2A, 0xF1, 0xF7, 0x1A, 0xF0, 0x36,
    0xAE, 0x51, 0x3A, 0xAE, 0x6E, 0x48, 0x7D, 0xC7,
    0x5C, 0xF3, 0xDC, 0xF6, 0xED, 0x27, 0x4E, 0x8E,
    0x48, 0x18, 0x3E, 0x08, 0xF1, 0xD8, 0x3D, 0x0D,
    0xE7, 0x2F, 0x65, 0x8A, 0x6F, 0xE2, 0x1E, 0x06,
    0xC1, 0x04, 0x58, 0x7B, 0x4A, 0x75, 0x60, 0x92,
    0x13, 0xC6, 0x40, 0x2D, 0x3A, 0x8A, 0xD1, 0x03,
    0x05, 0x1F, 0x28, 0x66, 0xC2, 0x57, 0x2A, 0x4C,
    0xE1, 0xA3, 0xCB, 0xA1, 0x95, 0x30, 0x10, 0xED,
    0xDF, 0xAE, 0x70, 0x49, 0x4E, 0xF6, 0xB4, 0x5A,
    0xB6, 0x22, 0x56, 0x37, 0x05, 0xE7, 0x3E, 0xB2,
    0xE3, 0x96, 0x62, 0xEC, 0x09, 0x53, 0xC0, 0x50,
    0x3D, 0xA7, 0xBC, 0x9B, 0x39, 0x02, 0x26, 0x16,
    0xB5, 0x34, 0x17, 0xD4, 0xCA, 0xFE, 0x1D, 0xE4,
    0x5A, 0xDA, 0x4C, 0xC2, 0xCA, 0x8E, 0x79, 0xBF,
    0xD8, 0x4C, 0xBB, 0xFA, 0x30, 0x7B, 0xA9, 0x3E,
    0x52, 0x19, 0xB1, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00
};
#endif /* HAVE_PKCS7 && !NO_FILESYSTEM && ASN_BER_TO_DER && !NO_DES3 */

/*
 * Testing wc_PKCS7_BER()
 */
static void test_wc_PKCS7_BER(void)
{
#if defined(HAVE_PKCS7) && !defined(NO_FILESYSTEM) && \
    defined(ASN_BER_TO_DER)
    PKCS7* pkcs7;
    char   fName[] = "./certs/test-ber-exp02-05-2022.p7b";
    XFILE  f;
    byte   der[4096];
#ifndef NO_DES3
    byte   decoded[1024];
#endif
    word32 derSz;
    int    ret;

    printf(testingFmt, "wc_PKCS7_BER()");

    AssertNotNull(f = XFOPEN(fName, "rb"));
    AssertIntGT((ret = (int)fread(der, 1, sizeof(der), f)), 0);
    derSz = (word32)ret;
    XFCLOSE(f);

    AssertNotNull(pkcs7 = wc_PKCS7_New(HEAP_HINT, devId));
    AssertIntEQ(wc_PKCS7_Init(pkcs7, HEAP_HINT, INVALID_DEVID), 0);
    AssertIntEQ(wc_PKCS7_InitWithCert(pkcs7, NULL, 0), 0);
#ifndef NO_RSA
    AssertIntEQ(wc_PKCS7_VerifySignedData(pkcs7, der, derSz), 0);
#else
    AssertIntNE(wc_PKCS7_VerifySignedData(pkcs7, der, derSz), 0);
#endif
    wc_PKCS7_Free(pkcs7);

#ifndef NO_DES3
    /* decode BER content */
    AssertNotNull(f = XFOPEN("./certs/1024/client-cert.der", "rb"));
    AssertIntGT((ret = (int)fread(der, 1, sizeof(der), f)), 0);
    derSz = (word32)ret;
    XFCLOSE(f);
    AssertNotNull(pkcs7 = wc_PKCS7_New(HEAP_HINT, devId));
#ifndef NO_RSA
    AssertIntEQ(wc_PKCS7_InitWithCert(pkcs7, der, derSz), 0);
#else
    AssertIntNE(wc_PKCS7_InitWithCert(pkcs7, der, derSz), 0);
#endif

    AssertNotNull(f = XFOPEN("./certs/1024/client-key.der", "rb"));
    AssertIntGT((ret = (int)fread(der, 1, sizeof(der), f)), 0);
    derSz = (word32)ret;
    XFCLOSE(f);
    pkcs7->privateKey   = der;
    pkcs7->privateKeySz = derSz;
#ifndef NO_RSA
    AssertIntGT(wc_PKCS7_DecodeEnvelopedData(pkcs7, berContent,
        sizeof(berContent), decoded, sizeof(decoded)), 0);
#else
    AssertIntEQ(wc_PKCS7_DecodeEnvelopedData(pkcs7, berContent,
        sizeof(berContent), decoded, sizeof(decoded)), NOT_COMPILED_IN);
#endif
    wc_PKCS7_Free(pkcs7);
#endif /* !NO_DES3 */

    printf(resultFmt, passed);
#endif
} /* END test_wc_PKCS7_BER() */

static void test_PKCS7_signed_enveloped(void)
{
#if defined(HAVE_PKCS7) && !defined(NO_FILESYSTEM) && !defined(NO_RSA) \
    && !defined(NO_AES)
    XFILE  f;
    PKCS7* pkcs7;
    PKCS7* inner;
    void*  pt;
    WC_RNG rng;
    unsigned char key[FOURK_BUF/2];
    unsigned char cert[FOURK_BUF/2];
    unsigned char env[FOURK_BUF];
    int envSz  = FOURK_BUF;
    int keySz;
    int certSz;

    unsigned char sig[FOURK_BUF * 2];
    int sigSz = FOURK_BUF * 2;
    unsigned char decoded[FOURK_BUF];
    int decodedSz = FOURK_BUF;

    printf(testingFmt, "PKCS7_signed_enveloped");

    /* load cert */
    AssertNotNull(f = XFOPEN(cliCertDerFile, "rb"));
    AssertIntGT((certSz = (int)XFREAD(cert, 1, sizeof(cert), f)), 0);
    XFCLOSE(f);

    /* load key */
    AssertNotNull(f = XFOPEN(cliKeyFile, "rb"));
    AssertIntGT((keySz = (int)XFREAD(key, 1, sizeof(key), f)), 0);
    XFCLOSE(f);
    keySz = wolfSSL_KeyPemToDer(key, keySz, key, keySz, NULL);

    /* sign cert for envelope */
    AssertNotNull(pkcs7 = wc_PKCS7_New(NULL, 0));
    AssertIntEQ(wc_InitRng(&rng), 0);
    AssertIntEQ(wc_PKCS7_InitWithCert(pkcs7, cert, certSz), 0);
    pkcs7->content    = cert;
    pkcs7->contentSz  = certSz;
    pkcs7->contentOID = DATA;
    pkcs7->privateKey   = key;
    pkcs7->privateKeySz = keySz;
    pkcs7->encryptOID   = RSAk;
    pkcs7->hashOID      = SHA256h;
    pkcs7->rng          = &rng;
    AssertIntGT((sigSz = wc_PKCS7_EncodeSignedData(pkcs7, sig, sigSz)), 0);
    wc_PKCS7_Free(pkcs7);
    wc_FreeRng(&rng);

    /* create envelope */
    AssertNotNull(pkcs7 = wc_PKCS7_New(NULL, 0));
    AssertIntEQ(wc_PKCS7_InitWithCert(pkcs7, cert, certSz), 0);
    pkcs7->content   = sig;
    pkcs7->contentSz = sigSz;
    pkcs7->contentOID = DATA;
    pkcs7->encryptOID = AES256CBCb;
    pkcs7->privateKey   = key;
    pkcs7->privateKeySz = keySz;
    AssertIntGT((envSz = wc_PKCS7_EncodeEnvelopedData(pkcs7, env, envSz)), 0);
    wc_PKCS7_Free(pkcs7);

    /* create bad signed enveloped data */
    sigSz = FOURK_BUF * 2;
    AssertNotNull(pkcs7 = wc_PKCS7_New(NULL, 0));
    AssertIntEQ(wc_InitRng(&rng), 0);
    AssertIntEQ(wc_PKCS7_InitWithCert(pkcs7, cert, certSz), 0);
    pkcs7->content    = env;
    pkcs7->contentSz  = envSz;
    pkcs7->contentOID = DATA;
    pkcs7->privateKey   = key;
    pkcs7->privateKeySz = keySz;
    pkcs7->encryptOID   = RSAk;
    pkcs7->hashOID      = SHA256h;
    pkcs7->rng = &rng;

    /* Set no certs in bundle for this test. Hang on to the pointer though to
     * free it later. */
    pt = (void*)pkcs7->certList;
    pkcs7->certList = NULL; /* no certs in bundle */
    AssertIntGT((sigSz = wc_PKCS7_EncodeSignedData(pkcs7, sig, sigSz)), 0);
    pkcs7->certList = (Pkcs7Cert*)pt; /* restore pointer for PKCS7 free call */
    wc_PKCS7_Free(pkcs7);

    /* check verify fails */
    AssertNotNull(pkcs7 = wc_PKCS7_New(NULL, 0));
    AssertIntEQ(wc_PKCS7_InitWithCert(pkcs7, NULL, 0), 0);
    AssertIntEQ(wc_PKCS7_VerifySignedData(pkcs7, sig, sigSz),
            PKCS7_SIGNEEDS_CHECK);

    /* try verifying the signature manually */
    {
        RsaKey rKey;
        word32 idx = 0;
        byte digest[MAX_SEQ_SZ + MAX_ALGO_SZ + MAX_OCTET_STR_SZ +
            WC_MAX_DIGEST_SIZE];
        int  digestSz;

        AssertIntEQ(wc_InitRsaKey(&rKey, HEAP_HINT), 0);
        AssertIntEQ(wc_RsaPrivateKeyDecode(key, &idx, &rKey, keySz), 0);
        digestSz = wc_RsaSSL_Verify(pkcs7->signature, pkcs7->signatureSz,
                    digest, sizeof(digest), &rKey);
        AssertIntGT(digestSz, 0);
        AssertIntEQ(digestSz, pkcs7->pkcs7DigestSz);
        AssertIntEQ(XMEMCMP(digest, pkcs7->pkcs7Digest, digestSz), 0);
        AssertIntEQ(wc_FreeRsaKey(&rKey), 0);
        /* verify was success */
    }

    wc_PKCS7_Free(pkcs7);

    /* initializing the PKCS7 struct with the signing certificate should pass */
    AssertNotNull(pkcs7 = wc_PKCS7_New(NULL, 0));
    AssertIntEQ(wc_PKCS7_InitWithCert(pkcs7, cert, certSz), 0);
    AssertIntEQ(wc_PKCS7_VerifySignedData(pkcs7, sig, sigSz), 0);
    wc_PKCS7_Free(pkcs7);

    /* create valid degenerate bundle */
    sigSz = FOURK_BUF * 2;
    AssertNotNull(pkcs7 = wc_PKCS7_New(NULL, 0));
    pkcs7->content    = env;
    pkcs7->contentSz  = envSz;
    pkcs7->contentOID = DATA;
    pkcs7->privateKey   = key;
    pkcs7->privateKeySz = keySz;
    pkcs7->encryptOID   = RSAk;
    pkcs7->hashOID      = SHA256h;
    pkcs7->rng = &rng;
    AssertIntEQ(wc_PKCS7_SetSignerIdentifierType(pkcs7, DEGENERATE_SID), 0);
    AssertIntGT((sigSz = wc_PKCS7_EncodeSignedData(pkcs7, sig, sigSz)), 0);
    wc_PKCS7_Free(pkcs7);
    wc_FreeRng(&rng);

    /* check verify */
    AssertNotNull(pkcs7 = wc_PKCS7_New(NULL, 0));
    AssertIntEQ(wc_PKCS7_Init(pkcs7, HEAP_HINT, devId), 0);
    AssertIntEQ(wc_PKCS7_VerifySignedData(pkcs7, sig, sigSz), 0);
    AssertNotNull(pkcs7->content);

    /* check decode */
    AssertNotNull(inner = wc_PKCS7_New(NULL, 0));
    AssertIntEQ(wc_PKCS7_InitWithCert(inner, cert, certSz), 0);
    inner->privateKey   = key;
    inner->privateKeySz = keySz;
    AssertIntGT((decodedSz = wc_PKCS7_DecodeEnvelopedData(inner, pkcs7->content,
                   pkcs7->contentSz, decoded, decodedSz)), 0);
    wc_PKCS7_Free(inner);
    wc_PKCS7_Free(pkcs7);

    /* check cert set */
    AssertNotNull(pkcs7 = wc_PKCS7_New(NULL, 0));
    AssertIntEQ(wc_PKCS7_InitWithCert(pkcs7, NULL, 0), 0);
    AssertIntEQ(wc_PKCS7_VerifySignedData(pkcs7, decoded, decodedSz), 0);
    AssertNotNull(pkcs7->singleCert);
    AssertIntNE(pkcs7->singleCertSz, 0);
    wc_PKCS7_Free(pkcs7);

    printf(resultFmt, passed);

#endif
}

static void test_wc_i2d_PKCS12(void)
{
#if !defined(NO_ASN) && !defined(NO_PWDBASED) && defined(HAVE_PKCS12) \
    && !defined(NO_FILESYSTEM) && !defined(NO_RSA) \
    && !defined(NO_AES) && !defined(NO_DES3) && !defined(NO_SHA)
    WC_PKCS12* pkcs12 = NULL;
    unsigned char der[FOURK_BUF * 2];
    unsigned char* pt;
    int derSz;
    unsigned char out[FOURK_BUF * 2];
    int outSz = FOURK_BUF * 2;

    const char p12_f[] = "./certs/test-servercert.p12";
    XFILE f;

    printf(testingFmt, "wc_i2d_PKCS12");

    f =  XFOPEN(p12_f, "rb");
    AssertNotNull(f);
    derSz = (int)XFREAD(der, 1, sizeof(der), f);
    AssertIntGT(derSz, 0);
    XFCLOSE(f);

    AssertNotNull(pkcs12 = wc_PKCS12_new());
    AssertIntEQ(wc_d2i_PKCS12(der, derSz, pkcs12), 0);
    AssertIntEQ(wc_i2d_PKCS12(pkcs12, NULL, &outSz), LENGTH_ONLY_E);
    AssertIntEQ(outSz, derSz);

    outSz = derSz - 1;
    pt = out;
    AssertIntLE(wc_i2d_PKCS12(pkcs12, &pt, &outSz), 0);

    outSz = derSz;
    AssertIntEQ(wc_i2d_PKCS12(pkcs12, &pt, &outSz), derSz);
    AssertIntEQ((pt == out), 0);

    pt = NULL;
    AssertIntEQ(wc_i2d_PKCS12(pkcs12, &pt, NULL), derSz);
    XFREE(pt, NULL, DYNAMIC_TYPE_PKCS);
    wc_PKCS12_free(pkcs12);

    printf(resultFmt, passed);

#endif
}


/* Testing wc_SignatureGetSize() for signature type ECC */
static int test_wc_SignatureGetSize_ecc(void)
{
    int ret = 0;
    #ifndef NO_SIG_WRAPPER
    #if defined(HAVE_ECC) && !defined(NO_ECC256)
        enum wc_SignatureType sig_type;
        word32 key_len;

        /* Initialize ECC Key */
        ecc_key ecc;
        const char* qx =
            "fa2737fb93488d19caef11ae7faf6b7f4bcd67b286e3fc54e8a65c2b74aeccb0";
        const char* qy =
            "d4ccd6dae698208aa8c3a6f39e45510d03be09b2f124bfc067856c324f9b4d09";
        const char* d =
            "be34baa8d040a3b991f9075b56ba292f755b90e4b6dc10dad36715c33cfdac25";

        ret = wc_ecc_init(&ecc);
        if (ret == 0) {
            ret = wc_ecc_import_raw(&ecc, qx, qy, d, "SECP256R1");
        }
        printf(testingFmt, "wc_SigntureGetSize_ecc()");
        if (ret == 0) {
            /* Input for signature type ECC */
            sig_type = WC_SIGNATURE_TYPE_ECC;
            key_len = sizeof(ecc_key);
            ret = wc_SignatureGetSize(sig_type, &ecc, key_len);

            /* Test bad args */
            if (ret > 0) {
                sig_type = (enum wc_SignatureType) 100;
                ret = wc_SignatureGetSize(sig_type, &ecc, key_len);
                if (ret == BAD_FUNC_ARG) {
                    sig_type = WC_SIGNATURE_TYPE_ECC;
                    ret = wc_SignatureGetSize(sig_type, NULL, key_len);
                }
                if (ret >= 0) {
                    key_len = (word32) 0;
                    ret = wc_SignatureGetSize(sig_type, &ecc, key_len);
                }
                if (ret == BAD_FUNC_ARG) {
                    ret = SIG_TYPE_E;
                }
            }
        } else {
            ret = WOLFSSL_FATAL_ERROR;
        }
        wc_ecc_free(&ecc);
    #else
        ret = SIG_TYPE_E;
    #endif

    if (ret == SIG_TYPE_E) {
        ret = 0;
    }
    else {
        ret = WOLFSSL_FATAL_ERROR;
    }

    printf(resultFmt, ret == 0 ? passed : failed);
    #endif /* NO_SIG_WRAPPER */
    return ret;
}/* END test_wc_SignatureGetSize_ecc() */

/* Testing wc_SignatureGetSize() for signature type rsa */
static int test_wc_SignatureGetSize_rsa(void)
{
    int ret = 0;
    #ifndef NO_SIG_WRAPPER
    #ifndef NO_RSA
        enum wc_SignatureType sig_type;
        word32 key_len;
        word32 idx = 0;

        /* Initialize RSA Key */
        RsaKey rsa_key;
        byte* tmp = NULL;
        size_t bytes;

        #ifdef USE_CERT_BUFFERS_1024
            bytes = (size_t)sizeof_client_key_der_1024;
            if (bytes < (size_t)sizeof_client_key_der_1024)
                bytes = (size_t)sizeof_client_cert_der_1024;
        #elif defined(USE_CERT_BUFFERS_2048)
            bytes = (size_t)sizeof_client_key_der_2048;
            if (bytes < (size_t)sizeof_client_cert_der_2048)
                bytes = (size_t)sizeof_client_cert_der_2048;
        #else
            bytes = FOURK_BUF;
        #endif

        tmp = (byte*)XMALLOC(bytes, HEAP_HINT, DYNAMIC_TYPE_TMP_BUFFER);
        if (tmp != NULL) {
            #ifdef USE_CERT_BUFFERS_1024
                XMEMCPY(tmp, client_key_der_1024,
                    (size_t)sizeof_client_key_der_1024);
            #elif defined(USE_CERT_BUFFERS_2048)
                XMEMCPY(tmp, client_key_der_2048,
                    (size_t)sizeof_client_key_der_2048);
            #elif !defined(NO_FILESYSTEM)
                file = XFOPEN(clientKey, "rb");
                if (file != XBADFILE) {
                    bytes = (size_t)XFREAD(tmp, 1, FOURK_BUF, file);
                    XFCLOSE(file);
                }
                else {
                    ret = WOLFSSL_FATAL_ERROR;
                }
            #else
                ret = WOLFSSL_FATAL_ERROR;
            #endif
        } else {
            ret = WOLFSSL_FATAL_ERROR;
        }

        if (ret == 0) {
            ret = wc_InitRsaKey_ex(&rsa_key, HEAP_HINT, devId);
        }
        if (ret == 0) {
            ret = wc_RsaPrivateKeyDecode(tmp, &idx, &rsa_key, (word32)bytes);
        }

        printf(testingFmt, "wc_SigntureGetSize_rsa()");
        if (ret == 0) {
            /* Input for signature type RSA */
            sig_type = WC_SIGNATURE_TYPE_RSA;
            key_len = sizeof(RsaKey);
            ret = wc_SignatureGetSize(sig_type, &rsa_key, key_len);

            /* Test bad args */
            if (ret > 0) {
                sig_type = (enum wc_SignatureType) 100;
                ret = wc_SignatureGetSize(sig_type, &rsa_key, key_len);
                if (ret == BAD_FUNC_ARG) {
                    sig_type = WC_SIGNATURE_TYPE_RSA;
                    ret = wc_SignatureGetSize(sig_type, NULL, key_len);
                }
            #ifndef HAVE_USER_RSA
                if (ret == BAD_FUNC_ARG) {
            #else
                if (ret == 0) {
            #endif
                    key_len = (word32)0;
                    ret = wc_SignatureGetSize(sig_type, &rsa_key, key_len);
                }
                if (ret == BAD_FUNC_ARG) {
                    ret = SIG_TYPE_E;
                }
            }
        } else {
            ret = WOLFSSL_FATAL_ERROR;
        }
        wc_FreeRsaKey(&rsa_key);
        XFREE(tmp, HEAP_HINT, DYNAMIC_TYPE_TMP_BUFFER);
    #else
        ret = SIG_TYPE_E;
    #endif

    if (ret == SIG_TYPE_E) {
        ret = 0;
    }else {
        ret = WOLFSSL_FATAL_ERROR;
    }

   printf(resultFmt, ret == 0 ? passed : failed);
   #endif /* NO_SIG_WRAPPER */
   return ret;
}/* END test_wc_SignatureGetSize_rsa(void) */

/*----------------------------------------------------------------------------*
 | hash.h Tests
 *----------------------------------------------------------------------------*/

static int test_wc_HashInit(void)
{
    int ret = 0, i;  /* 0 indicates tests passed, 1 indicates failure */

    wc_HashAlg hash;

    /* enum for holding supported algorithms, #ifndef's restrict if disabled */
    enum wc_HashType enumArray[] = {
    #ifndef NO_MD5
            WC_HASH_TYPE_MD5,
    #endif
    #ifndef NO_SHA
            WC_HASH_TYPE_SHA,
    #endif
    #ifndef WOLFSSL_SHA224
            WC_HASH_TYPE_SHA224,
    #endif
    #ifndef NO_SHA256
            WC_HASH_TYPE_SHA256,
    #endif
    #ifndef WOLFSSL_SHA384
            WC_HASH_TYPE_SHA384,
    #endif
    #ifndef WOLFSSL_SHA512
            WC_HASH_TYPE_SHA512,
    #endif
    };
    /* dynamically finds the length */
    int enumlen = (sizeof(enumArray)/sizeof(enum wc_HashType));

    /* For loop to test various arguments... */
    for (i = 0; i < enumlen; i++) {
        /* check for bad args */
        if (wc_HashInit(&hash, enumArray[i]) == BAD_FUNC_ARG) {
            ret = 1;
            break;
        }
        wc_HashFree(&hash, enumArray[i]);

        /* check for null ptr */
        if (wc_HashInit(NULL, enumArray[i]) != BAD_FUNC_ARG) {
            ret = 1;
            break;
        }

    }  /* end of for loop */

    printf(testingFmt, "wc_HashInit()");
    if (ret==0) {  /* all tests have passed */
        printf(resultFmt, passed);
    }
    else {  /* a test has failed */
        printf(resultFmt, failed);
    }
    return ret;
}  /* end of test_wc_HashInit */

/*----------------------------------------------------------------------------*
 | Compatibility Tests
 *----------------------------------------------------------------------------*/

static void test_wolfSSL_X509_NAME(void)
{
    #if defined(OPENSSL_EXTRA) && !defined(NO_CERTS) && !defined(NO_FILESYSTEM) \
        && !defined(NO_RSA) && defined(WOLFSSL_CERT_GEN)
    X509* x509;
    const unsigned char* c;
    unsigned char buf[4096];
    int bytes;
    XFILE f;
    const X509_NAME* a;
    const X509_NAME* b;
    int sz;
    unsigned char* tmp;
    char file[] = "./certs/ca-cert.der";

    printf(testingFmt, "wolfSSL_X509_NAME()");

    /* test compile of depricated function, returns 0 */
    AssertIntEQ(CRYPTO_thread_id(), 0);

    AssertNotNull(a = X509_NAME_new());
    X509_NAME_free((X509_NAME*)a);

    f = XFOPEN(file, "rb");
    AssertTrue(f != XBADFILE);
    bytes = (int)XFREAD(buf, 1, sizeof(buf), f);
    XFCLOSE(f);

    c = buf;
    AssertNotNull(x509 = wolfSSL_X509_load_certificate_buffer(c, bytes,
                SSL_FILETYPE_ASN1));

    /* test cmp function */
    AssertNotNull(a = X509_get_issuer_name(x509));
    AssertNotNull(b = X509_get_subject_name(x509));

    AssertIntEQ(X509_NAME_cmp(a, b), 0); /* self signed should be 0 */

    tmp = buf;
    AssertIntGT((sz = i2d_X509_NAME((X509_NAME*)a, &tmp)), 0);
    if (sz > 0 && tmp == buf) {
        printf("\nERROR - %s line %d failed with:", __FILE__, __LINE__);           \
        printf(" Expected pointer to be incremented\n");
        abort();
    }

    /* retry but with the function creating a buffer */
    tmp = NULL;
    AssertIntGT((sz = i2d_X509_NAME((X509_NAME*)b, &tmp)), 0);
    XFREE(tmp, NULL, DYNAMIC_TYPE_OPENSSL);


    AssertNotNull(b = X509_NAME_dup((X509_NAME*)a));
    AssertIntEQ(X509_NAME_cmp(a, b), 0);
    X509_NAME_free((X509_NAME*)b);

    X509_free(x509);

    printf(resultFmt, passed);
    #endif /* defined(OPENSSL_EXTRA) && !defined(NO_DES3) */
}

static void test_wolfSSL_X509_INFO(void)
{
#if defined(OPENSSL_ALL)
    STACK_OF(X509_INFO) *info_stack;
    X509_INFO *info;
    BIO *cert;
    int i;

    printf(testingFmt, "wolfSSL_X509_INFO");

    AssertNotNull(cert = BIO_new_file(cliCertFileExt, "r"));
    AssertNotNull(info_stack = PEM_X509_INFO_read_bio(cert, NULL, NULL, NULL));
    for (i = 0; i < sk_X509_INFO_num(info_stack); i++) {
        AssertNotNull(info = sk_X509_INFO_value(info_stack, i));
        AssertNotNull(info->x509);
        AssertNull(info->crl);
    }
    sk_X509_INFO_pop_free(info_stack, X509_INFO_free);
    BIO_free(cert);

    AssertNotNull(cert = BIO_new_file(cliCertFileExt, "r"));
    AssertNotNull(info_stack = PEM_X509_INFO_read_bio(cert, NULL, NULL, NULL));
    sk_X509_INFO_free(info_stack);
    BIO_free(cert);

    printf(resultFmt, passed);
#endif
}

static void test_wolfSSL_X509_subject_name_hash(void)
{
#if defined(OPENSSL_EXTRA) && !defined(NO_CERTS) && !defined(NO_FILESYSTEM) \
    && !defined(NO_SHA) && !defined(NO_RSA)

    X509* x509;
    X509_NAME* subjectName = NULL;
    unsigned long ret = 0;

    printf(testingFmt, "wolfSSL_X509_subject_name_hash()");

    AssertNotNull(x509 = wolfSSL_X509_load_certificate_file(cliCertFile,
                SSL_FILETYPE_PEM));

    AssertNotNull(subjectName = wolfSSL_X509_get_subject_name(x509));

    ret = X509_subject_name_hash(x509);

    AssertIntNE(ret, WOLFSSL_FAILURE);

    X509_free(x509);
    printf(resultFmt, passed);

#endif
}

static void test_wolfSSL_DES(void)
{
    #if defined(OPENSSL_EXTRA) && !defined(NO_DES3)
    const_DES_cblock myDes;
    DES_cblock iv;
    DES_key_schedule key;
    word32 i;
    DES_LONG dl;
    unsigned char msg[] = "hello wolfssl";

    printf(testingFmt, "wolfSSL_DES()");

    DES_check_key(1);
    DES_set_key(&myDes, &key);

    /* check, check of odd parity */
    XMEMSET(myDes, 4, sizeof(const_DES_cblock));  myDes[0] = 6; /*set even parity*/
    XMEMSET(key, 5, sizeof(DES_key_schedule));
    AssertIntEQ(DES_set_key_checked(&myDes, &key), -1);
    AssertIntNE(key[0], myDes[0]); /* should not have copied over key */

    /* set odd parity for success case */
    DES_set_odd_parity(&myDes);
    AssertIntEQ(DES_check_key_parity(&myDes), 1);
    printf("%02x %02x %02x %02x", myDes[0], myDes[1], myDes[2], myDes[3]);
    AssertIntEQ(DES_set_key_checked(&myDes, &key), 0);
    for (i = 0; i < sizeof(DES_key_schedule); i++) {
        AssertIntEQ(key[i], myDes[i]);
    }
    AssertIntEQ(DES_is_weak_key(&myDes), 0);

    /* check weak key */
    XMEMSET(myDes, 1, sizeof(const_DES_cblock));
    XMEMSET(key, 5, sizeof(DES_key_schedule));
    AssertIntEQ(DES_set_key_checked(&myDes, &key), -2);
    AssertIntNE(key[0], myDes[0]); /* should not have copied over key */

    /* now do unchecked copy of a weak key over */
    DES_set_key_unchecked(&myDes, &key);
    /* compare arrays, should be the same */
    for (i = 0; i < sizeof(DES_key_schedule); i++) {
        AssertIntEQ(key[i], myDes[i]);
    }
    AssertIntEQ(DES_is_weak_key(&myDes), 1);

    /* check DES_key_sched API */
    XMEMSET(key, 1, sizeof(DES_key_schedule));
    AssertIntEQ(DES_key_sched(&myDes, NULL), 0);
    AssertIntEQ(DES_key_sched(NULL, &key),   0);
    AssertIntEQ(DES_key_sched(&myDes, &key), 0);
    /* compare arrays, should be the same */
    for (i = 0; i < sizeof(DES_key_schedule); i++) {
        AssertIntEQ(key[i], myDes[i]);
    }

    /* DES_cbc_cksum should return the last 4 of the last 8 bytes after
     * DES_cbc_encrypt on the input */
    XMEMSET(iv, 0, sizeof(DES_cblock));
    XMEMSET(myDes, 5, sizeof(DES_key_schedule));
    AssertIntGT((dl = DES_cbc_cksum(msg, &key, sizeof(msg), &myDes, &iv)), 0);
    AssertIntEQ(dl, 480052723);

    printf(resultFmt, passed);
    #endif /* defined(OPENSSL_EXTRA) && !defined(NO_DES3) */
}

static void test_wc_PemToDer(void)
{
#if !defined(NO_CERTS) && defined(WOLFSSL_PEM_TO_DER)
    int ret;
    DerBuffer* pDer = NULL;
    const char* ca_cert = "./certs/server-cert.pem";
    byte* cert_buf = NULL;
    size_t cert_sz = 0;
    int eccKey = 0;
    EncryptedInfo info;

    printf(testingFmt, "wc_PemToDer()");

    memset(&info, 0, sizeof(info));

    ret = load_file(ca_cert, &cert_buf, &cert_sz);
    if (ret == 0) {
        ret = wc_PemToDer(cert_buf, cert_sz, CERT_TYPE,
            &pDer, NULL, &info, &eccKey);
        AssertIntEQ(ret, 0);

        wc_FreeDer(&pDer);
    }

    if (cert_buf)
        free(cert_buf);
    printf(resultFmt, passed);
#endif
}

static void test_wc_AllocDer(void)
{
#if !defined(NO_CERTS)
    int ret;
    DerBuffer* pDer = NULL;
    word32 testSize = 1024;

    printf(testingFmt, "wc_AllocDer()");

    ret = wc_AllocDer(&pDer, testSize, CERT_TYPE, HEAP_HINT);
    AssertIntEQ(ret, 0);
    AssertNotNull(pDer);
    wc_FreeDer(&pDer);
    printf(resultFmt, passed);
#endif
}

static void test_wc_CertPemToDer(void)
{
#if !defined(NO_CERTS) && defined(WOLFSSL_PEM_TO_DER)
    int ret;
    const char* ca_cert = "./certs/ca-cert.pem";
    byte* cert_buf = NULL;
    size_t cert_sz = 0, cert_dersz = 0;
    byte* cert_der = NULL;

    printf(testingFmt, "wc_CertPemToDer()");

    ret = load_file(ca_cert, &cert_buf, &cert_sz);
    if (ret == 0) {
        cert_dersz = cert_sz; /* DER will be smaller than PEM */
        cert_der = (byte*)malloc(cert_dersz);
        if (cert_der) {
            ret = wc_CertPemToDer(cert_buf, (int)cert_sz,
                cert_der, (int)cert_dersz, CERT_TYPE);
            AssertIntGE(ret, 0);
        }
    }

    if (cert_der)
        free(cert_der);
    if (cert_buf)
        free(cert_buf);
#endif
}

static void test_wc_PubKeyPemToDer(void)
{
#ifdef WOLFSSL_PEM_TO_DER
#if defined(WOLFSSL_CERT_EXT) || defined(WOLFSSL_PUB_PEM_TO_DER)
    int ret;
    const char* key = "./certs/ecc-client-keyPub.pem";
    byte* cert_buf = NULL;
    size_t cert_sz = 0, cert_dersz = 0;
    byte* cert_der = NULL;

    printf(testingFmt, "wc_PubKeyPemToDer()");


    ret = wc_PubKeyPemToDer(cert_buf, (int)cert_sz,
        cert_der, (int)cert_dersz);
    AssertIntGE(ret, BAD_FUNC_ARG);

    ret = load_file(key, &cert_buf, &cert_sz);
    if (ret == 0) {
        cert_dersz = cert_sz; /* DER will be smaller than PEM */
        cert_der = (byte*)malloc(cert_dersz);
        if (cert_der) {
            ret = wc_PubKeyPemToDer(cert_buf, (int)cert_sz,
                cert_der, (int)cert_dersz);
            AssertIntGE(ret, 0);
        }
    }

    if (cert_der)
        free(cert_der);
    if (cert_buf)
        free(cert_buf);
#endif
#endif
}

static void test_wc_PemPubKeyToDer(void)
{
#if defined(WOLFSSL_CERT_EXT) || defined(WOLFSSL_PUB_PEM_TO_DER)
    int ret;
    const char* key = "./certs/ecc-client-keyPub.pem";
    size_t cert_dersz = 1024;
    byte* cert_der = (byte*)malloc(cert_dersz);

    printf(testingFmt, "wc_PemPubKeyToDer()");

    ret = wc_PemPubKeyToDer(NULL, cert_der, (int)cert_dersz);
    AssertIntGE(ret, BAD_FUNC_ARG);

    if (cert_der) {
        ret = wc_PemPubKeyToDer(key, cert_der, (int)cert_dersz);
        AssertIntGE(ret, 0);

        free(cert_der);
    }
#endif
}


static void test_wolfSSL_certs(void)
{
#if defined(OPENSSL_EXTRA) && !defined(NO_CERTS) && !defined(NO_FILESYSTEM) && \
    !defined(NO_RSA)
    X509*  x509;
    WOLFSSL*     ssl;
    WOLFSSL_CTX* ctx;
    STACK_OF(ASN1_OBJECT)* sk;
    ASN1_BIT_STRING* bit_str;
    int crit;

    printf(testingFmt, "wolfSSL_certs()");

#ifndef NO_WOLFSSL_SERVER
    AssertNotNull(ctx = SSL_CTX_new(SSLv23_server_method()));
#else
    AssertNotNull(ctx = SSL_CTX_new(SSLv23_client_method()));
#endif
    AssertTrue(SSL_CTX_use_certificate_file(ctx, svrCertFile, SSL_FILETYPE_PEM));
    AssertTrue(SSL_CTX_use_PrivateKey_file(ctx, svrKeyFile, SSL_FILETYPE_PEM));
    #ifndef HAVE_USER_RSA
    AssertTrue(SSL_CTX_use_PrivateKey_file(ctx, cliKeyFile, SSL_FILETYPE_PEM));
    AssertIntEQ(SSL_CTX_check_private_key(ctx), SSL_FAILURE);
    AssertTrue(SSL_CTX_use_PrivateKey_file(ctx, svrKeyFile, SSL_FILETYPE_PEM));
    AssertIntEQ(SSL_CTX_check_private_key(ctx), SSL_SUCCESS);
    #endif
    AssertNotNull(ssl = SSL_new(ctx));

    AssertIntEQ(wolfSSL_check_private_key(ssl), WOLFSSL_SUCCESS);

    #ifdef HAVE_PK_CALLBACKS
    AssertIntEQ((int)SSL_set_tlsext_debug_arg(ssl, NULL), WOLFSSL_SUCCESS);
    #endif /* HAVE_PK_CALLBACKS */

    /* create and use x509 */
    x509 = wolfSSL_X509_load_certificate_file(cliCertFileExt, WOLFSSL_FILETYPE_PEM);
    AssertNotNull(x509);
    AssertIntEQ(SSL_use_certificate(ssl, x509), WOLFSSL_SUCCESS);

    #ifndef HAVE_USER_RSA
    /* with loading in a new cert the check on private key should now fail */
    AssertIntNE(wolfSSL_check_private_key(ssl), WOLFSSL_SUCCESS);
    #endif


    #if defined(USE_CERT_BUFFERS_2048)
        AssertIntEQ(SSL_use_certificate_ASN1(ssl,
                                  (unsigned char*)server_cert_der_2048,
                                  sizeof_server_cert_der_2048), WOLFSSL_SUCCESS);
    #endif

    #if !defined(NO_SHA) && !defined(NO_SHA256)
    /************* Get Digest of Certificate ******************/
    {
        byte   digest[64]; /* max digest size */
        word32 digestSz;

        XMEMSET(digest, 0, sizeof(digest));
        AssertIntEQ(X509_digest(x509, wolfSSL_EVP_sha1(), digest, &digestSz),
                    WOLFSSL_SUCCESS);
        AssertIntEQ(X509_digest(x509, wolfSSL_EVP_sha256(), digest, &digestSz),
                    WOLFSSL_SUCCESS);

        AssertIntEQ(X509_digest(NULL, wolfSSL_EVP_sha1(), digest, &digestSz),
                    WOLFSSL_FAILURE);
    }
    #endif /* !NO_SHA && !NO_SHA256*/

    /* test and checkout X509 extensions */
    sk = (STACK_OF(ASN1_OBJECT)*)X509_get_ext_d2i(x509, NID_basic_constraints,
            &crit, NULL);
    AssertNotNull(sk);
    AssertIntEQ(crit, 0);
    sk_ASN1_OBJECT_free(sk);

    bit_str = (ASN1_BIT_STRING*)X509_get_ext_d2i(x509, NID_key_usage, &crit, NULL);
    AssertNotNull(bit_str);
    AssertIntEQ(crit, 1);
    AssertIntEQ(bit_str->type, NID_key_usage);
    ASN1_BIT_STRING_free(bit_str);

    sk = (STACK_OF(ASN1_OBJECT)*)X509_get_ext_d2i(x509, NID_ext_key_usage,
            &crit, NULL);
    /* AssertNotNull(sk); no extension set */
    sk_ASN1_OBJECT_free(sk);

    sk = (STACK_OF(ASN1_OBJECT)*)X509_get_ext_d2i(x509,
            NID_authority_key_identifier, &crit, NULL);
    AssertNotNull(sk);
    sk_ASN1_OBJECT_free(sk);

    sk = (STACK_OF(ASN1_OBJECT)*)X509_get_ext_d2i(x509,
            NID_private_key_usage_period, &crit, NULL);
    /* AssertNotNull(sk); NID not yet supported */
    AssertIntEQ(crit, -1);
    sk_ASN1_OBJECT_free(sk);

    sk = (STACK_OF(ASN1_OBJECT)*)X509_get_ext_d2i(x509, NID_subject_alt_name,
            &crit, NULL);
    /* AssertNotNull(sk); no alt names set */
    sk_GENERAL_NAME_free(sk);

    sk = (STACK_OF(ASN1_OBJECT)*)X509_get_ext_d2i(x509, NID_issuer_alt_name,
            &crit, NULL);
    /* AssertNotNull(sk); NID not yet supported */
    AssertIntEQ(crit, -1);
    sk_ASN1_OBJECT_free(sk);

    sk = (STACK_OF(ASN1_OBJECT)*)X509_get_ext_d2i(x509, NID_info_access, &crit,
            NULL);
    /* AssertNotNull(sk); no auth info set */
    sk_ASN1_OBJECT_free(sk);

    sk = (STACK_OF(ASN1_OBJECT)*)X509_get_ext_d2i(x509, NID_sinfo_access,
            &crit, NULL);
    /* AssertNotNull(sk); NID not yet supported */
    AssertIntEQ(crit, -1);
    sk_ASN1_OBJECT_free(sk);

    sk = (STACK_OF(ASN1_OBJECT)*)X509_get_ext_d2i(x509, NID_name_constraints,
            &crit, NULL);
    /* AssertNotNull(sk); NID not yet supported */
    AssertIntEQ(crit, -1);
    sk_ASN1_OBJECT_free(sk);

    sk = (STACK_OF(ASN1_OBJECT)*)X509_get_ext_d2i(x509,
            NID_certificate_policies, &crit, NULL);
    #if !defined(WOLFSSL_SEP) && !defined(WOLFSSL_CERT_EXT)
        AssertNull(sk);
    #else
        /* AssertNotNull(sk); no cert policy set */
    #endif
    sk_ASN1_OBJECT_free(sk);

    sk = (STACK_OF(ASN1_OBJECT)*)X509_get_ext_d2i(x509, NID_policy_mappings,
            &crit, NULL);
    /* AssertNotNull(sk); NID not yet supported */
    AssertIntEQ(crit, -1);
    sk_ASN1_OBJECT_free(sk);

    sk = (STACK_OF(ASN1_OBJECT)*)X509_get_ext_d2i(x509, NID_policy_constraints,
            &crit, NULL);
    /* AssertNotNull(sk); NID not yet supported */
    AssertIntEQ(crit, -1);
    sk_ASN1_OBJECT_free(sk);

    sk = (STACK_OF(ASN1_OBJECT)*)X509_get_ext_d2i(x509, NID_inhibit_any_policy,
            &crit, NULL);
    /* AssertNotNull(sk); NID not yet supported */
    AssertIntEQ(crit, -1);
    sk_ASN1_OBJECT_free(sk);

    sk = (STACK_OF(ASN1_OBJECT)*)X509_get_ext_d2i(x509, NID_tlsfeature, &crit,
            NULL);
    /* AssertNotNull(sk); NID not yet supported */
    AssertIntEQ(crit, -1);
    sk_ASN1_OBJECT_free(sk);

    /* test invalid cases */
    crit = 0;
    sk = (STACK_OF(ASN1_OBJECT)*)X509_get_ext_d2i(x509, -1, &crit, NULL);
    AssertNull(sk);
    AssertIntEQ(crit, -1);
    sk = (STACK_OF(ASN1_OBJECT)*)X509_get_ext_d2i(NULL, NID_tlsfeature,
            NULL, NULL);
    AssertNull(sk);

    AssertIntEQ(SSL_get_hit(ssl), 0);
    X509_free(x509);
    SSL_free(ssl);
    SSL_CTX_free(ctx);

    printf(resultFmt, passed);
#endif /* OPENSSL_EXTRA && !NO_CERTS */
}


static void test_wolfSSL_ASN1_TIME_print(void)
{
    #if defined(OPENSSL_EXTRA) && !defined(NO_CERTS) && !defined(NO_RSA) \
        && (defined(WOLFSSL_MYSQL_COMPATIBLE) || defined(WOLFSSL_NGINX) || \
            defined(WOLFSSL_HAPROXY)) && defined(USE_CERT_BUFFERS_2048)
    BIO*  bio;
    X509*  x509;
    const unsigned char* der = client_cert_der_2048;
    ASN1_TIME* t;
    unsigned char buf[25];

    printf(testingFmt, "wolfSSL_ASN1_TIME_print()");

    AssertNotNull(bio = BIO_new(BIO_s_mem()));
    AssertNotNull(x509 = wolfSSL_X509_load_certificate_buffer(der,
                sizeof_client_cert_der_2048, WOLFSSL_FILETYPE_ASN1));
    AssertIntEQ(ASN1_TIME_print(bio, X509_get_notBefore(x509)), 1);
    AssertIntEQ(BIO_read(bio, buf, sizeof(buf)), 24);
    AssertIntEQ(XMEMCMP(buf, "Apr 13 15:23:09 2018 GMT", sizeof(buf) - 1), 0);

    /* create a bad time and test results */
    AssertNotNull(t = X509_get_notAfter(x509));
    AssertIntEQ(ASN1_TIME_check(t), WOLFSSL_SUCCESS);
    t->data[8] = 0;
    t->data[3]  = 0;
    AssertIntNE(ASN1_TIME_print(bio, t), 1);
    AssertIntEQ(BIO_read(bio, buf, sizeof(buf)), 14);
    AssertIntEQ(XMEMCMP(buf, "Bad time value", 14), 0);
    AssertIntEQ(ASN1_TIME_check(t), WOLFSSL_FAILURE);

    BIO_free(bio);
    X509_free(x509);

    printf(resultFmt, passed);
    #endif
}

static void test_wolfSSL_ASN1_UTCTIME_print(void)
{
    #if defined(OPENSSL_EXTRA)
    BIO*  bio;
    ASN1_UTCTIME* utc = NULL;
    unsigned char buf[25];
    const char* validDate   = "190424111501Z"; /* UTC = YYMMDDHHMMSSZ */
    const char* invalidDate = "190424111501X"; /* UTC = YYMMDDHHMMSSZ */

    printf(testingFmt, "ASN1_UTCTIME_print()");

    /* NULL parameter check */
    AssertNotNull(bio = BIO_new(BIO_s_mem()));
    AssertIntEQ(ASN1_UTCTIME_print(bio, utc), 0);
    BIO_free(bio);

    /* Valid date */
    AssertNotNull(bio = BIO_new(BIO_s_mem()));
    AssertNotNull(utc = (ASN1_UTCTIME*)XMALLOC(sizeof(ASN1_UTCTIME), NULL,
                                                           DYNAMIC_TYPE_ASN1));
    utc->type = ASN_UTC_TIME;
    utc->length = ASN_UTC_TIME_SIZE;
    XMEMCPY(utc->data, (byte*)validDate, ASN_UTC_TIME_SIZE);
    AssertIntEQ(ASN1_UTCTIME_print(bio, utc), 1);
    AssertIntEQ(BIO_read(bio, buf, sizeof(buf)), 24);
    AssertIntEQ(XMEMCMP(buf, "Apr 24 11:15:01 2019 GMT", sizeof(buf)-1), 0);

    XMEMSET(buf, 0, sizeof(buf));
    BIO_free(bio);

    /* Invalid format */
    AssertNotNull(bio = BIO_new(BIO_s_mem()));
    utc->type = ASN_UTC_TIME;
    utc->length = ASN_UTC_TIME_SIZE;
    XMEMCPY(utc->data, (byte*)invalidDate, ASN_UTC_TIME_SIZE);
    AssertIntEQ(ASN1_UTCTIME_print(bio, utc), 0);
    AssertIntEQ(BIO_read(bio, buf, sizeof(buf)), 14);
    AssertIntEQ(XMEMCMP(buf, "Bad time value", 14), 0);

    XFREE(utc, NULL, DYNAMIC_TYPE_ASN1);
    BIO_free(bio);

    printf(resultFmt, passed);
    #endif /* OPENSSL_EXTRA */
}


static void test_wolfSSL_ASN1_GENERALIZEDTIME_free(void)
{
    #if defined(OPENSSL_EXTRA)
    WOLFSSL_ASN1_GENERALIZEDTIME* asn1_gtime;
    unsigned char nullstr[32];

    XMEMSET(nullstr, 0, 32);
    asn1_gtime = (WOLFSSL_ASN1_GENERALIZEDTIME*)XMALLOC(
                    sizeof(WOLFSSL_ASN1_GENERALIZEDTIME), NULL,
                    DYNAMIC_TYPE_TMP_BUFFER);
    if (asn1_gtime) {
        XMEMCPY(asn1_gtime->data,"20180504123500Z",ASN_GENERALIZED_TIME_SIZE);

        wolfSSL_ASN1_GENERALIZEDTIME_free(asn1_gtime);
        AssertIntEQ(0, XMEMCMP(asn1_gtime->data, nullstr, 32));

        XFREE(asn1_gtime, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    }
    #endif /* OPENSSL_EXTRA */
}


static void test_wolfSSL_private_keys(void)
{
    #if defined(OPENSSL_EXTRA) && !defined(NO_CERTS) && \
       !defined(NO_FILESYSTEM)
    WOLFSSL*     ssl;
    WOLFSSL_CTX* ctx;
    EVP_PKEY* pkey = NULL;

    printf(testingFmt, "wolfSSL_private_keys()");

    OpenSSL_add_all_digests();
    OpenSSL_add_all_algorithms();

#ifndef NO_RSA
    #ifndef NO_WOLFSSL_SERVER
    AssertNotNull(ctx = SSL_CTX_new(wolfSSLv23_server_method()));
    #else
    AssertNotNull(ctx = SSL_CTX_new(wolfSSLv23_client_method()));
    #endif
    AssertTrue(SSL_CTX_use_certificate_file(ctx, svrCertFile, WOLFSSL_FILETYPE_PEM));
    AssertTrue(SSL_CTX_use_PrivateKey_file(ctx, svrKeyFile, WOLFSSL_FILETYPE_PEM));
    AssertNotNull(ssl = SSL_new(ctx));

    AssertIntEQ(wolfSSL_check_private_key(ssl), WOLFSSL_SUCCESS);

#ifdef USE_CERT_BUFFERS_2048
    {
    const unsigned char* server_key = (const unsigned char*)server_key_der_2048;
    unsigned char buf[FOURK_BUF];
    word32 bufSz;

    AssertIntEQ(SSL_use_RSAPrivateKey_ASN1(ssl,
                (unsigned char*)client_key_der_2048,
                sizeof_client_key_der_2048), WOLFSSL_SUCCESS);
#ifndef HAVE_USER_RSA
    /* Should mismatch now that a different private key loaded */
    AssertIntNE(wolfSSL_check_private_key(ssl), WOLFSSL_SUCCESS);
#endif

    AssertIntEQ(SSL_use_PrivateKey_ASN1(0, ssl,
                (unsigned char*)server_key,
                sizeof_server_key_der_2048), WOLFSSL_SUCCESS);
    /* After loading back in DER format of original key, should match */
    AssertIntEQ(wolfSSL_check_private_key(ssl), WOLFSSL_SUCCESS);

    /* test loading private key to the WOLFSSL_CTX */
    AssertIntEQ(SSL_CTX_use_PrivateKey_ASN1(0, ctx,
                (unsigned char*)client_key_der_2048,
                sizeof_client_key_der_2048), WOLFSSL_SUCCESS);
#ifndef NO_CHECK_PRIVATE_KEY
#ifndef HAVE_USER_RSA
    /* Should mismatch now that a different private key loaded */
    AssertIntNE(wolfSSL_CTX_check_private_key(ctx), WOLFSSL_SUCCESS);
#endif

    AssertIntEQ(SSL_CTX_use_PrivateKey_ASN1(0, ctx,
                (unsigned char*)server_key,
                sizeof_server_key_der_2048), WOLFSSL_SUCCESS);
    /* After loading back in DER format of original key, should match */
    AssertIntEQ(wolfSSL_CTX_check_private_key(ctx), WOLFSSL_SUCCESS);
#endif /* !NO_CHECK_PRIVATE_KEY */

    /* pkey not set yet, expecting to fail */
    AssertIntEQ(SSL_use_PrivateKey(ssl, pkey), WOLFSSL_FAILURE);

    /* set PKEY and test again */
    AssertNotNull(wolfSSL_d2i_PrivateKey(EVP_PKEY_RSA, &pkey,
                &server_key, (long)sizeof_server_key_der_2048));
    AssertIntEQ(SSL_use_PrivateKey(ssl, pkey), WOLFSSL_SUCCESS);

    /* reuse PKEY structure and test
     * this should be checked with a memory management sanity checker */
    AssertFalse(server_key == (const unsigned char*)server_key_der_2048);
    server_key = (const unsigned char*)server_key_der_2048;
    AssertNotNull(wolfSSL_d2i_PrivateKey(EVP_PKEY_RSA, &pkey,
                &server_key, (long)sizeof_server_key_der_2048));
    AssertIntEQ(SSL_use_PrivateKey(ssl, pkey), WOLFSSL_SUCCESS);

    /* check striping PKCS8 header with wolfSSL_d2i_PrivateKey */
    bufSz = FOURK_BUF;
    AssertIntGT((bufSz = wc_CreatePKCS8Key(buf, &bufSz,
                    (byte*)server_key_der_2048, sizeof_server_key_der_2048,
                    RSAk, NULL, 0)), 0);
    server_key = (const unsigned char*)buf;
    AssertNotNull(wolfSSL_d2i_PrivateKey(EVP_PKEY_RSA, &pkey, &server_key,
                (long)bufSz));
    }
#endif


    EVP_PKEY_free(pkey);
    SSL_free(ssl); /* frees x509 also since loaded into ssl */
    SSL_CTX_free(ctx);
#endif /* end of RSA private key match tests */


#ifdef HAVE_ECC
    #ifndef NO_WOLFSSL_SERVER
    AssertNotNull(ctx = SSL_CTX_new(wolfSSLv23_server_method()));
    #else
    AssertNotNull(ctx = SSL_CTX_new(wolfSSLv23_client_method()));
    #endif
    AssertTrue(SSL_CTX_use_certificate_file(ctx, eccCertFile,
                                                         WOLFSSL_FILETYPE_PEM));
    AssertTrue(SSL_CTX_use_PrivateKey_file(ctx, eccKeyFile,
                                                         WOLFSSL_FILETYPE_PEM));
    AssertNotNull(ssl = SSL_new(ctx));

    AssertIntEQ(wolfSSL_check_private_key(ssl), WOLFSSL_SUCCESS);
    SSL_free(ssl);


    AssertTrue(SSL_CTX_use_PrivateKey_file(ctx, cliEccKeyFile,
                                                         WOLFSSL_FILETYPE_PEM));
    AssertNotNull(ssl = SSL_new(ctx));

    AssertIntNE(wolfSSL_check_private_key(ssl), WOLFSSL_SUCCESS);

    SSL_free(ssl);
    SSL_CTX_free(ctx);
#endif /* end of ECC private key match tests */

#ifdef HAVE_ED25519
    #ifndef NO_WOLFSSL_SERVER
    AssertNotNull(ctx = SSL_CTX_new(wolfSSLv23_server_method()));
    #else
    AssertNotNull(ctx = SSL_CTX_new(wolfSSLv23_client_method()));
    #endif
    AssertTrue(SSL_CTX_use_certificate_file(ctx, edCertFile,
                                                         WOLFSSL_FILETYPE_PEM));
    AssertTrue(SSL_CTX_use_PrivateKey_file(ctx, edKeyFile,
                                                         WOLFSSL_FILETYPE_PEM));
    AssertNotNull(ssl = SSL_new(ctx));

    AssertIntEQ(wolfSSL_check_private_key(ssl), WOLFSSL_SUCCESS);
    SSL_free(ssl);


    AssertTrue(SSL_CTX_use_PrivateKey_file(ctx, cliEdKeyFile,
                                                         WOLFSSL_FILETYPE_PEM));
    AssertNotNull(ssl = SSL_new(ctx));

    AssertIntNE(wolfSSL_check_private_key(ssl), WOLFSSL_SUCCESS);

    SSL_free(ssl);
    SSL_CTX_free(ctx);
#endif /* end of Ed25519 private key match tests */
    EVP_cleanup();

    /* test existence of no-op macros in wolfssl/openssl/ssl.h */
    CONF_modules_free();
    ENGINE_cleanup();
    CONF_modules_unload();

    (void)ssl;
    (void)ctx;
    (void)pkey;

    printf(resultFmt, passed);
    #endif /* defined(OPENSSL_EXTRA) && !defined(NO_CERTS) */
}


static void test_wolfSSL_PEM_PrivateKey(void)
{
#if defined(OPENSSL_EXTRA) && !defined(NO_CERTS) && \
    (!defined(NO_RSA) || defined(HAVE_ECC)) && defined(USE_CERT_BUFFERS_2048)

    BIO*      bio = NULL;
    EVP_PKEY* pkey  = NULL;
    const unsigned char* server_key = (const unsigned char*)server_key_der_2048;

    /* test creating new EVP_PKEY with bad arg */
    AssertNull((pkey = PEM_read_bio_PrivateKey(NULL, NULL, NULL, NULL)));

    /* test loading RSA key using BIO */
#if !defined(NO_RSA) && !defined(NO_FILESYSTEM)
    {
        XFILE file;
        const char* fname = "./certs/server-key.pem";
        size_t sz;
        byte* buf;

        file = XFOPEN(fname, "rb");
        AssertTrue((file != XBADFILE));
        AssertTrue(XFSEEK(file, 0, XSEEK_END) == 0);
        sz = XFTELL(file);
        XREWIND(file);
        AssertNotNull(buf = (byte*)XMALLOC(sz, NULL, DYNAMIC_TYPE_FILE));
        if (buf) {
            AssertIntEQ(XFREAD(buf, 1, sz, file), sz);
        }
        XFCLOSE(file);

        /* Test using BIO new mem and loading PEM private key */
        AssertNotNull(bio = BIO_new_mem_buf(buf, (int)sz));
        AssertNotNull((pkey = PEM_read_bio_PrivateKey(bio, NULL, NULL, NULL)));
        XFREE(buf, NULL, DYNAMIC_TYPE_FILE);
        BIO_free(bio);
        bio = NULL;
        EVP_PKEY_free(pkey);
        pkey  = NULL;
    }
#endif

    /* test loading ECC key using BIO */
#if defined(HAVE_ECC) && !defined(NO_FILESYSTEM)
    {
        XFILE file;
        const char* fname = "./certs/ecc-key.pem";
        size_t sz;
        byte* buf;

        file = XFOPEN(fname, "rb");
        AssertTrue((file != XBADFILE));
        AssertTrue(XFSEEK(file, 0, XSEEK_END) == 0);
        sz = XFTELL(file);
        XREWIND(file);
        AssertNotNull(buf = (byte*)XMALLOC(sz, NULL, DYNAMIC_TYPE_FILE));
        if (buf)
            AssertIntEQ(XFREAD(buf, 1, sz, file), sz);
        XFCLOSE(file);

        /* Test using BIO new mem and loading PEM private key */
        AssertNotNull(bio = BIO_new_mem_buf(buf, (int)sz));
        AssertNotNull((pkey = PEM_read_bio_PrivateKey(bio, NULL, NULL, NULL)));
        XFREE(buf, NULL, DYNAMIC_TYPE_FILE);
        BIO_free(bio);
        bio = NULL;
        EVP_PKEY_free(pkey);
        pkey  = NULL;
    }
#endif

#if !defined(NO_RSA) && (defined(WOLFSSL_KEY_GEN) || defined(WOLFSSL_CERT_GEN))
    {
        #define BIO_PEM_TEST_CHAR 'a'
        EVP_PKEY* pkey2 = NULL;
        unsigned char extra[10];
        int i;

        printf(testingFmt, "wolfSSL_PEM_PrivateKey()");

        XMEMSET(extra, BIO_PEM_TEST_CHAR, sizeof(extra));

        AssertNotNull(bio = wolfSSL_BIO_new(wolfSSL_BIO_s_mem()));
        AssertIntEQ(BIO_set_write_buf_size(bio, 4096), SSL_FAILURE);

        AssertNull(d2i_PrivateKey(EVP_PKEY_EC, &pkey,
                &server_key, (long)sizeof_server_key_der_2048));
        AssertNull(pkey);

        AssertNotNull(wolfSSL_d2i_PrivateKey(EVP_PKEY_RSA, &pkey,
                &server_key, (long)sizeof_server_key_der_2048));
        AssertIntEQ(PEM_write_bio_PrivateKey(bio, pkey, NULL, NULL, 0, NULL, NULL),
                WOLFSSL_SUCCESS);

        /* test creating new EVP_PKEY with good args */
        AssertNotNull((pkey2 = PEM_read_bio_PrivateKey(bio, NULL, NULL, NULL)));
        if (pkey && pkey->pkey.ptr && pkey2 && pkey2->pkey.ptr)
            AssertIntEQ((int)XMEMCMP(pkey->pkey.ptr, pkey2->pkey.ptr, pkey->pkey_sz), 0);

        /* test of reuse of EVP_PKEY */
        AssertNull(PEM_read_bio_PrivateKey(bio, &pkey, NULL, NULL));
        AssertIntEQ(BIO_pending(bio), 0);
        AssertIntEQ(PEM_write_bio_PrivateKey(bio, pkey, NULL, NULL, 0, NULL, NULL),
                SSL_SUCCESS);
        AssertIntEQ(BIO_write(bio, extra, 10), 10); /* add 10 extra bytes after PEM */
        AssertNotNull(PEM_read_bio_PrivateKey(bio, &pkey, NULL, NULL));
        AssertNotNull(pkey);
        if (pkey && pkey->pkey.ptr && pkey2 && pkey2->pkey.ptr) {
            AssertIntEQ((int)XMEMCMP(pkey->pkey.ptr, pkey2->pkey.ptr, pkey->pkey_sz),0);
        }
        AssertIntEQ(BIO_pending(bio), 10); /* check 10 extra bytes still there */
        AssertIntEQ(BIO_read(bio, extra, 10), 10);
        for (i = 0; i < 10; i++) {
            AssertIntEQ(extra[i], BIO_PEM_TEST_CHAR);
        }

        BIO_free(bio);
        bio = NULL;
        EVP_PKEY_free(pkey);
        pkey  = NULL;
        EVP_PKEY_free(pkey2);
    }
    #endif

    /* key is DES encrypted */
    #if !defined(NO_DES3) && defined(WOLFSSL_ENCRYPTED_KEYS) && \
    !defined(NO_RSA) && !defined(NO_FILESYSTEM) && !defined(NO_MD5)
    {
        XFILE f;
        pem_password_cb* passwd_cb;
        void* passwd_cb_userdata;
        SSL_CTX* ctx;
        char passwd[] = "bad password";

    #ifndef WOLFSSL_NO_TLS12
        #ifndef NO_WOLFSSL_SERVER
        AssertNotNull(ctx = SSL_CTX_new(TLSv1_2_server_method()));
        #else
        AssertNotNull(ctx = SSL_CTX_new(TLSv1_2_client_method()));
        #endif
    #else
        #ifndef NO_WOLFSSL_SERVER
        AssertNotNull(ctx = SSL_CTX_new(wolfTLSv1_3_server_method()));
        #else
        AssertNotNull(ctx = SSL_CTX_new(wolfTLSv1_3_client_method()));
        #endif
    #endif

        AssertNotNull(bio = BIO_new_file("./certs/server-keyEnc.pem", "rb"));
        SSL_CTX_set_default_passwd_cb(ctx, PasswordCallBack);
        AssertNotNull(passwd_cb = SSL_CTX_get_default_passwd_cb(ctx));
        AssertNull(passwd_cb_userdata =
            SSL_CTX_get_default_passwd_cb_userdata(ctx));

        /* fail case with password call back */
        AssertNull(pkey = PEM_read_bio_PrivateKey(bio, NULL, NULL,
                    (void*)passwd));
        BIO_free(bio);
        AssertNotNull(bio = BIO_new_file("./certs/server-keyEnc.pem", "rb"));
        AssertNull(pkey = PEM_read_bio_PrivateKey(bio, NULL, passwd_cb,
                    (void*)passwd));
        BIO_free(bio);

        f = XFOPEN("./certs/server-keyEnc.pem", "rb");
        AssertNotNull(bio = BIO_new_fp(f, BIO_CLOSE));

        /* use callback that works */
        AssertNotNull(pkey = PEM_read_bio_PrivateKey(bio, NULL, passwd_cb,
                (void*)"yassl123"));

        AssertIntEQ(SSL_CTX_use_PrivateKey(ctx, pkey), SSL_SUCCESS);

        EVP_PKEY_free(pkey);
        pkey  = NULL;
        BIO_free(bio);
        bio = NULL;
        SSL_CTX_free(ctx);
    }
    #endif /* !defined(NO_DES3) */

    #if defined(HAVE_ECC) && !defined(NO_FILESYSTEM)
    {
        unsigned char buf[2048];
        size_t bytes;
        XFILE f;
        SSL_CTX* ctx;

    #ifndef WOLFSSL_NO_TLS12
        #ifndef NO_WOLFSSL_SERVER
        AssertNotNull(ctx = SSL_CTX_new(TLSv1_2_server_method()));
        #else
        AssertNotNull(ctx = SSL_CTX_new(TLSv1_2_client_method()));
        #endif
    #else
        #ifndef NO_WOLFSSL_SERVER
        AssertNotNull(ctx = SSL_CTX_new(wolfTLSv1_3_server_method()));
        #else
        AssertNotNull(ctx = SSL_CTX_new(wolfTLSv1_3_client_method()));
        #endif
    #endif

        f = XFOPEN("./certs/ecc-key.der", "rb");
        AssertTrue((f != XBADFILE));
        bytes = (size_t)XFREAD(buf, 1, sizeof(buf), f);
        XFCLOSE(f);

        server_key = buf;
        pkey = NULL;
        AssertNull(d2i_PrivateKey(EVP_PKEY_RSA, &pkey, &server_key, bytes));
        AssertNull(pkey);
        AssertNotNull(d2i_PrivateKey(EVP_PKEY_EC, &pkey, &server_key, bytes));
        AssertIntEQ(SSL_CTX_use_PrivateKey(ctx, pkey), SSL_SUCCESS);

        EVP_PKEY_free(pkey);
        pkey = NULL;
        SSL_CTX_free(ctx);
    }
    #endif

    printf(resultFmt, passed);

    (void)bio;
    (void)pkey;
    (void)server_key;

#endif /* OPENSSL_EXTRA && !NO_CERTS && !NO_RSA && USE_CERT_BUFFERS_2048 */
}

static void test_wolfSSL_PEM_bio_RSAKey(void)
{
#if (defined(OPENSSL_EXTRA) || defined(OPENSSL_ALL)) && \
    defined(WOLFSSL_KEY_GEN) && \
    !defined(NO_FILESYSTEM) && !defined(NO_RSA) && !defined(NO_CERTS)
    RSA* rsa = NULL;
    BIO* bio = NULL;

    printf(testingFmt, "wolfSSL_PEM_bio_RSAKey");

    /* PrivateKey */
    AssertNotNull(bio = BIO_new_file(svrKeyFile, "rb"));
    AssertNull((rsa = PEM_read_bio_RSAPrivateKey(NULL, NULL, NULL, NULL)));
    AssertNotNull((rsa = PEM_read_bio_RSAPrivateKey(bio, NULL, NULL, NULL)));
    AssertIntEQ(RSA_size(rsa), 256);
    AssertIntEQ(PEM_write_bio_RSAPrivateKey(NULL, NULL, NULL, NULL, 0, NULL, \
                                            NULL), WOLFSSL_FAILURE);
    BIO_free(bio);
    AssertNotNull(bio = wolfSSL_BIO_new(wolfSSL_BIO_s_mem()));
    AssertIntEQ(PEM_write_bio_RSAPrivateKey(bio, rsa, NULL, NULL, 0, NULL, \
                                            NULL), WOLFSSL_SUCCESS);
    BIO_free(bio);
    RSA_free(rsa);

    /* PUBKEY */
    AssertNotNull(bio = BIO_new_file("./certs/rsa-pub-2048.pem", "rb"));
    AssertNull((rsa = PEM_read_bio_RSA_PUBKEY(NULL, NULL, NULL, NULL)));
    AssertNotNull((rsa = PEM_read_bio_RSA_PUBKEY(bio, NULL, NULL, NULL)));
    AssertIntEQ(RSA_size(rsa), 256);
    AssertIntEQ(PEM_write_bio_RSA_PUBKEY(NULL, NULL), WOLFSSL_FAILURE);
    BIO_free(bio);
    AssertNotNull(bio = wolfSSL_BIO_new(wolfSSL_BIO_s_mem()));
    AssertIntEQ(PEM_write_bio_RSA_PUBKEY(bio, rsa), WOLFSSL_SUCCESS);
    BIO_free(bio);
    RSA_free(rsa);

    #ifdef HAVE_ECC
    /* ensure that non-rsa keys do not work */
    AssertNotNull(bio = BIO_new_file(eccKeyFile, "rb")); /* ecc key */
    AssertNull((rsa = PEM_read_bio_RSAPrivateKey(bio, NULL, NULL, NULL)));
    AssertNull((rsa = PEM_read_bio_RSA_PUBKEY(bio, NULL, NULL, NULL)));
    BIO_free(bio);
    RSA_free(rsa);
    #endif /* HAVE_ECC */

    printf(resultFmt, passed);
#endif /* defined(OPENSSL_EXTRA) || defined(OPENSSL_ALL)) && \
         (defined(WOLFSSL_KEY_GEN) || WOLFSSL_CERT_GEN) && \
         !defined(NO_FILESYSTEM) && !defined(NO_RSA) && !defined(NO_CERTS) */
}

static void test_wolfSSL_PEM_RSAPrivateKey(void)
{
    #if defined(OPENSSL_EXTRA) && !defined(NO_CERTS) && \
       !defined(NO_FILESYSTEM) && !defined(NO_RSA)
    RSA* rsa = NULL;
    BIO* bio = NULL;

    printf(testingFmt, "wolfSSL_PEM_RSAPrivateKey()");

    AssertNotNull(bio = BIO_new_file(svrKeyFile, "rb"));
    AssertNotNull((rsa = PEM_read_bio_RSAPrivateKey(bio, NULL, NULL, NULL)));
    AssertIntEQ(RSA_size(rsa), 256);

    BIO_free(bio);
    RSA_free(rsa);

#ifdef HAVE_ECC
    AssertNotNull(bio = BIO_new_file(eccKeyFile, "rb"));
    AssertNull((rsa = PEM_read_bio_RSAPrivateKey(bio, NULL, NULL, NULL)));

    BIO_free(bio);
#endif /* HAVE_ECC */

    printf(resultFmt, passed);
    #endif /* defined(OPENSSL_EXTRA) && !defined(NO_CERTS) */
}

static void test_wolfSSL_PEM_bio_DSAKey(void)
{
#ifndef HAVE_SELFTEST
#if (defined(WOLFSSL_QT) || defined(OPENSSL_ALL)) && !defined(NO_CERTS) && \
    defined(WOLFSSL_KEY_GEN) && !defined(NO_FILESYSTEM) && !defined(NO_DSA)
    DSA* dsa = NULL;
    BIO* bio = NULL;

    printf(testingFmt, "wolfSSL_PEM_bio_DSAKey");

    /* PrivateKey */
    AssertNotNull(bio = BIO_new_file("./certs/1024/dsa1024.pem", "rb"));
    AssertNull((dsa = PEM_read_bio_DSAPrivateKey(NULL, NULL, NULL, NULL)));
    AssertNotNull((dsa = PEM_read_bio_DSAPrivateKey(bio, NULL, NULL, NULL)));
    AssertIntEQ(BN_num_bytes(dsa->g), 128);
    AssertIntEQ(PEM_write_bio_DSAPrivateKey(NULL, NULL, NULL, NULL, 0, NULL, NULL),
            WOLFSSL_FAILURE);
    BIO_free(bio);
    AssertNotNull(bio = wolfSSL_BIO_new(wolfSSL_BIO_s_mem()));
    AssertIntEQ(PEM_write_bio_DSAPrivateKey(bio, dsa, NULL, NULL, 0, NULL, NULL),
            WOLFSSL_SUCCESS);
    BIO_free(bio);
    DSA_free(dsa);

    /* PUBKEY */
    AssertNotNull(bio = BIO_new_file("./certs/1024/dsa-pub-1024.pem", "rb"));
    AssertNull((dsa = PEM_read_bio_DSA_PUBKEY(NULL, NULL, NULL, NULL)));
    AssertNotNull((dsa = PEM_read_bio_DSA_PUBKEY(bio, NULL, NULL, NULL)));
    AssertIntEQ(BN_num_bytes(dsa->g), 128);
    AssertIntEQ(PEM_write_bio_DSA_PUBKEY(NULL, NULL), WOLFSSL_FAILURE);
    BIO_free(bio);
    AssertNotNull(bio = wolfSSL_BIO_new(wolfSSL_BIO_s_mem()));
    AssertIntEQ(PEM_write_bio_DSA_PUBKEY(bio, dsa), WOLFSSL_SUCCESS);
    BIO_free(bio);
    DSA_free(dsa);

    #ifdef HAVE_ECC
    /* ensure that non-dsa keys do not work */
    AssertNotNull(bio = BIO_new_file(eccKeyFile, "rb")); /* ecc key */
    AssertNull((dsa = PEM_read_bio_DSAPrivateKey(bio, NULL, NULL, NULL)));
    AssertNull((dsa = PEM_read_bio_DSA_PUBKEY(bio, NULL, NULL, NULL)));
    BIO_free(bio);
    DSA_free(dsa);
    #endif /* HAVE_ECC */

    printf(resultFmt, passed);
#endif /* defined(WOLFSSL_QT) || defined(OPENSSL_ALL)) && \
         !defined(NO_CERTS) && defined(WOLFSSL_KEY_GEN) && \
         !defined(NO_FILESYSTEM) && !defined(NO_DSA) */
#endif /* HAVE_SELFTEST */
}

static void test_wolfSSL_PEM_bio_ECKey(void)
{
#if (defined(OPENSSL_EXTRA) || defined(OPENSSL_ALL)) && \
    defined(WOLFSSL_KEY_GEN) && !defined(NO_FILESYSTEM) && defined(HAVE_ECC)
    EC_KEY* ec = NULL;
    BIO* bio = NULL;

    printf(testingFmt, "wolfSSL_PEM_bio_ECKey");

    /* PrivateKey */
    AssertNotNull(bio = BIO_new_file("./certs/ecc-key.pem", "rb"));
    AssertNull((ec = PEM_read_bio_ECPrivateKey(NULL, NULL, NULL, NULL)));
    AssertNotNull((ec = PEM_read_bio_ECPrivateKey(bio, NULL, NULL, NULL)));
    AssertIntEQ(wc_ecc_size((ecc_key*)ec->internal), 32);
    AssertIntEQ(PEM_write_bio_ECPrivateKey(NULL, NULL, NULL, NULL, 0, NULL, \
                                           NULL),WOLFSSL_FAILURE);
    BIO_free(bio);
    AssertNotNull(bio = wolfSSL_BIO_new(wolfSSL_BIO_s_mem()));
    AssertIntEQ(PEM_write_bio_ECPrivateKey(bio, ec, NULL, NULL, 0, NULL, \
                                           NULL), WOLFSSL_SUCCESS);
    BIO_free(bio);
    EC_KEY_free(ec);

    /* PUBKEY */
    AssertNotNull(bio = BIO_new_file("./certs/ecc-client-keyPub.pem", "rb"));
    AssertNull((ec = PEM_read_bio_EC_PUBKEY(NULL, NULL, NULL, NULL)));
    AssertNotNull((ec = PEM_read_bio_EC_PUBKEY(bio, NULL, NULL, NULL)));
    AssertIntEQ(wc_ecc_size((ecc_key*)ec->internal), 32);
    AssertIntEQ(PEM_write_bio_EC_PUBKEY(NULL, NULL), WOLFSSL_FAILURE);
    BIO_free(bio);
    AssertNotNull(bio = wolfSSL_BIO_new(wolfSSL_BIO_s_mem()));
    AssertIntEQ(PEM_write_bio_EC_PUBKEY(bio, ec), WOLFSSL_SUCCESS);
    BIO_free(bio);
    EC_KEY_free(ec);

    #ifndef NO_RSA
    /* ensure that non-ec keys do not work */
    AssertNotNull(bio = BIO_new_file(svrKeyFile, "rb")); /* rsa key */
    AssertNull((ec = PEM_read_bio_ECPrivateKey(bio, NULL, NULL, NULL)));
    AssertNull((ec = PEM_read_bio_EC_PUBKEY(bio, NULL, NULL, NULL)));
    BIO_free(bio);
    EC_KEY_free(ec);
    #endif /* HAVE_ECC */

    printf(resultFmt, passed);
#endif /* defined(OPENSSL_EXTRA) && !defined(NO_CERTS) */
}

static void test_wolfSSL_PEM_PUBKEY(void)
{
#if defined(OPENSSL_EXTRA) && defined(HAVE_ECC)
    BIO*      bio = NULL;
    EVP_PKEY* pkey  = NULL;

    /* test creating new EVP_PKEY with bad arg */
    AssertNull((pkey = PEM_read_bio_PUBKEY(NULL, NULL, NULL, NULL)));

    /* test loading ECC key using BIO */
#if defined(HAVE_ECC) && !defined(NO_FILESYSTEM)
    {
        XFILE file;
        const char* fname = "./certs/ecc-client-keyPub.pem";
        size_t sz;
        byte* buf;

        file = XFOPEN(fname, "rb");
        AssertTrue((file != XBADFILE));
        XFSEEK(file, 0, XSEEK_END);
        sz = XFTELL(file);
        XREWIND(file);
        AssertNotNull(buf = (byte*)XMALLOC(sz, NULL, DYNAMIC_TYPE_FILE));
        if (buf)
            AssertIntEQ(XFREAD(buf, 1, sz, file), sz);
        XFCLOSE(file);

        /* Test using BIO new mem and loading PEM private key */
        AssertNotNull(bio = BIO_new_mem_buf(buf, (int)sz));
        AssertNotNull((pkey = PEM_read_bio_PUBKEY(bio, NULL, NULL, NULL)));
        XFREE(buf, NULL, DYNAMIC_TYPE_FILE);
        BIO_free(bio);
        bio = NULL;
        EVP_PKEY_free(pkey);
        pkey  = NULL;
    }
#endif

    (void)bio;
    (void)pkey;
#endif
}


static void test_wolfSSL_tmp_dh(void)
{
#if defined(OPENSSL_EXTRA) && !defined(NO_CERTS) && !defined(NO_FILESYSTEM) && \
    !defined(NO_DSA) && !defined(NO_RSA) && !defined(NO_DH)
    byte buffer[5300];
    char file[] = "./certs/dsaparams.pem";
    XFILE f;
    int  bytes;
    DSA* dsa;
    DH*  dh;
    BIO*     bio;
    SSL*     ssl;
    SSL_CTX* ctx;

    printf(testingFmt, "wolfSSL_tmp_dh()");

    #ifndef NO_WOLFSSL_SERVER
    AssertNotNull(ctx = SSL_CTX_new(wolfSSLv23_server_method()));
    #else
    AssertNotNull(ctx = SSL_CTX_new(wolfSSLv23_client_method()));
    #endif
    AssertTrue(SSL_CTX_use_certificate_file(ctx, svrCertFile, WOLFSSL_FILETYPE_PEM));
    AssertTrue(SSL_CTX_use_PrivateKey_file(ctx, svrKeyFile, WOLFSSL_FILETYPE_PEM));
    AssertNotNull(ssl = SSL_new(ctx));

    f = XFOPEN(file, "rb");
    AssertTrue((f != XBADFILE));
    bytes = (int)XFREAD(buffer, 1, sizeof(buffer), f);
    XFCLOSE(f);

    bio = BIO_new_mem_buf((void*)buffer, bytes);
    AssertNotNull(bio);

    dsa = wolfSSL_PEM_read_bio_DSAparams(bio, NULL, NULL, NULL);
    AssertNotNull(dsa);

    dh = wolfSSL_DSA_dup_DH(dsa);
    AssertNotNull(dh);

    AssertIntEQ((int)SSL_CTX_set_tmp_dh(ctx, dh), WOLFSSL_SUCCESS);
    #ifndef NO_WOLFSSL_SERVER
    AssertIntEQ((int)SSL_set_tmp_dh(ssl, dh), WOLFSSL_SUCCESS);
    #else
    AssertIntEQ((int)SSL_set_tmp_dh(ssl, dh), SIDE_ERROR);
    #endif

    BIO_free(bio);
    DSA_free(dsa);
    DH_free(dh);
    SSL_free(ssl);
    SSL_CTX_free(ctx);

    printf(resultFmt, passed);
#endif
}

static void test_wolfSSL_ctrl(void)
{
#if defined (OPENSSL_EXTRA)
    byte buff[5300];
    BIO* bio;
    int  bytes;
    BUF_MEM* ptr = NULL;

    printf(testingFmt, "wolfSSL_crtl()");

    bytes = sizeof(buff);
    bio = BIO_new_mem_buf((void*)buff, bytes);
    AssertNotNull(bio);
    AssertNotNull(BIO_s_socket());

    AssertIntEQ((int)wolfSSL_BIO_get_mem_ptr(bio, &ptr), WOLFSSL_SUCCESS);

    /* needs tested after stubs filled out @TODO
        SSL_ctrl
        SSL_CTX_ctrl
    */

    BIO_free(bio);
    printf(resultFmt, passed);
#endif /* defined(OPENSSL_EXTRA) */
}


static void test_wolfSSL_EVP_PKEY_new_mac_key(void)
{
#ifdef OPENSSL_EXTRA
    static const unsigned char pw[] = "password";
    static const int pwSz = sizeof(pw) - 1;
    size_t checkPwSz = 0;
    const unsigned char* checkPw = NULL;
    WOLFSSL_EVP_PKEY* key = NULL;

    printf(testingFmt, "wolfSSL_EVP_PKEY_new_mac_key()");

    AssertNull(key = wolfSSL_EVP_PKEY_new_mac_key(0, NULL, pw, pwSz));
    AssertNull(key = wolfSSL_EVP_PKEY_new_mac_key(0, NULL, NULL, pwSz));

    AssertNotNull(key = wolfSSL_EVP_PKEY_new_mac_key(EVP_PKEY_HMAC, NULL, pw, pwSz));
    if (key) {
        AssertIntEQ(key->type, EVP_PKEY_HMAC);
        AssertIntEQ(key->save_type, EVP_PKEY_HMAC);
        AssertIntEQ(key->pkey_sz, pwSz);
        AssertIntEQ(XMEMCMP(key->pkey.ptr, pw, pwSz), 0);
    }
    AssertNotNull(checkPw = wolfSSL_EVP_PKEY_get0_hmac(key, &checkPwSz));
    AssertIntEQ((int)checkPwSz, pwSz);
    if (checkPw) {
        AssertIntEQ(XMEMCMP(checkPw, pw, pwSz), 0);
    }
    wolfSSL_EVP_PKEY_free(key);

    AssertNotNull(key = wolfSSL_EVP_PKEY_new_mac_key(EVP_PKEY_HMAC, NULL, pw, 0));
    if (key) {
        AssertIntEQ(key->pkey_sz, 0);
    }
    checkPw = wolfSSL_EVP_PKEY_get0_hmac(key, &checkPwSz);
    (void)checkPw;
    AssertIntEQ((int)checkPwSz, 0);
    wolfSSL_EVP_PKEY_free(key);

    AssertNotNull(key = wolfSSL_EVP_PKEY_new_mac_key(EVP_PKEY_HMAC, NULL, NULL, 0));
    if (key) {
        AssertIntEQ(key->pkey_sz, 0);
    }
    checkPw = wolfSSL_EVP_PKEY_get0_hmac(key, &checkPwSz);
    (void)checkPw;
    AssertIntEQ((int)checkPwSz, 0);
    wolfSSL_EVP_PKEY_free(key);

    printf(resultFmt, passed);
#endif /* OPENSSL_EXTRA */
}
static void test_wolfSSL_EVP_Digest(void)
{
#if defined(OPENSSL_EXTRA) && !defined(NO_SHA256)


    const char* in = "abc";
    int   inLen = (int)XSTRLEN(in);
    byte  out[WC_SHA256_DIGEST_SIZE];
    unsigned int outLen;
    const char* expOut = "\xBA\x78\x16\xBF\x8F\x01\xCF\xEA\x41\x41\x40\xDE\x5D\xAE\x22"
               "\x23\xB0\x03\x61\xA3\x96\x17\x7A\x9C\xB4\x10\xFF\x61\xF2\x00"
               "\x15\xAD";
    printf(testingFmt, "wolfSSL_EVP_Digest()");

    AssertIntEQ(wolfSSL_EVP_Digest((unsigned char*)in, inLen, out, &outLen, "SHA256", NULL), 1);
    AssertIntEQ(outLen, WC_SHA256_DIGEST_SIZE);
    AssertIntEQ(XMEMCMP(out, expOut, WC_SHA256_DIGEST_SIZE), 0);

    printf(resultFmt, passed);

#endif /* OPEN_EXTRA && ! NO_SHA256 */
}

static void test_wolfSSL_EVP_MD_size(void)
{
#ifdef OPENSSL_EXTRA

    WOLFSSL_EVP_MD_CTX mdCtx;

    printf(testingFmt, "wolfSSL_EVP_MD_size()");

#ifndef NO_SHA256
    wolfSSL_EVP_MD_CTX_init(&mdCtx);

    AssertIntEQ(wolfSSL_EVP_DigestInit(&mdCtx, "SHA256"), 1);
    AssertIntEQ(wolfSSL_EVP_MD_size(wolfSSL_EVP_MD_CTX_md(&mdCtx)), WC_SHA256_DIGEST_SIZE);
    AssertIntEQ(wolfSSL_EVP_MD_CTX_block_size(&mdCtx), WC_SHA256_BLOCK_SIZE);
    AssertIntEQ(wolfSSL_EVP_MD_CTX_cleanup(&mdCtx), 1);

#endif

#ifndef NO_MD5
    wolfSSL_EVP_MD_CTX_init(&mdCtx);

    AssertIntEQ(wolfSSL_EVP_DigestInit(&mdCtx, "MD5"), 1);
    AssertIntEQ(wolfSSL_EVP_MD_CTX_size(&mdCtx), WC_MD5_DIGEST_SIZE);
    AssertIntEQ(wolfSSL_EVP_MD_CTX_block_size(&mdCtx), WC_MD5_BLOCK_SIZE);
    AssertIntEQ(wolfSSL_EVP_MD_CTX_cleanup(&mdCtx), 1);

#endif

#ifdef WOLFSSL_SHA224
    wolfSSL_EVP_MD_CTX_init(&mdCtx);

    AssertIntEQ(wolfSSL_EVP_DigestInit(&mdCtx, "SHA224"), 1);
    AssertIntEQ(wolfSSL_EVP_MD_CTX_size(&mdCtx), WC_SHA224_DIGEST_SIZE);
    AssertIntEQ(wolfSSL_EVP_MD_CTX_block_size(&mdCtx), WC_SHA224_BLOCK_SIZE);
    AssertIntEQ(wolfSSL_EVP_MD_CTX_cleanup(&mdCtx), 1);

#endif

#ifdef WOLFSSL_SHA384
    wolfSSL_EVP_MD_CTX_init(&mdCtx);

    AssertIntEQ(wolfSSL_EVP_DigestInit(&mdCtx, "SHA384"), 1);
    AssertIntEQ(wolfSSL_EVP_MD_CTX_size(&mdCtx), WC_SHA384_DIGEST_SIZE);
    AssertIntEQ(wolfSSL_EVP_MD_CTX_block_size(&mdCtx), WC_SHA384_BLOCK_SIZE);
    AssertIntEQ(wolfSSL_EVP_MD_CTX_cleanup(&mdCtx), 1);

#endif

#ifdef WOLFSSL_SHA512
    wolfSSL_EVP_MD_CTX_init(&mdCtx);

    AssertIntEQ(wolfSSL_EVP_DigestInit(&mdCtx, "SHA512"), 1);
    AssertIntEQ(wolfSSL_EVP_MD_CTX_size(&mdCtx), WC_SHA512_DIGEST_SIZE);
    AssertIntEQ(wolfSSL_EVP_MD_CTX_block_size(&mdCtx), WC_SHA512_BLOCK_SIZE);
    AssertIntEQ(wolfSSL_EVP_MD_CTX_cleanup(&mdCtx), 1);

#endif

#ifndef NO_SHA
    wolfSSL_EVP_MD_CTX_init(&mdCtx);

    AssertIntEQ(wolfSSL_EVP_DigestInit(&mdCtx, "SHA"), 1);
    AssertIntEQ(wolfSSL_EVP_MD_CTX_size(&mdCtx), WC_SHA_DIGEST_SIZE);
    AssertIntEQ(wolfSSL_EVP_MD_CTX_block_size(&mdCtx), WC_SHA_BLOCK_SIZE);
    AssertIntEQ(wolfSSL_EVP_MD_CTX_cleanup(&mdCtx), 1);

#endif
    /* error case */
    wolfSSL_EVP_MD_CTX_init(&mdCtx);

    AssertIntEQ(wolfSSL_EVP_DigestInit(&mdCtx, ""), BAD_FUNC_ARG);
    AssertIntEQ(wolfSSL_EVP_MD_size(wolfSSL_EVP_MD_CTX_md(&mdCtx)), BAD_FUNC_ARG);
    AssertIntEQ(wolfSSL_EVP_MD_CTX_block_size(&mdCtx), BAD_FUNC_ARG);
    AssertIntEQ(wolfSSL_EVP_MD_CTX_cleanup(&mdCtx), 0);

    printf(resultFmt, passed);

#endif /* OPENSSL_EXTRA */
}

static void test_wolfSSL_EVP_MD_hmac_signing(void)
{
#ifdef OPENSSL_EXTRA
    const unsigned char testKey[] =
    {
        0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b,
        0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b,
        0x0b, 0x0b, 0x0b, 0x0b
    };
    const char testData[] = "Hi There";
    const unsigned char testResult[] =
    {
        0xb0, 0x34, 0x4c, 0x61, 0xd8, 0xdb, 0x38, 0x53,
        0x5c, 0xa8, 0xaf, 0xce, 0xaf, 0x0b, 0xf1, 0x2b,
        0x88, 0x1d, 0xc2, 0x00, 0xc9, 0x83, 0x3d, 0xa7,
        0x26, 0xe9, 0x37, 0x6c, 0x2e, 0x32, 0xcf, 0xf7
    };
    unsigned char check[sizeof(testResult)];
    size_t checkSz = -1;
    WOLFSSL_EVP_PKEY* key;
    WOLFSSL_EVP_MD_CTX mdCtx;

    printf(testingFmt, "wolfSSL_EVP_MD_hmac_signing()");
    AssertNotNull(key = wolfSSL_EVP_PKEY_new_mac_key(EVP_PKEY_HMAC, NULL,
                                               testKey, (int)sizeof(testKey)));
    wolfSSL_EVP_MD_CTX_init(&mdCtx);
    AssertIntEQ(wolfSSL_EVP_DigestSignInit(&mdCtx, NULL, wolfSSL_EVP_sha256(),
                                                                NULL, key), 1);
    AssertIntEQ(wolfSSL_EVP_DigestSignUpdate(&mdCtx, testData,
                                          (unsigned int)XSTRLEN(testData)), 1);
    AssertIntEQ(wolfSSL_EVP_DigestSignFinal(&mdCtx, NULL, &checkSz), 1);
    AssertIntEQ((int)checkSz, sizeof(testResult));
    AssertIntEQ(wolfSSL_EVP_DigestSignFinal(&mdCtx, check, &checkSz), 1);
    AssertIntEQ((int)checkSz,(int)sizeof(testResult));
    AssertIntEQ(XMEMCMP(testResult, check, sizeof(testResult)), 0);
    AssertIntEQ(wolfSSL_EVP_MD_CTX_cleanup(&mdCtx), 1);

    AssertIntEQ(wolfSSL_EVP_DigestVerifyInit(&mdCtx, NULL, wolfSSL_EVP_sha256(),
                                                                 NULL, key), 1);
    AssertIntEQ(wolfSSL_EVP_DigestVerifyUpdate(&mdCtx, testData,
                                               (unsigned int)XSTRLEN(testData)),
                1);
    AssertIntEQ(wolfSSL_EVP_DigestVerifyFinal(&mdCtx, testResult, checkSz), 1);

    AssertIntEQ(wolfSSL_EVP_MD_CTX_cleanup(&mdCtx), 1);
    wolfSSL_EVP_MD_CTX_init(&mdCtx);
    AssertIntEQ(wolfSSL_EVP_DigestSignInit(&mdCtx, NULL, wolfSSL_EVP_sha256(),
                                                                NULL, key), 1);
    AssertIntEQ(wolfSSL_EVP_DigestSignUpdate(&mdCtx, testData, 4), 1);
    AssertIntEQ(wolfSSL_EVP_DigestSignFinal(&mdCtx, NULL, &checkSz), 1);
    AssertIntEQ((int)checkSz, sizeof(testResult));
    AssertIntEQ(wolfSSL_EVP_DigestSignFinal(&mdCtx, check, &checkSz), 1);
    AssertIntEQ((int)checkSz,(int)sizeof(testResult));
    AssertIntEQ(wolfSSL_EVP_DigestSignUpdate(&mdCtx, testData + 4,
                                      (unsigned int)XSTRLEN(testData) - 4), 1);
    AssertIntEQ(wolfSSL_EVP_DigestSignFinal(&mdCtx, check, &checkSz), 1);
    AssertIntEQ((int)checkSz,(int)sizeof(testResult));
    AssertIntEQ(XMEMCMP(testResult, check, sizeof(testResult)), 0);

    AssertIntEQ(wolfSSL_EVP_MD_CTX_cleanup(&mdCtx), 1);
    AssertIntEQ(wolfSSL_EVP_DigestVerifyInit(&mdCtx, NULL, wolfSSL_EVP_sha256(),
                                                                 NULL, key), 1);
    AssertIntEQ(wolfSSL_EVP_DigestVerifyUpdate(&mdCtx, testData, 4), 1);
    AssertIntEQ(wolfSSL_EVP_DigestVerifyUpdate(&mdCtx, testData + 4,
                                           (unsigned int)XSTRLEN(testData) - 4),
                1);
    AssertIntEQ(wolfSSL_EVP_DigestVerifyFinal(&mdCtx, testResult, checkSz), 1);

    AssertIntEQ(wolfSSL_EVP_MD_CTX_cleanup(&mdCtx), 1);

    wolfSSL_EVP_PKEY_free(key);
    printf(resultFmt, passed);
#endif /* OPENSSL_EXTRA */
}


static void test_wolfSSL_EVP_MD_rsa_signing(void)
{
#if defined(OPENSSL_EXTRA) && !defined(NO_RSA) && !defined(HAVE_USER_RSA) && \
                                                  defined(USE_CERT_BUFFERS_2048)
    WOLFSSL_EVP_PKEY* privKey;
    WOLFSSL_EVP_PKEY* pubKey;
    const char testData[] = "Hi There";
    WOLFSSL_EVP_MD_CTX mdCtx;
    size_t checkSz = -1;
    int sz = 2048 / 8;
    const unsigned char* cp;
    unsigned char* p;
    unsigned char check[2048/8];

    printf(testingFmt, "wolfSSL_EVP_MD_rsa_signing()");

    cp = client_key_der_2048;
    AssertNotNull((privKey = wolfSSL_d2i_PrivateKey(EVP_PKEY_RSA, NULL, &cp,
                                                  sizeof_client_key_der_2048)));
    p = (unsigned char *)client_keypub_der_2048;
    AssertNotNull((pubKey = wolfSSL_d2i_PUBKEY(NULL, &p,
                                               sizeof_client_keypub_der_2048)));

    wolfSSL_EVP_MD_CTX_init(&mdCtx);
    AssertIntEQ(wolfSSL_EVP_DigestSignInit(&mdCtx, NULL, wolfSSL_EVP_sha256(),
                                                             NULL, privKey), 1);
    AssertIntEQ(wolfSSL_EVP_DigestSignUpdate(&mdCtx, testData,
                                          (unsigned int)XSTRLEN(testData)), 1);
    AssertIntEQ(wolfSSL_EVP_DigestSignFinal(&mdCtx, NULL, &checkSz), 1);
    AssertIntEQ((int)checkSz, sz);
    AssertIntEQ(wolfSSL_EVP_DigestSignFinal(&mdCtx, check, &checkSz), 1);
    AssertIntEQ((int)checkSz,sz);
    AssertIntEQ(wolfSSL_EVP_MD_CTX_cleanup(&mdCtx), 1);

    wolfSSL_EVP_MD_CTX_init(&mdCtx);
    AssertIntEQ(wolfSSL_EVP_DigestVerifyInit(&mdCtx, NULL, wolfSSL_EVP_sha256(),
                                                              NULL, pubKey), 1);
    AssertIntEQ(wolfSSL_EVP_DigestVerifyUpdate(&mdCtx, testData,
                                               (unsigned int)XSTRLEN(testData)),
                1);
    AssertIntEQ(wolfSSL_EVP_DigestVerifyFinal(&mdCtx, check, checkSz), 1);
    AssertIntEQ(wolfSSL_EVP_MD_CTX_cleanup(&mdCtx), 1);

    wolfSSL_EVP_MD_CTX_init(&mdCtx);
    AssertIntEQ(wolfSSL_EVP_DigestSignInit(&mdCtx, NULL, wolfSSL_EVP_sha256(),
                                                             NULL, privKey), 1);
    AssertIntEQ(wolfSSL_EVP_DigestSignUpdate(&mdCtx, testData, 4), 1);
    AssertIntEQ(wolfSSL_EVP_DigestSignFinal(&mdCtx, NULL, &checkSz), 1);
    AssertIntEQ((int)checkSz, sz);
    AssertIntEQ(wolfSSL_EVP_DigestSignFinal(&mdCtx, check, &checkSz), 1);
    AssertIntEQ((int)checkSz, sz);
    AssertIntEQ(wolfSSL_EVP_DigestSignUpdate(&mdCtx, testData + 4,
                                      (unsigned int)XSTRLEN(testData) - 4), 1);
    AssertIntEQ(wolfSSL_EVP_DigestSignFinal(&mdCtx, check, &checkSz), 1);
    AssertIntEQ((int)checkSz, sz);
    AssertIntEQ(wolfSSL_EVP_MD_CTX_cleanup(&mdCtx), 1);

    wolfSSL_EVP_MD_CTX_init(&mdCtx);
    AssertIntEQ(wolfSSL_EVP_DigestVerifyInit(&mdCtx, NULL, wolfSSL_EVP_sha256(),
                                                              NULL, pubKey), 1);
    AssertIntEQ(wolfSSL_EVP_DigestVerifyUpdate(&mdCtx, testData, 4), 1);
    AssertIntEQ(wolfSSL_EVP_DigestVerifyUpdate(&mdCtx, testData + 4,
                                           (unsigned int)XSTRLEN(testData) - 4),
                1);
    AssertIntEQ(wolfSSL_EVP_DigestVerifyFinal(&mdCtx, check, checkSz), 1);
    AssertIntEQ(wolfSSL_EVP_MD_CTX_cleanup(&mdCtx), 1);

    wolfSSL_EVP_PKEY_free(pubKey);
    wolfSSL_EVP_PKEY_free(privKey);

    printf(resultFmt, passed);
#endif
}


static void test_wolfSSL_EVP_MD_ecc_signing(void)
{
#if defined(OPENSSL_EXTRA) && defined(HAVE_ECC) && defined(USE_CERT_BUFFERS_256)
    WOLFSSL_EVP_PKEY* privKey;
    WOLFSSL_EVP_PKEY* pubKey;
    const char testData[] = "Hi There";
    WOLFSSL_EVP_MD_CTX mdCtx;
    size_t checkSz = -1;
    const unsigned char* cp;
    unsigned char* p;
    unsigned char check[2048/8];

    printf(testingFmt, "wolfSSL_EVP_MD_ecc_signing()");

    cp = ecc_clikey_der_256;
    AssertNotNull((privKey = wolfSSL_d2i_PrivateKey(EVP_PKEY_EC, NULL, &cp,
                                                   sizeof_ecc_clikey_der_256)));
    p = (unsigned char *)ecc_clikeypub_der_256;
    AssertNotNull((pubKey = wolfSSL_d2i_PUBKEY(NULL, &p,
                                                sizeof_ecc_clikeypub_der_256)));

    wolfSSL_EVP_MD_CTX_init(&mdCtx);
    AssertIntEQ(wolfSSL_EVP_DigestSignInit(&mdCtx, NULL, wolfSSL_EVP_sha256(),
                                                             NULL, privKey), 1);
    AssertIntEQ(wolfSSL_EVP_DigestSignUpdate(&mdCtx, testData,
                                          (unsigned int)XSTRLEN(testData)), 1);
    AssertIntEQ(wolfSSL_EVP_DigestSignFinal(&mdCtx, NULL, &checkSz), 1);
    AssertIntEQ(wolfSSL_EVP_DigestSignFinal(&mdCtx, check, &checkSz), 1);
    AssertIntEQ(wolfSSL_EVP_MD_CTX_cleanup(&mdCtx), 1);

    wolfSSL_EVP_MD_CTX_init(&mdCtx);
    AssertIntEQ(wolfSSL_EVP_DigestVerifyInit(&mdCtx, NULL, wolfSSL_EVP_sha256(),
                                                              NULL, pubKey), 1);
    AssertIntEQ(wolfSSL_EVP_DigestVerifyUpdate(&mdCtx, testData,
                                               (unsigned int)XSTRLEN(testData)),
                1);
    AssertIntEQ(wolfSSL_EVP_DigestVerifyFinal(&mdCtx, check, checkSz), 1);
    AssertIntEQ(wolfSSL_EVP_MD_CTX_cleanup(&mdCtx), 1);

    wolfSSL_EVP_MD_CTX_init(&mdCtx);
    AssertIntEQ(wolfSSL_EVP_DigestSignInit(&mdCtx, NULL, wolfSSL_EVP_sha256(),
                                                             NULL, privKey), 1);
    AssertIntEQ(wolfSSL_EVP_DigestSignUpdate(&mdCtx, testData, 4), 1);
    AssertIntEQ(wolfSSL_EVP_DigestSignFinal(&mdCtx, NULL, &checkSz), 1);
    AssertIntEQ(wolfSSL_EVP_DigestSignFinal(&mdCtx, check, &checkSz), 1);
    AssertIntEQ(wolfSSL_EVP_DigestSignUpdate(&mdCtx, testData + 4,
                                      (unsigned int)XSTRLEN(testData) - 4), 1);
    AssertIntEQ(wolfSSL_EVP_DigestSignFinal(&mdCtx, check, &checkSz), 1);
    AssertIntEQ(wolfSSL_EVP_MD_CTX_cleanup(&mdCtx), 1);

    wolfSSL_EVP_MD_CTX_init(&mdCtx);
    AssertIntEQ(wolfSSL_EVP_DigestVerifyInit(&mdCtx, NULL, wolfSSL_EVP_sha256(),
                                                              NULL, pubKey), 1);
    AssertIntEQ(wolfSSL_EVP_DigestVerifyUpdate(&mdCtx, testData, 4), 1);
    AssertIntEQ(wolfSSL_EVP_DigestVerifyUpdate(&mdCtx, testData + 4,
                                           (unsigned int)XSTRLEN(testData) - 4),
                1);
    AssertIntEQ(wolfSSL_EVP_DigestVerifyFinal(&mdCtx, check, checkSz), 1);
    AssertIntEQ(wolfSSL_EVP_MD_CTX_cleanup(&mdCtx), 1);

    wolfSSL_EVP_PKEY_free(pubKey);
    wolfSSL_EVP_PKEY_free(privKey);

    printf(resultFmt, passed);
#endif
}


static void test_wolfSSL_CTX_add_extra_chain_cert(void)
{
#if defined(OPENSSL_EXTRA) && !defined(NO_CERTS) && \
   !defined(NO_FILESYSTEM) && !defined(NO_RSA)
    char caFile[] = "./certs/client-ca.pem";
    char clientFile[] = "./certs/client-cert.pem";
    SSL_CTX* ctx;
    X509* x509 = NULL;

    printf(testingFmt, "wolfSSL_CTX_add_extra_chain_cert()");

    #ifndef NO_WOLFSSL_SERVER
    AssertNotNull(ctx = SSL_CTX_new(wolfSSLv23_server_method()));
    #else
    AssertNotNull(ctx = SSL_CTX_new(wolfSSLv23_client_method()));
    #endif

    x509 = wolfSSL_X509_load_certificate_file(caFile, WOLFSSL_FILETYPE_PEM);
    AssertNotNull(x509);
    AssertIntEQ((int)SSL_CTX_add_extra_chain_cert(ctx, x509), WOLFSSL_SUCCESS);

    x509 = wolfSSL_X509_load_certificate_file(clientFile, WOLFSSL_FILETYPE_PEM);
    AssertNotNull(x509);

    #if !defined(HAVE_USER_RSA) && !defined(HAVE_FAST_RSA)
    /* additional test of getting EVP_PKEY key size from X509
     * Do not run with user RSA because wolfSSL_RSA_size is not currently
     * allowed with user RSA */
    {
        EVP_PKEY* pkey;
        #if defined(HAVE_ECC)
        X509* ecX509;
        #endif /* HAVE_ECC */

        AssertNotNull(pkey = X509_get_pubkey(x509));
        /* current RSA key is 2048 bit (256 bytes) */
        AssertIntEQ(EVP_PKEY_size(pkey), 256);

        EVP_PKEY_free(pkey);

        #if defined(HAVE_ECC)
        #if defined(USE_CERT_BUFFERS_256)
        AssertNotNull(ecX509 = wolfSSL_X509_load_certificate_buffer(
                    cliecc_cert_der_256, sizeof_cliecc_cert_der_256,
                    SSL_FILETYPE_ASN1));
        #else
        AssertNotNull(ecX509 = wolfSSL_X509_load_certificate_file(cliEccCertFile,
                    SSL_FILETYPE_PEM));
        #endif
        AssertNotNull(pkey = X509_get_pubkey(ecX509));
        /* current ECC key is 256 bit (32 bytes) */
        AssertIntEQ(EVP_PKEY_size(pkey), 32);

        X509_free(ecX509);
        EVP_PKEY_free(pkey);
        #endif /* HAVE_ECC */
    }
#endif /* !defined(HAVE_USER_RSA) && !defined(HAVE_FAST_RSA) */

    AssertIntEQ((int)SSL_CTX_add_extra_chain_cert(ctx, x509), SSL_SUCCESS);

#ifdef WOLFSSL_ENCRYPTED_KEYS
    AssertNull(SSL_CTX_get_default_passwd_cb(ctx));
    AssertNull(SSL_CTX_get_default_passwd_cb_userdata(ctx));
#endif

    SSL_CTX_free(ctx);
    printf(resultFmt, passed);
    #endif /* defined(OPENSSL_EXTRA) && !defined(NO_CERTS) && \
             !defined(NO_FILESYSTEM) && !defined(NO_RSA) */
}


#if !defined(NO_WOLFSSL_CLIENT) && !defined(NO_WOLFSSL_SERVER)
static void test_wolfSSL_ERR_peek_last_error_line(void)
{
    #if defined(OPENSSL_EXTRA) && !defined(NO_CERTS) && \
       !defined(NO_FILESYSTEM) && defined(DEBUG_WOLFSSL) && \
       !defined(NO_OLD_TLS) && !defined(WOLFSSL_NO_TLS12) && \
       defined(HAVE_IO_TESTS_DEPENDENCIES)
    tcp_ready ready;
    func_args client_args;
    func_args server_args;
#ifndef SINGLE_THREADED
    THREAD_TYPE serverThread;
#endif
    callback_functions client_cb;
    callback_functions server_cb;
    int         line = 0;
    int         flag = ERR_TXT_STRING;
    const char* file = NULL;
    const char* data = NULL;

    printf(testingFmt, "wolfSSL_ERR_peek_last_error_line()");

    /* create a failed connection and inspect the error */
#ifdef WOLFSSL_TIRTOS
    fdOpenSession(Task_self());
#endif
    XMEMSET(&client_args, 0, sizeof(func_args));
    XMEMSET(&server_args, 0, sizeof(func_args));

    StartTCP();
    InitTcpReady(&ready);

    XMEMSET(&client_cb, 0, sizeof(callback_functions));
    XMEMSET(&server_cb, 0, sizeof(callback_functions));
    client_cb.method  = wolfTLSv1_1_client_method;
    server_cb.method  = wolfTLSv1_2_server_method;

    server_args.signal    = &ready;
    server_args.callbacks = &server_cb;
    client_args.signal    = &ready;
    client_args.callbacks = &client_cb;

#ifndef SINGLE_THREADED
    start_thread(test_server_nofail, &server_args, &serverThread);
    wait_tcp_ready(&server_args);
    test_client_nofail(&client_args, NULL);
    join_thread(serverThread);
#endif

    FreeTcpReady(&ready);

    AssertIntGT(ERR_get_error_line_data(NULL, NULL, &data, &flag), 0);
    AssertNotNull(data);

    /* check clearing error state */
    ERR_remove_state(0);
    AssertIntEQ((int)ERR_peek_last_error_line(NULL, NULL), 0);
    ERR_peek_last_error_line(NULL, &line);
    AssertIntEQ(line, 0);
    ERR_peek_last_error_line(&file, NULL);
    AssertNull(file);

    /* retry connection to fill error queue */
    XMEMSET(&client_args, 0, sizeof(func_args));
    XMEMSET(&server_args, 0, sizeof(func_args));

    StartTCP();
    InitTcpReady(&ready);

    client_cb.method  = wolfTLSv1_1_client_method;
    server_cb.method  = wolfTLSv1_2_server_method;

    server_args.signal    = &ready;
    server_args.callbacks = &server_cb;
    client_args.signal    = &ready;
    client_args.callbacks = &client_cb;

    start_thread(test_server_nofail, &server_args, &serverThread);
    wait_tcp_ready(&server_args);
    test_client_nofail(&client_args, NULL);
    join_thread(serverThread);

    FreeTcpReady(&ready);

    /* check that error code was stored */
    AssertIntNE((int)ERR_peek_last_error_line(NULL, NULL), 0);
    ERR_peek_last_error_line(NULL, &line);
    AssertIntNE(line, 0);
    ERR_peek_last_error_line(&file, NULL);
    AssertNotNull(file);

#ifdef WOLFSSL_TIRTOS
    fdOpenSession(Task_self());
#endif

    printf(resultFmt, passed);

    printf("\nTesting error print out\n");
    ERR_print_errors_fp(stdout);
    printf("Done testing print out\n\n");
    fflush(stdout);
    #endif /* defined(OPENSSL_EXTRA) && !defined(NO_CERTS) && \
             !defined(NO_FILESYSTEM) && !defined(DEBUG_WOLFSSL) */
}
#endif

#if defined(OPENSSL_EXTRA) && !defined(NO_CERTS) && \
       !defined(NO_FILESYSTEM) && !defined(NO_RSA)
static int verify_cb(int ok, X509_STORE_CTX *ctx)
{
    (void) ok;
    (void) ctx;
    printf("ENTER verify_cb\n");
    return SSL_SUCCESS;
}
#endif


static void test_wolfSSL_X509_STORE_CTX_get0_current_issuer(void)
{
#if defined(OPENSSL_EXTRA) && !defined(NO_RSA)
    #ifdef WOLFSSL_SIGNER_DER_CERT
    int cmp;
    #endif
    X509_STORE_CTX* ctx;
    X509_STORE* str;
    X509* x509Ca;
    X509* x509Svr;
    X509* issuer;
    X509_NAME* caName;
    X509_NAME* issuerName;

    printf(testingFmt, "wolfSSL_X509_STORE_CTX_get0_current_issuer()");

    AssertNotNull(ctx = X509_STORE_CTX_new());
    AssertNotNull((str = wolfSSL_X509_STORE_new()));
    AssertNotNull((x509Ca =
            wolfSSL_X509_load_certificate_file(caCertFile, SSL_FILETYPE_PEM)));
    AssertIntEQ(X509_STORE_add_cert(str, x509Ca), SSL_SUCCESS);
    AssertNotNull((x509Svr =
            wolfSSL_X509_load_certificate_file(svrCertFile, SSL_FILETYPE_PEM)));

    AssertIntEQ(X509_STORE_CTX_init(ctx, str, x509Svr, NULL), SSL_SUCCESS);

    AssertNull(X509_STORE_CTX_get0_current_issuer(NULL));
    issuer = X509_STORE_CTX_get0_current_issuer(ctx);
    AssertNotNull(issuer);

    caName = X509_get_subject_name(x509Ca);
    AssertNotNull(caName);
    issuerName = X509_get_subject_name(issuer);
    #ifdef WOLFSSL_SIGNER_DER_CERT
        AssertNotNull(issuerName);
        cmp = X509_NAME_cmp(caName, issuerName);
        AssertIntEQ(cmp, 0);
    #else
        /* X509_STORE_CTX_get0_current_issuer() returns empty issuer */
        AssertNull(issuerName);
    #endif

    X509_free(issuer);
    X509_STORE_CTX_free(ctx);
    #ifdef WOLFSSL_KEEP_STORE_CERTS
        X509_free(x509Svr);
    #endif
    X509_free(x509Ca);

    printf(resultFmt, passed);
#endif
}


static void test_wolfSSL_X509_STORE_CTX(void)
{
    #if defined(OPENSSL_EXTRA) && !defined(NO_CERTS) && \
       !defined(NO_FILESYSTEM) && !defined(NO_RSA)

    X509_STORE_CTX* ctx;
    X509_STORE* str;
    X509* x509;
#ifdef OPENSSL_ALL
    X509* x5092;
    STACK_OF(X509) *sk, *sk2, *sk3;
#endif

    printf(testingFmt, "wolfSSL_X509_STORE_CTX()");
    AssertNotNull(ctx = X509_STORE_CTX_new());
    AssertNotNull((str = wolfSSL_X509_STORE_new()));
    AssertNotNull((x509 =
                wolfSSL_X509_load_certificate_file(svrCertFile, SSL_FILETYPE_PEM)));
    AssertIntEQ(X509_STORE_add_cert(str, x509), SSL_SUCCESS);
#ifdef OPENSSL_ALL
    /* sk_X509_new only in OPENSSL_ALL */
    AssertNotNull(sk = sk_X509_new());
    AssertIntEQ(X509_STORE_CTX_init(ctx, str, x509, sk), SSL_SUCCESS);
#else
    AssertIntEQ(X509_STORE_CTX_init(ctx, str, x509, NULL), SSL_SUCCESS);
#endif
    AssertIntEQ(SSL_get_ex_data_X509_STORE_CTX_idx(), 0);
    X509_STORE_CTX_set_error(ctx, -5);
    X509_STORE_CTX_set_error(NULL, -5);

    X509_STORE_CTX_free(ctx);
#ifdef OPENSSL_ALL
    sk_X509_free(sk);
#endif
    #ifdef WOLFSSL_KEEP_STORE_CERTS
    X509_free(x509);
    #endif

    AssertNotNull(ctx = X509_STORE_CTX_new());
    X509_STORE_CTX_set_verify_cb(ctx, (void *)verify_cb);
    X509_STORE_CTX_free(ctx);

#ifdef OPENSSL_ALL
    /* test X509_STORE_CTX_get(1)_chain */
    AssertNotNull((x509 = X509_load_certificate_file(svrCertFile,
                                                     SSL_FILETYPE_PEM)));
    AssertNotNull((x5092 = X509_load_certificate_file(cliCertFile,
                                                     SSL_FILETYPE_PEM)));
    AssertNotNull((sk = sk_X509_new()));
    AssertIntEQ(sk_X509_push(sk, x509), 1);
    AssertNotNull((str = X509_STORE_new()));
    AssertNotNull((ctx = X509_STORE_CTX_new()));
    AssertIntEQ(X509_STORE_CTX_init(ctx, str, x5092, sk), 1);
    AssertNull((sk2 = X509_STORE_CTX_get_chain(NULL)));
    AssertNotNull((sk2 = X509_STORE_CTX_get_chain(ctx)));
    AssertIntEQ(sk_num(sk2), 1); /* sanity, make sure chain has 1 cert */
    AssertNull((sk3 = X509_STORE_CTX_get1_chain(NULL)));
    AssertNotNull((sk3 = X509_STORE_CTX_get1_chain(ctx)));
    AssertIntEQ(sk_num(sk3), 1); /* sanity, make sure chain has 1 cert */
    X509_STORE_CTX_free(ctx);
    sk_X509_free(sk);
    #ifdef WOLFSSL_KEEP_STORE_CERTS
    /* CTX certs not freed yet */
    X509_free(x5092);
    #endif
    /* sk2 freed as part of X509_STORE_CTX_free(), sk3 is dup so free here */
    sk_X509_free(sk3);
#endif

    /* test X509_STORE_CTX_get/set_ex_data */
    {
        int i = 0, tmpData = 5;
        void* tmpDataRet;
        AssertNotNull(ctx = X509_STORE_CTX_new());
    #if defined(HAVE_EX_DATA) || defined(FORTRESS)
        for (i = 0; i < MAX_EX_DATA; i++) {
            AssertIntEQ(X509_STORE_CTX_set_ex_data(ctx, i, &tmpData),
                        WOLFSSL_SUCCESS);
            tmpDataRet = (int*)X509_STORE_CTX_get_ex_data(ctx, i);
            AssertNotNull(tmpDataRet);
            AssertIntEQ(tmpData, *(int*)tmpDataRet);
        }
    #else
        AssertIntEQ(X509_STORE_CTX_set_ex_data(ctx, i, &tmpData),
                    WOLFSSL_FAILURE);
        tmpDataRet = (int*)X509_STORE_CTX_get_ex_data(ctx, i);
        AssertNull(tmpDataRet);
    #endif
        X509_STORE_CTX_free(ctx);
    }

    printf(resultFmt, passed);
    #endif /* defined(OPENSSL_EXTRA) && !defined(NO_CERTS) && \
             !defined(NO_FILESYSTEM) && !defined(NO_RSA) */
}

static void test_wolfSSL_X509_STORE_set_flags(void)
{
    #if defined(OPENSSL_EXTRA) && !defined(NO_CERTS) && \
       !defined(NO_FILESYSTEM) && !defined(NO_RSA)

    X509_STORE* store;
    X509* x509;

    printf(testingFmt, "wolfSSL_X509_STORE_set_flags()");
    AssertNotNull((store = wolfSSL_X509_STORE_new()));
    AssertNotNull((x509 =
                wolfSSL_X509_load_certificate_file(svrCertFile, WOLFSSL_FILETYPE_PEM)));
    AssertIntEQ(X509_STORE_add_cert(store, x509), WOLFSSL_SUCCESS);

#ifdef HAVE_CRL
    AssertIntEQ(X509_STORE_set_flags(store, WOLFSSL_CRL_CHECKALL), WOLFSSL_SUCCESS);
#else
    AssertIntEQ(X509_STORE_set_flags(store, WOLFSSL_CRL_CHECKALL),
        NOT_COMPILED_IN);
#endif

    wolfSSL_X509_free(x509);
    wolfSSL_X509_STORE_free(store);

    printf(resultFmt, passed);
    #endif /* defined(OPENSSL_EXTRA) && !defined(NO_CERTS) && \
             !defined(NO_FILESYSTEM) && !defined(NO_RSA) */
}

static void test_wolfSSL_X509_LOOKUP_load_file(void)
{
    #if defined(OPENSSL_EXTRA) && defined(HAVE_CRL) && \
       !defined(NO_FILESYSTEM) && !defined(NO_RSA)
    WOLFSSL_X509_STORE*  store;
    WOLFSSL_X509_LOOKUP* lookup;

    printf(testingFmt, "wolfSSL_X509_LOOKUP_load_file()");

    AssertNotNull(store = wolfSSL_X509_STORE_new());
    AssertNotNull(lookup = X509_STORE_add_lookup(store, X509_LOOKUP_file()));
    AssertIntEQ(wolfSSL_X509_LOOKUP_load_file(lookup, "certs/client-ca.pem",
                                              X509_FILETYPE_PEM), 1);
    AssertIntEQ(wolfSSL_X509_LOOKUP_load_file(lookup, "certs/crl/crl2.pem",
                                                         X509_FILETYPE_PEM), 1);

    if (store) {
        AssertIntEQ(wolfSSL_CertManagerVerify(store->cm, cliCertFile,
                    WOLFSSL_FILETYPE_PEM), 1);
        AssertIntEQ(wolfSSL_CertManagerVerify(store->cm, svrCertFile,
                    WOLFSSL_FILETYPE_PEM), ASN_NO_SIGNER_E);
    }
    AssertIntEQ(wolfSSL_X509_LOOKUP_load_file(lookup, "certs/ca-cert.pem",
                                              X509_FILETYPE_PEM), 1);
    if (store) {
        AssertIntEQ(wolfSSL_CertManagerVerify(store->cm, svrCertFile,
                    WOLFSSL_FILETYPE_PEM), 1);
    }

    wolfSSL_X509_STORE_free(store);

    printf(resultFmt, passed);
    #endif /* defined(OPENSSL_EXTRA) && defined(HAVE_CRL) && \
             !defined(NO_FILESYSTEM) && !defined(NO_RSA) */
}

static void test_wolfSSL_X509_STORE_CTX_set_time(void)
{
    #if defined(OPENSSL_EXTRA)
    WOLFSSL_X509_STORE_CTX*  ctx;
    time_t c_time;

    printf(testingFmt, "wolfSSL_X509_set_time()");
    AssertNotNull(ctx = wolfSSL_X509_STORE_CTX_new());
    c_time = 365*24*60*60;
    wolfSSL_X509_STORE_CTX_set_time(ctx, 0, c_time);
    AssertTrue(
      (ctx->param->flags & WOLFSSL_USE_CHECK_TIME) == WOLFSSL_USE_CHECK_TIME);
    AssertTrue(ctx->param->check_time == c_time);
    wolfSSL_X509_STORE_CTX_free(ctx);

    printf(resultFmt, passed);
    #endif /* OPENSSL_EXTRA */
}

static void test_wolfSSL_get0_param(void)
{
#if defined(OPENSSL_EXTRA) && !defined(NO_RSA)
    SSL_CTX* ctx;
    SSL*     ssl;
    WOLFSSL_X509_VERIFY_PARAM* pParam;

    printf(testingFmt, "wolfSSL_get0_param()");

    #ifndef NO_WOLFSSL_SERVER
    AssertNotNull(ctx = SSL_CTX_new(wolfSSLv23_server_method()));
    #else
    AssertNotNull(ctx = SSL_CTX_new(wolfSSLv23_client_method()));
    #endif
    AssertTrue(SSL_CTX_use_certificate_file(ctx, svrCertFile, SSL_FILETYPE_PEM));
    AssertTrue(SSL_CTX_use_PrivateKey_file(ctx, svrKeyFile, SSL_FILETYPE_PEM));
    AssertNotNull(ssl = SSL_new(ctx));

    pParam = SSL_get0_param(ssl);

    (void)pParam;

    SSL_free(ssl);
    SSL_CTX_free(ctx);
    printf(resultFmt, passed);
#endif /* OPENSSL_EXTRA && !defined(NO_RSA)*/
}

static void test_wolfSSL_X509_VERIFY_PARAM_set1_host(void)
{
#if defined(OPENSSL_EXTRA)
    const char host[] = "www.example.com";
    WOLFSSL_X509_VERIFY_PARAM* pParam;

    printf(testingFmt, "wolfSSL_X509_VERIFY_PARAM_set1_host()");

    AssertNotNull(pParam = (WOLFSSL_X509_VERIFY_PARAM*)XMALLOC(
                           sizeof(WOLFSSL_X509_VERIFY_PARAM),
                           HEAP_HINT, DYNAMIC_TYPE_OPENSSL));

    XMEMSET(pParam, 0, sizeof(WOLFSSL_X509_VERIFY_PARAM));

    X509_VERIFY_PARAM_set1_host(pParam, host, sizeof(host));

    AssertIntEQ(XMEMCMP(pParam->hostName, host, sizeof(host)), 0);

    XMEMSET(pParam, 0, sizeof(WOLFSSL_X509_VERIFY_PARAM));

    AssertIntNE(XMEMCMP(pParam->hostName, host, sizeof(host)), 0);

    XFREE(pParam, HEAP_HINT, DYNAMIC_TYPE_OPENSSL);

    printf(resultFmt, passed);
#endif /* OPENSSL_EXTRA */
}

static void test_wolfSSL_X509_STORE_CTX_get0_store(void)
{
    #if defined(OPENSSL_EXTRA)
    X509_STORE* store;
    X509_STORE_CTX* ctx;
    X509_STORE_CTX* ctx_no_init;

    printf(testingFmt, "wolfSSL_X509_STORE_CTX_get0_store()");
    AssertNotNull((store = X509_STORE_new()));
    AssertNotNull(ctx = X509_STORE_CTX_new());
    AssertNotNull(ctx_no_init = X509_STORE_CTX_new());
    AssertIntEQ(X509_STORE_CTX_init(ctx, store, NULL, NULL), SSL_SUCCESS);

    AssertNull(X509_STORE_CTX_get0_store(NULL));
    /* should return NULL if ctx has not bee initialized */
    AssertNull(X509_STORE_CTX_get0_store(ctx_no_init));
    AssertNotNull(X509_STORE_CTX_get0_store(ctx));

    wolfSSL_X509_STORE_CTX_free(ctx);
    wolfSSL_X509_STORE_CTX_free(ctx_no_init);

    printf(resultFmt, passed);
    #endif /* OPENSSL_EXTRA */
}

static void test_wolfSSL_CTX_set_client_CA_list(void)
{
#if defined(OPENSSL_ALL) && !defined(NO_RSA) && !defined(NO_CERTS) && \
    !defined(NO_WOLFSSL_CLIENT)
    WOLFSSL_CTX* ctx;
    X509_NAME* name = NULL;
    STACK_OF(X509_NAME)* names = NULL;
    STACK_OF(X509_NAME)* ca_list = NULL;
    int i, names_len;

    printf(testingFmt, "wolfSSL_CTX_set_client_CA_list()");
    AssertNotNull(ctx = wolfSSL_CTX_new(wolfSSLv23_client_method()));
    names = SSL_load_client_CA_file(cliCertFile);
    AssertNotNull(names);
    SSL_CTX_set_client_CA_list(ctx,names);
    AssertNotNull(ca_list = SSL_CTX_get_client_CA_list(ctx));

    AssertIntGT((names_len = sk_X509_NAME_num(names)), 0);
    for (i=0; i<names_len; i++) {
        AssertNotNull(name = sk_X509_NAME_value(names, i));
        AssertIntEQ(sk_X509_NAME_find(names, name), i);
    }

    wolfSSL_CTX_free(ctx);
    printf(resultFmt, passed);
#endif /* OPENSSL_EXTRA  && !NO_RSA && !NO_CERTS && !NO_WOLFSSL_CLIENT */
}

static void test_wolfSSL_CTX_add_client_CA(void)
{
#if defined(OPENSSL_EXTRA) && !defined(NO_RSA) && !defined(NO_CERTS) && \
    !defined(NO_WOLFSSL_CLIENT)
    WOLFSSL_CTX* ctx;
    WOLFSSL_X509* x509;
    WOLFSSL_X509* x509_a;
    STACK_OF(X509_NAME)* ca_list;
    int ret = 0;

    printf(testingFmt, "wolfSSL_CTX_add_client_CA()");
    AssertNotNull(ctx = SSL_CTX_new(wolfSSLv23_client_method()));
    /* Add client cert */
    AssertNotNull(x509 = X509_load_certificate_file(cliCertFile,
                                                      SSL_FILETYPE_PEM));
    ret = SSL_CTX_add_client_CA(ctx, x509);
    AssertIntEQ(ret, SSL_SUCCESS);
    AssertNotNull(ca_list = SSL_CTX_get_client_CA_list(ctx));
    /* Add another client cert */
    AssertNotNull(x509_a = X509_load_certificate_file(cliCertFile,
                                                        SSL_FILETYPE_PEM));
    AssertIntEQ(SSL_CTX_add_client_CA(ctx, x509_a), SSL_SUCCESS);

    X509_free(x509);
    X509_free(x509_a);
    SSL_CTX_free(ctx);

    printf(resultFmt, passed);
#endif /* OPENSSL_EXTRA  && !NO_RSA && !NO_CERTS && !NO_WOLFSSL_CLIENT */
}

static void test_wolfSSL_X509_NID(void)
{
    #if (defined(OPENSSL_EXTRA) || defined(OPENSSL_EXTRA_X509_SMALL)) && \
    !defined(NO_RSA) && defined(USE_CERT_BUFFERS_2048) && !defined(NO_ASN)
    int   sigType;
    int   nameSz;

    X509*  cert;
    EVP_PKEY*  pubKeyTmp;
    X509_NAME* name;

    char commonName[80];
    char countryName[80];
    char localityName[80];
    char stateName[80];
    char orgName[80];
    char orgUnit[80];

    printf(testingFmt, "wolfSSL_X509_NID()");
    /* ------ PARSE ORIGINAL SELF-SIGNED CERTIFICATE ------ */

    /* convert cert from DER to internal WOLFSSL_X509 struct */
    AssertNotNull(cert = wolfSSL_X509_d2i(&cert, client_cert_der_2048,
            sizeof_client_cert_der_2048));

    /* ------ EXTRACT CERTIFICATE ELEMENTS ------ */

    /* extract PUBLIC KEY from cert */
    AssertNotNull(pubKeyTmp = X509_get_pubkey(cert));

    /* extract signatureType */
    AssertIntNE((sigType = wolfSSL_X509_get_signature_type(cert)), 0);

    /* extract subjectName info */
    AssertNotNull(name = X509_get_subject_name(cert));
    AssertIntEQ(X509_NAME_get_text_by_NID(name, -1, NULL, 0), -1);
    AssertIntGT((nameSz = X509_NAME_get_text_by_NID(name, ASN_COMMON_NAME,
                                           NULL, 0)), 0);
    AssertIntEQ(nameSz, 15);
    AssertIntGT((nameSz = X509_NAME_get_text_by_NID(name, ASN_COMMON_NAME,
                                           commonName, sizeof(commonName))), 0);
    AssertIntEQ(nameSz, 15);
    AssertIntEQ(XMEMCMP(commonName, "www.wolfssl.com", nameSz), 0);
    AssertIntGT((nameSz = X509_NAME_get_text_by_NID(name, ASN_COMMON_NAME,
                                            commonName, 9)), 0);
    AssertIntEQ(nameSz, 8);
    AssertIntEQ(XMEMCMP(commonName, "www.wolf", nameSz), 0);

    AssertIntGT((nameSz = X509_NAME_get_text_by_NID(name, ASN_COUNTRY_NAME,
                                         countryName, sizeof(countryName))), 0);
    AssertIntEQ(XMEMCMP(countryName, "US", nameSz), 0);

    AssertIntGT((nameSz = X509_NAME_get_text_by_NID(name, ASN_LOCALITY_NAME,
                                       localityName, sizeof(localityName))), 0);
    AssertIntEQ(XMEMCMP(localityName, "Bozeman", nameSz), 0);

    AssertIntGT((nameSz = X509_NAME_get_text_by_NID(name, ASN_STATE_NAME,
                                            stateName, sizeof(stateName))), 0);
    AssertIntEQ(XMEMCMP(stateName, "Montana", nameSz), 0);

    AssertIntGT((nameSz = X509_NAME_get_text_by_NID(name, ASN_ORG_NAME,
                                            orgName, sizeof(orgName))), 0);
    AssertIntEQ(XMEMCMP(orgName, "wolfSSL_2048", nameSz), 0);

    AssertIntGT((nameSz = X509_NAME_get_text_by_NID(name, ASN_ORGUNIT_NAME,
                                            orgUnit, sizeof(orgUnit))), 0);
    AssertIntEQ(XMEMCMP(orgUnit, "Programming-2048", nameSz), 0);

    EVP_PKEY_free(pubKeyTmp);
    X509_free(cert);

    printf(resultFmt, passed);
    #endif
}

static void test_wolfSSL_CTX_set_srp_username(void)
{
#if defined(OPENSSL_EXTRA) && defined(WOLFCRYPT_HAVE_SRP) \
    && !defined(NO_SHA256) && !defined(WC_NO_RNG)
    WOLFSSL_CTX* ctx;
    const char *username = "TESTUSER";
    const char *password = "TESTPASSWORD";
    int r;

    printf(testingFmt, "wolfSSL_CTX_set_srp_username()");

    ctx = wolfSSL_CTX_new(wolfSSLv23_client_method());
    AssertNotNull(ctx);
    r = wolfSSL_CTX_set_srp_username(ctx, (char *)username);
    AssertIntEQ(r,SSL_SUCCESS);
    wolfSSL_CTX_free(ctx);

    ctx = wolfSSL_CTX_new(wolfSSLv23_client_method());
    AssertNotNull(ctx);
    r = wolfSSL_CTX_set_srp_password(ctx, (char *)password);
    AssertIntEQ(r,SSL_SUCCESS);
    r = wolfSSL_CTX_set_srp_username(ctx, (char *)username);
    AssertIntEQ(r,SSL_SUCCESS);
    wolfSSL_CTX_free(ctx);

    printf(resultFmt, passed);
#endif /* OPENSSL_EXTRA && WOLFCRYPT_HAVE_SRP */
       /* && !NO_SHA256 && !WC_NO_RNG */
}

static void test_wolfSSL_CTX_set_srp_password(void)
{
#if defined(OPENSSL_EXTRA) && defined(WOLFCRYPT_HAVE_SRP) \
    && !defined(NO_SHA256) && !defined(WC_NO_RNG)
    WOLFSSL_CTX* ctx;
    const char *username = "TESTUSER";
    const char *password = "TESTPASSWORD";
    int r;

    printf(testingFmt, "wolfSSL_CTX_set_srp_password()");
    ctx = wolfSSL_CTX_new(wolfSSLv23_client_method());
    AssertNotNull(ctx);
    r = wolfSSL_CTX_set_srp_password(ctx, (char *)password);
    AssertIntEQ(r,SSL_SUCCESS);
    wolfSSL_CTX_free(ctx);

    ctx = wolfSSL_CTX_new(wolfSSLv23_client_method());
    AssertNotNull(ctx);
    r = wolfSSL_CTX_set_srp_username(ctx, (char *)username);
    AssertIntEQ(r,SSL_SUCCESS);
    r = wolfSSL_CTX_set_srp_password(ctx, (char *)password);
    AssertIntEQ(r,SSL_SUCCESS);
    wolfSSL_CTX_free(ctx);

    printf(resultFmt, passed);
#endif /* OPENSSL_EXTRA && WOLFCRYPT_HAVE_SRP */
       /* && !NO_SHA256 && !WC_NO_RNG */
}

static void test_wolfSSL_X509_STORE(void)
{
#if defined(OPENSSL_EXTRA) && defined(HAVE_CRL) && !defined(NO_RSA)
    X509_STORE *store;
    X509_CRL *crl;
    X509 *x509;
    const char crl_pem[] = "./certs/crl/crl.pem";
    const char svrCert[] = "./certs/server-cert.pem";
    XFILE fp;

    printf(testingFmt, "test_wolfSSL_X509_STORE");
    AssertNotNull(store = (X509_STORE *)X509_STORE_new());
    AssertNotNull((x509 =
                       wolfSSL_X509_load_certificate_file(svrCert, SSL_FILETYPE_PEM)));
    AssertIntEQ(X509_STORE_add_cert(store, x509), SSL_SUCCESS);
    X509_free(x509);

    fp = XFOPEN(crl_pem, "rb");
    AssertTrue((fp != XBADFILE));
    AssertNotNull(crl = (X509_CRL *)PEM_read_X509_CRL(fp, (X509_CRL **)NULL, NULL, NULL));
    XFCLOSE(fp);
    AssertIntEQ(X509_STORE_add_crl(store, crl), SSL_SUCCESS);
    X509_CRL_free(crl);
    X509_STORE_free(store);
    printf(resultFmt, passed);
#endif
    return;
}

static void test_wolfSSL_X509_STORE_load_locations(void)
{
#if (defined(OPENSSL_ALL) || defined(WOLFSSL_APACHE_HTTPD)) && !defined(NO_FILESYSTEM)
    SSL_CTX *ctx;
    X509_STORE *store;

    const char ca_file[] = "./certs/ca-cert.pem";
    const char client_pem_file[] = "./certs/client-cert.pem";
    const char client_der_file[] = "./certs/client-cert.der";
    const char ecc_file[] = "./certs/ecc-key.pem";
    const char certs_path[] = "./certs/";
    const char bad_path[] = "./bad-path/";
#ifdef HAVE_CRL
    const char crl_path[] = "./certs/crl/";
    const char crl_file[] = "./certs/crl/crl.pem";
#endif

    printf(testingFmt, "wolfSSL_X509_STORE_load_locations");

#ifndef NO_WOLFSSL_SERVER
    AssertNotNull(ctx = SSL_CTX_new(SSLv23_server_method()));
#else
    AssertNotNull(ctx = SSL_CTX_new(SSLv23_client_method()));
#endif
    AssertNotNull(store = SSL_CTX_get_cert_store(ctx));
    AssertIntEQ(wolfSSL_CertManagerLoadCA(store->cm, ca_file, NULL), WOLFSSL_SUCCESS);

    /* Test bad arguments */
    AssertIntEQ(X509_STORE_load_locations(NULL, ca_file, NULL), WOLFSSL_FAILURE);
    AssertIntEQ(X509_STORE_load_locations(store, NULL, NULL), WOLFSSL_FAILURE);
    AssertIntEQ(X509_STORE_load_locations(store, client_der_file, NULL), WOLFSSL_FAILURE);
    AssertIntEQ(X509_STORE_load_locations(store, ecc_file, NULL), WOLFSSL_FAILURE);
    AssertIntEQ(X509_STORE_load_locations(store, NULL, bad_path), WOLFSSL_FAILURE);

#ifdef HAVE_CRL
    /* Test with CRL */
    AssertIntEQ(X509_STORE_load_locations(store, crl_file, NULL), WOLFSSL_SUCCESS);
    AssertIntEQ(X509_STORE_load_locations(store, NULL, crl_path), WOLFSSL_SUCCESS);
#endif

    /* Test with CA */
    AssertIntEQ(X509_STORE_load_locations(store, ca_file, NULL), WOLFSSL_SUCCESS);

    /* Test with client_cert and certs path */
    AssertIntEQ(X509_STORE_load_locations(store, client_pem_file, NULL), WOLFSSL_SUCCESS);
    AssertIntEQ(X509_STORE_load_locations(store, NULL, certs_path), WOLFSSL_SUCCESS);

    SSL_CTX_free(ctx);
    printf(resultFmt, passed);
#endif
}

static void test_wolfSSL_BN(void)
{
    #if defined(OPENSSL_EXTRA) && !defined(NO_ASN)
    BIGNUM* a;
    BIGNUM* b;
    BIGNUM* c;
    BIGNUM* d;
    ASN1_INTEGER* ai;
    unsigned char value[1];

    printf(testingFmt, "wolfSSL_BN()");

    AssertNotNull(b = BN_new());
    AssertNotNull(c = BN_new());
    AssertNotNull(d = BN_new());

    value[0] = 0x03;

    AssertNotNull(ai = ASN1_INTEGER_new());
    /* at the moment hard setting since no set function */
    ai->data[0] = 0x02; /* tag for ASN_INTEGER */
    ai->data[1] = 0x01; /* length of integer */
    ai->data[2] = value[0];

    AssertNotNull(a = ASN1_INTEGER_to_BN(ai, NULL));
    ASN1_INTEGER_free(ai);

    value[0] = 0x02;
    AssertNotNull(BN_bin2bn(value, sizeof(value), b));

    value[0] = 0x05;
    AssertNotNull(BN_bin2bn(value, sizeof(value), c));

    /* a^b mod c = */
    AssertIntEQ(BN_mod_exp(d, NULL, b, c, NULL), WOLFSSL_FAILURE);
    AssertIntEQ(BN_mod_exp(d, a, b, c, NULL), WOLFSSL_SUCCESS);

    /* check result  3^2 mod 5 */
    value[0] = 0;
    AssertIntEQ(BN_bn2bin(d, value), sizeof(value));
    AssertIntEQ((int)(value[0]), 4);

    /* a*b mod c = */
    AssertIntEQ(BN_mod_mul(d, NULL, b, c, NULL), SSL_FAILURE);
    AssertIntEQ(BN_mod_mul(d, a, b, c, NULL), SSL_SUCCESS);

    /* check result  3*2 mod 5 */
    value[0] = 0;
    AssertIntEQ(BN_bn2bin(d, value), sizeof(value));
    AssertIntEQ((int)(value[0]), 1);

    /* BN_mod_inverse test */
    value[0] = 0;
    BIGNUM *r = BN_new();
    BIGNUM *val = BN_mod_inverse(r,b,c,NULL);
    AssertIntEQ(BN_bn2bin(r, value), 1);
    AssertIntEQ((int)(value[0] & 0x03), 3);
    BN_free(val);

    AssertIntEQ(BN_set_word(a, 1), SSL_SUCCESS);
    AssertIntEQ(BN_set_word(b, 5), SSL_SUCCESS);
    AssertIntEQ(BN_is_word(a, BN_get_word(a)), SSL_SUCCESS);
    AssertIntEQ(BN_is_word(a, 3), SSL_FAILURE);
    AssertIntEQ(BN_sub(c, a, b), SSL_SUCCESS);
#if defined(WOLFSSL_KEY_GEN) || defined(HAVE_COMP_KEY)
    {
    char* ret;
    AssertNotNull(ret = BN_bn2dec(c));
    AssertIntEQ(XMEMCMP(ret, "-4", sizeof("-4")), 0);
    XFREE(ret, NULL, DYNAMIC_TYPE_OPENSSL);
    }
#endif
    AssertIntEQ(BN_get_word(c), 4);

    BN_free(a);
    BN_free(b);
    BN_free(c);
    BN_clear_free(d);

    /* check that converting NULL and the null string returns an error */
    a = NULL;
    AssertIntLE(BN_hex2bn(&a, NULL), 0);
    AssertIntLE(BN_hex2bn(&a, ""), 0);
    AssertNull(a);

    /* check that getting a string and a bin of the same number are equal,
     * and that the comparison works EQ, LT and GT */
    AssertIntGT(BN_hex2bn(&a, "03"), 0);
    value[0] = 0x03;
    AssertNotNull(b = BN_new());
    AssertNotNull(BN_bin2bn(value, sizeof(value), b));
    value[0] = 0x04;
    AssertNotNull(c = BN_new());
    AssertNotNull(BN_bin2bn(value, sizeof(value), c));
    AssertIntEQ(BN_cmp(a, b), 0);
    AssertIntLT(BN_cmp(a, c), 0);
    AssertIntGT(BN_cmp(c, b), 0);

    BN_free(a);
    BN_free(b);
    BN_free(c);

    #if defined(USE_FAST_MATH) && !defined(HAVE_WOLF_BIGINT)
    {
        BIGNUM *ap;
        BIGNUM bv;
        BIGNUM cv;
        BIGNUM dv;

        AssertNotNull(ap = BN_new());
        BN_init(&bv);
        BN_init(&cv);
        BN_init(&dv);

        value[0] = 0x3;
        AssertNotNull(BN_bin2bn(value, sizeof(value), ap));

        value[0] = 0x02;
        AssertNotNull(BN_bin2bn(value, sizeof(value), &bv));

        value[0] = 0x05;
        AssertNotNull(BN_bin2bn(value, sizeof(value), &cv));

        /* a^b mod c = */
        AssertIntEQ(BN_mod_exp(&dv, NULL, &bv, &cv, NULL), WOLFSSL_FAILURE);
        AssertIntEQ(BN_mod_exp(&dv, ap, &bv, &cv, NULL), WOLFSSL_SUCCESS);

        /* check result  3^2 mod 5 */
        value[0] = 0;
        AssertIntEQ(BN_bn2bin(&dv, value), sizeof(value));
        AssertIntEQ((int)(value[0]), 4);

        /* a*b mod c = */
        AssertIntEQ(BN_mod_mul(&dv, NULL, &bv, &cv, NULL), SSL_FAILURE);
        AssertIntEQ(BN_mod_mul(&dv, ap, &bv, &cv, NULL), SSL_SUCCESS);

        /* check result  3*2 mod 5 */
        value[0] = 0;
        AssertIntEQ(BN_bn2bin(&dv, value), sizeof(value));
        AssertIntEQ((int)(value[0]), 1);

        BN_free(ap);
    }
    #endif

    printf(resultFmt, passed);
    #endif /* defined(OPENSSL_EXTRA) && !defined(NO_ASN) */
}

#if defined(OPENSSL_EXTRA) && !defined(NO_CERTS) && \
   !defined(NO_FILESYSTEM) && !defined(NO_RSA)
#define TEST_ARG 0x1234
static void msg_cb(int write_p, int version, int content_type,
                   const void *buf, size_t len, SSL *ssl, void *arg)
{
    (void)write_p;
    (void)version;
    (void)content_type;
    (void)buf;
    (void)len;
    (void)ssl;

    AssertTrue(arg == (void*)TEST_ARG);
}
#endif

#if defined(OPENSSL_EXTRA) && !defined(NO_CERTS) && \
     !defined(NO_FILESYSTEM) && defined(DEBUG_WOLFSSL) && \
     defined(HAVE_IO_TESTS_DEPENDENCIES) && !defined(NO_WOLFSSL_CLIENT) && \
     !defined(NO_WOLFSSL_SERVER)
#ifndef SINGLE_THREADED
#if defined(SESSION_CERTS)
#include "wolfssl/internal.h"
#endif
static int msgCb(SSL_CTX *ctx, SSL *ssl)
{
    (void) ctx;
    (void) ssl;
    #ifdef WOLFSSL_QT
        STACK_OF(X509)* sk;
        X509* x509;
        int i, num;
        BIO* bio;
    #endif
    printf("\n===== msgcb called ====\n");
    #if defined(SESSION_CERTS) && defined(TEST_PEER_CERT_CHAIN)
    AssertTrue(SSL_get_peer_cert_chain(ssl) != NULL);
    AssertIntEQ(((WOLFSSL_X509_CHAIN *)SSL_get_peer_cert_chain(ssl))->count, 1);
    #endif

    #ifdef WOLFSSL_QT
    bio = BIO_new(BIO_s_file());
    BIO_set_fp(bio, stdout, BIO_NOCLOSE);
    sk = SSL_get_peer_cert_chain(ssl);
    AssertNotNull(sk);
    if (!sk) {
        BIO_free(bio);
        return SSL_FAILURE;
    }
    num = sk_X509_num(sk);
    AssertTrue(num > 0);
    for (i = 0; i < num; i++) {
        x509 = sk_X509_value(sk,i);
        AssertNotNull(x509);
        if (!x509)
            break;
        printf("Certificate at index [%d] = :\n",i);
        X509_print(bio,x509);
        printf("\n\n");
    }
    BIO_free(bio);
    #endif
    return SSL_SUCCESS;
}
#endif
#endif

static void test_wolfSSL_msgCb(void)
{
  #if defined(OPENSSL_EXTRA) && !defined(NO_CERTS) && \
     !defined(NO_FILESYSTEM) && defined(DEBUG_WOLFSSL) && \
     defined(HAVE_IO_TESTS_DEPENDENCIES) && !defined(NO_WOLFSSL_CLIENT) && \
     !defined(NO_WOLFSSL_SERVER)

    tcp_ready ready;
    func_args client_args;
    func_args server_args;
    #ifndef SINGLE_THREADED
    THREAD_TYPE serverThread;
    #endif
    callback_functions client_cb;
    callback_functions server_cb;

    printf(testingFmt, "test_wolfSSL_msgCb");

/* create a failed connection and inspect the error */
#ifdef WOLFSSL_TIRTOS
    fdOpenSession(Task_self());
#endif
    XMEMSET(&client_args, 0, sizeof(func_args));
    XMEMSET(&server_args, 0, sizeof(func_args));

    StartTCP();
    InitTcpReady(&ready);

    XMEMSET(&client_cb, 0, sizeof(callback_functions));
    XMEMSET(&server_cb, 0, sizeof(callback_functions));
#ifndef WOLFSSL_NO_TLS12
    client_cb.method  = wolfTLSv1_2_client_method;
    server_cb.method  = wolfTLSv1_2_server_method;
#else
    client_cb.method  = wolfTLSv1_3_client_method;
    server_cb.method  = wolfTLSv1_3_server_method;
#endif

    server_args.signal    = &ready;
    server_args.callbacks = &server_cb;
    client_args.signal    = &ready;
    client_args.callbacks = &client_cb;
    client_args.return_code = TEST_FAIL;

    #ifndef SINGLE_THREADED
    start_thread(test_server_nofail, &server_args, &serverThread);
    wait_tcp_ready(&server_args);
    test_client_nofail(&client_args, (void *)msgCb);
    join_thread(serverThread);
    AssertTrue(client_args.return_code);
    AssertTrue(server_args.return_code);
    #endif

    FreeTcpReady(&ready);

#ifdef WOLFSSL_TIRTOS
    fdOpenSession(Task_self());
#endif

    printf(resultFmt, passed);

#endif
}

static void test_wolfSSL_either_side(void)
{
#if (defined(OPENSSL_EXTRA) || defined(WOLFSSL_EITHER_SIDE)) && \
    !defined(NO_FILESYSTEM) && defined(HAVE_IO_TESTS_DEPENDENCIES) && \
    !defined(NO_WOLFSSL_CLIENT) && !defined(NO_WOLFSSL_SERVER)

    tcp_ready ready;
    func_args client_args;
    func_args server_args;
    #ifndef SINGLE_THREADED
    THREAD_TYPE serverThread;
    #endif
    callback_functions client_cb;
    callback_functions server_cb;

    printf(testingFmt, "test_wolfSSL_either_side");

/* create a failed connection and inspect the error */
#ifdef WOLFSSL_TIRTOS
    fdOpenSession(Task_self());
#endif
    XMEMSET(&client_args, 0, sizeof(func_args));
    XMEMSET(&server_args, 0, sizeof(func_args));

    StartTCP();
    InitTcpReady(&ready);

    XMEMSET(&client_cb, 0, sizeof(callback_functions));
    XMEMSET(&server_cb, 0, sizeof(callback_functions));

    /* Use same CTX for both client and server */
    client_cb.ctx = wolfSSL_CTX_new(wolfSSLv23_method());
    AssertNotNull(client_cb.ctx);
    server_cb.ctx = client_cb.ctx;

    server_args.signal    = &ready;
    server_args.callbacks = &server_cb;
    client_args.signal    = &ready;
    client_args.callbacks = &client_cb;
    client_args.return_code = TEST_FAIL;

    #ifndef SINGLE_THREADED
    start_thread(test_server_nofail, &server_args, &serverThread);
    wait_tcp_ready(&server_args);
    test_client_nofail(&client_args, NULL);
    join_thread(serverThread);
    AssertTrue(client_args.return_code);
    AssertTrue(server_args.return_code);
    #endif

    wolfSSL_CTX_free(client_cb.ctx);

    FreeTcpReady(&ready);

#ifdef WOLFSSL_TIRTOS
    fdOpenSession(Task_self());
#endif

    printf(resultFmt, passed);

#endif
}

static void test_wolfSSL_DTLS_either_side(void)
{
#if (defined(OPENSSL_EXTRA) || defined(WOLFSSL_EITHER_SIDE)) && \
    !defined(NO_FILESYSTEM) && defined(HAVE_IO_TESTS_DEPENDENCIES) && \
    !defined(NO_WOLFSSL_CLIENT) && !defined(NO_WOLFSSL_SERVER) && \
    defined(WOLFSSL_DTLS)

    tcp_ready ready;
    func_args client_args;
    func_args server_args;
    #ifndef SINGLE_THREADED
    THREAD_TYPE serverThread;
    #endif
    callback_functions client_cb;
    callback_functions server_cb;

    printf(testingFmt, "test_wolfSSL_DTLS_either_side");

/* create a failed connection and inspect the error */
#ifdef WOLFSSL_TIRTOS
    fdOpenSession(Task_self());
#endif
    XMEMSET(&client_args, 0, sizeof(func_args));
    XMEMSET(&server_args, 0, sizeof(func_args));

    StartTCP();
    InitTcpReady(&ready);

    XMEMSET(&client_cb, 0, sizeof(callback_functions));
    XMEMSET(&server_cb, 0, sizeof(callback_functions));

    /* Use same CTX for both client and server */
    client_cb.ctx = wolfSSL_CTX_new(wolfDTLS_method());
    AssertNotNull(client_cb.ctx);
    server_cb.ctx = client_cb.ctx;

    server_args.signal    = &ready;
    server_args.callbacks = &server_cb;
    client_args.signal    = &ready;
    client_args.callbacks = &client_cb;
    client_args.return_code = TEST_FAIL;

    #ifndef SINGLE_THREADED
    start_thread(test_server_nofail, &server_args, &serverThread);
    wait_tcp_ready(&server_args);
    test_client_nofail(&client_args, NULL);
    join_thread(serverThread);
    AssertTrue(client_args.return_code);
    AssertTrue(server_args.return_code);
    #endif

    wolfSSL_CTX_free(client_cb.ctx);

    FreeTcpReady(&ready);

#ifdef WOLFSSL_TIRTOS
    fdOpenSession(Task_self());
#endif

    printf(resultFmt, passed);

#endif
}

static void test_generate_cookie(void)
{
#if defined(WOLFSSL_DTLS) && defined(OPENSSL_EXTRA)
    SSL_CTX* ctx;
    SSL* ssl;
    byte    buf[FOURK_BUF] = {0};

    printf(testingFmt, "test_generate_cookie");

    AssertNotNull(ctx = wolfSSL_CTX_new(wolfDTLS_method()));
    AssertNotNull(ssl = SSL_new(ctx));

    /* Test unconnected */
    AssertIntEQ(EmbedGenerateCookie(ssl, buf, FOURK_BUF, NULL), GEN_COOKIE_E);

    wolfSSL_CTX_SetGenCookie(ctx, EmbedGenerateCookie);

    wolfSSL_SetCookieCtx(ssl, ctx);

    AssertNotNull(wolfSSL_GetCookieCtx(ssl));

    AssertNull(wolfSSL_GetCookieCtx(NULL));

    SSL_free(ssl);
    SSL_CTX_free(ctx);

    printf(resultFmt, passed);
#endif
}

static void test_wolfSSL_set_options(void)
{
#if defined(OPENSSL_EXTRA) && !defined(NO_CERTS) && \
   !defined(NO_FILESYSTEM) && !defined(NO_RSA)
    SSL*     ssl;
    SSL_CTX* ctx;
    char appData[] = "extra msg";

    unsigned char protos[] = {
        7, 't', 'l', 's', '/', '1', '.', '2',
        8, 'h', 't', 't', 'p', '/', '1', '.', '1'
    };
    unsigned int len = sizeof(protos);

    void *arg = (void *)TEST_ARG;

    printf(testingFmt, "wolfSSL_set_options()");

#ifndef NO_WOLFSSL_SERVER
    AssertNotNull(ctx = SSL_CTX_new(wolfSSLv23_server_method()));
#else
    AssertNotNull(ctx = SSL_CTX_new(wolfSSLv23_client_method()));
#endif
    AssertTrue(SSL_CTX_use_certificate_file(ctx, svrCertFile, SSL_FILETYPE_PEM));
    AssertTrue(SSL_CTX_use_PrivateKey_file(ctx, svrKeyFile, SSL_FILETYPE_PEM));

    AssertTrue(SSL_CTX_set_options(ctx, SSL_OP_NO_TLSv1) == SSL_OP_NO_TLSv1);
    AssertTrue(SSL_CTX_get_options(ctx) == SSL_OP_NO_TLSv1);

    AssertIntGT((int)SSL_CTX_set_options(ctx, (SSL_OP_COOKIE_EXCHANGE |
                                                              SSL_OP_NO_SSLv2)), 0);
    AssertTrue((SSL_CTX_set_options(ctx, SSL_OP_COOKIE_EXCHANGE) &
                                 SSL_OP_COOKIE_EXCHANGE) == SSL_OP_COOKIE_EXCHANGE);
    AssertTrue((SSL_CTX_set_options(ctx, SSL_OP_NO_TLSv1_2) &
                                           SSL_OP_NO_TLSv1_2) == SSL_OP_NO_TLSv1_2);
    AssertTrue((SSL_CTX_set_options(ctx, SSL_OP_NO_COMPRESSION) &
                                   SSL_OP_NO_COMPRESSION) == SSL_OP_NO_COMPRESSION);
    AssertNull((SSL_CTX_clear_options(ctx, SSL_OP_NO_COMPRESSION) &
                                               SSL_OP_NO_COMPRESSION));

    SSL_CTX_free(ctx);

#ifndef NO_WOLFSSL_SERVER
    AssertNotNull(ctx = SSL_CTX_new(wolfSSLv23_server_method()));
#else
    AssertNotNull(ctx = SSL_CTX_new(wolfSSLv23_client_method()));
#endif
    AssertTrue(SSL_CTX_use_certificate_file(ctx, svrCertFile, SSL_FILETYPE_PEM));
    AssertTrue(SSL_CTX_use_PrivateKey_file(ctx, svrKeyFile, SSL_FILETYPE_PEM));

    AssertNotNull(ssl = SSL_new(ctx));
#if defined(HAVE_EX_DATA) || defined(FORTRESS)
    AssertIntEQ(SSL_set_app_data(ssl, (void*)appData), SSL_SUCCESS);
    AssertNotNull(SSL_get_app_data((const WOLFSSL*)ssl));
    if (ssl) {
        AssertIntEQ(XMEMCMP(SSL_get_app_data((const WOLFSSL*)ssl),
                    appData, sizeof(appData)), 0);
    }
#else
    AssertIntEQ(SSL_set_app_data(ssl, (void*)appData), SSL_FAILURE);
    AssertNull(SSL_get_app_data((const WOLFSSL*)ssl));
#endif

    AssertTrue(SSL_set_options(ssl, SSL_OP_NO_TLSv1) == SSL_OP_NO_TLSv1);
    AssertTrue(SSL_get_options(ssl) == SSL_OP_NO_TLSv1);

    AssertIntGT((int)SSL_set_options(ssl, (SSL_OP_COOKIE_EXCHANGE |
                                                          WOLFSSL_OP_NO_SSLv2)), 0);
    AssertTrue((SSL_set_options(ssl, SSL_OP_COOKIE_EXCHANGE) &
                             SSL_OP_COOKIE_EXCHANGE) == SSL_OP_COOKIE_EXCHANGE);
    AssertTrue((SSL_set_options(ssl, SSL_OP_NO_TLSv1_2) &
                                       SSL_OP_NO_TLSv1_2) == SSL_OP_NO_TLSv1_2);
    AssertTrue((SSL_set_options(ssl, SSL_OP_NO_COMPRESSION) &
                               SSL_OP_NO_COMPRESSION) == SSL_OP_NO_COMPRESSION);
    AssertNull((SSL_clear_options(ssl, SSL_OP_NO_COMPRESSION) &
                                       SSL_OP_NO_COMPRESSION));

    AssertTrue(SSL_set_msg_callback(ssl, msg_cb) == SSL_SUCCESS);
    SSL_set_msg_callback_arg(ssl, arg);

    AssertTrue(SSL_CTX_set_alpn_protos(ctx, protos, len) == SSL_SUCCESS);

    SSL_free(ssl);
    SSL_CTX_free(ctx);

    printf(resultFmt, passed);
#endif /* defined(OPENSSL_EXTRA) && !defined(NO_CERTS) && \
         !defined(NO_FILESYSTEM) && !defined(NO_RSA) */
}

static void test_wolfSSL_sk_SSL_CIPHER(void)
{
    #if defined(OPENSSL_ALL) && !defined(NO_CERTS) && \
       !defined(NO_FILESYSTEM) && !defined(NO_RSA)
    SSL*     ssl;
    SSL_CTX* ctx;
    STACK_OF(SSL_CIPHER) *sk, *dup;

    printf(testingFmt, "wolfSSL_sk_SSL_CIPHER_*()");

#ifndef NO_WOLFSSL_SERVER
    AssertNotNull(ctx = SSL_CTX_new(wolfSSLv23_server_method()));
#else
    AssertNotNull(ctx = SSL_CTX_new(wolfSSLv23_client_method()));
#endif
    AssertTrue(SSL_CTX_use_certificate_file(ctx, svrCertFile, SSL_FILETYPE_PEM));
    AssertTrue(SSL_CTX_use_PrivateKey_file(ctx, svrKeyFile, SSL_FILETYPE_PEM));
    AssertNotNull(ssl = SSL_new(ctx));
    AssertNotNull(sk = SSL_get_ciphers(ssl));
    AssertNotNull(dup = sk_SSL_CIPHER_dup(sk));
    AssertIntGT(sk_SSL_CIPHER_num(sk), 0);
    AssertIntEQ(sk_SSL_CIPHER_num(sk), sk_SSL_CIPHER_num(dup));

    /* error case because connection has not been established yet */
    AssertIntEQ(sk_SSL_CIPHER_find(sk, SSL_get_current_cipher(ssl)), -1);
    sk_SSL_CIPHER_free(dup);

    /* sk is pointer to internal struct that should be free'd in SSL_free */
    SSL_free(ssl);
    SSL_CTX_free(ctx);

    printf(resultFmt, passed);
    #endif /* defined(OPENSSL_EXTRA) && !defined(NO_CERTS) && \
             !defined(NO_FILESYSTEM) && !defined(NO_RSA) */
}

/* Testing  wolfSSL_set_tlsext_status_type funciton.
 * PRE: OPENSSL and HAVE_CERTIFICATE_STATUS_REQUEST defined.
 */
static void test_wolfSSL_set_tlsext_status_type(void){
    #if defined(OPENSSL_EXTRA) && defined(HAVE_CERTIFICATE_STATUS_REQUEST) && \
    !defined(NO_RSA) && !defined(NO_WOLFSSL_SERVER)
    SSL*     ssl;
    SSL_CTX* ctx;

    printf(testingFmt, "wolfSSL_set_tlsext_status_type()");

    AssertNotNull(ctx = SSL_CTX_new(wolfSSLv23_server_method()));
    AssertTrue(SSL_CTX_use_certificate_file(ctx, svrCertFile, SSL_FILETYPE_PEM));
    AssertTrue(SSL_CTX_use_PrivateKey_file(ctx, svrKeyFile, SSL_FILETYPE_PEM));
    AssertNotNull(ssl = SSL_new(ctx));
    AssertTrue(SSL_set_tlsext_status_type(ssl,TLSEXT_STATUSTYPE_ocsp)
               == SSL_SUCCESS);
    SSL_free(ssl);
    SSL_CTX_free(ctx);
    #endif  /* OPENSSL_EXTRA && HAVE_CERTIFICATE_STATUS_REQUEST && !NO_RSA */
}

static void test_wolfSSL_PEM_read_bio(void)
{
    #if defined(OPENSSL_EXTRA) && !defined(NO_CERTS) && \
       !defined(NO_FILESYSTEM) && !defined(NO_RSA)
    byte buff[5300];
    XFILE f;
    int  bytes;
    X509* x509;
    BIO*  bio = NULL;
    BUF_MEM* buf;

    printf(testingFmt, "wolfSSL_PEM_read_bio()");

    f = XFOPEN(cliCertFile, "rb");
    AssertTrue((f != XBADFILE));
    bytes = (int)XFREAD(buff, 1, sizeof(buff), f);
    XFCLOSE(f);

    AssertNull(x509 = PEM_read_bio_X509_AUX(bio, NULL, NULL, NULL));
    AssertNotNull(bio = BIO_new_mem_buf((void*)buff, bytes));
    AssertNotNull(x509 = PEM_read_bio_X509_AUX(bio, NULL, NULL, NULL));
    AssertIntEQ((int)BIO_set_fd(bio, 0, BIO_NOCLOSE), 1);
    AssertIntEQ(SSL_SUCCESS, BIO_get_mem_ptr(bio, &buf));

    BIO_free(bio);
    BUF_MEM_free(buf);
    X509_free(x509);

    printf(resultFmt, passed);
    #endif /* defined(OPENSSL_EXTRA) && !defined(NO_CERTS) && \
             !defined(NO_FILESYSTEM) && !defined(NO_RSA) */
}


#if defined(OPENSSL_EXTRA)
static long bioCallback(BIO *bio, int cmd, const char* argp, int argi,
                 long argl, long ret)
{
    (void)bio;
    (void)cmd;
    (void)argp;
    (void)argi;
    (void)argl;
    return ret;
}
#endif


static void test_wolfSSL_BIO(void)
{
    #if defined(OPENSSL_EXTRA)
    const unsigned char* p;
    byte buff[20];
    BIO* bio1;
    BIO* bio2;
    BIO* bio3;
    char* bufPt;
    int i;

    printf(testingFmt, "wolfSSL_BIO()");

    for (i = 0; i < 20; i++) {
        buff[i] = i;
    }

    /* Creating and testing type BIO_s_bio */
    AssertNotNull(bio1 = BIO_new(BIO_s_bio()));
    AssertNotNull(bio2 = BIO_new(BIO_s_bio()));
    AssertNotNull(bio3 = BIO_new(BIO_s_bio()));

    /* read/write before set up */
    AssertIntEQ(BIO_read(bio1, buff, 2),  WOLFSSL_BIO_UNSET);
    AssertIntEQ(BIO_write(bio1, buff, 2), WOLFSSL_BIO_UNSET);

    AssertIntEQ(BIO_set_nbio(bio1, 1), 1);
    AssertIntEQ(BIO_set_write_buf_size(bio1, 20), WOLFSSL_SUCCESS);
    AssertIntEQ(BIO_set_write_buf_size(bio2, 8),  WOLFSSL_SUCCESS);
    AssertIntEQ(BIO_make_bio_pair(bio1, bio2),    WOLFSSL_SUCCESS);

    AssertIntEQ(BIO_nwrite(bio1, &bufPt, 10), 10);
    XMEMCPY(bufPt, buff, 10);
    AssertIntEQ(BIO_write(bio1, buff + 10, 10), 10);
    /* write buffer full */
    AssertIntEQ(BIO_write(bio1, buff, 10), WOLFSSL_BIO_ERROR);
    AssertIntEQ(BIO_flush(bio1), WOLFSSL_SUCCESS);
    AssertIntEQ((int)BIO_ctrl_pending(bio1), 0);

    /* write the other direction with pair */
    AssertIntEQ((int)BIO_nwrite(bio2, &bufPt, 10), 8);
    XMEMCPY(bufPt, buff, 8);
    AssertIntEQ(BIO_write(bio2, buff, 10), WOLFSSL_BIO_ERROR);

    /* try read */
    AssertIntEQ((int)BIO_ctrl_pending(bio1), 8);
    AssertIntEQ((int)BIO_ctrl_pending(bio2), 20);

    /* try read using ctrl function */
    AssertIntEQ((int)BIO_ctrl(bio1, BIO_CTRL_WPENDING, 0, NULL), 8);
    AssertIntEQ((int)BIO_ctrl(bio2, BIO_CTRL_WPENDING, 0, NULL), 20);

    AssertIntEQ(BIO_nread(bio2, &bufPt, (int)BIO_ctrl_pending(bio2)), 20);
    for (i = 0; i < 20; i++) {
        AssertIntEQ((int)bufPt[i], i);
    }
    AssertIntEQ(BIO_nread(bio2, &bufPt, 1), WOLFSSL_BIO_ERROR);
    AssertIntEQ(BIO_nread(bio1, &bufPt, (int)BIO_ctrl_pending(bio1)), 8);
    for (i = 0; i < 8; i++) {
        AssertIntEQ((int)bufPt[i], i);
    }
    AssertIntEQ(BIO_nread(bio1, &bufPt, 1), WOLFSSL_BIO_ERROR);
    AssertIntEQ(BIO_ctrl_reset_read_request(bio1), 1);

    /* new pair */
    AssertIntEQ(BIO_make_bio_pair(bio1, bio3), WOLFSSL_FAILURE);
    BIO_free(bio2); /* free bio2 and automaticly remove from pair */
    AssertIntEQ(BIO_make_bio_pair(bio1, bio3), WOLFSSL_SUCCESS);
    AssertIntEQ((int)BIO_ctrl_pending(bio3), 0);
    AssertIntEQ(BIO_nread(bio3, &bufPt, 10), WOLFSSL_BIO_ERROR);

    /* test wrap around... */
    AssertIntEQ(BIO_reset(bio1), 0);
    AssertIntEQ(BIO_reset(bio3), 0);

    /* fill write buffer, read only small amount then write again */
    AssertIntEQ(BIO_nwrite(bio1, &bufPt, 20), 20);
    XMEMCPY(bufPt, buff, 20);
    AssertIntEQ(BIO_nread(bio3, &bufPt, 4), 4);
    for (i = 0; i < 4; i++) {
        AssertIntEQ(bufPt[i], i);
    }

    /* try writing over read index */
    AssertIntEQ(BIO_nwrite(bio1, &bufPt, 5), 4);
    XMEMSET(bufPt, 0, 4);
    AssertIntEQ((int)BIO_ctrl_pending(bio3), 20);

    /* read and write 0 bytes */
    AssertIntEQ(BIO_nread(bio3, &bufPt, 0), 0);
    AssertIntEQ(BIO_nwrite(bio1, &bufPt, 0), 0);

    /* should read only to end of write buffer then need to read again */
    AssertIntEQ(BIO_nread(bio3, &bufPt, 20), 16);
    for (i = 0; i < 16; i++) {
        AssertIntEQ(bufPt[i], buff[4 + i]);
    }

    AssertIntEQ(BIO_nread(bio3, NULL, 0), WOLFSSL_FAILURE);
    AssertIntEQ(BIO_nread0(bio3, &bufPt), 4);
    for (i = 0; i < 4; i++) {
        AssertIntEQ(bufPt[i], 0);
    }

    /* read index should not have advanced with nread0 */
    AssertIntEQ(BIO_nread(bio3, &bufPt, 5), 4);
    for (i = 0; i < 4; i++) {
        AssertIntEQ(bufPt[i], 0);
    }

    /* write and fill up buffer checking reset of index state */
    AssertIntEQ(BIO_nwrite(bio1, &bufPt, 20), 20);
    XMEMCPY(bufPt, buff, 20);

    /* test reset on data in bio1 write buffer */
    AssertIntEQ(BIO_reset(bio1), 0);
    AssertIntEQ((int)BIO_ctrl_pending(bio3), 0);
    AssertIntEQ(BIO_nread(bio3, &bufPt, 3), WOLFSSL_BIO_ERROR);
    AssertIntEQ(BIO_nwrite(bio1, &bufPt, 20), 20);
    AssertIntEQ((int)BIO_ctrl(bio1, BIO_CTRL_INFO, 0, &p), 20);
    AssertNotNull(p);
    XMEMCPY(bufPt, buff, 20);
    AssertIntEQ(BIO_nread(bio3, &bufPt, 6), 6);
    for (i = 0; i < 6; i++) {
        AssertIntEQ(bufPt[i], i);
    }

    /* test case of writing twice with offset read index */
    AssertIntEQ(BIO_nwrite(bio1, &bufPt, 3), 3);
    AssertIntEQ(BIO_nwrite(bio1, &bufPt, 4), 3); /* try overwriting */
    AssertIntEQ(BIO_nwrite(bio1, &bufPt, 4), WOLFSSL_BIO_ERROR);
    AssertIntEQ(BIO_nread(bio3, &bufPt, 0), 0);
    AssertIntEQ(BIO_nwrite(bio1, &bufPt, 4), WOLFSSL_BIO_ERROR);
    AssertIntEQ(BIO_nread(bio3, &bufPt, 1), 1);
    AssertIntEQ(BIO_nwrite(bio1, &bufPt, 4), 1);
    AssertIntEQ(BIO_nwrite(bio1, &bufPt, 4), WOLFSSL_BIO_ERROR);

    BIO_free(bio1);
    BIO_free(bio3);

    #if defined(OPENSSL_ALL) || defined(WOLFSSL_ASIO)
    {
        BIO* bioA = NULL;
        BIO* bioB = NULL;
        AssertIntEQ(BIO_new_bio_pair(NULL, 256, NULL, 256), BAD_FUNC_ARG);
        AssertIntEQ(BIO_new_bio_pair(&bioA, 256, &bioB, 256), WOLFSSL_SUCCESS);
        BIO_free(bioA);
        bioA = NULL;
        BIO_free(bioB);
        bioB = NULL;
    }
    #endif /* OPENSSL_ALL || WOLFSSL_ASIO */

    /* BIOs with file pointers */
    #if !defined(NO_FILESYSTEM)
    {
        XFILE f1;
        XFILE f2;
        BIO*  f_bio1;
        BIO*  f_bio2;
        unsigned char cert[300];
        char testFile[] = "tests/bio_write_test.txt";
        char msg[]      = "bio_write_test.txt contains the first 300 bytes of certs/server-cert.pem\ncreated by tests/unit.test\n\n";

        AssertNotNull(f_bio1 = BIO_new(BIO_s_file()));
        AssertNotNull(f_bio2 = BIO_new(BIO_s_file()));

        AssertIntEQ((int)BIO_set_mem_eof_return(f_bio1, -1), 0);
        AssertIntEQ((int)BIO_set_mem_eof_return(NULL, -1),   0);

        f1 = XFOPEN(svrCertFile, "rwb");
        AssertTrue((f1 != XBADFILE));
        AssertIntEQ((int)BIO_set_fp(f_bio1, f1, BIO_CLOSE), WOLFSSL_SUCCESS);
        AssertIntEQ(BIO_write_filename(f_bio2, testFile),
                WOLFSSL_SUCCESS);

        AssertIntEQ(BIO_read(f_bio1, cert, sizeof(cert)), sizeof(cert));
        AssertIntEQ(BIO_write(f_bio2, msg, sizeof(msg)), sizeof(msg));
        AssertIntEQ(BIO_write(f_bio2, cert, sizeof(cert)), sizeof(cert));

        AssertIntEQ((int)BIO_get_fp(f_bio2, &f2), WOLFSSL_SUCCESS);
        AssertIntEQ(BIO_reset(f_bio2), 0);
        AssertIntEQ(BIO_seek(f_bio2, 4), 0);

        BIO_free(f_bio1);
        BIO_free(f_bio2);

        AssertNotNull(f_bio1 = BIO_new_file(svrCertFile, "rwb"));
        AssertIntEQ((int)BIO_set_mem_eof_return(f_bio1, -1), 0);
        AssertIntEQ(BIO_read(f_bio1, cert, sizeof(cert)), sizeof(cert));
        BIO_free(f_bio1);

    }
    #endif /* !defined(NO_FILESYSTEM) */

    /* BIO info callback */
    {
        const char* testArg = "test";
        BIO* cb_bio;
        AssertNotNull(cb_bio = BIO_new(BIO_s_mem()));

        BIO_set_callback(cb_bio, bioCallback);
        AssertNotNull(BIO_get_callback(cb_bio));
        BIO_set_callback(cb_bio, NULL);
        AssertNull(BIO_get_callback(cb_bio));

        BIO_set_callback_arg(cb_bio, (char*)testArg);
        AssertStrEQ(BIO_get_callback_arg(cb_bio), testArg);
        AssertNull(BIO_get_callback_arg(NULL));

        BIO_free(cb_bio);
    }

    /* BIO_vfree */
    AssertNotNull(bio1 = BIO_new(BIO_s_bio()));
    BIO_vfree(NULL);
    BIO_vfree(bio1);

    printf(resultFmt, passed);
    #endif
}


static void test_wolfSSL_ASN1_STRING(void)
{
    #if defined(OPENSSL_EXTRA)
    ASN1_STRING* str = NULL;
    const char data[] = "hello wolfSSL";

    printf(testingFmt, "wolfSSL_ASN1_STRING()");

    AssertNotNull(str = ASN1_STRING_type_new(V_ASN1_OCTET_STRING));
    AssertIntEQ(ASN1_STRING_type(str), V_ASN1_OCTET_STRING);
    AssertIntEQ(ASN1_STRING_set(str, (const void*)data, sizeof(data)), 1);
    AssertIntEQ(ASN1_STRING_set(str, (const void*)data, -1), 1);
    AssertIntEQ(ASN1_STRING_set(str, NULL, -1), 0);

    ASN1_STRING_free(str);

    printf(resultFmt, passed);
    #endif
}

static void test_wolfSSL_ASN1_BIT_STRING(void)
{
#ifdef OPENSSL_ALL
    ASN1_BIT_STRING* str;

    printf(testingFmt, "test_wolfSSL_ASN1_BIT_STRING()");
    AssertNotNull(str = ASN1_BIT_STRING_new());

    AssertIntEQ(ASN1_BIT_STRING_set_bit(str, 42, 1), 1);
    AssertIntEQ(ASN1_BIT_STRING_get_bit(str, 42), 1);
    AssertIntEQ(ASN1_BIT_STRING_get_bit(str, 41), 0);
    AssertIntEQ(ASN1_BIT_STRING_set_bit(str, 84, 1), 1);
    AssertIntEQ(ASN1_BIT_STRING_get_bit(str, 84), 1);
    AssertIntEQ(ASN1_BIT_STRING_get_bit(str, 83), 0);

    ASN1_BIT_STRING_free(str);
    printf(resultFmt, passed);
#endif
}


static void test_wolfSSL_DES_ecb_encrypt(void)
{
    #if defined(OPENSSL_EXTRA) && !defined(NO_DES3) && defined(WOLFSSL_DES_ECB)
    WOLFSSL_DES_cblock input1,input2,output1,output2,back1,back2;
    WOLFSSL_DES_key_schedule key;

    printf(testingFmt, "wolfSSL_DES_ecb_encrypt()");

    XMEMCPY(key,"12345678",sizeof(WOLFSSL_DES_key_schedule));
    XMEMCPY(input1, "Iamhuman",sizeof(WOLFSSL_DES_cblock));
    XMEMCPY(input2, "Whoisit?",sizeof(WOLFSSL_DES_cblock));
    XMEMSET(output1, 0, sizeof(WOLFSSL_DES_cblock));
    XMEMSET(output2, 0, sizeof(WOLFSSL_DES_cblock));
    XMEMSET(back1, 0, sizeof(WOLFSSL_DES_cblock));
    XMEMSET(back2, 0, sizeof(WOLFSSL_DES_cblock));

    /* Encrypt messages */
    wolfSSL_DES_ecb_encrypt(&input1,&output1,&key,DES_ENCRYPT);
    wolfSSL_DES_ecb_encrypt(&input2,&output2,&key,DES_ENCRYPT);

    /* Decrypt messages */
    int ret1 = 0;
    int ret2 = 0;
    wolfSSL_DES_ecb_encrypt(&output1,&back1,&key,DES_DECRYPT);
    ret1 = XMEMCMP((unsigned char *) back1,(unsigned char *) input1,sizeof(WOLFSSL_DES_cblock));
    AssertIntEQ(ret1,0);
    wolfSSL_DES_ecb_encrypt(&output2,&back2,&key,DES_DECRYPT);
    ret2 = XMEMCMP((unsigned char *) back2,(unsigned char *) input2,sizeof(WOLFSSL_DES_cblock));
    AssertIntEQ(ret2,0);

    printf(resultFmt, passed);
    #endif
}

static void test_wolfSSL_ASN1_TIME_adj(void)
{
#if defined(OPENSSL_EXTRA) && !defined(NO_ASN1_TIME) \
&& !defined(USER_TIME) && !defined(TIME_OVERRIDES)

    const int year = 365*24*60*60;
    const int day  = 24*60*60;
    const int hour = 60*60;
    const int mini = 60;
    const byte asn_utc_time = ASN_UTC_TIME;
#if !defined(TIME_T_NOT_64BIT) && !defined(NO_64BIT)
    const byte asn_gen_time = ASN_GENERALIZED_TIME;
#endif
    WOLFSSL_ASN1_TIME *asn_time, *s;
    int offset_day;
    long offset_sec;
    char date_str[CTC_DATE_SIZE];
    time_t t;

    printf(testingFmt, "wolfSSL_ASN1_TIME_adj()");

    s = (WOLFSSL_ASN1_TIME*)XMALLOC(sizeof(WOLFSSL_ASN1_TIME), NULL,
                                    DYNAMIC_TYPE_OPENSSL);
    /* UTC notation test */
    /* 2000/2/15 20:30:00 */
    t = (time_t)30 * year + 45 * day + 20 * hour + 30 * mini + 7 * day;
    offset_day = 7;
    offset_sec = 45 * mini;
    /* offset_sec = -45 * min;*/
    asn_time = wolfSSL_ASN1_TIME_adj(s, t, offset_day, offset_sec);
    AssertTrue(asn_time->type == asn_utc_time);
    XSTRNCPY(date_str, (const char*)&asn_time->data, sizeof(date_str));
    AssertIntEQ(0, XMEMCMP(date_str, "000222211500Z", 13));

    /* negative offset */
    offset_sec = -45 * mini;
    asn_time = wolfSSL_ASN1_TIME_adj(s, t, offset_day, offset_sec);
    AssertTrue(asn_time->type == asn_utc_time);
    XSTRNCPY(date_str, (const char*)&asn_time->data, sizeof(date_str));
    AssertIntEQ(0, XMEMCMP(date_str, "000222194500Z", 13));

    XFREE(s, NULL, DYNAMIC_TYPE_OPENSSL);
    XMEMSET(date_str, 0, sizeof(date_str));

    /* Generalized time will overflow time_t if not long */
#if !defined(TIME_T_NOT_64BIT) && !defined(NO_64BIT)
    s = (WOLFSSL_ASN1_TIME*)XMALLOC(sizeof(WOLFSSL_ASN1_TIME), NULL,
                                    DYNAMIC_TYPE_OPENSSL);
    /* GeneralizedTime notation test */
    /* 2055/03/01 09:00:00 */
    t = (time_t)85 * year + 59 * day + 9 * hour + 21 * day;
        offset_day = 12;
        offset_sec = 10 * mini;
    asn_time = wolfSSL_ASN1_TIME_adj(s, t, offset_day, offset_sec);
    AssertTrue(asn_time->type == asn_gen_time);
    XSTRNCPY(date_str, (const char*)&asn_time->data, sizeof(date_str));
    AssertIntEQ(0, XMEMCMP(date_str, "20550313091000Z", 15));

    XFREE(s, NULL, DYNAMIC_TYPE_OPENSSL);
    XMEMSET(date_str, 0, sizeof(date_str));
#endif /* !TIME_T_NOT_64BIT && !NO_64BIT */

    /* if WOLFSSL_ASN1_TIME struct is not allocated */
    s = NULL;

    t = (time_t)30 * year + 45 * day + 20 * hour + 30 * mini + 15 + 7 * day;
    offset_day = 7;
    offset_sec = 45 * mini;
    asn_time = wolfSSL_ASN1_TIME_adj(s, t, offset_day, offset_sec);
    AssertTrue(asn_time->type == asn_utc_time);
    XSTRNCPY(date_str, (const char*)&asn_time->data, sizeof(date_str));
    AssertIntEQ(0, XMEMCMP(date_str, "000222211515Z", 13));
    XFREE(asn_time, NULL, DYNAMIC_TYPE_OPENSSL);

    asn_time = wolfSSL_ASN1_TIME_adj(NULL, t, offset_day, offset_sec);
    AssertTrue(asn_time->type == asn_utc_time);
    XSTRNCPY(date_str, (const char*)&asn_time->data, sizeof(date_str));
    AssertIntEQ(0, XMEMCMP(date_str, "000222211515Z", 13));
    XFREE(asn_time, NULL, DYNAMIC_TYPE_OPENSSL);

    printf(resultFmt, passed);
#endif
}


static void test_wolfSSL_X509_cmp_time(void)
{
#if defined(OPENSSL_EXTRA) && !defined(NO_ASN1_TIME) \
&& !defined(USER_TIME) && !defined(TIME_OVERRIDES)
    WOLFSSL_ASN1_TIME asn_time;
    time_t t;

    printf(testingFmt, "wolfSSL_X509_cmp_time()");

    AssertIntEQ(0, wolfSSL_X509_cmp_time(NULL, &t));
    XMEMSET(&asn_time, 0, sizeof(WOLFSSL_ASN1_TIME));
    AssertIntEQ(0, wolfSSL_X509_cmp_time(&asn_time, &t));

    asn_time.type = ASN_UTC_TIME;
    asn_time.length = ASN_UTC_TIME_SIZE;
    XMEMCPY(&asn_time.data, "000222211515Z", 13);
    AssertIntEQ(-1, wolfSSL_X509_cmp_time(&asn_time, NULL));

    printf(resultFmt, passed);
#endif
}

static void test_wolfSSL_X509_time_adj(void)
{
#if defined(OPENSSL_EXTRA) && !defined(NO_ASN1_TIME) && \
    !defined(USER_TIME) && !defined(TIME_OVERRIDES) && \
    defined(USE_CERT_BUFFERS_2048) && !defined(NO_RSA)
    X509*  x509;
    time_t t, not_before, not_after;

    printf(testingFmt, "wolfSSL_X509_time_adj()");

    AssertNotNull(x509 = wolfSSL_X509_load_certificate_buffer(
        client_cert_der_2048, sizeof_client_cert_der_2048,
        WOLFSSL_FILETYPE_ASN1));

    t = 0;
    not_before = XTIME(0);
    not_after = XTIME(0) + (60 * 24 * 30); /* 30 days after */
    AssertNotNull(X509_time_adj(X509_get_notBefore(x509), not_before, &t));
    AssertNotNull(X509_time_adj(X509_get_notAfter(x509), not_after, &t));

    X509_free(x509);

    printf(resultFmt, passed);
#endif
}


static void test_wolfSSL_X509(void)
{
    #if defined(OPENSSL_EXTRA) && !defined(NO_CERTS) && !defined(NO_FILESYSTEM)\
    && !defined(NO_RSA)
    X509* x509;
    BIO*  bio;
    X509_STORE_CTX* ctx;
    X509_STORE* store;

    char der[] = "certs/ca-cert.der";
    XFILE fp;

    printf(testingFmt, "wolfSSL_X509()");

    AssertNotNull(x509 = X509_new());
    X509_free(x509);

    x509 = wolfSSL_X509_load_certificate_file(cliCertFile, SSL_FILETYPE_PEM);

    AssertNotNull(bio = BIO_new(BIO_s_mem()));

    AssertIntEQ(i2d_X509_bio(bio, x509), SSL_SUCCESS);

    AssertNotNull(ctx = X509_STORE_CTX_new());

    AssertIntEQ(X509_verify_cert(ctx), SSL_FATAL_ERROR);

    AssertNotNull(store = X509_STORE_new());
    AssertIntEQ(X509_STORE_add_cert(store, x509), SSL_SUCCESS);
    AssertIntEQ(X509_STORE_CTX_init(ctx, store, x509, NULL), SSL_SUCCESS);
    AssertIntEQ(X509_verify_cert(ctx), SSL_SUCCESS);


    X509_STORE_CTX_free(ctx);
    #if defined(WOLFSSL_KEEP_STORE_CERTS) || defined(WOLFSSL_QT)
    X509_STORE_free(store);
    X509_free(x509);
    #endif
    BIO_free(bio);

    /** d2i_X509_fp test **/
    fp = XFOPEN(der, "rb");
    AssertTrue((fp != XBADFILE));
    AssertNotNull(x509 = (X509 *)d2i_X509_fp(fp, (X509 **)NULL));
    AssertNotNull(x509);
    X509_free(x509);
    XFCLOSE(fp);
    fp = XFOPEN(der, "rb");
    AssertTrue((fp != XBADFILE));
    AssertNotNull((X509 *)d2i_X509_fp(fp, (X509 **)&x509));
    AssertNotNull(x509);
    X509_free(x509);
    XFCLOSE(fp);

    /* X509_up_ref test */
    AssertIntEQ(X509_up_ref(NULL), 0);
    AssertNotNull(x509 = X509_new());   /* refCount = 1 */
    AssertIntEQ(X509_up_ref(x509), 1);  /* refCount = 2 */
    AssertIntEQ(X509_up_ref(x509), 1);  /* refCount = 3 */
    X509_free(x509); /* refCount = 2 */
    X509_free(x509); /* refCount = 1 */
    X509_free(x509); /* refCount = 0, free */

    printf(resultFmt, passed);
    #endif
}

static void test_wolfSSL_X509_get_ext_count(void)
{
#if defined(OPENSSL_ALL) && !defined(NO_CERTS) && !defined(NO_FILESYSTEM)
    int ret = 0;
    WOLFSSL_X509* x509;
    const char ocspRootCaFile[] = "./certs/ocsp/root-ca-cert.pem";
    FILE* f;

    printf(testingFmt, "wolfSSL_X509_get_ext_count()");

    /* NULL parameter check */
    AssertIntEQ(X509_get_ext_count(NULL), WOLFSSL_FAILURE);

    AssertNotNull(x509 = wolfSSL_X509_load_certificate_file(svrCertFile,
                                                             SSL_FILETYPE_PEM));
    AssertIntEQ(X509_get_ext_count(x509), 3);
    wolfSSL_X509_free(x509);

    AssertNotNull(x509 = wolfSSL_X509_load_certificate_file(ocspRootCaFile,
                                                             SSL_FILETYPE_PEM));
    AssertIntEQ(X509_get_ext_count(x509), 5);
    wolfSSL_X509_free(x509);

    AssertNotNull(f = fopen("./certs/server-cert.pem", "rb"));
    AssertNotNull(x509 = wolfSSL_PEM_read_X509(f, NULL, NULL, NULL));
    fclose(f);

    printf(testingFmt, "wolfSSL_X509_get_ext_count() valid input");
    AssertIntEQ((ret = wolfSSL_X509_get_ext_count(x509)), 3);
    printf(resultFmt, ret == 3 ? passed : failed);

    printf(testingFmt, "wolfSSL_X509_get_ext_count() NULL argument");
    AssertIntEQ((ret = wolfSSL_X509_get_ext_count(NULL)), WOLFSSL_FAILURE);
    printf(resultFmt, ret == WOLFSSL_FAILURE ? passed : failed);

    wolfSSL_X509_free(x509);

    printf(resultFmt, passed);
#endif
}

static void test_wolfSSL_X509_sign(void)
{
#if defined(OPENSSL_EXTRA) && !defined(NO_CERTS) && \
    defined(WOLFSSL_CERT_GEN) && defined(WOLFSSL_CERT_REQ)
    int ret;
    X509_NAME *name;
    X509 *x509;
    EVP_PKEY *pub;
    EVP_PKEY *priv;
#if defined(USE_CERT_BUFFERS_1024)
    const unsigned char* rsaPriv = (const unsigned char*)client_key_der_1024;
    unsigned char* rsaPub = (unsigned char*)client_keypub_der_1024;
    long clientKeySz = (long)sizeof_client_key_der_1024;
    long clientPubKeySz = (long)sizeof_client_keypub_der_1024;
#elif defined(USE_CERT_BUFFERS_2048)
    const unsigned char* rsaPriv = (const unsigned char*)client_key_der_2048;
    unsigned char* rsaPub = (unsigned char*)client_keypub_der_2048;
    long clientKeySz = (long)sizeof_client_key_der_2048;
    long clientPubKeySz = (long)sizeof_client_keypub_der_2048;
#endif

    printf(testingFmt, "wolfSSL_X509_sign\n");

    /* Set X509_NAME fields */
    AssertNotNull(name = X509_NAME_new());
    AssertIntEQ(X509_NAME_add_entry_by_txt(name, "country", MBSTRING_UTF8,
                                       (byte*)"US", 2, -1, 0), SSL_SUCCESS);
    AssertIntEQ(X509_NAME_add_entry_by_txt(name, "commonName", MBSTRING_UTF8,
                             (byte*)"wolfssl.com", 11, -1, 0), SSL_SUCCESS);
    AssertIntEQ(X509_NAME_add_entry_by_txt(name, "emailAddress", MBSTRING_UTF8,
                     (byte*)"support@wolfssl.com", 19, -1, 0), SSL_SUCCESS);

    /* Get private and public keys */
    AssertNotNull(priv = wolfSSL_d2i_PrivateKey(EVP_PKEY_RSA, NULL, &rsaPriv,
                                                                  clientKeySz));
    AssertNotNull(pub = wolfSSL_d2i_PUBKEY(NULL, &rsaPub, clientPubKeySz));
    AssertNotNull(x509 = X509_new());
    /* Set version 3 */
    AssertIntNE(X509_set_version(x509, 2L), 0);
    /* Set subject name, add pubkey, and sign certificate */
    AssertIntEQ(X509_set_subject_name(x509, name), SSL_SUCCESS);
    AssertIntEQ(X509_set_pubkey(x509, pub), SSL_SUCCESS);
    /* Test invalid parameters */
    AssertIntEQ(X509_sign(NULL, priv, EVP_sha256()), 0);
    AssertIntEQ(X509_sign(x509, NULL, EVP_sha256()), 0);
    AssertIntEQ(X509_sign(x509, priv, NULL), 0);

    ret = X509_sign(x509, priv, EVP_sha256());

#if 0
    /* example for writting to file */
    XFILE tmpFile = XFOPEN("./signed.der", "wb");
    if (tmpFile) {
        int derSz = 0;
        const byte* der = wolfSSL_X509_get_der(x509, &derSz);
        XFWRITE(der, 1, derSz, tmpFile);
    }
    XFCLOSE(tmpFile);
#endif

    /* Valid case - size should be 798 */
    AssertIntEQ(ret, 798);

    X509_NAME_free(name);
    EVP_PKEY_free(priv);
    EVP_PKEY_free(pub);
    X509_free(x509);

    printf(resultFmt, passed);
#endif
}

static void test_wolfSSL_X509_get0_tbs_sigalg(void)
{
#if (defined(OPENSSL_ALL) || defined(WOLFSSL_APACHE_HTTPD))
    X509* x509 = NULL;
    const X509_ALGOR* alg;
    printf(testingFmt, "wolfSSL_X509_get0_tbs_sigalg");

    AssertNotNull(x509 = X509_new());

    AssertNull(alg = X509_get0_tbs_sigalg(NULL));
    AssertNotNull(alg = X509_get0_tbs_sigalg(x509));

    X509_free(x509);

    printf(resultFmt, passed);
#endif
}

static void test_wolfSSL_X509_ALGOR_get0(void)
{
#if (defined(OPENSSL_ALL) || defined(WOLFSSL_APACHE_HTTPD)) && !defined(NO_SHA256)
    X509* x509 = NULL;
    const ASN1_OBJECT* obj = NULL;
    const X509_ALGOR* alg;
    printf(testingFmt, "wolfSSL_X509_ALGOR_get0");

    AssertNotNull(x509 = wolfSSL_X509_load_certificate_file(cliCertFile,
                                                             SSL_FILETYPE_PEM));
    AssertNotNull(alg = X509_get0_tbs_sigalg(x509));

    /* Invalid case */
    X509_ALGOR_get0(&obj, NULL, NULL, NULL);
    AssertNull(obj);

    /* Valid case */
    X509_ALGOR_get0(&obj, NULL, NULL, alg);
    AssertNotNull(obj);
    /* Make sure NID of X509_ALGOR is Sha256 with RSA */
    AssertIntEQ(OBJ_obj2nid(obj), CTC_SHA256wRSA);

    X509_free(x509);

    printf(resultFmt, passed);
#endif
}


static void test_wolfSSL_X509_VERIFY_PARAM(void)
{
#if defined(OPENSSL_EXTRA)
    WOLFSSL_X509_VERIFY_PARAM *param;
    int ret;
    char testIPv4[] = "127.0.0.1";
    char testIPv6[] = "0001:0000:0000:0000:0000:0000:0000:0000/32";

    printf(testingFmt, "wolfSSL_X509()");

    /* Initializer function is not ported */
    /* param = wolfSSL_X509_VERIFY_PARAM_new(); */

    param = (WOLFSSL_X509_VERIFY_PARAM *)XMALLOC(
                sizeof(WOLFSSL_X509_VERIFY_PARAM), NULL, DYNAMIC_TYPE_OPENSSL);
    AssertNotNull(param);

    XMEMSET(param, 0, sizeof(WOLFSSL_X509_VERIFY_PARAM ));

    wolfSSL_X509_VERIFY_PARAM_set_hostflags(NULL, 0x00);

    wolfSSL_X509_VERIFY_PARAM_set_hostflags(param, 0x01);
    AssertIntEQ(0x01, param->hostFlags);

    ret = wolfSSL_X509_VERIFY_PARAM_set1_ip_asc(NULL, testIPv4);
    AssertIntEQ(0, ret);

    ret = wolfSSL_X509_VERIFY_PARAM_set1_ip_asc(param, testIPv4);
    AssertIntEQ(1, ret);
    AssertIntEQ(0, XSTRNCMP(param->ipasc, testIPv4, WOLFSSL_MAX_IPSTR));

    ret = wolfSSL_X509_VERIFY_PARAM_set1_ip_asc(param, NULL);
    AssertIntEQ(1, ret);

    ret = wolfSSL_X509_VERIFY_PARAM_set1_ip_asc(param, testIPv6);
    AssertIntEQ(1, ret);
    AssertIntEQ(0, XSTRNCMP(param->ipasc, testIPv6, WOLFSSL_MAX_IPSTR));

    XFREE(param, NULL, DYNAMIC_TYPE_OPENSSL);

    printf(resultFmt, passed);

#endif
}

static void test_wolfSSL_X509_get_X509_PUBKEY(void)
{
#if (defined(OPENSSL_ALL) || defined(WOLFSSL_APACHE_HTTPD))
    X509* x509 = NULL;
    X509_PUBKEY* pubKey;
    printf(testingFmt, "wolfSSL_X509_get_X509_PUBKEY");

    AssertNotNull(x509 = X509_new());

    AssertNull(pubKey = wolfSSL_X509_get_X509_PUBKEY(NULL));
    AssertNotNull(pubKey = wolfSSL_X509_get_X509_PUBKEY(x509));

    X509_free(x509);

    printf(resultFmt, passed);
#endif
}

static void test_wolfSSL_X509_PUBKEY_get0_param(void)
{
#if (defined(OPENSSL_ALL) || defined(WOLFSSL_APACHE_HTTPD)) && !defined(NO_SHA256)
    X509* x509 = NULL;
    ASN1_OBJECT* obj = NULL;
    X509_PUBKEY* pubKey;
    printf(testingFmt, "wolfSSL_X509_get_X509_PUBKEY");

    AssertNotNull(x509 = wolfSSL_X509_load_certificate_file(cliCertFile,
                                                             SSL_FILETYPE_PEM));

    AssertNotNull(pubKey = wolfSSL_X509_get_X509_PUBKEY(x509));
    X509_PUBKEY_get0_param(&obj, NULL, 0, NULL, pubKey);
    AssertNotNull(pubKey);

    AssertIntEQ(OBJ_obj2nid(obj), RSAk);

    X509_free(x509);
//    AssertIntEQ(1,0);

    printf(resultFmt, passed);
#endif
}

static void test_wolfSSL_RAND(void)
{
    #if defined(OPENSSL_EXTRA)
    byte seed[16];

    printf(testingFmt, "wolfSSL_RAND()");

    RAND_seed(seed, sizeof(seed));
    AssertIntEQ(RAND_poll(), 1);
    RAND_cleanup();

    AssertIntEQ(RAND_egd(NULL), -1);
#ifndef NO_FILESYSTEM
    {
        char fname[100];

        AssertNotNull(RAND_file_name(fname, (sizeof(fname) - 1)));
        AssertIntEQ(RAND_write_file(NULL), 0);
    }
#endif

    printf(resultFmt, passed);
    #endif
}


static void test_wolfSSL_BUF(void)
{
    #if defined(OPENSSL_EXTRA)
    BUF_MEM* buf;
    AssertNotNull(buf = BUF_MEM_new());
    AssertIntEQ(BUF_MEM_grow(buf, 10), 10);
    AssertIntEQ(BUF_MEM_grow(buf, -1), 0);
    BUF_MEM_free(buf);
    #endif /* OPENSSL_EXTRA */
}


static void test_wolfSSL_pseudo_rand(void)
{
    #if defined(OPENSSL_EXTRA)
    BIGNUM* bn;
    unsigned char bin[8];
    int i;

    printf(testingFmt, "wolfSSL_pseudo_rand()");

    /* BN_pseudo_rand returns 1 on success 0 on failure
     * int BN_pseudo_rand(BIGNUM* bn, int bits, int top, int bottom) */
    for (i = 0; i < 10; i++) {
        AssertNotNull(bn = BN_new());
        AssertIntEQ(BN_pseudo_rand(bn, 8, 0, 0), SSL_SUCCESS);
        AssertIntGT(BN_bn2bin(bn, bin),0);
        AssertIntEQ((bin[0] & 0x80), 0x80); /* top bit should be set */
        BN_free(bn);
    }

    for (i = 0; i < 10; i++) {
        AssertNotNull(bn = BN_new());
        AssertIntEQ(BN_pseudo_rand(bn, 8, 1, 1), SSL_SUCCESS);
        AssertIntGT(BN_bn2bin(bn, bin),0);
        AssertIntEQ((bin[0] & 0xc1), 0xc1); /* top bit should be set */
        BN_free(bn);
    }

    printf(resultFmt, passed);
    #endif
}

static void test_wolfSSL_PKCS8_Compat(void)
{
    #if defined(OPENSSL_EXTRA) && !defined(NO_FILESYSTEM) && defined(HAVE_ECC)
    PKCS8_PRIV_KEY_INFO* pt;
    BIO* bio;
    XFILE f;
    int bytes;
    char pkcs8_buffer[512];

    printf(testingFmt, "wolfSSL_pkcs8()");

    /* file from wolfssl/certs/ directory */
    f = XFOPEN("./certs/ecc-keyPkcs8.pem", "rb");
    AssertTrue(f != XBADFILE);
    AssertIntGT((bytes = (int)XFREAD(pkcs8_buffer, 1, sizeof(pkcs8_buffer), f)), 0);
    XFCLOSE(f);
    AssertNotNull(bio = BIO_new_mem_buf((void*)pkcs8_buffer, bytes));
    AssertNotNull(pt = d2i_PKCS8_PRIV_KEY_INFO_bio(bio, NULL));
    BIO_free(bio);
    PKCS8_PRIV_KEY_INFO_free(pt);

    printf(resultFmt, passed);
    #endif
}

static void test_wolfSSL_PKCS8_d2i(void)
{
#ifndef HAVE_FIPS
    /* This test ends up using HMAC as a part of PBKDF2, and HMAC
     * requires a 12 byte password in FIPS mode. This test ends up
     * trying to use an 8 byte password. */
#ifdef OPENSSL_ALL
    WOLFSSL_EVP_PKEY* pkey = NULL;
#ifndef NO_FILESYSTEM
    unsigned char pkcs8_buffer[2048];
    const unsigned char* p;
    int bytes;
    XFILE file;
    BIO* bio;
    WOLFSSL_EVP_PKEY* evpPkey = NULL;
#endif
    #ifndef NO_RSA
        #ifndef NO_FILESYSTEM
    const char rsaDerPkcs8File[] = "./certs/server-keyPkcs8.der";
    const char rsaPemPkcs8File[] = "./certs/server-keyPkcs8.pem";
#ifndef NO_DES3
    const char rsaDerPkcs8EncFile[] = "./certs/server-keyPkcs8Enc.der";
#endif
        #endif
        #ifdef USE_CERT_BUFFERS_1024
    const unsigned char* rsa = (unsigned char*)server_key_der_1024;
    int rsaSz = sizeof_server_key_der_1024;
        #else
    const unsigned char* rsa = (unsigned char*)server_key_der_2048;
    int rsaSz = sizeof_server_key_der_2048;
        #endif
    #endif
    #ifdef HAVE_ECC
    const unsigned char* ec = (unsigned char*)ecc_key_der_256;
    int ecSz = sizeof_ecc_key_der_256;
        #ifndef NO_FILESYSTEM
    const char ecDerPkcs8File[] = "certs/ecc-keyPkcs8.der";
    const char ecPemPkcs8File[] = "certs/ecc-keyPkcs8.pem";
#ifndef NO_DES3
    const char ecDerPkcs8EncFile[] = "certs/ecc-keyPkcs8Enc.der";
#endif
        #endif
    #endif

    #ifndef NO_RSA
    /* Try to auto-detect normal RSA private key */
    AssertNotNull(pkey = d2i_AutoPrivateKey(NULL, &rsa, rsaSz));
    wolfSSL_EVP_PKEY_free(pkey);
    #endif
    #ifdef HAVE_ECC
    /* Try to auto-detect normal EC private key */
    AssertNotNull(pkey = d2i_AutoPrivateKey(NULL, &ec, ecSz));
    wolfSSL_EVP_PKEY_free(pkey);
    #endif
    #ifndef NO_FILESYSTEM
        #ifndef NO_RSA
    /* Get DER encoded RSA PKCS#8 data. */
    file = XFOPEN(rsaDerPkcs8File, "rb");
    AssertTrue(file != XBADFILE);
    AssertIntGT((bytes = (int)XFREAD(pkcs8_buffer, 1, sizeof(pkcs8_buffer),
                                                                     file)), 0);
    XFCLOSE(file);
    p = pkcs8_buffer;
    /* Try to decode - auto-detect key type. */
    AssertNotNull(pkey = d2i_AutoPrivateKey(NULL, &p, bytes));
    /* Get PEM encoded RSA PKCS#8 data. */
    file = XFOPEN(rsaPemPkcs8File, "rb");
    AssertTrue(file != XBADFILE);
    AssertIntGT((bytes = (int)XFREAD(pkcs8_buffer, 1, sizeof(pkcs8_buffer),
                                                                     file)), 0);
    XFCLOSE(file);
    AssertNotNull(bio = BIO_new(BIO_s_mem()));
    /* Write PKCS#8 PEM to BIO. */
    AssertIntEQ(PEM_write_bio_PKCS8PrivateKey(bio, pkey, NULL, NULL, 0, NULL,
                                                                  NULL), bytes);
    /* Compare file and written data */
    AssertIntEQ(wolfSSL_BIO_get_mem_data(bio, &p), bytes);
    AssertIntEQ(XMEMCMP(p, pkcs8_buffer, bytes), 0);
    BIO_free(bio);
#ifndef NO_DES3
    AssertNotNull(bio = BIO_new(BIO_s_mem()));
    /* Write Encrypted PKCS#8 PEM to BIO. */
    bytes = 1834;
    AssertIntEQ(PEM_write_bio_PKCS8PrivateKey(bio, pkey, EVP_des_ede3_cbc(),
                          NULL, 0, PasswordCallBack, (void*)"yassl123"), bytes);
    AssertNotNull(evpPkey = PEM_read_bio_PrivateKey(bio, NULL, PasswordCallBack,
                                                            (void*)"yassl123"));
    wolfSSL_EVP_PKEY_free(evpPkey);
    BIO_free(bio);
#endif
    wolfSSL_EVP_PKEY_free(pkey);

    /* PKCS#8 encrypted RSA key */
#ifndef NO_DES3
    file = XFOPEN(rsaDerPkcs8EncFile, "rb");
    AssertTrue(file != XBADFILE);
    AssertIntGT((bytes = (int)XFREAD(pkcs8_buffer, 1, sizeof(pkcs8_buffer),
                                                                     file)), 0);
    XFCLOSE(file);
    AssertNotNull(bio = BIO_new_mem_buf((void*)pkcs8_buffer, bytes));
    AssertNotNull(pkey = d2i_PKCS8PrivateKey_bio(bio, NULL, PasswordCallBack,
                                                            (void*)"yassl123"));
    wolfSSL_EVP_PKEY_free(pkey);
    BIO_free(bio);
#endif
        #endif
        #ifdef HAVE_ECC
    /* PKCS#8 encode EC key */
    file = XFOPEN(ecDerPkcs8File, "rb");
    AssertTrue(file != XBADFILE);
    AssertIntGT((bytes = (int)XFREAD(pkcs8_buffer, 1, sizeof(pkcs8_buffer),
                                                                     file)), 0);
    XFCLOSE(file);
    p = pkcs8_buffer;
    /* Try to decode - auto-detect key type. */
    AssertNotNull(pkey = d2i_AutoPrivateKey(NULL, &p, bytes));
    /* Get PEM encoded RSA PKCS#8 data. */
    file = XFOPEN(ecPemPkcs8File, "rb");
    AssertTrue(file != XBADFILE);
    AssertIntGT((bytes = (int)XFREAD(pkcs8_buffer, 1, sizeof(pkcs8_buffer),
                                                                     file)), 0);
    XFCLOSE(file);
    AssertNotNull(bio = BIO_new(BIO_s_mem()));
    /* Write PKCS#8 PEM to BIO. */
    AssertIntEQ(PEM_write_bio_PKCS8PrivateKey(bio, pkey, NULL, NULL, 0, NULL,
                                                                  NULL), bytes);
    /* Compare file and written data */
    AssertIntEQ(wolfSSL_BIO_get_mem_data(bio, &p), bytes);
    AssertIntEQ(XMEMCMP(p, pkcs8_buffer, bytes), 0);
    BIO_free(bio);
    AssertNotNull(bio = BIO_new(BIO_s_mem()));
    /* Write Encrypted PKCS#8 PEM to BIO. */
    bytes = 379;
    AssertIntEQ(PEM_write_bio_PKCS8PrivateKey(bio, pkey, EVP_aes_256_cbc(),
                          NULL, 0, PasswordCallBack, (void*)"yassl123"), bytes);
    AssertNotNull(evpPkey = PEM_read_bio_PrivateKey(bio, NULL, PasswordCallBack,
                                                            (void*)"yassl123"));
    wolfSSL_EVP_PKEY_free(evpPkey);
    BIO_free(bio);
    wolfSSL_EVP_PKEY_free(pkey);

    /* PKCS#8 encrypted EC key */
#ifndef NO_DES3
    file = XFOPEN(ecDerPkcs8EncFile, "rb");
    AssertTrue(file != XBADFILE);
    AssertIntGT((bytes = (int)XFREAD(pkcs8_buffer, 1, sizeof(pkcs8_buffer),
                                                                     file)), 0);
    XFCLOSE(file);
    AssertNotNull(bio = BIO_new_mem_buf((void*)pkcs8_buffer, bytes));
    AssertNotNull(pkey = d2i_PKCS8PrivateKey_bio(bio, NULL, PasswordCallBack,
                                                            (void*)"yassl123"));
    wolfSSL_EVP_PKEY_free(pkey);
    BIO_free(bio);
#endif
        #endif
    #endif

    printf(resultFmt, passed);
#endif
#endif /* HAVE_FIPS */
}

static void test_wolfSSL_ERR_put_error(void)
{
    #if defined(OPENSSL_EXTRA) && defined(DEBUG_WOLFSSL)
    const char* file;
    int line;

    printf(testingFmt, "wolfSSL_ERR_put_error()");


    ERR_clear_error(); /* clear out any error nodes */
    ERR_put_error(0,SYS_F_ACCEPT, 0, "this file", 0);
    AssertIntEQ(ERR_get_error_line(&file, &line), 0);
    ERR_put_error(0,SYS_F_BIND, 1, "this file", 1);
    AssertIntEQ(ERR_get_error_line(&file, &line), 1);
    ERR_put_error(0,SYS_F_CONNECT, 2, "this file", 2);
    AssertIntEQ(ERR_get_error_line(&file, &line), 2);
    ERR_put_error(0,SYS_F_FOPEN, 3, "this file", 3);
    AssertIntEQ(ERR_get_error_line(&file, &line), 3);
    ERR_put_error(0,SYS_F_FREAD, 4, "this file", 4);
    AssertIntEQ(ERR_get_error_line(&file, &line), 4);
    ERR_put_error(0,SYS_F_GETADDRINFO, 5, "this file", 5);
    AssertIntEQ(ERR_get_error_line(&file, &line), 5);
    ERR_put_error(0,SYS_F_GETSOCKOPT, 6, "this file", 6);
    AssertIntEQ(ERR_get_error_line(&file, &line), 6);
    ERR_put_error(0,SYS_F_GETSOCKNAME, 7, "this file", 7);
    AssertIntEQ(ERR_get_error_line(&file, &line), 7);
    ERR_put_error(0,SYS_F_GETHOSTBYNAME, 8, "this file", 8);
    AssertIntEQ(ERR_get_error_line(&file, &line), 8);
    ERR_put_error(0,SYS_F_GETNAMEINFO, 9, "this file", 9);
    AssertIntEQ(ERR_get_error_line(&file, &line), 9);
    ERR_put_error(0,SYS_F_GETSERVBYNAME, 10, "this file", 10);
    AssertIntEQ(ERR_get_error_line(&file, &line), 10);
    ERR_put_error(0,SYS_F_IOCTLSOCKET, 11, "this file", 11);
    AssertIntEQ(ERR_get_error_line(&file, &line), 11);
    ERR_put_error(0,SYS_F_LISTEN, 12, "this file", 12);
    AssertIntEQ(ERR_get_error_line(&file, &line), 12);
    ERR_put_error(0,SYS_F_OPENDIR, 13, "this file", 13);
    AssertIntEQ(ERR_get_error_line(&file, &line), 13);
    ERR_put_error(0,SYS_F_SETSOCKOPT, 14, "this file", 14);
    AssertIntEQ(ERR_get_error_line(&file, &line), 14);
    ERR_put_error(0,SYS_F_SOCKET, 15, "this file", 15);
    AssertIntEQ(ERR_get_error_line(&file, &line), 15);

    /* try reading past end of error queue */
    file = NULL;
    AssertIntEQ(ERR_get_error_line(&file, &line), 0);
    AssertNull(file);
    AssertIntEQ(ERR_get_error_line_data(&file, &line, NULL, NULL), 0);

    PEMerr(4,4);
    #if defined(OPENSSL_ALL) || defined(WOLFSSL_NGINX) || \
        defined(WOLFSSL_HAPROXY)
    AssertIntEQ(ERR_get_error(), -4);
    #else
    AssertIntEQ(ERR_get_error(), 4);
    #endif
    /* Empty and free up all error nodes */
    ERR_clear_error();

    /* Verify all nodes are cleared */
    ERR_put_error(0,SYS_F_ACCEPT, 0, "this file", 0);
    ERR_clear_error();
    AssertIntEQ(ERR_get_error_line(&file, &line), 0);

    printf(resultFmt, passed);
    #endif
}


static void test_wolfSSL_ERR_print_errors(void)
{
    #if defined(OPENSSL_EXTRA) && defined(DEBUG_WOLFSSL)
    BIO* bio;
    char buf[1024];

    printf(testingFmt, "wolfSSL_ERR_print_errors()");


    AssertNotNull(bio = BIO_new(BIO_s_mem()));
    ERR_clear_error(); /* clear out any error nodes */
    ERR_put_error(0,SYS_F_ACCEPT, -173, "ssl.c", 0);
    ERR_put_error(0,SYS_F_BIND, -275, "asn.c", 100);

    ERR_print_errors(bio);
    AssertIntEQ(BIO_gets(bio, buf, sizeof(buf)), 56);
    AssertIntEQ(XSTRNCMP("error:173:wolfSSL library:Bad function argument:ssl.c:0",
                buf, 55), 0);
    AssertIntEQ(BIO_gets(bio, buf, sizeof(buf)), 57);
    AssertIntEQ(XSTRNCMP("error:275:wolfSSL library:unknown error number:asn.c:100",
                buf, 56), 0);
    AssertIntEQ(BIO_gets(bio, buf, sizeof(buf)), 0);
    AssertIntEQ(ERR_get_error_line(NULL, NULL), 0);

    BIO_free(bio);
    printf(resultFmt, passed);
    #endif
}


static void test_wolfSSL_HMAC(void)
{
    #if defined(OPENSSL_EXTRA) && !defined(NO_SHA256)
    HMAC_CTX* hmac;
    ENGINE* e = NULL;
    const unsigned char key[] = "simple test key";
    unsigned char hash[WC_MAX_DIGEST_SIZE];
    unsigned int len;


    printf(testingFmt, "wolfSSL_HMAC()");

    AssertNotNull(hmac = HMAC_CTX_new());
    HMAC_CTX_init(hmac);
    AssertIntEQ(HMAC_Init_ex(hmac, (void*)key, (int)sizeof(key),
                EVP_sha256(), e), SSL_SUCCESS);

    /* re-using test key as data to hash */
    AssertIntEQ(HMAC_Update(hmac, key, (int)sizeof(key)), SSL_SUCCESS);
    AssertIntEQ(HMAC_Update(hmac, NULL, 0), SSL_SUCCESS);
    AssertIntEQ(HMAC_Final(hmac, hash, &len), SSL_SUCCESS);
    AssertIntEQ(len, (int)WC_SHA256_DIGEST_SIZE);
    AssertIntEQ(HMAC_size(hmac), WC_SHA256_DIGEST_SIZE);

    HMAC_cleanup(hmac);
    HMAC_CTX_free(hmac);
#endif

#if defined(OPENSSL_EXTRA) && !defined(NO_SHA256)
    len = 0;
    AssertNotNull(HMAC(EVP_sha256(), key, (int)sizeof(key), NULL, 0, hash, &len));
    AssertIntEQ(len, (int)WC_SHA256_DIGEST_SIZE);
#endif
#if defined(OPENSSL_EXTRA) && defined(WOLFSSL_SHA224)
    len = 0;
    AssertNotNull(HMAC(EVP_sha224(), key, (int)sizeof(key), NULL, 0, hash, &len));
    AssertIntEQ(len, (int)WC_SHA224_DIGEST_SIZE);
#endif
#if defined(OPENSSL_EXTRA) && defined(WOLFSSL_SHA384)
    len = 0;
    AssertNotNull(HMAC(EVP_sha384(), key, (int)sizeof(key), NULL, 0, hash, &len));
    AssertIntEQ(len, (int)WC_SHA384_DIGEST_SIZE);
#endif
#if defined(OPENSSL_EXTRA) && defined(WOLFSSL_SHA512)
    len = 0;
    AssertNotNull(HMAC(EVP_sha512(), key, (int)sizeof(key), NULL, 0, hash, &len));
    AssertIntEQ(len, (int)WC_SHA512_DIGEST_SIZE);
#endif

    printf(resultFmt, passed);

}


static void test_wolfSSL_OBJ(void)
{
/* Password "wolfSSL test" is only 12 (96-bit) too short for testing in FIPS
 * mode
 */
#if defined(OPENSSL_EXTRA) && !defined(NO_SHA256) && !defined(NO_ASN) && \
    !defined(HAVE_FIPS) && !defined(NO_SHA) && defined(WOLFSSL_CERT_EXT) && \
    defined(WOLFSSL_CERT_GEN)
    ASN1_OBJECT *obj = NULL;
    char buf[50];

    XFILE fp;
    X509 *x509 = NULL;
    X509_NAME *x509Name;
    X509_NAME_ENTRY *x509NameEntry;
    ASN1_OBJECT *asn1Name = NULL;
    int numNames;
    BIO *bio = NULL;
    int nid;
    int i, j;
    const char *f[] = {
        #ifndef NO_RSA
        "./certs/ca-cert.der",
        #endif
        #ifdef HAVE_ECC
        "./certs/ca-ecc-cert.der",
        "./certs/ca-ecc384-cert.der",
        #endif
        NULL};
    ASN1_OBJECT *field_name_obj = NULL;
    int lastpos = -1;
    int tmp = -1;
    ASN1_STRING *asn1 = NULL;
    unsigned char *buf_dyn = NULL;

    PKCS12 *p12;
    int boolRet;
    EVP_PKEY *pkey = NULL;
    const char *p12_f[] = {
        #if !defined(NO_DES3) && !defined(NO_RSA)
        "./certs/test-servercert.p12",
        #endif
        NULL};

    printf(testingFmt, "wolfSSL_OBJ()");

    AssertIntEQ(OBJ_obj2txt(buf, (int)sizeof(buf), obj, 1), SSL_FAILURE);
    AssertNotNull(obj = OBJ_nid2obj(NID_any_policy));
    AssertIntEQ(OBJ_obj2nid(obj), NID_any_policy);
    AssertIntEQ(OBJ_obj2txt(buf, (int)sizeof(buf), obj, 1), 11);
    AssertIntGT(OBJ_obj2txt(buf, (int)sizeof(buf), obj, 0), 0);
    ASN1_OBJECT_free(obj);

    AssertNotNull(obj = OBJ_nid2obj(NID_sha256));
    AssertIntEQ(OBJ_obj2nid(obj), NID_sha256);
    AssertIntEQ(OBJ_obj2txt(buf, (int)sizeof(buf), obj, 1), 22);
#ifdef WOLFSSL_CERT_EXT
    AssertIntEQ(OBJ_txt2nid(buf), NID_sha256);
#endif
    AssertIntGT(OBJ_obj2txt(buf, (int)sizeof(buf), obj, 0), 0);
    ASN1_OBJECT_free(obj);

    for (i = 0; f[i] != NULL; i++)
    {
        AssertTrue((fp = XFOPEN(f[i], "r")) != XBADFILE);
        AssertNotNull(x509 = d2i_X509_fp(fp, NULL));
        XFCLOSE(fp);
        AssertNotNull(x509Name = X509_get_issuer_name(x509));
        AssertIntNE((numNames = X509_NAME_entry_count(x509Name)), 0);

        /* Get the Common Name by using OBJ_txt2obj */
        AssertNotNull(field_name_obj = OBJ_txt2obj("CN", 0));
        do
        {
            lastpos = tmp;
            tmp = X509_NAME_get_index_by_OBJ(x509Name, field_name_obj, lastpos);
        } while (tmp > -1);
        AssertIntNE(lastpos, -1);
        ASN1_OBJECT_free(field_name_obj);
        AssertNotNull(x509NameEntry = X509_NAME_get_entry(x509Name, lastpos));
        AssertNotNull(asn1 = X509_NAME_ENTRY_get_data(x509NameEntry));
        AssertIntGE(ASN1_STRING_to_UTF8(&buf_dyn, asn1), 0);
        /*
         * All Common Names should be www.wolfssl.com
         * This makes testing easier as we can test for the expected value.
         */
        AssertStrEQ((char*)buf_dyn, "www.wolfssl.com");
        OPENSSL_free(buf_dyn);

        AssertTrue((bio = BIO_new(BIO_s_mem())) != NULL);
        for (j = 0; j < numNames; j++)
        {
            AssertNotNull(x509NameEntry = X509_NAME_get_entry(x509Name, j));
            AssertNotNull(asn1Name = X509_NAME_ENTRY_get_object(x509NameEntry));
            AssertTrue((nid = OBJ_obj2nid(asn1Name)) > 0);
        }
        BIO_free(bio);
        ASN1_OBJECT_free(asn1Name);
        X509_free(x509);

    }

    for (i = 0; p12_f[i] != NULL; i++)
    {
        AssertTrue((fp = XFOPEN(p12_f[i], "r")) != XBADFILE);
        AssertNotNull(p12 = d2i_PKCS12_fp(fp, NULL));
        XFCLOSE(fp);
        AssertTrue((boolRet = PKCS12_parse(p12, "wolfSSL test", &pkey, &x509, NULL)) > 0);
        wc_PKCS12_free(p12);
        EVP_PKEY_free(pkey);
        AssertNotNull((x509Name = X509_get_issuer_name(x509)) != NULL);
        AssertIntNE((numNames = X509_NAME_entry_count(x509Name)), 0);
        AssertTrue((bio = BIO_new(BIO_s_mem())) != NULL);
        for (j = 0; j < numNames; j++)
        {
            AssertNotNull(x509NameEntry = X509_NAME_get_entry(x509Name, j));
            AssertNotNull(asn1Name = X509_NAME_ENTRY_get_object(x509NameEntry));
            AssertTrue((nid = OBJ_obj2nid(asn1Name)) > 0);
        }
        BIO_free(bio);
        ASN1_OBJECT_free(asn1Name);
        X509_free(x509);
    }

    printf(resultFmt, passed);
#endif
}

static void test_wolfSSL_i2a_ASN1_OBJECT(void)
{
#if defined(OPENSSL_EXTRA) && !defined(NO_ASN)
    ASN1_OBJECT *obj = NULL;
    BIO *bio = NULL;

    AssertNotNull(obj = OBJ_nid2obj(NID_sha256));
    AssertTrue((bio = BIO_new(BIO_s_mem())) != NULL);

    AssertIntGT(wolfSSL_i2a_ASN1_OBJECT(bio, obj), 0);
    AssertIntGT(wolfSSL_i2a_ASN1_OBJECT(bio, NULL), 0);

    AssertIntEQ(wolfSSL_i2a_ASN1_OBJECT(NULL, obj), 0);

    BIO_free(bio);
    ASN1_OBJECT_free(obj);
#endif
}

static void test_wolfSSL_OBJ_cmp(void)
{
#if defined(OPENSSL_EXTRA) && !defined(NO_SHA256)
    ASN1_OBJECT *obj = NULL;
    ASN1_OBJECT *obj2 = NULL;

    printf(testingFmt, "wolfSSL_OBJ_cmp()");

    AssertNotNull(obj = OBJ_nid2obj(NID_any_policy));
    AssertNotNull(obj2 = OBJ_nid2obj(NID_sha256));

    AssertIntEQ(OBJ_cmp(NULL, NULL), WOLFSSL_FATAL_ERROR);
    AssertIntEQ(OBJ_cmp(obj, NULL), WOLFSSL_FATAL_ERROR);
    AssertIntEQ(OBJ_cmp(NULL, obj2), WOLFSSL_FATAL_ERROR);
    AssertIntEQ(OBJ_cmp(obj, obj2), WOLFSSL_FATAL_ERROR);
    AssertIntEQ(OBJ_cmp(obj, obj), 0);
    AssertIntEQ(OBJ_cmp(obj2, obj2), 0);

    ASN1_OBJECT_free(obj);
    ASN1_OBJECT_free(obj2);

    printf(resultFmt, passed);
#endif
}

static void test_wolfSSL_OBJ_txt2nid(void)
{
#if !defined(NO_WOLFSSL_STUB) && defined(WOLFSSL_APACHE_HTTPD)
    int i;
    static const struct {
        const char* sn;
        const char* ln;
        const char* oid;
        int nid;
    } testVals[] = {
        { "tlsfeature", "TLS Feature", "1.3.6.1.5.5.7.1.24", NID_tlsfeature },
        { "id-on-dnsSRV", "SRVName otherName form", "1.3.6.1.5.5.7.8.7",
                                                             NID_id_on_dnsSRV },
        { "msUPN", "Microsoft Universal Principal Name",
                                         "1.3.6.1.4.1.311.20.2.3", NID_ms_upn },
        { NULL, NULL, NULL, NID_undef }
    };

    printf(testingFmt, "wolfSSL_OBJ_txt2nid()");

    /* Invalid cases */
    AssertIntEQ(OBJ_txt2nid(NULL), NID_undef);
    AssertIntEQ(OBJ_txt2nid("Bad name"), NID_undef);

    /* Valid cases */
    for (i = 0; testVals[i].sn != NULL; i++) {
        AssertIntEQ(OBJ_txt2nid(testVals[i].sn), testVals[i].nid);
        AssertIntEQ(OBJ_txt2nid(testVals[i].ln), testVals[i].nid);
        AssertIntEQ(OBJ_txt2nid(testVals[i].oid), testVals[i].nid);
    }

    printf(resultFmt, passed);
#endif
}

static void test_wolfSSL_OBJ_txt2obj(void)
{
#if defined(WOLFSSL_APACHE_HTTPD) || (defined(OPENSSL_EXTRA) && \
        defined(WOLFSSL_CERT_EXT) && defined(WOLFSSL_CERT_GEN))
    int i;
    char buf[50];
    ASN1_OBJECT* obj;
    static const struct {
        const char* oidStr;
        const char* sn;
        const char* ln;
    } objs_list[] = {
    #if defined(WOLFSSL_APACHE_HTTPD)
        { "1.3.6.1.5.5.7.1.24", "tlsfeature", "TLS Feature" },
        { "1.3.6.1.5.5.7.8.7", "id-on-dnsSRV", "SRVName otherName form" },
    #endif
        { "2.5.29.19", "X509 basic ca", "X509v3 Basic Constraints"},
        { NULL, NULL, NULL }
    };

    printf(testingFmt, "wolfSSL_OBJ_txt2obj()");

    AssertNull(obj = OBJ_txt2obj("Bad name", 0));
    AssertNull(obj = OBJ_txt2obj(NULL, 0));

    for (i = 0; objs_list[i].oidStr != NULL; i++) {
        /* Test numerical value of oid (oidStr) */
        AssertNotNull(obj = OBJ_txt2obj(objs_list[i].oidStr, 1));
        /* Convert object back to text to confirm oid is correct */
        wolfSSL_OBJ_obj2txt(buf, (int)sizeof(buf), obj, 1);
        AssertIntEQ(XSTRNCMP(buf, objs_list[i].oidStr, (int)XSTRLEN(buf)), 0);
        ASN1_OBJECT_free(obj);
        XMEMSET(buf, 0, sizeof(buf));

        /* Test short name (sn) */
        AssertNull(obj = OBJ_txt2obj(objs_list[i].sn, 1));
        AssertNotNull(obj = OBJ_txt2obj(objs_list[i].sn, 0));
        /* Convert object back to text to confirm oid is correct */
        wolfSSL_OBJ_obj2txt(buf, (int)sizeof(buf), obj, 1);
        AssertIntEQ(XSTRNCMP(buf, objs_list[i].oidStr, (int)XSTRLEN(buf)), 0);
        ASN1_OBJECT_free(obj);
        XMEMSET(buf, 0, sizeof(buf));

        /* Test long name (ln) - should fail when no_name = 1 */
        AssertNull(obj = OBJ_txt2obj(objs_list[i].ln, 1));
        AssertNotNull(obj = OBJ_txt2obj(objs_list[i].ln, 0));
        /* Convert object back to text to confirm oid is correct */
        wolfSSL_OBJ_obj2txt(buf, (int)sizeof(buf), obj, 1);
        AssertIntEQ(XSTRNCMP(buf, objs_list[i].oidStr, (int)XSTRLEN(buf)), 0);
        ASN1_OBJECT_free(obj);
        XMEMSET(buf, 0, sizeof(buf));
    }

    printf(resultFmt, passed);

#endif
}

static void test_wolfSSL_X509_NAME_ENTRY(void)
{
#if defined(OPENSSL_EXTRA) && !defined(NO_CERTS) && !defined(NO_FILESYSTEM) && \
    !defined(NO_RSA) && defined(WOLFSSL_CERT_GEN)
    X509*      x509;
    BIO*       bio;
    X509_NAME* nm;
    X509_NAME_ENTRY* entry;
    unsigned char cn[] = "another name to add";


    printf(testingFmt, "wolfSSL_X509_NAME_ENTRY()");

    AssertNotNull(x509 =
            wolfSSL_X509_load_certificate_file(cliCertFile, SSL_FILETYPE_PEM));
    AssertNotNull(bio = BIO_new(BIO_s_mem()));
    AssertIntEQ(PEM_write_bio_X509_AUX(bio, x509), SSL_SUCCESS);

#ifdef WOLFSSL_CERT_REQ
    {
        X509_REQ* req;
        BIO*      bReq;

        AssertNotNull(req =
            wolfSSL_X509_load_certificate_file(cliCertFile, SSL_FILETYPE_PEM));
        AssertNotNull(bReq = BIO_new(BIO_s_mem()));
        AssertIntEQ(PEM_write_bio_X509_REQ(bReq, req), SSL_SUCCESS);

        BIO_free(bReq);
        X509_free(req);
    }
#endif

    AssertNotNull(nm = X509_get_subject_name(x509));

    /* Test add entry */
    AssertNotNull(entry = X509_NAME_ENTRY_create_by_NID(NULL, NID_commonName,
                0x0c, cn, (int)sizeof(cn)));
    AssertIntEQ(X509_NAME_add_entry(nm, entry, -1, 0), SSL_SUCCESS);

#ifdef WOLFSSL_CERT_EXT
    AssertIntEQ(X509_NAME_add_entry_by_txt(nm, "emailAddress", MBSTRING_UTF8,
                                           (byte*)"support@wolfssl.com", 19, -1,
                                           1), WOLFSSL_SUCCESS);
#endif
    X509_NAME_ENTRY_free(entry);

    /* Test add entry by NID */
    AssertIntEQ(X509_NAME_add_entry_by_NID(nm, NID_commonName, MBSTRING_UTF8,
                                       cn, -1, -1, 0), WOLFSSL_SUCCESS);

    BIO_free(bio);
    X509_free(x509); /* free's nm */

    printf(resultFmt, passed);
#endif
}


static void test_wolfSSL_X509_set_name(void)
{
#if defined(OPENSSL_EXTRA) && !defined(NO_CERTS) && \
    defined(WOLFSSL_CERT_GEN) && defined(WOLFSSL_CERT_REQ)
    X509* x509;
    X509_NAME* name;

    printf(testingFmt, "wolfSSL_X509_set_name()");

    AssertNotNull(name = X509_NAME_new());
    AssertIntEQ(X509_NAME_add_entry_by_txt(name, "commonName", MBSTRING_UTF8,
                                           (byte*)"wolfssl.com", 11, 0, 1),
                WOLFSSL_SUCCESS);
    AssertIntEQ(X509_NAME_add_entry_by_txt(name, "emailAddress", MBSTRING_UTF8,
                                           (byte*)"support@wolfssl.com", 19, -1,
                                           1), WOLFSSL_SUCCESS);
    AssertNotNull(x509 = X509_new());

    AssertIntEQ(X509_set_subject_name(NULL, NULL), WOLFSSL_FAILURE);
    AssertIntEQ(X509_set_subject_name(x509, NULL), WOLFSSL_FAILURE);
    AssertIntEQ(X509_set_subject_name(NULL, name), WOLFSSL_FAILURE);
    AssertIntEQ(X509_set_subject_name(x509, name), WOLFSSL_SUCCESS);

    AssertIntEQ(X509_set_issuer_name(NULL, NULL), WOLFSSL_FAILURE);
    AssertIntEQ(X509_set_issuer_name(x509, NULL), WOLFSSL_FAILURE);
    AssertIntEQ(X509_set_issuer_name(NULL, name), WOLFSSL_FAILURE);
    AssertIntEQ(X509_set_issuer_name(x509, name), WOLFSSL_SUCCESS);

    X509_free(x509);
    X509_NAME_free(name);

    printf(resultFmt, passed);
#endif /* OPENSSL_ALL && !NO_CERTS */
}

static void test_wolfSSL_X509_set_notAfter(void)
{
#if (defined(OPENSSL_ALL) || defined(WOLFSSL_APACHE_HTTPD)) \
    && !defined(NO_ASN1_TIME) && !defined(USER_TIME) && \
    !defined(TIME_OVERRIDES) && !defined(NO_CERTS) && \
    defined(WOLFSSL_CERT_GEN) && defined(WOLFSSL_CERT_REQ) &&\
    !defined(TIME_T_NOT_64BIT) && !defined(NO_64BIT)
    /* Generalized time will overflow time_t if not long */

    X509* x;
    BIO*  bio;
    ASN1_TIME *asn_time, *time_check;
    const int year = 365*24*60*60;
    const int day  = 24*60*60;
    const int hour = 60*60;
    const int mini = 60;
    int offset_day;
    unsigned char buf[25];
    time_t t;

    printf(testingFmt, "wolfSSL_X509_set_notAfter()");
    /*
     * Setup asn_time. APACHE HTTPD uses time(NULL)
     */
    t = (time_t)107 * year + 31 * day + 34 * hour + 30 * mini + 7 * day;
    offset_day = 7;
    /*
     * Free these.
     */
    asn_time = wolfSSL_ASN1_TIME_adj(NULL, t, offset_day, 0);
    AssertNotNull(asn_time);
    AssertNotNull(x = X509_new());
    AssertNotNull(bio = BIO_new(BIO_s_mem()));
    /*
     * Tests
     */
    AssertTrue(wolfSSL_X509_set_notAfter(x, asn_time));
    /* time_check is simply (ANS1_TIME*)x->notAfter */
    AssertNotNull(time_check = X509_get_notAfter(x));
    /* ANS1_TIME_check validates by checking if argument can be parsed */
    AssertIntEQ(ASN1_TIME_check(time_check), WOLFSSL_SUCCESS);
    /* Convert to human readable format and compare to intended date */
    AssertIntEQ(ASN1_TIME_print(bio, time_check), 1);
    AssertIntEQ(BIO_read(bio, buf, sizeof(buf)), 24);
    AssertIntEQ(XMEMCMP(buf, "Jan 20 10:30:00 2077 GMT", sizeof(buf) - 1), 0);
    /*
     * Cleanup
     */
    XFREE(asn_time,NULL,DYNAMIC_TYPE_OPENSSL);
    X509_free(x);
    BIO_free(bio);
    printf(resultFmt, passed);
#endif
}

static void test_wolfSSL_X509_set_notBefore(void)
{
#if (defined(OPENSSL_ALL) || defined(WOLFSSL_APACHE_HTTPD)) \
    && !defined(NO_ASN1_TIME) && !defined(USER_TIME) && \
    !defined(TIME_OVERRIDES) && !defined(NO_CERTS) && \
    defined(WOLFSSL_CERT_GEN) && defined(WOLFSSL_CERT_REQ)

    X509* x;
    BIO*  bio;
    ASN1_TIME *asn_time, *time_check;
    const int year = 365*24*60*60;
    const int day  = 24*60*60;
    const int hour = 60*60;
    const int mini = 60;
    int offset_day;
    unsigned char buf[25];
    time_t t;

    printf(testingFmt, "wolfSSL_X509_set_notBefore()");
    /*
     * Setup asn_time. APACHE HTTPD uses time(NULL)
     */
    t = (time_t)49 * year + 125 * day + 20 * hour + 30 * mini + 7 * day;
    offset_day = 7;

    /*
     * Free these.
     */
    asn_time = wolfSSL_ASN1_TIME_adj(NULL, t, offset_day, 0);
    AssertNotNull(asn_time);
    AssertNotNull(x = X509_new());
    AssertNotNull(bio = BIO_new(BIO_s_mem()));
    AssertIntEQ(ASN1_TIME_check(asn_time), WOLFSSL_SUCCESS);

    /*
     * Main Tests
     */
    AssertTrue(wolfSSL_X509_set_notBefore(x, asn_time));
    /* time_check == (ANS1_TIME*)x->notBefore */
    AssertNotNull(time_check = X509_get_notBefore(x));
    /* ANS1_TIME_check validates by checking if arguement can be parsed */
    AssertIntEQ(ASN1_TIME_check(time_check), WOLFSSL_SUCCESS);
    /* Convert to human readable format and compare to intended date */
    AssertIntEQ(ASN1_TIME_print(bio, time_check), 1);
    AssertIntEQ(BIO_read(bio, buf, sizeof(buf)), 24);
    AssertIntEQ(XMEMCMP(buf, "May  8 20:30:00 2019 GMT", sizeof(buf) - 1), 0);
    /*
     * Cleanup
     */
    XFREE(asn_time,NULL,DYNAMIC_TYPE_OPENSSL);
    X509_free(x);
    BIO_free(bio);
    printf(resultFmt, passed);
#endif
}

static void test_wolfSSL_X509_set_version(void)
{
#if (defined(OPENSSL_ALL) || defined(WOLFSSL_APACHE_HTTPD)) && \
    !defined(NO_CERTS) && defined(WOLFSSL_CERT_GEN) && defined(WOLFSSL_CERT_REQ)
    X509* x509;
    long v = 2L;
    long max = INT_MAX;

    AssertNotNull(x509 = X509_new());
    /* These should pass. */
    AssertTrue(wolfSSL_X509_set_version(x509, v));
    AssertIntEQ(v, wolfSSL_X509_get_version(x509));
    /* Fail Case: When v(long) is greater than x509->version(int). */
    v = max+1;
    AssertFalse(wolfSSL_X509_set_version(x509, v));
    /* Cleanup */
    X509_free(x509);
    printf(resultFmt, passed);
#endif
}

static void test_wolfSSL_BIO_gets(void)
{
    #if defined(OPENSSL_EXTRA)
    BIO* bio;
    BIO* bio2;
    char msg[] = "\nhello wolfSSL\n security plus\t---...**adf\na...b.c";
    char emp[] = "";
    char bio_buffer[20];
    int bufferSz = 20;

    printf(testingFmt, "wolfSSL_BIO_gets()");

    /* try with bad args */
    AssertNull(bio = BIO_new_mem_buf(NULL, sizeof(msg)));
    AssertNull(bio = BIO_new_mem_buf((void*)msg, -1));

    /* try with real msg */
    AssertNotNull(bio = BIO_new_mem_buf((void*)msg, sizeof(msg)));
    XMEMSET(bio_buffer, 0, bufferSz);
    AssertNotNull(BIO_push(bio, BIO_new(BIO_s_bio())));
    AssertNull(bio2 = BIO_find_type(bio, BIO_TYPE_FILE));
    AssertNotNull(bio2 = BIO_find_type(bio, BIO_TYPE_BIO));
    AssertFalse(bio2 != BIO_next(bio));

    /* make buffer filled with no terminating characters */
    XMEMSET(bio_buffer, 1, bufferSz);

    /* BIO_gets reads a line of data */
    AssertIntEQ(BIO_gets(bio, bio_buffer, -3), 0);
    AssertIntEQ(BIO_gets(bio, bio_buffer, bufferSz), 1);
    AssertIntEQ(BIO_gets(bio, bio_buffer, bufferSz), 14);
    AssertStrEQ(bio_buffer, "hello wolfSSL\n");
    AssertIntEQ(BIO_gets(bio, bio_buffer, bufferSz), 19);
    AssertIntEQ(BIO_gets(bio, bio_buffer, bufferSz), 8);
    AssertIntEQ(BIO_gets(bio, bio_buffer, -1), 0);

    /* check not null terminated string */
    BIO_free(bio);
    msg[0] = 0x33;
    msg[1] = 0x33;
    msg[2] = 0x33;
    AssertNotNull(bio = BIO_new_mem_buf((void*)msg, 3));
    AssertIntEQ(BIO_gets(bio, bio_buffer, 3), 2);
    AssertIntEQ(bio_buffer[0], msg[0]);
    AssertIntEQ(bio_buffer[1], msg[1]);
    AssertIntNE(bio_buffer[2], msg[2]);

    BIO_free(bio);
    msg[3]    = 0x33;
    bio_buffer[3] = 0x33;
    AssertNotNull(bio = BIO_new_mem_buf((void*)msg, 3));
    AssertIntEQ(BIO_gets(bio, bio_buffer, bufferSz), 3);
    AssertIntEQ(bio_buffer[0], msg[0]);
    AssertIntEQ(bio_buffer[1], msg[1]);
    AssertIntEQ(bio_buffer[2], msg[2]);
    AssertIntNE(bio_buffer[3], 0x33); /* make sure null terminator was set */

    /* check reading an empty string */
    BIO_free(bio);
    AssertNotNull(bio = BIO_new_mem_buf((void*)emp, sizeof(emp)));
    AssertIntEQ(BIO_gets(bio, bio_buffer, bufferSz), 1); /* just terminator */
    AssertStrEQ(emp, bio_buffer);
    AssertIntEQ(BIO_gets(bio, bio_buffer, bufferSz), 0); /* Nothing to read */

    /* check error cases */
    BIO_free(bio);
    AssertIntEQ(BIO_gets(NULL, NULL, 0), SSL_FAILURE);
    AssertNotNull(bio = BIO_new(BIO_s_mem()));
    AssertIntEQ(BIO_gets(bio, bio_buffer, 2), 0); /* nothing to read */

#if !defined(NO_FILESYSTEM)
    {
        BIO*  f_bio;
        XFILE f;
        AssertNotNull(f_bio = BIO_new(BIO_s_file()));
        AssertIntLE(BIO_gets(f_bio, bio_buffer, bufferSz), 0);

        f = XFOPEN(svrCertFile, "rb");
        AssertTrue((f != XBADFILE));
        AssertIntEQ((int)BIO_set_fp(f_bio, f, BIO_CLOSE), SSL_SUCCESS);
        AssertIntGT(BIO_gets(f_bio, bio_buffer, bufferSz), 0);

        BIO_free(f_bio);
    }
#endif /* NO_FILESYSTEM */

    BIO_free(bio);
    BIO_free(bio2);

    /* try with type BIO */
    XMEMCPY(msg, "\nhello wolfSSL\n security plus\t---...**adf\na...b.c",
            sizeof(msg));
    AssertNotNull(bio = BIO_new(BIO_s_bio()));
    AssertIntEQ(BIO_gets(bio, bio_buffer, 2), 0); /* nothing to read */
    AssertNotNull(bio2 = BIO_new(BIO_s_bio()));

    AssertIntEQ(BIO_set_write_buf_size(bio, 10),           SSL_SUCCESS);
    AssertIntEQ(BIO_set_write_buf_size(bio2, sizeof(msg)), SSL_SUCCESS);
    AssertIntEQ(BIO_make_bio_pair(bio, bio2),              SSL_SUCCESS);

    AssertIntEQ(BIO_write(bio2, msg, sizeof(msg)), sizeof(msg));
    AssertIntEQ(BIO_gets(bio, bio_buffer, -3), 0);
    AssertIntEQ(BIO_gets(bio, bio_buffer, bufferSz), 1);
    AssertIntEQ(BIO_gets(bio, bio_buffer, bufferSz), 14);
    AssertStrEQ(bio_buffer, "hello wolfSSL\n");
    AssertIntEQ(BIO_gets(bio, bio_buffer, bufferSz), 19);
    AssertIntEQ(BIO_gets(bio, bio_buffer, bufferSz), 8);
    AssertIntEQ(BIO_gets(bio, bio_buffer, -1), 0);

    BIO_free(bio);
    BIO_free(bio2);

    /* check reading an empty string */
    AssertNotNull(bio = BIO_new(BIO_s_bio()));
    AssertIntEQ(BIO_set_write_buf_size(bio, sizeof(emp)), SSL_SUCCESS);
    AssertIntEQ(BIO_gets(bio, bio_buffer, bufferSz), 0); /* Nothing to read */
    AssertStrEQ(emp, bio_buffer);

    BIO_free(bio);

    printf(resultFmt, passed);
    #endif
}


static void test_wolfSSL_BIO_puts(void)
{
    #if defined(OPENSSL_EXTRA)
    BIO* bio;
    char input[] = "hello\0world\n.....ok\n\0";
    char output[128];

    printf(testingFmt, "wolfSSL_BIO_puts()");

    XMEMSET(output, 0, sizeof(output));
    AssertNotNull(bio = BIO_new(BIO_s_mem()));
    AssertIntEQ(BIO_puts(bio, input), 5);
    AssertIntEQ(BIO_pending(bio), 5);
    AssertIntEQ(BIO_puts(bio, input + 6), 14);
    AssertIntEQ(BIO_pending(bio), 19);
    AssertIntEQ(BIO_gets(bio, output, sizeof(output)), 11);
    AssertStrEQ(output, "helloworld\n");
    AssertIntEQ(BIO_pending(bio), 8);
    AssertIntEQ(BIO_gets(bio, output, sizeof(output)), 8);
    AssertStrEQ(output, ".....ok\n");
    AssertIntEQ(BIO_pending(bio), 0);
    AssertIntEQ(BIO_puts(bio, ""), -1);

    BIO_free(bio);
    printf(resultFmt, passed);
    #endif
}

static void test_wolfSSL_BIO_write(void)
{
    #if defined(OPENSSL_EXTRA) && defined(WOLFSSL_BASE64_ENCODE)
    BIO* bio;
    BIO* bio64;
    BIO* ptr;
    int  sz;
    char msg[] = "conversion test";
    char out[40];
    char expected[] = "Y29udmVyc2lvbiB0ZXN0AA==\n";
    BUF_MEM* buf = NULL;

    printf(testingFmt, "wolfSSL_BIO_write()");

    AssertNotNull(bio64 = BIO_new(BIO_f_base64()));
    AssertNotNull(bio   = BIO_push(bio64, BIO_new(BIO_s_mem())));

    /* now should convert to base64 then write to memory */
    AssertIntEQ(BIO_write(bio, msg, sizeof(msg)), sizeof(msg));
    BIO_flush(bio);

    /* test BIO chain */
    AssertIntEQ(SSL_SUCCESS, (int)BIO_get_mem_ptr(bio, &buf));
    AssertNotNull(buf);
    AssertIntEQ(buf->length, 25);

    AssertNotNull(ptr = BIO_find_type(bio, BIO_TYPE_MEM));
    sz = sizeof(out);
    XMEMSET(out, 0, sz);
    AssertIntEQ((sz = BIO_read(ptr, out, sz)), 25);
    AssertIntEQ(XMEMCMP(out, expected, sz), 0);

    /* write then read should return the same message */
    AssertIntEQ(BIO_write(bio, msg, sizeof(msg)), sizeof(msg));
    sz = sizeof(out);
    XMEMSET(out, 0, sz);
    AssertIntEQ(BIO_read(bio, out, sz), 16);
    AssertIntEQ(XMEMCMP(out, msg, sizeof(msg)), 0);

    /* now try encoding with no line ending */
    BIO_set_flags(bio64, BIO_FLAG_BASE64_NO_NL);
    #ifdef HAVE_EX_DATA
    BIO_set_ex_data(bio64, 0, (void*) "data");
    AssertIntEQ(strcmp(BIO_get_ex_data(bio64, 0), "data"), 0);
    #endif
    AssertIntEQ(BIO_write(bio, msg, sizeof(msg)), sizeof(msg));
    BIO_flush(bio);
    sz = sizeof(out);
    XMEMSET(out, 0, sz);
    AssertIntEQ((sz = BIO_read(ptr, out, sz)), 24);
    AssertIntEQ(XMEMCMP(out, expected, sz), 0);

    BIO_free_all(bio); /* frees bio64 also */

    /* test with more than one bio64 in list */
    AssertNotNull(bio64 = BIO_new(BIO_f_base64()));
    AssertNotNull(bio   = BIO_push(BIO_new(BIO_f_base64()), bio64));
    AssertNotNull(BIO_push(bio64, BIO_new(BIO_s_mem())));

    /* now should convert to base64 when stored and then decode with read */
    AssertIntEQ(BIO_write(bio, msg, sizeof(msg)), 25);
    BIO_flush(bio);
    sz = sizeof(out);
    XMEMSET(out, 0, sz);
    AssertIntEQ((sz = BIO_read(bio, out, sz)), 16);
    AssertIntEQ(XMEMCMP(out, msg, sz), 0);
    BIO_clear_flags(bio64, ~0);
    BIO_set_retry_read(bio);
    BIO_free_all(bio); /* frees bio64s also */

    printf(resultFmt, passed);
    #endif
}


static void test_wolfSSL_BIO_printf(void)
{
    #if defined(OPENSSL_ALL)
    BIO* bio;
    int  sz = 7;
    char msg[] = "TLS 1.3 for the world";
    char out[60];
    char expected[] = "TLS 1.3 for the world : sz = 7";

    printf(testingFmt, "wolfSSL_BIO_printf()");

    XMEMSET(out, 0, sizeof(out));
    AssertNotNull(bio = BIO_new(BIO_s_mem()));
    AssertIntEQ(BIO_printf(bio, "%s : sz = %d", msg, sz), 30);
    AssertIntEQ(BIO_printf(NULL, ""), WOLFSSL_FATAL_ERROR);
    AssertIntEQ(BIO_read(bio, out, sizeof(out)), 30);
    AssertIntEQ(XSTRNCMP(out, expected, sizeof(expected)), 0);
    BIO_free(bio);

    printf(resultFmt, passed);
    #endif
}

static void test_wolfSSL_SESSION(void)
{
#if defined(OPENSSL_EXTRA) && !defined(NO_FILESYSTEM) && !defined(NO_CERTS) && \
    !defined(NO_RSA) && defined(HAVE_EXT_CACHE) && \
    defined(HAVE_IO_TESTS_DEPENDENCIES)

    WOLFSSL*     ssl;
    WOLFSSL_CTX* ctx;
    WOLFSSL_SESSION* sess;
    WOLFSSL_SESSION* sess_copy;
    const unsigned char context[] = "user app context";
    unsigned char* sessDer = NULL;
    unsigned char* ptr     = NULL;
    unsigned int contextSz = (unsigned int)sizeof(context);
    int ret, err, sockfd, sz;
    tcp_ready ready;
    func_args server_args;
    THREAD_TYPE serverThread;
    char msg[80];

    printf(testingFmt, "wolfSSL_SESSION()");
    AssertNotNull(ctx = wolfSSL_CTX_new(wolfSSLv23_client_method()));

    AssertTrue(wolfSSL_CTX_use_certificate_file(ctx, cliCertFile, SSL_FILETYPE_PEM));
    AssertTrue(wolfSSL_CTX_use_PrivateKey_file(ctx, cliKeyFile, SSL_FILETYPE_PEM));
    AssertIntEQ(wolfSSL_CTX_load_verify_locations(ctx, caCertFile, 0), SSL_SUCCESS);
#ifdef WOLFSSL_ENCRYPTED_KEYS
    wolfSSL_CTX_set_default_passwd_cb(ctx, PasswordCallBack);
#endif

    XMEMSET(&server_args, 0, sizeof(func_args));
#ifdef WOLFSSL_TIRTOS
    fdOpenSession(Task_self());
#endif

    StartTCP();
    InitTcpReady(&ready);

#if defined(USE_WINDOWS_API)
    /* use RNG to get random port if using windows */
    ready.port = GetRandomPort();
#endif

    server_args.signal = &ready;
    start_thread(test_server_nofail, &server_args, &serverThread);
    wait_tcp_ready(&server_args);

    /* client connection */
    ssl = wolfSSL_new(ctx);
    tcp_connect(&sockfd, wolfSSLIP, ready.port, 0, 0, ssl);
    AssertIntEQ(wolfSSL_set_fd(ssl, sockfd), SSL_SUCCESS);

    err = 0; /* Reset error */
    do {
#ifdef WOLFSSL_ASYNC_CRYPT
        if (err == WC_PENDING_E) {
            ret = wolfSSL_AsyncPoll(ssl, WOLF_POLL_FLAG_CHECK_HW);
            if (ret < 0) { break; } else if (ret == 0) { continue; }
        }
#endif

        ret = wolfSSL_connect(ssl);
        if (ret != SSL_SUCCESS) {
            err = wolfSSL_get_error(ssl, 0);
        }
    } while (ret != SSL_SUCCESS && err == WC_PENDING_E);
    AssertIntEQ(ret, SSL_SUCCESS);

    AssertIntEQ(wolfSSL_write(ssl, "GET", 3), 3);
    AssertIntEQ(wolfSSL_read(ssl, msg, sizeof(msg)), 23);

    sess = wolfSSL_get_session(ssl);
    wolfSSL_shutdown(ssl);
    wolfSSL_free(ssl);

    join_thread(serverThread);

    FreeTcpReady(&ready);

#ifdef WOLFSSL_TIRTOS
    fdOpenSession(Task_self());
#endif

    AssertNotNull(sess_copy = wolfSSL_SESSION_dup(sess));
    wolfSSL_SESSION_free(sess_copy);

    /* get session from DER and update the timeout */
    AssertIntEQ(wolfSSL_i2d_SSL_SESSION(NULL, &sessDer), BAD_FUNC_ARG);
    AssertIntGT((sz = wolfSSL_i2d_SSL_SESSION(sess, &sessDer)), 0);
    wolfSSL_SESSION_free(sess);
    ptr = sessDer;
    AssertNull(sess = wolfSSL_d2i_SSL_SESSION(NULL, NULL, sz));
    AssertNotNull(sess = wolfSSL_d2i_SSL_SESSION(NULL,
                (const unsigned char**)&ptr, sz));
    XFREE(sessDer, NULL, DYNAMIC_TYPE_OPENSSL);
    AssertIntGT(wolfSSL_SESSION_get_time(sess), 0);
    AssertIntEQ(wolfSSL_SSL_SESSION_set_timeout(sess, 500), SSL_SUCCESS);

    /* successful set session test */
    AssertNotNull(ssl = wolfSSL_new(ctx));
    AssertIntEQ(wolfSSL_set_session(ssl, sess), SSL_SUCCESS);

#ifdef HAVE_SESSION_TICKET
    /* Test set/get session ticket */
    {
        const char* ticket = "This is a session ticket";
        char buf[64] = {0};
        word32 bufSz = (word32)sizeof(buf);

        AssertIntEQ(SSL_SUCCESS,
            wolfSSL_set_SessionTicket(ssl, (byte *)ticket, (word32)XSTRLEN(ticket)));
        AssertIntEQ(SSL_SUCCESS,
            wolfSSL_get_SessionTicket(ssl, (byte *)buf, &bufSz));
        AssertStrEQ(ticket, buf);
    }
#endif

    /* fail case with miss match session context IDs (use compatibility API) */
    AssertIntEQ(SSL_set_session_id_context(ssl, context, contextSz),
            SSL_SUCCESS);
    AssertIntEQ(wolfSSL_set_session(ssl, sess), SSL_FAILURE);
    wolfSSL_free(ssl);
    AssertIntEQ(SSL_CTX_set_session_id_context(NULL, context, contextSz),
            SSL_FAILURE);
    AssertIntEQ(SSL_CTX_set_session_id_context(ctx, context, contextSz),
            SSL_SUCCESS);
    AssertNotNull(ssl = wolfSSL_new(ctx));
    AssertIntEQ(wolfSSL_set_session(ssl, sess), SSL_FAILURE);
    wolfSSL_free(ssl);

    SSL_SESSION_free(sess);
    wolfSSL_CTX_free(ctx);
    printf(resultFmt, passed);
#endif
}


static void test_wolfSSL_d2i_PUBKEY(void)
{
    #if defined(OPENSSL_EXTRA)
    BIO*  bio;
    EVP_PKEY* pkey;

    printf(testingFmt, "wolfSSL_d2i_PUBKEY()");

    AssertNotNull(bio = BIO_new(BIO_s_mem()));
    AssertNull(d2i_PUBKEY_bio(NULL, NULL));

#if defined(USE_CERT_BUFFERS_2048) && !defined(NO_RSA)
    /* RSA PUBKEY test */
    AssertIntGT(BIO_write(bio, client_keypub_der_2048,
                sizeof_client_keypub_der_2048), 0);
    AssertNotNull(pkey = d2i_PUBKEY_bio(bio, NULL));
    EVP_PKEY_free(pkey);
#endif

#if defined(USE_CERT_BUFFERS_256) && defined(HAVE_ECC)
    /* ECC PUBKEY test */
    AssertIntGT(BIO_write(bio, ecc_clikeypub_der_256,
                sizeof_ecc_clikeypub_der_256), 0);
    AssertNotNull(pkey = d2i_PUBKEY_bio(bio, NULL));
    EVP_PKEY_free(pkey);
#endif

    BIO_free(bio);

    (void)pkey;
    printf(resultFmt, passed);
    #endif
}

#if (defined(OPENSSL_ALL) || defined(WOLFSSL_ASIO)) && !defined(NO_RSA)
static void test_wolfSSL_d2i_PrivateKeys_bio(void)
{
    BIO*      bio = NULL;
    EVP_PKEY* pkey  = NULL;
#ifndef NO_RSA
    RSA*  rsa  = NULL;
#endif
    WOLFSSL_CTX* ctx;

#if defined(WOLFSSL_KEY_GEN)
    unsigned char buff[4096];
    unsigned char* bufPtr;
    bufPtr = buff;
#endif

    printf(testingFmt, "wolfSSL_d2i_PrivateKeys_bio()");

    /* test creating new EVP_PKEY with bad arg */
    AssertNull((pkey = d2i_PrivateKey_bio(NULL, NULL)));

    /* test loading RSA key using BIO */
#if !defined(NO_RSA) && !defined(NO_FILESYSTEM)
    {
        XFILE file;
        const char* fname = "./certs/server-key.der";
        size_t sz;
        byte* buf;

        file = XFOPEN(fname, "rb");
        AssertTrue((file != XBADFILE));
        AssertTrue(XFSEEK(file, 0, XSEEK_END) == 0);
        sz = XFTELL(file);
        XREWIND(file);
        AssertNotNull(buf = (byte*)XMALLOC(sz, HEAP_HINT, DYNAMIC_TYPE_FILE));
        AssertIntEQ(XFREAD(buf, 1, sz, file), sz);
        XFCLOSE(file);

        /* Test using BIO new mem and loading DER private key */
        AssertNotNull(bio = BIO_new_mem_buf(buf, (int)sz));
        AssertNotNull((pkey = d2i_PrivateKey_bio(bio, NULL)));
        XFREE(buf, HEAP_HINT, DYNAMIC_TYPE_FILE);
        BIO_free(bio);
        bio = NULL;
        EVP_PKEY_free(pkey);
        pkey  = NULL;
    }
#endif

    /* test loading ECC key using BIO */
#if defined(HAVE_ECC) && !defined(NO_FILESYSTEM)
    {
        XFILE file;
        const char* fname = "./certs/ecc-key.der";
        size_t sz;
        byte* buf;

        file = XFOPEN(fname, "rb");
        AssertTrue((file != XBADFILE));
        AssertTrue(XFSEEK(file, 0, XSEEK_END) == 0);
        sz = XFTELL(file);
        XREWIND(file);
        AssertNotNull(buf = (byte*)XMALLOC(sz, HEAP_HINT, DYNAMIC_TYPE_FILE));
        AssertIntEQ(XFREAD(buf, 1, sz, file), sz);
        XFCLOSE(file);

        /* Test using BIO new mem and loading DER private key */
        AssertNotNull(bio = BIO_new_mem_buf(buf, (int)sz));
        AssertNotNull((pkey = d2i_PrivateKey_bio(bio, NULL)));
        XFREE(buf, HEAP_HINT, DYNAMIC_TYPE_FILE);
        BIO_free(bio);
        bio = NULL;
        EVP_PKEY_free(pkey);
        pkey = NULL;
    }
#endif

    AssertNotNull(bio = BIO_new(BIO_s_mem()));
#ifndef NO_WOLFSSL_SERVER
    AssertNotNull(ctx = SSL_CTX_new(wolfSSLv23_server_method()));
#else
    AssertNotNull(ctx = SSL_CTX_new(wolfSSLv23_client_method()));
#endif

#ifndef NO_RSA
    /* Tests bad parameters */
    AssertNull(d2i_RSAPrivateKey_bio(NULL, NULL));

    /* RSA not set yet, expecting to fail*/
    AssertIntEQ(SSL_CTX_use_RSAPrivateKey(ctx, rsa), BAD_FUNC_ARG);

#if defined(USE_CERT_BUFFERS_2048) && defined(WOLFSSL_KEY_GEN)
    /* set RSA using bio*/
    AssertIntGT(BIO_write(bio, client_key_der_2048,
                sizeof_client_key_der_2048), 0);
    AssertNotNull(rsa = d2i_RSAPrivateKey_bio(bio, NULL));

    AssertIntEQ(SSL_CTX_use_RSAPrivateKey(ctx, rsa), WOLFSSL_SUCCESS);

    /*i2d RSAprivate key tests */
    AssertIntEQ(wolfSSL_i2d_RSAPrivateKey(NULL, NULL), BAD_FUNC_ARG);
    AssertIntEQ(wolfSSL_i2d_RSAPrivateKey(rsa, NULL), 1192);
    AssertIntEQ(wolfSSL_i2d_RSAPrivateKey(rsa, &bufPtr),
                                           sizeof_client_key_der_2048);
    bufPtr = NULL;
    AssertIntEQ(wolfSSL_i2d_RSAPrivateKey(rsa, &bufPtr),
                                           sizeof_client_key_der_2048);
    AssertNotNull(bufPtr);
    XFREE(bufPtr, NULL, DYNAMIC_TYPE_OPENSSL);
#endif /* USE_CERT_BUFFERS_2048 WOLFSSL_KEY_GEN */
    RSA_free(rsa);
#endif /* NO_RSA */
    SSL_CTX_free(ctx);
    ctx = NULL;
    BIO_free(bio);
    bio = NULL;
    printf(resultFmt, passed);
}
#endif /* OPENSSL_ALL || WOLFSSL_ASIO */


static void test_wolfSSL_sk_GENERAL_NAME(void)
{
#if defined(OPENSSL_EXTRA) && !defined(NO_FILESYSTEM) && !defined(NO_CERTS) && \
    !defined(NO_RSA)
    X509* x509;
    GENERAL_NAME* gn;
    unsigned char buf[4096];
    const unsigned char* bufPt;
    int bytes, i;
    XFILE f;
    STACK_OF(GENERAL_NAME)* sk;

    printf(testingFmt, "wolfSSL_sk_GENERAL_NAME()");

    f = XFOPEN(cliCertDerFileExt, "rb");
    AssertTrue((f != XBADFILE));
    AssertIntGT((bytes = (int)XFREAD(buf, 1, sizeof(buf), f)), 0);
    XFCLOSE(f);

    bufPt = buf;
    AssertNotNull(x509 = d2i_X509(NULL, &bufPt, bytes));

    AssertNotNull(sk = (STACK_OF(ASN1_OBJECT)*)X509_get_ext_d2i(x509,
                NID_subject_alt_name, NULL, NULL));

    AssertIntEQ(sk_GENERAL_NAME_num(sk), 1);
    for (i = 0; i < sk_GENERAL_NAME_num(sk); i++) {
        AssertNotNull(gn = sk_GENERAL_NAME_value(sk, i));

        switch (gn->type) {
        case GEN_DNS:
            printf("found type GEN_DNS\n");
            break;
        case GEN_EMAIL:
            printf("found type GEN_EMAIL\n");
            break;
        case GEN_URI:
            printf("found type GEN_URI\n");
            break;
        }
    }
    X509_free(x509);
    sk_GENERAL_NAME_pop_free(sk, GENERAL_NAME_free);

    printf(resultFmt, passed);
#endif
}

static void test_wolfSSL_MD4(void)
{
#if defined(OPENSSL_EXTRA) && !defined(NO_MD4)
    MD4_CTX md4;
    unsigned char out[16]; /* MD4_DIGEST_SIZE */
    const char* msg  = "12345678901234567890123456789012345678901234567890123456"
                       "789012345678901234567890";
    const char* test = "\xe3\x3b\x4d\xdc\x9c\x38\xf2\x19\x9c\x3e\x7b\x16\x4f"
                       "\xcc\x05\x36";
    int msgSz        = (int)XSTRLEN(msg);

    printf(testingFmt, "wolfSSL_MD4()");

    XMEMSET(out, 0, sizeof(out));
    MD4_Init(&md4);
    MD4_Update(&md4, (const void*)msg, (unsigned long)msgSz);
    MD4_Final(out, &md4);
    AssertIntEQ(XMEMCMP(out, test, sizeof(out)), 0);

    printf(resultFmt, passed);
#endif
}


static void test_wolfSSL_RSA(void)
{
#if defined(OPENSSL_EXTRA) && !defined(NO_RSA) && !defined(HAVE_USER_RSA) && \
    defined(WOLFSSL_KEY_GEN)
    RSA* rsa;
    const BIGNUM *n;
    const BIGNUM *e;
    const BIGNUM *d;

    printf(testingFmt, "wolfSSL_RSA()");

    AssertNotNull(rsa = RSA_generate_key(2048, 3, NULL, NULL));
    AssertIntEQ(RSA_size(rsa), 256);
    RSA_get0_key(rsa, &n, &e, &d);
    AssertPtrEq(rsa->n, n);
    AssertPtrEq(rsa->e, e);
    AssertPtrEq(rsa->d, d);
    AssertNotNull(n = BN_new());
    AssertNotNull(e = BN_new());
    AssertNotNull(d = BN_new());
    AssertIntEQ(RSA_set0_key(rsa, (BIGNUM*)n, (BIGNUM*)e, (BIGNUM*)d), 1);
    AssertPtrEq(rsa->n, n);
    AssertPtrEq(rsa->e, e);
    AssertPtrEq(rsa->d, d);
    RSA_free(rsa);

#if !defined(USE_FAST_MATH) || (FP_MAX_BITS >= (3072*2))
    AssertNotNull(rsa = RSA_generate_key(3072, 17, NULL, NULL));
    AssertIntEQ(RSA_size(rsa), 384);
    RSA_free(rsa);
#endif

    /* remove for now with odd key size until adjusting rsa key size check with
       wc_MakeRsaKey()
    AssertNotNull(rsa = RSA_generate_key(2999, 65537, NULL, NULL));
    RSA_free(rsa);
    */

    AssertNull(RSA_generate_key(-1, 3, NULL, NULL));
    AssertNull(RSA_generate_key(511, 3, NULL, NULL)); /* RSA_MIN_SIZE - 1 */
    AssertNull(RSA_generate_key(4097, 3, NULL, NULL)); /* RSA_MAX_SIZE + 1 */
    AssertNull(RSA_generate_key(2048, 0, NULL, NULL));

    printf(resultFmt, passed);
#endif
}

static void test_wolfSSL_RSA_DER(void)
{
#if defined(OPENSSL_EXTRA) && !defined(NO_RSA) && !defined(HAVE_FAST_RSA)

    RSA *rsa;
    int i;
    const unsigned char *buff;

    struct tbl_s
    {
        const unsigned char *der;
        int sz;
    } tbl[] = {

#ifdef USE_CERT_BUFFERS_1024
        {client_key_der_1024, sizeof_client_key_der_1024},
        {server_key_der_1024, sizeof_server_key_der_1024},
#endif
#ifdef USE_CERT_BUFFERS_2048
        {client_key_der_2048, sizeof_client_key_der_2048},
        {server_key_der_2048, sizeof_server_key_der_2048},
#endif
        {NULL, 0}
    };

    /* Public Key DER */
    struct tbl_s pub[] = {
#ifdef USE_CERT_BUFFERS_1024
        {client_keypub_der_1024, sizeof_client_keypub_der_1024},
#endif
#ifdef USE_CERT_BUFFERS_2048
        {client_keypub_der_2048, sizeof_client_keypub_der_2048},
#endif
        {NULL, 0}
    };

    printf(testingFmt, "test_wolfSSL_RSA_DER()");

    for (i = 0; tbl[i].der != NULL; i++)
    {
        AssertNotNull(d2i_RSAPublicKey(&rsa, &tbl[i].der, tbl[i].sz));
        AssertNotNull(rsa);
        RSA_free(rsa);
    }
    for (i = 0; tbl[i].der != NULL; i++)
    {
        AssertNotNull(d2i_RSAPrivateKey(&rsa, &tbl[i].der, tbl[i].sz));
        AssertNotNull(rsa);
        RSA_free(rsa);
    }

    for (i = 0; pub[i].der != NULL; i++)
    {
        AssertNotNull(d2i_RSAPublicKey(&rsa, &pub[i].der, pub[i].sz));
        AssertNotNull(rsa);
        AssertIntEQ(i2d_RSAPublicKey(rsa, NULL), pub[i].sz);
        buff = NULL;
        AssertIntEQ(i2d_RSAPublicKey(rsa, &buff), pub[i].sz);
        AssertNotNull(buff);
        AssertIntEQ(0, memcmp((void *)buff, (void *)pub[i].der, pub[i].sz));
        XFREE((void *)buff, NULL, DYNAMIC_TYPE_TMP_BUFFER);
        RSA_free(rsa);
    }

    printf(resultFmt, passed);

#endif
}

static void test_wolfSSL_RSA_get0_key(void)
{
#if defined(OPENSSL_EXTRA) && !defined(NO_RSA) && !defined(HAVE_USER_RSA)
    RSA *rsa = NULL;
    const BIGNUM* n = NULL;
    const BIGNUM* e = NULL;
    const BIGNUM* d = NULL;

    const unsigned char* der = NULL;
    int derSz = 0;

#ifdef USE_CERT_BUFFERS_1024
    der = client_key_der_1024;
    derSz = sizeof_client_key_der_1024;
#elif defined(USE_CERT_BUFFERS_2048)
    der = client_key_der_2048;
    derSz = sizeof_client_key_der_2048;
#endif

    printf(testingFmt, "test_wolfSSL_RSA_get0_key()");

    if (der != NULL) {
        RSA_get0_key(NULL, NULL, NULL, NULL);
        RSA_get0_key(rsa, NULL, NULL, NULL);
        RSA_get0_key(NULL, &n, &e, &d);
        AssertNull(n);
        AssertNull(e);
        AssertNull(d);

        AssertNotNull(d2i_RSAPrivateKey(&rsa, &der, derSz));
        AssertNotNull(rsa);

        RSA_get0_key(rsa, NULL, NULL, NULL);
        RSA_get0_key(rsa, &n, NULL, NULL);
        AssertNotNull(n);
        RSA_get0_key(rsa, NULL, &e, NULL);
        AssertNotNull(e);
        RSA_get0_key(rsa, NULL, NULL, &d);
        AssertNotNull(d);
        RSA_get0_key(rsa, &n, &e, &d);
        AssertNotNull(n);
        AssertNotNull(e);
        AssertNotNull(d);

        RSA_free(rsa);
    }
    printf(resultFmt, passed);
#endif
}

static void test_wolfSSL_RSA_meth(void)
{
#if defined(OPENSSL_EXTRA) && !defined(NO_RSA) && !defined(HAVE_FAST_RSA)
    RSA *rsa;
    RSA_METHOD *rsa_meth;

    printf(testingFmt, "test_wolfSSL_RSA_meth");

#ifdef WOLFSSL_KEY_GEN
    AssertNotNull(rsa = RSA_generate_key(2048, 3, NULL, NULL));
    RSA_free(rsa);
#else
    AssertNull(rsa = RSA_generate_key(2048, 3, NULL, NULL));
#endif

    AssertNotNull(rsa_meth =
            RSA_meth_new("placeholder RSA method", RSA_METHOD_FLAG_NO_CHECK));

    AssertIntEQ(RSA_meth_set_pub_enc(rsa_meth, NULL), 1);
    AssertIntEQ(RSA_meth_set_pub_dec(rsa_meth, NULL), 1);
    AssertIntEQ(RSA_meth_set_priv_enc(rsa_meth, NULL), 1);
    AssertIntEQ(RSA_meth_set_priv_dec(rsa_meth, NULL), 1);
    AssertIntEQ(RSA_meth_set_init(rsa_meth, NULL), 1);
    AssertIntEQ(RSA_meth_set_finish(rsa_meth, NULL), 1);
    AssertIntEQ(RSA_meth_set0_app_data(rsa_meth, NULL), 1);

    AssertNotNull(rsa = RSA_new());
    AssertIntEQ(RSA_set_method(rsa, rsa_meth), 1);
    AssertPtrEq(RSA_get_method(rsa), rsa_meth);
    AssertIntEQ(RSA_flags(rsa), RSA_METHOD_FLAG_NO_CHECK);
    RSA_set_flags(rsa, RSA_FLAG_CACHE_PUBLIC);
    AssertIntEQ(RSA_flags(rsa), RSA_FLAG_CACHE_PUBLIC);

    /* rsa_meth is freed here */
    RSA_free(rsa);

    printf(resultFmt, passed);
#endif
}

static void test_wolfSSL_verify_depth(void)
{
#if defined(OPENSSL_EXTRA) && !defined(NO_RSA) && !defined(NO_WOLFSSL_CLIENT)
    WOLFSSL*     ssl;
    WOLFSSL_CTX* ctx;
    long         depth;

    printf(testingFmt, "test_wolfSSL_verify_depth()");
    AssertNotNull(ctx = wolfSSL_CTX_new(wolfSSLv23_client_method()));

    AssertTrue(wolfSSL_CTX_use_certificate_file(ctx, cliCertFile, SSL_FILETYPE_PEM));
    AssertTrue(wolfSSL_CTX_use_PrivateKey_file(ctx, cliKeyFile, SSL_FILETYPE_PEM));
    AssertIntEQ(wolfSSL_CTX_load_verify_locations(ctx, caCertFile, 0), SSL_SUCCESS);

    AssertIntGT((depth = SSL_CTX_get_verify_depth(ctx)), 0);
    AssertNotNull(ssl = SSL_new(ctx));
    AssertIntEQ(SSL_get_verify_depth(ssl), SSL_CTX_get_verify_depth(ctx));
    SSL_free(ssl);

    SSL_CTX_set_verify_depth(ctx, -1);
    AssertIntEQ(depth, SSL_CTX_get_verify_depth(ctx));

    SSL_CTX_set_verify_depth(ctx, 2);
    AssertIntEQ(2, SSL_CTX_get_verify_depth(ctx));
    AssertNotNull(ssl = SSL_new(ctx));
    AssertIntEQ(2, SSL_get_verify_depth(ssl));

    SSL_free(ssl);
    SSL_CTX_free(ctx);
    printf(resultFmt, passed);
#endif
}

#if defined(OPENSSL_EXTRA) && !defined(NO_HMAC)
/* helper function for test_wolfSSL_HMAC_CTX, digest size is expected to be a
 * buffer of 64 bytes.
 *
 * returns the size of the digest buffer on success and a negative value on
 * failure.
 */
static int test_HMAC_CTX_helper(const EVP_MD* type, unsigned char* digest)
{
    HMAC_CTX ctx1;
    HMAC_CTX ctx2;

    unsigned char key[] = "\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b\x0b"
                          "\x0b\x0b\x0b\x0b\x0b\x0b\x0b";
    unsigned char long_key[] =
        "0123456789012345678901234567890123456789"
        "0123456789012345678901234567890123456789"
        "0123456789012345678901234567890123456789"
        "0123456789012345678901234567890123456789";

    unsigned char msg[] = "message to hash";
    unsigned int  digestSz = 64;
    int keySz = sizeof(key);
    int long_keySz = sizeof(long_key);
    int msgSz = sizeof(msg);

    unsigned char digest2[64];
    unsigned int digestSz2 = 64;

    HMAC_CTX_init(&ctx1);

    AssertIntEQ(HMAC_Init(&ctx1, (const void*)key, keySz, type), SSL_SUCCESS);
    AssertIntEQ(HMAC_Update(&ctx1, msg, msgSz), SSL_SUCCESS);
    AssertIntEQ(HMAC_CTX_copy(&ctx2, &ctx1), SSL_SUCCESS);

    AssertIntEQ(HMAC_Update(&ctx1, msg, msgSz), SSL_SUCCESS);
    AssertIntEQ(HMAC_Final(&ctx1, digest, &digestSz), SSL_SUCCESS);
    HMAC_CTX_cleanup(&ctx1);

    AssertIntEQ(HMAC_Update(&ctx2, msg, msgSz), SSL_SUCCESS);
    AssertIntEQ(HMAC_Final(&ctx2, digest2, &digestSz2), SSL_SUCCESS);
    HMAC_CTX_cleanup(&ctx2);

    AssertIntEQ(digestSz, digestSz2);
    AssertIntEQ(XMEMCMP(digest, digest2, digestSz), 0);

    /* test HMAC_Init with NULL key */

    /* init after copy */
    printf("test HMAC_Init with NULL key (0)\n");
    HMAC_CTX_init(&ctx1);
    AssertIntEQ(HMAC_Init(&ctx1, (const void*)key, keySz, type), SSL_SUCCESS);
    AssertIntEQ(HMAC_Update(&ctx1, msg, msgSz), SSL_SUCCESS);
    AssertIntEQ(HMAC_CTX_copy(&ctx2, &ctx1), SSL_SUCCESS);

    AssertIntEQ(HMAC_Init(&ctx1, NULL, 0, NULL), SSL_SUCCESS);

    AssertIntEQ(HMAC_Update(&ctx1, msg, msgSz), SSL_SUCCESS);
    AssertIntEQ(HMAC_Update(&ctx1, msg, msgSz), SSL_SUCCESS);
    AssertIntEQ(HMAC_Final(&ctx1, digest, &digestSz), SSL_SUCCESS);

    AssertIntEQ(HMAC_Init(&ctx2, NULL, 0, NULL), SSL_SUCCESS);

    AssertIntEQ(HMAC_Update(&ctx2, msg, msgSz), SSL_SUCCESS);
    AssertIntEQ(HMAC_Update(&ctx2, msg, msgSz), SSL_SUCCESS);
    AssertIntEQ(HMAC_Final(&ctx2, digest2, &digestSz), SSL_SUCCESS);

    HMAC_CTX_cleanup(&ctx2);
    AssertIntEQ(digestSz, digestSz2);
    AssertIntEQ(XMEMCMP(digest, digest2, digestSz), 0);

    /* long key */
    printf("test HMAC_Init with NULL key (1)\n");
    HMAC_CTX_init(&ctx1);
    AssertIntEQ(HMAC_Init(&ctx1, (const void*)long_key, long_keySz, type), SSL_SUCCESS);
    AssertIntEQ(HMAC_Update(&ctx1, msg, msgSz), SSL_SUCCESS);
    AssertIntEQ(HMAC_CTX_copy(&ctx2, &ctx1), SSL_SUCCESS);

    AssertIntEQ(HMAC_Init(&ctx1, NULL, 0, NULL), SSL_SUCCESS);

    AssertIntEQ(HMAC_Update(&ctx1, msg, msgSz), SSL_SUCCESS);
    AssertIntEQ(HMAC_Update(&ctx1, msg, msgSz), SSL_SUCCESS);
    AssertIntEQ(HMAC_Final(&ctx1, digest, &digestSz), SSL_SUCCESS);

    AssertIntEQ(HMAC_Init(&ctx2, NULL, 0, NULL), SSL_SUCCESS);

    AssertIntEQ(HMAC_Update(&ctx2, msg, msgSz), SSL_SUCCESS);
    AssertIntEQ(HMAC_Update(&ctx2, msg, msgSz), SSL_SUCCESS);
    AssertIntEQ(HMAC_Final(&ctx2, digest2, &digestSz), SSL_SUCCESS);

    HMAC_CTX_cleanup(&ctx2);
    AssertIntEQ(digestSz, digestSz2);
    AssertIntEQ(XMEMCMP(digest, digest2, digestSz), 0);

    /* init before copy */
    printf("test HMAC_Init with NULL key (2)\n");
    HMAC_CTX_init(&ctx1);
    AssertIntEQ(HMAC_Init(&ctx1, (const void*)key, keySz, type), SSL_SUCCESS);
    AssertIntEQ(HMAC_Update(&ctx1, msg, msgSz), SSL_SUCCESS);
    AssertIntEQ(HMAC_Init(&ctx1, NULL, 0, NULL), SSL_SUCCESS);
    AssertIntEQ(HMAC_CTX_copy(&ctx2, &ctx1), SSL_SUCCESS);

    AssertIntEQ(HMAC_Update(&ctx1, msg, msgSz), SSL_SUCCESS);
    AssertIntEQ(HMAC_Update(&ctx1, msg, msgSz), SSL_SUCCESS);
    AssertIntEQ(HMAC_Final(&ctx1, digest, &digestSz), SSL_SUCCESS);

    AssertIntEQ(HMAC_Update(&ctx2, msg, msgSz), SSL_SUCCESS);
    AssertIntEQ(HMAC_Update(&ctx2, msg, msgSz), SSL_SUCCESS);
    AssertIntEQ(HMAC_Final(&ctx2, digest2, &digestSz), SSL_SUCCESS);

    HMAC_CTX_cleanup(&ctx1);
    HMAC_CTX_cleanup(&ctx2);
    AssertIntEQ(digestSz, digestSz2);
    AssertIntEQ(XMEMCMP(digest, digest2, digestSz), 0);

    return digestSz;
}
#endif /* defined(OPENSSL_EXTRA) && !defined(NO_HMAC) */

static void test_wolfSSL_HMAC_CTX(void)
{
#if defined(OPENSSL_EXTRA) && !defined(NO_HMAC)
    unsigned char digest[64];
    int digestSz;

    printf(testingFmt, "wolfSSL_HMAC_CTX()");

    #ifndef NO_SHA
    AssertIntEQ((digestSz = test_HMAC_CTX_helper(EVP_sha1(), digest)), 20);
    AssertIntEQ(XMEMCMP("\xD9\x68\x77\x23\x70\xFB\x53\x70\x53\xBA\x0E\xDC\xDA"
                          "\xBF\x03\x98\x31\x19\xB2\xCC", digest, digestSz), 0);
    #endif /* !NO_SHA */
    #ifdef WOLFSSL_SHA224
    AssertIntEQ((digestSz = test_HMAC_CTX_helper(EVP_sha224(), digest)), 28);
    AssertIntEQ(XMEMCMP("\x57\xFD\xF4\xE1\x2D\xB0\x79\xD7\x4B\x25\x7E\xB1\x95"
                          "\x9C\x11\xAC\x2D\x1E\x78\x94\x4F\x3A\x0F\xED\xF8\xAD"
                          "\x02\x0E", digest, digestSz), 0);

    #endif /* WOLFSSL_SHA224 */
    #ifndef NO_SHA256
    AssertIntEQ((digestSz = test_HMAC_CTX_helper(EVP_sha256(), digest)), 32);
    AssertIntEQ(XMEMCMP("\x13\xAB\x76\x91\x0C\x37\x86\x8D\xB3\x7E\x30\x0C\xFC"
                          "\xB0\x2E\x8E\x4A\xD7\xD4\x25\xCC\x3A\xA9\x0F\xA2\xF2"
                          "\x47\x1E\x62\x6F\x5D\xF2", digest, digestSz), 0);

    #endif /* !NO_SHA256 */

    #ifdef WOLFSSL_SHA384
    AssertIntEQ((digestSz = test_HMAC_CTX_helper(EVP_sha384(), digest)), 48);
    AssertIntEQ(XMEMCMP("\x9E\xCB\x07\x0C\x11\x76\x3F\x23\xC3\x25\x0E\xC4\xB7"
                          "\x28\x77\x95\x99\xD5\x9D\x7A\xBB\x1A\x9F\xB7\xFD\x25"
                          "\xC9\x72\x47\x9F\x8F\x86\x76\xD6\x20\x57\x87\xB7\xE7"
                          "\xCD\xFB\xC2\xCC\x9F\x2B\xC5\x41\xAB",
                          digest, digestSz), 0);
    #endif /* WOLFSSL_SHA384 */
    #ifdef WOLFSSL_SHA512
    AssertIntEQ((digestSz = test_HMAC_CTX_helper(EVP_sha512(), digest)), 64);
    AssertIntEQ(XMEMCMP("\xD4\x21\x0C\x8B\x60\x6F\xF4\xBF\x07\x2F\x26\xCC\xAD"
                          "\xBC\x06\x0B\x34\x78\x8B\x4F\xD6\xC0\x42\xF1\x33\x10"
                          "\x6C\x4F\x1E\x55\x59\xDD\x2A\x9F\x15\x88\x62\xF8\x60"
                          "\xA3\x99\x91\xE2\x08\x7B\xF7\x95\x3A\xB0\x92\x48\x60"
                          "\x88\x8B\x5B\xB8\x5F\xE9\xB6\xB1\x96\xE3\xB5\xF0",
                          digest, digestSz), 0);
    #endif /* WOLFSSL_SHA512 */

    #ifndef NO_MD5
    AssertIntEQ((digestSz = test_HMAC_CTX_helper(EVP_md5(), digest)), 16);
    AssertIntEQ(XMEMCMP("\xB7\x27\xC4\x41\xE5\x2E\x62\xBA\x54\xED\x72\x70\x9F"
                          "\xE4\x98\xDD", digest, digestSz), 0);
    #endif /* !NO_MD5 */

    printf(resultFmt, passed);
#endif
}

#if defined(OPENSSL_EXTRA) && !defined(NO_RSA) && !defined(NO_WOLFSSL_CLIENT)
static void sslMsgCb(int w, int version, int type, const void* buf,
        size_t sz, SSL* ssl, void* arg)
{
    int i;
    unsigned char* pt = (unsigned char*)buf;

    printf("%s %d bytes of version %d , type %d : ", (w)?"Writing":"Reading",
            (int)sz, version, type);
    for (i = 0; i < (int)sz; i++) printf("%02X", pt[i]);
    printf("\n");
    (void)ssl;
    (void)arg;
}
#endif /* OPENSSL_EXTRA */

static void test_wolfSSL_msg_callback(void)
{
#if defined(OPENSSL_EXTRA) && !defined(NO_RSA) && !defined(NO_WOLFSSL_CLIENT)
    WOLFSSL*     ssl;
    WOLFSSL_CTX* ctx;

    printf(testingFmt, "wolfSSL_msg_callback()");
    AssertNotNull(ctx = wolfSSL_CTX_new(wolfSSLv23_client_method()));

    AssertTrue(wolfSSL_CTX_use_certificate_file(ctx, cliCertFile,
                SSL_FILETYPE_PEM));
    AssertTrue(wolfSSL_CTX_use_PrivateKey_file(ctx, cliKeyFile,
                SSL_FILETYPE_PEM));
    AssertIntEQ(wolfSSL_CTX_load_verify_locations(ctx, caCertFile, 0),
                SSL_SUCCESS);

    AssertNotNull(ssl = SSL_new(ctx));
    AssertIntEQ(SSL_set_msg_callback(ssl, NULL), SSL_SUCCESS);
    AssertIntEQ(SSL_set_msg_callback(ssl, &sslMsgCb), SSL_SUCCESS);
    AssertIntEQ(SSL_set_msg_callback(NULL, &sslMsgCb), SSL_FAILURE);

    SSL_free(ssl);
    SSL_CTX_free(ctx);

    printf(resultFmt, passed);
#endif
}

static void test_wolfSSL_SHA(void)
{
#if defined(OPENSSL_EXTRA) && !defined(HAVE_SELFTEST)
    printf(testingFmt, "wolfSSL_SHA()");

    #if !defined(NO_SHA)
    {
        const unsigned char in[] = "abc";
        unsigned char expected[] = "\xA9\x99\x3E\x36\x47\x06\x81\x6A\xBA\x3E"
                                    "\x25\x71\x78\x50\xC2\x6C\x9C\xD0\xD8\x9D";
        unsigned char out[WC_SHA_DIGEST_SIZE];

        XMEMSET(out, 0, WC_SHA_DIGEST_SIZE);
        AssertNotNull(SHA1(in, XSTRLEN((char*)in), out));
        AssertIntEQ(XMEMCMP(out, expected, WC_SHA_DIGEST_SIZE), 0);
    }
    #endif

    #if !defined(NO_SHA256)
    {
        const unsigned char in[] = "abc";
        unsigned char expected[] = "\xBA\x78\x16\xBF\x8F\x01\xCF\xEA\x41\x41\x40\xDE\x5D\xAE\x22"
            "\x23\xB0\x03\x61\xA3\x96\x17\x7A\x9C\xB4\x10\xFF\x61\xF2\x00"
            "\x15\xAD";
        unsigned char out[WC_SHA256_DIGEST_SIZE];

        XMEMSET(out, 0, WC_SHA256_DIGEST_SIZE);
#if !defined(NO_OLD_NAMES) && !defined(HAVE_FIPS)
        AssertNotNull(SHA256(in, XSTRLEN((char*)in), out));
#else
        AssertNotNull(wolfSSL_SHA256(in, XSTRLEN((char*)in), out));
#endif
        AssertIntEQ(XMEMCMP(out, expected, WC_SHA256_DIGEST_SIZE), 0);
    }
    #endif

    #if defined(WOLFSSL_SHA384)
    {
        const unsigned char in[] = "abc";
        unsigned char expected[] = "\xcb\x00\x75\x3f\x45\xa3\x5e\x8b\xb5\xa0\x3d\x69\x9a\xc6\x50"
            "\x07\x27\x2c\x32\xab\x0e\xde\xd1\x63\x1a\x8b\x60\x5a\x43\xff"
            "\x5b\xed\x80\x86\x07\x2b\xa1\xe7\xcc\x23\x58\xba\xec\xa1\x34"
            "\xc8\x25\xa7";
        unsigned char out[WC_SHA384_DIGEST_SIZE];

        XMEMSET(out, 0, WC_SHA384_DIGEST_SIZE);
#if !defined(NO_OLD_NAMES) && !defined(HAVE_FIPS)
        AssertNotNull(SHA384(in, XSTRLEN((char*)in), out));
#else
        AssertNotNull(wolfSSL_SHA384(in, XSTRLEN((char*)in), out));
#endif
        AssertIntEQ(XMEMCMP(out, expected, WC_SHA384_DIGEST_SIZE), 0);
    }
    #endif

    #if defined(WOLFSSL_SHA512)
    {
        const unsigned char in[] = "abc";
        unsigned char expected[] = "\xdd\xaf\x35\xa1\x93\x61\x7a\xba\xcc\x41\x73\x49\xae\x20\x41"
           "\x31\x12\xe6\xfa\x4e\x89\xa9\x7e\xa2\x0a\x9e\xee\xe6\x4b\x55"
            "\xd3\x9a\x21\x92\x99\x2a\x27\x4f\xc1\xa8\x36\xba\x3c\x23\xa3"
            "\xfe\xeb\xbd\x45\x4d\x44\x23\x64\x3c\xe8\x0e\x2a\x9a\xc9\x4f"
            "\xa5\x4c\xa4\x9f";
        unsigned char out[WC_SHA512_DIGEST_SIZE];

        XMEMSET(out, 0, WC_SHA512_DIGEST_SIZE);
#if !defined(NO_OLD_NAMES) && !defined(HAVE_FIPS)
        AssertNotNull(SHA512(in, XSTRLEN((char*)in), out));
#else
        AssertNotNull(wolfSSL_SHA512(in, XSTRLEN((char*)in), out));
#endif
        AssertIntEQ(XMEMCMP(out, expected, WC_SHA512_DIGEST_SIZE), 0);
    }
    #endif

    printf(resultFmt, passed);
#endif
}

static void test_wolfSSL_DH_1536_prime(void)
{
#if defined(OPENSSL_EXTRA) && !defined(NO_DH)
    BIGNUM* bn;
    unsigned char bits[200];
    int sz = 192; /* known binary size */
    const byte expected[] = {
        0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
        0xC9,0x0F,0xDA,0xA2,0x21,0x68,0xC2,0x34,
        0xC4,0xC6,0x62,0x8B,0x80,0xDC,0x1C,0xD1,
        0x29,0x02,0x4E,0x08,0x8A,0x67,0xCC,0x74,
        0x02,0x0B,0xBE,0xA6,0x3B,0x13,0x9B,0x22,
        0x51,0x4A,0x08,0x79,0x8E,0x34,0x04,0xDD,
        0xEF,0x95,0x19,0xB3,0xCD,0x3A,0x43,0x1B,
        0x30,0x2B,0x0A,0x6D,0xF2,0x5F,0x14,0x37,
        0x4F,0xE1,0x35,0x6D,0x6D,0x51,0xC2,0x45,
        0xE4,0x85,0xB5,0x76,0x62,0x5E,0x7E,0xC6,
        0xF4,0x4C,0x42,0xE9,0xA6,0x37,0xED,0x6B,
        0x0B,0xFF,0x5C,0xB6,0xF4,0x06,0xB7,0xED,
        0xEE,0x38,0x6B,0xFB,0x5A,0x89,0x9F,0xA5,
        0xAE,0x9F,0x24,0x11,0x7C,0x4B,0x1F,0xE6,
        0x49,0x28,0x66,0x51,0xEC,0xE4,0x5B,0x3D,
        0xC2,0x00,0x7C,0xB8,0xA1,0x63,0xBF,0x05,
        0x98,0xDA,0x48,0x36,0x1C,0x55,0xD3,0x9A,
        0x69,0x16,0x3F,0xA8,0xFD,0x24,0xCF,0x5F,
        0x83,0x65,0x5D,0x23,0xDC,0xA3,0xAD,0x96,
        0x1C,0x62,0xF3,0x56,0x20,0x85,0x52,0xBB,
        0x9E,0xD5,0x29,0x07,0x70,0x96,0x96,0x6D,
        0x67,0x0C,0x35,0x4E,0x4A,0xBC,0x98,0x04,
        0xF1,0x74,0x6C,0x08,0xCA,0x23,0x73,0x27,
        0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
    };

    printf(testingFmt, "wolfSSL_DH_1536_prime()");

    AssertNotNull(bn = get_rfc3526_prime_1536(NULL));
    AssertIntEQ(sz, BN_bn2bin((const BIGNUM*)bn, bits));
    AssertIntEQ(0, XMEMCMP(expected, bits, sz));

    BN_free(bn);
    printf(resultFmt, passed);
#endif
}

static void test_wolfSSL_PEM_write_DHparams(void)
{
#if defined(OPENSSL_EXTRA) && !defined(NO_DH) && !defined(NO_FILESYSTEM)
    DH* dh;
    BIO* bio;
    XFILE fp;
    byte pem[2048];
    int  pemSz;
    const char expected[] =
"-----BEGIN DH PARAMETERS-----\n\
MIIBCAKCAQEAsKEIBpwIE7pZBjy8MNX1AMFPRKfW70rGJScc6NKWUwpckd2iwpSE\n\
v32yRJ+b0sGKxb5yXKfnkebUn3MHhVtmSMdw+rTuAsk9mkraPcFGPhlp0RdGB6NN\n\
nyuWFzltMI0q85TTdc+gdebykh8acAWqBINXMPvadpM4UOgn/WPuPOW3yAmub1A1\n\
joTOSgDpEn5aMdcz/CETdswWMNsM/MVipzW477ewrMA29tnJRkj5QJAAKxuqbOMa\n\
wwsDnhvCRuRITiJzb8Nf1JrWMAdI1oyQq9T28eNI01hLprnNKb9oHwhLY4YvXGvW\n\
tgZl96bcAGdru8OpQYP7x/rI4h5+rwA/kwIBAg==\n\
-----END DH PARAMETERS-----\n";
    printf(testingFmt, "wolfSSL_PEM_write_DHparams()");

    AssertNotNull(fp = XFOPEN(dhParamFile, "rb"));
    AssertIntGT((pemSz = (int)XFREAD(pem, 1, sizeof(pem), fp)), 0);
    XFCLOSE(fp);

    AssertNotNull(bio = BIO_new(BIO_s_mem()));
    AssertIntEQ(BIO_write(bio, pem, pemSz), pemSz);
    AssertNotNull(dh = PEM_read_bio_DHparams(bio, NULL, NULL, NULL));
    BIO_free(bio);

    AssertNotNull(fp = XFOPEN("./test-write-dhparams.pem", "wb"));
    AssertIntEQ(PEM_write_DHparams(fp, dh), WOLFSSL_SUCCESS);
    AssertIntEQ(PEM_write_DHparams(fp, NULL), WOLFSSL_FAILURE);
    XFCLOSE(fp);
    DH_free(dh);

    /* check results */
    XMEMSET(pem, 0, sizeof(pem));
    AssertNotNull(fp = XFOPEN("./test-write-dhparams.pem", "rb"));
    AssertIntGT((pemSz = (int)XFREAD(pem, 1, sizeof(pem), fp)), 0);
    AssertIntEQ(XMEMCMP(pem, expected, pemSz), 0);
    XFCLOSE(fp);

    printf(resultFmt, passed);
#endif
}

static void test_wolfSSL_AES_ecb_encrypt(void)
{
#if defined(OPENSSL_EXTRA) && !defined(NO_AES) && defined(HAVE_AES_ECB)
    AES_KEY aes;
    const byte msg[] =
    {
      0x6b,0xc1,0xbe,0xe2,0x2e,0x40,0x9f,0x96,
      0xe9,0x3d,0x7e,0x11,0x73,0x93,0x17,0x2a
    };

    const byte verify[] =
    {
        0xf3,0xee,0xd1,0xbd,0xb5,0xd2,0xa0,0x3c,
        0x06,0x4b,0x5a,0x7e,0x3d,0xb1,0x81,0xf8
    };

    const byte key[] =
    {
      0x60,0x3d,0xeb,0x10,0x15,0xca,0x71,0xbe,
      0x2b,0x73,0xae,0xf0,0x85,0x7d,0x77,0x81,
      0x1f,0x35,0x2c,0x07,0x3b,0x61,0x08,0xd7,
      0x2d,0x98,0x10,0xa3,0x09,0x14,0xdf,0xf4
    };


    byte out[AES_BLOCK_SIZE];

    printf(testingFmt, "wolfSSL_AES_ecb_encrypt()");

    AssertIntEQ(AES_set_encrypt_key(key, sizeof(key)*8, &aes), 0);
    XMEMSET(out, 0, AES_BLOCK_SIZE);
    AES_ecb_encrypt(msg, out, &aes, AES_ENCRYPT);
    AssertIntEQ(XMEMCMP(out, verify, AES_BLOCK_SIZE), 0);

#ifdef HAVE_AES_DECRYPT
    AssertIntEQ(AES_set_decrypt_key(key, sizeof(key)*8, &aes), 0);
    XMEMSET(out, 0, AES_BLOCK_SIZE);
    AES_ecb_encrypt(verify, out, &aes, AES_DECRYPT);
    AssertIntEQ(XMEMCMP(out, msg, AES_BLOCK_SIZE), 0);
#endif

    /* test bad arguments */
    AES_ecb_encrypt(NULL, out, &aes, AES_DECRYPT);
    AES_ecb_encrypt(verify, NULL, &aes, AES_DECRYPT);
    AES_ecb_encrypt(verify, out, NULL, AES_DECRYPT);

    printf(resultFmt, passed);
#endif
}

static void test_wolfSSL_SHA256(void)
{
#if defined(OPENSSL_EXTRA) && !defined(NO_SHA256) && \
    defined(NO_OLD_SHA_NAMES) && !defined(HAVE_FIPS) && !defined(HAVE_SELFTEST)
    unsigned char input[] =
        "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq";
    unsigned char output[] =
        "\x24\x8D\x6A\x61\xD2\x06\x38\xB8\xE5\xC0\x26\x93\x0C\x3E\x60"
        "\x39\xA3\x3C\xE4\x59\x64\xFF\x21\x67\xF6\xEC\xED\xD4\x19\xDB"
        "\x06\xC1";
    size_t inLen;
    byte hash[WC_SHA256_DIGEST_SIZE];

    printf(testingFmt, "wolfSSL_SHA256()");
    inLen  = XSTRLEN((char*)input);

    XMEMSET(hash, 0, WC_SHA256_DIGEST_SIZE);
    AssertNotNull(SHA256(input, inLen, hash));
    AssertIntEQ(XMEMCMP(hash, output, WC_SHA256_DIGEST_SIZE), 0);

    printf(resultFmt, passed);
#endif
}

static void test_wolfSSL_X509_get_serialNumber(void)
{
#if defined(OPENSSL_EXTRA) && !defined(NO_CERTS) && !defined(NO_RSA)
    ASN1_INTEGER* a;
    BIGNUM* bn;
    X509*   x509;
    char *serialHex;

    printf(testingFmt, "wolfSSL_X509_get_serialNumber()");

    AssertNotNull(x509 = wolfSSL_X509_load_certificate_file(svrCertFile,
                                                      SSL_FILETYPE_PEM));
    AssertNotNull(a = X509_get_serialNumber(x509));

    /* check on value of ASN1 Integer */
    AssertNotNull(bn = ASN1_INTEGER_to_BN(a, NULL));

    X509_free(x509); /* free's a */

    AssertNotNull(serialHex = BN_bn2hex(bn));
#ifndef WC_DISABLE_RADIX_ZERO_PAD
    AssertStrEQ(serialHex, "01");
#else
    AssertStrEQ(serialHex, "1");
#endif
    OPENSSL_free(serialHex);

    AssertIntEQ(BN_get_word(bn), 1);

    BN_free(bn);

    /* hard test free'ing with dynamic buffer to make sure there is no leaks */
    a = ASN1_INTEGER_new();
    if (a) {
        AssertNotNull(a->data = (unsigned char*)XMALLOC(100, NULL,
                    DYNAMIC_TYPE_OPENSSL));
        a->isDynamic = 1;
        ASN1_INTEGER_free(a);
    }

    printf(resultFmt, passed);
#endif
}


static void test_wolfSSL_OpenSSL_add_all_algorithms(void){
#if defined(OPENSSL_EXTRA)
    printf(testingFmt, "wolfSSL_OpenSSL_add_all_algorithms()");

    AssertIntEQ(wolfSSL_add_all_algorithms(),WOLFSSL_SUCCESS);
    wolfSSL_Cleanup();

    AssertIntEQ(wolfSSL_OpenSSL_add_all_algorithms_noconf(),WOLFSSL_SUCCESS);
    wolfSSL_Cleanup();

    AssertIntEQ(wolfSSL_OpenSSL_add_all_algorithms_conf(),WOLFSSL_SUCCESS);
    wolfSSL_Cleanup();

    printf(resultFmt, passed);
#endif
}

static void test_wolfSSL_ASN1_STRING_print_ex(void){
#if defined(OPENSSL_EXTRA) && !defined(NO_ASN)
    ASN1_STRING* asn_str = NULL;
    const char data[] = "Hello wolfSSL!";
    ASN1_STRING* esc_str = NULL;
    const char esc_data[] = "a+;<>";
    BIO *bio;
    unsigned long flags;
    int p_len;
    unsigned char rbuf[255];

    printf(testingFmt, "wolfSSL_ASN1_STRING_print_ex()");

    /* setup */
    XMEMSET(rbuf, 0, 255);
    bio = BIO_new(BIO_s_mem());
    BIO_set_write_buf_size(bio,255);

    asn_str = ASN1_STRING_type_new(V_ASN1_OCTET_STRING);
    ASN1_STRING_set(asn_str, (const void*)data, sizeof(data));
    esc_str = ASN1_STRING_type_new(V_ASN1_OCTET_STRING);
    ASN1_STRING_set(esc_str, (const void*)esc_data, sizeof(esc_data));

    /* no flags */
    XMEMSET(rbuf, 0, 255);
    flags = 0;
    p_len = wolfSSL_ASN1_STRING_print_ex(bio, asn_str, flags);
    AssertIntEQ(p_len, 15);
    BIO_read(bio, (void*)rbuf, 15);
    AssertStrEQ((char*)rbuf, "Hello wolfSSL!");

    /* RFC2253 Escape */
    XMEMSET(rbuf, 0, 255);
    flags = ASN1_STRFLGS_ESC_2253;
    p_len = wolfSSL_ASN1_STRING_print_ex(bio, esc_str, flags);
    AssertIntEQ(p_len, 9);
    BIO_read(bio, (void*)rbuf, 9);
    AssertStrEQ((char*)rbuf, "a\\+\\;\\<\\>");

    /* Show type */
    XMEMSET(rbuf, 0, 255);
    flags = ASN1_STRFLGS_SHOW_TYPE;
    p_len = wolfSSL_ASN1_STRING_print_ex(bio, asn_str, flags);
    AssertIntEQ(p_len, 28);
    BIO_read(bio, (void*)rbuf, 28);
    AssertStrEQ((char*)rbuf, "OCTET STRING:Hello wolfSSL!");

    /* Dump All */
    XMEMSET(rbuf, 0, 255);
    flags = ASN1_STRFLGS_DUMP_ALL;
    p_len = wolfSSL_ASN1_STRING_print_ex(bio, asn_str, flags);
    AssertIntEQ(p_len, 31);
    BIO_read(bio, (void*)rbuf, 31);
    AssertStrEQ((char*)rbuf, "#48656C6C6F20776F6C6653534C2100");

    /* Dump Der */
    XMEMSET(rbuf, 0, 255);
    flags = ASN1_STRFLGS_DUMP_ALL | ASN1_STRFLGS_DUMP_DER;
    p_len = wolfSSL_ASN1_STRING_print_ex(bio, asn_str, flags);
    AssertIntEQ(p_len, 35);
    BIO_read(bio, (void*)rbuf, 35);
    AssertStrEQ((char*)rbuf, "#040F48656C6C6F20776F6C6653534C2100");

    /* Dump All + Show type */
    XMEMSET(rbuf, 0, 255);
    flags = ASN1_STRFLGS_DUMP_ALL | ASN1_STRFLGS_SHOW_TYPE;
    p_len = wolfSSL_ASN1_STRING_print_ex(bio, asn_str, flags);
    AssertIntEQ(p_len, 44);
    BIO_read(bio, (void*)rbuf, 44);
    AssertStrEQ((char*)rbuf, "OCTET STRING:#48656C6C6F20776F6C6653534C2100");

    BIO_free(bio);
    ASN1_STRING_free(asn_str);
    ASN1_STRING_free(esc_str);

    printf(resultFmt, passed);
#endif
}

static void test_wolfSSL_ASN1_TIME_to_generalizedtime(void){
#if defined(OPENSSL_EXTRA) && !defined(NO_ASN1_TIME)
    WOLFSSL_ASN1_TIME *t;
    WOLFSSL_ASN1_TIME *out;
    WOLFSSL_ASN1_TIME *gtime;
    int tlen = 0;
    unsigned char *data;

    printf(testingFmt, "wolfSSL_ASN1_TIME_to_generalizedtime()");

    /* UTC Time test */
    AssertNotNull(t = (WOLFSSL_ASN1_TIME*)XMALLOC(sizeof(WOLFSSL_ASN1_TIME),
                NULL, DYNAMIC_TYPE_TMP_BUFFER));
    XMEMSET(t->data, 0, ASN_GENERALIZED_TIME_SIZE);
    AssertNotNull(out = (WOLFSSL_ASN1_TIME*)XMALLOC(sizeof(WOLFSSL_ASN1_TIME),
                NULL, DYNAMIC_TYPE_TMP_BUFFER));
    t->type = ASN_UTC_TIME;
    t->length = ASN_UTC_TIME_SIZE;
    XMEMCPY(t->data, "050727123456Z", ASN_UTC_TIME_SIZE);

    tlen = wolfSSL_ASN1_TIME_get_length(t);
    AssertIntEQ(tlen, ASN_UTC_TIME_SIZE);
    data = wolfSSL_ASN1_TIME_get_data(t);
    AssertStrEQ((char*)data, "050727123456Z");
    gtime = wolfSSL_ASN1_TIME_to_generalizedtime(t, &out);
    AssertIntEQ(gtime->type, ASN_GENERALIZED_TIME);
    AssertIntEQ(gtime->length, ASN_GENERALIZED_TIME_SIZE);
    AssertStrEQ((char*)gtime->data, "20050727123456Z");

    /* Generalized Time test */
    XMEMSET(t, 0, ASN_GENERALIZED_TIME_SIZE);
    XMEMSET(out, 0, ASN_GENERALIZED_TIME_SIZE);
    XMEMSET(data, 0, ASN_GENERALIZED_TIME_SIZE);
    gtime = NULL;
    t->type = ASN_GENERALIZED_TIME;
    t->length = ASN_GENERALIZED_TIME_SIZE;
    XMEMCPY(t->data, "20050727123456Z", ASN_GENERALIZED_TIME_SIZE);

    tlen = wolfSSL_ASN1_TIME_get_length(t);
    AssertIntEQ(tlen, ASN_GENERALIZED_TIME_SIZE);
    data = wolfSSL_ASN1_TIME_get_data(t);
    AssertStrEQ((char*)data, "20050727123456Z");
    gtime = wolfSSL_ASN1_TIME_to_generalizedtime(t, &out);
    AssertIntEQ(gtime->type, ASN_GENERALIZED_TIME);
    AssertIntEQ(gtime->length, ASN_GENERALIZED_TIME_SIZE);
    AssertStrEQ((char*)gtime->data, "20050727123456Z");
    XFREE(out, NULL, DYNAMIC_TYPE_TMP_BUFFER);

    /* Null parameter test */
    XMEMSET(t, 0, ASN_GENERALIZED_TIME_SIZE);
    gtime = NULL;
    out = NULL;
    t->type = ASN_UTC_TIME;
    t->length = ASN_UTC_TIME_SIZE;
    XMEMCPY(t->data, "050727123456Z", ASN_UTC_TIME_SIZE);
    AssertNotNull(gtime = wolfSSL_ASN1_TIME_to_generalizedtime(t, NULL));
    AssertIntEQ(gtime->type, ASN_GENERALIZED_TIME);
    AssertIntEQ(gtime->length, ASN_GENERALIZED_TIME_SIZE);
    AssertStrEQ((char*)gtime->data, "20050727123456Z");

    XFREE(gtime, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    XFREE(t, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    printf(resultFmt, passed);
#endif
}

static void test_wolfSSL_X509_CA_num(void){
#if defined(OPENSSL_EXTRA) && !defined(NO_CERTS) && !defined(NO_FILESYSTEM) && \
    defined(HAVE_ECC) && !defined(NO_RSA)
    WOLFSSL_X509_STORE *store;
    WOLFSSL_X509 *x509_1, *x509_2;
    int ca_num = 0;

    printf(testingFmt, "wolfSSL_X509_CA_num()");

    store = wolfSSL_X509_STORE_new();
    x509_1 = wolfSSL_X509_load_certificate_file(svrCertFile, WOLFSSL_FILETYPE_PEM);
    wolfSSL_X509_STORE_add_cert(store, x509_1);
    ca_num = wolfSSL_X509_CA_num(store);
    AssertIntEQ(ca_num, 1);

    x509_2 = wolfSSL_X509_load_certificate_file(eccCertFile, WOLFSSL_FILETYPE_PEM);
    wolfSSL_X509_STORE_add_cert(store, x509_2);
    ca_num = wolfSSL_X509_CA_num(store);
    AssertIntEQ(ca_num, 2);

    wolfSSL_X509_free(x509_1);
    wolfSSL_X509_free(x509_2);
    wolfSSL_X509_STORE_free(store);
    printf(resultFmt, passed);
#endif
}

static void test_wolfSSL_X509_check_ca(void){
#if defined(OPENSSL_EXTRA) && !defined(NO_RSA) && !defined(NO_FILESYSTEM)
    WOLFSSL_X509 *x509;

    printf(testingFmt, "wolfSSL_X509_check_ca()");

    x509 = wolfSSL_X509_load_certificate_file(svrCertFile, WOLFSSL_FILETYPE_PEM);
    AssertIntEQ(wolfSSL_X509_check_ca(x509), 1);
    wolfSSL_X509_free(x509);

    x509 = wolfSSL_X509_load_certificate_file(ntruCertFile, WOLFSSL_FILETYPE_PEM);
    AssertIntEQ(wolfSSL_X509_check_ca(x509), 0);
    wolfSSL_X509_free(x509);

    printf(resultFmt, passed);
#endif
}

static void test_wolfSSL_DC_cert(void)
{
#if defined(OPENSSL_EXTRA) && !defined(NO_RSA) && !defined(NO_FILESYSTEM) && \
    defined(WOLFSSL_CERT_GEN) && defined(WOLFSSL_KEY_GEN) && \
    defined(WOLFSSL_CERT_EXT)
    Cert    cert;
    RsaKey  key;
    WC_RNG  rng;
    byte    der[FOURK_BUF];
    int     certSz;
    int     ret, idx;
    const byte  mySerial[8] = {1,2,3,4,5,6,7,8};
    const unsigned char* pt;

    X509*   x509;
    X509_NAME* x509name;
    X509_NAME_ENTRY* entry;
    ASN1_STRING* entryValue;

    CertName name;
    printf(testingFmt, "wolfSSL Certs with DC");

    XMEMSET(&name, 0, sizeof(CertName));

    /* set up cert name */
    XMEMCPY(name.country, "US", sizeof("US"));
    name.countryEnc = CTC_PRINTABLE;
    XMEMCPY(name.state, "Oregon", sizeof("Oregon"));
    name.stateEnc = CTC_UTF8;
    XMEMCPY(name.locality, "Portland", sizeof("Portland"));
    name.localityEnc = CTC_UTF8;
    XMEMCPY(name.sur, "Test", sizeof("Test"));
    name.surEnc = CTC_UTF8;
    XMEMCPY(name.org, "wolfSSL", sizeof("wolfSSL"));
    name.orgEnc = CTC_UTF8;
    XMEMCPY(name.unit, "Development", sizeof("Development"));
    name.unitEnc = CTC_UTF8;
    XMEMCPY(name.commonName, "www.wolfssl.com", sizeof("www.wolfssl.com"));
    name.commonNameEnc = CTC_UTF8;
    XMEMCPY(name.serialDev, "wolfSSL12345", sizeof("wolfSSL12345"));
    name.serialDevEnc = CTC_PRINTABLE;
#ifdef WOLFSSL_MULTI_ATTRIB
    #if CTC_MAX_ATTRIB > 2
    {
        NameAttrib* n;
        n = &name.name[0];
        n->id   = ASN_DOMAIN_COMPONENT;
        n->type = CTC_UTF8;
        n->sz   = sizeof("com");
        XMEMCPY(n->value, "com", sizeof("com"));

        n = &name.name[1];
        n->id   = ASN_DOMAIN_COMPONENT;
        n->type = CTC_UTF8;
        n->sz   = sizeof("wolfssl");
        XMEMCPY(n->value, "wolfssl", sizeof("wolfssl"));
    }
    #endif
#endif /* WOLFSSL_MULTI_ATTRIB */

    AssertIntEQ(wc_InitRsaKey(&key, HEAP_HINT), 0);
#ifndef HAVE_FIPS
    AssertIntEQ(wc_InitRng_ex(&rng, HEAP_HINT, devId), 0);
#else
    AssertIntEQ(wc_InitRng(&rng), 0);
#endif
    AssertIntEQ(wc_MakeRsaKey(&key, 1024, 3, &rng), 0);


    XMEMSET(&cert, 0 , sizeof(Cert));
    AssertIntEQ(wc_InitCert(&cert), 0);

    XMEMCPY(&cert.subject, &name, sizeof(CertName));
    XMEMCPY(cert.serial, mySerial, sizeof(mySerial));
    cert.serialSz = (int)sizeof(mySerial);
    cert.isCA     = 1;
#ifndef NO_SHA256
    cert.sigType = CTC_SHA256wRSA;
#else
    cert.sigType = CTC_SHAwRSA;
#endif

    /* add SKID from the Public Key */
    AssertIntEQ(wc_SetSubjectKeyIdFromPublicKey(&cert, &key, NULL), 0);

    /* add AKID from the Public Key */
    AssertIntEQ(wc_SetAuthKeyIdFromPublicKey(&cert, &key, NULL), 0);

    ret = 0;
    do {
#if defined(WOLFSSL_ASYNC_CRYPT)
        ret = wc_AsyncWait(ret, &key.asyncDev, WC_ASYNC_FLAG_CALL_AGAIN);
#endif
        if (ret >= 0) {
            ret = wc_MakeSelfCert(&cert, der, FOURK_BUF, &key, &rng);
        }
    } while (ret == WC_PENDING_E);
    AssertIntGT(ret, 0);
    certSz = ret;

    /* der holds a certificate with DC's now check X509 parsing of it */
    pt = der;
    AssertNotNull(x509 = d2i_X509(NULL, &pt, certSz));
    AssertNotNull(x509name = X509_get_subject_name(x509));
#ifdef WOLFSSL_MULTI_ATTRIB
    AssertIntEQ((idx = X509_NAME_get_index_by_NID(x509name, NID_domainComponent,
                    -1)), 5);
    AssertIntEQ((idx = X509_NAME_get_index_by_NID(x509name, NID_domainComponent,
                    idx)), 6);
    AssertIntEQ((idx = X509_NAME_get_index_by_NID(x509name, NID_domainComponent,
                    idx)), -1);
#endif /* WOLFSSL_MULTI_ATTRIB */

    /* compare DN at index 0 */
    AssertNotNull(entry = X509_NAME_get_entry(x509name, 0));
    AssertNotNull(entryValue = X509_NAME_ENTRY_get_data(entry));
    AssertIntEQ(ASN1_STRING_length(entryValue), 2);
    AssertStrEQ((const char*)ASN1_STRING_data(entryValue), "US");

#ifdef WOLFSSL_MULTI_ATTRIB
    /* get first and second DC and compare result */
    AssertIntEQ((idx = X509_NAME_get_index_by_NID(x509name, NID_domainComponent,
                    -1)), 5);
    AssertNotNull(entry = X509_NAME_get_entry(x509name, idx));
    AssertNotNull(entryValue = X509_NAME_ENTRY_get_data(entry));
    AssertStrEQ((const char *)ASN1_STRING_data(entryValue), "com");

    AssertIntEQ((idx = X509_NAME_get_index_by_NID(x509name, NID_domainComponent,
                   idx)), 6);
    AssertNotNull(entry = X509_NAME_get_entry(x509name, idx));
    AssertNotNull(entryValue = X509_NAME_ENTRY_get_data(entry));
    AssertStrEQ((const char *)ASN1_STRING_data(entryValue), "wolfssl");
#endif /* WOLFSSL_MULTI_ATTRIB */

    /* try invalid index locations for regression test and sanity check */
    AssertNull(entry = X509_NAME_get_entry(x509name, 11));
    AssertNull(entry = X509_NAME_get_entry(x509name, 20));

    (void)idx;
    X509_free(x509);
    wc_FreeRsaKey(&key);
    wc_FreeRng(&rng);
    printf(resultFmt, passed);
#endif
}


static void test_wolfSSL_X509_get_version(void){
#if defined(OPENSSL_EXTRA) && !defined(NO_FILESYSTEM) && !defined(NO_RSA)
    WOLFSSL_X509 *x509;

    printf(testingFmt, "wolfSSL_X509_get_version()");

    x509 = wolfSSL_X509_load_certificate_file(svrCertFile, WOLFSSL_FILETYPE_PEM);
    AssertNotNull(x509);
    AssertIntEQ((int)wolfSSL_X509_get_version(x509), 2);
    wolfSSL_X509_free(x509);

    printf(resultFmt, passed);
#endif
}

static void test_wolfSSL_DES_ncbc(void){
#if defined(OPENSSL_EXTRA) && !defined(NO_DES3)
    const_DES_cblock myDes;
    DES_cblock iv = {1};
    DES_key_schedule key = {0};
    unsigned char msg[] = "hello wolfssl";
    unsigned char out[DES_BLOCK_SIZE * 2] = {0};
    unsigned char pln[DES_BLOCK_SIZE * 2] = {0};

    unsigned char exp[]  = {0x31, 0x98, 0x2F, 0x3A, 0x55, 0xBF, 0xD8, 0xC4};
    unsigned char exp2[] = {0xC7, 0x45, 0x8B, 0x28, 0x10, 0x53, 0xE0, 0x58};

    printf(testingFmt, "wolfSSL_DES_ncbc()");

    /* partial block test */
    DES_set_key(&key, &myDes);
    DES_ncbc_encrypt(msg, out, 3, &myDes, &iv, DES_ENCRYPT);
    AssertIntEQ(XMEMCMP(exp, out, DES_BLOCK_SIZE), 0);
    AssertIntEQ(XMEMCMP(exp, iv, DES_BLOCK_SIZE), 0);

    DES_set_key(&key, &myDes);
    XMEMSET((byte*)&iv, 0, DES_BLOCK_SIZE);
    *((byte*)&iv) = 1;
    DES_ncbc_encrypt(out, pln, 3, &myDes, &iv, DES_DECRYPT);
    AssertIntEQ(XMEMCMP(msg, pln, 3), 0);
    AssertIntEQ(XMEMCMP(exp, iv, DES_BLOCK_SIZE), 0);

    /* full block test */
    DES_set_key(&key, &myDes);
    XMEMSET(pln, 0, DES_BLOCK_SIZE);
    XMEMSET((byte*)&iv, 0, DES_BLOCK_SIZE);
    *((byte*)&iv) = 1;
    DES_ncbc_encrypt(msg, out, 8, &myDes, &iv, DES_ENCRYPT);
    AssertIntEQ(XMEMCMP(exp2, out, DES_BLOCK_SIZE), 0);
    AssertIntEQ(XMEMCMP(exp2, iv, DES_BLOCK_SIZE), 0);

    DES_set_key(&key, &myDes);
    XMEMSET((byte*)&iv, 0, DES_BLOCK_SIZE);
    *((byte*)&iv) = 1;
    DES_ncbc_encrypt(out, pln, 8, &myDes, &iv, DES_DECRYPT);
    AssertIntEQ(XMEMCMP(msg, pln, 8), 0);
    AssertIntEQ(XMEMCMP(exp2, iv, DES_BLOCK_SIZE), 0);

    printf(resultFmt, passed);
#endif
}

static void test_wolfSSL_AES_cbc_encrypt()
{
#if !defined(NO_AES) && defined(HAVE_AES_CBC) && defined(OPENSSL_EXTRA)
    AES_KEY aes;
    AES_KEY* aesN = NULL;
    size_t len = 0;
    size_t lenB = 0;
    int keySz0 = 0;
    int keySzN = -1;
    byte out[AES_BLOCK_SIZE] = {0};
    byte* outN = NULL;
    const int enc1 = AES_ENCRYPT;
    const int enc2 = AES_DECRYPT;

    /* Test vectors retrieved from:
     *   <begin URL>
     *       https://csrc.nist.gov/
     *       CSRC/media/Projects/Cryptographic-Algorithm-Validation-Program/
     *       documents/aes/KAT_AES.zip
     *   </end URL>
     */
    const byte* pt128N  = NULL;
    byte* key128N       = NULL;
    byte* iv128N        = NULL;
    byte iv128tmp[AES_BLOCK_SIZE] = {0};

    const byte pt128[]  = { 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
                            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00 };

    const byte ct128[]  = { 0x87,0x85,0xb1,0xa7,0x5b,0x0f,0x3b,0xd9,
                            0x58,0xdc,0xd0,0xe2,0x93,0x18,0xc5,0x21 };

    const byte iv128[]  = { 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
                            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00 };

    byte key128[]       = { 0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
                            0xff,0xff,0xf0,0x00,0x00,0x00,0x00,0x00 };


    len = sizeof(pt128);

    #define STRESS_T(a, b, c, d, e, f, g, h, i) \
            wolfSSL_AES_cbc_encrypt(a, b, c, d, e, f); \
            AssertIntNE(XMEMCMP(b, g, h), i)

    #define RESET_IV(x, y) XMEMCPY(x, y, AES_BLOCK_SIZE)

    printf(testingFmt, "Stressing wolfSSL_AES_cbc_encrypt()");
    STRESS_T(pt128N, out, len, &aes, iv128tmp, enc1, ct128, AES_BLOCK_SIZE, 0);
    STRESS_T(pt128, out, len, &aes, iv128N, enc1, ct128, AES_BLOCK_SIZE, 0);

    wolfSSL_AES_cbc_encrypt(pt128, outN, len, &aes, iv128tmp, enc1);
    AssertIntNE(XMEMCMP(out, ct128, AES_BLOCK_SIZE), 0);
    wolfSSL_AES_cbc_encrypt(pt128, out, len, aesN, iv128tmp, enc1);
    AssertIntNE(XMEMCMP(out, ct128, AES_BLOCK_SIZE), 0);

    STRESS_T(pt128, out, lenB, &aes, iv128tmp, enc1, ct128, AES_BLOCK_SIZE, 0);
    printf(resultFmt, "Stress Tests: passed");

    printf(testingFmt, "Stressing wolfSSL_AES_set_encrypt_key");
    AssertIntNE(wolfSSL_AES_set_encrypt_key(key128N, sizeof(key128)*8, &aes),0);
    AssertIntNE(wolfSSL_AES_set_encrypt_key(key128, sizeof(key128)*8, aesN),0);
    AssertIntNE(wolfSSL_AES_set_encrypt_key(key128, keySz0, &aes), 0);
    AssertIntNE(wolfSSL_AES_set_encrypt_key(key128, keySzN, &aes), 0);
    printf(resultFmt, "Stress Tests: passed");

    printf(testingFmt, "Stressing wolfSSL_AES_set_decrypt_key");
    AssertIntNE(wolfSSL_AES_set_decrypt_key(key128N, sizeof(key128)*8, &aes),0);
    AssertIntNE(wolfSSL_AES_set_decrypt_key(key128N, sizeof(key128)*8, aesN),0);
    AssertIntNE(wolfSSL_AES_set_decrypt_key(key128, keySz0, &aes), 0);
    AssertIntNE(wolfSSL_AES_set_decrypt_key(key128, keySzN, &aes), 0);
    printf(resultFmt, "Stress Tests: passed");

  #ifdef WOLFSSL_AES_128

    printf(testingFmt, "wolfSSL_AES_cbc_encrypt() 128-bit");
    XMEMSET(out, 0, AES_BLOCK_SIZE);
    RESET_IV(iv128tmp, iv128);

    AssertIntEQ(wolfSSL_AES_set_encrypt_key(key128, sizeof(key128)*8, &aes), 0);
    wolfSSL_AES_cbc_encrypt(pt128, out, len, &aes, iv128tmp, enc1);
    AssertIntEQ(XMEMCMP(out, ct128, AES_BLOCK_SIZE), 0);
    printf(resultFmt, "passed");

    #ifdef HAVE_AES_DECRYPT

    printf(testingFmt, "wolfSSL_AES_cbc_encrypt() 128-bit in decrypt mode");
    XMEMSET(out, 0, AES_BLOCK_SIZE);
    RESET_IV(iv128tmp, iv128);
    len = sizeof(ct128);

    AssertIntEQ(wolfSSL_AES_set_decrypt_key(key128, sizeof(key128)*8, &aes), 0);
    wolfSSL_AES_cbc_encrypt(ct128, out, len, &aes, iv128tmp, enc2);
    AssertIntEQ(XMEMCMP(out, pt128, AES_BLOCK_SIZE), 0);
    printf(resultFmt, "passed");

    #endif

  #endif /* WOLFSSL_AES_128 */
  #ifdef WOLFSSL_AES_192
    /* Test vectors from NIST Special Publication 800-38A, 2001 Edition
     * Appendix F.2.3  */

    byte iv192tmp[AES_BLOCK_SIZE] = {0};

    const byte pt192[]  = { 0x6b,0xc1,0xbe,0xe2,0x2e,0x40,0x9f,0x96,
                            0xe9,0x3d,0x7e,0x11,0x73,0x93,0x17,0x2a };

    const byte ct192[]  = { 0x4f,0x02,0x1d,0xb2,0x43,0xbc,0x63,0x3d,
                            0x71,0x78,0x18,0x3a,0x9f,0xa0,0x71,0xe8 };

    const byte iv192[]  = { 0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,
                            0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F };

    byte key192[]       = { 0x8e,0x73,0xb0,0xf7,0xda,0x0e,0x64,0x52,
                            0xc8,0x10,0xf3,0x2b,0x80,0x90,0x79,0xe5,
                            0x62,0xf8,0xea,0xd2,0x52,0x2c,0x6b,0x7b };

    len = sizeof(pt192);

    printf(testingFmt, "wolfSSL_AES_cbc_encrypt() 192-bit");
    XMEMSET(out, 0, AES_BLOCK_SIZE);
    RESET_IV(iv192tmp, iv192);

    AssertIntEQ(wolfSSL_AES_set_encrypt_key(key192, sizeof(key192)*8, &aes), 0);
    wolfSSL_AES_cbc_encrypt(pt192, out, len, &aes, iv192tmp, enc1);
    AssertIntEQ(XMEMCMP(out, ct192, AES_BLOCK_SIZE), 0);
    printf(resultFmt, "passed");

    #ifdef HAVE_AES_DECRYPT

    printf(testingFmt, "wolfSSL_AES_cbc_encrypt() 192-bit in decrypt mode");
    len = sizeof(ct192);
    RESET_IV(iv192tmp, iv192);
    XMEMSET(out, 0, AES_BLOCK_SIZE);

    AssertIntEQ(wolfSSL_AES_set_decrypt_key(key192, sizeof(key192)*8, &aes), 0);
    wolfSSL_AES_cbc_encrypt(ct192, out, len, &aes, iv192tmp, enc2);
    AssertIntEQ(XMEMCMP(out, pt192, AES_BLOCK_SIZE), 0);
    printf(resultFmt, "passed");

    #endif
  #endif /* WOLFSSL_AES_192 */
  #ifdef WOLFSSL_AES_256
    /* Test vectors from NIST Special Publication 800-38A, 2001 Edition,
     * Appendix F.2.5  */
    byte iv256tmp[AES_BLOCK_SIZE] = {0};

    const byte pt256[]  = { 0x6b,0xc1,0xbe,0xe2,0x2e,0x40,0x9f,0x96,
                            0xe9,0x3d,0x7e,0x11,0x73,0x93,0x17,0x2a };

    const byte ct256[]  = { 0xf5,0x8c,0x4c,0x04,0xd6,0xe5,0xf1,0xba,
                            0x77,0x9e,0xab,0xfb,0x5f,0x7b,0xfb,0xd6 };

    const byte iv256[]  = { 0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,
                            0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F };

    byte key256[]       = { 0x60,0x3d,0xeb,0x10,0x15,0xca,0x71,0xbe,
                            0x2b,0x73,0xae,0xf0,0x85,0x7d,0x77,0x81,
                            0x1f,0x35,0x2c,0x07,0x3b,0x61,0x08,0xd7,
                            0x2d,0x98,0x10,0xa3,0x09,0x14,0xdf,0xf4 };


    len = sizeof(pt256);

    printf(testingFmt, "wolfSSL_AES_cbc_encrypt() 256-bit");
    XMEMSET(out, 0, AES_BLOCK_SIZE);
    RESET_IV(iv256tmp, iv256);

    AssertIntEQ(wolfSSL_AES_set_encrypt_key(key256, sizeof(key256)*8, &aes), 0);
    wolfSSL_AES_cbc_encrypt(pt256, out, len, &aes, iv256tmp, enc1);
    AssertIntEQ(XMEMCMP(out, ct256, AES_BLOCK_SIZE), 0);
    printf(resultFmt, "passed");

    #ifdef HAVE_AES_DECRYPT

    printf(testingFmt, "wolfSSL_AES_cbc_encrypt() 256-bit in decrypt mode");
    len = sizeof(ct256);
    RESET_IV(iv256tmp, iv256);
    XMEMSET(out, 0, AES_BLOCK_SIZE);

    AssertIntEQ(wolfSSL_AES_set_decrypt_key(key256, sizeof(key256)*8, &aes), 0);
    wolfSSL_AES_cbc_encrypt(ct256, out, len, &aes, iv256tmp, enc2);
    AssertIntEQ(XMEMCMP(out, pt256, AES_BLOCK_SIZE), 0);
    printf(resultFmt, "passed");

    #endif
  #endif /* WOLFSSL_AES_256 */
#endif
}

#if defined(WOLFSSL_QT)
#if !defined(NO_ASN)
static void test_wolfSSL_ASN1_STRING_to_UTF8(void)
{
    WOLFSSL_X509* x509;
    WOLFSSL_X509_NAME* subject;
    WOLFSSL_X509_NAME_ENTRY* e;
    WOLFSSL_ASN1_STRING* a;
    FILE* file;
    int idx = 0;
    char targetOutput[15] = "www.wolfssl.com";
    unsigned char* actual_output;
    int len = 0;
    int result = 0;

    AssertNotNull(file = fopen("./certs/server-cert.pem", "rb"));
    AssertNotNull(x509 = wolfSSL_PEM_read_X509(file, NULL, NULL, NULL));
    fclose(file);

    printf(testingFmt, "wolfSSL_ASN1_STRING_to_UTF8(): NID_commonName");
    AssertNotNull(subject = wolfSSL_X509_get_subject_name(x509));
    AssertIntEQ((idx = wolfSSL_X509_NAME_get_index_by_NID(subject,
                    NID_commonName, -1)), 5);
    AssertNotNull(e = wolfSSL_X509_NAME_get_entry(subject, idx));
    AssertNotNull(a = wolfSSL_X509_NAME_ENTRY_get_data(e));
    AssertIntEQ((len = wolfSSL_ASN1_STRING_to_UTF8(&actual_output, a)), 15);
    result = strncmp((const char*)actual_output, targetOutput, len);
    AssertIntEQ(result, 0);
    printf(resultFmt, result == 0 ? passed : failed);

    printf(testingFmt, "wolfSSL_ASN1_STRING_to_UTF8(NULL, valid): ");
    AssertIntEQ((len = wolfSSL_ASN1_STRING_to_UTF8(NULL, a)),
            WOLFSSL_FATAL_ERROR);
    printf(resultFmt, len == WOLFSSL_FATAL_ERROR ? passed : failed);

    printf(testingFmt, "wolfSSL_ASN1_STRING_to_UTF8(valid, NULL): ");
    AssertIntEQ((len = wolfSSL_ASN1_STRING_to_UTF8(&actual_output, NULL)),
            WOLFSSL_FATAL_ERROR);
    printf(resultFmt, len == WOLFSSL_FATAL_ERROR ? passed : failed);

    printf(testingFmt, "wolfSSL_ASN1_STRING_to_UTF8(NULL, NULL): ");
    AssertIntEQ((len = wolfSSL_ASN1_STRING_to_UTF8(NULL, NULL)),
            WOLFSSL_FATAL_ERROR);
    printf(resultFmt, len == WOLFSSL_FATAL_ERROR ? passed : failed);

    wolfSSL_X509_free(x509);
    XFREE(actual_output, NULL, DYNAMIC_TYPE_TMP_BUFFER);
}
#endif /* !defined(NO_ASN) */

static void test_wolfSSL_sk_CIPHER_description(void)
{
    const long flags = SSL_OP_NO_SSLv2 | SSL_OP_NO_COMPRESSION;
    int i,j,k;
    int numCiphers = 0;
    const SSL_METHOD *method = NULL;
    const SSL_CIPHER *cipher = NULL;
    STACK_OF(SSL_CIPHER) *supportedCiphers = NULL;
    SSL_CTX *ctx = NULL;
    SSL *ssl = NULL;
    char buf[256];
    char test_str[9] = "0000000\0";
    const char badStr[] = "unknown\0";
    const char certPath[] = "./certs/client-cert.pem";
    XMEMSET(buf, 0, sizeof(buf));

    printf(testingFmt, "wolfSSL_sk_CIPHER_description");

    SSL_library_init();

    AssertNotNull(method = TLSv1_client_method());
    AssertNotNull(ctx = SSL_CTX_new(method));

    SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER, 0);
    SSL_CTX_set_verify_depth(ctx, 4);

    SSL_CTX_set_options(ctx, flags);
    AssertIntEQ(SSL_CTX_load_verify_locations(ctx, certPath, NULL),
                WOLFSSL_SUCCESS);

    AssertNotNull(ssl = SSL_new(ctx));
    /* SSL_get_ciphers returns a stack of all configured ciphers
     * A flag, getCipherAtOffset, is set to later have SSL_CIPHER_description
     */
    AssertNotNull(supportedCiphers = SSL_get_ciphers(ssl));

    /* loop through the amount of supportedCiphers */
    numCiphers = sk_num(supportedCiphers);
    for (i = 0; i < numCiphers; ++i) {

        /* sk_value increments "sk->data.cipher->cipherOffset".
         * wolfSSL_sk_CIPHER_description sets the description for
         * the cipher based on the provided offset.
         */

        if ((cipher = sk_value(supportedCiphers, i))) {
            SSL_CIPHER_description(cipher, buf, sizeof(buf));
        }

        /* Search cipher description string for "unknown" descriptor */
        for (j=0; j < (int)XSTRLEN(buf); j++) {
            k=0;
            while ((buf[j] == badStr[k]) && (k < (int)XSTRLEN(badStr))) {
                test_str[k] = badStr[k];
                j++,k++;
            }
        }
        /* Fail if test_str == badStr == "unknown" */
        AssertStrNE(test_str,badStr);
    }
    SSL_free(ssl);
    SSL_CTX_free(ctx);

    printf(resultFmt, passed);
}

static void test_wolfSSL_get_ciphers_compat(void)
{
    const SSL_METHOD *method = NULL;
    const char certPath[] = "./certs/client-cert.pem";
    STACK_OF(SSL_CIPHER) *supportedCiphers = NULL;
    SSL_CTX *ctx = NULL;
    WOLFSSL *ssl = NULL;
    const long flags = SSL_OP_NO_SSLv2 | SSL_OP_NO_COMPRESSION;

    printf(testingFmt, "wolfSSL_get_ciphers_compat");

    SSL_library_init();

    AssertNotNull(method = TLSv1_client_method());
    AssertNotNull(ctx = SSL_CTX_new(method));

    SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER, 0);
    SSL_CTX_set_verify_depth(ctx, 4);

    SSL_CTX_set_options(ctx, flags);
    AssertIntEQ(SSL_CTX_load_verify_locations(ctx, certPath, NULL),
                WOLFSSL_SUCCESS);

    AssertNotNull(ssl = SSL_new(ctx));

    /* Test Bad NULL input */
    AssertNull(supportedCiphers = SSL_get_ciphers(NULL));
    /* Test for Good input */
    AssertNotNull(supportedCiphers = SSL_get_ciphers(ssl));
    /* Further usage of SSL_get_ciphers/wolfSSL_get_ciphers_compat is
     * tested in test_wolfSSL_sk_CIPHER_description according to Qt usage */

    SSL_free(ssl);
    SSL_CTX_free(ctx);

    printf(resultFmt, passed);
}

static void test_wolfSSL_X509_PUBKEY_get(void)
{
    WOLFSSL_X509_PUBKEY pubkey;
    WOLFSSL_X509_PUBKEY* key;
    WOLFSSL_EVP_PKEY evpkey;
    WOLFSSL_EVP_PKEY* evpPkey;
    WOLFSSL_EVP_PKEY* retEvpPkey;

    key = &pubkey;
    evpPkey = &evpkey;

    evpPkey->type = WOLFSSL_SUCCESS;
    key->pkey = evpPkey;

    printf(testingFmt, "wolfSSL_X509_PUBKEY_get() valid input");
    AssertNotNull(retEvpPkey = wolfSSL_X509_PUBKEY_get(key));
    AssertIntEQ(retEvpPkey->type, WOLFSSL_SUCCESS);
    printf(resultFmt,retEvpPkey->type  == WOLFSSL_SUCCESS ? passed : failed);

    printf(testingFmt, "wolfSSL_X509_PUBKEY_get() NULL WOLFSSL_X509_PUBKEY");
    AssertNull(retEvpPkey = wolfSSL_X509_PUBKEY_get(NULL));
    printf(resultFmt,retEvpPkey  == NULL ? passed : failed);

    printf(testingFmt, "wolfSSL_X509_PUBKEY_get() valid PUBKEY, NULL EVP_PKEY");
    key->pkey = NULL;
    AssertNull(retEvpPkey = wolfSSL_X509_PUBKEY_get(key));
    printf(resultFmt,retEvpPkey == NULL ? passed : failed);
}

static void test_wolfSSL_d2i_DHparams()
{
#if !defined(NO_DH)
    FILE* f = NULL;
    unsigned char buf[4096];
    const unsigned char* pt = buf;
    const char* params1 = "./certs/dh2048.der";
    const char* params2 = "./certs/dh3072.der";
    long len = 0;
    WOLFSSL_DH* dh = NULL;
    XMEMSET(buf, 0, sizeof(buf));

    /* Test 2048 bit parameters */
    printf(testingFmt, "wolfSSL_d2i_DHparams() 2048-bit");
    f = XFOPEN(params1, "rb");
    AssertTrue(f != XBADFILE);
    len = (long)XFREAD(buf, 1, sizeof(buf), f);
    XFCLOSE(f);

    /* Valid case */
    AssertNotNull(dh = wolfSSL_d2i_DHparams(NULL, &pt, len));
    AssertNotNull(dh->p);
    AssertNotNull(dh->g);
    AssertTrue(pt != buf);
    AssertIntEQ(DH_generate_key(dh), WOLFSSL_SUCCESS);

    /* Invalid cases */
    AssertNull(wolfSSL_d2i_DHparams(NULL, NULL, len));
    AssertNull(wolfSSL_d2i_DHparams(NULL, &pt, -1));
    AssertNull(wolfSSL_d2i_DHparams(NULL, &pt, 10));

    DH_free(dh);
    printf(resultFmt, passed);

    *buf = 0;
    pt = buf;

    /* Test 3072 bit parameters */
    printf(testingFmt, "wolfSSL_d2i_DHparams() 3072-bit");
    f = XFOPEN(params2, "rb");
    AssertTrue(f != XBADFILE);
    len = (long)XFREAD(buf, 1, sizeof(buf), f);
    XFCLOSE(f);

    /* Valid case */
    AssertNotNull(dh = wolfSSL_d2i_DHparams(NULL, &pt, len));
    AssertNotNull(dh->p);
    AssertNotNull(dh->g);
    AssertTrue(pt != buf);
    AssertIntEQ(DH_generate_key(dh), 1);

    /* Invalid cases */
    AssertNull(wolfSSL_d2i_DHparams(NULL, NULL, len));
    AssertNull(wolfSSL_d2i_DHparams(NULL, &pt, -1));

    DH_free(dh);
    printf(resultFmt, passed);

#endif
}

static void test_wolfSSL_i2d_DHparams()
{
#if !defined(NO_DH)
    FILE* f;
    unsigned char buf[4096];
    const unsigned char* pt = buf;
    unsigned char* pt2 = buf;
    const char* params1 = "./certs/dh2048.der";
    const char* params2 = "./certs/dh3072.der";
    long len;
    WOLFSSL_DH* dh;

    /* Test 2048 bit parameters */
    printf(testingFmt, "wolfSSL_i2d_DHparams() 2048-bit");
    f = XFOPEN(params1, "rb");
    AssertTrue(f != XBADFILE);
    len = (long)XFREAD(buf, 1, sizeof(buf), f);
    XFCLOSE(f);

    /* Valid case */
    AssertNotNull(dh = wolfSSL_d2i_DHparams(NULL, &pt, len));
    AssertTrue(pt != buf);
    AssertIntEQ(DH_generate_key(dh), 1);
    AssertIntEQ(wolfSSL_i2d_DHparams(dh, &pt2), 268);

    /* Invalid cases */
    AssertIntEQ(wolfSSL_i2d_DHparams(NULL, &pt2), 0);
    AssertIntEQ(wolfSSL_i2d_DHparams(dh, NULL), 264);

    DH_free(dh);
    printf(resultFmt, passed);

    *buf = 0;
    pt = buf;
    pt2 = buf;

    /* Test 3072 bit parameters */
    printf(testingFmt, "wolfSSL_i2d_DHparams() 3072-bit");
    f = XFOPEN(params2, "rb");
    AssertTrue(f != XBADFILE);
    len = (long)XFREAD(buf, 1, sizeof(buf), f);
    XFCLOSE(f);

    /* Valid case */
    AssertNotNull(dh = wolfSSL_d2i_DHparams(NULL, &pt, len));
    AssertTrue(pt != buf);
    AssertIntEQ(DH_generate_key(dh), 1);
    AssertIntEQ(wolfSSL_i2d_DHparams(dh, &pt2), 396);

    /* Invalid cases */
    AssertIntEQ(wolfSSL_i2d_DHparams(NULL, &pt2), 0);
    AssertIntEQ(wolfSSL_i2d_DHparams(dh, NULL), 392);

    DH_free(dh);
    printf(resultFmt, passed);
#endif
}

static void test_wolfSSL_EC_KEY_dup(void)
{
#if defined(HAVE_ECC) && (defined(OPENSSL_EXTRA) || \
    defined(OPENSSL_EXTRA_X509_SMALL))

    WOLFSSL_EC_KEY* ecKey;
    WOLFSSL_EC_KEY* dupKey;
#if defined(WOLFSSL_PUBLIC_MP)
    mp_int* srcKey;
    mp_int* destKey;
#endif

    printf(testingFmt, "wolfSSL_EC_KEY_dup()");

    AssertNotNull(ecKey = wolfSSL_EC_KEY_new());
    AssertIntEQ(wolfSSL_EC_KEY_generate_key(ecKey), 1);

    /* Valid cases */
    AssertNotNull(dupKey = wolfSSL_EC_KEY_dup(ecKey));
#if defined(WOLFSSL_PUBLIC_MP)
    srcKey = (mp_int*)ecKey->internal;
    destKey = (mp_int*)dupKey->internal;
    AssertIntEQ(mp_cmp(srcKey, destKey), MP_EQ);
#endif

    /* compare EC_GROUP */
    AssertIntEQ(wolfSSL_EC_GROUP_cmp(ecKey->group, dupKey->group, NULL), MP_EQ);

    /* compare EC_POINT */
    AssertIntEQ(wolfSSL_EC_POINT_cmp(ecKey->group, ecKey->pub_key, \
                dupKey->pub_key, NULL), MP_EQ);

    /* compare BIGNUM */
    AssertIntEQ(wolfSSL_BN_cmp(ecKey->priv_key, dupKey->priv_key), MP_EQ);
    wolfSSL_EC_KEY_free(dupKey);

    /* Invalid cases */
    /* NULL key */
    AssertNull(dupKey = wolfSSL_EC_KEY_dup(NULL));
    /* NULL ecc_key */
    wc_ecc_free((ecc_key*)ecKey->internal);
    XFREE(ecKey->internal, NULL, DYNAMIC_TYPE_ECC);
    ecKey->internal = NULL; /* Set ecc_key to NULL */
    AssertNull(dupKey = wolfSSL_EC_KEY_dup(ecKey));
    wolfSSL_EC_KEY_free(ecKey);
    wolfSSL_EC_KEY_free(dupKey);

    /* NULL Group */
    AssertNotNull(ecKey = wolfSSL_EC_KEY_new());
    AssertIntEQ(wolfSSL_EC_KEY_generate_key(ecKey), 1);
    wolfSSL_EC_GROUP_free(ecKey->group);
    ecKey->group = NULL; /* Set group to NULL */
    AssertNull(dupKey = wolfSSL_EC_KEY_dup(ecKey));
    wolfSSL_EC_KEY_free(ecKey);
    wolfSSL_EC_KEY_free(dupKey);

    /* NULL public key */
    AssertNotNull(ecKey = wolfSSL_EC_KEY_new());
    AssertIntEQ(wolfSSL_EC_KEY_generate_key(ecKey), 1);
    wc_ecc_del_point((ecc_point*)ecKey->pub_key->internal);
    ecKey->pub_key->internal = NULL; /* Set ecc_point to NULL */
    AssertNull(dupKey = wolfSSL_EC_KEY_dup(ecKey));

    wolfSSL_EC_POINT_free(ecKey->pub_key);
    ecKey->pub_key = NULL; /* Set pub_key to NULL */
    AssertNull(dupKey = wolfSSL_EC_KEY_dup(ecKey));
    wolfSSL_EC_KEY_free(ecKey);
    wolfSSL_EC_KEY_free(dupKey);

    /* NULL private key */
    AssertNotNull(ecKey = wolfSSL_EC_KEY_new());
    AssertIntEQ(wolfSSL_EC_KEY_generate_key(ecKey), 1);
#if defined(WOLFSSL_PUBLIC_MP)
    mp_int* mp = (mp_int*)ecKey->priv_key->internal;
    mp_forcezero(mp);
    mp_free(mp);
    ecKey->priv_key->internal = NULL; /* Set internal key to NULL */
    AssertNull(dupKey = wolfSSL_EC_KEY_dup(ecKey));
#endif
    wolfSSL_BN_free(ecKey->priv_key);
    ecKey->priv_key = NULL; /* Set priv_key to NULL */
    AssertNull(dupKey = wolfSSL_EC_KEY_dup(ecKey));
    wolfSSL_EC_KEY_free(ecKey);
    wolfSSL_EC_KEY_free(dupKey);

#if defined(WOLFSSL_PUBLIC_MP)
    mp_free(srcKey);
    mp_free(destKey);
#endif

    printf(resultFmt, passed);
#endif
}

static void test_wolfSSL_EVP_PKEY_set1_get1_DSA(void)
{
#if !defined(NO_DSA)
    DSA       *dsa  = NULL;
    DSA       *setDsa  = NULL;
    EVP_PKEY  *pkey = NULL;
    EVP_PKEY  *set1Pkey = NULL;

    SHA_CTX sha;
    byte    signature[DSA_SIG_SIZE];
    byte    hash[WC_SHA_DIGEST_SIZE];
    word32  bytes;
    int     answer;
#ifdef USE_CERT_BUFFERS_1024
    const unsigned char* dsaKeyDer = dsa_key_der1024;
    int dsaKeySz  = sizeof_dsa_key_der_1024;
    byte    tmp[ONEK_BUF];
    XMEMSET(tmp, 0, sizeof(tmp));
    XMEMCPY(tmp, dsaKeyDer , dsaKeySz);
    bytes = dsa_key_der_sz;
#elif defined(USE_CERT_BUFFERS_2048)
    const unsigned char* dsaKeyDer = dsa_key_der_2048;
    int dsaKeySz  = sizeof_dsa_key_der_2048;
    byte    tmp[TWOK_BUF];
    XMEMSET(tmp, 0, sizeof(tmp));
    XMEMCPY(tmp, dsaKeyDer , dsaKeySz);
    bytes = dsaKeySz;
#else
    const unsigned char* dsaKeyDer = dsa_key_der_2048;
    int dsaKeySz  = sizeof_dsa_key_der_2048;
    byte    tmp[TWOK_BUF];
    XMEMSET(tmp, 0, sizeof(tmp));
    XMEMCPY(tmp, dsaKeyDer , dsaKeySz);
    XFILE fp = XOPEN("./certs/dsa2048.der", "rb");
    if (fp == XBADFILE) {
        return WOLFSSL_BAD_FILE;
    }
    bytes = (word32) XFREAD(tmp, 1, sizeof(tmp), fp);
    XFCLOSE(fp);
#endif /* END USE_CERT_BUFFERS_1024 */

    printf(testingFmt,
           "wolfSSL_EVP_PKEY_set1_DSA and wolfSSL_EVP_PKEY_get1_DSA");

    /* Create hash to later Sign and Verify */
    AssertIntEQ(SHA1_Init(&sha), WOLFSSL_SUCCESS);
    AssertIntEQ(SHA1_Update(&sha, tmp, bytes), WOLFSSL_SUCCESS);
    AssertIntEQ(SHA1_Final(hash,&sha), WOLFSSL_SUCCESS);

    /* Initialize pkey with der format dsa key */
    AssertNotNull(wolfSSL_d2i_PrivateKey(EVP_PKEY_DSA, &pkey,
                &dsaKeyDer ,(long)dsaKeySz));

    /* Test wolfSSL_EVP_PKEY_get1_DSA */
    /* Should Fail: NULL argument */
    AssertNull(dsa = wolfSSL_EVP_PKEY_get1_DSA(NULL));
    /* Should Pass: Initialized pkey argument */
    AssertNotNull(dsa = wolfSSL_EVP_PKEY_get1_DSA(pkey));

    /* Sign */
    AssertIntEQ(wolfSSL_DSA_do_sign(hash, signature, dsa), WOLFSSL_SUCCESS);
    /* Verify. */
    AssertIntEQ(wolfSSL_DSA_do_verify(hash, signature, dsa, &answer),
                WOLFSSL_SUCCESS);

    /* Test wolfSSL_EVP_PKEY_set1_DSA */
    /* Should Fail: set1Pkey not initialized */
    AssertIntNE(wolfSSL_EVP_PKEY_set1_DSA(set1Pkey, dsa), WOLFSSL_SUCCESS);

    /* Initialize set1Pkey */
    set1Pkey = wolfSSL_PKEY_new();

    /* Should Fail Verify: setDsa not initialized from set1Pkey */
    AssertIntNE(wolfSSL_DSA_do_verify(hash,signature,setDsa,&answer),
                WOLFSSL_SUCCESS);

    /* Should Pass: set dsa into set1Pkey */
    AssertIntEQ(wolfSSL_EVP_PKEY_set1_DSA(set1Pkey, dsa), WOLFSSL_SUCCESS);
    printf(resultFmt, passed);

    DSA_free(dsa);
    DSA_free(setDsa);
    EVP_PKEY_free(pkey);
    EVP_PKEY_free(set1Pkey);
#endif /* NO_DSA */
} /* END test_EVP_PKEY_set1_get1_DSA */

static void test_wolfSSL_EVP_PKEY_set1_get1_EC_KEY (void)
{
#ifdef HAVE_ECC
    WOLFSSL_EC_KEY  *ecKey  = NULL;
    WOLFSSL_EC_KEY  *ecGet1  = NULL;
    EVP_PKEY  *pkey = NULL;

    printf(testingFmt,
           "wolfSSL_EVP_PKEY_set1_EC_KEY and wolfSSL_EVP_PKEY_get1_EC_KEY");
    AssertNotNull(ecKey = wolfSSL_EC_KEY_new());
    AssertNotNull(pkey = wolfSSL_PKEY_new());

    /* Test wolfSSL_EVP_PKEY_set1_EC_KEY */
    AssertIntEQ(wolfSSL_EVP_PKEY_set1_EC_KEY(NULL, ecKey), WOLFSSL_FAILURE);
    AssertIntEQ(wolfSSL_EVP_PKEY_set1_EC_KEY(pkey, NULL), WOLFSSL_FAILURE);
    AssertIntEQ(wolfSSL_EVP_PKEY_set1_EC_KEY(pkey, ecKey), WOLFSSL_SUCCESS);

    /* Test wolfSSL_EVP_PKEY_get1_EC_KEY */
    AssertNull(wolfSSL_EVP_PKEY_get1_EC_KEY(NULL));
    AssertNotNull(ecGet1 = wolfSSL_EVP_PKEY_get1_EC_KEY(pkey));

    wolfSSL_EC_KEY_free(ecKey);
    wolfSSL_EC_KEY_free(ecGet1);
    EVP_PKEY_free(pkey);

    /* PASSED */
    printf(resultFmt, passed);
#endif /* HAVE_ECC */
} /* END test_EVP_PKEY_set1_get1_EC_KEY */

static void test_wolfSSL_EVP_PKEY_set1_get1_DH (void)
{
#if !defined(NO_DH)
    DH       *dh    = NULL;
    DH       *setDh = NULL;
    EVP_PKEY *pkey  = NULL;

    FILE* f = NULL;
    unsigned char buf[4096];
    const unsigned char* pt = buf;
    const char* dh2048 = "./certs/dh2048.der";
    long len = 0;
    int code = -1;

    printf(testingFmt,"wolfSSL_EVP_PKEY_set1_DH and wolfSSL_EVP_PKEY_get1_DH");

    XMEMSET(buf, 0, sizeof(buf));

    f = XFOPEN(dh2048, "rb");
    AssertTrue(f != XBADFILE);
    len = (long)XFREAD(buf, 1, sizeof(buf), f);
    XFCLOSE(f);

    /* Load dh2048.der into DH with internal format */
    AssertNotNull(setDh = wolfSSL_d2i_DHparams(NULL, &pt, len));

    AssertIntEQ(wolfSSL_DH_check(setDh, &code), WOLFSSL_SUCCESS);
    AssertIntEQ(code, 0);
    code = -1;

    pkey = wolfSSL_PKEY_new();

    /* Set DH into PKEY */
    AssertIntEQ(wolfSSL_EVP_PKEY_set1_DH(pkey, setDh), WOLFSSL_SUCCESS);

    /* Get DH from PKEY */
    AssertNotNull(dh = wolfSSL_EVP_PKEY_get1_DH(pkey));

    AssertIntEQ(wolfSSL_DH_check(dh, &code), WOLFSSL_SUCCESS);
    AssertIntEQ(code, 0);

    EVP_PKEY_free(pkey);
    DH_free(setDh);
    DH_free(dh);
    printf(resultFmt, passed);
#endif /* NO_DH */
} /* END test_EVP_PKEY_set1_get1_DH */

static void test_wolfSSL_CTX_ctrl(void)
{
#if defined(OPENSSL_EXTRA) && !defined(NO_CERTS) && \
    !defined(NO_FILESYSTEM) && !defined(NO_RSA)
    char caFile[] = "./certs/client-ca.pem";
    char clientFile[] = "./certs/client-cert.pem";
    SSL_CTX* ctx;
    X509* x509 = NULL;
#if !defined(NO_DH) && !defined(NO_DSA)
    byte buf[5300];
    char file[] = "./certs/dsaparams.pem";
    XFILE f;
    int  bytes;
    BIO* bio;
    DSA* dsa;
    DH*  dh;
#endif
#ifdef HAVE_ECC
    WOLFSSL_EC_KEY* ecKey;
#endif
    printf(testingFmt, "wolfSSL_CTX_ctrl");

    AssertNotNull(ctx = SSL_CTX_new(wolfSSLv23_server_method()));

    x509 = wolfSSL_X509_load_certificate_file(caFile, WOLFSSL_FILETYPE_PEM);
    AssertNotNull(x509);
    AssertIntEQ((int)SSL_CTX_add_extra_chain_cert(ctx, x509), WOLFSSL_SUCCESS);

    x509 = wolfSSL_X509_load_certificate_file(clientFile, WOLFSSL_FILETYPE_PEM);
    AssertNotNull(x509);

#if !defined(NO_DH) && !defined(NO_DSA)
    /* Initialize DH */
    f = XFOPEN(file, "rb");
    AssertTrue((f != XBADFILE));
    bytes = (int)XFREAD(buf, 1, sizeof(buf), f);
    XFCLOSE(f);

    bio = BIO_new_mem_buf((void*)buf, bytes);
    AssertNotNull(bio);

    dsa = wolfSSL_PEM_read_bio_DSAparams(bio, NULL, NULL, NULL);
    AssertNotNull(dsa);

    dh = wolfSSL_DSA_dup_DH(dsa);
    AssertNotNull(dh);
#endif
#ifdef HAVE_ECC
    /* Initialize WOLFSSL_EC_KEY */
    AssertNotNull(ecKey = wolfSSL_EC_KEY_new());
    AssertIntEQ(wolfSSL_EC_KEY_generate_key(ecKey),1);
#endif

#if !defined(HAVE_USER_RSA) && !defined(HAVE_FAST_RSA)
    /* additional test of getting EVP_PKEY key size from X509
     * Do not run with user RSA because wolfSSL_RSA_size is not currently
     * allowed with user RSA */
    {
        EVP_PKEY* pkey;
#if defined(HAVE_ECC)
        X509* ecX509;
#endif /* HAVE_ECC */

        AssertNotNull(pkey = X509_get_pubkey(x509));
        /* current RSA key is 2048 bit (256 bytes) */
        AssertIntEQ(EVP_PKEY_size(pkey), 256);

        EVP_PKEY_free(pkey);

#if defined(HAVE_ECC)
#if defined(USE_CERT_BUFFERS_256)
        AssertNotNull(ecX509 = wolfSSL_X509_load_certificate_buffer(
                      cliecc_cert_der_256, sizeof_cliecc_cert_der_256,
                      SSL_FILETYPE_ASN1));
#else
        AssertNotNull(ecX509 = wolfSSL_X509_load_certificate_file(
                      cliEccCertFile, SSL_FILETYPE_PEM));
#endif
        AssertNotNull(pkey = X509_get_pubkey(ecX509));
        /* current ECC key is 256 bit (32 bytes) */
        AssertIntEQ(EVP_PKEY_size(pkey), 32);

        X509_free(ecX509);
        EVP_PKEY_free(pkey);
#endif /* HAVE_ECC */
    }
#endif /* !defined(HAVE_USER_RSA) && !defined(HAVE_FAST_RSA) */

    /* Tests should fail with passed in NULL pointer */
    AssertIntEQ((int)wolfSSL_CTX_ctrl(ctx,SSL_CTRL_EXTRA_CHAIN_CERT,0,NULL),
                SSL_FAILURE);
#if !defined(NO_DH) && !defined(NO_DSA)
    AssertIntEQ((int)wolfSSL_CTX_ctrl(ctx,SSL_CTRL_SET_TMP_DH,0,NULL),
                SSL_FAILURE);
#endif
#ifdef HAVE_ECC
    AssertIntEQ((int)wolfSSL_CTX_ctrl(ctx,SSL_CTRL_SET_TMP_ECDH,0,NULL),
                SSL_FAILURE);
#endif

    /* Test with SSL_CTRL_EXTRA_CHAIN_CERT
     * wolfSSL_CTX_ctrl should succesffuly call SSL_CTX_add_extra_chain_cert
     */
    AssertIntEQ((int)wolfSSL_CTX_ctrl(ctx,SSL_CTRL_EXTRA_CHAIN_CERT,0,x509),
                SSL_SUCCESS);

    /* Test with SSL_CTRL_OPTIONS
     * wolfSSL_CTX_ctrl should succesffuly call SSL_CTX_set_options
     */
    AssertTrue(wolfSSL_CTX_ctrl(ctx,SSL_CTRL_OPTIONS,SSL_OP_NO_TLSv1,NULL)
               == SSL_OP_NO_TLSv1);
    AssertTrue(SSL_CTX_get_options(ctx) == SSL_OP_NO_TLSv1);

    /* Test with SSL_CTRL_SET_TMP_DH
     * wolfSSL_CTX_ctrl should succesffuly call wolfSSL_SSL_CTX_set_tmp_dh
     */
#if !defined(NO_DH) && !defined(NO_DSA)
    AssertIntEQ((int)wolfSSL_CTX_ctrl(ctx,SSL_CTRL_SET_TMP_DH,0,dh),
                SSL_SUCCESS);
#endif

    /* Test with SSL_CTRL_SET_TMP_ECDH
     * wolfSSL_CTX_ctrl should succesffuly call wolfSSL_SSL_CTX_set_tmp_ecdh
     */
#ifdef HAVE_ECC
    AssertIntEQ((int)wolfSSL_CTX_ctrl(ctx,SSL_CTRL_SET_TMP_ECDH,0,ecKey),
                SSL_SUCCESS);
#endif

#ifdef WOLFSSL_ENCRYPTED_KEYS
    AssertNull(SSL_CTX_get_default_passwd_cb(ctx));
    AssertNull(SSL_CTX_get_default_passwd_cb_userdata(ctx));
#endif

    /* Cleanup and Pass */
#if !defined(NO_DH) && !defined(NO_DSA)
    BIO_free(bio);
    DSA_free(dsa);
    DH_free(dh);
#endif
#ifdef HAVE_ECC
    wolfSSL_EC_KEY_free(ecKey);
#endif
    SSL_CTX_free(ctx);
    printf(resultFmt, passed);
#endif /* defined(OPENSSL_EXTRA) && !defined(NO_CERTS) && \
          !defined(NO_FILESYSTEM) && !defined(NO_RSA) */
}

static void test_wolfSSL_DH_check(void)
{
#if !defined(NO_DH) && !defined(NO_DSA)
    byte buf[5300];
    char file[] = "./certs/dsaparams.pem";
    XFILE f;
    int  bytes;
    BIO* bio;
    DSA* dsa;
    DH*  dh = NULL;
    WOLFSSL_BIGNUM* pTmp = NULL;
    WOLFSSL_BIGNUM* gTmp = NULL;
    int codes = -1;

    printf(testingFmt, "wolfSSL_DH_check");

    /* Initialize DH */
    f = XFOPEN(file, "rb");
    AssertTrue((f != XBADFILE));
    bytes = (int)XFREAD(buf, 1, sizeof(buf), f);
    XFCLOSE(f);

    bio = BIO_new_mem_buf((void*)buf, bytes);
    AssertNotNull(bio);

    dsa = wolfSSL_PEM_read_bio_DSAparams(bio, NULL, NULL, NULL);
    AssertNotNull(dsa);

    dh = wolfSSL_DSA_dup_DH(dsa);
    AssertNotNull(dh);

    /* Test assumed to be valid dh.
     * Should return WOLFSSL_SUCCESS
     * codes should be 0
     * Invalid codes = {DH_NOT_SUITABLE_GENERATOR, DH_CHECK_P_NOT_PRIME}
     */
    AssertIntEQ(wolfSSL_DH_check(dh, &codes), WOLFSSL_SUCCESS);
    AssertIntEQ(codes, 0);

    /* Test NULL dh: expected BAD_FUNC_ARG */
    AssertIntEQ(wolfSSL_DH_check(NULL, &codes), WOLFSSL_FAILURE);

    /* Break dh prime to test if codes = DH_CHECK_P_NOT_PRIME */
    pTmp = dh->p;
    dh->p  = NULL;
    AssertIntEQ(wolfSSL_DH_check(dh, &codes), WOLFSSL_FAILURE);
    AssertIntEQ(codes, DH_CHECK_P_NOT_PRIME);
    /* set dh->p back to normal so it wont fail on next tests */
    dh->p = pTmp;
    pTmp = NULL;

    /* Break dh generator to test if codes = DH_NOT_SUITABLE_GENERATOR */
    gTmp = dh->g;
    dh->g = NULL;
    AssertIntEQ(wolfSSL_DH_check(dh, &codes), WOLFSSL_FAILURE);
    AssertIntEQ(codes, DH_NOT_SUITABLE_GENERATOR);
    dh->g = gTmp;
    gTmp = NULL;

    /* Cleanup and Pass Test */
    BIO_free(bio);
    DSA_free(dsa);
    DH_free(dh);
    printf(resultFmt, passed);
#endif /* !NO_DH  && !NO_DSA */
}

static void test_wolfSSL_EC_get_builtin_curves(void)
{
#if defined(HAVE_ECC)
    /* If this test fails then perhaps ecc_sets was updated past 27 */
    size_t max_items = 27;
    size_t nitems = 12;
    size_t numCurves, x;
    int eccEnum;
    EC_builtin_curve r[max_items];

    printf(testingFmt, "wolfSSL_EC_get_builtin_curves");
    SSL_library_init();

    /* Test invalid NULL EC_builtin_curve */
    numCurves = EC_get_builtin_curves(NULL,nitems);
    AssertIntEQ(numCurves,max_items);

    /* Test invalid nitems */
    numCurves = EC_get_builtin_curves(r,-1);
    AssertIntEQ(numCurves,max_items);

    /* Test very large nitems */
    numCurves = EC_get_builtin_curves(r,1000);
    AssertIntEQ(numCurves,max_items);

    /* Test that passed in nitems matches return numCurves */
    numCurves = EC_get_builtin_curves(r,nitems);
    AssertIntEQ(numCurves,max_items);

    /* Test if the EC_builtin_curve name and nid match
     * wc_ecc_get_curve_id_from_name
     */
    for (x = 0; x < nitems; x++) {
        if (x < numCurves) {
            eccEnum = wc_ecc_get_curve_id_from_name(r[x].comment);
            /* Convert OpenSSL NID back to internal enum value */
            AssertIntEQ(r[x].nid, EccEnumToNID(eccEnum));
        }
        else
            break;
    }

    printf(resultFmt, passed);

#endif /* HAVE_ECC */
}

static void test_wolfSSL_EVP_PKEY_assign(void)
{
#if defined(OPENSSL_ALL)
    int type;
    WOLFSSL_EVP_PKEY* pkey;
#ifndef NO_RSA
    WOLFSSL_RSA* rsa;
#endif
#ifndef NO_DSA
    WOLFSSL_DSA* dsa;
#endif
#ifdef HAVE_ECC
    WOLFSSL_EC_KEY* ecKey;
#endif

    printf(testingFmt, "wolfSSL_EVP_PKEY_assign");
#ifndef NO_RSA
    type = EVP_PKEY_RSA;
    AssertNotNull(pkey = wolfSSL_PKEY_new());
    AssertNotNull(rsa = wolfSSL_RSA_new());
    AssertIntEQ(wolfSSL_EVP_PKEY_assign(NULL,type,rsa),  WOLFSSL_FAILURE);
    AssertIntEQ(wolfSSL_EVP_PKEY_assign(pkey,type,NULL), WOLFSSL_FAILURE);
    AssertIntEQ(wolfSSL_EVP_PKEY_assign(pkey,-1,rsa),    WOLFSSL_FAILURE);
    AssertIntEQ(wolfSSL_EVP_PKEY_assign(pkey,type,rsa),  WOLFSSL_SUCCESS);
    wolfSSL_EVP_PKEY_free(pkey);
#endif /* NO_RSA */

#ifndef NO_DSA
    type = EVP_PKEY_DSA;
    AssertNotNull(pkey = wolfSSL_PKEY_new());
    AssertNotNull(dsa = wolfSSL_DSA_new());
    AssertIntEQ(wolfSSL_EVP_PKEY_assign(NULL,type,dsa),  WOLFSSL_FAILURE);
    AssertIntEQ(wolfSSL_EVP_PKEY_assign(pkey,type,NULL), WOLFSSL_FAILURE);
    AssertIntEQ(wolfSSL_EVP_PKEY_assign(pkey,-1,dsa),    WOLFSSL_FAILURE);
    AssertIntEQ(wolfSSL_EVP_PKEY_assign(pkey,type,dsa),  WOLFSSL_SUCCESS);
    wolfSSL_EVP_PKEY_free(pkey);
#endif /* NO_DSA */

#ifdef HAVE_ECC
    type = EVP_PKEY_EC;
    AssertNotNull(pkey = wolfSSL_PKEY_new());
    AssertNotNull(ecKey = wolfSSL_EC_KEY_new());
    AssertIntEQ(wolfSSL_EVP_PKEY_assign(NULL,type,ecKey), WOLFSSL_FAILURE);
    AssertIntEQ(wolfSSL_EVP_PKEY_assign(pkey,type,NULL),  WOLFSSL_FAILURE);
    AssertIntEQ(wolfSSL_EVP_PKEY_assign(pkey,-1,ecKey),   WOLFSSL_FAILURE);
    AssertIntEQ(wolfSSL_EVP_PKEY_assign(pkey,type,ecKey), WOLFSSL_SUCCESS);
    wolfSSL_EVP_PKEY_free(pkey);
#endif /* HAVE_ECC */
    printf(resultFmt, passed);
#endif /* OPENSSL_ALL */
}

static void test_wolfSSL_OBJ_ln(void)
{
    int i = 0, maxIdx = 7;
    const int nid_set[] = {NID_commonName,NID_countryName,NID_localityName,
                           NID_stateOrProvinceName,NID_organizationName,
                           NID_organizationalUnitName,NID_emailAddress};
    const char* ln_set[] = {WOLFSSL_LN_COMMON_NAME,WOLFSSL_LN_COUNTRY_NAME,
                            WOLFSSL_LN_LOCALITY_NAME,WOLFSSL_LN_STATE_NAME,
                            WOLFSSL_LN_ORG_NAME,WOLFSSL_LN_ORGUNIT_NAME,
                            WOLFSSL_EMAIL_ADDR};

    printf(testingFmt, "wolfSSL_OBJ_ln");

    AssertIntEQ(OBJ_ln2nid(NULL), BAD_FUNC_ARG);

#ifdef HAVE_ECC
    {
        int nCurves = 27;
        EC_builtin_curve r[nCurves];
        EC_get_builtin_curves(r,nCurves);

        for (i = 0; i < nCurves; i++) {
            AssertIntEQ(OBJ_ln2nid(r[i].comment), r[i].nid);
            AssertStrEQ(OBJ_nid2ln(r[i].nid), r[i].comment);
        }
    }
#endif

    for (i = 0; i < maxIdx; i++) {
        AssertIntEQ(OBJ_ln2nid(ln_set[i]), nid_set[i]);
        AssertStrEQ(OBJ_nid2ln(nid_set[i]), ln_set[i]);
    }

    printf(resultFmt, passed);
}

static void test_wolfSSL_OBJ_sn(void)
{
    int i = 0, maxIdx = 7;
    const int nid_set[] = {NID_commonName,NID_countryName,NID_localityName,
                           NID_stateOrProvinceName,NID_organizationName,
                           NID_organizationalUnitName,NID_emailAddress};
    const char* sn_open_set[] = {"CN","C","L","ST","O","OU","emailAddress"};
    const char* sn_wolf_set[] = {WOLFSSL_COMMON_NAME,WOLFSSL_COUNTRY_NAME,
                                WOLFSSL_LOCALITY_NAME, WOLFSSL_STATE_NAME,
                                WOLFSSL_ORG_NAME, WOLFSSL_ORGUNIT_NAME,
                                WOLFSSL_EMAIL_ADDR};

    printf(testingFmt, "wolfSSL_OBJ_sn");

    AssertIntEQ(wolfSSL_OBJ_sn2nid(NULL), NID_undef);

    #ifdef HAVE_ECC
    {
        int nCurves = 27;
        EC_builtin_curve r[nCurves];
        EC_get_builtin_curves(r,nCurves);

        for (i = 0; i < nCurves; i++) {
            AssertIntEQ(wolfSSL_OBJ_sn2nid(r[i].comment), r[i].nid);
            AssertStrEQ(wolfSSL_OBJ_nid2sn(r[i].nid), r[i].comment);
        }
    }
    #endif
    for (i = 0; i < maxIdx; i++) {
        AssertIntEQ(wolfSSL_OBJ_sn2nid(sn_wolf_set[i]), nid_set[i]);
        AssertStrEQ(wolfSSL_OBJ_nid2sn(nid_set[i]), sn_open_set[i]);
    }

    printf(resultFmt, passed);
}
//carie
#endif /* WOLFSSL_QT */


static void test_wolfSSL_X509V3_EXT_get(void) {
#if !defined(NO_FILESYSTEM) && defined (OPENSSL_ALL)
    FILE* f;
    int numOfExt =0;
    int extNid = 0;
    int i = 0;
    WOLFSSL_X509* x509;
    WOLFSSL_X509_EXTENSION* ext;
    const WOLFSSL_v3_ext_method* method;

    AssertNotNull(f = fopen("./certs/server-cert.pem", "rb"));
    AssertNotNull(x509 = wolfSSL_PEM_read_X509(f, NULL, NULL, NULL));
    fclose(f);

    printf(testingFmt, "wolfSSL_X509V3_EXT_get() return struct and nid test");
    AssertIntEQ((numOfExt = wolfSSL_X509_get_ext_count(x509)), 3);
    for (i = 0; i < numOfExt; i++) {
        AssertNotNull(ext = wolfSSL_X509_get_ext(x509, i));
        AssertNotNull(extNid = ext->obj->nid);
        AssertNotNull(method = wolfSSL_X509V3_EXT_get(ext));
        AssertIntEQ(method->ext_nid, extNid);
    }
    printf(resultFmt, "passed");

    printf(testingFmt, "wolfSSL_X509V3_EXT_get() NULL argument test");
    AssertNull(method = wolfSSL_X509V3_EXT_get(NULL));
    printf(resultFmt, "passed");

    wolfSSL_X509_free(x509);
#endif
}

static void test_wolfSSL_X509V3_EXT_d2i(void) {
#if !defined(NO_FILESYSTEM) && defined (OPENSSL_ALL)
    FILE* f;
    int numOfExt = 0, nid = 0, i = 0, expected, actual;
    char* str;
    unsigned char* data;
    const WOLFSSL_v3_ext_method* method;
    WOLFSSL_X509* x509;
    WOLFSSL_X509_EXTENSION* ext;
    WOLFSSL_ASN1_OBJECT *obj, *adObj;
    WOLFSSL_ASN1_STRING* asn1str;
    WOLFSSL_AUTHORITY_KEYID* aKeyId;
    WOLFSSL_AUTHORITY_INFO_ACCESS* aia;
    WOLFSSL_BASIC_CONSTRAINTS* bc;
    WOLFSSL_ACCESS_DESCRIPTION* ad;
    WOLFSSL_GENERAL_NAME* gn;

    printf(testingFmt, "wolfSSL_X509V3_EXT_d2i()");

    /* Check NULL argument */
    AssertNull(wolfSSL_X509V3_EXT_d2i(NULL));

    /* Using OCSP cert with X509V3 extensions */
    AssertNotNull(f = fopen("./certs/ocsp/root-ca-cert.pem", "rb"));
    AssertNotNull(x509 = wolfSSL_PEM_read_X509(f, NULL, NULL, NULL));
    fclose(f);

    AssertIntEQ((numOfExt = wolfSSL_X509_get_ext_count(x509)), 5);

    /* Basic Constraints */
    AssertNotNull(ext = wolfSSL_X509_get_ext(x509, i));
    AssertNotNull(obj = wolfSSL_X509_EXTENSION_get_object(ext));
    AssertIntEQ((nid = wolfSSL_OBJ_obj2nid(obj)), NID_basic_constraints);
    AssertNotNull(bc = (WOLFSSL_BASIC_CONSTRAINTS*)wolfSSL_X509V3_EXT_d2i(ext));

    AssertIntEQ(bc->ca, 1);
    AssertNull(bc->pathlen);
    wolfSSL_BASIC_CONSTRAINTS_free(bc);
    i++;

    /* Subject Key Identifier */
    AssertNotNull(ext = wolfSSL_X509_get_ext(x509, i));
    AssertNotNull(obj = wolfSSL_X509_EXTENSION_get_object(ext));
    AssertIntEQ((nid = wolfSSL_OBJ_obj2nid(obj)), NID_subject_key_identifier);

    AssertNotNull(asn1str = (WOLFSSL_ASN1_STRING*)wolfSSL_X509V3_EXT_d2i(ext));
    AssertNotNull(method = wolfSSL_X509V3_EXT_get(ext));
    AssertNotNull(method->i2s);
    AssertNotNull(str = method->i2s((WOLFSSL_v3_ext_method*)method, asn1str));
    wolfSSL_ASN1_STRING_free(asn1str);
    actual = strcmp(str,
                 "73:B0:1C:A4:2F:82:CB:CF:47:A5:38:D7:B0:04:82:3A:7E:72:15:21");
    AssertIntEQ(actual, 0);
    XFREE(str, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    i++;

    /* Authority Key Identifier */
    AssertNotNull(ext = wolfSSL_X509_get_ext(x509, i));
    AssertNotNull(obj = wolfSSL_X509_EXTENSION_get_object(ext));
    AssertIntEQ((nid = wolfSSL_OBJ_obj2nid(obj)), NID_authority_key_identifier);

    AssertNotNull(aKeyId =
                         (WOLFSSL_AUTHORITY_KEYID*)wolfSSL_X509V3_EXT_d2i(ext));
    AssertNotNull(method = wolfSSL_X509V3_EXT_get(ext));
    AssertNotNull(asn1str = aKeyId->keyid);
    AssertNotNull(str =
              wolfSSL_i2s_ASN1_STRING((WOLFSSL_v3_ext_method*)method, asn1str));
    actual = strcmp(str,
                 "73:B0:1C:A4:2F:82:CB:CF:47:A5:38:D7:B0:04:82:3A:7E:72:15:21");
    AssertIntEQ(actual, 0);
    XFREE(str, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    wolfSSL_AUTHORITY_KEYID_free(aKeyId);
    i++;

    /* Key Usage */
    AssertNotNull(ext = wolfSSL_X509_get_ext(x509, i));
    AssertNotNull(obj = wolfSSL_X509_EXTENSION_get_object(ext));
    AssertIntEQ((nid = wolfSSL_OBJ_obj2nid(obj)), NID_key_usage);

    AssertNotNull(asn1str = (WOLFSSL_ASN1_STRING*)wolfSSL_X509V3_EXT_d2i(ext));
    AssertNotNull(data = wolfSSL_ASN1_STRING_data(asn1str));
    expected = KEYUSE_KEY_CERT_SIGN | KEYUSE_CRL_SIGN;
    actual = data[0];
    AssertIntEQ(actual, expected);
    wolfSSL_ASN1_STRING_free(asn1str);
#if 0
    i++;

    /* Authority Info Access */
    AssertNotNull(ext = wolfSSL_X509_get_ext(x509, i));
    AssertNotNull(obj = wolfSSL_X509_EXTENSION_get_object(ext));
    AssertIntEQ((nid = wolfSSL_OBJ_obj2nid(obj)), NID_info_access);
    AssertNotNull(aia = wolfSSL_X509V3_EXT_d2i(ext));
    AssertIntEQ(wolfSSL_sk_num(aia), 1); /* Only one URI entry for this cert */

    /* URI entry is an ACCESS_DESCRIPTION type */
    AssertNotNull(ad = wolfSSL_sk_value(aia, 0));
    AssertNotNull(adObj = ad->method);
    /* Make sure nid is OCSP */
    AssertIntEQ(wolfSSL_OBJ_obj2nid(adObj), AIA_OCSP_OID);

    /* GENERAL_NAME stores URI as an ASN1_STRING */
    AssertNotNull(gn = ad->location);
    AssertIntEQ(gn->type, GEN_URI); /* Type should always be GEN_URI */
    AssertNotNull(asn1str = gn->d.uniformResourceIdentifier);
    AssertIntEQ(wolfSSL_ASN1_STRING_length(asn1str), 22);
    str = (char*)wolfSSL_ASN1_STRING_data(asn1str);
    actual = strcmp(str, "http://127.0.0.1:22220");
    AssertIntEQ(actual, 0);

    wolfSSL_sk_ACCESS_DESCRIPTION_pop_free(aia, NULL);
    XFREE(ad, NULL, DYNAMIC_TYPE_X509_EXT);
#else
    (void) aia; (void) ad; (void) adObj; (void) gn;
#endif
    wolfSSL_X509_free(x509);
    printf(resultFmt, "passed");
#endif
}

static void test_wolfSSL_X509_get_ext(void){
#if !defined(NO_FILESYSTEM) && defined (OPENSSL_ALL)
    int ret = 0;
    FILE* f;
    WOLFSSL_X509* x509;
    WOLFSSL_X509_EXTENSION* foundExtension;

    AssertNotNull(f = fopen("./certs/server-cert.pem", "rb"));
    AssertNotNull(x509 = wolfSSL_PEM_read_X509(f, NULL, NULL, NULL));
    fclose(f);
    AssertIntEQ((ret = wolfSSL_X509_get_ext_count(x509)), 3);

    printf(testingFmt, "wolfSSL_X509_get_ext() valid input");
    AssertNotNull(foundExtension = wolfSSL_X509_get_ext(x509, 0));
    printf(resultFmt, "passed");

    printf(testingFmt, "wolfSSL_X509_get_ext() valid x509, idx out of bounds");
    AssertNull(foundExtension = wolfSSL_X509_get_ext(x509, -1));
    AssertNull(foundExtension = wolfSSL_X509_get_ext(x509, 100));
    printf(resultFmt, "passed");

    printf(testingFmt, "wolfSSL_X509_get_ext() NULL x509, idx out of bounds");
    AssertNull(foundExtension = wolfSSL_X509_get_ext(NULL, -1));
    AssertNull(foundExtension = wolfSSL_X509_get_ext(NULL, 100));
    printf(resultFmt, "passed");

    printf(testingFmt, "wolfSSL_X509_get_ext() NULL x509, valid idx");
    AssertNull(foundExtension = wolfSSL_X509_get_ext(NULL, 0));
    printf(resultFmt, "passed");

    wolfSSL_X509_free(x509);
#endif
}

static void test_wolfSSL_X509_get_ext_by_NID(void)
{
#if defined(OPENSSL_ALL)
    int rc;
    FILE* f;
    WOLFSSL_X509* x509;

    AssertNotNull(f = fopen("./certs/server-cert.pem", "rb"));
    AssertNotNull(x509 = wolfSSL_PEM_read_X509(f, NULL, NULL, NULL));
    fclose(f);

    rc = wolfSSL_X509_get_ext_by_NID(x509, NID_basic_constraints, -1);
    AssertIntGE(rc, 0);

    /* Start search from last location (should fail) */
    rc = wolfSSL_X509_get_ext_by_NID(x509, NID_basic_constraints, rc);
    AssertIntGE(rc, -1);

    rc = wolfSSL_X509_get_ext_by_NID(x509, NID_basic_constraints, -2);
    AssertIntGE(rc, -1);

    rc = wolfSSL_X509_get_ext_by_NID(NULL, NID_basic_constraints, -1);
    AssertIntEQ(rc, -1);

    rc = wolfSSL_X509_get_ext_by_NID(x509, NID_undef, -1);
    AssertIntEQ(rc, -1);

    wolfSSL_X509_free(x509);

#endif
}

static void test_wolfSSL_X509_EXTENSION_new(void)
{
#if defined (OPENSSL_ALL)
    WOLFSSL_X509_EXTENSION* ext;

    AssertNotNull(ext = wolfSSL_X509_EXTENSION_new());
    AssertNotNull(ext->obj = wolfSSL_ASN1_OBJECT_new());
    ext->obj->nid = WOLFSSL_SUCCESS;
    AssertIntEQ(WOLFSSL_SUCCESS, ext->obj->nid);

    wolfSSL_X509_EXTENSION_free(ext);
#endif
}

static void test_wolfSSL_X509_EXTENSION_get_object(void)
{
#if !defined(NO_FILESYSTEM) && defined (OPENSSL_ALL)
    WOLFSSL_X509* x509;
    WOLFSSL_X509_EXTENSION* ext;
    WOLFSSL_ASN1_OBJECT* o;
    FILE* file;
    int nid = 0;

    AssertNotNull(file = fopen("./certs/server-cert.pem", "rb"));
    AssertNotNull(x509 = wolfSSL_PEM_read_X509(file, NULL, NULL, NULL));
    fclose(file);

    printf(testingFmt, "wolfSSL_X509_EXTENSION_get_object() testing ext idx 0");
    AssertNotNull(ext = wolfSSL_X509_get_ext(x509, 0));
    AssertNotNull(o = wolfSSL_X509_EXTENSION_get_object(ext));
    AssertIntEQ(o->nid, 128);
    nid = o->nid;
    printf(resultFmt, nid == 128 ? passed : failed);

    printf(testingFmt, "wolfSSL_X509_EXTENSION_get_object() NULL argument");
    AssertNull(o = wolfSSL_X509_EXTENSION_get_object(NULL));
    printf(resultFmt, passed);

    wolfSSL_X509_free(x509);
#endif
}

static void test_wolfSSL_X509_EXTENSION_get_data(void)
{
#if !defined(NO_FILESYSTEM) && defined (OPENSSL_ALL)
    WOLFSSL_X509* x509;
    WOLFSSL_X509_EXTENSION* ext;
    WOLFSSL_ASN1_STRING* str;
    FILE* file;

    printf(testingFmt, "wolfSSL_X509_EXTENSION_get_data");

    AssertNotNull(file = fopen("./certs/server-cert.pem", "rb"));
    AssertNotNull(x509 = wolfSSL_PEM_read_X509(file, NULL, NULL, NULL));
    fclose(file);
    AssertNotNull(ext = wolfSSL_X509_get_ext(x509, 0));

    AssertNotNull(str = wolfSSL_X509_EXTENSION_get_data(ext));
    printf(resultFmt, passed);

    wolfSSL_X509_free(x509);
#endif
}

static void test_wolfSSL_X509_EXTENSION_get_critical(void)
{
#if !defined(NO_FILESYSTEM) && defined (OPENSSL_ALL)
    WOLFSSL_X509* x509;
    WOLFSSL_X509_EXTENSION* ext;
    FILE* file;
    int crit = -1;

    printf(testingFmt, "wolfSSL_X509_EXTENSION_get_critical");

    AssertNotNull(file = fopen("./certs/server-cert.pem", "rb"));
    AssertNotNull(x509 = wolfSSL_PEM_read_X509(file, NULL, NULL, NULL));
    fclose(file);
    AssertNotNull(ext = wolfSSL_X509_get_ext(x509, 0));

    crit = wolfSSL_X509_EXTENSION_get_critical(ext);
    AssertIntEQ(crit, 0);
    printf(resultFmt, passed);

    wolfSSL_X509_free(x509);
#endif
}

static void test_wolfSSL_X509V3_EXT_print(void)
{
#if !defined(NO_FILESYSTEM) && defined (OPENSSL_ALL)
    printf(testingFmt, "wolfSSL_X509V3_EXT_print");

    {
        FILE* f;
        WOLFSSL_X509* x509;
        X509_EXTENSION * ext = NULL;
        int loc;
        BIO *bio = NULL;

        AssertNotNull(f = fopen(svrCertFile, "rb"));
        AssertNotNull(x509 = wolfSSL_PEM_read_X509(f, NULL, NULL, NULL));
        fclose(f);

        AssertNotNull(bio = wolfSSL_BIO_new(BIO_s_mem()));

        loc = wolfSSL_X509_get_ext_by_NID(x509, NID_basic_constraints, -1);
        AssertIntGT(loc, -1);
        AssertNotNull(ext = wolfSSL_X509_get_ext(x509, loc));
        AssertIntEQ(wolfSSL_X509V3_EXT_print(bio, ext, 0, 0), WOLFSSL_SUCCESS);

        loc = wolfSSL_X509_get_ext_by_NID(x509, NID_subject_key_identifier, -1);
        AssertIntGT(loc, -1);
        AssertNotNull(ext = wolfSSL_X509_get_ext(x509, loc));
        AssertIntEQ(wolfSSL_X509V3_EXT_print(bio, ext, 0, 0), WOLFSSL_SUCCESS);

        loc = wolfSSL_X509_get_ext_by_NID(x509, NID_authority_key_identifier, -1);
        AssertIntGT(loc, -1);
        AssertNotNull(ext = wolfSSL_X509_get_ext(x509, loc));
        AssertIntEQ(wolfSSL_X509V3_EXT_print(bio, ext, 0, 0), WOLFSSL_SUCCESS);

        wolfSSL_BIO_free(bio);
        wolfSSL_X509_free(x509);
    }

    {
        X509 *x509;
        BIO *bio;
        X509_EXTENSION *ext;
        unsigned int i;
        unsigned int idx;
        /* Some NIDs to test with */
        int nids[] = {
                /* NID_key_usage, currently X509_get_ext returns this as a bit
                 * string, which messes up X509V3_EXT_print */
                /* NID_ext_key_usage, */
                NID_subject_alt_name,
        };
        int* n;

        printf(testingFmt, "wolfSSL_X509V3_EXT_print");
        AssertNotNull(bio = BIO_new_fp(stderr, BIO_NOCLOSE));

        AssertNotNull(x509 = wolfSSL_X509_load_certificate_file(cliCertFileExt,
            WOLFSSL_FILETYPE_PEM));

        printf("\nPrinting extension values:\n");

        for (i = 0, n = nids; i<(sizeof(nids)/sizeof(int)); i++, n++) {
            /* X509_get_ext_by_NID should return 3 for now. If that changes then
             * update the index */
            AssertIntEQ((idx = X509_get_ext_by_NID(x509, *n, -1)), 3);
            AssertNotNull(ext = X509_get_ext(x509, idx));
            AssertIntEQ(X509V3_EXT_print(bio, ext, 0, 0), 1);
            printf("\n");
        }

        BIO_free(bio);
        X509_free(x509);
    }
    printf(resultFmt, passed);
#endif
}

static void test_wolfSSL_X509_cmp(void)
{
#if defined(OPENSSL_ALL)
    FILE* file1;
    FILE* file2;
    WOLFSSL_X509* cert1;
    WOLFSSL_X509* cert2;
    int ret = 0;

    AssertNotNull(file1=fopen("./certs/server-cert.pem", "rb"));
    AssertNotNull(file2=fopen("./certs/client-cert-3072.pem", "rb"));

    AssertNotNull(cert1 = wolfSSL_PEM_read_X509(file1, NULL, NULL, NULL));
    AssertNotNull(cert2 = wolfSSL_PEM_read_X509(file2, NULL, NULL, NULL));
    fclose(file1);
    fclose(file2);

    printf(testingFmt, "wolfSSL_X509_cmp() testing matching certs");
    ret = wolfSSL_X509_cmp(cert1, cert1);
    AssertIntEQ(0, wolfSSL_X509_cmp(cert1, cert1));
    printf(resultFmt, ret == 0 ? passed : failed);

    printf(testingFmt, "wolfSSL_X509_cmp() testing mismatched certs");
    ret = wolfSSL_X509_cmp(cert1, cert2);
    AssertIntEQ(-1, wolfSSL_X509_cmp(cert1, cert2));
    printf(resultFmt, ret == -1 ? passed : failed);

    printf(testingFmt, "wolfSSL_X509_cmp() testing NULL, valid args");
    ret = wolfSSL_X509_cmp(NULL, cert2);
    AssertIntEQ(BAD_FUNC_ARG, wolfSSL_X509_cmp(NULL, cert2));
    printf(resultFmt, ret == BAD_FUNC_ARG ? passed : failed);

    printf(testingFmt, "wolfSSL_X509_cmp() testing valid, NULL args");
    ret = wolfSSL_X509_cmp(cert1, NULL);
    AssertIntEQ(BAD_FUNC_ARG, wolfSSL_X509_cmp(cert1, NULL));
    printf(resultFmt, ret == BAD_FUNC_ARG ? passed : failed);

    printf(testingFmt, "wolfSSL_X509_cmp() testing NULL, NULL args");
    ret = wolfSSL_X509_cmp(NULL, NULL);
    AssertIntEQ(BAD_FUNC_ARG, wolfSSL_X509_cmp(NULL, NULL));
    printf(resultFmt, ret == BAD_FUNC_ARG ? passed : failed);

    wolfSSL_X509_free(cert1);
    wolfSSL_X509_free(cert2);
#endif
}

static void test_wolfSSL_PKEY_up_ref()
{
#if defined(OPENSSL_ALL)
    EVP_PKEY* pkey;
    printf(testingFmt, "wolfSSL_PKEY_up_ref()");

    pkey = EVP_PKEY_new();
    AssertIntEQ(EVP_PKEY_up_ref(NULL), 0);
    AssertIntEQ(EVP_PKEY_up_ref(pkey), 1);
    EVP_PKEY_free(pkey);
    AssertIntEQ(EVP_PKEY_up_ref(pkey), 1);
    EVP_PKEY_free(pkey);
    EVP_PKEY_free(pkey);

    printf(resultFmt, "passed");
#endif
}

static void test_wolfSSL_i2d_PrivateKey()
{
#if (!defined(NO_RSA) || defined(HAVE_ECC)) && defined(OPENSSL_EXTRA)

    printf(testingFmt, "wolfSSL_i2d_PrivateKey()");
#if !defined(NO_RSA) && defined(USE_CERT_BUFFERS_2048)
    {
        EVP_PKEY* pkey;
        const unsigned char* server_key = (const unsigned char*)server_key_der_2048;
        unsigned char buf[FOURK_BUF];
        unsigned char* pt;
        int bufSz;

        AssertNotNull(pkey = d2i_PrivateKey(EVP_PKEY_RSA, NULL, &server_key,
                    (long)sizeof_server_key_der_2048));
        AssertIntEQ(i2d_PrivateKey(pkey, NULL), 1193);
        pt = buf;
        AssertIntEQ((bufSz = i2d_PrivateKey(pkey, &pt)), 1193);
        AssertIntNE((pt - buf), 0);
        AssertIntEQ(XMEMCMP(buf, server_key_der_2048, bufSz), 0);
        EVP_PKEY_free(pkey);
    }
#endif
#if defined(OPENSSL_EXTRA) && defined(HAVE_ECC) && defined(USE_CERT_BUFFERS_256)
    {
        EVP_PKEY* pkey;
        const unsigned char* client_key =
            (const unsigned char*)ecc_clikey_der_256;
        unsigned char buf[FOURK_BUF];
        unsigned char* pt;
        int bufSz;

        AssertNotNull((pkey = d2i_PrivateKey(EVP_PKEY_EC, NULL, &client_key,
                                                   sizeof_ecc_clikey_der_256)));
        AssertIntEQ(i2d_PrivateKey(pkey, NULL), 121);
        pt = buf;
        AssertIntEQ((bufSz = i2d_PrivateKey(pkey, &pt)), 121);
        AssertIntNE((pt - buf), 0);
        AssertIntEQ(XMEMCMP(buf, ecc_clikey_der_256, bufSz), 0);
        EVP_PKEY_free(pkey);
    }
#endif

    printf(resultFmt, "passed");
#endif
}

static void test_wolfSSL_OCSP_get0_info()
{
#if defined(OPENSSL_ALL) && defined(HAVE_OCSP) && !defined(NO_FILESYSTEM)
    X509* cert;
    X509* issuer;
    OCSP_CERTID* id;

    ASN1_STRING* name = NULL;
    ASN1_OBJECT* pmd  = NULL;
    ASN1_STRING* keyHash = NULL;
    ASN1_INTEGER* serial = NULL;
    ASN1_INTEGER* x509Int = NULL;

    printf(testingFmt, "wolfSSL_OCSP_get0_info()");

    AssertNotNull(cert =
            wolfSSL_X509_load_certificate_file(svrCertFile, SSL_FILETYPE_PEM));
    AssertNotNull(issuer =
            wolfSSL_X509_load_certificate_file(caCertFile, SSL_FILETYPE_PEM));

    id = OCSP_cert_to_id(NULL, cert, issuer);
    AssertNotNull(id);

    AssertIntEQ(OCSP_id_get0_info(NULL, NULL, NULL, NULL, NULL), 0);
    AssertIntEQ(OCSP_id_get0_info(NULL, NULL, NULL, NULL, id), 1);

    /* name, pmd, keyHash not supported yet, expect failure if not NULL */
    AssertIntEQ(OCSP_id_get0_info(&name, NULL, NULL, NULL, id), 0);
    AssertIntEQ(OCSP_id_get0_info(NULL, &pmd, NULL, NULL, id), 0);
    AssertIntEQ(OCSP_id_get0_info(NULL, NULL, &keyHash, NULL, id), 0);

    AssertIntEQ(OCSP_id_get0_info(NULL, NULL, NULL, &serial, id), 1);
    AssertNotNull(serial);

    /* compare serial number to one in cert, should be equal */
    x509Int = X509_get_serialNumber(cert);
    AssertNotNull(x509Int);
    AssertIntEQ(x509Int->dataMax, serial->dataMax);
    AssertIntEQ(XMEMCMP(x509Int->data, serial->data, serial->dataMax), 0);

    OCSP_CERTID_free(id);
    X509_free(cert); /* free's x509Int */
    X509_free(issuer);

    printf(resultFmt, "passed");
#endif /* OPENSSL_EXTRA & HAVE_OCSP */
}

static void test_wolfSSL_EC_get_builtin_curves(void)
{
#if defined(HAVE_ECC) && (defined(OPENSSL_EXTRA) || defined(OPENSSL_ALL))
    EC_builtin_curve* curves = NULL;
    size_t crv_len = 0;
    size_t i = 0;

    printf(testingFmt, "wolfSSL_EC_get_builtin_curves");

    AssertIntGT((crv_len = EC_get_builtin_curves(NULL, 0)), 0);
    AssertNotNull(curves = (EC_builtin_curve*)
            XMALLOC(sizeof(EC_builtin_curve)*crv_len, NULL,
                    DYNAMIC_TYPE_TMP_BUFFER));
    AssertIntEQ(EC_get_builtin_curves(curves, crv_len), crv_len);

    for (i = 0; i < crv_len; i++)
    {
        AssertStrEQ(OBJ_nid2sn(curves[i].nid), curves[i].comment);
    }

    XFREE(curves, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    printf(resultFmt, passed);
#endif /* defined(HAVE_ECC) || defined(OPENSSL_EXTRA) || defined(OPENSSL_ALL) */
}

static void test_no_op_functions(void)
{
    #if defined(OPENSSL_EXTRA)
    printf(testingFmt, "no_op_functions()");

    /* this makes sure wolfSSL can compile and run these no-op functions */
    SSL_load_error_strings();
    ENGINE_load_builtin_engines();
    OpenSSL_add_all_ciphers();
    AssertIntEQ(CRYPTO_malloc_init(), 0);

    printf(resultFmt, passed);
    #endif
}


/*----------------------------------------------------------------------------*
 | wolfCrypt ASN
 *----------------------------------------------------------------------------*/

static void test_wc_GetPkcs8TraditionalOffset(void)
{
#if !defined(NO_ASN) && !defined(NO_FILESYSTEM) && defined(HAVE_PKCS8)
    int length, derSz;
    word32 inOutIdx;
    const char* path = "./certs/server-keyPkcs8.der";
    XFILE file;
    byte der[2048];

    printf(testingFmt, "wc_GetPkcs8TraditionalOffset");

    file = XFOPEN(path, "rb");
    AssertTrue(file != XBADFILE);
    derSz = (int)XFREAD(der, 1, sizeof(der), file);
    XFCLOSE(file);

    /* valid case */
    inOutIdx = 0;
    length = wc_GetPkcs8TraditionalOffset(der, &inOutIdx, derSz);
    AssertIntGT(length, 0);

    /* inOutIdx > sz */
    inOutIdx = 4000;
    length = wc_GetPkcs8TraditionalOffset(der, &inOutIdx, derSz);
    AssertIntEQ(length, BAD_FUNC_ARG);

    /* null input */
    inOutIdx = 0;
    length = wc_GetPkcs8TraditionalOffset(NULL, &inOutIdx, 0);
    AssertIntEQ(length, BAD_FUNC_ARG);

    /* invalid input, fill buffer with 1's */
    XMEMSET(der, 1, sizeof(der));
    inOutIdx = 0;
    length = wc_GetPkcs8TraditionalOffset(der, &inOutIdx, derSz);
    AssertIntEQ(length, ASN_PARSE_E);

    printf(resultFmt, passed);
#endif /* NO_ASN */
}

static void test_wc_SetSubjectRaw(void)
{
#if !defined(NO_ASN) && !defined(NO_FILESYSTEM) && defined(OPENSSL_EXTRA) && \
    defined(WOLFSSL_CERT_GEN) && defined(WOLFSSL_CERT_EXT) && !defined(NO_RSA)
    const char* joiCertFile = "./certs/test/cert-ext-joi.pem";
    WOLFSSL_X509* x509;
    int peerCertSz;
    const byte* peerCertBuf;
    Cert forgedCert;

    printf(testingFmt, "test_wc_SetSubjectRaw()");

    AssertNotNull(x509 = wolfSSL_X509_load_certificate_file(joiCertFile, WOLFSSL_FILETYPE_PEM));

    AssertNotNull(peerCertBuf = wolfSSL_X509_get_der(x509, &peerCertSz));

    AssertIntEQ(0, wc_InitCert(&forgedCert));

    AssertIntEQ(0, wc_SetSubjectRaw(&forgedCert, peerCertBuf, peerCertSz));

    wolfSSL_FreeX509(x509);

    printf(resultFmt, passed);
#endif
}

static void test_wc_GetSubjectRaw(void)
{
#if !defined(NO_ASN) && !defined(NO_FILESYSTEM) && defined(OPENSSL_EXTRA) && \
    defined(WOLFSSL_CERT_GEN) && defined(WOLFSSL_CERT_EXT)
    Cert cert;
    byte *subjectRaw;

    printf(testingFmt, "test_wc_GetSubjectRaw()");

    AssertIntEQ(0, wc_InitCert(&cert));
    AssertIntEQ(0, wc_GetSubjectRaw(&subjectRaw, &cert));

    printf(resultFmt, passed);
#endif
}

static void test_wc_SetIssuerRaw(void)
{
#if !defined(NO_ASN) && !defined(NO_FILESYSTEM) && defined(OPENSSL_EXTRA) && \
    defined(WOLFSSL_CERT_GEN) && defined(WOLFSSL_CERT_EXT) && !defined(NO_RSA)
    const char* joiCertFile = "./certs/test/cert-ext-joi.pem";
    WOLFSSL_X509* x509;
    int peerCertSz;
    const byte* peerCertBuf;
    Cert forgedCert;

    printf(testingFmt, "test_wc_SetIssuerRaw()");

    AssertNotNull(x509 = wolfSSL_X509_load_certificate_file(joiCertFile, WOLFSSL_FILETYPE_PEM));

    AssertNotNull(peerCertBuf = wolfSSL_X509_get_der(x509, &peerCertSz));

    AssertIntEQ(0, wc_InitCert(&forgedCert));

    AssertIntEQ(0, wc_SetIssuerRaw(&forgedCert, peerCertBuf, peerCertSz));

    wolfSSL_FreeX509(x509);

    printf(resultFmt, passed);
#endif
}

static void test_wc_SetIssueBuffer(void)
{
#if !defined(NO_ASN) && !defined(NO_FILESYSTEM) && defined(OPENSSL_EXTRA) && \
    defined(WOLFSSL_CERT_GEN) && defined(WOLFSSL_CERT_EXT) && !defined(NO_RSA)
    const char* joiCertFile = "./certs/test/cert-ext-joi.pem";
    WOLFSSL_X509* x509;
    int peerCertSz;
    const byte* peerCertBuf;
    Cert forgedCert;

    printf(testingFmt, "test_wc_SetIssuerBuffer()");

    AssertNotNull(x509 = wolfSSL_X509_load_certificate_file(joiCertFile, WOLFSSL_FILETYPE_PEM));

    AssertNotNull(peerCertBuf = wolfSSL_X509_get_der(x509, &peerCertSz));

    AssertIntEQ(0, wc_InitCert(&forgedCert));

    AssertIntEQ(0, wc_SetIssuerBuffer(&forgedCert, peerCertBuf, peerCertSz));

    wolfSSL_FreeX509(x509);

    printf(resultFmt, passed);
#endif
}

/*
 * Testing wc_SetSubjectKeyId
 */
static void test_wc_SetSubjectKeyId(void)
{
#if !defined(NO_ASN) && !defined(NO_FILESYSTEM) && defined(OPENSSL_EXTRA) && \
    defined(WOLFSSL_CERT_GEN) && defined(WOLFSSL_CERT_EXT)
    Cert cert;
    const char* file = "certs/ecc-client-keyPub.pem";

    printf(testingFmt, "wc_SetSubjectKeyId()");

    AssertIntEQ(0, wc_InitCert(&cert));
    AssertIntEQ(0, wc_SetSubjectKeyId(&cert, file));

    AssertIntEQ(BAD_FUNC_ARG, wc_SetSubjectKeyId(NULL, file));
    AssertIntGT(0, wc_SetSubjectKeyId(&cert, "badfile.name"));

    printf(resultFmt, passed);
#endif
} /* END test_wc_SetSubjectKeyId */

/*
 * Testing wc_SetSubject
 */
static void test_wc_SetSubject(void)
{
#if !defined(NO_ASN) && !defined(NO_FILESYSTEM) && defined(OPENSSL_EXTRA) && \
    defined(WOLFSSL_CERT_GEN) && defined(WOLFSSL_CERT_EXT)
    Cert cert;
    const char* file = "./certs/ca-ecc-cert.pem";

    printf(testingFmt, "wc_SetSubject()");

    AssertIntEQ(0, wc_InitCert(&cert));
    AssertIntEQ(0, wc_SetSubject(&cert, file));

    AssertIntEQ(BAD_FUNC_ARG, wc_SetSubject(NULL, file));
    AssertIntGT(0, wc_SetSubject(&cert, "badfile.name"));

    printf(resultFmt, passed);
#endif
} /* END test_wc_SetSubject */


static void test_CheckCertSignature(void)
{
#if !defined(NO_CERTS) && defined(WOLFSSL_SMALL_CERT_VERIFY)
    WOLFSSL_CERT_MANAGER* cm = NULL;
#if !defined(NO_FILESYSTEM) && (!defined(NO_RSA) || defined(HAVE_ECC))
    FILE* fp;
    byte  cert[4096];
    int   certSz;
#endif

    AssertIntEQ(BAD_FUNC_ARG, CheckCertSignature(NULL, 0, NULL, NULL));
    AssertNotNull(cm = wolfSSL_CertManagerNew_ex(NULL));
    AssertIntEQ(BAD_FUNC_ARG, CheckCertSignature(NULL, 0, NULL, cm));

#ifndef NO_RSA
#ifdef USE_CERT_BUFFERS_1024
    AssertIntEQ(ASN_NO_SIGNER_E, CheckCertSignature(server_cert_der_1024,
                sizeof_server_cert_der_1024, NULL, cm));
    AssertIntEQ(WOLFSSL_SUCCESS, wolfSSL_CertManagerLoadCABuffer(cm,
                ca_cert_der_1024, sizeof_ca_cert_der_1024,
                WOLFSSL_FILETYPE_ASN1));
    AssertIntEQ(0, CheckCertSignature(server_cert_der_1024,
                sizeof_server_cert_der_1024, NULL, cm));
#elif defined(USE_CERT_BUFFERS_2048)
    AssertIntEQ(ASN_NO_SIGNER_E, CheckCertSignature(server_cert_der_2048,
                sizeof_server_cert_der_2048, NULL, cm));
    AssertIntEQ(WOLFSSL_SUCCESS, wolfSSL_CertManagerLoadCABuffer(cm,
                ca_cert_der_2048, sizeof_ca_cert_der_2048,
                WOLFSSL_FILETYPE_ASN1));
    AssertIntEQ(0, CheckCertSignature(server_cert_der_2048,
                sizeof_server_cert_der_2048, NULL, cm));
#endif
#endif

#if defined(HAVE_ECC) && defined(USE_CERT_BUFFERS_256)
    AssertIntEQ(ASN_NO_SIGNER_E, CheckCertSignature(serv_ecc_der_256,
                sizeof_serv_ecc_der_256, NULL, cm));
    AssertIntEQ(WOLFSSL_SUCCESS, wolfSSL_CertManagerLoadCABuffer(cm,
                ca_ecc_cert_der_256, sizeof_ca_ecc_cert_der_256,
                WOLFSSL_FILETYPE_ASN1));
    AssertIntEQ(0, CheckCertSignature(serv_ecc_der_256, sizeof_serv_ecc_der_256,
                NULL, cm));
#endif

#if !defined(NO_FILESYSTEM)
    wolfSSL_CertManagerFree(cm);
    AssertNotNull(cm = wolfSSL_CertManagerNew_ex(NULL));
#ifndef NO_RSA
    AssertNotNull(fp = XFOPEN("./certs/server-cert.der", "rb"));
    AssertIntGT((certSz = (int)XFREAD(cert, 1, sizeof(cert), fp)), 0);
    XFCLOSE(fp);
    AssertIntEQ(ASN_NO_SIGNER_E, CheckCertSignature(cert, certSz, NULL, cm));
    AssertIntEQ(WOLFSSL_SUCCESS, wolfSSL_CertManagerLoadCA(cm,
                "./certs/ca-cert.pem", NULL));
    AssertIntEQ(0, CheckCertSignature(cert, certSz, NULL, cm));
#endif
#ifdef HAVE_ECC
    AssertNotNull(fp = XFOPEN("./certs/server-ecc.der", "rb"));
    AssertIntGT((certSz = (int)XFREAD(cert, 1, sizeof(cert), fp)), 0);
    XFCLOSE(fp);
    AssertIntEQ(ASN_NO_SIGNER_E, CheckCertSignature(cert, certSz, NULL, cm));
    AssertIntEQ(WOLFSSL_SUCCESS, wolfSSL_CertManagerLoadCA(cm,
                "./certs/ca-ecc-cert.pem", NULL));
    AssertIntEQ(0, CheckCertSignature(cert, certSz, NULL, cm));
#endif
#endif

#if !defined(NO_FILESYSTEM) && (!defined(NO_RSA) || defined(HAVE_ECC))
    (void)fp;
    (void)cert;
    (void)certSz;
#endif

    wolfSSL_CertManagerFree(cm);
#endif
}

/*----------------------------------------------------------------------------*
 | wolfCrypt ECC
 *----------------------------------------------------------------------------*/

static void test_wc_ecc_get_curve_size_from_name(void)
{
#ifdef HAVE_ECC
    int ret;

    printf(testingFmt, "wc_ecc_get_curve_size_from_name");

    #if !defined(NO_ECC256) && !defined(NO_ECC_SECP)
        ret = wc_ecc_get_curve_size_from_name("SECP256R1");
        AssertIntEQ(ret, 32);
    #endif

    /* invalid case */
    ret = wc_ecc_get_curve_size_from_name("BADCURVE");
    AssertIntEQ(ret, -1);

    /* NULL input */
    ret = wc_ecc_get_curve_size_from_name(NULL);
    AssertIntEQ(ret, BAD_FUNC_ARG);

    printf(resultFmt, passed);
#endif /* HAVE_ECC */
}

static void test_wc_ecc_get_curve_id_from_name(void)
{
#ifdef HAVE_ECC
    int id;

    printf(testingFmt, "wc_ecc_get_curve_id_from_name");

    #if !defined(NO_ECC256) && !defined(NO_ECC_SECP)
        id = wc_ecc_get_curve_id_from_name("SECP256R1");
        AssertIntEQ(id, ECC_SECP256R1);
    #endif

    /* invalid case */
    id = wc_ecc_get_curve_id_from_name("BADCURVE");
    AssertIntEQ(id, -1);

    /* NULL input */
    id = wc_ecc_get_curve_id_from_name(NULL);
    AssertIntEQ(id, BAD_FUNC_ARG);

    printf(resultFmt, passed);
#endif /* HAVE_ECC */
}

#if defined(OPENSSL_EXTRA) && defined(HAVE_ECC) && \
    !defined(HAVE_SELFTEST) && \
    !(defined(HAVE_FIPS) || defined(HAVE_FIPS_VERSION))

static void test_wc_ecc_get_curve_id_from_dp_params(void)
{
    int id;
    int curve_id;
    int ret = 0;
    WOLFSSL_EC_KEY *ecKey;
    ecc_key* key;
    const ecc_set_type* params;

    printf(testingFmt, "wc_ecc_get_curve_id_from_dp_params");

    #if !defined(NO_ECC256) && !defined(NO_ECC_SECP)
        id = wc_ecc_get_curve_id_from_name("SECP256R1");
        AssertIntEQ(id, ECC_SECP256R1);

        ecKey = wolfSSL_EC_KEY_new_by_curve_name(id);
        AssertNotNull(ecKey);

        ret = wolfSSL_EC_KEY_generate_key(ecKey);

        if (ret == 0) {
            /* normal test */
            key = (ecc_key*)ecKey->internal;
            params = key->dp;

            curve_id = wc_ecc_get_curve_id_from_dp_params(params);
            AssertIntEQ(curve_id, id);
        }
    #endif
    /* invalid case, NULL input*/

    id = wc_ecc_get_curve_id_from_dp_params(NULL);
    AssertIntEQ(id, BAD_FUNC_ARG);
    wolfSSL_EC_KEY_free(ecKey);

    printf(resultFmt, passed);
}
#endif /* defined(OPENSSL_EXTRA) && defined(HAVE_ECC) */

static void test_wc_ecc_get_curve_id_from_params(void)
{
#ifdef HAVE_ECC
    int id;

    const byte prime[] =
    {
        0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x01,
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
        0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,
        0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF
    };

    const byte primeInvalid[] =
    {
        0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x01,
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
        0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,
        0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x01,0x01
    };

    const byte Af[] =
    {
        0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x01,
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
        0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,
        0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFC
    };

    const byte Bf[] =
    {
        0x5A,0xC6,0x35,0xD8,0xAA,0x3A,0x93,0xE7,
        0xB3,0xEB,0xBD,0x55,0x76,0x98,0x86,0xBC,
        0x65,0x1D,0x06,0xB0,0xCC,0x53,0xB0,0xF6,
        0x3B,0xCE,0x3C,0x3E,0x27,0xD2,0x60,0x4B
    };

    const byte order[] =
    {
        0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,
        0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
        0xBC,0xE6,0xFA,0xAD,0xA7,0x17,0x9E,0x84,
        0xF3,0xB9,0xCA,0xC2,0xFC,0x63,0x25,0x51
    };

    const byte Gx[] =
    {
        0x6B,0x17,0xD1,0xF2,0xE1,0x2C,0x42,0x47,
        0xF8,0xBC,0xE6,0xE5,0x63,0xA4,0x40,0xF2,
        0x77,0x03,0x7D,0x81,0x2D,0xEB,0x33,0xA0,
        0xF4,0xA1,0x39,0x45,0xD8,0x98,0xC2,0x96
    };

    const byte Gy[] =
    {
        0x4F,0xE3,0x42,0xE2,0xFE,0x1A,0x7F,0x9B,
        0x8E,0xE7,0xEB,0x4A,0x7C,0x0F,0x9E,0x16,
        0x2B,0xCE,0x33,0x57,0x6B,0x31,0x5E,0xCE,
        0xCB,0xB6,0x40,0x68,0x37,0xBF,0x51,0xF5
    };

    int cofactor = 1;
    int fieldSize = 256;

    printf(testingFmt, "wc_ecc_get_curve_id_from_params");

    #if !defined(NO_ECC256) && !defined(NO_ECC_SECP)
        id = wc_ecc_get_curve_id_from_params(fieldSize, prime, sizeof(prime),
                Af, sizeof(Af), Bf, sizeof(Bf), order, sizeof(order),
                Gx, sizeof(Gx), Gy, sizeof(Gy), cofactor);
        AssertIntEQ(id, ECC_SECP256R1);
    #endif

    /* invalid case, fieldSize = 0 */
    id = wc_ecc_get_curve_id_from_params(0, prime, sizeof(prime),
            Af, sizeof(Af), Bf, sizeof(Bf), order, sizeof(order),
            Gx, sizeof(Gx), Gy, sizeof(Gy), cofactor);
    AssertIntEQ(id, ECC_CURVE_INVALID);

    /* invalid case, NULL prime */
    id = wc_ecc_get_curve_id_from_params(fieldSize, NULL, sizeof(prime),
            Af, sizeof(Af), Bf, sizeof(Bf), order, sizeof(order),
            Gx, sizeof(Gx), Gy, sizeof(Gy), cofactor);
    AssertIntEQ(id, BAD_FUNC_ARG);

    /* invalid case, invalid prime */
    id = wc_ecc_get_curve_id_from_params(fieldSize,
            primeInvalid, sizeof(primeInvalid),
            Af, sizeof(Af), Bf, sizeof(Bf), order, sizeof(order),
            Gx, sizeof(Gx), Gy, sizeof(Gy), cofactor);
    AssertIntEQ(id, ECC_CURVE_INVALID);

    printf(resultFmt, passed);
#endif
}
static void test_wolfSSL_EVP_PKEY_encrypt(void)
{
#if defined(OPENSSL_EXTRA) && !defined(NO_RSA) && defined(WOLFSSL_KEY_GEN) && \
    !defined(HAVE_FAST_RSA)
    WOLFSSL_RSA* rsa = NULL;
    WOLFSSL_EVP_PKEY* pkey = NULL;
    WOLFSSL_EVP_PKEY_CTX* ctx = NULL;
    const char* in = "What is easy to do is easy not to do.";
    size_t inlen = XSTRLEN(in);
    size_t outEncLen = 0;
    byte*  outEnc = NULL;
    byte*  outDec = NULL;
    size_t outDecLen = 0;
    size_t rsaKeySz = 2048/8;  /* Bytes */
#ifdef WC_RSA_NO_PADDING
    byte*  inTmp = NULL;
    byte*  outEncTmp = NULL;
    byte*  outDecTmp = NULL;
#endif
    printf(testingFmt, "wolfSSL_EVP_PKEY_encrypt()");

    AssertNotNull(outEnc = (byte*)XMALLOC(rsaKeySz, HEAP_HINT, DYNAMIC_TYPE_TMP_BUFFER));
    XMEMSET(outEnc, 0, rsaKeySz);
    AssertNotNull(outDec = (byte*)XMALLOC(rsaKeySz, HEAP_HINT, DYNAMIC_TYPE_TMP_BUFFER));
    XMEMSET(outDec, 0, rsaKeySz);

    AssertNotNull(rsa = RSA_generate_key(2048, 3, NULL, NULL));
    AssertNotNull(pkey = wolfSSL_PKEY_new());
    AssertIntEQ(EVP_PKEY_assign_RSA(pkey, rsa), WOLFSSL_SUCCESS);
    AssertNotNull(ctx = EVP_PKEY_CTX_new(pkey, NULL));
    AssertIntEQ(EVP_PKEY_encrypt_init(ctx), WOLFSSL_SUCCESS);
    AssertIntEQ(EVP_PKEY_CTX_set_rsa_padding(ctx, RSA_PKCS1_PADDING),
                WOLFSSL_SUCCESS);

    /* Encrypt data */
    AssertIntEQ(EVP_PKEY_encrypt(ctx, outEnc, &outEncLen,
                            (const unsigned char*)in, inlen), WOLFSSL_SUCCESS);
    /* Decrypt data */
    AssertIntEQ(EVP_PKEY_decrypt_init(ctx), WOLFSSL_SUCCESS);

    AssertIntEQ(EVP_PKEY_decrypt(ctx, outDec, &outDecLen, outEnc, outEncLen),
                                 WOLFSSL_SUCCESS);

    AssertIntEQ(XMEMCMP(in, outDec, outDecLen), 0);

#ifdef WC_RSA_NO_PADDING
    /* The input length must be the same size as the RSA key.*/
    AssertNotNull(inTmp = (byte*)XMALLOC(rsaKeySz, HEAP_HINT, DYNAMIC_TYPE_TMP_BUFFER));
    XMEMSET(inTmp, 9, rsaKeySz);
    AssertNotNull(outEncTmp = (byte*)XMALLOC(rsaKeySz, HEAP_HINT, DYNAMIC_TYPE_TMP_BUFFER));
    XMEMSET(outEncTmp, 0, rsaKeySz);
    AssertNotNull(outDecTmp = (byte*)XMALLOC(rsaKeySz, HEAP_HINT, DYNAMIC_TYPE_TMP_BUFFER));
    XMEMSET(outDecTmp, 0, rsaKeySz);
    AssertIntEQ(EVP_PKEY_encrypt_init(ctx), WOLFSSL_SUCCESS);
    AssertIntEQ(EVP_PKEY_CTX_set_rsa_padding(ctx, RSA_NO_PADDING),
                                             WOLFSSL_SUCCESS);
    AssertIntEQ(EVP_PKEY_encrypt(ctx, outEncTmp, &outEncLen, inTmp, rsaKeySz),
                                 WOLFSSL_SUCCESS);
    AssertIntEQ(EVP_PKEY_decrypt_init(ctx), WOLFSSL_SUCCESS);
    AssertIntEQ(EVP_PKEY_decrypt(ctx, outDecTmp, &outDecLen, outEncTmp, outEncLen),
                                 WOLFSSL_SUCCESS);
    AssertIntEQ(XMEMCMP(inTmp, outDecTmp, outDecLen), 0);
#endif

    EVP_PKEY_free(pkey);
    EVP_PKEY_CTX_free(ctx);
    XFREE(outEnc, HEAP_HINT, DYNAMIC_TYPE_TMP_BUFFER);
    XFREE(outDec, HEAP_HINT, DYNAMIC_TYPE_TMP_BUFFER);
#ifdef WC_RSA_NO_PADDING
    XFREE(inTmp, HEAP_HINT, DYNAMIC_TYPE_TMP_BUFFER);
    XFREE(outEncTmp, HEAP_HINT, DYNAMIC_TYPE_TMP_BUFFER);
    XFREE(outDecTmp, HEAP_HINT, DYNAMIC_TYPE_TMP_BUFFER);
#endif
    printf(resultFmt, passed);
#endif
}
static void test_wolfSSL_EVP_PKEY_sign(void)
{
#if defined(OPENSSL_EXTRA) && !defined(NO_RSA) && defined(WOLFSSL_KEY_GEN) && \
    !defined(HAVE_FAST_RSA)
    WOLFSSL_RSA* rsa = NULL;
    WOLFSSL_EVP_PKEY* pkey = NULL;
    WOLFSSL_EVP_PKEY_CTX* ctx = NULL;
    const char* in = "What is easy to do is easy not to do.";
    size_t inlen = XSTRLEN(in);
    byte hash[SHA256_DIGEST_LENGTH] = {0};
    SHA256_CTX c;
    byte*  sig = NULL;
    byte*  sigVerify = NULL;
    size_t siglen = 0;
    size_t rsaKeySz = 2048/8;  /* Bytes */

    printf(testingFmt, "wolfSSL_EVP_PKEY_sign()");

    AssertNotNull(sig = (byte*)XMALLOC(rsaKeySz, HEAP_HINT, DYNAMIC_TYPE_TMP_BUFFER));
    XMEMSET(sig, 0, rsaKeySz);
    AssertNotNull(sigVerify = (byte*)XMALLOC(rsaKeySz, HEAP_HINT, DYNAMIC_TYPE_TMP_BUFFER));
    XMEMSET(sigVerify, 0, rsaKeySz);

    /* Generate hash */
    SHA256_Init(&c);
    SHA256_Update(&c, in, inlen);
    SHA256_Final(hash, &c);

    AssertNotNull(rsa = RSA_generate_key(2048, 3, NULL, NULL));
    AssertNotNull(pkey = wolfSSL_PKEY_new());
    AssertIntEQ(EVP_PKEY_assign_RSA(pkey, rsa), WOLFSSL_SUCCESS);
    AssertNotNull(ctx = EVP_PKEY_CTX_new(pkey, NULL));
    AssertIntEQ(EVP_PKEY_sign_init(ctx), WOLFSSL_SUCCESS);
#ifdef WC_RSA_PSS
    AssertIntEQ(EVP_PKEY_CTX_set_rsa_padding(ctx, RSA_PKCS1_PSS_PADDING),
                WOLFSSL_SUCCESS);
#else
    AssertIntEQ(EVP_PKEY_CTX_set_rsa_padding(ctx, RSA_PKCS1_PADDING),
                WOLFSSL_SUCCESS);
#endif

    /* Sign data */
    AssertIntEQ(EVP_PKEY_sign(ctx, sig, &siglen, hash, SHA256_DIGEST_LENGTH),
                              WOLFSSL_SUCCESS);
    /* Verify signature.
       EVP_PKEY_verify() doesn't exist yet, so use RSA_public_decrypt(). */
#ifdef WC_RSA_PSS
    AssertIntEQ(RSA_public_decrypt((int)siglen, sig, sigVerify,
                             rsa, RSA_PKCS1_PSS_PADDING), SHA256_DIGEST_LENGTH);
#else
    AssertIntEQ(RSA_public_decrypt((int)siglen, sig, sigVerify,
                             rsa, RSA_PKCS1_PADDING), SHA256_DIGEST_LENGTH);
#endif

    AssertIntEQ(XMEMCMP(hash, sigVerify, SHA256_DIGEST_LENGTH), 0);
    /* error cases */

    AssertIntNE(EVP_PKEY_sign_init(NULL), WOLFSSL_SUCCESS);
    ctx->pkey->type = EVP_PKEY_RSA2;
    AssertIntNE(EVP_PKEY_sign_init(ctx), WOLFSSL_SUCCESS);
    AssertIntNE(EVP_PKEY_sign(NULL, sig, &siglen, (byte*)in, inlen),
                              WOLFSSL_SUCCESS);
    AssertIntNE(EVP_PKEY_sign(ctx, sig, &siglen, (byte*)in, inlen),
                              WOLFSSL_SUCCESS);

    EVP_PKEY_free(pkey);
    EVP_PKEY_CTX_free(ctx);
    wolfSSL_RSA_free(rsa);
    XFREE(sig, HEAP_HINT, DYNAMIC_TYPE_TMP_BUFFER);
    XFREE(sigVerify, HEAP_HINT, DYNAMIC_TYPE_TMP_BUFFER);
    printf(resultFmt, passed);
#endif
}

static void test_EVP_PKEY_rsa(void)
{
#if defined(OPENSSL_EXTRA) && !defined(NO_RSA)
    WOLFSSL_RSA* rsa;
    WOLFSSL_EVP_PKEY* pkey;

    AssertNotNull(rsa = wolfSSL_RSA_new());
    AssertNotNull(pkey = wolfSSL_PKEY_new());
    AssertIntEQ(EVP_PKEY_assign_RSA(NULL, rsa), WOLFSSL_FAILURE);
    AssertIntEQ(EVP_PKEY_assign_RSA(pkey, NULL), WOLFSSL_FAILURE);
    AssertIntEQ(EVP_PKEY_assign_RSA(pkey, rsa), WOLFSSL_SUCCESS);
    AssertPtrEq(EVP_PKEY_get0_RSA(pkey), rsa);
    wolfSSL_EVP_PKEY_free(pkey);

    printf(resultFmt, passed);

#endif
}

static void test_EVP_PKEY_ec(void)
{
#if defined(OPENSSL_EXTRA) && defined(HAVE_ECC)
    WOLFSSL_EC_KEY* ecKey;
    WOLFSSL_EVP_PKEY* pkey;

    AssertNotNull(ecKey = wolfSSL_EC_KEY_new());
    AssertNotNull(pkey = wolfSSL_PKEY_new());
    AssertIntEQ(EVP_PKEY_assign_EC_KEY(NULL, ecKey), WOLFSSL_FAILURE);
    AssertIntEQ(EVP_PKEY_assign_EC_KEY(pkey, NULL), WOLFSSL_FAILURE);
    AssertIntEQ(EVP_PKEY_assign_EC_KEY(pkey, ecKey), WOLFSSL_SUCCESS);
    wolfSSL_EVP_PKEY_free(pkey);

    printf(resultFmt, passed);
#endif
}

static void test_EVP_PKEY_cmp(void)
{
#if defined(OPENSSL_EXTRA)
    EVP_PKEY *a, *b;
    const unsigned char *in;

#if !defined(NO_RSA) && defined(USE_CERT_BUFFERS_2048)
    in = client_key_der_2048;
    AssertNotNull(a = wolfSSL_d2i_PrivateKey(EVP_PKEY_RSA, NULL,
        &in, (long)sizeof_client_key_der_2048));
    in = client_key_der_2048;
    AssertNotNull(b = wolfSSL_d2i_PrivateKey(EVP_PKEY_RSA, NULL,
        &in, (long)sizeof_client_key_der_2048));

    /* Test success case RSA */
    AssertIntEQ(EVP_PKEY_cmp(a, b), 0);

    EVP_PKEY_free(b);
    EVP_PKEY_free(a);
#endif

#if defined(HAVE_ECC) && defined(USE_CERT_BUFFERS_256)
    in = ecc_clikey_der_256;
    AssertNotNull(a = wolfSSL_d2i_PrivateKey(EVP_PKEY_EC, NULL,
        &in, (long)sizeof_ecc_clikey_der_256));
    in = ecc_clikey_der_256;
    AssertNotNull(b = wolfSSL_d2i_PrivateKey(EVP_PKEY_EC, NULL,
        &in, (long)sizeof_ecc_clikey_der_256));

    /* Test success case ECC */
    AssertIntEQ(EVP_PKEY_cmp(a, b), 0);

    EVP_PKEY_free(b);
    EVP_PKEY_free(a);
#endif

    /* Test failure cases */
#if !defined(NO_RSA) && defined(USE_CERT_BUFFERS_2048) && \
     defined(HAVE_ECC) && defined(USE_CERT_BUFFERS_256)

    in = client_key_der_2048;
    AssertNotNull(a = wolfSSL_d2i_PrivateKey(EVP_PKEY_RSA, NULL,
        &in, (long)sizeof_client_key_der_2048));
    in = ecc_clikey_der_256;
    AssertNotNull(b = wolfSSL_d2i_PrivateKey(EVP_PKEY_EC, NULL,
        &in, (long)sizeof_ecc_clikey_der_256));

    AssertIntNE(EVP_PKEY_cmp(a, b), 0);

    EVP_PKEY_free(b);
    EVP_PKEY_free(a);
#endif

    /* invalid or empty failure cases */
    a = EVP_PKEY_new();
    b = EVP_PKEY_new();
    AssertIntNE(EVP_PKEY_cmp(NULL, NULL), 0);
    AssertIntNE(EVP_PKEY_cmp(a, NULL), 0);
    AssertIntNE(EVP_PKEY_cmp(NULL, b), 0);
    AssertIntNE(EVP_PKEY_cmp(a, b), 0);
    EVP_PKEY_free(b);
    EVP_PKEY_free(a);

    (void)in;

    printf(resultFmt, passed);
#endif
}

static void test_ERR_load_crypto_strings(void)
{
#if defined(OPENSSL_ALL)
    ERR_load_crypto_strings();

    printf(resultFmt, passed);
#endif
}

#if defined(OPENSSL_ALL) && !defined(NO_CERTS)
static void free_x509(X509* x)
{
    AssertIntEQ((x == (X509*)1 || x == (X509*)2), 1);
}
#endif

static void test_sk_X509(void)
{
#if defined(OPENSSL_ALL) && !defined(NO_CERTS)
    STACK_OF(X509)* s;

    AssertNotNull(s = sk_X509_new());
    AssertIntEQ(sk_X509_num(s), 0);
    sk_X509_free(s);

    AssertNotNull(s = sk_X509_new_null());
    AssertIntEQ(sk_X509_num(s), 0);
    sk_X509_free(s);

    AssertNotNull(s = sk_X509_new());
    sk_X509_push(s, (X509*)1);
    AssertIntEQ(sk_X509_num(s), 1);
    AssertIntEQ((sk_X509_value(s, 0) == (X509*)1), 1);
    sk_X509_push(s, (X509*)2);
    AssertIntEQ(sk_X509_num(s), 2);
    AssertIntEQ((sk_X509_value(s, 0) == (X509*)2), 1);
    AssertIntEQ((sk_X509_value(s, 1) == (X509*)1), 1);
    sk_X509_push(s, (X509*)2);
    sk_X509_pop_free(s, free_x509);

    printf(resultFmt, passed);
#endif
}

static void test_X509_get_signature_nid(void)
{
#if defined(OPENSSL_EXTRA) && !defined(NO_FILESYSTEM) && !defined(NO_RSA)
    X509*   x509;

    AssertIntEQ(X509_get_signature_nid(NULL), 0);
    AssertNotNull(x509 = wolfSSL_X509_load_certificate_file(svrCertFile,
                                                             SSL_FILETYPE_PEM));
    AssertIntEQ(X509_get_signature_nid(x509), CTC_SHA256wRSA);
    X509_free(x509);

    printf(resultFmt, passed);
#endif
}

static void test_X509_REQ(void)
{
#if defined(OPENSSL_ALL) && !defined(NO_CERTS) && defined(WOLFSSL_CERT_GEN) && \
                                                       defined(WOLFSSL_CERT_REQ)
    X509_NAME* name;
#if !defined(NO_RSA) || defined(HAVE_ECC)
    X509_REQ* req;
    EVP_PKEY* priv;
    EVP_PKEY* pub;
    unsigned char* der = NULL;
#endif
#ifndef NO_RSA
    #ifdef USE_CERT_BUFFERS_1024
    const unsigned char* rsaPriv = (const unsigned char*)client_key_der_1024;
    unsigned char* rsaPub = (unsigned char*)client_keypub_der_1024;
    #elif defined(USE_CERT_BUFFERS_2048)
    const unsigned char* rsaPriv = (const unsigned char*)client_key_der_2048;
    unsigned char* rsaPub = (unsigned char*)client_keypub_der_2048;
    #endif
#endif
#ifdef HAVE_ECC
    const unsigned char* ecPriv = (const unsigned char*)ecc_clikey_der_256;
    unsigned char* ecPub = (unsigned char*)ecc_clikeypub_der_256;
    int len;
#endif

    AssertNotNull(name = X509_NAME_new());
    AssertIntEQ(X509_NAME_add_entry_by_txt(name, "commonName", MBSTRING_UTF8,
                                           (byte*)"wolfssl.com", 11, 0, 1),
                WOLFSSL_SUCCESS);
    AssertIntEQ(X509_NAME_add_entry_by_txt(name, "emailAddress", MBSTRING_UTF8,
                                           (byte*)"support@wolfssl.com", 19, -1,
                                           1), WOLFSSL_SUCCESS);

#ifndef NO_RSA
    AssertNotNull(priv = wolfSSL_d2i_PrivateKey(EVP_PKEY_RSA, NULL, &rsaPriv,
                                             (long)sizeof_client_key_der_2048));
    AssertNotNull(pub = wolfSSL_d2i_PUBKEY(NULL, &rsaPub,
                                          (long)sizeof_client_keypub_der_2048));
    AssertNotNull(req = X509_REQ_new());
    AssertIntEQ(X509_REQ_set_subject_name(NULL, name), WOLFSSL_FAILURE);
    AssertIntEQ(X509_REQ_set_subject_name(req, NULL), WOLFSSL_FAILURE);
    AssertIntEQ(X509_REQ_set_subject_name(req, name), WOLFSSL_SUCCESS);
    AssertIntEQ(X509_REQ_set_pubkey(NULL, pub), WOLFSSL_FAILURE);
    AssertIntEQ(X509_REQ_set_pubkey(req, NULL), WOLFSSL_FAILURE);
    AssertIntEQ(X509_REQ_set_pubkey(req, pub), WOLFSSL_SUCCESS);
    AssertIntEQ(X509_REQ_sign(NULL, priv, EVP_sha256()), WOLFSSL_FAILURE);
    AssertIntEQ(X509_REQ_sign(req, NULL, EVP_sha256()), WOLFSSL_FAILURE);
    AssertIntEQ(X509_REQ_sign(req, priv, NULL), WOLFSSL_FAILURE);
    AssertIntEQ(X509_REQ_sign(req, priv, EVP_sha256()), WOLFSSL_SUCCESS);
    AssertIntEQ(i2d_X509_REQ(req, &der), 643);
    XFREE(der, NULL, DYNAMIC_TYPE_OPENSSL);
    der = NULL;
    X509_REQ_free(NULL);
    X509_REQ_free(req);
    EVP_PKEY_free(pub);
    EVP_PKEY_free(priv);
#endif
#ifdef HAVE_ECC
    AssertNotNull(priv = wolfSSL_d2i_PrivateKey(EVP_PKEY_EC, NULL, &ecPriv,
                                                    sizeof_ecc_clikey_der_256));
    AssertNotNull(pub = wolfSSL_d2i_PUBKEY(NULL, &ecPub,
                                                 sizeof_ecc_clikeypub_der_256));
    AssertNotNull(req = X509_REQ_new());
    AssertIntEQ(X509_REQ_set_subject_name(req, name), WOLFSSL_SUCCESS);
    AssertIntEQ(X509_REQ_set_pubkey(req, pub), WOLFSSL_SUCCESS);
    AssertIntEQ(X509_REQ_sign(req, priv, EVP_sha256()), WOLFSSL_SUCCESS);
    /* Signature is random and may be shorter or longer. */
    AssertIntGE((len = i2d_X509_REQ(req, &der)), 245);
    AssertIntLE(len, 253);
    XFREE(der, NULL, DYNAMIC_TYPE_OPENSSL);
    X509_REQ_free(req);
    EVP_PKEY_free(pub);
    EVP_PKEY_free(priv);

#ifdef FP_ECC
    wc_ecc_fp_free();
#endif
#endif /* HAVE_ECC */

    X509_NAME_free(name);

    printf(resultFmt, passed);
#endif
}

static void test_wolfssl_PKCS7(void)
{
#if defined(OPENSSL_ALL) && defined(HAVE_PKCS7)
    PKCS7* pkcs7;
    byte   data[FOURK_BUF];
    word32 len = sizeof(data);
    const byte*  p = data;
    byte   content[] = "Test data to encode.";

    AssertIntGT((len = CreatePKCS7SignedData(data, len, content,
                                             (word32)sizeof(content),
                                             0, 0)), 0);

    AssertNull(pkcs7 = d2i_PKCS7(NULL, NULL, len));
    AssertNull(pkcs7 = d2i_PKCS7(NULL, &p, 0));
    AssertNotNull(pkcs7 = d2i_PKCS7(NULL, &p, len));
    AssertIntEQ(wolfSSL_PKCS7_verify(NULL, NULL, NULL, NULL, NULL,
                                              PKCS7_NOVERIFY), WOLFSSL_FAILURE);
    PKCS7_free(pkcs7);

    /* fail case, without PKCS7_NOVERIFY */
    p = data;
    AssertNotNull(pkcs7 = d2i_PKCS7(NULL, &p, len));
    AssertIntEQ(wolfSSL_PKCS7_verify(pkcs7, NULL, NULL, NULL, NULL,
                                                           0), WOLFSSL_FAILURE);
    PKCS7_free(pkcs7);

    /* success case, with PKCS7_NOVERIFY */
    p = data;
    AssertNotNull(pkcs7 = d2i_PKCS7(NULL, &p, len));
    AssertIntEQ(wolfSSL_PKCS7_verify(pkcs7, NULL, NULL, NULL, NULL,
                                              PKCS7_NOVERIFY), WOLFSSL_SUCCESS);

    PKCS7_free(NULL);
    PKCS7_free(pkcs7);

    printf(resultFmt, passed);
#endif
}

static void test_wolfSSL_PKCS7_SIGNED_new(void)
{
#if defined(OPENSSL_ALL) && defined(HAVE_PKCS7)
    PKCS7_SIGNED* pkcs7;

    printf(testingFmt, "wolfSSL_PKCS7_SIGNED_new()");

    pkcs7 = PKCS7_SIGNED_new();
    AssertNotNull(pkcs7);
    AssertIntEQ(pkcs7->contentOID, SIGNED_DATA);

    PKCS7_SIGNED_free(pkcs7);
    printf(resultFmt, passed);
#endif
}

static void test_wolfSSL_PEM_write_bio_PKCS7(void)
{
#if defined(OPENSSL_ALL) && defined(HAVE_PKCS7) && !defined(NO_FILESYSTEM)
    PKCS7* pkcs7 = NULL;
    BIO* bio = NULL;
    const byte* cert_buf = NULL;
    int ret = 0;
    WC_RNG rng;
    const byte data[] = { /* Hello World */
        0x48,0x65,0x6c,0x6c,0x6f,0x20,0x57,0x6f,
        0x72,0x6c,0x64
    };
#ifndef NO_RSA
    #if defined(USE_CERT_BUFFERS_2048)
        byte        key[sizeof_client_key_der_2048];
        byte        cert[sizeof_client_cert_der_2048];
        word32      keySz = (word32)sizeof(key);
        word32      certSz = (word32)sizeof(cert);
        XMEMSET(key, 0, keySz);
        XMEMSET(cert, 0, certSz);
        XMEMCPY(key, client_key_der_2048, keySz);
        XMEMCPY(cert, client_cert_der_2048, certSz);
    #elif defined(USE_CERT_BUFFERS_1024)
        byte        key[sizeof_client_key_der_1024];
        byte        cert[sizeof_client_cert_der_1024];
        word32      keySz = (word32)sizeof(key);
        word32      certSz = (word32)sizeof(cert);
        XMEMSET(key, 0, keySz);
        XMEMSET(cert, 0, certSz);
        XMEMCPY(key, client_key_der_1024, keySz);
        XMEMCPY(cert, client_cert_der_1024, certSz);
    #else
        unsigned char   cert[ONEK_BUF];
        unsigned char   key[ONEK_BUF];
        XFILE           fp;
        int             certSz;
        int             keySz;

        fp = XFOPEN("./certs/1024/client-cert.der", "rb");
        AssertTrue((fp != XBADFILE));
        certSz = (int)XFREAD(cert, 1, sizeof_client_cert_der_1024, fp);
        XFCLOSE(fp);

        fp = XFOPEN("./certs/1024/client-key.der", "rb");
        AssertTrue(fp != XBADFILE);
        keySz = (int)XFREAD(key, 1, sizeof_client_key_der_1024, fp);
        XFCLOSE(fp);
    #endif
#elif defined(HAVE_ECC)
    #if defined(USE_CERT_BUFFERS_256)
        unsigned char    cert[sizeof_cliecc_cert_der_256];
        unsigned char    key[sizeof_ecc_clikey_der_256];
        int              certSz = (int)sizeof(cert);
        int              keySz = (int)sizeof(key);
        XMEMSET(cert, 0, certSz);
        XMEMSET(key, 0, keySz);
        XMEMCPY(cert, cliecc_cert_der_256, sizeof_cliecc_cert_der_256);
        XMEMCPY(key, ecc_clikey_der_256, sizeof_ecc_clikey_der_256);
    #else
        unsigned char   cert[ONEK_BUF];
        unsigned char   key[ONEK_BUF];
        XFILE           fp;
        int             certSz, keySz;

        fp = XFOPEN("./certs/client-ecc-cert.der", "rb");
        AssertTrue(fp != XBADFILE);
        certSz = (int)XFREAD(cert, 1, sizeof_cliecc_cert_der_256, fp);
        XFCLOSE(fp);

        fp = XFOPEN("./certs/client-ecc-key.der", "rb");
        AssertTrue(fp != XBADFILE);
        keySz = (int)XFREAD(key, 1, sizeof_ecc_clikey_der_256, fp);
        XFCLOSE(fp);
    #endif
#else
    #error PKCS7 requires ECC or RSA
#endif
    printf(testingFmt, "wolfSSL_PEM_write_bio_PKCS7()");

    AssertNotNull(pkcs7 = wc_PKCS7_New(HEAP_HINT, devId));
    /* initialize with DER encoded cert */
    AssertIntEQ(wc_PKCS7_InitWithCert(pkcs7, (byte*)cert, (word32)certSz), 0);

    /* init rng */
    AssertIntEQ(wc_InitRng(&rng), 0);

    pkcs7->rng = &rng;
    pkcs7->content   = (byte*)data; /* not used for ex */
    pkcs7->contentSz = (word32)sizeof(data);
    pkcs7->contentOID = SIGNED_DATA;
    pkcs7->privateKey = key;
    pkcs7->privateKeySz = (word32)sizeof(key);
    pkcs7->encryptOID = RSAk;
    pkcs7->hashOID = SHAh;
    pkcs7->signedAttribs   = NULL;
    pkcs7->signedAttribsSz = 0;

    AssertNotNull(bio = BIO_new(BIO_s_mem()));
    /* Write PKCS#7 PEM to BIO, the function converts the DER to PEM cert*/
    AssertIntEQ(PEM_write_bio_PKCS7(bio, pkcs7), WOLFSSL_SUCCESS);

    /* Read PKCS#7 PEM from BIO */
    ret = wolfSSL_BIO_get_mem_data(bio, &cert_buf);
    AssertIntGE(ret, 0);

    BIO_free(bio);
    wc_PKCS7_Free(pkcs7);
    wc_FreeRng(&rng);

    printf(resultFmt, passed);

#endif
}

/*----------------------------------------------------------------------------*
 | Certficate Failure Checks
 *----------------------------------------------------------------------------*/
#ifndef NO_CERTS
    /* Use the Cert Manager(CM) API to generate the error ASN_SIG_CONFIRM_E */
    static int verify_sig_cm(const char* ca, byte* cert_buf, size_t cert_sz,
        int type)
    {
        int ret;
        WOLFSSL_CERT_MANAGER* cm = NULL;

        switch (type) {
            case TESTING_RSA:
            #ifdef NO_RSA
                printf("RSA disabled, skipping test\n");
                return ASN_SIG_CONFIRM_E;
            #else
                break;
            #endif
            case TESTING_ECC:
            #ifndef HAVE_ECC
                printf("ECC disabled, skipping test\n");
                return ASN_SIG_CONFIRM_E;
            #else
                break;
            #endif
            default:
                printf("Bad function argument\n");
                return BAD_FUNC_ARG;
        }
        cm = wolfSSL_CertManagerNew();
        if (cm == NULL) {
            printf("wolfSSL_CertManagerNew failed\n");
            return -1;
        }

    #ifndef NO_FILESYSTEM
        ret = wolfSSL_CertManagerLoadCA(cm, ca, 0);
        if (ret != WOLFSSL_SUCCESS) {
            printf("wolfSSL_CertManagerLoadCA failed\n");
            wolfSSL_CertManagerFree(cm);
            return ret;
        }
    #else
        (void)ca;
    #endif

        ret = wolfSSL_CertManagerVerifyBuffer(cm, cert_buf, cert_sz, WOLFSSL_FILETYPE_ASN1);
        /* Let AssertIntEQ handle return code */

        wolfSSL_CertManagerFree(cm);

        return ret;
    }

    static int test_RsaSigFailure_cm(void)
    {
        int ret = 0;
        const char* ca_cert = "./certs/ca-cert.pem";
        const char* server_cert = "./certs/server-cert.der";
        byte* cert_buf = NULL;
        size_t cert_sz = 0;

        ret = load_file(server_cert, &cert_buf, &cert_sz);
        if (ret == 0) {
            /* corrupt DER - invert last byte, which is signature */
            cert_buf[cert_sz-1] = ~cert_buf[cert_sz-1];

            /* test bad cert */
            ret = verify_sig_cm(ca_cert, cert_buf, cert_sz, TESTING_RSA);
        }

        printf("Signature failure test: RSA: Ret %d\n", ret);

        if (cert_buf)
            free(cert_buf);

        return ret;
    }

    static int test_EccSigFailure_cm(void)
    {
        int ret = 0;
        /* self-signed ECC cert, so use server cert as CA */
        const char* ca_cert = "./certs/ca-ecc-cert.pem";
        const char* server_cert = "./certs/server-ecc.der";
        byte* cert_buf = NULL;
        size_t cert_sz = 0;

        ret = load_file(server_cert, &cert_buf, &cert_sz);
        if (ret == 0) {
            /* corrupt DER - invert last byte, which is signature */
            cert_buf[cert_sz-1] = ~cert_buf[cert_sz-1];

            /* test bad cert */
            ret = verify_sig_cm(ca_cert, cert_buf, cert_sz, TESTING_ECC);
        }

        printf("Signature failure test: ECC: Ret %d\n", ret);

        if (cert_buf)
            free(cert_buf);

#ifdef FP_ECC
    wc_ecc_fp_free();
#endif
        return ret;
    }

#endif /* NO_CERTS */

#ifdef WOLFSSL_TLS13
#if defined(WOLFSSL_SEND_HRR_COOKIE) && !defined(NO_WOLFSSL_SERVER)
static byte fixedKey[WC_SHA384_DIGEST_SIZE] = { 0, };
#endif
#ifdef WOLFSSL_EARLY_DATA
static const char earlyData[] = "Early Data";
static       char earlyDataBuffer[1];
#endif

static int test_tls13_apis(void)
{
    int          ret = 0;
#ifndef WOLFSSL_NO_TLS12
#ifndef NO_WOLFSSL_CLIENT
    WOLFSSL_CTX* clientTls12Ctx;
    WOLFSSL*     clientTls12Ssl;
#endif
#ifndef NO_WOLFSSL_SERVER
    WOLFSSL_CTX* serverTls12Ctx;
    WOLFSSL*     serverTls12Ssl;
#endif
#endif
#ifndef NO_WOLFSSL_CLIENT
    WOLFSSL_CTX* clientCtx;
    WOLFSSL*     clientSsl;
#endif
#ifndef NO_WOLFSSL_SERVER
    WOLFSSL_CTX* serverCtx;
    WOLFSSL*     serverSsl;
#ifndef NO_CERTS
    const char*  ourCert = svrCertFile;
    const char*  ourKey  = svrKeyFile;
#endif
#endif
#ifdef WOLFSSL_EARLY_DATA
    int          outSz;
#endif
    int          groups[1] = { WOLFSSL_ECC_X25519 };
    int          numGroups = 1;

#ifndef WOLFSSL_NO_TLS12
#ifndef NO_WOLFSSL_CLIENT
    clientTls12Ctx = wolfSSL_CTX_new(wolfTLSv1_2_client_method());
    clientTls12Ssl = wolfSSL_new(clientTls12Ctx);
#endif
#ifndef NO_WOLFSSL_SERVER
    serverTls12Ctx = wolfSSL_CTX_new(wolfTLSv1_2_server_method());
#ifndef NO_CERTS
    wolfSSL_CTX_use_certificate_chain_file(serverTls12Ctx, ourCert);
    wolfSSL_CTX_use_PrivateKey_file(serverTls12Ctx, ourKey, WOLFSSL_FILETYPE_PEM);
#endif
    serverTls12Ssl = wolfSSL_new(serverTls12Ctx);
#endif
#endif

#ifndef NO_WOLFSSL_CLIENT
    clientCtx = wolfSSL_CTX_new(wolfTLSv1_3_client_method());
    clientSsl = wolfSSL_new(clientCtx);
#endif
#ifndef NO_WOLFSSL_SERVER
    serverCtx = wolfSSL_CTX_new(wolfTLSv1_3_server_method());
#ifndef NO_CERTS
    wolfSSL_CTX_use_certificate_chain_file(serverCtx, ourCert);
    wolfSSL_CTX_use_PrivateKey_file(serverCtx, ourKey, WOLFSSL_FILETYPE_PEM);
#endif
    serverSsl = wolfSSL_new(serverCtx);
#endif

#ifdef WOLFSSL_SEND_HRR_COOKIE
    AssertIntEQ(wolfSSL_send_hrr_cookie(NULL, NULL, 0), BAD_FUNC_ARG);
#ifndef NO_WOLFSSL_CLIENT
    AssertIntEQ(wolfSSL_send_hrr_cookie(clientSsl, NULL, 0), SIDE_ERROR);
#endif
#ifndef NO_WOLFSSL_SERVER
#ifndef WOLFSSL_NO_TLS12
    AssertIntEQ(wolfSSL_send_hrr_cookie(serverTls12Ssl, NULL, 0), BAD_FUNC_ARG);
#endif

    AssertIntEQ(wolfSSL_send_hrr_cookie(serverSsl, NULL, 0), WOLFSSL_SUCCESS);
    AssertIntEQ(wolfSSL_send_hrr_cookie(serverSsl, fixedKey, sizeof(fixedKey)),
                WOLFSSL_SUCCESS);
#endif
#endif

#ifdef HAVE_ECC
    AssertIntEQ(wolfSSL_UseKeyShare(NULL, WOLFSSL_ECC_SECP256R1), BAD_FUNC_ARG);
#ifndef NO_WOLFSSL_SERVER
    AssertIntEQ(wolfSSL_UseKeyShare(serverSsl, WOLFSSL_ECC_SECP256R1),
                WOLFSSL_SUCCESS);
#endif
#ifndef NO_WOLFSSL_CLIENT
#ifndef WOLFSSL_NO_TLS12
    AssertIntEQ(wolfSSL_UseKeyShare(clientTls12Ssl, WOLFSSL_ECC_SECP256R1),
                WOLFSSL_SUCCESS);
#endif
    AssertIntEQ(wolfSSL_UseKeyShare(clientSsl, WOLFSSL_ECC_SECP256R1),
                WOLFSSL_SUCCESS);
#endif
#elif defined(HAVE_CURVE25519)
    AssertIntEQ(wolfSSL_UseKeyShare(NULL, WOLFSSL_ECC_X25519), BAD_FUNC_ARG);
#ifndef NO_WOLFSSL_SERVER
    AssertIntEQ(wolfSSL_UseKeyShare(serverSsl, WOLFSSL_ECC_X25519),
                WOLFSSL_SUCCESS);
#endif
#ifndef NO_WOLFSSL_CLIENT
#ifndef WOLFSSL_NO_TLS12
    AssertIntEQ(wolfSSL_UseKeyShare(clientTls12Ssl, WOLFSSL_ECC_X25519),
                WOLFSSL_SUCCESS);
#endif
    AssertIntEQ(wolfSSL_UseKeyShare(clientSsl, WOLFSSL_ECC_X25519),
                WOLFSSL_SUCCESS);
#endif
#else
    AssertIntEQ(wolfSSL_UseKeyShare(NULL, WOLFSSL_ECC_SECP256R1), BAD_FUNC_ARG);
#ifndef NO_WOLFSSL_CLIENT
#ifndef WOLFSSL_NO_TLS12
    AssertIntEQ(wolfSSL_UseKeyShare(clientTls12Ssl, WOLFSSL_ECC_SECP256R1),
                NOT_COMPILED_IN);
#endif
    AssertIntEQ(wolfSSL_UseKeyShare(clientSsl, WOLFSSL_ECC_SECP256R1),
                NOT_COMPILED_IN);
#endif
#endif

    AssertIntEQ(wolfSSL_NoKeyShares(NULL), BAD_FUNC_ARG);
#ifndef NO_WOLFSSL_SERVER
    AssertIntEQ(wolfSSL_NoKeyShares(serverSsl), SIDE_ERROR);
#endif
#ifndef NO_WOLFSSL_CLIENT
#ifndef WOLFSSL_NO_TLS12
    AssertIntEQ(wolfSSL_NoKeyShares(clientTls12Ssl), WOLFSSL_SUCCESS);
#endif
    AssertIntEQ(wolfSSL_NoKeyShares(clientSsl), WOLFSSL_SUCCESS);
#endif

    AssertIntEQ(wolfSSL_CTX_no_ticket_TLSv13(NULL), BAD_FUNC_ARG);
#ifndef NO_WOLFSSL_CLIENT
    AssertIntEQ(wolfSSL_CTX_no_ticket_TLSv13(clientCtx), SIDE_ERROR);
#endif
#ifndef NO_WOLFSSL_SERVER
#ifndef WOLFSSL_NO_TLS12
    AssertIntEQ(wolfSSL_CTX_no_ticket_TLSv13(serverTls12Ctx), BAD_FUNC_ARG);
#endif
    AssertIntEQ(wolfSSL_CTX_no_ticket_TLSv13(serverCtx), 0);
#endif

    AssertIntEQ(wolfSSL_no_ticket_TLSv13(NULL), BAD_FUNC_ARG);
#ifndef NO_WOLFSSL_CLIENT
    AssertIntEQ(wolfSSL_no_ticket_TLSv13(clientSsl), SIDE_ERROR);
#endif
#ifndef NO_WOLFSSL_SERVER
#ifndef WOLFSSL_NO_TLS12
    AssertIntEQ(wolfSSL_no_ticket_TLSv13(serverTls12Ssl), BAD_FUNC_ARG);
#endif
    AssertIntEQ(wolfSSL_no_ticket_TLSv13(serverSsl), 0);
#endif

    AssertIntEQ(wolfSSL_CTX_no_dhe_psk(NULL), BAD_FUNC_ARG);
#ifndef NO_WOLFSSL_CLIENT
#ifndef WOLFSSL_NO_TLS12
    AssertIntEQ(wolfSSL_CTX_no_dhe_psk(clientTls12Ctx), BAD_FUNC_ARG);
#endif
    AssertIntEQ(wolfSSL_CTX_no_dhe_psk(clientCtx), 0);
#endif
#ifndef NO_WOLFSSL_SERVER
    AssertIntEQ(wolfSSL_CTX_no_dhe_psk(serverCtx), 0);
#endif

    AssertIntEQ(wolfSSL_no_dhe_psk(NULL), BAD_FUNC_ARG);
#ifndef NO_WOLFSSL_CLIENT
#ifndef WOLFSSL_NO_TLS12
    AssertIntEQ(wolfSSL_no_dhe_psk(clientTls12Ssl), BAD_FUNC_ARG);
#endif
    AssertIntEQ(wolfSSL_no_dhe_psk(clientSsl), 0);
#endif
#ifndef NO_WOLFSSL_SERVER
    AssertIntEQ(wolfSSL_no_dhe_psk(serverSsl), 0);
#endif

    AssertIntEQ(wolfSSL_update_keys(NULL), BAD_FUNC_ARG);
#ifndef NO_WOLFSSL_CLIENT
#ifndef WOLFSSL_NO_TLS12
    AssertIntEQ(wolfSSL_update_keys(clientTls12Ssl), BAD_FUNC_ARG);
#endif
    AssertIntEQ(wolfSSL_update_keys(clientSsl), BUILD_MSG_ERROR);
#endif
#ifndef NO_WOLFSSL_SERVER
    AssertIntEQ(wolfSSL_update_keys(serverSsl), BUILD_MSG_ERROR);
#endif

#if !defined(NO_CERTS) && defined(WOLFSSL_POST_HANDSHAKE_AUTH)
    AssertIntEQ(wolfSSL_CTX_allow_post_handshake_auth(NULL), BAD_FUNC_ARG);
#ifndef NO_WOLFSSL_SERVER
    AssertIntEQ(wolfSSL_CTX_allow_post_handshake_auth(serverCtx), SIDE_ERROR);
#endif
#ifndef NO_WOLFSSL_CLIENT
#ifndef WOLFSSL_NO_TLS12
    AssertIntEQ(wolfSSL_CTX_allow_post_handshake_auth(clientTls12Ctx),
                BAD_FUNC_ARG);
#endif
    AssertIntEQ(wolfSSL_CTX_allow_post_handshake_auth(clientCtx), 0);
#endif

    AssertIntEQ(wolfSSL_allow_post_handshake_auth(NULL), BAD_FUNC_ARG);
#ifndef NO_WOLFSSL_SERVER
    AssertIntEQ(wolfSSL_allow_post_handshake_auth(serverSsl), SIDE_ERROR);
#endif
#ifndef NO_WOLFSSL_CLIENT
#ifndef WOLFSSL_NO_TLS12
    AssertIntEQ(wolfSSL_allow_post_handshake_auth(clientTls12Ssl),
                BAD_FUNC_ARG);
#endif
    AssertIntEQ(wolfSSL_allow_post_handshake_auth(clientSsl), 0);
#endif

    AssertIntEQ(wolfSSL_request_certificate(NULL), BAD_FUNC_ARG);
#ifndef NO_WOLFSSL_CLIENT
    AssertIntEQ(wolfSSL_request_certificate(clientSsl), SIDE_ERROR);
#endif
#ifndef NO_WOLFSSL_SERVER
#ifndef WOLFSSL_NO_TLS12
    AssertIntEQ(wolfSSL_request_certificate(serverTls12Ssl),
                BAD_FUNC_ARG);
#endif
    AssertIntEQ(wolfSSL_request_certificate(serverSsl), NOT_READY_ERROR);
#endif
#endif

#ifndef WOLFSSL_NO_SERVER_GROUPS_EXT
    AssertIntEQ(wolfSSL_preferred_group(NULL), BAD_FUNC_ARG);
#ifndef NO_WOLFSSL_SERVER
    AssertIntEQ(wolfSSL_preferred_group(serverSsl), SIDE_ERROR);
#endif
#ifndef NO_WOLFSSL_CLIENT
#ifndef WOLFSSL_NO_TLS12
    AssertIntEQ(wolfSSL_preferred_group(clientTls12Ssl), BAD_FUNC_ARG);
#endif
    AssertIntEQ(wolfSSL_preferred_group(clientSsl), NOT_READY_ERROR);
#endif
#endif

    AssertIntEQ(wolfSSL_CTX_set_groups(NULL, NULL, 0), BAD_FUNC_ARG);
#ifndef NO_WOLFSSL_CLIENT
    AssertIntEQ(wolfSSL_CTX_set_groups(clientCtx, NULL, 0), BAD_FUNC_ARG);
#endif
    AssertIntEQ(wolfSSL_CTX_set_groups(NULL, groups, numGroups), BAD_FUNC_ARG);
#ifndef NO_WOLFSSL_CLIENT
#ifndef WOLFSSL_NO_TLS12
    AssertIntEQ(wolfSSL_CTX_set_groups(clientTls12Ctx, groups, numGroups),
                BAD_FUNC_ARG);
#endif
    AssertIntEQ(wolfSSL_CTX_set_groups(clientCtx, groups,
                                       WOLFSSL_MAX_GROUP_COUNT + 1),
                BAD_FUNC_ARG);
    AssertIntEQ(wolfSSL_CTX_set_groups(clientCtx, groups, numGroups),
                WOLFSSL_SUCCESS);
#endif
#ifndef NO_WOLFSSL_SERVER
    AssertIntEQ(wolfSSL_CTX_set_groups(serverCtx, groups, numGroups),
                WOLFSSL_SUCCESS);
#endif

    AssertIntEQ(wolfSSL_set_groups(NULL, NULL, 0), BAD_FUNC_ARG);
#ifndef NO_WOLFSSL_CLIENT
    AssertIntEQ(wolfSSL_set_groups(clientSsl, NULL, 0), BAD_FUNC_ARG);
#endif
    AssertIntEQ(wolfSSL_set_groups(NULL, groups, numGroups), BAD_FUNC_ARG);
#ifndef NO_WOLFSSL_CLIENT
#ifndef WOLFSSL_NO_TLS12
    AssertIntEQ(wolfSSL_set_groups(clientTls12Ssl, groups, numGroups),
                BAD_FUNC_ARG);
#endif
    AssertIntEQ(wolfSSL_set_groups(clientSsl, groups,
                                   WOLFSSL_MAX_GROUP_COUNT + 1), BAD_FUNC_ARG);
    AssertIntEQ(wolfSSL_set_groups(clientSsl, groups, numGroups),
                WOLFSSL_SUCCESS);
#endif
#ifndef NO_WOLFSSL_SERVER
    AssertIntEQ(wolfSSL_set_groups(serverSsl, groups, numGroups),
                WOLFSSL_SUCCESS);
#endif

#ifdef WOLFSSL_EARLY_DATA
    AssertIntEQ(wolfSSL_CTX_set_max_early_data(NULL, 0), BAD_FUNC_ARG);
#ifndef NO_WOLFSSL_CLIENT
    AssertIntEQ(wolfSSL_CTX_set_max_early_data(clientCtx, 0), SIDE_ERROR);
#endif
#ifndef NO_WOLFSSL_SERVER
#ifndef WOLFSSL_NO_TLS12
    AssertIntEQ(wolfSSL_CTX_set_max_early_data(serverTls12Ctx, 0),
                BAD_FUNC_ARG);
#endif
    AssertIntEQ(wolfSSL_CTX_set_max_early_data(serverCtx, 0), 0);
#endif

    AssertIntEQ(wolfSSL_set_max_early_data(NULL, 0), BAD_FUNC_ARG);
#ifndef NO_WOLFSSL_CLIENT
    AssertIntEQ(wolfSSL_set_max_early_data(clientSsl, 0), SIDE_ERROR);
#endif
#ifndef NO_WOLFSSL_SERVER
#ifndef WOLFSSL_NO_TLS12
    AssertIntEQ(wolfSSL_set_max_early_data(serverTls12Ssl, 0), BAD_FUNC_ARG);
#endif
    AssertIntEQ(wolfSSL_set_max_early_data(serverSsl, 0), 0);
#endif

    AssertIntEQ(wolfSSL_write_early_data(NULL, earlyData, sizeof(earlyData),
                                         &outSz), BAD_FUNC_ARG);
#ifndef NO_WOLFSSL_CLIENT
    AssertIntEQ(wolfSSL_write_early_data(clientSsl, NULL, sizeof(earlyData),
                                         &outSz), BAD_FUNC_ARG);
    AssertIntEQ(wolfSSL_write_early_data(clientSsl, earlyData, -1, &outSz),
                BAD_FUNC_ARG);
    AssertIntEQ(wolfSSL_write_early_data(clientSsl, earlyData,
                                         sizeof(earlyData), NULL),
                BAD_FUNC_ARG);
#endif
#ifndef NO_WOLFSSL_SERVER
    AssertIntEQ(wolfSSL_write_early_data(serverSsl, earlyData,
                                         sizeof(earlyData), &outSz),
                SIDE_ERROR);
#endif
#ifndef NO_WOLFSSL_CLIENT
#ifndef WOLFSSL_NO_TLS12
    AssertIntEQ(wolfSSL_write_early_data(clientTls12Ssl, earlyData,
                                         sizeof(earlyData), &outSz),
                BAD_FUNC_ARG);
#endif
    AssertIntEQ(wolfSSL_write_early_data(clientSsl, earlyData,
                                         sizeof(earlyData), &outSz),
                WOLFSSL_FATAL_ERROR);
#endif

    AssertIntEQ(wolfSSL_read_early_data(NULL, earlyDataBuffer,
                                        sizeof(earlyDataBuffer), &outSz),
                BAD_FUNC_ARG);
#ifndef NO_WOLFSSL_SERVER
    AssertIntEQ(wolfSSL_read_early_data(serverSsl, NULL,
                                        sizeof(earlyDataBuffer), &outSz),
                BAD_FUNC_ARG);
    AssertIntEQ(wolfSSL_read_early_data(serverSsl, earlyDataBuffer, -1, &outSz),
                BAD_FUNC_ARG);
    AssertIntEQ(wolfSSL_read_early_data(serverSsl, earlyDataBuffer,
                                        sizeof(earlyDataBuffer), NULL),
                BAD_FUNC_ARG);
#endif
#ifndef NO_WOLFSSL_CLIENT
    AssertIntEQ(wolfSSL_read_early_data(clientSsl, earlyDataBuffer,
                                        sizeof(earlyDataBuffer), &outSz),
                SIDE_ERROR);
#endif
#ifndef NO_WOLFSSL_SERVER
#ifndef WOLFSSL_NO_TLS12
    AssertIntEQ(wolfSSL_read_early_data(serverTls12Ssl, earlyDataBuffer,
                                        sizeof(earlyDataBuffer), &outSz),
                BAD_FUNC_ARG);
#endif
    AssertIntEQ(wolfSSL_read_early_data(serverSsl, earlyDataBuffer,
                                        sizeof(earlyDataBuffer), &outSz),
                WOLFSSL_FATAL_ERROR);
#endif
#endif

#ifndef NO_WOLFSSL_SERVER
    wolfSSL_free(serverSsl);
    wolfSSL_CTX_free(serverCtx);
#endif
#ifndef NO_WOLFSSL_CLIENT
    wolfSSL_free(clientSsl);
    wolfSSL_CTX_free(clientCtx);
#endif

#ifndef WOLFSSL_NO_TLS12
#ifndef NO_WOLFSSL_SERVER
    wolfSSL_free(serverTls12Ssl);
    wolfSSL_CTX_free(serverTls12Ctx);
#endif
#ifndef NO_WOLFSSL_CLIENT
    wolfSSL_free(clientTls12Ssl);
    wolfSSL_CTX_free(clientTls12Ctx);
#endif
#endif

    return ret;
}

#endif

#ifdef HAVE_PK_CALLBACKS
#if !defined(NO_FILESYSTEM) && !defined(NO_CERTS) && !defined(NO_RSA) && \
        !defined(NO_WOLFSSL_CLIENT) && !defined(NO_DH) && !defined(NO_AES) && \
         defined(HAVE_IO_TESTS_DEPENDENCIES) && !defined(SINGLE_THREADED)
static int my_DhCallback(WOLFSSL* ssl, struct DhKey* key,
        const unsigned char* priv, unsigned int privSz,
        const unsigned char* pubKeyDer, unsigned int pubKeySz,
        unsigned char* out, unsigned int* outlen,
        void* ctx)
{
    /* Test fail when context associated with WOLFSSL is NULL */
    if (ctx == NULL) {
        return -1;
    }

    (void)ssl;
    /* return 0 on success */
    return wc_DhAgree(key, out, outlen, priv, privSz, pubKeyDer, pubKeySz);
};

static void test_dh_ctx_setup(WOLFSSL_CTX* ctx) {
    wolfSSL_CTX_SetDhAgreeCb(ctx, my_DhCallback);
#ifdef WOLFSSL_AES_128
    AssertIntEQ(wolfSSL_CTX_set_cipher_list(ctx, "DHE-RSA-AES128-SHA256"),
            WOLFSSL_SUCCESS);
#endif
#ifdef WOLFSSL_AES_256
    AssertIntEQ(wolfSSL_CTX_set_cipher_list(ctx, "DHE-RSA-AES256-SHA256"),
            WOLFSSL_SUCCESS);
#endif
}

static void test_dh_ssl_setup(WOLFSSL* ssl)
{
    static int dh_test_ctx = 1;
    int ret;

    wolfSSL_SetDhAgreeCtx(ssl, &dh_test_ctx);
    AssertIntEQ(*((int*)wolfSSL_GetDhAgreeCtx(ssl)), dh_test_ctx);
    ret = wolfSSL_SetTmpDH_file(ssl, dhParamFile, WOLFSSL_FILETYPE_PEM);
    if (ret != WOLFSSL_SUCCESS && ret != SIDE_ERROR) {
        AssertIntEQ(ret, WOLFSSL_SUCCESS);
    }
}

static void test_dh_ssl_setup_fail(WOLFSSL* ssl)
{
    int ret;

    wolfSSL_SetDhAgreeCtx(ssl, NULL);
    AssertNull(wolfSSL_GetDhAgreeCtx(ssl));
    ret = wolfSSL_SetTmpDH_file(ssl, dhParamFile, WOLFSSL_FILETYPE_PEM);
    if (ret != WOLFSSL_SUCCESS && ret != SIDE_ERROR) {
        AssertIntEQ(ret, WOLFSSL_SUCCESS);
    }
}
#endif

static void test_DhCallbacks(void)
{
#if !defined(NO_FILESYSTEM) && !defined(NO_CERTS) && !defined(NO_RSA) && \
        !defined(NO_WOLFSSL_CLIENT) && !defined(NO_DH) && !defined(NO_AES) && \
         defined(HAVE_IO_TESTS_DEPENDENCIES) && !defined(SINGLE_THREADED)
    WOLFSSL_CTX *ctx;
    WOLFSSL     *ssl;
    tcp_ready   ready;
    func_args   server_args;
    func_args   client_args;
    THREAD_TYPE serverThread;
    callback_functions func_cb_client;
    callback_functions func_cb_server;
    int  test;

    printf(testingFmt, "test_DhCallbacks");

#ifndef NO_WOLFSSL_CLIENT
    AssertNotNull(ctx = wolfSSL_CTX_new(wolfSSLv23_client_method()));
#else
    AssertNotNull(ctx = wolfSSL_CTX_new(wolfSSLv23_server_method()));
#endif

    AssertIntEQ(wolfSSL_CTX_set_cipher_list(NULL, "NONE"), WOLFSSL_FAILURE);

    wolfSSL_CTX_SetDhAgreeCb(ctx, &my_DhCallback);

    /* load client ca cert */
    AssertIntEQ(wolfSSL_CTX_load_verify_locations(ctx, caCertFile, 0),
            WOLFSSL_SUCCESS);

    /* test with NULL arguments */
    wolfSSL_SetDhAgreeCtx(NULL, &test);
    AssertNull(wolfSSL_GetDhAgreeCtx(NULL));

    /* test success case */
    test = 1;
    AssertNotNull(ssl = wolfSSL_new(ctx));
    wolfSSL_SetDhAgreeCtx(ssl, &test);
    AssertIntEQ(*((int*)wolfSSL_GetDhAgreeCtx(ssl)), test);

    wolfSSL_free(ssl);
    wolfSSL_CTX_free(ctx);

    /* test a connection where callback is used */
#ifdef WOLFSSL_TIRTOS
    fdOpenSession(Task_self());
#endif
    XMEMSET(&server_args, 0, sizeof(func_args));
    XMEMSET(&client_args, 0, sizeof(func_args));
    XMEMSET(&func_cb_client, 0, sizeof(callback_functions));
    XMEMSET(&func_cb_server, 0, sizeof(callback_functions));

    StartTCP();
    InitTcpReady(&ready);

#if defined(USE_WINDOWS_API)
    /* use RNG to get random port if using windows */
    ready.port = GetRandomPort();
#endif

    server_args.signal = &ready;
    client_args.signal = &ready;
    server_args.return_code = TEST_FAIL;
    client_args.return_code = TEST_FAIL;

    /* set callbacks to use DH functions */
    func_cb_client.ctx_ready = &test_dh_ctx_setup;
    func_cb_client.ssl_ready = &test_dh_ssl_setup;
#ifndef WOLFSSL_NO_TLS12
    func_cb_client.method = wolfTLSv1_2_client_method;
#else
    func_cb_client.method = wolfTLSv1_3_client_method;
#endif
    client_args.callbacks = &func_cb_client;

    func_cb_server.ctx_ready = &test_dh_ctx_setup;
    func_cb_server.ssl_ready = &test_dh_ssl_setup;
#ifndef WOLFSSL_NO_TLS12
    func_cb_server.method = wolfTLSv1_2_server_method;
#else
    func_cb_server.method = wolfTLSv1_3_server_method;
#endif
    server_args.callbacks = &func_cb_server;

    start_thread(test_server_nofail, &server_args, &serverThread);
    wait_tcp_ready(&server_args);
    test_client_nofail(&client_args, NULL);
    join_thread(serverThread);

    AssertTrue(client_args.return_code);
    AssertTrue(server_args.return_code);

    FreeTcpReady(&ready);

#ifdef WOLFSSL_TIRTOS
    fdOpenSession(Task_self());
#endif

    /* now set user ctx to not be 1 so that the callback returns fail case */
#ifdef WOLFSSL_TIRTOS
    fdOpenSession(Task_self());
#endif
    XMEMSET(&server_args, 0, sizeof(func_args));
    XMEMSET(&client_args, 0, sizeof(func_args));
    XMEMSET(&func_cb_client, 0, sizeof(callback_functions));
    XMEMSET(&func_cb_server, 0, sizeof(callback_functions));

    StartTCP();
    InitTcpReady(&ready);

#if defined(USE_WINDOWS_API)
    /* use RNG to get random port if using windows */
    ready.port = GetRandomPort();
#endif

    server_args.signal = &ready;
    client_args.signal = &ready;
    server_args.return_code = TEST_FAIL;
    client_args.return_code = TEST_FAIL;

    /* set callbacks to use DH functions */
    func_cb_client.ctx_ready = &test_dh_ctx_setup;
    func_cb_client.ssl_ready = &test_dh_ssl_setup_fail;
#ifndef WOLFSSL_NO_TLS12
    func_cb_client.method = wolfTLSv1_2_client_method;
#else
    func_cb_client.method = wolfTLSv1_3_client_method;
#endif
    client_args.callbacks = &func_cb_client;

    func_cb_server.ctx_ready = &test_dh_ctx_setup;
    func_cb_server.ssl_ready = &test_dh_ssl_setup_fail;
#ifndef WOLFSSL_NO_TLS12
    func_cb_server.method = wolfTLSv1_2_server_method;
#else
    func_cb_server.method = wolfTLSv1_3_server_method;
#endif
    server_args.callbacks = &func_cb_server;

    start_thread(test_server_nofail, &server_args, &serverThread);
    wait_tcp_ready(&server_args);
    test_client_nofail(&client_args, NULL);
    join_thread(serverThread);

    AssertIntEQ(client_args.return_code, TEST_FAIL);
    AssertIntEQ(server_args.return_code, TEST_FAIL);

    FreeTcpReady(&ready);

#ifdef WOLFSSL_TIRTOS
    fdOpenSession(Task_self());
#endif

    printf(resultFmt, passed);
#endif
}
#endif /* HAVE_PK_CALLBACKS */

#ifdef HAVE_HASHDRBG

#ifdef TEST_RESEED_INTERVAL
static int test_wc_RNG_GenerateBlock_Reseed(void)
{
    int i, ret;
    WC_RNG rng;
    byte key[32];

    ret = wc_InitRng(&rng);

    if (ret == 0) {
        for(i = 0; i < WC_RESEED_INTERVAL + 10; i++) {
            ret = wc_RNG_GenerateBlock(&rng, key, sizeof(key));
            if (ret != 0) {
                break;
            }
        }
    }

    wc_FreeRng(&rng);

    return ret;
}
#endif /* TEST_RESEED_INTERVAL */

static int test_wc_RNG_GenerateBlock(void)
{
    int i, ret;
    WC_RNG rng;
    byte key[32];

    ret = wc_InitRng(&rng);

    if (ret == 0) {
        for(i = 0; i < 10; i++) {
            ret = wc_RNG_GenerateBlock(&rng, key, sizeof(key));
            if (ret != 0) {
                break;
            }
        }
    }

    wc_FreeRng(&rng);

    (void)rng; /* for WC_NO_RNG case */
    (void)key;

    return ret;
}
#endif

static void test_wolfSSL_X509_CRL(void)
{
#if defined(OPENSSL_EXTRA) && defined(HAVE_CRL)

    X509_CRL *crl;
    char pem[][100] = {
        "./certs/crl/crl.pem",
        "./certs/crl/crl2.pem",
        "./certs/crl/caEccCrl.pem",
        "./certs/crl/eccCliCRL.pem",
        "./certs/crl/eccSrvCRL.pem",
        ""
    };
    BIO *bio;

#ifdef HAVE_TEST_d2i_X509_CRL_fp
    char der[][100] = {
        "./certs/crl/crl.der",
        "./certs/crl/crl2.der",
        ""};
#endif

    XFILE fp;
    int i;

    printf(testingFmt, "test_wolfSSL_X509_CRL");

    for (i = 0; pem[i][0] != '\0'; i++)
    {
        fp = XFOPEN(pem[i], "rb");
        AssertTrue((fp != XBADFILE));
        AssertNotNull(crl = (X509_CRL *)PEM_read_X509_CRL(fp, (X509_CRL **)NULL, NULL, NULL));
        AssertNotNull(crl);
        X509_CRL_free(crl);
        XFCLOSE(fp);
        fp = XFOPEN(pem[i], "rb");
        AssertTrue((fp != XBADFILE));
        AssertNotNull((X509_CRL *)PEM_read_X509_CRL(fp, (X509_CRL **)&crl, NULL, NULL));
        AssertNotNull(crl);
        X509_CRL_free(crl);
        XFCLOSE(fp);
    }

    for (i = 0; pem[i][0] != '\0'; i++)
    {
        AssertNotNull(bio = BIO_new_file(pem[i], "r"));
        AssertNotNull(crl = PEM_read_bio_X509_CRL(bio, NULL, NULL, NULL));
        X509_CRL_free(crl);
        BIO_free(bio);
    }

#ifdef HAVE_TEST_d2i_X509_CRL_fp
    for(i = 0; der[i][0] != '\0'; i++){
        fp = XFOPEN(der[i], "rb");
        AssertTrue((fp != XBADFILE));
        AssertNotNull(crl = (X509_CRL *)d2i_X509_CRL_fp((fp, X509_CRL **)NULL));
        AssertNotNull(crl);
        X509_CRL_free(crl);
        XFCLOSE(fp);
        fp = XFOPEN(der[i], "rb");
        AssertTrue((fp != XBADFILE));
        AssertNotNull((X509_CRL *)d2i_X509_CRL_fp(fp, (X509_CRL **)&crl));
        AssertNotNull(crl);
        X509_CRL_free(crl);
        XFCLOSE(fp);
    }
#endif

    printf(resultFmt, passed);
#endif
        return;
}

static void test_wolfSSL_PEM_read_X509(void)
{
#if defined(OPENSSL_EXTRA) && defined(HAVE_CRL) && !defined(NO_FILESYSTEM) && \
    !defined(NO_RSA)
    X509 *x509 = NULL;
    XFILE fp;

    printf(testingFmt, "wolfSSL_PEM_read_X509");
    fp = XFOPEN(svrCertFile, "rb");
    AssertTrue((fp != XBADFILE));
    AssertNotNull(x509 = (X509 *)PEM_read_X509(fp, (X509 **)NULL, NULL, NULL));
    X509_free(x509);
    XFCLOSE(fp);

    printf(resultFmt, passed);
#endif
}

static void test_wolfSSL_PEM_read(void)
{
#if defined(OPENSSL_EXTRA) && !defined(NO_FILESYSTEM)
    const char* filename = "./certs/server-keyEnc.pem";
    XFILE fp;
    char* name = NULL;
    char* header = NULL;
    byte* data = NULL;
    long len;
    EVP_CIPHER_INFO cipher;
    WOLFSSL_BIO* bio;
    byte* fileData;
    size_t fileDataSz;
    byte* out;

    printf(testingFmt, "wolfSSL_PEM_read");
    fp = XFOPEN(filename, "rb");
    AssertTrue((fp != XBADFILE));

    /* Fail cases. */
    AssertIntEQ(PEM_read(fp, NULL, &header, &data, &len), WOLFSSL_FAILURE);
    AssertIntEQ(PEM_read(fp, &name, NULL, &data, &len), WOLFSSL_FAILURE);
    AssertIntEQ(PEM_read(fp, &name, &header, NULL, &len), WOLFSSL_FAILURE);
    AssertIntEQ(PEM_read(fp, &name, &header, &data, NULL), WOLFSSL_FAILURE);

    AssertIntEQ(PEM_read(fp, &name, &header, &data, &len), WOLFSSL_SUCCESS);

    AssertIntEQ(XSTRNCMP(name, "RSA PRIVATE KEY", 15), 0);
    AssertIntGT(XSTRLEN(header), 0);
    AssertIntGT(len, 0);

    AssertIntEQ(XFSEEK(fp, 0, SEEK_END), 0);
    AssertIntGT((fileDataSz = XFTELL(fp)), 0);
    AssertIntEQ(XFSEEK(fp, 0, SEEK_SET), 0);
    AssertNotNull(fileData = (unsigned char*)XMALLOC(fileDataSz, NULL,
                                                      DYNAMIC_TYPE_TMP_BUFFER));
    AssertIntEQ(XFREAD(fileData, 1, fileDataSz, fp), fileDataSz);
    XFCLOSE(fp);

    AssertNotNull(bio = wolfSSL_BIO_new(wolfSSL_BIO_s_mem()));

    /* Fail cases. */
    AssertIntEQ(PEM_write_bio(NULL, name, header, data, len), 0);
    AssertIntEQ(PEM_write_bio(bio, NULL, header, data, len), 0);
    AssertIntEQ(PEM_write_bio(bio, name, NULL, data, len), 0);
    AssertIntEQ(PEM_write_bio(bio, name, header, NULL, len), 0);

    AssertIntEQ(PEM_write_bio(bio, name, header, data, len), fileDataSz);
    AssertIntEQ(wolfSSL_BIO_get_mem_data(bio, &out), fileDataSz);
    AssertIntEQ(XMEMCMP(out, fileData, fileDataSz), 0);

    /* Fail cases. */
    AssertIntEQ(PEM_get_EVP_CIPHER_INFO(NULL, &cipher), WOLFSSL_FAILURE);
    AssertIntEQ(PEM_get_EVP_CIPHER_INFO(header, NULL), WOLFSSL_FAILURE);
    AssertIntEQ(PEM_get_EVP_CIPHER_INFO((char*)"", &cipher), WOLFSSL_FAILURE);

#ifndef NO_DES3
    AssertIntEQ(PEM_get_EVP_CIPHER_INFO(header, &cipher), WOLFSSL_SUCCESS);
#endif

    /* Fail cases. */
    AssertIntEQ(PEM_do_header(&cipher, NULL, &len, PasswordCallBack,
                              (void*)"yassl123"), WOLFSSL_FAILURE);
    AssertIntEQ(PEM_do_header(&cipher, data, NULL, PasswordCallBack,
                              (void*)"yassl123"), WOLFSSL_FAILURE);
    AssertIntEQ(PEM_do_header(&cipher, data, &len, NULL,
                              (void*)"yassl123"), WOLFSSL_FAILURE);

#if !defined(NO_DES3) && !defined(NO_MD5)
    AssertIntEQ(PEM_do_header(&cipher, data, &len, PasswordCallBack,
                              (void*)"yassl123"), WOLFSSL_SUCCESS);
#endif

    BIO_free(bio);
    XFREE(fileData, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    XFREE(name, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    XFREE(header, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    XFREE(data, NULL, DYNAMIC_TYPE_TMP_BUFFER);

    name = NULL;
    header = NULL;
    data = NULL;
    fp = XFOPEN(svrKeyFile, "rb");
    AssertTrue((fp != XBADFILE));
    AssertIntEQ(PEM_read(fp, &name, &header, &data, &len), WOLFSSL_SUCCESS);
    AssertIntEQ(XSTRNCMP(name, "RSA PRIVATE KEY", 15), 0);
    AssertIntEQ(XSTRLEN(header), 0);
    AssertIntGT(len, 0);

    AssertIntEQ(XFSEEK(fp, 0, SEEK_END), 0);
    AssertIntGT((fileDataSz = XFTELL(fp)), 0);
    AssertIntEQ(XFSEEK(fp, 0, SEEK_SET), 0);
    AssertNotNull(fileData = (unsigned char*)XMALLOC(fileDataSz, NULL,
                                                      DYNAMIC_TYPE_TMP_BUFFER));
    AssertIntEQ(XFREAD(fileData, 1, fileDataSz, fp), fileDataSz);
    XFCLOSE(fp);

    AssertNotNull(bio = wolfSSL_BIO_new(wolfSSL_BIO_s_mem()));
    AssertIntEQ(PEM_write_bio(bio, name, header, data, len), fileDataSz);
    AssertIntEQ(wolfSSL_BIO_get_mem_data(bio, &out), fileDataSz);
    AssertIntEQ(XMEMCMP(out, fileData, fileDataSz), 0);

    BIO_free(bio);
    XFREE(fileData, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    XFREE(name, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    XFREE(header, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    XFREE(data, NULL, DYNAMIC_TYPE_TMP_BUFFER);

    printf(resultFmt, passed);
#endif
}

static void test_wolfssl_EVP_aes_gcm(void)
{
#if defined(OPENSSL_EXTRA) && !defined(NO_AES) && defined(HAVE_AESGCM)

    /* A 256 bit key, AES_128 will use the first 128 bit*/
    byte *key = (byte*)"01234567890123456789012345678901";
    /* A 128 bit IV */
    byte *iv = (byte*)"0123456789012345";
    int ivSz = AES_BLOCK_SIZE;
    /* Message to be encrypted */
    byte *plaintxt = (byte*)"for things to change you have to change";
    /* Additional non-confidential data */
    byte *aad = (byte*)"Don't spend major time on minor things.";

    unsigned char tag[AES_BLOCK_SIZE] = {0};
    int plaintxtSz = (int)XSTRLEN((char*)plaintxt);
    int aadSz = (int)XSTRLEN((char*)aad);
    byte ciphertxt[AES_BLOCK_SIZE * 4] = {0};
    byte decryptedtxt[AES_BLOCK_SIZE * 4] = {0};
    int ciphertxtSz = 0;
    int decryptedtxtSz = 0;
    int len = 0;
    int i = 0;
    EVP_CIPHER_CTX en[2];
    EVP_CIPHER_CTX de[2];

    printf(testingFmt, "wolfssl_EVP_aes_gcm");

    for (i = 0; i < 2; i++) {

        EVP_CIPHER_CTX_init(&en[i]);

        if (i == 0) {
            /* Default uses 96-bits IV length */
#ifdef WOLFSSL_AES_128
            AssertIntEQ(1, EVP_EncryptInit_ex(&en[i], EVP_aes_128_gcm(), NULL, key, iv));
#elif defined(WOLFSSL_AES_192)
            AssertIntEQ(1, EVP_EncryptInit_ex(&en[i], EVP_aes_192_gcm(), NULL, key, iv));
#elif defined(WOLFSSL_AES_256)
            AssertIntEQ(1, EVP_EncryptInit_ex(&en[i], EVP_aes_256_gcm(), NULL, key, iv));
#endif
        }
        else {
#ifdef WOLFSSL_AES_128
            AssertIntEQ(1, EVP_EncryptInit_ex(&en[i], EVP_aes_128_gcm(), NULL, NULL, NULL));
#elif defined(WOLFSSL_AES_192)
            AssertIntEQ(1, EVP_EncryptInit_ex(&en[i], EVP_aes_192_gcm(), NULL, NULL, NULL));
#elif defined(WOLFSSL_AES_256)
            AssertIntEQ(1, EVP_EncryptInit_ex(&en[i], EVP_aes_256_gcm(), NULL, NULL, NULL));
#endif
             /* non-default must to set the IV length first */
            AssertIntEQ(1, EVP_CIPHER_CTX_ctrl(&en[i], EVP_CTRL_GCM_SET_IVLEN, ivSz, NULL));
            AssertIntEQ(1, EVP_EncryptInit_ex(&en[i], NULL, NULL, key, iv));
        }
        AssertIntEQ(1, EVP_EncryptUpdate(&en[i], NULL, &len, aad, aadSz));
        AssertIntEQ(1, EVP_EncryptUpdate(&en[i], ciphertxt, &len, plaintxt, plaintxtSz));
        ciphertxtSz = len;
        AssertIntEQ(1, EVP_EncryptFinal_ex(&en[i], ciphertxt, &len));
        ciphertxtSz += len;
        AssertIntEQ(1, EVP_CIPHER_CTX_ctrl(&en[i], EVP_CTRL_GCM_GET_TAG, AES_BLOCK_SIZE, tag));

        EVP_CIPHER_CTX_init(&de[i]);
        if (i == 0) {
            /* Default uses 96-bits IV length */
#ifdef WOLFSSL_AES_128
            AssertIntEQ(1, EVP_EncryptInit_ex(&de[i], EVP_aes_128_gcm(), NULL, key, iv));
#elif defined(WOLFSSL_AES_192)
        AssertIntEQ(1, EVP_EncryptInit_ex(&de[i], EVP_aes_192_gcm(), NULL, key, iv));
#elif defined(WOLFSSL_AES_256)
        AssertIntEQ(1, EVP_EncryptInit_ex(&de[i], EVP_aes_256_gcm(), NULL, key, iv));
#endif
        }
        else {
#ifdef WOLFSSL_AES_128
            AssertIntEQ(1, EVP_EncryptInit_ex(&de[i], EVP_aes_128_gcm(), NULL, NULL, NULL));
#elif defined(WOLFSSL_AES_192)
            AssertIntEQ(1, EVP_EncryptInit_ex(&de[i], EVP_aes_192_gcm(), NULL, NULL, NULL));
#elif defined(WOLFSSL_AES_256)
            AssertIntEQ(1, EVP_EncryptInit_ex(&de[i], EVP_aes_256_gcm(), NULL, NULL, NULL));
#endif
            /* non-default must to set the IV length first */
            AssertIntEQ(1, EVP_CIPHER_CTX_ctrl(&de[i], EVP_CTRL_GCM_SET_IVLEN, ivSz, NULL));
            AssertIntEQ(1, EVP_EncryptInit_ex(&de[i], NULL, NULL, key, iv));

        }
        AssertIntEQ(1, EVP_EncryptUpdate(&de[i], NULL, &len, aad, aadSz));
        AssertIntEQ(1, EVP_CIPHER_CTX_ctrl(&de[i], EVP_CTRL_GCM_SET_TAG, AES_BLOCK_SIZE, tag));
        AssertIntEQ(1, EVP_DecryptUpdate(&de[i], decryptedtxt, &len, ciphertxt, ciphertxtSz));
        decryptedtxtSz = len;
        AssertIntGT(EVP_DecryptFinal_ex(&de[i], decryptedtxt, &len), 0);
        decryptedtxtSz += len;
        AssertIntEQ(ciphertxtSz, decryptedtxtSz);
        AssertIntEQ(0, XMEMCMP(plaintxt, decryptedtxt, decryptedtxtSz));

        /* modify tag*/
        tag[AES_BLOCK_SIZE-1]+=0xBB;
        AssertIntEQ(1, EVP_EncryptUpdate(&de[i], NULL, &len, aad, aadSz));
        AssertIntEQ(1, EVP_CIPHER_CTX_ctrl(&de[i], EVP_CTRL_GCM_SET_TAG, AES_BLOCK_SIZE, tag));
        AssertIntEQ(1, EVP_DecryptUpdate(&de[i], decryptedtxt, &len, ciphertxt, ciphertxtSz));
        decryptedtxtSz = len;
        AssertIntGT(EVP_DecryptFinal_ex(&de[i], decryptedtxt, &len), 0);
        decryptedtxtSz += len;
        AssertIntEQ(ciphertxtSz, decryptedtxtSz);
        /* decrypted text should not be equal to plain text*/
        AssertIntNE(0, XMEMCMP(plaintxt, decryptedtxt, decryptedtxtSz));
    }
    printf(resultFmt, passed);

#endif /* OPENSSL_EXTRA && !NO_AES && HAVE_AESGCM */
}

static void test_wolfSSL_PEM_X509_INFO_read_bio(void)
{
#if defined(OPENSSL_ALL) && !defined(NO_FILESYSTEM)
    BIO* bio;
    X509_INFO* info;
    STACK_OF(X509_INFO)* sk;
    char* subject;
    char exp1[] = "/C=US/ST=Montana/L=Bozeman/O=Sawtooth/OU=Consulting/CN=www.wolfssl.com/emailAddress=info@wolfssl.com";
    char exp2[] = "/C=US/ST=Montana/L=Bozeman/O=wolfSSL/OU=Support/CN=www.wolfssl.com/emailAddress=info@wolfssl.com";

    printf(testingFmt, "wolfSSL_PEM_X509_INFO_read_bio");
    AssertNotNull(bio = BIO_new(BIO_s_file()));
    AssertIntGT(BIO_read_filename(bio, svrCertFile), 0);
    AssertNotNull(sk = PEM_X509_INFO_read_bio(bio, NULL, NULL, NULL));
    AssertIntEQ(sk_X509_INFO_num(sk), 2);

    /* using dereference to maintain testing for Apache port*/
    AssertNotNull(info = sk_X509_INFO_pop(sk));
    AssertNotNull(info->x_pkey);
    AssertNotNull(info->x_pkey->dec_pkey);
    AssertIntEQ(EVP_PKEY_bits(info->x_pkey->dec_pkey), 2048);
    AssertNotNull(subject =
            X509_NAME_oneline(X509_get_subject_name(info->x509), 0, 0));

    AssertIntEQ(0, XSTRNCMP(subject, exp1, sizeof(exp1)));
    XFREE(subject, 0, DYNAMIC_TYPE_OPENSSL);
    X509_INFO_free(info);

    AssertNotNull(info = sk_X509_INFO_pop(sk));
    AssertNotNull(subject =
            X509_NAME_oneline(X509_get_subject_name(info->x509), 0, 0));

    AssertIntEQ(0, XSTRNCMP(subject, exp2, sizeof(exp2)));
    XFREE(subject, 0, DYNAMIC_TYPE_OPENSSL);
    X509_INFO_free(info);
    AssertNull(info = sk_X509_INFO_pop(sk));

    sk_X509_INFO_pop_free(sk, X509_INFO_free);
    BIO_free(bio);
    printf(resultFmt, passed);
#endif
}

static void test_wolfSSL_X509_NAME_ENTRY_get_object()
{
#if defined(OPENSSL_EXTRA) && !defined(NO_FILESYSTEM) && !defined(NO_RSA)
    X509 *x509 = NULL;
    X509_NAME* name = NULL;
    int idx = 0;
    X509_NAME_ENTRY *ne = NULL;
    ASN1_OBJECT *object = NULL;

    printf(testingFmt, "wolfSSL_X509_NAME_ENTRY_get_object");
    x509 = wolfSSL_X509_load_certificate_file(cliCertFile, WOLFSSL_FILETYPE_PEM);
    AssertNotNull(x509);
    name = X509_get_subject_name(x509);
    idx = X509_NAME_get_index_by_NID(name, NID_commonName, -1);
    AssertIntGE(idx, 0);

    ne = X509_NAME_get_entry(name, idx);
    AssertNotNull(ne);
    AssertNotNull(object = X509_NAME_ENTRY_get_object(ne));

    X509_free(x509);

    printf(resultFmt, passed);
#endif
}

static void test_wolfSSL_ASN1_INTEGER_set()
{
#if defined(OPENSSL_EXTRA) && !defined(NO_ASN)
    ASN1_INTEGER *a;
    long val;
    int ret;

    printf(testingFmt, "wolfSSL_ASN1_INTEGER_set");

    a = wolfSSL_ASN1_INTEGER_new();
    val = 0;
    ret = ASN1_INTEGER_set(NULL, val);
    AssertIntEQ(ret, 0);
    wolfSSL_ASN1_INTEGER_free(a);

    /* 0 */
    a = wolfSSL_ASN1_INTEGER_new();
    val = 0;
    ret = ASN1_INTEGER_set(a, val);
    AssertIntEQ(ret, 1);
    wolfSSL_ASN1_INTEGER_free(a);

    /* 40 */
    a = wolfSSL_ASN1_INTEGER_new();
    val = 40;
    ret = ASN1_INTEGER_set(a, val);
    AssertIntEQ(ret, 1);
    wolfSSL_ASN1_INTEGER_free(a);

    /* -40 */
    a = wolfSSL_ASN1_INTEGER_new();
    val = -40;
    ret = ASN1_INTEGER_set(a, val);
    AssertIntEQ(ret, 1);
    AssertIntEQ(a->negative, 1);
    wolfSSL_ASN1_INTEGER_free(a);

    /* 128 */
    a = wolfSSL_ASN1_INTEGER_new();
    val = 128;
    ret = ASN1_INTEGER_set(a, val);
    AssertIntEQ(ret, 1);
    wolfSSL_ASN1_INTEGER_free(a);

    /* -128 */
    a = wolfSSL_ASN1_INTEGER_new();
    val = -128;
    ret = ASN1_INTEGER_set(a, val);
    AssertIntEQ(ret, 1);
    AssertIntEQ(a->negative, 1);
    wolfSSL_ASN1_INTEGER_free(a);

    /* 200 */
    a = wolfSSL_ASN1_INTEGER_new();
    val = 200;
    ret = ASN1_INTEGER_set(a, val);
    AssertIntEQ(ret, 1);
    wolfSSL_ASN1_INTEGER_free(a);

#ifndef TIME_T_NOT_64BIT
    /* 2147483648 */
    a = wolfSSL_ASN1_INTEGER_new();
    val = 2147483648;
    ret = ASN1_INTEGER_set(a, val);
    AssertIntEQ(ret, 1);
    wolfSSL_ASN1_INTEGER_free(a);

    /* -2147483648 */
    a = wolfSSL_ASN1_INTEGER_new();
    val = -2147483648;
    ret = ASN1_INTEGER_set(a, val);
    AssertIntEQ(a->negative, 1);
    AssertIntEQ(ret, 1);
    wolfSSL_ASN1_INTEGER_free(a);
#endif

    printf(resultFmt, passed);
#endif
}

static void test_wolfSSL_i2c_ASN1_INTEGER()
{
#if defined(OPENSSL_EXTRA) && !defined(NO_ASN)
    ASN1_INTEGER *a;
    unsigned char *pp,*tpp;
    int ret;

    printf(testingFmt, "wolfSSL_i2c_ASN1_INTEGER");

    a = wolfSSL_ASN1_INTEGER_new();

    /* 40 */
    a->intData[0] = ASN_INTEGER;
    a->intData[1] = 1;
    a->intData[2] = 40;
    ret = i2c_ASN1_INTEGER(a, NULL);
    AssertIntEQ(ret, 1);
    AssertNotNull(pp = (unsigned char*)XMALLOC(ret + 1, NULL,
                DYNAMIC_TYPE_TMP_BUFFER));
    tpp = pp;
    XMEMSET(pp, 0, ret + 1);
    i2c_ASN1_INTEGER(a, &pp);
    pp--;
    AssertIntEQ(*pp, 40);
    XFREE(tpp, NULL, DYNAMIC_TYPE_TMP_BUFFER);

    /* 128 */
    a->intData[0] = ASN_INTEGER;
    a->intData[1] = 1;
    a->intData[2] = 128;
    ret = wolfSSL_i2c_ASN1_INTEGER(a, NULL);
    AssertIntEQ(ret, 2);
    AssertNotNull(pp = (unsigned char*)XMALLOC(ret + 1, NULL,
                DYNAMIC_TYPE_TMP_BUFFER));
    tpp = pp;
    XMEMSET(pp, 0, ret + 1);
    wolfSSL_i2c_ASN1_INTEGER(a, &pp);
    pp--;
    AssertIntEQ(*(pp--), 128);
    AssertIntEQ(*pp, 0);
    XFREE(tpp, NULL, DYNAMIC_TYPE_TMP_BUFFER);

    /* -40 */
    a->intData[0] = ASN_INTEGER;
    a->intData[1] = 1;
    a->intData[2] = 40;
    a->negative = 1;
    ret = wolfSSL_i2c_ASN1_INTEGER(a, NULL);
    AssertIntEQ(ret, 1);
    AssertNotNull(pp = (unsigned char*)XMALLOC(ret + 1, NULL,
                DYNAMIC_TYPE_TMP_BUFFER));
    tpp = pp;
    XMEMSET(pp, 0, ret + 1);
    wolfSSL_i2c_ASN1_INTEGER(a, &pp);
    pp--;
    AssertIntEQ(*pp, 216);
    XFREE(tpp, NULL, DYNAMIC_TYPE_TMP_BUFFER);

    /* -128 */
    a->intData[0] = ASN_INTEGER;
    a->intData[1] = 1;
    a->intData[2] = 128;
    a->negative = 1;
    ret = wolfSSL_i2c_ASN1_INTEGER(a, NULL);
    AssertIntEQ(ret, 1);
    AssertNotNull(pp = (unsigned char*)XMALLOC(ret + 1, NULL,
                DYNAMIC_TYPE_TMP_BUFFER));
    tpp = pp;
    XMEMSET(pp, 0, ret + 1);
    wolfSSL_i2c_ASN1_INTEGER(a, &pp);
    pp--;
    AssertIntEQ(*pp, 128);
    XFREE(tpp, NULL, DYNAMIC_TYPE_TMP_BUFFER);

    /* -200 */
    a->intData[0] = ASN_INTEGER;
    a->intData[1] = 1;
    a->intData[2] = 200;
    a->negative = 1;
    ret = wolfSSL_i2c_ASN1_INTEGER(a, NULL);
    AssertIntEQ(ret, 2);
    AssertNotNull(pp = (unsigned char*)XMALLOC(ret + 1, NULL,
            DYNAMIC_TYPE_TMP_BUFFER));
    tpp = pp;
    XMEMSET(pp, 0, ret + 1);
    wolfSSL_i2c_ASN1_INTEGER(a, &pp);
    pp--;
    AssertIntEQ(*(pp--), 56);
    AssertIntEQ(*pp, 255);

    XFREE(tpp, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    wolfSSL_ASN1_INTEGER_free(a);

    printf(resultFmt, passed);
#endif /* OPENSSL_EXTRA && !NO_ASN */
}

#ifndef NO_INLINE
#define WOLFSSL_MISC_INCLUDED
#include <wolfcrypt/src/misc.c>
#else
#include <wolfssl/wolfcrypt/misc.h>
#endif

static int test_ForceZero(void)
{
    unsigned char data[32];
    unsigned int i, j, len;

    /* Test case with 0 length */
    ForceZero(data, 0);

    /* Test ForceZero */
    for (i = 0; i < sizeof(data); i++) {
        for (len = 1; len < sizeof(data) - i; len++) {
            for (j = 0; j < sizeof(data); j++)
                data[j] = j + 1;

            ForceZero(data + i, len);

            for (j = 0; j < sizeof(data); j++) {
                if (j < i || j >= i + len) {
                    if (data[j] == 0x00)
                        return -10200;
                }
                else if (data[j] != 0x00)
                    return -10201;
            }
        }
    }

    return 0;
}

static void test_wolfSSL_X509_print()
{
#if defined(OPENSSL_EXTRA) && !defined(NO_FILESYSTEM) && \
   !defined(NO_RSA) && !defined(HAVE_FAST_RSA) && defined(XSNPRINTF)
    X509 *x509;
    BIO *bio;

    printf(testingFmt, "wolfSSL_X509_print");
    x509 = X509_load_certificate_file(svrCertFile, WOLFSSL_FILETYPE_PEM);
    AssertNotNull(x509);

    /* print to memory */
    AssertNotNull(bio = BIO_new(BIO_s_mem()));
    AssertIntEQ(X509_print(bio, x509), SSL_SUCCESS);

    AssertIntEQ(BIO_get_mem_data(bio, NULL), 3212);
    BIO_free(bio);

    /* print to stdout */
    AssertNotNull(bio = BIO_new(BIO_s_file()));
    wolfSSL_BIO_set_fp(bio, stdout, BIO_NOCLOSE);
    AssertIntEQ(X509_print(bio, x509), SSL_SUCCESS);
    BIO_free(bio);

    X509_free(x509);
    printf(resultFmt, passed);
#endif
}

static void test_wolfSSL_RSA_print()
{
#if defined(OPENSSL_EXTRA) && !defined(NO_FILESYSTEM) && \
   !defined(NO_RSA) && !defined(HAVE_FAST_RSA) && defined(WOLFSSL_KEY_GEN) && \
   !defined(HAVE_FAST_RSA)
    BIO *bio;
    WOLFSSL_RSA* rsa = NULL;
    printf(testingFmt, "wolfSSL_RSA_print");

    AssertNotNull(rsa = RSA_generate_key(2048, 3, NULL, NULL));
    AssertNotNull(bio = wolfSSL_BIO_new(wolfSSL_BIO_s_file()));
    wolfSSL_BIO_set_fp(bio, stdout, BIO_NOCLOSE);
    AssertIntEQ(RSA_print(bio, rsa, 0), SSL_SUCCESS);

    BIO_free(bio);
    wolfSSL_RSA_free(rsa);
    printf(resultFmt, passed);
#endif
}

static void test_wolfSSL_BIO_get_len()
{
#if defined(OPENSSL_EXTRA)
    BIO *bio;
    const char txt[] = "Some example text to push to the BIO.";
    printf(testingFmt, "wolfSSL_BIO_get_len");

    AssertNotNull(bio = wolfSSL_BIO_new(wolfSSL_BIO_s_mem()));
    AssertIntEQ(wolfSSL_BIO_write(bio, txt, sizeof(txt)), sizeof(txt));
    AssertIntEQ(wolfSSL_BIO_get_len(bio), sizeof(txt));

    BIO_free(bio);
    printf(resultFmt, passed);
#endif
}

static void test_wolfSSL_ASN1_STRING_print(void){
#if defined(OPENSSL_ALL) && !defined(NO_ASN) && !defined(NO_CERTS)
    ASN1_STRING* asnStr = NULL;
    const char HELLO_DATA[]= \
                      {'H','e','l','l','o',' ','w','o','l','f','S','S','L','!'};
    const unsigned int MAX_UNPRINTABLE_CHAR = 32;
    const unsigned int MAX_BUF = 255;
    const int LF = 10, CR = 13;
    unsigned char unprintableData[MAX_UNPRINTABLE_CHAR + sizeof(HELLO_DATA)];
    unsigned char expected[sizeof(unprintableData)+1];
    unsigned char rbuf[MAX_BUF];

    BIO *bio;
    int p_len, i;

    printf(testingFmt, "wolfSSL_ASN1_STRING_print()");

    /* setup */

    for (i = 0; i < (int)sizeof(HELLO_DATA); i++) {
        unprintableData[i]  = HELLO_DATA[i];
        expected[i]         = HELLO_DATA[i];
    }

    for (i = 0; i < (int)MAX_UNPRINTABLE_CHAR; i++) {
        unprintableData[sizeof(HELLO_DATA)+i] = i;

        if (i == LF || i == CR)
            expected[sizeof(HELLO_DATA)+i] = i;
        else
            expected[sizeof(HELLO_DATA)+i] = '.';
    }

    unprintableData[sizeof(unprintableData)-1] = '\0';
    expected[sizeof(expected)-1] = '\0';

    XMEMSET(rbuf, 0, MAX_BUF);
    bio = BIO_new(BIO_s_mem());
    BIO_set_write_buf_size(bio, MAX_BUF);

    asnStr = ASN1_STRING_type_new(V_ASN1_OCTET_STRING);
    ASN1_STRING_set(asnStr,(const void*)unprintableData,
            sizeof(unprintableData));
    /* test */
    p_len = wolfSSL_ASN1_STRING_print(bio, asnStr);
    AssertIntEQ(p_len, 46);
    BIO_read(bio, (void*)rbuf, 46);

    AssertStrEQ((char*)rbuf, (const char*)expected);

    BIO_free(bio);
    ASN1_STRING_free(asnStr);

    printf(resultFmt, passed);
#endif /* OPENSSL_EXTRA && !NO_ASN && !NO_CERTS */
}

static void test_wolfSSL_RSA_verify()
{
#if defined(OPENSSL_EXTRA) && !defined(NO_RSA) && !defined(HAVE_FAST_RSA) && \
    !defined(NO_FILESYSTEM) && defined(HAVE_CRL)
    XFILE fp;
    RSA *pKey, *pubKey;
    X509 *cert;
    const char *text = "Hello wolfSSL !";
    unsigned char hash[SHA256_DIGEST_LENGTH];
    unsigned char signature[2048/8];
    unsigned int signatureLength;
    byte *buf;
    BIO *bio;
    SHA256_CTX c;
    EVP_PKEY *evpPkey, *evpPubkey;
    size_t sz;

    printf(testingFmt, "wolfSSL_RSA_verify");

    /* generate hash */
    SHA256_Init(&c);
    SHA256_Update(&c, text, strlen(text));
    SHA256_Final(hash, &c);

    /* read privete key file */
    fp = XFOPEN(svrKeyFile, "r");
    AssertTrue((fp != XBADFILE));
    XFSEEK(fp, 0, XSEEK_END);
    sz = XFTELL(fp);
    XREWIND(fp);
    AssertNotNull(buf = (byte*)XMALLOC(sz, NULL, DYNAMIC_TYPE_FILE));
    AssertIntEQ(XFREAD(buf, 1, sz, fp), sz);
    XFCLOSE(fp);

    /* read private key and sign hash data */
    AssertNotNull(bio = BIO_new_mem_buf(buf, (int)sz));
    AssertNotNull(evpPkey = PEM_read_bio_PrivateKey(bio, NULL, NULL, NULL));
    AssertNotNull(pKey = EVP_PKEY_get1_RSA(evpPkey));
    AssertIntEQ(RSA_sign(NID_sha256, hash, SHA256_DIGEST_LENGTH,
                            signature, &signatureLength, pKey), SSL_SUCCESS);

    /* read public key and verify signed data */
    fp = XFOPEN(svrCertFile,"r");
    AssertTrue((fp != XBADFILE));
    cert = PEM_read_X509(fp, 0, 0, 0 );
    XFCLOSE(fp);
    evpPubkey = X509_get_pubkey(cert);
    pubKey = EVP_PKEY_get1_RSA(evpPubkey);
    AssertIntEQ(RSA_verify(NID_sha256, hash, SHA256_DIGEST_LENGTH, signature,
                                signatureLength, pubKey), SSL_SUCCESS);

    RSA_free(pKey);
    EVP_PKEY_free(evpPkey);
    RSA_free(pubKey);
    EVP_PKEY_free(evpPubkey);
    X509_free(cert);
    BIO_free(bio);
    XFREE(buf, NULL, DYNAMIC_TYPE_FILE);
    printf(resultFmt, passed);
#endif
}


#if defined(OPENSSL_EXTRA) && !defined(NO_CERTS) && \
    defined(WOLFSSL_CERT_GEN) && defined(WOLFSSL_CERT_REQ)
static void test_openssl_make_self_signed_certificate(EVP_PKEY* pkey)
{
    X509* x509 = NULL;
    BIGNUM* serial_number = NULL;
    X509_NAME* name = NULL;
    time_t epoch_off = 0;
    ASN1_INTEGER* asn1_serial_number;
    long not_before, not_after;

    AssertNotNull(x509 = X509_new());

    AssertIntNE(X509_set_pubkey(x509, pkey), 0);

    AssertNotNull(serial_number = BN_new());
    AssertIntNE(BN_pseudo_rand(serial_number, 64, 0, 0), 0);
    AssertNotNull(asn1_serial_number = X509_get_serialNumber(x509));
    AssertNotNull(BN_to_ASN1_INTEGER(serial_number, asn1_serial_number));

    /* version 3 */
    AssertIntNE(X509_set_version(x509, 2L), 0);

    AssertNotNull(name = X509_NAME_new());

    AssertIntNE(X509_NAME_add_entry_by_NID(name, NID_commonName, MBSTRING_UTF8,
        (unsigned char*)"www.wolfssl.com", -1, -1, 0), 0);

    AssertIntNE(X509_set_subject_name(x509, name), 0);
    AssertIntNE(X509_set_issuer_name(x509, name), 0);

    not_before = (long)XTIME(NULL);
    not_after = not_before + (365 * 24 * 60 * 60);
    AssertNotNull(X509_time_adj(X509_get_notBefore(x509), not_before, &epoch_off));
    AssertNotNull(X509_time_adj(X509_get_notAfter(x509), not_after, &epoch_off));

    AssertIntNE(X509_sign(x509, pkey, EVP_sha256()), 0);

    BN_free(serial_number);
    X509_NAME_free(name);
    X509_free(x509);
}
#endif

static void test_openssl_generate_key_and_cert(void)
{
#if defined(OPENSSL_EXTRA)
#if !defined(NO_RSA)
    {
        EVP_PKEY* pkey = EVP_PKEY_new();
        int key_length = 2048;
        BIGNUM* exponent = BN_new();
        RSA* rsa = RSA_new();

        AssertNotNull(pkey);
        AssertNotNull(exponent);
        AssertNotNull(rsa);

        AssertIntNE(BN_set_word(exponent, WC_RSA_EXPONENT), 0);
    #ifndef WOLFSSL_KEY_GEN
        AssertIntEQ(RSA_generate_key_ex(rsa, key_length, exponent, NULL), WOLFSSL_FAILURE);

        #if defined(USE_CERT_BUFFERS_1024)
        AssertIntNE(wolfSSL_RSA_LoadDer_ex(rsa, server_key_der_1024,
            sizeof_server_key_der_1024, WOLFSSL_RSA_LOAD_PRIVATE), 0);
        key_length = 1024;
        #elif defined(USE_CERT_BUFFERS_2048)
        AssertIntNE(wolfSSL_RSA_LoadDer_ex(rsa, server_key_der_2048,
            sizeof_server_key_der_2048, WOLFSSL_RSA_LOAD_PRIVATE), 0);
        #else
        RSA_free(rsa);
        rsa = NULL;
        #endif
    #else
        AssertIntNE(RSA_generate_key_ex(rsa, key_length, exponent, NULL), 0);
    #endif

        if (rsa) {
            AssertIntNE(EVP_PKEY_assign_RSA(pkey, rsa), 0);

            BN_free(exponent);

        #if !defined(NO_CERTS) && defined(WOLFSSL_CERT_GEN) && defined(WOLFSSL_CERT_REQ)
            test_openssl_make_self_signed_certificate(pkey);
        #endif
        }

        EVP_PKEY_free(pkey);
    }
#endif /* !NO_RSA */

#ifdef HAVE_ECC
    {
        EVP_PKEY* pkey = EVP_PKEY_new();
        EC_KEY* ec_key = EC_KEY_new_by_curve_name(NID_X9_62_prime256v1);

        AssertNotNull(pkey);
        AssertNotNull(ec_key);

        EC_KEY_set_asn1_flag(ec_key, OPENSSL_EC_NAMED_CURVE);

        AssertIntNE(EC_KEY_generate_key(ec_key), 0);
        AssertIntNE(EVP_PKEY_assign_EC_KEY(pkey, ec_key), 0);

    #if !defined(NO_CERTS) && defined(WOLFSSL_CERT_GEN) && defined(WOLFSSL_CERT_REQ)
        test_openssl_make_self_signed_certificate(pkey);
    #endif

        EVP_PKEY_free(pkey);
    }
#endif /* HAVE_ECC */
#endif /* OPENSSL_EXTRA */
}

static void test_stubs_are_stubs()
{
#if defined(OPENSSL_EXTRA) && !defined(NO_WOLFSSL_STUB)
    WOLFSSL_CTX* ctx = NULL;
    WOLFSSL_CTX* ctxN = NULL;
  #ifndef NO_WOLFSSL_CLIENT
    AssertNotNull(ctx = wolfSSL_CTX_new(wolfSSLv23_client_method()));
  #elif !defined(NO_WOLFSSL_SERVER)
    AssertNotNull(ctx = wolfSSL_CTX_new(wolfSSLv23_server_method()));
  #else
    return;
  #endif

    #define CHECKZERO_RET(x, y, z) AssertIntEQ((int) x(y), 0); \
                     AssertIntEQ((int) x(z), 0)
    /* test logic, all stubs return same result regardless of ctx being NULL
     * as there are no sanity checks, it's just a stub! If at some
     * point a stub is not a stub it should begin to return BAD_FUNC_ARG
     * if invalid inputs are supplied. Test calling both
     * with and without valid inputs, if a stub functionality remains unchanged.
     */
    CHECKZERO_RET(wolfSSL_CTX_sess_accept, ctx, ctxN);
    CHECKZERO_RET(wolfSSL_CTX_sess_connect, ctx, ctxN);
    CHECKZERO_RET(wolfSSL_CTX_sess_accept_good, ctx, ctxN);
    CHECKZERO_RET(wolfSSL_CTX_sess_connect_good, ctx, ctxN);
    CHECKZERO_RET(wolfSSL_CTX_sess_accept_renegotiate, ctx, ctxN);
    CHECKZERO_RET(wolfSSL_CTX_sess_connect_renegotiate, ctx, ctxN);
    CHECKZERO_RET(wolfSSL_CTX_sess_hits, ctx, ctxN);
    CHECKZERO_RET(wolfSSL_CTX_sess_cb_hits, ctx, ctxN);
    CHECKZERO_RET(wolfSSL_CTX_sess_cache_full, ctx, ctxN);
    CHECKZERO_RET(wolfSSL_CTX_sess_misses, ctx, ctxN);
    CHECKZERO_RET(wolfSSL_CTX_sess_timeouts, ctx, ctxN);
    wolfSSL_CTX_free(ctx);
    ctx = NULL;
#endif /* OPENSSL_EXTRA && !NO_WOLFSSL_STUB */
}

static void test_wolfSSL_CTX_LoadCRL()
{
#ifdef HAVE_CRL
    WOLFSSL_CTX* ctx = NULL;
    const char* badPath = "dummypath";
    const char* validPath = "./certs/crl";
    int derType = WOLFSSL_FILETYPE_ASN1;
    int rawType = WOLFSSL_FILETYPE_RAW;
    int pemType = WOLFSSL_FILETYPE_PEM;
    int monitor = WOLFSSL_CRL_MONITOR;

    #define FAIL_T1(x, y, z, p, d) AssertIntEQ((int) x(y, z, p, d), \
                                                BAD_FUNC_ARG)
    #define SUCC_T(x, y, z, p, d) AssertIntEQ((int) x(y, z, p, d), \
                                                WOLFSSL_SUCCESS)

    FAIL_T1(wolfSSL_CTX_LoadCRL, ctx, validPath, pemType, monitor);

  #ifndef NO_WOLFSSL_CLIENT
    AssertNotNull(ctx = wolfSSL_CTX_new(wolfSSLv23_client_method()));
  #elif !defined(NO_WOLFSSL_SERVER)
    AssertNotNull(ctx = wolfSSL_CTX_new(wolfSSLv23_server_method()));
  #else
    return;
  #endif

    SUCC_T (wolfSSL_CTX_LoadCRL, ctx, validPath, pemType, monitor);
    SUCC_T (wolfSSL_CTX_LoadCRL, ctx, badPath, pemType, monitor);
    SUCC_T (wolfSSL_CTX_LoadCRL, ctx, badPath, derType, monitor);
    SUCC_T (wolfSSL_CTX_LoadCRL, ctx, badPath, rawType, monitor);

    wolfSSL_CTX_free(ctx);
    ctx = NULL;
#endif
}

static void test_SetTmpEC_DHE_Sz(void)
{
#if defined(HAVE_ECC) && !defined(NO_WOLFSSL_CLIENT)
    WOLFSSL_CTX *ctx;
    WOLFSSL *ssl;

    AssertNotNull(ctx = wolfSSL_CTX_new(wolfSSLv23_client_method()));
    AssertIntEQ(WOLFSSL_SUCCESS, wolfSSL_CTX_SetTmpEC_DHE_Sz(ctx, 32));
    AssertNotNull(ssl = wolfSSL_new(ctx));
    AssertIntEQ(WOLFSSL_SUCCESS, wolfSSL_SetTmpEC_DHE_Sz(ssl, 32));

    wolfSSL_free(ssl);
    wolfSSL_CTX_free(ctx);
#endif
}

#if !defined(NO_RSA) && !defined(NO_SHA) && !defined(NO_FILESYSTEM) && \
    !defined(NO_CERTS)
static int load_ca_into_cm(WOLFSSL_CERT_MANAGER* cm, char* certA)
{
    int ret;

    if ((ret = wolfSSL_CertManagerLoadCA(cm, certA, 0)) != WOLFSSL_SUCCESS) {
        printf("loading cert %s failed\n", certA);
        printf("Error: (%d): %s\n", ret, wolfSSL_ERR_reason_error_string(ret));
        return -1;
    }

    return 0;
}

static int verify_cert_with_cm(WOLFSSL_CERT_MANAGER* cm, char* certA)
{
    int ret;
    if ((ret = wolfSSL_CertManagerVerify(cm, certA, WOLFSSL_FILETYPE_PEM))
                                                         != WOLFSSL_SUCCESS) {
        printf("could not verify the cert: %s\n", certA);
        printf("Error: (%d): %s\n", ret, wolfSSL_ERR_reason_error_string(ret));
        return -1;
    } else {
        printf("successfully verified: %s\n", certA);
    }

    return 0;
}
#define LOAD_ONE_CA(a, b, c, d)                         \
                    do {                                \
                        a = load_ca_into_cm(c, d);      \
                        if (a != 0)                     \
                            return b;                   \
                        else                            \
                            b--;                        \
                    } while(0)

#define VERIFY_ONE_CERT(a, b, c, d)                     \
                    do {                                \
                        a = verify_cert_with_cm(c, d);  \
                        if (a != 0)                     \
                            return b;                   \
                        else                            \
                            b--;                        \
                    } while(0)

static int test_chainG(WOLFSSL_CERT_MANAGER* cm)
{
    int ret;
    int i = -1;
    /* Chain G is a valid chain per RFC 5280 section 4.2.1.9 */
    char chainGArr[9][50] = {"certs/ca-cert.pem",
                             "certs/test-pathlen/chainG-ICA7-pathlen100.pem",
                             "certs/test-pathlen/chainG-ICA6-pathlen10.pem",
                             "certs/test-pathlen/chainG-ICA5-pathlen20.pem",
                             "certs/test-pathlen/chainG-ICA4-pathlen5.pem",
                             "certs/test-pathlen/chainG-ICA3-pathlen99.pem",
                             "certs/test-pathlen/chainG-ICA2-pathlen1.pem",
                             "certs/test-pathlen/chainG-ICA1-pathlen0.pem",
                             "certs/test-pathlen/chainG-entity.pem"};

    LOAD_ONE_CA(ret, i, cm, chainGArr[0]); /* if failure, i = -1 here */
    LOAD_ONE_CA(ret, i, cm, chainGArr[1]); /* if failure, i = -2 here */
    LOAD_ONE_CA(ret, i, cm, chainGArr[2]); /* if failure, i = -3 here */
    LOAD_ONE_CA(ret, i, cm, chainGArr[3]); /* if failure, i = -4 here */
    LOAD_ONE_CA(ret, i, cm, chainGArr[4]); /* if failure, i = -5 here */
    LOAD_ONE_CA(ret, i, cm, chainGArr[5]); /* if failure, i = -6 here */
    LOAD_ONE_CA(ret, i, cm, chainGArr[6]); /* if failure, i = -7 here */
    LOAD_ONE_CA(ret, i, cm, chainGArr[7]); /* if failure, i = -8 here */
    VERIFY_ONE_CERT(ret, i, cm, chainGArr[1]); /* if failure, i = -9 here */
    VERIFY_ONE_CERT(ret, i, cm, chainGArr[2]); /* if failure, i = -10 here */
    VERIFY_ONE_CERT(ret, i, cm, chainGArr[3]); /* if failure, i = -11 here */
    VERIFY_ONE_CERT(ret, i, cm, chainGArr[4]); /* if failure, i = -12 here */
    VERIFY_ONE_CERT(ret, i, cm, chainGArr[5]); /* if failure, i = -13 here */
    VERIFY_ONE_CERT(ret, i, cm, chainGArr[6]); /* if failure, i = -14 here */
    VERIFY_ONE_CERT(ret, i, cm, chainGArr[7]); /* if failure, i = -15 here */
    VERIFY_ONE_CERT(ret, i, cm, chainGArr[8]); /* if failure, i = -16 here */

/* test validating the entity twice, should have no effect on pathLen since
 * entity/leaf cert */
    VERIFY_ONE_CERT(ret, i, cm, chainGArr[8]); /* if failure, i = -17 here */

    return ret;
}

static int test_chainH(WOLFSSL_CERT_MANAGER* cm)
{
    int ret;
    int i = -1;
    /* Chain H is NOT a valid chain per RFC5280 section 4.2.1.9:
     * ICA4-pathlen of 2 signing ICA3-pathlen of 2 (reduce max path len to 2)
     * ICA3-pathlen of 2 signing ICA2-pathlen of 2 (reduce max path len to 1)
     * ICA2-pathlen of 2 signing ICA1-pathlen of 0 (reduce max path len to 0)
     * ICA1-pathlen of 0 signing entity (pathlen is already 0, ERROR)
     * Test should successfully verify ICA4, ICA3, ICA2 and then fail on ICA1
     */
    char chainHArr[6][50] = {"certs/ca-cert.pem",
                             "certs/test-pathlen/chainH-ICA4-pathlen2.pem",
                             "certs/test-pathlen/chainH-ICA3-pathlen2.pem",
                             "certs/test-pathlen/chainH-ICA2-pathlen2.pem",
                             "certs/test-pathlen/chainH-ICA1-pathlen0.pem",
                             "certs/test-pathlen/chainH-entity.pem"};

    LOAD_ONE_CA(ret, i, cm, chainHArr[0]); /* if failure, i = -1 here */
    LOAD_ONE_CA(ret, i, cm, chainHArr[1]); /* if failure, i = -2 here */
    LOAD_ONE_CA(ret, i, cm, chainHArr[2]); /* if failure, i = -3 here */
    LOAD_ONE_CA(ret, i, cm, chainHArr[3]); /* if failure, i = -4 here */
    LOAD_ONE_CA(ret, i, cm, chainHArr[4]); /* if failure, i = -5 here */
    VERIFY_ONE_CERT(ret, i, cm, chainHArr[1]); /* if failure, i = -6 here */
    VERIFY_ONE_CERT(ret, i, cm, chainHArr[2]); /* if failure, i = -7 here */
    VERIFY_ONE_CERT(ret, i, cm, chainHArr[3]); /* if failure, i = -8 here */
    VERIFY_ONE_CERT(ret, i, cm, chainHArr[4]); /* if failure, i = -9 here */
    VERIFY_ONE_CERT(ret, i, cm, chainHArr[5]); /* if failure, i = -10 here */

    return ret;
}

static int test_chainI(WOLFSSL_CERT_MANAGER* cm)
{
    int ret;
    int i = -1;
    /* Chain I is a valid chain per RFC5280 section 4.2.1.9:
     * ICA3-pathlen of 2 signing ICA2 without a pathlen (reduce maxPathLen to 2)
     * ICA2-no_pathlen signing ICA1-no_pathlen (reduce maxPathLen to 1)
     * ICA1-no_pathlen signing entity (reduce maxPathLen to 0)
     * Test should successfully verify ICA4, ICA3, ICA2 and then fail on ICA1
     */
    char chainIArr[5][50] = {"certs/ca-cert.pem",
                             "certs/test-pathlen/chainI-ICA3-pathlen2.pem",
                             "certs/test-pathlen/chainI-ICA2-no_pathlen.pem",
                             "certs/test-pathlen/chainI-ICA1-no_pathlen.pem",
                             "certs/test-pathlen/chainI-entity.pem"};

    LOAD_ONE_CA(ret, i, cm, chainIArr[0]); /* if failure, i = -1 here */
    LOAD_ONE_CA(ret, i, cm, chainIArr[1]); /* if failure, i = -2 here */
    LOAD_ONE_CA(ret, i, cm, chainIArr[2]); /* if failure, i = -3 here */
    LOAD_ONE_CA(ret, i, cm, chainIArr[3]); /* if failure, i = -4 here */
    VERIFY_ONE_CERT(ret, i, cm, chainIArr[1]); /* if failure, i = -5 here */
    VERIFY_ONE_CERT(ret, i, cm, chainIArr[2]); /* if failure, i = -6 here */
    VERIFY_ONE_CERT(ret, i, cm, chainIArr[3]); /* if failure, i = -7 here */
    VERIFY_ONE_CERT(ret, i, cm, chainIArr[4]); /* if failure, i = -8 here */

    return ret;
}

static int test_chainJ(WOLFSSL_CERT_MANAGER* cm)
{
    int ret;
    int i = -1;
    /* Chain J is NOT a valid chain per RFC5280 section 4.2.1.9:
     * ICA4-pathlen of 2 signing ICA3 without a pathlen (reduce maxPathLen to 2)
     * ICA3-pathlen of 2 signing ICA2 without a pathlen (reduce maxPathLen to 1)
     * ICA2-no_pathlen signing ICA1-no_pathlen (reduce maxPathLen to 0)
     * ICA1-no_pathlen signing entity (ERROR, pathlen zero and non-leaf cert)
     */
    char chainJArr[6][50] = {"certs/ca-cert.pem",
                             "certs/test-pathlen/chainJ-ICA4-pathlen2.pem",
                             "certs/test-pathlen/chainJ-ICA3-no_pathlen.pem",
                             "certs/test-pathlen/chainJ-ICA2-no_pathlen.pem",
                             "certs/test-pathlen/chainJ-ICA1-no_pathlen.pem",
                             "certs/test-pathlen/chainJ-entity.pem"};

    LOAD_ONE_CA(ret, i, cm, chainJArr[0]); /* if failure, i = -1 here */
    LOAD_ONE_CA(ret, i, cm, chainJArr[1]); /* if failure, i = -2 here */
    LOAD_ONE_CA(ret, i, cm, chainJArr[2]); /* if failure, i = -3 here */
    LOAD_ONE_CA(ret, i, cm, chainJArr[3]); /* if failure, i = -4 here */
    LOAD_ONE_CA(ret, i, cm, chainJArr[4]); /* if failure, i = -5 here */
    VERIFY_ONE_CERT(ret, i, cm, chainJArr[1]); /* if failure, i = -6 here */
    VERIFY_ONE_CERT(ret, i, cm, chainJArr[2]); /* if failure, i = -7 here */
    VERIFY_ONE_CERT(ret, i, cm, chainJArr[3]); /* if failure, i = -8 here */
    VERIFY_ONE_CERT(ret, i, cm, chainJArr[4]); /* if failure, i = -9 here */
    VERIFY_ONE_CERT(ret, i, cm, chainJArr[5]); /* if failure, i = -10 here */

    return ret;
}

static int test_various_pathlen_chains(void)
{
    int ret;
    WOLFSSL_CERT_MANAGER* cm;

    /* Test chain G (large chain with varying pathLens) */
    if ((cm = wolfSSL_CertManagerNew()) == NULL) {
        printf("cert manager new failed\n");
        return -1;
    }

    AssertIntEQ(test_chainG(cm), 0);

    ret = wolfSSL_CertManagerUnloadCAs(cm);
    if (ret != WOLFSSL_SUCCESS)
        return -1;
    wolfSSL_CertManagerFree(cm);
    /* end test chain G */

    /* Test chain H (5 chain with same pathLens) */
    if ((cm = wolfSSL_CertManagerNew()) == NULL) {
        printf("cert manager new failed\n");
        return -1;
    }
    AssertIntLT(test_chainH(cm), 0);

    wolfSSL_CertManagerUnloadCAs(cm);
    wolfSSL_CertManagerFree(cm);
    if ((cm = wolfSSL_CertManagerNew()) == NULL) {
        printf("cert manager new failed\n");
        return -1;
    }

    ret = wolfSSL_CertManagerUnloadCAs(cm);
    if (ret != WOLFSSL_SUCCESS)
        return -1;
    wolfSSL_CertManagerFree(cm);
    /* end test chain H */

    /* Test chain I (only first ICA has pathLen set and it's set to 2,
     * followed by 2 ICA's, should pass) */
    if ((cm = wolfSSL_CertManagerNew()) == NULL) {
        printf("cert manager new failed\n");
        return -1;
    }
    AssertIntEQ(test_chainI(cm), 0);

    wolfSSL_CertManagerUnloadCAs(cm);
    wolfSSL_CertManagerFree(cm);
    if ((cm = wolfSSL_CertManagerNew()) == NULL) {
        printf("cert manager new failed\n");
        return -1;
    }

    ret = wolfSSL_CertManagerUnloadCAs(cm);
    if (ret != WOLFSSL_SUCCESS)
        return -1;
    wolfSSL_CertManagerFree(cm);

    /* Test chain J (Again only first ICA has pathLen set and it's set to 2,
     * this time followed by 3 ICA's, should fail */
    if ((cm = wolfSSL_CertManagerNew()) == NULL) {
        printf("cert manager new failed\n");
        return -1;
    }
    AssertIntLT(test_chainJ(cm), 0);

    wolfSSL_CertManagerUnloadCAs(cm);
    wolfSSL_CertManagerFree(cm);
    if ((cm = wolfSSL_CertManagerNew()) == NULL) {
        printf("cert manager new failed\n");
        return -1;
    }

    ret = wolfSSL_CertManagerUnloadCAs(cm);
    wolfSSL_CertManagerFree(cm);

    return ret;
}
#endif /* !NO_RSA && !NO_SHA && !NO_FILESYSTEM && !NO_CERTS */

/*----------------------------------------------------------------------------*
 | Main
 *----------------------------------------------------------------------------*/

void ApiTest(void)
{
    printf("\n-----------------Porting tests------------------\n");
    AssertTrue(test_fileAccess());

    printf(" Begin API Tests\n");
    AssertIntEQ(test_wolfSSL_Init(), WOLFSSL_SUCCESS);
    /* wolfcrypt initialization tests */
    test_wolfSSL_Method_Allocators();
#ifndef NO_WOLFSSL_SERVER
    test_wolfSSL_CTX_new(wolfSSLv23_server_method());
#endif
#if (!defined(NO_WOLFSSL_CLIENT) || !defined(NO_WOLFSSL_SERVER)) && \
(!defined(NO_RSA) || defined(HAVE_ECC))
    test_for_double_Free();
#endif
    test_wolfSSL_CTX_use_certificate_file();
    AssertIntEQ(test_wolfSSL_CTX_use_certificate_buffer(), WOLFSSL_SUCCESS);
    test_wolfSSL_CTX_use_PrivateKey_file();
    test_wolfSSL_CTX_load_verify_locations();
    test_wolfSSL_CertManagerLoadCABuffer();
    test_wolfSSL_CertManagerGetCerts();
    test_wolfSSL_CertManagerCRL();
    test_wolfSSL_CTX_load_verify_locations_ex();
    test_wolfSSL_CTX_load_verify_buffer_ex();
    test_wolfSSL_CTX_load_verify_chain_buffer_format();
    test_wolfSSL_CTX_use_certificate_chain_file_format();
    test_wolfSSL_CTX_trust_peer_cert();
    test_wolfSSL_CTX_SetTmpDH_file();
    test_wolfSSL_CTX_SetTmpDH_buffer();
    test_wolfSSL_CTX_SetMinMaxDhKey_Sz();
    test_wolfSSL_CTX_der_load_verify_locations();
    test_wolfSSL_CTX_enable_disable();
    test_server_wolfSSL_new();
    test_client_wolfSSL_new();
    test_wolfSSL_SetTmpDH_file();
    test_wolfSSL_SetTmpDH_buffer();
    test_wolfSSL_SetMinMaxDhKey_Sz();
    test_SetTmpEC_DHE_Sz();
#if !defined(NO_WOLFSSL_CLIENT) && !defined(NO_WOLFSSL_SERVER) && \
    defined(HAVE_IO_TESTS_DEPENDENCIES)
    test_wolfSSL_read_write();
#if defined(OPENSSL_EXTRA) && !defined(NO_SESSION_CACHE) && !defined(WOLFSSL_TLS13)
    test_wolfSSL_reuse_WOLFSSLobj();
#endif
    test_wolfSSL_dtls_export();
#endif
    AssertIntEQ(test_wolfSSL_SetMinVersion(), WOLFSSL_SUCCESS);
    AssertIntEQ(test_wolfSSL_CTX_SetMinVersion(), WOLFSSL_SUCCESS);

    /* TLS extensions tests */
#ifdef HAVE_IO_TESTS_DEPENDENCIES
    test_wolfSSL_UseSNI();
#endif
    test_wolfSSL_UseTrustedCA();
    test_wolfSSL_UseMaxFragment();
    test_wolfSSL_UseTruncatedHMAC();
    test_wolfSSL_UseSupportedCurve();
    test_wolfSSL_UseALPN();
    test_wolfSSL_DisableExtendedMasterSecret();
    test_wolfSSL_wolfSSL_UseSecureRenegotiation();

    /* X509 tests */
    test_wolfSSL_X509_NAME_get_entry();
    test_wolfSSL_PKCS12();
    test_wolfSSL_no_password_cb();
    test_wolfSSL_PKCS8();
    test_wolfSSL_PKCS5();
    test_wolfSSL_URI();
    test_wolfSSL_TBS();
    test_wolfSSL_X509_verify();

    test_wc_PemToDer();
    test_wc_AllocDer();
    test_wc_CertPemToDer();
    test_wc_PubKeyPemToDer();
    test_wc_PemPubKeyToDer();

    /*OCSP Stapling. */
    AssertIntEQ(test_wolfSSL_UseOCSPStapling(), WOLFSSL_SUCCESS);
    AssertIntEQ(test_wolfSSL_UseOCSPStaplingV2(), WOLFSSL_SUCCESS);

    /* Multicast */
    test_wolfSSL_mcast();

    /* compatibility tests */
    test_wolfSSL_X509_NAME();
    test_wolfSSL_X509_INFO();
    test_wolfSSL_X509_subject_name_hash();
    test_wolfSSL_DES();
    test_wolfSSL_certs();
    test_wolfSSL_ASN1_TIME_print();
    test_wolfSSL_ASN1_UTCTIME_print();
    test_wolfSSL_ASN1_GENERALIZEDTIME_free();
    test_wolfSSL_private_keys();
    test_wolfSSL_PEM_PrivateKey();
    test_wolfSSL_PEM_bio_RSAKey();
    test_wolfSSL_PEM_bio_DSAKey();
    test_wolfSSL_PEM_bio_ECKey();
    test_wolfSSL_PEM_RSAPrivateKey();
    test_wolfSSL_PEM_PUBKEY();
    test_wolfSSL_tmp_dh();
    test_wolfSSL_ctrl();
    test_wolfSSL_EVP_MD_size();
    test_wolfSSL_EVP_Digest();
    test_wolfSSL_EVP_PKEY_new_mac_key();
    test_wolfSSL_EVP_MD_hmac_signing();
    test_wolfSSL_EVP_MD_rsa_signing();
    test_wolfSSL_EVP_MD_ecc_signing();
    test_wolfSSL_CTX_add_extra_chain_cert();
#if !defined(NO_WOLFSSL_CLIENT) && !defined(NO_WOLFSSL_SERVER)
    test_wolfSSL_ERR_peek_last_error_line();
#endif
    test_wolfSSL_set_options();
    test_wolfSSL_sk_SSL_CIPHER();
    test_wolfSSL_X509_STORE_CTX();
    test_wolfSSL_X509_STORE_CTX_get0_current_issuer();
    test_wolfSSL_msgCb();
    test_wolfSSL_either_side();
    test_wolfSSL_DTLS_either_side();
    test_generate_cookie();
    test_wolfSSL_X509_STORE_set_flags();
    test_wolfSSL_X509_LOOKUP_load_file();
    test_wolfSSL_X509_NID();
    test_wolfSSL_X509_STORE_CTX_set_time();
    test_wolfSSL_get0_param();
    test_wolfSSL_X509_VERIFY_PARAM_set1_host();
    test_wolfSSL_X509_STORE_CTX_get0_store();
    test_wolfSSL_X509_STORE();
    test_wolfSSL_X509_STORE_load_locations();
    test_wolfSSL_BN();
    test_wolfSSL_PEM_read_bio();
    test_wolfSSL_BIO();
    test_wolfSSL_ASN1_STRING();
    test_wolfSSL_ASN1_BIT_STRING();
    test_wolfSSL_X509();
    test_wolfSSL_X509_VERIFY_PARAM();
    test_wolfSSL_X509_sign();
    test_wolfSSL_X509_get0_tbs_sigalg();
    test_wolfSSL_X509_ALGOR_get0();
    test_wolfSSL_X509_get_X509_PUBKEY();
    test_wolfSSL_X509_PUBKEY_get0_param();
    test_wolfSSL_RAND();
    test_wolfSSL_BUF();
    test_wolfSSL_set_tlsext_status_type();
    test_wolfSSL_ASN1_TIME_adj();
    test_wolfSSL_X509_cmp_time();
    test_wolfSSL_X509_time_adj();
    test_wolfSSL_CTX_set_client_CA_list();
    test_wolfSSL_CTX_add_client_CA();
    test_wolfSSL_CTX_set_srp_username();
    test_wolfSSL_CTX_set_srp_password();
    test_wolfSSL_pseudo_rand();
    test_wolfSSL_PKCS8_Compat();
    test_wolfSSL_PKCS8_d2i();
    test_wolfSSL_ERR_put_error();
    test_wolfSSL_ERR_print_errors();
    test_wolfSSL_HMAC();
    test_wolfSSL_OBJ();
    test_wolfSSL_i2a_ASN1_OBJECT();
    test_wolfSSL_OBJ_cmp();
    test_wolfSSL_OBJ_txt2nid();
    test_wolfSSL_OBJ_txt2obj();
    test_wolfSSL_X509_NAME_ENTRY();
    test_wolfSSL_X509_set_name();
    test_wolfSSL_X509_set_notAfter();
    test_wolfSSL_X509_set_notBefore();
    test_wolfSSL_X509_set_version();
    test_wolfSSL_BIO_gets();
    test_wolfSSL_BIO_puts();
    test_wolfSSL_d2i_PUBKEY();
    test_wolfSSL_BIO_write();
    test_wolfSSL_BIO_printf();
    test_wolfSSL_SESSION();
    test_wolfSSL_DES_ecb_encrypt();
    test_wolfSSL_sk_GENERAL_NAME();
    test_wolfSSL_MD4();
    test_wolfSSL_RSA();
    test_wolfSSL_RSA_DER();
    test_wolfSSL_RSA_get0_key();
    test_wolfSSL_RSA_meth();
    test_wolfSSL_verify_depth();
    test_wolfSSL_HMAC_CTX();
    test_wolfSSL_msg_callback();
    test_wolfSSL_SHA();
    test_wolfSSL_DH_1536_prime();
    test_wolfSSL_PEM_write_DHparams();
    test_wolfSSL_AES_ecb_encrypt();
    test_wolfSSL_SHA256();
    test_wolfSSL_X509_get_serialNumber();
    test_wolfSSL_X509_CRL();
    test_wolfSSL_PEM_read_X509();
    test_wolfSSL_PEM_read();
    test_wolfSSL_PEM_X509_INFO_read_bio();
    test_wolfSSL_PEM_read_bio_ECPKParameters();
    test_wolfSSL_X509_NAME_ENTRY_get_object();
    test_wolfSSL_OpenSSL_add_all_algorithms();
    test_wolfSSL_ASN1_STRING_print_ex();
    test_wolfSSL_ASN1_TIME_to_generalizedtime();
    test_wolfSSL_ASN1_INTEGER_set();
    test_wolfSSL_i2c_ASN1_INTEGER();
    test_wolfSSL_X509_check_ca();
    test_wolfSSL_DC_cert();
    test_wolfSSL_DES_ncbc();
    test_wolfSSL_AES_cbc_encrypt();
    test_wolfssl_EVP_aes_gcm();
    test_wolfSSL_PKEY_up_ref();
    test_wolfSSL_i2d_PrivateKey();
    test_wolfSSL_OCSP_get0_info();

#if defined(WOLFSSL_QT)
    printf("\n----------------Qt Unit Tests-------------------\n");
    //carie
    test_wolfSSL_X509_PUBKEY_get();
    test_wolfSSL_sk_CIPHER_description();
    test_wolfSSL_get_ciphers_compat();
    test_wolfSSL_d2i_DHparams();
    test_wolfSSL_i2d_DHparams();
    test_wolfSSL_ASN1_STRING_to_UTF8();
    test_wolfSSL_EC_KEY_dup();
    test_wolfSSL_EVP_PKEY_set1_get1_DSA();
    test_wolfSSL_EVP_PKEY_set1_get1_EC_KEY();
    test_wolfSSL_EVP_PKEY_set1_get1_DH();
    test_wolfSSL_CTX_ctrl();
    test_wolfSSL_DH_check();
    test_wolfSSL_EC_get_builtin_curves();
    test_wolfSSL_EVP_PKEY_assign();
    test_wolfSSL_OBJ_ln();
    test_wolfSSL_OBJ_sn();

    printf("\n-------------End Of Qt Unit Tests---------------\n");

#endif /* WOLFSSL_QT */

#if (defined(OPENSSL_ALL) || defined(WOLFSSL_ASIO)) && !defined(NO_RSA)
    AssertIntEQ(test_wolfSSL_CTX_use_certificate_ASN1(), WOLFSSL_SUCCESS);
    test_wolfSSL_d2i_PrivateKeys_bio();
#endif /* OPENSSL_ALL || WOLFSSL_ASIO */

    test_wolfSSL_X509_CA_num();
    test_wolfSSL_X509_get_version();
    test_wolfSSL_X509_print();
    test_wolfSSL_BIO_get_len();
    test_wolfSSL_RSA_verify();
    test_wolfSSL_X509V3_EXT_get();
    test_wolfSSL_X509V3_EXT_d2i();
    test_wolfSSL_X509_get_ext();
    test_wolfSSL_X509_get_ext_by_NID();
    test_wolfSSL_X509_get_ext_count();
    test_wolfSSL_X509_EXTENSION_new();
    test_wolfSSL_X509_EXTENSION_get_object();
    test_wolfSSL_X509_EXTENSION_get_data();
    test_wolfSSL_X509_EXTENSION_get_critical();
    test_wolfSSL_X509V3_EXT_print();
    test_wolfSSL_X509_cmp();
    test_wolfSSL_RSA_print();
    test_wolfSSL_ASN1_STRING_print();
    test_openssl_generate_key_and_cert();

    test_wolfSSL_EC_get_builtin_curves();

    /* test the no op functions for compatibility */
    test_no_op_functions();

    /* OpenSSL EVP_PKEY API tests */
    test_EVP_PKEY_rsa();
    test_wolfSSL_EVP_PKEY_encrypt();
    test_wolfSSL_EVP_PKEY_sign();
    test_EVP_PKEY_ec();
    test_EVP_PKEY_cmp();
    /* OpenSSL error API tests */
    test_ERR_load_crypto_strings();
    /* OpenSSL sk_X509 API test */
    test_sk_X509();
    /* OpenSSL X509 API test */
    test_X509_get_signature_nid();
    /* OpenSSL X509 REQ API test */
    test_X509_REQ();
    /* OpenSSL PKCS7 API test */
    test_wolfssl_PKCS7();
    test_wolfSSL_PKCS7_SIGNED_new();
    test_wolfSSL_PEM_write_bio_PKCS7();

    /* wolfCrypt ASN tests */
    test_wc_GetPkcs8TraditionalOffset();
    test_wc_SetSubjectRaw();
    test_wc_GetSubjectRaw();
    test_wc_SetIssuerRaw();
    test_wc_SetIssueBuffer();
    test_wc_SetSubjectKeyId();
    test_wc_SetSubject();
    test_CheckCertSignature();

    /* wolfCrypt ECC tests */
    test_wc_ecc_get_curve_size_from_name();
    test_wc_ecc_get_curve_id_from_name();
    test_wc_ecc_get_curve_id_from_params();
#ifdef WOLFSSL_TLS13
    /* TLS v1.3 API tests */
    test_tls13_apis();
#endif

#ifndef NO_CERTS
    /* Bad certificate signature tests */
    AssertIntEQ(test_EccSigFailure_cm(), ASN_SIG_CONFIRM_E);
    AssertIntEQ(test_RsaSigFailure_cm(), ASN_SIG_CONFIRM_E);
#endif /* NO_CERTS */

#ifdef HAVE_PK_CALLBACKS
    /* public key callback tests */
    test_DhCallbacks();
#endif

    /*wolfcrypt */
    printf("\n-----------------wolfcrypt unit tests------------------\n");
    AssertFalse(test_wolfCrypt_Init());
    AssertFalse(test_wc_InitMd5());
    AssertFalse(test_wc_Md5Update());
    AssertFalse(test_wc_Md5Final());
    AssertFalse(test_wc_InitSha());
    AssertFalse(test_wc_ShaUpdate());
    AssertFalse(test_wc_ShaFinal());
    AssertFalse(test_wc_InitSha256());
    AssertFalse(test_wc_Sha256Update());
    AssertFalse(test_wc_Sha256Final());
    AssertFalse(test_wc_InitSha512());
    AssertFalse(test_wc_Sha512Update());
    AssertFalse(test_wc_Sha512Final());
    AssertFalse(test_wc_InitSha384());
    AssertFalse(test_wc_Sha384Update());
    AssertFalse(test_wc_Sha384Final());
    AssertFalse(test_wc_InitSha224());
    AssertFalse(test_wc_Sha224Update());
    AssertFalse(test_wc_Sha224Final());
    AssertFalse(test_wc_InitBlake2b());
    AssertFalse(test_wc_InitRipeMd());
    AssertFalse(test_wc_RipeMdUpdate());
    AssertFalse(test_wc_RipeMdFinal());

    AssertIntEQ(test_wc_InitSha3(), 0);
    AssertIntEQ(testing_wc_Sha3_Update(), 0);
    AssertIntEQ(test_wc_Sha3_224_Final(), 0);
    AssertIntEQ(test_wc_Sha3_256_Final(), 0);
    AssertIntEQ(test_wc_Sha3_384_Final(), 0);
    AssertIntEQ(test_wc_Sha3_512_Final(), 0);
    AssertIntEQ(test_wc_Sha3_224_Copy(), 0);
    AssertIntEQ(test_wc_Sha3_256_Copy(), 0);
    AssertIntEQ(test_wc_Sha3_384_Copy(), 0);
    AssertIntEQ(test_wc_Sha3_512_Copy(), 0);

    AssertFalse(test_wc_Md5HmacSetKey());
    AssertFalse(test_wc_Md5HmacUpdate());
    AssertFalse(test_wc_Md5HmacFinal());
    AssertFalse(test_wc_ShaHmacSetKey());
    AssertFalse(test_wc_ShaHmacUpdate());
    AssertFalse(test_wc_ShaHmacFinal());
    AssertFalse(test_wc_Sha224HmacSetKey());
    AssertFalse(test_wc_Sha224HmacUpdate());
    AssertFalse(test_wc_Sha224HmacFinal());
    AssertFalse(test_wc_Sha256HmacSetKey());
    AssertFalse(test_wc_Sha256HmacUpdate());
    AssertFalse(test_wc_Sha256HmacFinal());
    AssertFalse(test_wc_Sha384HmacSetKey());
    AssertFalse(test_wc_Sha384HmacUpdate());
    AssertFalse(test_wc_Sha384HmacFinal());

    AssertIntEQ(test_wc_HashInit(), 0);

    AssertIntEQ(test_wc_InitCmac(), 0);
    AssertIntEQ(test_wc_CmacUpdate(), 0);
    AssertIntEQ(test_wc_CmacFinal(), 0);
    AssertIntEQ(test_wc_AesCmacGenerate(), 0);

    AssertIntEQ(test_wc_Des3_SetIV(), 0);
    AssertIntEQ(test_wc_Des3_SetKey(), 0);
    AssertIntEQ(test_wc_Des3_CbcEncryptDecrypt(), 0);
    AssertIntEQ(test_wc_Des3_CbcEncryptDecryptWithKey(), 0);
    AssertIntEQ(test_wc_IdeaSetKey(), 0);
    AssertIntEQ(test_wc_IdeaSetIV(), 0);
    AssertIntEQ(test_wc_IdeaCipher(), 0);
    AssertIntEQ(test_wc_IdeaCbcEncyptDecrypt(), 0);
    AssertIntEQ(test_wc_Chacha_SetKey(), 0);
    AssertIntEQ(test_wc_Chacha_Process(), 0);
    AssertIntEQ(test_wc_ChaCha20Poly1305_aead(), 0);
    AssertIntEQ(test_wc_Poly1305SetKey(), 0);

    AssertIntEQ(test_wc_CamelliaSetKey(), 0);
    AssertIntEQ(test_wc_CamelliaSetIV(), 0);
    AssertIntEQ(test_wc_CamelliaEncryptDecryptDirect(), 0);
    AssertIntEQ(test_wc_CamelliaCbcEncryptDecrypt(), 0);


    AssertIntEQ(test_wc_RabbitSetKey(), 0);
    AssertIntEQ(test_wc_RabbitProcess(), 0);

    AssertIntEQ(test_wc_Arc4SetKey(), 0);
    AssertIntEQ(test_wc_Arc4Process(), 0);

    AssertIntEQ(test_wc_AesSetKey(), 0);
    AssertIntEQ(test_wc_AesSetIV(), 0);
    AssertIntEQ(test_wc_AesCbcEncryptDecrypt(), 0);
    AssertIntEQ(test_wc_AesCtrEncryptDecrypt(), 0);
    AssertIntEQ(test_wc_AesGcmSetKey(), 0);
    AssertIntEQ(test_wc_AesGcmEncryptDecrypt(), 0);
    AssertIntEQ(test_wc_GmacSetKey(), 0);
    AssertIntEQ(test_wc_GmacUpdate(), 0);
    AssertIntEQ(test_wc_InitRsaKey(), 0);
    AssertIntEQ(test_wc_RsaPrivateKeyDecode(), 0);
    AssertIntEQ(test_wc_RsaPublicKeyDecode(), 0);
    AssertIntEQ(test_wc_RsaPublicKeyDecodeRaw(), 0);
    AssertIntEQ(test_wc_MakeRsaKey(), 0);
    AssertIntEQ(test_wc_SetKeyUsage (), 0);


    AssertIntEQ(test_wc_RsaKeyToDer(), 0);
    AssertIntEQ(test_wc_RsaKeyToPublicDer(), 0);
    AssertIntEQ(test_wc_RsaPublicEncryptDecrypt(), 0);
    AssertIntEQ(test_wc_RsaPublicEncryptDecrypt_ex(), 0);
    AssertIntEQ(test_wc_RsaEncryptSize(), 0);
    AssertIntEQ(test_wc_RsaSSL_SignVerify(), 0);
    AssertIntEQ(test_wc_RsaFlattenPublicKey(), 0);
    AssertIntEQ(test_RsaDecryptBoundsCheck(), 0);
    AssertIntEQ(test_wc_AesCcmSetKey(), 0);
    AssertIntEQ(test_wc_AesCcmEncryptDecrypt(), 0);
    AssertIntEQ(test_wc_Hc128_SetKey(), 0);
    AssertIntEQ(test_wc_Hc128_Process(), 0);
    AssertIntEQ(test_wc_InitDsaKey(), 0);
    AssertIntEQ(test_wc_DsaSignVerify(), 0);
    AssertIntEQ(test_wc_DsaPublicPrivateKeyDecode(), 0);
    AssertIntEQ(test_wc_MakeDsaKey(), 0);
    AssertIntEQ(test_wc_DsaKeyToDer(), 0);
    AssertIntEQ(test_wc_DsaKeyToPublicDer(), 0);
    AssertIntEQ(test_wc_DsaImportParamsRaw(), 0);
    AssertIntEQ(test_wc_DsaImportParamsRawCheck(), 0);
    AssertIntEQ(test_wc_DsaExportParamsRaw(), 0);
    AssertIntEQ(test_wc_DsaExportKeyRaw(), 0);
    AssertIntEQ(test_wc_SignatureGetSize_ecc(), 0);
    AssertIntEQ(test_wc_SignatureGetSize_rsa(), 0);
    wolfCrypt_Cleanup();

#ifdef OPENSSL_EXTRA
    /*wolfSSL_EVP_get_cipherbynid test*/
    test_wolfSSL_EVP_get_cipherbynid();
    test_wolfSSL_EVP_CIPHER_CTX();
    test_wolfSSL_EC();
    test_wolfSSL_ECDSA_SIG();
#endif
#if defined(OPENSSL_EXTRA) && defined(HAVE_ECC) && \
    !defined(HAVE_SELFTEST) && \
    !(defined(HAVE_FIPS) || defined(HAVE_FIPS_VERSION))
    test_wc_ecc_get_curve_id_from_dp_params();
#endif

#ifdef HAVE_HASHDRBG
    #ifdef TEST_RESEED_INTERVAL
    AssertIntEQ(test_wc_RNG_GenerateBlock_Reseed(), 0);
    #endif
    AssertIntEQ(test_wc_RNG_GenerateBlock(), 0);
#endif

    AssertIntEQ(test_wc_ed25519_make_key(), 0);
    AssertIntEQ(test_wc_ed25519_init(), 0);
    AssertIntEQ(test_wc_ed25519_sign_msg(), 0);
    AssertIntEQ(test_wc_ed25519_import_public(), 0);
    AssertIntEQ(test_wc_ed25519_import_private_key(), 0);
    AssertIntEQ(test_wc_ed25519_export(), 0);
    AssertIntEQ(test_wc_ed25519_size(), 0);
    AssertIntEQ(test_wc_ed25519_exportKey(), 0);
    AssertIntEQ(test_wc_Ed25519PublicKeyToDer(), 0);
    AssertIntEQ(test_wc_curve25519_init(), 0);
    AssertIntEQ(test_wc_curve25519_size (), 0);
    AssertIntEQ(test_wc_ecc_make_key(), 0);
    AssertIntEQ(test_wc_ecc_init(), 0);
    AssertIntEQ(test_wc_ecc_check_key(), 0);
    AssertIntEQ(test_wc_ecc_size(), 0);
    test_wc_ecc_params();
    AssertIntEQ(test_wc_ecc_signVerify_hash(), 0);
    AssertIntEQ(test_wc_ecc_shared_secret(), 0);
    AssertIntEQ(test_wc_ecc_export_x963(), 0);
    AssertIntEQ(test_wc_ecc_export_x963_ex(), 0);
    AssertIntEQ(test_wc_ecc_import_x963(), 0);
    AssertIntEQ(ecc_import_private_key(), 0);
    AssertIntEQ(test_wc_ecc_export_private_only(), 0);
    AssertIntEQ(test_wc_ecc_rs_to_sig(), 0);
    AssertIntEQ(test_wc_ecc_import_raw(), 0);
    AssertIntEQ(test_wc_ecc_sig_size(), 0);
    AssertIntEQ(test_wc_ecc_ctx_new(), 0);
    AssertIntEQ(test_wc_ecc_ctx_reset(), 0);
    AssertIntEQ(test_wc_ecc_ctx_set_peer_salt(), 0);
    AssertIntEQ(test_wc_ecc_ctx_set_info(), 0);
    AssertIntEQ(test_wc_ecc_encryptDecrypt(), 0);
    AssertIntEQ(test_wc_ecc_del_point(), 0);
    AssertIntEQ(test_wc_ecc_pointFns(), 0);
    AssertIntEQ(test_wc_ecc_shared_secret_ssh(), 0);
    AssertIntEQ(test_wc_ecc_verify_hash_ex(), 0);
    AssertIntEQ(test_wc_ecc_mulmod(), 0);
    AssertIntEQ(test_wc_ecc_is_valid_idx(), 0);

    test_wc_PKCS7_New();
    test_wc_PKCS7_Init();
    test_wc_PKCS7_InitWithCert();
    test_wc_PKCS7_EncodeData();
    test_wc_PKCS7_EncodeSignedData();
    test_wc_PKCS7_EncodeSignedData_ex();
    test_wc_PKCS7_VerifySignedData();
    test_wc_PKCS7_EncodeDecodeEnvelopedData();
    test_wc_PKCS7_EncodeEncryptedData();
    test_wc_PKCS7_Degenerate();
    test_wc_PKCS7_BER();
    test_PKCS7_signed_enveloped();

    test_wc_i2d_PKCS12();

    test_wolfSSL_CTX_LoadCRL();

    AssertIntEQ(test_ForceZero(), 0);

    AssertIntEQ(test_wolfSSL_Cleanup(), WOLFSSL_SUCCESS);

#if !defined(NO_RSA) && !defined(NO_SHA) && !defined(NO_FILESYSTEM) && \
    !defined(NO_CERTS)
    AssertIntEQ(test_various_pathlen_chains(), WOLFSSL_SUCCESS);
#endif

    /* If at some point a stub get implemented this test should fail indicating
     * a need to implement a new test case
     */
    test_stubs_are_stubs();
#if defined(HAVE_ECC) && defined(FP_ECC) && defined(HAVE_THREAD_LS) \
                      && (defined(NO_MAIN_DRIVER) || defined(HAVE_STACK_SIZE))
    wc_ecc_fp_free();  /* free per thread cache */
#endif
    wolfSSL_Cleanup();

    printf(" End API Tests\n");

}
