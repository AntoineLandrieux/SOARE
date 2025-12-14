
# SOARE Documentation

Welcome to the SOARE documentation! SOARE is a simple, interpreted programming language write in C, designed to be easy to learn, especially for beginners. This documentation will guide you step by step, explaining basic concepts with concrete examples. We'll draw from the original documentation and practical examples like those in the `test/array.soare` file.

---

## Table of Contents

- [What is SOARE?](#what-is-soare)
- [Interpreter](#interpreter)
  - [Installing SOARE](#installing-soare)
  - [Compiling the Interpreter](#compiling-the-interpreter)
  - [Interpreter Commands](#interpreter-commands)
  - [Add Interpreter Keywords/Functions in C/C++](#add-interpreter-keywordsfunctions-in-cc)
  - [Loading a File](#loading-a-file)
  - [Create Your Own Interpreter](#create-your-own-interpreter)
- [SOARE Language](#soare-language)
  - [Introduction](#introduction)
  - [Text and Numbers](#text-and-numbers)
  - [Storing Data](#storing-data)
  - [Defining Functions](#defining-functions)
  - [Loops and Conditional Structures](#loops-and-conditional-structures)
  - [Arrays](#arrays)
  - [User Inputs](#user-inputs)
  - [Escape Sequences](#escape-sequences)
  - [Predefined Functions](#predefined-functions)
  - [Practical Examples](#practical-examples)

---

## What is SOARE?

SOARE is an interpreted programming language, meaning you can write code and run it immediately without compiling first. It's simple, write in C, and perfect for learning the basics of programming. You can use it for scripts, calculations, or even more complex programs.

Before starting, make sure you have installed SOARE (see [Installing SOARE](#installing-soare)).

---

## Interpreter

The SOARE interpreter allows you to run code from the command line or via files.

### Installing SOARE

To install SOARE, download the release from the official repository: [https://github.com/AntoineLandrieux/SOARE/](https://github.com/AntoineLandrieux/SOARE/).

**With Git:**

```sh
git clone https://github.com/AntoineLandrieux/SOARE.git
```

On Windows, after compiling the interpreter (see below), run the `install.bat` file.

To launch SOARE:

```sh
# Linux
bin/soare
# Windows
%SOARE%\soare.exe
```

### Compiling the Interpreter

**Recommended Tools for Windows:**

- [w64devkit](https://github.com/skeeto/w64devkit)

**Recommended Tools for Linux:**

- `binutils`
- `build-essential`
- `make`

```sh
# Ubuntu/Debian
sudo apt update && sudo apt upgrade -y
sudo apt install binutils build-essential make -y
```

**Compilation:**

```sh
make
```

**Run Executable:**

```sh
# Windows/Linux
make run

# Windows
.\bin\soare.exe
# Linux
./bin/soare
```

### Interpreter Commands

The interpreter works in interactive mode. Type code and press Enter to execute it.

**Simple Execution:**

```txt
>>> write("Hello!");
Hello!
>>>
```

**With statement:**

```txt
>>> let x = 0;

>>> if (x < 0)
...   write("x is less than 0");
... or (x > 0)
...   write("x is greater than 0")
... else
...   write("x is equal to 0");
... end;
x is equal to 0
>>>
```

### Add Interpreter Keywords/Functions in C/C++

> [!IMPORTANT]
> See [core/Custom.c](../core/Custom.c) for examples.
>

#### Functions

**Example: Custom Function - Add Numbers with Variable Arguments:**

This example shows how to implement a custom function that adds an unknown number of integer arguments.

- Function Name:
  - `char *int_add(soare_arguments_list args)`
- Arguments:
  - `args`: A linked list of arguments passed to the function. Use `soare_getarg(args, i)` to retrieve the i-th argument as a string.
- Return Value:
  - Returns `NULL` (no value returned to SOARE).
  - If you want to return a value, allocate memory for the result string.

**Implementation Steps:**

1. Initialize the result accumulator.
2. Loop through arguments using `soare_getarg`.
3. Convert each argument from string to integer.
4. Add to the result.
5. Print the result.
6. Return `NULL` (or a string if needed).

- Memory Management:
  - If returning a string, always allocate memory.

**Example:**

```c
char *ret = malloc(6 * sizeof(char));

if (!ret)
  return NULL;

strcpy(ret, "value");
return ret;
```

**Code:**

```c
char *int_add(soare_arguments_list args)
{
  // Accumulator for the sum
  int result = 0;
  // Pointer to current argument string
  char *x = NULL;

  // Loop through all arguments
  for (int i = 0; 1; i++)
  {
    // Retrieve the i-th argument
    x = soare_getarg(args, i);
    if (!x)
      // Exit loop if no more arguments
      break;

    // Convert argument to integer and add to result
    result += atoi(x);
  }

  // Output the result to the console
  printf("%d", result);

  // Return NULL (no value returned to SOARE)
  // If you need to return a value, allocate memory as shown above
  return NULL;
}
```

**Implement this function:** `soare_addfunction(<function name>, <function>)`

```c
soare_addfunction("int_add", int_add);
```

#### Keywords

**Example: Custom Keyword - Clear Screen:**

This example shows how to implement a custom keyword.

- Function Name:
  - Example: `void clear(void)`

**Code:**

```c
void clear(void)
{
  // ANSI escape code to clear the screen
  printf("\033c\033[3J");
}
```

**Implement this keyword:** `soare_addkeyword(<keyword name>, <function>)`

```c
soare_addkeyword("clear", clear);
```

### Loading a File

To load a file, use the command:

```sh
soare "filename.soare"
```

### Create Your Own Interpreter

> [!IMPORTANT]
> Functions like `write`, `input`, or `system` are [predefined functions](#predefined-functions).
>
> Copy [`src/Predefined.c`](../src/Predefined.c) to create your own interpreter with these functions.
>

**Minimal Code:**

```c
#include <stdio.h>
#include <stdlib.h>
#include <SOARE/SOARE.h>

int main(void)
{
  // Initialize SOARE
  soare_init();

  // Predefined functions in SOARE
  // write, input, system...
  // See `src/Predefined.c`
  // predefined_functions();

  // Execute(filename, code)
  char *value = Execute("<input>", "return 'Hello World!';");

  // Print returned value and free it
  printf("%s", value);
  free(value);

  // Kill SOARE
  soare_kill();
  return 0;
}
```

---

## SOARE Language

Now, let's dive into the language itself. We'll start with the basics and progress to more advanced concepts.

### Introduction

SOARE programs consist of statements that the interpreter executes line by line. Comments start with `?` and are ignored by the interpreter. They are used to explain the code.

**Examples:**

```soare
? This is a comment
let number = 1; ? And this is another comment
                ? ... and one more!
let text = "? This is not a comment because it's inside quotes.";
```

> [!NOTE]
> You can use `;` at the end of a statement to indicate it is complete, but it's not always required.

### Text and Numbers

In SOARE, you can manipulate text (called strings) and numbers. Strings are enclosed in single quotes (`'...'`) or double quotes (`"..."`).

To display text, use `write`:

```soare
write("Hello World!\n"); ? \n is a newline character
write(123.456);
```

To display errors, use `werr`:

```soare
werr("Error: an error!");
```

> See [Escape Sequences](#escape-sequences) for more details on `\n`.

### Storing Data

Variables allow you to store values (text or numbers) for later reuse. Use `let` to create a new variable.

```soare
? Create a new variable with let
let name = "Antoine";
let age = 15;

? For an existing variable, no need for let
age = age + 1;

? Concatenate text with ,
let hello_name = "Hello ", name;

? write can take multiple arguments separated by ;
write(hello_name; " "; age; " years old");
```

**Beginner Tip:** Think of variables as boxes where you store things. You give the box a name and put a value in it.

### Defining Functions

Functions are reusable blocks of code. They take parameters and can return a value.

Example: A function to compute the Fibonacci sequence:

```soare
fn fib(n)
  ? Print the Fibonacci series up to n
  let a = 0;
  let b = 1;
  let next = b;

  while (a < n)
    write(a; '\n');
    a = b;
    b = next;
    next = a + b;
  end;

  return a;
end;

write("The last number is ", fib(100));
```

The `fn` keyword defines a function. Followed by the name and parameters in parentheses (separated by `;`).

`return` exits the function and returns a value.

**Tip:** Functions are like recipes: you call them with ingredients (parameters), they do something, and can give you a result.

### Loops and Conditional Structures

#### While Loops

A `while` loop repeats code as long as a condition is true.

```soare
? Fibonacci series
let a = 0;
let b = 1;
let next = b;

while (a < 100)
  write(a; '\n');
  a = b;
  b = next;
  next = a + b;
end;
```

In SOARE, like in C, any non-zero integer is true; zero is false. Comparison operators are `<`, `>`, `==`, `<=`, `>=`, `!=`.

#### Break

Use `break` to exit a loop prematurely.

```soare
let i = 0;

while (1) ? Infinite loop
  if (i > 10)
    break;
  end;

  write(i; '\n');
  i = i + 1;
end;
```

**Résultat :**

```txt
0
1
2
3
4
5
6
7
8
9
10
```

#### If and Iferror Conditions

`if` tests a condition and executes code if true. `or` allows alternative conditions, `else` for the otherwise case.

```soare
try
  ? Include standard functions like NaN (Not a Number)
  loadimport "script/math.soare";

  let x = input("Enter a number: ");

  if (NaN(x))
    write("Not a number\n");
  or (x > 0)
    write("Positive\n");
  or (x < 0)
    write("Negative\n");
  else
    write("Zero\n");
  end;

? Store the error name in the variable "error"
iferror as error
  werr("Error: "; error);
end;
```

`iferror` handles errors.

**Tip:** Think of conditions as decisions: "If it's raining, take an umbrella; otherwise, go for a walk."

### Arrays

In SOARE, strings and numbers are treated as arrays of characters. Access an element with `:` followed by the index (starting at 0).

```soare
let msg = "Hello";
let num = 713705;

write(msg:0; '\n'); ? Prints H
write(num:3; '\n'); ? Prints 7
```

The index can be a variable or negative (to count from the end).

```soare
loadimport "script/std.soare";

let msg = "Hello World!";

let index = 0;
let size = len(msg); ? Length of the string

while (index < size)
  write(msg:index; '\n');
  index = index + 1;
end;

? Negative to start from the end
write(msg:(0-1); '\n'); ? Last character: !
write(msg:(0-2); '\n'); ? Second to last: d
```

**Practical Example:** See the `test/array.soare` file for array tests, like counting vowels.

### User Inputs

Use `input()` to get user input.

```soare
let user = input("Enter your name: ");
write("Hello "; user; "!");
```

### Escape Sequences

Escape sequences start with `\` and represent special characters.

| Sequence | Name               | Description                                             |
|----------|--------------------|---------------------------------------------------------|
| `\a`     | Alarm or Beep      | Generates a bell sound.                                 |
| `\b`     | Backspace          | Moves the cursor one place backward.                    |
| `\f`     | Form Feed          | Moves the cursor to the start of the next logical page. |
| `\n`     | New Line           | Moves the cursor to the start of the next line.         |
| `\r`     | Carriage Return    | Moves the cursor to the start of the current line.      |
| `\t`     | Horizontal Tab     | Inserts whitespace and moves the cursor accordingly.    |
| `\v`     | Vertical Tab       | Inserts vertical space.                                 |
| `\\`     | Backslash          | Inserts the backslash character.                        |
| `\``     | Single Quote       | Displays a single quotation mark.                       |
| `\'`     | Single Quote       | Displays a single quotation mark.                       |
| `\"`     | Double Quote       | Displays double quotation marks.                        |
| `\ooo`   | Octal Number       | Represents an octal number.                             |
| `\xhh`   | Hexadecimal Number | Represents a hexadecimal number.                        |

### Predefined Functions

| Function     | Description                                       |
|--------------|---------------------------------------------------|
| chr(integer) | Get char from ASCII number                        |
| eval(code)   | Execute SOARE code and return value               |
| exit(status) | Quit SOARE                                        |
| input(...)   | User input, print text                            |
| ord(char)    | Get ASCII number from char                        |
| random(seed) | Generate a random number [0; 255] based on a seed |
| system(...)  | Execute shell command                             |
| time()       | Show current timestamp                            |
| werr(...)    | Print error                                       |
| write(...)   | Print text                                        |

### Practical Examples

Based on `test/array.soare`, here are examples to practice with arrays.

**Indexing Test:**

```soare
let msg = "Hello";
let num = 713705;

write("msg:0 -> '"; msg:0; "'\n"); ? Expected: H
write("num:3 -> '"; num:3; "'\n"); ? Expected: 7
```

**Iteration and Vowel Counting:**

```soare
loadimport "script/std.soare";

let s = "Hello World!";
let i = 0;
let n = len(s);
let vowels = 0;

while (i < n)
  let ch = s:i;
  if (ch == "a" or ch == "e" or ch == "i" or ch == "o" or ch == "u" or ch == "y")
    vowels = vowels + 1;
  end;
  i = i + 1;
end;

write("Number of vowels: "; vowels; '\n');
```

**Reversing a String:**

```soare
loadimport "script/std.soare";

let original = "Hello";
let reversed = "";
let i = len(original) - 1;

while (i >= 0)
  reversed = reversed, original:i;
  i = i - 1;
end;

write("Original: "; original; '\n');
write("Reversed: "; reversed; '\n');
```

These examples will help you understand how to manipulate strings as arrays. Experiment by changing the values!

---

> SOARE Antoine LANDRIEUX <https://github.com/AntoineLandrieux/SOARE> (MIT LICENSE) ❤️
