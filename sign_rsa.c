#include <stdio.h>
#include <stdlib.h>
#include <openssl/sha.h>
#include <openssl/pem.h>
#include <openssl/evp.h>

/**
 * Signs a sha256 hash with RSA private key.
 * IN: pemfile - The .pem file containing the private key.
 * IN: hsh - SHA256 hash to sign.
 * OUT: sig - RSA signature of the hash.
 */
unsigned int signRSA(char* pemfile, unsigned char* hsh, unsigned char** sig){
	// Variables.
	EVP_PKEY* skey;
	FILE *fptr;
	size_t siglen = 256;
	EVP_MD_CTX* ctx;

	// Reading key from PEM file.
	fptr = fopen(pemfile, "r");
	if (fptr) {
		skey = PEM_read_PrivateKey(fptr, NULL, NULL, NULL);
		fclose(fptr);

		// Generating signature.
		ctx = EVP_MD_CTX_new();
		EVP_DigestSignInit(ctx, NULL, NULL, NULL, skey);
		*sig = (unsigned char *)malloc(siglen);
		EVP_DigestSign(ctx, *sig, &siglen, hsh, SHA256_DIGEST_LENGTH);

		// Writing to file.
		fptr = fopen("rsa_sig.bin", "wb");
		fwrite(sig, 1, siglen, fptr);
		fclose(fptr);

		return 1;
	}
	return 0;
}

/**
 * Main function.
 * IN: The .pem file (argv[1]).
 * IN: SHA256 hash (argv[2]).
 * Signs using the private key and prints it to console.
 */
int main(int argc, char **argv) {
	unsigned char* sig = NULL;

	if (argc > 2 && signRSA(argv[1], (unsigned char*) argv[2], &sig)) {
		// Printing in hex.
		for(int i = 0; i < 256; i++) {
			printf("%02x", sig[i]);
		}
		free(sig);
	} else if (argc <= 2) {
		printf("ERROR: Missing one or more arguments (.pem file, hash).\n");
	} else {
		printf("ERROR: Could not generate signature.\n");
	}

	return EXIT_SUCCESS;
}
