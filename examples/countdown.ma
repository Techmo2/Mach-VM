// Push the first value on the stack
pushn 10

:main

// If the counter isnt zero, jump
jumpne 0 :printdec

halt

// Print number, then decrement
:printdec
	pushc '\n'
	emitc 1
	pushn 1
	subn
	printn
jump :main
