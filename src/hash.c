/* hash.c */
#include <stdio.h>
#include <stdlib.h>
#include <openssl/sha.h>

static int hash_file(const char *filename, unsigned char out[SHA256_DIGEST_LENGTH]) {
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

int main(int argc, char **argv) {
    unsigned char digest[SHA256_DIGEST_LENGTH];

    if (argc != 2) {
        fprintf(stderr, "usage: %s <file>\n", argv[0]);
        return EXIT_FAILURE;
    }

    if (!hash_file(argv[1], digest)) {
        fprintf(stderr, "ERROR: Failed to hash file: %s\n", argv[1]);
        return EXIT_FAILURE;
    }

    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        printf("%02x", digest[i]);
    }
    printf("\n");

    return EXIT_SUCCESS;
}
