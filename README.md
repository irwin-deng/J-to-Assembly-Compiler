# J-to-Assembly-Compiler


This is	a	compiler	that	will	compile code written in <a href="https://en.wikipedia.org/wiki/J_(programming_language)">J</a>,	a stack-oriented language, into	<a href="LC4 Instruction Set.pdf">assembly code for LC4</a>, an ISA created by Penn.	This	compiler	uses	the	same	calling	convention	as <a href="https://en.wikipedia.org/wiki/LCC_(compiler)">lcc</a>, so	J	programs	compiled with this can	call	subroutines	compiled	by	lcc	and	vice	versa.

To compile, run the following command:

```>>		make jc```

This	program	will	act	much	like	lcc	does,	when	provided with	a	properly	formatted	source	file	in	the	J	language	it	will	produce	the corresponding	assembly	code.		That	is

```>>		./jc	foo.j```

will	produce	a	new	file called `foo.asm`	if	`foo.j`	is a well formed J program.

### Implemented functionality
- Decimal and hexadecimal literals
- Arithmetic operations (+, -, *, /, %)
- Comparisons (<, <=, ==, >=, >)
- Logical operations (and, not, or)
- Stack operations (drop, dup, swap, rot, argN)
- If statements (supports nesting)
- Comments
- Functions
