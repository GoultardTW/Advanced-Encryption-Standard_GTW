# include <stdio.h>
# include <stdlib.h>
# include <openssl/rand.h>


int main(int argc, char *argv[]){

    // Apro il file result.txt
    FILE *file = fopen("./result.txt", "rb");
    if (file == NULL){
        fprintf(stderr, "File not found or not readable\n");
        return 1;
    }

    // Genero un buffer per il testo cifrato
    fseek(file, 0L, SEEK_END);
    long int filesize = ftell(file);
    uint8_t* cipherT = (uint8_t *)malloc(filesize);
    if (cipherT == NULL){
        fprintf(stderr, "First uffer allocation error\n");
        fclose(file);
        return 1;
    }
    rewind(file);
    fread(cipherT, 1, filesize, file);
    fclose(file);

    // Genero un buffer con il testo da XORare
    uint8_t* xorT = (uint8_t *)malloc(filesize);
    if (xorT == NULL){
        fprintf(stderr, "Second buffer allocation error\n");
        return 1;
    }
    RAND_bytes(xorT, filesize);

    // Eseguo lo XOR
    for(int i=0; i<filesize-1; i++){
        cipherT[i]=cipherT[i]^xorT[i];
    }

    // Salvo tutto dentro result.txt
    file = fopen("result.txt", "wb");
    if(file == NULL){
        fprintf(stderr, "Impossible to save encryption");
        return 1;
    }
    fwrite(cipherT, 1, filesize, file);
    fclose(file);

    // Invio al Decryption Oracle il file XORato
    if (system("./AES result.txt -d") == 1) {
        printf("Errore durante l'esecuzione del comando\n");
        return 1;
    }

    // Recupero il file decryptato
    file = fopen("./result.txt", "rb");
    if (file == NULL){
        fprintf(stderr, "File not available\n");
        return 1;
    }

    // Copio dentro cipherT il contenuto del file
    fread(cipherT, 1, filesize, file);

    
    // Eseguo lo XOR
    for(int i=0; i<filesize-1; i++){
        cipherT[i]=cipherT[i]^xorT[i];
    }

    // Salvo il risultato dentro result.txt
    file = fopen("result.txt", "wb");
    if(file == NULL){
        fprintf(stderr, "Impossible to save result");
        return 1;
    }
    fwrite(cipherT, 1, filesize, file);
    fclose(file);

    // Libero le due allocazioni di memoria
    free(cipherT);
    free(xorT);

}
// gcc -o CCA CCA.c -lcrypto