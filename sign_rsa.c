#include <stdio.h>
#include <stdlib.h>
#include <openssl/sha.h>
#include <openssl/pem.h>
#include <openssl/evp.h>

/**
 * Signs a sha256 hash with RSA private key and writes to .bin file.
 * IN: pemFile - The .pem file containing the private key.
 * IN: hsh - SHA256 hash to sign.
 * IN: sigFile - The .bin file to write the signature to.
 */
unsigned int signRSA(char* pemFile, unsigned char* hsh, char* sigFile){
	// Variables.
	EVP_PKEY* skey;
	FILE *fptr;
	size_t sigLen = 256;
	unsigned char sig[sigLen];
	EVP_MD_CTX* ctx;

	// Reading key from PEM file.
	fptr = fopen(pemFile, "r");
	if (fptr) {
		skey = PEM_read_PrivateKey(fptr, NULL, NULL, NULL);
		fclose(fptr);

		// Generating signature.
		ctx = EVP_MD_CTX_new();
		EVP_DigestSignInit(ctx, NULL, NULL, NULL, skey);
		EVP_DigestSign(ctx, (unsigned char *) &sig, &sigLen, hsh, SHA256_DIGEST_LENGTH);

		// Writing signature to .bin file.
		fptr = fopen(sigFile, "wb");
		if (fptr) {
			fwrite(sig, 1, sigLen, fptr);
			fclose(fptr);
			return 1;
		}
	}
	return 0;
}

/**
 * Main function.
 * IN: argv[1] - The .pem file containing the private key.
 * IN: argv[2] - SHA256 hash.
 * IN: argv[3] - .bin file to write signature to.
 * Signs using the private key and writes to .bin file.
 */
int main(int argc, char **argv) {
	if (argc <= 3) {
		printf("ERROR: Missing one or more arguments (.pem file, hash, .bin file).\n");
	} else if (!signRSA(argv[1], (unsigned char*) argv[2], argv[3])) {
		printf("ERROR: Failed generate signature.\n");
	} else {
		printf("Signature written to %s\n", argv[3]);
	}

	return EXIT_SUCCESS;
}
