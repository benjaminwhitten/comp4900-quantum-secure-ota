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

static int signMLDSA(const char *prvFile, const char *imageFile, const char *sigFile) {
    unsigned char digest[SHA256_DIGEST_LENGTH];
    if (!sha256_file(imageFile, digest)) {
        fprintf(stderr, "ERROR: failed to hash image file: %s\n", imageFile);
        return 0;
    }

    unsigned char *skey = NULL;
    size_t skeyLen = 0;
    if (!read_file_all(prvFile, &skey, &skeyLen)) {
        fprintf(stderr, "ERROR: failed to read private key file: %s\n", prvFile);
        return 0;
    }

    OQS_SIG *sig = OQS_SIG_new(MLDSA_ALG);
    if (sig == NULL) {
        free(skey);
        fprintf(stderr, "ERROR: failed to create ML-DSA object\n");
        return 0;
    }

    if (skeyLen != sig->length_secret_key) {
        fprintf(stderr, "ERROR: private key size mismatch (got %zu, expected %zu)\n",
                skeyLen, sig->length_secret_key);
        OQS_SIG_free(sig);
        free(skey);
        return 0;
    }

    unsigned char *sigBuf = (unsigned char *)malloc(sig->length_signature);
    if (sigBuf == NULL) {
        OQS_SIG_free(sig);
        free(skey);
        fprintf(stderr, "ERROR: failed to allocate signature buffer\n");
        return 0;
    }

    size_t sigLen = 0;
    OQS_STATUS rc = OQS_SIG_sign(sig, sigBuf, &sigLen, digest, SHA256_DIGEST_LENGTH, skey);

    free(skey);

    if (rc != OQS_SUCCESS) {
        free(sigBuf);
        OQS_SIG_free(sig);
        fprintf(stderr, "ERROR: ML-DSA signing failed\n");
        return 0;
    }

    FILE *fp = fopen(sigFile, "wb");
    if (!fp) {
        free(sigBuf);
        OQS_SIG_free(sig);
        fprintf(stderr, "ERROR: failed to open signature output file: %s\n", sigFile);
        return 0;
    }

    fwrite(sigBuf, 1, sigLen, fp);
    fclose(fp);

    free(sigBuf);
    OQS_SIG_free(sig);
    return 1;
}

int main(int argc, char **argv) {
    if (argc != 4) {
        fprintf(stderr, "usage: %s <private_key.bin> <image.bin> <signature.bin>\n", argv[0]);
        return EXIT_FAILURE;
    }

    if (!signMLDSA(argv[1], argv[2], argv[3])) {
        return EXIT_FAILURE;
    }

    printf("Signature written to %s\n", argv[3]);
    return EXIT_SUCCESS;
}
