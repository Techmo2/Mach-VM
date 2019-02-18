// A very simple countdown

// Push the first value on the stack
pushn 10

// Jump to :main
:main

// If the counter isnt zero, jump to :printdec
jumpne 0 :printdec

// Print newline
pushc '\n'
emitc 1

// Halt the program
halt

// Print number, then decrement
:printdec
	// Print newline
	pushc '\n'
	emitc 1
	
	// Print current value
	printn
	
	// Subtract 1
	pushn 1
	subn
	
// Go back to :main
jump :main
