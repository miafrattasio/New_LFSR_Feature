//NEED TO UPDATE BASED ON FRIDAY'S MEETING (Currently two tap vs amanda's addition/ + the 3 lfsr registers thing)

#include <stdio.h>

//summary:
//read the current states
// controller/LFSR1 decides the mode (LFSR2 a or b)
// lfsr2 calulcates the output based on that
// both registers shift to prepare for next cycle
// when you run, the output bit is what the attacker would see, the rest is for our viewing


// taps for 9-bit primitive polynomials
    // a 9-bit LFSR is described by an 9th degree polynomial (you ignore the +1 I guess, that was messing with me)
    // I've changed these so that they are known 9th degree primitives
    // we should consider changing these to be 32 bits. 
      // actually we should do ~64 bits, but that would require some extra trickiness that isn't needed if we stay within 32 bits
      // at least for the B-taps
  
#define TAPS_A      0b001101001  // x^9 + x^6 + x^4 + x^3 + 1
#define TAPS_B_ONE  0b101101101  // x^9 + x^7 + x^6 + x^4 + x^3 + x^1 + 1
#define TAPS_B_ZERO 0b010000111  // x^9 + x^8 + x^7 + x^2 + 1

#define LFSR_LENGTH 9 


/*
int getFeedback(int state, int taps)

  takes a state and set of gates (taps) and returns the output bit of the LFSR
  State and taps must both be of thength N, where N = LFSR_LENGTH

  input:
    state of the N-bit LFSR
    gates for that LFSR

  output:
    1 or 0, depending on what the output bit is
*/
int getFeedback(int state, int taps);

/*
void printBinary(int n)

prints the first N bits of the binary representation of an integer,
where N = LFSR_LENGTH

inputs:
  n : an integer
*/
void printBinary(int n);




int main() {
  //Initial seeds/secret keys
  //if we start with all 0's, feedback with always be 0, register will be stuck
  //with two registers, total key is 18 bits
  int stateA = 0b110101010; 
  int stateB = 0b101110001;
  int iterations = 511; //need to pick a legnth? random number
  
  printf("Cycle | LFSR1 Bit | Tap Used | LFSR2 State | Output Bit\n");
  printf("-------------------------------------------------------\n");
  
  for (int i = 0; i < iterations; i++) {
    //controller/LFSR1
    int lfsr1Bit = getFeedback(stateA, TAPS_A);
    
    // Choose taps for lsfr2 based on the controller lfsr(lfsr1)
    //this is where the modification happens, rather than using one set of taps, use if else to swap the rules
    // if the controller/lfsr1 outputs 1, lfsr2 uses taps b 1
    // if the controller/lfsr1 outputs 0, lfsr2 uses taps b 0
    //rewires lfsr2 on every clock cycle
    int currentTaps = (lfsr1Bit == 1) ? TAPS_B_ONE : TAPS_B_ZERO;
    char* tapName = (lfsr1Bit == 1) ? "a" : "b ";
    
    //lfsr using selected taps
    int lfsr2Bit = getFeedback(stateB, currentTaps);
    
    //ouput the values before shifting
    /*
    printf("%5d |    %d     |  %s   |  ", i, lfsr1Bit, tapName);
    printBinary(stateB);
    printf("  |    %d\n", lfsr2Bit);
    */

    printf("%d", lfsr1Bit);
    
    //shift and update the states
    // stateA >> 1 means to shift all bits one position to the right (Which drops oldest bit?)
    //lfsr1Bit << 8 means to take the new feedback bit and move to 9th position (far most left pos)
    // the OR combines the shifted state with the new bit
    //& 0x1FF- that is binary 111111111, this is here to ensure that if any bits shifted past the 9th, they are dropped, keeping 9 bits
    stateA = ((stateA >> 1) | (lfsr1Bit << 8)) & 0x1FF;
    stateB = ((stateB >> 1) | (lfsr2Bit << 8)) & 0x1FF;
  }
    return 0;
}


// helper function to get the feedback bit
// determines new bit that will be fed into the shift register
// inputs take current state (bits currently in register) and the taps (positions that are wired together to created the feedback)
// the XOR is the binary math, XORing a series of bits is like checking if the sum of 1s is even (0) or odd(1), and use % 2
int getFeedback(int state, int taps) {
  int xorSum = 0;
  for (int i = 0; i < LFSR_LENGTH; i++) {

    // _&1 == _%2
    if ((taps >> i) & 1 && (state >> i) & 1) {
      xorSum++;
    }
  }
  return xorSum % 2; 
}


void printBinary(int n){
  for (int i = LFSR_LENGTH-1; i >= 0; i--) {
    printf("%d", (n >> i) & 1);
  }
}



//resources:

// ideas: https://ijcsmc.com/docs/papers/November2018/V7I11201826.pdf
// https://www.cs.purdue.edu/homes/ninghui/courses/Fall05/lectures/355_Fall05_lect10.pdf

// for this specific thing: https://ieeexplore.ieee.org/document/4470524
// https://www.geeksforgeeks.org/computer-networks/stream-ciphers/
//
// https://www.partow.net/programming/polynomials/index.html#deg31

// for code: https://www.geeksforgeeks.org/c/bitwise-operators-in-c-cpp/
// https://www.geeksforgeeks.org/digital-logic/linear-feedback-shift-registers-lfsr/

// https://www.ti.com/lit/an/scta036a/scta036a.pdf
// gemini for small code fixes / debugging (he said this was okay?)
