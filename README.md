evil
# how to use
```
./build.sh
./aul < helloworld.aul
```
# standardy thing
`aX` stores the following number (X) in the variable a<br />
`bX` stores the following number (X) in the variable b<br />
`s` swaps the values of a and b<br />
`c` copies the value of a to b<br />
`i` inputs an ascii value into a<br />
`p` prints an ascii value from a<br />
`P` prints the value of a as an integer<br />
`x` prints the value of a as a hex byte<br />
`lX` defines a label X (labels are numbers)<br />
`gX` goes to a label X<br />
`GX` goes to a label X if the last operation resulted in a 0<br />
`rX` automatically pushes the return address to the call stack and jumps to a subroutine X<br />
`+`,`-`,`*`,`/` does an arithmetic operation on a and b as the operands<br />
`Sx` does a stack operation x (A to push a, B to push b, a to pop a, b to pop b)<br />
`CX` pushes a label X to the call stack<br />
`R` moves to the label on the top of the call stack and pops it<br />
`;` ignores everything for the rest of the line<br />
Whitespace is ignored<br />
Immediate values must be in the form of hexadecimal, and are limited to uint8_t<br />
    e.g. `aFFb01+p` outputs 00 due to wraparound yeah
