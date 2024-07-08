# include <stdio.h>
# include <stdlib.h>
# include <stdint.h>
# include <openssl/aes.h>
# include <openssl/rand.h>
# include <string.h>
# include <unistd.h>
# include "State.h"
# include "AES_utils.h"

int main(int argc, char *argv[]){
    
    // Controllo che ci sia almeno un argomento
    if (argc < 2){
        fprintf(stderr, "There should be at least one argument\n");
        return 1;
    }
    
    // Controllo che ci sia solo un argomento
    // Gestisco la modalita (Enc/Dec)
    char mode = 'E';
    if (argc > 2){
        if(strcmp(argv[2], "-d")==0){
            mode = 'D';
        }else if(strcmp(argv[2], "-e")==0){
            mode = 'E';
        }else{
            fprintf(stderr, "Mode is not valid\n");
        }
    }

    // Controllo che l'argomento sia valido e salvo il file
    FILE *file = fopen(argv[1], "rb");
    if (file == NULL){
        fprintf(stderr, "File not found or not readable\n");
        return 1;
    }
    
    // Genero un buffer su cui lavorare
    uint8_t* buffer;
    long filepadsize;
    generateBuffer(file, &buffer, &filepadsize);
    fclose(file);

    // Definisco una chiave e un nonce
    // Se sto crittografando le genero randomicamente
    // Se sto decifrando le recupero dai rispettivi file
    uint8_t key[16];
    uint8_t nonce[8]; 
    if (mode == 'E'){
        RAND_bytes(key, 16);
        RAND_bytes(nonce, 8);
    }else{
        file = fopen("key.txt", "rb");
        if (file == NULL){
            fprintf(stderr, "Key not found or not readable\n");
            return 1;
        }
        fread(key, 1, 16, file);
        fclose(file);
        file = fopen("nonce.txt", "rb");
        if (file == NULL){
            fprintf(stderr, "Nonce not found or not readable\n");
            return 1;
        }
        fread(nonce, 1, 8, file);
        fclose(file);
    }

    // Definisco il counter
    uint8_t counter[4][4];
    for(int i=0; i<4; i++){
        for(int j=0; j<4; j++){
            if(i<2){
                counter[i][j] = nonce[4*i+j];
            }
            else{
                counter[i][j] = 0x00;
            }
        }
    }
    
    // Espando la chiave
    uint8_t expKey[44][4];
    expandKey(expKey, key);

    // Per ogni blocco ottengo uno State e applico la crittografia
    AESState st;
    for (int i=0; i<filepadsize; i += 16){
        initState(&st, buffer, i);

        // Applico l'algoritmo AES
        addRoundKey(counter, expKey, 0);
        for(int j=1; j<10; j++){
            substituteBytes(counter);
            shiftRows(counter);
            mixColumns(counter);
            addRoundKey(counter, expKey, j);
        }
        substituteBytes(counter);
        shiftRows(counter);
        addRoundKey(counter, expKey, 10);

        // Faccio lo XOR con lo State
        genCipherText(&st, counter);

        // Salvo nel buffer il testo cifrato
        saveState(st, buffer, i);

        // Incremento il Counter
        incrementCounter(counter);
    }

    // Salvo il contenuto del buffer in un file
    file = fopen("result.txt", "wb");
    if(file == NULL){
        fprintf(stderr, "Impossible to save encryption");
        return 1;
    }
    if(mode == 'D'){
        filepadsize -= (int)buffer[filepadsize - 17]+17;
    }
    fwrite(buffer, 1, filepadsize, file);
    fclose(file);

    // Salvo la chiave e il nonce in altri 2 file
    file = fopen("key.txt", "wb");
    if(file == NULL){
        fprintf(stderr, "Impossible to save the key");
        return 1;
    }
    fwrite(key, 1, 16, file);
    fclose(file);
    file = fopen("nonce.txt", "wb");
    if(file == NULL){
        fprintf(stderr, "Impossible to save the nonce");
        return 1;
    }
    fwrite(nonce, 1, 8, file);
    fclose(file);

    // Libero l'allocazione
    free(buffer);
}
// gcc -o AES AES.c -lcrypto