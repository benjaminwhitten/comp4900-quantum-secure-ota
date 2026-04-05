#include <stdio.h>
#include <stdlib.h>
#include <openssl/sha.h>
#include <oqs/oqs.h>

#ifndef MLDSA_ALG
#define MLDSA_ALG OQS_SIG_alg_ml_dsa_44
#endif

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

static int read_file_all(const char *path, unsigned char **buf, size_t *len) {
    FILE *fp = fopen(path, "rb");
    if (!fp) {
        return 0;
    }

    if (fseek(fp, 0, SEEK_END) != 0) {
        fclose(fp);
        return 0;
    }

    long size = ftell(fp);
    if (size <= 0) {
        fclose(fp);
        return 0;
    }

    rewind(fp);

    *buf = (unsigned char *)malloc((size_t)size);
    if (*buf == NULL) {
        fclose(fp);
        return 0;
    }

    size_t got = fread(*buf, 1, (size_t)size, fp);
    fclose(fp);

    if (got != (size_t)size) {
        free(*buf);
        *buf = NULL;
        return 0;
    }

    *len = got;
    return 1;
}

static int validateMLDSA(const char *pubFile, const char *imageFile, const char *sigFile) {
    unsigned char digest[SHA256_DIGEST_LENGTH];
    if (!sha256_file(imageFile, digest)) {
        fprintf(stderr, "ERROR: failed to hash image file: %s\n", imageFile);
        return -1;
    }

    unsigned char *pkey = NULL;
    size_t pkeyLen = 0;
    if (!read_file_all(pubFile, &pkey, &pkeyLen)) {
        fprintf(stderr, "ERROR: failed to read public key file: %s\n", pubFile);
        return -1;
    }

    unsigned char *sigBuf = NULL;
    size_t sigLen = 0;
    if (!read_file_all(sigFile, &sigBuf, &sigLen)) {
        free(pkey);
        fprintf(stderr, "ERROR: failed to read signature file: %s\n", sigFile);
        return -1;
    }

    OQS_SIG *sig = OQS_SIG_new(MLDSA_ALG);
    if (sig == NULL) {
        free(pkey);
        free(sigBuf);
        fprintf(stderr, "ERROR: failed to create ML-DSA object\n");
        return -1;
    }

    if (pkeyLen != sig->length_public_key) {
        fprintf(stderr, "ERROR: public key size mismatch (got %zu, expected %zu)\n",
                pkeyLen, sig->length_public_key);
        OQS_SIG_free(sig);
        free(pkey);
        free(sigBuf);
        return -1;
    }

    OQS_STATUS rc = OQS_SIG_verify(sig, digest, SHA256_DIGEST_LENGTH, sigBuf, sigLen, pkey);

    OQS_SIG_free(sig);
    free(pkey);
    free(sigBuf);

    if (rc == OQS_SUCCESS) {
        return 1;
    }
    return 0;
}

int main(int argc, char **argv) {
    if (argc != 4) {
        fprintf(stderr, "usage: %s <public_key.bin> <image.bin> <signature.bin>\n", argv[0]);
        return EXIT_FAILURE;
    }

    int rc = validateMLDSA(argv[1], argv[2], argv[3]);
    if (rc == 1) {
        printf("VALID\n");
        return EXIT_SUCCESS;
    }
    if (rc == 0) {
        printf("INVALID\n");
        return EXIT_FAILURE;
    }

    return EXIT_FAILURE;
}
