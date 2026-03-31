#include <stdio.h>

//summary:
// Read the current states
// Controller/LFSR 1 decides the mode (LFSR2 taps a or b)
// LFSR 2 calulcates the output based on that
// Both registers shift to prepare for next cycle
// LFSR3 gives new gates to LFSRs 1 and 2 when they've reached their period


// taps for 9-bit primitive polynomials
    // A 9-bit LFSR is described by an 9th degree polynomial (you ignore the +1 I guess, that was messing with me)
    // These are known 9th degree primitives
    // Could be changed to be 32 bits. 
      // But, we should do ~64 bits, but that would require some extra trickiness that isn't needed if we stay within 32 bits, at least for the B-taps

#define LFSR_1_GATE   0b001101001  // 1 + x^3 + x^4 + x^6 + x^9
#define LFSR2_GATE_A  0b101101101  // 1 + x^1 + x^3 + x^4 + x^6 + x^7 + x^9
#define LFSR2_GATE_B  0b000100001  // 1 + x^4 x^9


/**
 * void clockCycle(int *stateA_ptr, int *stateB_ptr)
 * 
 * simulates a clock cycle on LFSR_A and the subsequent actions of LFSR_B, and prints the outpt of the clock cycle
 * 
 * inputs:
 *   int stateA_ptr: a pointer to the state of LFSR_A
 *   int stateB_ptr: a pointer to the state of LFSR_B
 * 
 * modifies:
 *   the integers pointed to by the inputs
*/
void clockCycle(int *lfsr1_state_ptr, int *lfsr2_state_ptr);

/**
 * int advance(int *state_ptr, int taps, int length)
 * 
 * advances a given LFSR
 * 
 * input:
 *  int *state: a pointer to a LFSR state
 *  int taps:   the gate positions of the passed LFSR
 *  int length  how many bits are in the state/gate of the LFSR
 * 
 * modifies:
 *  the state that state_ptr points to
 *
 * output:
 *  the output of the LFSR given the state and gates
*/
int advance(int *state_ptr, int taps, int length);

/**
 * int getFeedback(int state, int taps, int length)
 * 
 * takes a state and set of gates (taps) and returns the output bit of the LFSR
 * determines new bit that will be fed into the shift register
 * 
 * State and taps must both be of thength N, where N = length
 * 
 * input:
 *   current state of the N-bit LFSR
 *   gates for that LFSR
 *   how many bits are in the state/gate of the lfsr
 * 
 * output:
 *   1 or 0, depending on what the output bit is
*/
int getFeedback(int state, int taps, int length);

/**
 * void printBinary(int n, int bits)
 * 
 * prints the first bits of the binary representation of an integer,
 * for printing LFSRs, 'bits' is the length of the LFSR (# of registers)
 * 
 * inputs:
 *   n : an integer
 *   bits: in integer
*/
void printBinary(int n, int bits);


/**
 * void assignGate(int *m_gate_ptr, int length, int *state_ptr, int gate)
 * 
 * uses a given 9-bit LFSR to generate a new gate for another LFSR (of any length)
 * 
 * inputs:
 *   int *m_gate_ptr: a gate that need to be modified/updated
 *   int length: the number of bits in the gate being changed
 *   int *state: the current state of an LFSR
 *   int gate: the current gates of an LFSR
 * 
 * modifies:
 *  the state that state_ptr points to
 *  the gate that m_gate_ptr points to
 */
void assignGate(int *m_gate_ptr, int length, int *state_ptr, int gate);


FILE* outputFile;


int main() {

  outputFile = fopen("output.txt", "w");

  // Initial seeds/secret keys
  // if we start with all 0's, feedback with always be 0, register will be stuck
  // Total key is 36 "Truly Random" Bits.
  int LFSR1_INITIAL_STATE = 0b110101010; 
  int LFSR2_INITIAL_STATE = 0b101110001;
  int LFSR3_INITIAL_STATE = 0b010011001;
  int lfsr3_gate  = 0b110001101;


  // creating variables for the states and gates for all of the lfsrs
  int lfsr3_state = LFSR3_INITIAL_STATE;
  int lfsr3_next_gate;

  int lfsr1_state = LFSR1_INITIAL_STATE;
  int lfsr1_gate;

  int lfsr2_state = LFSR2_INITIAL_STATE;
  int lfsr2_gateA;
  int lfsr2_gateB;

  // assigning the initial gates and states where applicable
  assignGate(&lfsr3_next_gate, 9, &lfsr3_state, lfsr3_gate);
  assignGate(&lfsr1_gate, 9, &lfsr3_state, lfsr3_gate);
  assignGate(&lfsr2_gateA, 31, &lfsr3_state, lfsr3_gate);
  assignGate(&lfsr2_gateB, 31 , &lfsr3_state, lfsr3_gate);
  

  // and go!
  printf("Cycle | LFSR1 Bit | Tap Used | LFSR2 State | Output Bit\n");
  printf("-------------------------------------------------------\n");
  

  // we just need a bunch of clock cycles for now, this number can be changed later 
  // it's currently set to the LFSR1 maximum period
  int iterations = 10000; 

  for (int i = 0; i < iterations; i++) {

    clockCycle(&lfsr1_state, &lfsr2_state);

    // if lfsr1 has reached a state that could repeat, swap it out
    if (lfsr1_state == LFSR1_INITIAL_STATE){
      assignGate(&lfsr1_gate, 9, &lfsr3_state, lfsr3_gate);
    }
    // if lfsr 2 has reached a state that could repeat, swap out both sets of gates
    if (lfsr2_state == LFSR2_INITIAL_STATE){
        assignGate(&lfsr2_gateA, 31, &lfsr3_state, lfsr3_gate);
        assignGate(&lfsr2_gateB, 31 , &lfsr3_state, lfsr3_gate);
    }
    // if lfsr 2 has reached a state that will repeat, swap out both sets of gates
    // if it starts looping, that's not the end of the world, because the bits are being divided up so weirdly
    // between various LFSRs. If it lands cleanly on a repetition, reset it regardless.
    if (lfsr3_state == LFSR3_INITIAL_STATE){
      lfsr3_gate = lfsr3_next_gate;
      LFSR3_INITIAL_STATE = lfsr3_gate;

      assignGate(&lfsr3_next_gate, 9, &lfsr3_state, lfsr3_gate);
    }


  }

    return 0;
}





void clockCycle(int *lfsr1_state_ptr, int *lfsr2_state_ptr){

  static int cycles = 0;

  int lfsr1_state = *lfsr1_state_ptr;
  int lfsr2_state = *lfsr2_state_ptr;


  //controller/LFSR1
  int lfsr1Bit = getFeedback(lfsr1_state, LFSR_1_GATE, 9);
  
  // Choose taps for lsfr2 based on the controller lfsr(lfsr1)
  // this is where the modification happens, rather than using one set of taps, use if else to swap the rules
  // if the controller/lfsr1 outputs 1, lfsr2 uses taps b 1
  // if the controller/lfsr1 outputs 0, lfsr2 uses taps b 0
  // rewires lfsr2 on every clock cycle
  int currentTaps = (lfsr1Bit == 1) ? LFSR2_GATE_A : LFSR2_GATE_B;
  char* tapName = (lfsr1Bit == 1) ? "A" : "B";
  
  //lfsr using selected taps
  int lfsr2Bit = getFeedback(lfsr2_state, currentTaps, 31);

  int outputBit = lfsr2Bit ^ lfsr1Bit;
  
  //ouput the values before shifting
  //printf("%5d |    %d     |  %s   |  ", cycles, lfsr1Bit, tapName);
  //printBinary(lfsr2_state, 31);
  // deprecated line -> ////printf("  |    %d\n", lfsr2Bit);
  //printf("  |    %d\n", outputBit);


  ////fprintf(outputFile, "%d", lfsr2Bit);
  fprintf(outputFile, "%d", outputBit);
  if (cycles % 100 == 99){
    fprintf(outputFile, "\n");
  }
  
  // shift and update the states
  // stateA >> 1 means to shift all bits one position to the right (Which drops rightmost bit)
  // lfsr1Bit << 8 means to take the new feedback bit and move to 9th position (far most left pos, in a 9-bit LFSR)
  // the OR combines the shifted state with the new bit
  // & 0x1FF- that is binary 111111111, this is here to ensure that if any bits shifted past the 9th, they are dropped, keeping 9 bits
  *lfsr1_state_ptr = ((lfsr1_state >> 1) | (lfsr1Bit << (9-1))) & 0b111111111;  //lfsr1 is 9 bits long
  ////*lfsr2_state_ptr = ((lfsr2_state >> 1) | (lfsr2Bit << (31-1))) & 0b1111111111111111111111111111111; //lfsr2 is 31 bits long
  *lfsr2_state_ptr = ((lfsr2_state >> 1) | (outputBit << (31-1))) & 0b1111111111111111111111111111111; //lfsr2 is 31 bits long

  cycles ++;

}

int advance(int *state_ptr, int taps, int length){

  int state = *state_ptr;

  int outputBit = getFeedback(state, taps, length);

  *state_ptr = ((state >> 1) | (outputBit << (length-1)));

  return outputBit;

}

int getFeedback(int state, int taps, int length) {
  int xorSum = 0;
  for (int i = 0; i < length; i++) {

    // _&1 == _%2
    if ((taps >> i) & 1 && (state >> i) & 1) {
      xorSum++;
    }
  }
  // the XOR is the binary math, XORing a series of bits is like checking if the sum of 1s is even (0) or odd(1), and use % 2
  return xorSum % 2; 
}

void printBinary(int n, int bits){
  for (int i = bits-1; i >= 0; i--) {
    printf("%d", (n >> i) & 1);
  }
}

void assignGate(int *m_gate_ptr, int length, int *state_ptr, int gate){

  int newGate = 0;

  for(int i = 0; i < length; i++){

    int nextBit = advance(state_ptr, gate, 9);

    newGate = newGate | (nextBit << i);
  }

  *m_gate_ptr = newGate;

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
// gemini for small code fixes / debugging 
