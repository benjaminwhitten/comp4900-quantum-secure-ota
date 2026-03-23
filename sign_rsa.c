#include <stdio.h>
#include <stdlib.h>
#include <openssl/sha.h>
#include <openssl/pem.h>
#include <openssl/evp.h>

/**
 * Signs a sha256 hash with RSA private key.
 * IN:  SHA256 hash to sign.
 * OUT: RSA signature of the hash.
 */
unsigned int signRSA(unsigned char* hsh, unsigned char** sig){
	// Variables.
	EVP_PKEY* skey;
	FILE *fptr;
	size_t siglen = 256;
	EVP_MD_CTX* ctx;

	// Reading key from PEM file.
	fptr = fopen("rsakeypair.pem", "r");
	if (fptr) {
		skey = EVP_PKEY_new();
		PEM_read_PrivateKey(fptr, &skey, NULL, NULL);
		fclose(fptr);

		// Generating signature.
		ctx = EVP_MD_CTX_new();
		EVP_DigestSignInit(ctx, NULL, NULL, NULL, skey);
		*sig = (unsigned char *)malloc(siglen);
		EVP_DigestSign(ctx, *sig, &siglen, hsh, SHA256_DIGEST_LENGTH);
		return 1;
	}
	return 0;
}

// Main function for testing.
int main(int argc, char **argv) {
	unsigned char* sig = NULL;

	if (argc > 1 && signRSA((unsigned char*) argv[1], &sig)) {
		// Printing in hex.
		for(int i = 0; i < 256; i++) {
			printf("%02x", sig[i]);
		}
		printf("\n");
		free(sig);
	} else if (argc <= 1) {
		printf("ERROR: No hash provided.\n");
	} else {
		printf("ERROR: Could not generate signature.\n");
	}

	return EXIT_SUCCESS;
}
