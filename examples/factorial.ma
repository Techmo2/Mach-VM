// Calculate the factorial of 15
// You can put anything reasonable here
pushn 15

// Store the number in some registers
grow
grow
popt 1
popt 2

:loop
	pullt 1
	pushn 1
	subn
	grow
	popt 1
	jumplte 1 :end
	multn
jump :loop

:end

// Print string before result
pullt 2
emitn 1
pushc '\s'
pushc '='
pushc '\s'
pushc '!'
emitc 4

// Print result
pop
printn

// Newline
pushc '\n'
emitc 1

halt
