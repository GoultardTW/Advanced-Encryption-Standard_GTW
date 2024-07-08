# include <stdio.h>
# include <stdlib.h>
//-------------------------------------------------------------------------------------------------

// Costanti di round predefinite da AES
static const uint8_t roundConstants[10] = {
    0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1B, 0x36
};
//-------------------------------------------------------------------------------------------------

// Funzione g dell'algoritmo di espansione della chiave iniziale
void gFunction(uint8_t word[], int round){
    
    // Swappo a sinistra i byte
    uint8_t temp = word[0];
    word[0] = word[1];
    word[1] = word[2];
    word[2] = word[3];
    word[3] = temp;

    // Mappo i byte in base alla sbox
    for(int i=0; i<4; i++){
        word[i] = sbox[word[i]];
    }

    // Eseguo uno XOR tra il primo byte della Word e la attuale Round Constant
    word[0] = word[0]^roundConstants[round];
}

// Funzione di espansione della chiave iniziale
void expandKey(uint8_t expKey[][4], uint8_t key[]){
    
    // Le chiave iniziale viene copiata nelle prime 4 word
    for(int i=0; i<4; i++){
        for(int j=0; j<4; j++){
            expKey[i][j]= key[i*4+j];
        }
    }

    // Calcolo le ulteriori 40 word
    uint8_t temp[4];
    for(int i=4; i<44; i++){
        // Se i divisibile per 4 -> calcolo la gFunction e XOR con word della stessa colonna, una riga sopra
        if(i%4 == 0){
            for(int j=0; j<4; j++){
                temp[j] = expKey[i-1][j];
            }
            gFunction(temp, (i/4)-1);
            for(int j=0; j<4; j++){
                expKey[i][j] = temp[j]^expKey[i-1][j];
                //printf("%02x ", expKey[i][j]);
            }
        }
        // Di norma calcolo la chiave con XOR tra il byte precedente e il rispettivo della riga precedente
        else{
            for(int j=0; j<4; j++){
                expKey[i][j] = expKey[i][j-1]^expKey[i-1][j];
                //printf("%02x ", expKey[i][j]);
            }
        }
        //printf("\n");
    }
}


// Funzione per incrementare il counter di AES
void incrementCounter(uint8_t counter[][4]){
    for (int i=3; i>1; i--){
        for(int j=3; i>=0; i--){
            if(++counter[i][j]!=0){
                break;
            } 
        }
    }
}
//-----------------------------------------------------------------------------------------------

// Funzione generatrice del buffer utilizzato per la gestione del contenuto in blocchi da 16 byte
void generateBuffer(FILE *file, uint8_t **buffer, long *filepadsize){

    // Determino la lunghezza in byte del file per capire sa va aggiunto padding
    fseek(file, 0L, SEEK_END);
    long int filesize = ftell(file); 
    filesize++; // Qui ci aggiungo 1 perche un byte definisce la taglia del padding
    rewind(file);
    int padsize = 16 - (filesize % 16);
    *filepadsize = filesize + padsize;

    // Alloco un buffer su cui lavorare
    *buffer = (uint8_t *)malloc(*filepadsize);
    if (*buffer == NULL){
        fprintf(stderr, "Buffer allocation error\n");
        fclose(file);
        return;
    }

    // Copio nel buffer il file e il padding
    fread(*buffer, 1, filesize, file);
    for (int i=filesize; i<(*filepadsize)-1; i++){
        (*buffer)[i] = 0x00;
    }
    
    // Mi salvo nell'ultimo byte la taglia del padding
    (*buffer)[(*filepadsize)-1] = padsize;

}

