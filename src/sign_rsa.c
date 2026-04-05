/* sign_rsa.c */
#include <stdio.h>
#include <stdlib.h>
#include <openssl/sha.h>
#include <openssl/pem.h>
#include <openssl/evp.h>

static int sha256_file(const char *filename, unsigned char out[SHA256_DIGEST_LENGTH]) {
    FILE *fp = fopen(filename, "rb");
    if (!fp) {
        return 0;
    }

    SHA256_CTX ctx;
    if (SHA256_Init(&ctx) != 1) {
        fclose(fp);
        return 0;
    }

    unsigned char buf[4096];
    size_t n = 0;

    while ((n = fread(buf, 1, sizeof(buf), fp)) > 0) {
        if (SHA256_Update(&ctx, buf, n) != 1) {
            fclose(fp);
            return 0;
        }
    }

    fclose(fp);
    return SHA256_Final(out, &ctx) == 1;
}

static int signRSA(const char *pemFile, const char *imageFile, const char *sigFile) {
    unsigned char digest[SHA256_DIGEST_LENGTH];
    if (!sha256_file(imageFile, digest)) {
        fprintf(stderr, "ERROR: Failed to hash image file: %s\n", imageFile);
        return 0;
    }

    FILE *fptr = fopen(pemFile, "r");
    if (!fptr) {
        fprintf(stderr, "ERROR: Failed to open private key: %s\n", pemFile);
        return 0;
    }

    EVP_PKEY *skey = PEM_read_PrivateKey(fptr, NULL, NULL, NULL);
    fclose(fptr);
    if (!skey) {
        fprintf(stderr, "ERROR: Failed to read private key.\n");
        return 0;
    }

    EVP_MD_CTX *ctx = EVP_MD_CTX_new();
    if (!ctx) {
        EVP_PKEY_free(skey);
        fprintf(stderr, "ERROR: Failed to allocate signing context.\n");
        return 0;
    }

    if (EVP_DigestSignInit(ctx, NULL, NULL, NULL, skey) != 1) {
        EVP_MD_CTX_free(ctx);
        EVP_PKEY_free(skey);
        fprintf(stderr, "ERROR: EVP_DigestSignInit failed.\n");
        return 0;
    }

    size_t sigLen = 0;
    if (EVP_DigestSign(ctx, NULL, &sigLen, digest, SHA256_DIGEST_LENGTH) != 1) {
        EVP_MD_CTX_free(ctx);
        EVP_PKEY_free(skey);
        fprintf(stderr, "ERROR: Failed to determine signature length.\n");
        return 0;
    }

    unsigned char *sig = (unsigned char *)malloc(sigLen);
    if (!sig) {
        EVP_MD_CTX_free(ctx);
        EVP_PKEY_free(skey);
        fprintf(stderr, "ERROR: Failed to allocate signature buffer.\n");
        return 0;
    }

    if (EVP_DigestSign(ctx, sig, &sigLen, digest, SHA256_DIGEST_LENGTH) != 1) {
        free(sig);
        EVP_MD_CTX_free(ctx);
        EVP_PKEY_free(skey);
        fprintf(stderr, "ERROR: Failed to generate signature.\n");
        return 0;
    }

    fptr = fopen(sigFile, "wb");
    if (!fptr) {
        free(sig);
        EVP_MD_CTX_free(ctx);
        EVP_PKEY_free(skey);
        fprintf(stderr, "ERROR: Failed to open signature output file: %s\n", sigFile);
        return 0;
    }

    fwrite(sig, 1, sigLen, fptr);
    fclose(fptr);

    free(sig);
    EVP_MD_CTX_free(ctx);
    EVP_PKEY_free(skey);
    return 1;
}

int main(int argc, char **argv) {
    if (argc != 4) {
        fprintf(stderr, "usage: %s <private_key.pem> <image.bin> <signature.bin>\n", argv[0]);
        return EXIT_FAILURE;
    }

    if (!signRSA(argv[1], argv[2], argv[3])) {
        return EXIT_FAILURE;
    }

    printf("Signature written to %s\n", argv[3]);
    return EXIT_SUCCESS;
}
