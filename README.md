# New_LFSR: VariableTap
Team: Mia Frattasio, Emma Herman, Felicia Moua, Amanda Robinson, Ryan Vernick

Problem: A traditional LFSR has the Berklekamp-Massey vulnaurability, which allows attackers to reverse-engineer the LFSR's internal state from the reconstruction of the minimal polynomial.

Tweak: A more dynamic feedback selection with variable taps and three LFSRs. 

Logic: Using two-taps, we set up a main LFSR C and two LSFRs known as A and B. Once LFSR A reaches its period, it asks LFSR C for 9 bits that are turned into LSFR As new gate. 

Performace: - is the speed the same as lfsr? i think?
complexity: no longer linear?

how to use program: 


