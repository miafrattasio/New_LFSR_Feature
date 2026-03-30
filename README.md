# New_LFSR: VariableTap
Team: Mia Frattasio, Emma Herman, Felicia Moua, Amanda Robinson, Ryan Vernick

Problem: A traditional LFSR has the Berklekamp-Massey vulnaurability, which allows attackers to reverse-engineer the LFSR's internal state from the reconstruction of the minimal polynomial.

Tweak: A more dynamic feedback selection with variable taps and three LFSRs. 

Logic: To midigate the vulnaurabilities of the LFSRs, we created a system in which three LFSRs would work to create more dynamic outputs. 
LFSR 1 acts as the controller and signals which taps the other LFSRs should use. LFSR 2 had two sets of taps that it chooses between. When LFSR 1 or 2 reaches its period, LFSR 3 provides a new gate. In the case of LFSR 2, two new gates are provided by LFSR 3.  

Performace: system is much more secure and beats the Berlekamp Massey Algorithm.

complexity: no longer linear with a much longer period that allows for a larger output for a small amount of bits. 

How to use the program: ./TwoTap.exe

Resources:

Ideas: https://ijcsmc.com/docs/papers/November2018/V7I11201826.pdf
https://www.cs.purdue.edu/homes/ninghui/courses/Fall05/lectures/355_Fall05_lect10.pdf

https://ieeexplore.ieee.org/document/4470524
https://www.geeksforgeeks.org/computer-networks/stream-ciphers/

https://www.partow.net/programming/polynomials/index.html#deg31

for code: https://www.geeksforgeeks.org/c/bitwise-operators-in-c-cpp/
https://www.geeksforgeeks.org/digital-logic/linear-feedback-shift-registers-lfsr/

https://www.ti.com/lit/an/scta036a/scta036a.pdf
gemini for small code fixes / debugging 


