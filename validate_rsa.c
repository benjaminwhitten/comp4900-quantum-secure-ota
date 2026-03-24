#include <stdio.h>
#include <stdlib.h>
#include <openssl/sha.h>
#include <openssl/pem.h>
#include <openssl/evp.h>

/**
 * Validates a sha256 hash signature with RSA public key.
 * IN: pemFile - The .pem file containing the public key.
 * IN: hsh - SHA256 hash to validate.
 * IN: sigFile - The .bin file containing the signature.
 * Returns 1 if valid, 0 otherwise.
 */
unsigned int validateRSA(const char* pemFile, const unsigned char* hsh, const char* sigFile){
	// Variables.
	EVP_PKEY* pkey;
	FILE *fptr;
	size_t sigLen = 256;
	unsigned char sig[sigLen];
	EVP_MD_CTX* ctx;

	// Reading key from PEM file.
	fptr = fopen(pemFile, "r");
	if (fptr) {
		pkey = PEM_read_PUBKEY(fptr, NULL, NULL, NULL);
		fclose(fptr);

		// Reading signature from .bin file.
		fptr = fopen(sigFile, "rb");
		if (fptr) {
			fread(&sig, sizeof(char), sigLen, fptr);
			fclose(fptr);

			// Validating signature.
			ctx = EVP_MD_CTX_new();
			EVP_DigestVerifyInit(ctx, NULL, NULL, NULL, pkey);
			return EVP_DigestVerify(ctx, (unsigned char*) &sig, sigLen, hsh, SHA256_DIGEST_LENGTH);
		}
	}
	return 0;
}

/**
 * Main function.
 * IN: argv[1] - The .pem file containing the public key.
 * IN: argv[2] - SHA256 hash.
 * IN: argv[3] - .bin file to containing the signature.
 * Validates the signature using the public key and SHA256 hash.
 */
int main(int argc, char **argv) {
	if (argc > 3) {
		if (validateRSA(argv[1], (unsigned char*) argv[2], argv[3])) {
			printf("Valid");
		} else {
			printf("Invalid");
		}
	} else {
		printf("ERROR: Missing one or more arguments (.pem file, hash, signature).\n");
	}

	return EXIT_SUCCESS;
}
