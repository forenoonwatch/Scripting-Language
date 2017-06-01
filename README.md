# Scripting Language

A semester project I completed during my second semester of my senior year in high school. It is mostly functional as an interpreted scripting language, but its primary purpose was and still is to serve as a tool in order to learn what to do and what not to do when designing a scripting language.

### Syntax
The language was designed with relatively simple syntax, including soft typing and scope keywords that use words instead of symbols, for example `if/then ... end` instead of `if () { ... }`

It is inspired mostly by Lua and Basic, with smaller inspirations taken from languages such as C.

#### Example code:
```
fun fib(n)
  if n <= 2 then
    return 1
  end
  
  return fib(n - 1) + fib(n - 2)
end

print("Which fibonacci number do you want?")
let fibInput = get_input()
let fibNumber = fib(to_number(fibInput))
print("Your fibonacci number is " + fibNumber)
```

### Building the program
Download the GNU C++ compiler and a Makefile interpreter of your choice and run "make" in the main directory of the source code.

### Using the program
Type `lits [file name]` into the command line to run a source file. Note: there is currently no preferred nor required file extension.

You can also type simply `lits` or otherwise run the program with no arguments in order to open the simple built-in editor, where you can type out code and then type "exit" to quit and run the script.

### Future plans
This particular project is complete and will no longer be updated, but as stated before it functions as an educational tool in order for me to improve the design and efficiency of future script interpreters.
With this, I plan to create future scripting languages and scripting language interpreters using what I have learned here.
