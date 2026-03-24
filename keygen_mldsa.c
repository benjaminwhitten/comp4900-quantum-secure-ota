#include <stdio.h>
#include <stdlib.h>
#include <openssl/sha.h>
#include <oqs/oqs.h>
#ifndef MLDSA_ALG
#define MLDSA_ALG OQS_SIG_alg_ml_dsa_44
#endif

static void write_file(const char* path, const unsigned char *buf, size_t len) {
    FILE *f = fopen(path, "wb");
    fwrite(buf, 1, len, f);
    fclose(f);
}

/**
 * Generates mldsa public and private keys and writes them to .bin files.
 * IN: prvfile - What to name the .bin containing the private key.
 * IN: pubfile - What to name the .bin containing the public key.
 */
unsigned int keyGenMLDSA(const char* prvFile, const char* pubFile){
	// Variables.
	OQS_SIG *sig = OQS_SIG_new(MLDSA_ALG);
	unsigned char *pkey = malloc(sig->length_public_key);
	unsigned char *skey = malloc(sig->length_secret_key);

	// Generating keys.
	OQS_SIG_keypair(sig, pkey, skey);

	// Writing to files.
	write_file(pubFile, pkey, sig->length_public_key);
	write_file(prvFile, skey, sig->length_secret_key);

	free(pkey);
	free(skey);
	OQS_SIG_free(sig);
	return 1;
}

/**
 * Main function.
 * IN: argv[1] - What to name the .bin containing the private key.
 * IN: argv[2] - What to name the .bin containing the public key.
 * Generates mldsa public and private keys and writes them to .bin files.
 */
int main(int argc, char **argv) {
	if (argc <= 2) {
		printf("ERROR: Missing one or more arguments (.bin skey file, .bin pkey file).\n");
	} else if (!keyGenMLDSA(argv[1], argv[2])) {
		printf("ERROR: Failed generate keys.\n");
	} else {
		printf("Keys successfully generated.\n");
	}

	return EXIT_SUCCESS;
}
