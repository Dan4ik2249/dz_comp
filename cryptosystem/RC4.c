#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define N 256   // 2^8

void swap(unsigned char *a, unsigned char *b) {
    int tmp = *a;
    *a = *b;
    *b = tmp;
}

int KSA(char *key, unsigned char *S) {

    int len = strlen(key);
    int j = 0;

    for(int i = 0; i < N; i++)
        S[i] = i;

    for(int i = 0; i < N; i++) {
        j = (j + S[i] + key[i % len]) % N;

        swap(&S[i], &S[j]);
    }

    return 0;
}

int PRGA(unsigned char *S, char *plaintext, unsigned char *ciphertext) {

    int i = 0;
    int j = 0;

    for(size_t n = 0, len = strlen(plaintext); n < len; n++) {
        i = (i + 1) % N;
        j = (j + S[i]) % N;

        swap(&S[i], &S[j]);
        int rnd = S[(S[i] + S[j]) % N];

        ciphertext[n] = rnd ^ plaintext[n];

    }

    return 0;
}

int RC4(char *key, char *plaintext, unsigned char *ciphertext) {

    unsigned char S[N];

    KSA(key, S);
    PRGA(S, plaintext, ciphertext);

    return 0;
}

int main(int argc, char *argv[])
{
	FILE* filein; //файл на вход
	if ((filein = fopen(argv[2], "rb")) == NULL) {
		perror("Cannot open file");
		return __LINE__;
	}

	FILE* fileout; //файл на выход
       	if ((fileout = fopen(argv[3], "wb")) == NULL) {
		perror("Cannot open file");
		return __LINE__;
	}

	int n;
	
	fseek(filein, 0l, SEEK_END);
	int size = ftell(filein);
	rewind(filein);
	printf("%d", size);
	unsigned char *ciphertexts = malloc(size*sizeof(char));
	unsigned char *buffer = malloc(size*sizeof(char));
	
	if ((n = fread(buffer, sizeof(char), size, filein)) == EOF) {
		perror("Cannot read file");
		return __LINE__;
	}
	buffer[size+1] = '\0';
	RC4(argv[1], buffer, ciphertexts);// ключ, вход, выход
	if ((fwrite(ciphertexts, sizeof(char), n, fileout)) == EOF) {
		perror("Cannot write file");
		return __LINE__;
	}
	fclose(filein);
	fclose(fileout);
	free(ciphertexts);
	free(buffer);

	return 0;
}
