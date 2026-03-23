#include <stdio.h>
#include <stdlib.h>
#include <openssl/sha.h>

/**
 * Returns the SHA256 Hash of a file.
 * IN:  filename (including extension).
 * OUT: SHA256 Hash as unsigned char* or NULL if no file was found.
 *      Must be manually freed.
 */
unsigned int hash(char* filename, unsigned char** fhash){
	// Variables.
	FILE *fptr;
	char *fcontent = NULL;
	unsigned int fsize;

	fptr = fopen(filename, "r");
	if (fptr) {
		// Getting file size.
		fseek(fptr, 0L, SEEK_END);
		fsize = ftell(fptr) + 1;
		fseek(fptr, 0L, SEEK_SET);

		// Reading file.
		fcontent = (char *)malloc(fsize);
		fgets(fcontent, fsize, fptr);
		fclose(fptr);

		// Computing hash.
		*fhash = (unsigned char *)malloc(SHA256_DIGEST_LENGTH);
		SHA256((unsigned char *)fcontent, fsize, *fhash);

		free(fcontent);
		return 1;
	}
	return 0;
}

// Main function for testing.
int main(int argc, char **argv) {
	unsigned char* fhash = NULL;

	if (argc > 1 && hash(argv[1], &fhash)) {
		// Printing in hex.
		for(int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
			printf("%02x", fhash[i]);
		}
		printf("\n");
		free(fhash);
	} else if (argc <= 1) {
		printf("ERROR: No filename provided.\n");
	} else {
		printf("Error: Failed to hash file.\n");
	}

	return EXIT_SUCCESS;
}
