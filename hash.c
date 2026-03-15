#include <stdio.h>
#include <stdlib.h>
#include <openssl/sha.h>

/**
 * Returns the SHA256 Hash of a file.
 * IN:  filename (including extension).
 * OUT: SHA256 Hash as unsigned char* or NULL if no file was found.
 *      Must be manually freed.
 */
unsigned char* hash(char* filename){
	// Variables.
	FILE *fptr;
	char *fcontent = NULL;
	unsigned int fsize;
	unsigned char *fhash = NULL;

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
		fhash = (unsigned char *)malloc(SHA256_DIGEST_LENGTH);
		SHA256((unsigned char *)fcontent, fsize, fhash);

		free(fcontent);
	}

	return fhash;
}

// Main function for testing.
int main(int argc, char **argv) {
	unsigned char* fhash;

	fhash = hash("/tmp/test.txt");

	if (fhash) {
		printf("%s", fhash);
		free(fhash);
	}

	return EXIT_SUCCESS;
}
