#include <stdio.h>


/*
int clockCycle(int state, int gatePositions)

takes a given set of states and the gate positions, and outputs the next set of states

input:
    int state: the integer representation of the registers
    int gates: the integer representation of the gates

output:
    the integer representation of the next register states
*/
int clockCycle(int state, int gates);

/*
int listXOR(int *list1, int *list2)

takes two lists and returns whether or not they are the same

input:
    int *list1: a list of integers of length n
    int *list2: a list of integers of length n
    int length: the length of the input lists

output:
    0 : the lists are different
    1 : the lists are the same
*/
int listEquality(int *list1, int *list2, int size);

/*
https://stackoverflow.com/questions/2525310/
*/
void  SetBit( unsigned int A[],  unsigned int k, int sizeOfA);

/*
https://stackoverflow.com/questions/2525310/
*/
int GetBit( unsigned int A[], unsigned int k);

/*
void U32_0B_print (unsigned int number)

prints the binary representation of a 32 bit unsigned integer

input:
    unsigned int number: the number to print
*/
void U32_0B_print (unsigned int number);




int main(void){

    const int ITERATIONS = 512;
    const unsigned int P = 0b111011000001101110110100111110;

    int initialState  = 0b000110100;
    int gatePositions = 0b101000011;

    //int m_gatePositions = gatePositions;

    //int *list1 = {0,0,0,1,1,0,1,0,0};
    //int *list2 = {1,0,1,0,0,0,0,1,1};    

    int currentState = initialState;
    unsigned int output[16] = {0};

    
    int period = 0;

    printf("%d\n\n" , currentState);  

    for (int i = 0; i < ITERATIONS; i++){

        // update to the next state
            // run a clock cycle on each register
            int lastbit = clockCycle(currentState, gatePositions);

            // update the last bit
            lastbit = lastbit << 8;
            //shift the current state all down one
            currentState = currentState >> 1;

            // include the last bit in the current state
            currentState = currentState | lastbit;

            int outputBit = currentState % 2;

        // rejoice

            // print the associated number
            // also print the output bit for this state
            printf("%d - %d\n", outputBit, currentState); 

            if (outputBit == 1){
                SetBit(output, i, 512);
            }

            // check if it's the end of the period
            if (currentState == initialState){

                // print the useful information
                period = i+1;
                printf("the period is %d\n", period);

                for (int j = 0; j < period; j++){

                    //printf("j = %d\n", j);

                    if (GetBit(output, j) == 1){
                        //printf("bit %d is a 1", j);
                        SetBit(output, j+period, 512);
                    }

                }

                // print the output string
                // !!! may need to manually add more if the period is too long
                U32_0B_print(output[0]);

                /*

                U32_0B_print(output[1]);
                U32_0B_print(output[2]);
                U32_0B_print(output[3]);
                U32_0B_print(output[4]);
                
                U32_0B_print(output[5]);
                U32_0B_print(output[6]);
                U32_0B_print(output[7]);
                U32_0B_print(output[8]);
                
                U32_0B_print(output[9]);
                U32_0B_print(output[10]);
                U32_0B_print(output[11]);
                U32_0B_print(output[12]);
                U32_0B_print(output[13]);
                U32_0B_print(output[14]);
                U32_0B_print(output[15]);
                */
                
                unsigned int vernamCipher = P ^ output[0];

                printf("\n");
                U32_0B_print(vernamCipher);
                
                return 0;
            }
    }   

    U32_0B_print(output[0]);
    printf("\ndid not reach a period.");

}


void  SetBit( unsigned int A[],  unsigned int k, int sizeOfA){

    if (k >= sizeOfA){
        printf("could not set bit %u; segmentation fault", k);
        return;
    }

    //// gets which integer in A you want to change
    unsigned int i = k/32U;        //gives the corresponding index in the array A

    //// finds the location of the bit within the integer
    unsigned int pos = k%32U;      //gives the corresponding bit position in A[i]

    
    //// creates a single 1 in a 32-bit form
    unsigned int flag = 1U;   // flag = 0000.....00001
    // shifts the 1 to the correct position within the 32-bit integer
    flag = flag << pos;      // flag = 0000...010...000   (shifted k positions)

    //// bitwise ORs the preexisting number and the new bit, to make sure it's a 1 now
    A[i] = A[i] | flag;      // Set the bit at the k-th position in A[i]
}


int listEquality(int *list1, int *list2, int size){

    for (int i = 0; i < size; i++){

        // if the XOR is not 0, the numbers are different
        if ( (list1[i] ^ list2[i]) != 0){
            // return that they are unequal/
            return 0;
        }
    }

    // if you've gone through the list and none of them are unequal, return that the lists are equal.
    return 1;

}


int clockCycle(int state, int gates){

    int XORcounter = 0;

    // use this to iterate along the binary representation of the gate positions
    // goes from right to left
    for (int i = 0; i < 9; i++){

        // if the gate is open and the state has a '1' in the same position, add it to the XOR counter
        if (gates % 2 == 1 && state % 2 == 1){

            XORcounter += 1;
        
        }

        gates /= 2;
        state /= 2;

    }

    return XORcounter % 2;

}


void U32_0B_print (unsigned int number){

    for (int i = 0; i < 32; i++){

        printf("%d", number % 2);

        number = number >> 1;
    }

    printf("\n");

}


int GetBit( unsigned int A[], unsigned int k ){
    // the index within A
    int i = k/32U;
    // the location within A[i]
    unsigned int pos = k%32;

    // if there is a 1 at the index you are testing, return 1, otherwise return 0
    // ex (110 & 010) = 010 != 0,  
    return ( (A[i] & (1U << pos )) != 0 ) ;     
}



/*
void BinaryIterator(int *number, void (*function)){

    int myNumber = *number;

    for (int i = 0; i < 9; i++){

    //printf("initial state: %d, initial number: %d\n", initialState, initialNum);
    if (myNumber % 2 == 1){

        function;
    
    }
    myNumber /= 2;

    }

}
*/