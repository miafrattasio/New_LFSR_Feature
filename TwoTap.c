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

#define LFSR_1_GATE   0b001101001  // 1 + x^3 + x^4 + x^6 + x^9
#define LFSR2_GATE_A  0b101101101  // 1 + x^1 + x^3 + x^4 + x^6 + x^7 + x^9
#define LFSR2_GATE_B  0b000100001  // 1 + x^4 x^9

#define LFSR_LENGTH 9 


/*
void clockCycle(int *stateA_ptr, int *stateB_ptr)

simulates a clock cycle on LFSR_A and the subsequent actions of LFSR_B, and prints the outpt of the clock cycle

inputs:
  int stateA_ptr: a pointer to the state of LFSR_A
  int stateB_ptr: a pointer to the state of LFSR_B

modifies:
  the integers pointed to by the inputs

*/
void clockCycle(int *lfsr1_state_ptr, int *lfsr2_state_ptr);

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
void printBinary(int n, int bits)

prints the first bits of the binary representation of an integer,
for printing LFSRs, 'bits' is the length of the LFSR (# of registers)

inputs:
  n : an integer
  bits: in integer

*/
void printBinary(int n, int bits);





int main() {



  //Initial seeds/secret keys
  //if we start with all 0's, feedback with always be 0, register will be stuck
  //with two registers, total key is 18 bits
  int lfsr1_state = 0b110101010; 
  int lfsr2_state = 0b101110001;
  
  printf("Cycle | LFSR1 Bit | Tap Used | LFSR2 State | Output Bit\n");
  printf("-------------------------------------------------------\n");
  

  // we just need a bunch of clock cycles for now, this number can be changed later 
  // it's currently set to the LFSR1 maximum period
  int iterations = 511; 

  for (int i = 0; i < iterations; i++) {

    clockCycle(&lfsr1_state, &lfsr2_state);

  }
    return 0;
}





void clockCycle(int *lfsr1_state_ptr, int *lfsr2_state_ptr){

  static int cycles = 0;

  int lfsr1_state = *lfsr1_state_ptr;
  int lfsr2_state = *lfsr2_state_ptr;


  //controller/LFSR1
  int lfsr1Bit = getFeedback(lfsr1_state, LFSR_1_GATE);
  
  // Choose taps for lsfr2 based on the controller lfsr(lfsr1)
  //this is where the modification happens, rather than using one set of taps, use if else to swap the rules
  // if the controller/lfsr1 outputs 1, lfsr2 uses taps b 1
  // if the controller/lfsr1 outputs 0, lfsr2 uses taps b 0
  //rewires lfsr2 on every clock cycle
  int currentTaps = (lfsr1Bit == 1) ? LFSR2_GATE_A : LFSR2_GATE_B;
  char* tapName = (lfsr1Bit == 1) ? "A" : "B";
  
  //lfsr using selected taps
  int lfsr2Bit = getFeedback(lfsr2_state, currentTaps);
  
  //ouput the values before shifting
  printf("%5d |    %d     |  %s   |  ", cycles, lfsr1Bit, tapName);
  //!!! change this to 31 when you update LFSR2 to 31 bits
  printBinary(lfsr2_state, 9);
  printf("  |    %d\n", lfsr2Bit);
  
  //shift and update the states
  // stateA >> 1 means to shift all bits one position to the right (Which drops oldest bit?)
  // lfsr1Bit << 8 means to take the new feedback bit and move to 9th position (far most left pos)
  // the OR combines the shifted state with the new bit
  // & 0x1FF- that is binary 111111111, this is here to ensure that if any bits shifted past the 9th, they are dropped, keeping 9 bits
  *lfsr1_state_ptr = ((lfsr1_state >> 1) | (lfsr1Bit << (LFSR_LENGTH-1))) & 0b111111111;
  *lfsr2_state_ptr = ((lfsr2_state >> 1) | (lfsr2Bit << (LFSR_LENGTH-1))) & 0b111111111;

  cycles ++;

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

void printBinary(int n, int bits){
  for (int i = bits-1; i >= 0; i--) {
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
