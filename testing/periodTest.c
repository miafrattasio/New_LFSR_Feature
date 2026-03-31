#include <stdio.h>

int main (int argc, char argv[]){

    if (argc >= 2){
        printf("hello world");
    }

}

int checkPeriod(char fileName[], int length, int shift){

    FILE* file = fopen(fileName, "r");

    int firstBit;
    int previousBit;

    // store the first bit
    fscanf(fileName, "%d", previousBit);

    // for the length of the bit sequence, 
    // if the number is the same as the last one

}