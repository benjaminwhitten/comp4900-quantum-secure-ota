#include <stdio.h>
#include <stdlib.h>
#include <openssl/sha.h>
#include <oqs/oqs.h>

/**
 * Signs a sha256 hash with MLDSA private key and writes to .bin file.
 * IN: pemFile - The .bin file containing the private key.
 * IN: hsh - SHA256 hash to sign.
 * IN: sigFile - The .bin file to write the signature to.
 */
unsigned int signMLDSA(const char* prvFile, const unsigned char* hsh, const char* sigFile){
	// Variables.
	unsigned char skey[OQS_SIG_ml_dsa_44_length_secret_key];
	FILE *fptr;
	size_t sigLen;
	unsigned char sig[OQS_SIG_ml_dsa_44_length_signature];

	// Reading key from .bin file.
	fptr = fopen(prvFile, "rb");
	if (fptr) {
		fread(&skey, sizeof(char), OQS_SIG_ml_dsa_44_length_secret_key, fptr);
		fclose(fptr);

		// Generating signature.
		OQS_SIG_ml_dsa_44_sign(sig, &sigLen, hsh, SHA256_DIGEST_LENGTH, skey);

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
 * IN: argv[1] - The .bin file containing the private key.
 * IN: argv[2] - SHA256 hash.
 * IN: argv[3] - .bin file to write signature to.
 * Signs using the private key and writes to .bin file.
 */
int main(int argc, char **argv) {
	if (argc <= 3) {
		printf("ERROR: Missing one or more arguments (.pem file, hash, .bin file).\n");
	} else if (!signMLDSA(argv[1], (unsigned char*) argv[2], argv[3])) {
		printf("ERROR: Failed generate signature.\n");
	} else {
		printf("Signature written to %s\n", argv[3]);
	}

	return EXIT_SUCCESS;
}
