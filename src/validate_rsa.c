/* validate_rsa.c */
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

static int validateRSA(const char *pemFile, const char *imageFile, const char *sigFile) {
    unsigned char digest[SHA256_DIGEST_LENGTH];
    if (!sha256_file(imageFile, digest)) {
        fprintf(stderr, "ERROR: Failed to hash image file: %s\n", imageFile);
        return -1;
    }

    FILE *fptr = fopen(pemFile, "r");
    if (!fptr) {
        fprintf(stderr, "ERROR: Failed to open public key: %s\n", pemFile);
        return -1;
    }

    EVP_PKEY *pkey = PEM_read_PUBKEY(fptr, NULL, NULL, NULL);
    fclose(fptr);
    if (!pkey) {
        fprintf(stderr, "ERROR: Failed to read public key.\n");
        return -1;
    }

    fptr = fopen(sigFile, "rb");
    if (!fptr) {
        EVP_PKEY_free(pkey);
        fprintf(stderr, "ERROR: Failed to open signature file: %s\n", sigFile);
        return -1;
    }

    if (fseek(fptr, 0, SEEK_END) != 0) {
        fclose(fptr);
        EVP_PKEY_free(pkey);
        fprintf(stderr, "ERROR: Failed to seek signature file.\n");
        return -1;
    }

    long sigLenLong = ftell(fptr);
    if (sigLenLong <= 0) {
        fclose(fptr);
        EVP_PKEY_free(pkey);
        fprintf(stderr, "ERROR: Invalid signature file length.\n");
        return -1;
    }

    rewind(fptr);

    size_t sigLen = (size_t)sigLenLong;
    unsigned char *sig = (unsigned char *)malloc(sigLen);
    if (!sig) {
        fclose(fptr);
        EVP_PKEY_free(pkey);
        fprintf(stderr, "ERROR: Failed to allocate signature buffer.\n");
        return -1;
    }

    if (fread(sig, 1, sigLen, fptr) != sigLen) {
        free(sig);
        fclose(fptr);
        EVP_PKEY_free(pkey);
        fprintf(stderr, "ERROR: Failed to read signature file.\n");
        return -1;
    }

    fclose(fptr);

    EVP_MD_CTX *ctx = EVP_MD_CTX_new();
    if (!ctx) {
        free(sig);
        EVP_PKEY_free(pkey);
        fprintf(stderr, "ERROR: Failed to allocate verify context.\n");
        return -1;
    }

    if (EVP_DigestVerifyInit(ctx, NULL, NULL, NULL, pkey) != 1) {
        EVP_MD_CTX_free(ctx);
        free(sig);
        EVP_PKEY_free(pkey);
        fprintf(stderr, "ERROR: EVP_DigestVerifyInit failed.\n");
        return -1;
    }

    int rc = EVP_DigestVerify(ctx, sig, sigLen, digest, SHA256_DIGEST_LENGTH);

    EVP_MD_CTX_free(ctx);
    free(sig);
    EVP_PKEY_free(pkey);

    if (rc == 1) {
        return 1;
    }
    if (rc == 0) {
        return 0;
    }

    fprintf(stderr, "ERROR: EVP_DigestVerify failed internally.\n");
    return -1;
}

int main(int argc, char **argv) {
    if (argc != 4) {
        fprintf(stderr, "usage: %s <public_key.pem> <image.bin> <signature.bin>\n", argv[0]);
        return EXIT_FAILURE;
    }

    int rc = validateRSA(argv[1], argv[2], argv[3]);
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
