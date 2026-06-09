# US Secure Hash Algorithm 1

[RFC3174](https://datatracker.ietf.org/doc/html/rfc3174)

---

##  Definitions of Bit Strings and Integers

* A **hex digit**:
  * A hex digit is an element of the set {0, 1, ... , 9, A, ... , F}.
  * A hex digit is the representation of a 4-bit string.

* A **world**:
  * A word equals a 32-bit string which may be represented as a
    sequence of 8 hex digits.
  * An integer between 0 and 2^32 - 1 inclusive may be represented as
    a word.
  * The least significant four bits of the integer are
     represented by the right-most hex digit of the word
     representation. (Big Endian)

* 64-bit integer
  * If `z` is an integer, `0 <= z < 2^64`, then `z = (2^32)x + y` where 
    `0 <= x < 2^32` and `0 <= y < 2^32`.  
  * Since x and y can be represented as words X and Y, respectively,
    z can be represented as the pair of words (X,Y).
  
* A **block**:
  * block = 512-bit string.  A block (e.g., B) may be represented as a
    sequence of 16 words. 

---

## Operations on Words

### X+Y

The operation X + Y is defined as follows: 
words X and Y represent integers x and y, where:
 * `0 <= x < 2^32` 
 * `0 <= y < 2^32`

For positive integers n and m, let n mod m be the remainder upon
dividing n by m.  Compute

         `z  =  (x + y) mod 2^32`  

### Circular left shift

The circular left shift operation `S^n(X)`, where `X` is a word and `n`
is an integer with `0 <= n < 32` , is defined by

    `S^n(X)  =  (X << n) OR (X >> 32-n).`

---

##  Message Padding

* The length of the message is the number of bits in the message
* If the number of bits in a message is a multiple of 8, for compactness
  we can represent the message in hex.
* The purpose of message padding
  is to make the total length of a padded message a multiple of **512**.

The following specifies how this padding shall be
performed.

* As a summary, a "1" followed by m "0"s followed by a 64-
  bit integer are appended to the end of the message to produce a
  padded message of length 512 * n.
* The 64-bit integer is the length of the original message.
* The padded message is then processed by the SHA-1 as n 512-bit blocks.
* The last 64 bits of the last 512-bit block
  are reserved  for the length l of the original message.

`
message + "1" + "0000000...." + 64bit_integer
`
---

## Functions and Constants Used

* A sequence of logical functions `f(0), f(1),..., f(79)` is used in
  SHA-1.
* Each `f(t), 0 <= t <= 79`, operates on three 32-bit words `B, C, D`
 and produces a 32-bit word as output.
 
* `f(t;B,C,D)` is defined as follows: for words `B, C, D,`:
    * `( 0 <= t <= 19)` =>  `f(t;B,C,D) = (B AND C) OR ((NOT B) AND D)`
    * `(20 <= t <= 39)` => `f(t;B,C,D) = B XOR C XOR D`
    * `(40 <= t <= 59)` => `f(t;B,C,D) = (B AND C) OR (B AND D) OR (C AND D)`
    * `(60 <= t <= 79)` => `f(t;B,C,D) = B XOR C XOR D`
* `K(t)`  hex values:
  * `( 0 <= t <= 19)` => `K(t) = 5A827999`
  * `(20 <= t <= 39)` => `K(t) = 6ED9EBA1`
  * `(40 <= t <= 59)` => `K(t) = 8F1BBCDC`
  * `(60 <= t <= 79)` => `K(t) = CA62C1D6`

---

# Method 1

The computation is described using:
* two buffers, each consisting of five 32-bit words
  * The words of the first 5-word buffer are labeled `A,B,C,D,E`
  * The words of the second 5-word buffer are labeled `H0, H1, H2, H3, H4`. 
* and a sequence of eighty 32-bit words
  *  The words of the 80-word sequence are labeled `W(0), W(1),..., W(79).`
* A single word buffer TEMP is also employed.

Before processing any blocks, the H's are initialized as follows:
 * `H0 = 67452301`
 * `H1 = EFCDAB89`
 * `H2 = 98BADCFE`
 * `H3 = 10325476`
 * `H4 = C3D2E1F0`

To generate the message digest, the 16-word blocks M(1), M(2),...,
M(n) defined in section 4 are processed in order.  The processing of
each M(i) involves 80 steps.
    
* Divide `M(i)` into 16 words `W(0), W(1), ... , W(15)`, where W(0) is the left-most word.
* For `t = 16 to 79` let
    ```
    W(t) = S^1(W(t-3) XOR W(t-8) XOR W(t-14) XOR W(t-16)).
    ```
* Let 
    ```
    A = H0, B = H1, C = H2, D = H3, E = H4
    ```
* For `t = 0 to 79` do 
    ```
    TEMP = S^5(A) + f(t;B,C,D) + E + W(t) + K(t);
    E = D;  D = C;  C = S^30(B);  B = A; A = TEMP;
    ```
* Let 
    ```
    H0 = H0 + A, H1 = H1 + B, H2 = H2 + C, H3 = H3 + D, H4 = H4 + E.
    ```
* After processing M(n), the message digest is the 160-bit string
    represented by the 5 words
    ```
    H0 H1 H2 H3 H4
    ```  
