# include <stdio.h>
# include <stdlib.h>
#include <string.h>
//-------------------------------------------------------------------------------------------------

// S-Box presente sul libro
static const uint8_t sbox[256] = {
    0x63, 0x7C, 0x77, 0x7B, 0xF2, 0x6B, 0x6F, 0xC5, 0x30, 0x01, 0x67, 0x2B, 0xFE, 0xD7, 0xAB, 0x76,
    0xCA, 0x82, 0xC9, 0x7D, 0xFA, 0x59, 0x47, 0xF0, 0xAD, 0xD4, 0xA2, 0xAF, 0x9C, 0xA4, 0x72, 0xC0,
    0xB7, 0xFD, 0x93, 0x26, 0x36, 0x3F, 0xF7, 0xCC, 0x34, 0xA5, 0xE5, 0xF1, 0x71, 0xD8, 0x31, 0x15,
    0x04, 0xC7, 0x23, 0xC3, 0x18, 0x96, 0x05, 0x9A, 0x07, 0x12, 0x80, 0xE2, 0xEB, 0x27, 0xB2, 0x75,
    0x09, 0x83, 0x2C, 0x1A, 0x1B, 0x6E, 0x5A, 0xA0, 0x52, 0x3B, 0xD6, 0xB3, 0x29, 0xE3, 0x2F, 0x84,
    0x53, 0xD1, 0x00, 0xED, 0x20, 0xFC, 0xB1, 0x5B, 0x6A, 0xCB, 0xBE, 0x39, 0x4A, 0x4C, 0x58, 0xCF,
    0xD0, 0xEF, 0xAA, 0xFB, 0x43, 0x4D, 0x33, 0x85, 0x45, 0xF9, 0x02, 0x7F, 0x50, 0x3C, 0x9F, 0xA8,
    0x51, 0xA3, 0x40, 0x8F, 0x92, 0x9D, 0x38, 0xF5, 0xBC, 0xB6, 0xDA, 0x21, 0x10, 0xFF, 0xF3, 0xD2,
    0xCD, 0x0C, 0x13, 0xEC, 0x5F, 0x97, 0x44, 0x17, 0xC4, 0xA7, 0x7E, 0x3D, 0x64, 0x5D, 0x19, 0x73,
    0x60, 0x81, 0x4F, 0xDC, 0x22, 0x2A, 0x90, 0x88, 0x46, 0xEE, 0xB8, 0x14, 0xDE, 0x5E, 0x0B, 0xDB,
    0xE0, 0x32, 0x3A, 0x0A, 0x49, 0x06, 0x24, 0x5C, 0xC2, 0xD3, 0xAC, 0x62, 0x91, 0x95, 0xE4, 0x79,
    0xE7, 0xC8, 0x37, 0x6D, 0x8D, 0xD5, 0x4E, 0xA9, 0x6C, 0x56, 0xF4, 0xEA, 0x65, 0x7A, 0xAE, 0x08,
    0xBA, 0x78, 0x25, 0x2E, 0x1C, 0xA6, 0xB4, 0xC6, 0xE8, 0xDD, 0x74, 0x1F, 0x4B, 0xBD, 0x8B, 0x8A,
    0x70, 0x3E, 0xB5, 0x66, 0x48, 0x03, 0xF6, 0x0E, 0x61, 0x35, 0x57, 0xB9, 0x86, 0xC1, 0x1D, 0x9E,
    0xE1, 0xF8, 0x98, 0x11, 0x69, 0xD9, 0x8E, 0x94, 0x9B, 0x1E, 0x87, 0xE9, 0xCE, 0x55, 0x28, 0xDF,
    0x8C, 0xA1, 0x89, 0x0D, 0xBF, 0xE6, 0x42, 0x68, 0x41, 0x99, 0x2D, 0x0F, 0xB0, 0x54, 0xBB, 0x16
};
//-------------------------------------------------------------------------------------------------

// Definisco la struttura State descritta nel libro
typedef struct {
    uint8_t state[4][4];
} AESState;


// initState ripartisce adeguatamente un blocco del testo nello State
void initState(AESState *s, uint8_t *input, int inizio){
    for (int i=0; i < 4; ++i){
        for (int j=0; j < 4; ++j){
            s->state[i][j] = input[inizio+ (i*4 + j)];
            //printf("%02x ", s->state[i][j]);
        }
        //printf("\n");
    }
}

// Funzione per salvare dentro un buffer il ciphertext
void saveState(AESState s, uint8_t* output, int inizio){
    for (int i=0; i < 4; ++i){
        for (int j=0; j < 4; ++j){
            output[inizio + (i*4 + j)] = s.state[i][j];
            //printf("%02x ", s->state[i][j]);
        }
        //printf("\n");
    }
}
//-------------------------------------------------------------------------------------------------

// Esegue uno XOR tra la Round Key e lo State attuale
void addRoundKey(uint8_t counter[][4], uint8_t expKey[][4], int in){
    for(int i=0; i<4; i++){
        for(int j=0; j<4; j++){
            counter[i][j] = counter[i][j]^expKey[(in*4)+i][j];
            //printf("%02x ", st.state[i][j]);
        }
        //printf("\n");
    }
}

// Ogni byte in State viene mappato tramite la S-Box definita in AES_utils.h
void substituteBytes(uint8_t counter[][4]){
    for(int i=0; i<4; i++){
        for(int j=0; j<4; j++){
            counter[i][j] = sbox[counter[i][j]];
            //printf("%02x ", st.state[i][j]);
        }
        //printf("\n");
    }
}

// Eseguo lo Shift Row descritto nel libro
void shiftRows(uint8_t counter[][4]){

    uint8_t temp;

    // Shift di 1 a sinistra della seconda colonna
    // (Notare che shifto le colonne perche lo State e' disposto per righe)
    temp = counter[0][1];
    counter[0][1] = counter[1][1];
    counter[1][1] = counter[2][1];
    counter[2][1] = counter[3][1];
    counter[3][1] = temp;

    // Shift di 2 a sinistra della terza colonna
    temp = counter[0][2];
    counter[0][2] = counter[2][2];
    counter[2][2] = temp;
    temp = counter[1][2];
    counter[1][2] = counter[3][2];
    counter[3][2] = temp;

    // Shift di 3 a sinistra della quarta colonna
    temp = counter[0][3];
    counter[0][3] = counter[3][3];
    counter[3][3] = counter[2][3];
    counter[2][3] = counter[1][3];
    counter[1][3] = temp;
}

// Gestisco la moltiplicazione per 2 che nel contesto dei byte implica un possibile overflow
uint8_t twice(uint8_t byte){
    // Faccio uno shift a sinistra per moltiplicare per 2
    // Controllo il valore del MSB attraverso un And con 0x80 = 10000000
    // Se true faccio XOR con 0x1B, altrimenti XOR nullo per non modificare
    return (byte << 1) ^ ((byte & 0x80) ? 0x1B : 0x00);
}

// Riporto l'operazione per la moltiplicazione di matrice senza definire la matrice stessa
void mixColumn(uint8_t c[]){
    uint8_t temp[4];
    uint8_t temp2[4], temp3[4];
    memcpy(temp, c, 4);
    for(int i=0; i<4; i++){
        temp2[i] = twice(temp[i]);
        temp3[i] = temp2[i]^temp[i];
    }
    c[0] = temp2[0]^temp3[1]^temp[2]^temp[3];
    c[1] = temp[0]^temp2[1]^temp3[3]^temp[3];
    c[2] = temp[0]^temp[1]^temp2[2]^temp3[3];
    c[3] = temp3[0]^temp[1]^temp[2]^temp2[3];
}

// Gestisco Mix Columns in base alla matrice predefinita
void mixColumns(uint8_t counter[][4]){
    for(int i=0; i<4; i++){
        mixColumn(counter[i]);
    }
}

void genCipherText(AESState* st, uint8_t counter[][4]){
    for(int i=0; i<4; i++){
        for(int j=0; j<4; j++){
            st->state[i][j] = st->state[i][j]^counter[i][j];
        }
    }
}