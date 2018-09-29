// Calculate the factorial of 15
pushn 15

// Store the number in some registers
grow
grow
popr 1
popr 2

:loop
	pullr 1
	pushn 1
	subn
	grow
	popr 1
	jumplte 1 :end
	multn
jump :loop

:end

// Print string before result
pullr 2
emitn 1
pushc '='
pushc '!'
emitc 2

// Print result
pop
printn

// Newline
pushc '\n'
emitc 1

halt
