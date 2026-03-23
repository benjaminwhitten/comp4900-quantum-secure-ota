#include <stdio.h>
#include <stdlib.h>
#include <openssl/sha.h>
#include <openssl/pem.h>
#include <openssl/evp.h>

/**
 * Signs a sha256 hash with RSA private key.
 * IN: sig - The RSA signature to validate.
 * IN: crtFile - The .crt public key file.
 * IN: hsh - The signed hash.
 * Returns 1 if valid, 0 otherwise.
 */
unsigned int validateRSA(unsigned char* sig, char* crtFile, unsigned char* hsh){
	// Variables.
	EVP_PKEY* pkey;
	FILE *fptr;
	size_t siglen = 256;
	EVP_MD_CTX* ctx;

	printf("Sign: %s", sig);
	printf("\nPkey: %s", crtFile);
	printf("\nHash: %s\n", hsh);

	// Reading key from PEM file.
	fptr = fopen(crtFile, "r");
	printf("File read\n");
	if (fptr) {
		pkey = PEM_read_PrivateKey(fptr, NULL, NULL, NULL); //PEM_read_PUBKEY(fptr, NULL, NULL, NULL);
		fclose(fptr);

		// Reading signature.

		// Validating signature.
		ctx = EVP_MD_CTX_new();
		EVP_DigestVerifyInit(ctx, NULL, NULL, NULL, pkey);
		return EVP_DigestVerify(ctx, sig, siglen, hsh, SHA256_DIGEST_LENGTH);
	}
	return 0;
}

/**
 * Main function.
 * IN: RSA signature (argv[1]).
 * IN: The .crt public key file (argv[2]).
 * IN: SHA256 hash (argv[3]).
 * Validates the signature using the public key and SHA256 hash.
 */
int main(int argc, char **argv) {
	if (argc > 3) {
		if (validateRSA((unsigned char*) argv[1], argv[2], (unsigned char*) argv[3])) {
			printf("Valid");
		} else {
			printf("Invalid");
		}
	} else {
		printf("ERROR: Missing one or more arguments (.pem file, hash, signature).\n");
	}

	return EXIT_SUCCESS;
}
