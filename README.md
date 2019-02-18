# MACH Virtual Machine

Mach is a very simple virtual machine. This project was written to teach myself about how virtual machines function, and how various factors can effect its speed and feature set.
The binaries used by this virtual machine do not have separate .text and .data sections. Instead, data used by an instruction is written directly after its op code. This makes assembly and execution easier, but adds more complexity to jumps, and ensuring that data isn't read as an op code.

# Writing Programs
Currently, programs for mach are written in a simplified assembly. Mach uses a simple, stack based assembly language.

Here's an example of a program that prints "Hello!" to the console. Notice that since we are using a stack, characters must be added in reverse in order for them to be emitted in the correct order.
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

Here's an example of a program that multiplies two numbers, then prints the result to the console.
```lua
pushn 166.5
pushn 15
multn
emitn 1
```

# Compiling Programs
A few example programs can be found in the **examples** folder. To assemble a mach program, you can have two options:

Assemble using the assembly script:
```bash
./mass <yourassemblyfile>
```

Assemble with the assembler directly:
```bash
java -jar mass.jar <yourassemblyfile>
```

Currently all assembled programs will be written to ```out.m```, but an option to specify the output will be added soon.
The assembler will eventually be written in c.



# Using Mach

To use the virtual machine, simply execute the mach binary and specify a path to a compiled mach program.
```./mach coolprogram.m```

# Instruction Set
| Instruction | Example                 | Parameter Bytes | Description                                                                                                  |  Layout                    |
|-------------|-------------------------|-----------------|--------------------------------------------------------------------------------------------------------------|--------------------------|
| pushc       | ```pushc '\s'```              | 1               | Pushes a given character onto the stack.                                                                     | ```pushc <char>```             |
| emitc       | ```emitc 1```               | 4               | Pops the given number of characters off of the stack and prints them to the console.                         | ```emitc <number>```           |
| printc      | ```printc 1```                | 4               | Prints the given number of characters without popping them off of the stack.                                 | ```printc <number>```          |
| pushn       | ```pushn 0.75```              | 8               | Pushes the given number onto the stack.                                                                      | ```pushn <number>```           |
| emitn       | ```emitn 1```                 | 4               | Pops the given number of numbers off of the stack and prints them to the console.                            | ```emitn <number>```           |
| printn      | ```printn 1```                | 4               | Prints the given number of numbers without popping them off of the stack.                                    | ```printn <number>```          |
| multn       | ```multn```                   | 0               | Pops 2 numbers off of the stack then pushes the product back onto the stack.                                 | ```multn```                    |
| addn        | ```addn```                    | 0               | Pops 2 numbers off of the stack then pushes the sum back onto the stack.                                     | ```addn```                     |
| subn        | ```subn```                    | 0               | Pops 2 numbers off of the stack then pushes the difference back onto the stack.                              | ```subn```                     |
| divn        | ```divn```                    | 0               | Pops 2 numbers off of the stack then pushes the quotient back onto the stack.                                | ```divn```                     |
| pop         | ```pop```                     | 0               | Pops an item off of the stack.                                                                               | ```pop```                      |
| grow        | ```grow```                    | 0               | Clones the item at the top of the stack.                                                                     | ```grow```                     |
| popt        | ```popt 3```                  | 4               | Pops an item off the stack and stores it at the given table index.                                           | ```popt <number>```            |
| pullt       | ```pullt 3```                 | 4               | Pushes an item onto the stack from the given table index.                                                    | ```pullt <number>```           |
| jump        | ```jump :somewhere```         | 4               | Jumps to the given label.                                                                                    | ```jump <label>```             |
| jumplt      | ```jumplt 10 :somewhere```    | 12              | Jumps to the given label if the number on the top of the stack is less than the given number.                | ```jumplt <number> <label>```  |
| jumpgt      | ```jumpgt 0 :positive```      | 12              | Jumps to the given label if the number at the top of the stack is greater than the given number.             | ```jumpgt <number> <label>```  |
| jumpe       | ```jumpe 42 :theanswer```     | 12              | Jumps to the given label if the number at the top of the stack is equal to the given number.                 | ```jumpe <number> <label>```   |
| jumpne      | ```jumpne 0 :nonzero```       | 12              | Jumps to the given label if the number at the top of the stack is not equal to the given number.             | ```jumpne <number> <label>```  |
| jumplte     | ```jumplte 100 :somewhere```  | 12              | Jumps to the given label if the number at the top of the stack is less than or equal to the given number.    | ```jumplte <number> <label>``` |
| jumpgte     | ```jumpgte 1000 :bignumber``` | 12              | Jumps to the given label if the number at the top of the stack is greater then or equal to the given number. | ```jumpgte <number> <label>``` |
| halt        | ```halt```                    | 0               | Halts execution.                                                                                             | ```halt```                     |
| ptime    | ```ptime```                | 0               | Pushes the current system time in nanoseconds onto the stack.                                                | ```ptime```                 |
