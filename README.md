# MACH Virtual Machine

Mach is a simple virtual machine designed for speed, and simplicity. Mach uses single byte instructions with data embedded within the program binary.

# Building Mach
To compile, simply enter the project directory and run ```make```. You can also run ```make clean``` and ```make debug``` to clean the project directory, and build in debug mode respectively.

# Writing Programs
Currently, programs for mach are written in assembly. Mach uses a simple, stack based assembly language.

Here's an example of a program that prints "Hello!" to the console:
```lua
pushc '\n'
pushc '!'
pushc 'o'
pushc 'l'
pushc 'l'
pushc 'e'
pushc 'h'
emitc 6
halt
```

Here's an example of a program that multiplies two numbers, then prints the result to the console:
```lua
pushn 166.5
pushn 15
multn
emitn 1
```

# Compiling Programs
Mach comes with an assembler written in java. Eventually the assembler will be written in c as well, but for now I use java just to get development moving along. To use the assembler, you must have java installed. The assembler was built with java 10, but other versions should work.

To assemble using the assembly script:
```bash
./mass <yourassemblyfile>
```

To assemble with the assembler directly:
```bash
java -jar mass.jar <yourassemblyfile>
```

Currently all assembled programs will be written to ```out.m```, but an option to specify the output will be added soon.



# Using Mach

To use the virtual machine, simply execute the mach binary and specify a path to a compiled mach program.
```./mach coolprogram.m```

# Mach Instructions
| Instruction (1 Byte) | Parameter Types   | Parameter Examples | Parameter Sizes | Notes                                                                                                    |
|----------------------|-------------------|--------------------|-----------------|----------------------------------------------------------------------------------------------------------|
| pushc                | character literal | '\n'               | 1 Byte          | Pushes a character onto the stack                                                                        |
| pushn                | double literal    | 126.04             | 8 Bytes         | Pushes a number onto the stack                                                                           |
| emitc                | integer literal   | 6                  | 4 Bytes         | Pops a character off the stack. Prints to console.                                                       |
| emitn                | integer literal   | 120                | 4 Bytes         | Pops a number off the stack. Prints to console.                                                          |
| multn                |                   |                    | 0 Bytes         | Pops 2 numbers off the stack. Pushes the product back on the stack.                                      |
| addn                 |                   |                    | 0 Bytes         | Pops 2 numbers off the stack. Pushes the product back on the stack.                                      |
| divn                 |                   |                    | 0 Bytes         | Pops 2 numbers off the stack. Pushes the quotient back on the stack.                                     |
| subn                 |                   |                    | 0 Bytes         | Pops 2 numbers off the stack. Pushes the difference back on the stack.                                   |
| jump                 | label             | :function_start    | 4 Bytes         | Jumps to the specified label in the program. The label is replaced by an integer offset during assembly. |
| halt                 |                   |                    | 0 bytes         | Stops execution of the program, and closes the virtual machine.                                          |
