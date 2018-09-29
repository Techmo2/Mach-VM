// How long does it take to count to 1 billion?

// Print "start"
pushc '\n'
pushc 't'
pushc 'r'
pushc 'a'
pushc 't'
pushc 's'

emitc 6

// Push the current system time to the stack
ptime

// Push a new number to the stack
// We will use this as a counter
pushn 0

:start

// While the counter is less than or equal to 1,000,000,000
// add 1 to the number at the top of the stack
jumplte 1000000000 :increment

// Pop the counter off the stack, we don't need it anymore
pop

// Push the current time on the stack
ptime

// Subtract the current time from the start time
subn

// Multiply the result so it is positive
pushn -1
multn

// Divide the result so it is in seconds instead of nanoseconds
pushn 100000000000
divn

// Print the calculated runtime and pop it off the stack
emitn 1

// Halt
halt

// Increment the number at the top of the stack
:increment
pushn 1
addn
jump :start
