#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/sha.h>
#include <oqs/oqs.h>

#ifndef MLDSA_ALG
#define MLDSA_ALG OQS_SIG_alg_ml_dsa_44
#endif

static unsigned int read_file_all(const char *path,unsigned char **buf,size_t *len){
	//open file, store size
	FILE *fptr=fopen(path,"rb");
	long size;
	//failure to open
	if(!fptr){
		return 0;
	}
	//get file size
	fseek(fptr,0,SEEK_END);
	size=ftell(fptr);
	fseek(fptr,0,SEEK_SET);
	//allocate mry for file and check
	*buf=(unsigned char *)malloc((size_t)size);
	if(*buf==NULL){
		fclose(fptr);
		return 0;
	}
	//read full file
	fread(*buf,1,(size_t)size,fptr);
	//cleanup
	fclose(fptr);
	*len=(size_t)size;
	return 1;
}

//verifies an mldsa signature against a sha256 input
//pubfile is the public key file
//hsh is the sha256 sized input
//sigfile is the signature file
unsigned int validateMLDSA(const char *pubFile,const unsigned char *hsh,const char *sigFile){
	OQS_SIG *sig=OQS_SIG_new(MLDSA_ALG);
	unsigned char *pkey=NULL;
	unsigned char *sigBuf=NULL;
	size_t pkeyLen=0;
	size_t sigLen=0;
	OQS_STATUS rc;
	if(sig==NULL){
		return 0;
	}
	if(!read_file_all(pubFile,&pkey,&pkeyLen)){
		OQS_SIG_free(sig);
		return 0;
	}
	if(!read_file_all(sigFile,&sigBuf,&sigLen)){
		free(pkey);
		OQS_SIG_free(sig);
		return 0;
	}
	rc=OQS_SIG_verify(sig,hsh,SHA256_DIGEST_LENGTH,sigBuf,sigLen,pkey);
	free(pkey);
	free(sigBuf);
	OQS_SIG_free(sig);
	return (rc==OQS_SUCCESS);
}

//main function
//arguement 1 is the public key file
//arguement 2 is the sha256 hash input
//arguement 3 is the signature file
int main(int argc,char **argv){
	if(argc<=3){
		printf("error missing arguments\n");
	}
	else if(strlen(argv[2])<SHA256_DIGEST_LENGTH){
		printf("error hash input too short\n");
	}
	else if(validateMLDSA(argv[1],(unsigned char *)argv[2],argv[3])){
		printf("valid\n");
	}
	else{
		printf("invalid\n");
	}
	return EXIT_SUCCESS;
}
