
# SOARE documentation

---

## Table of Contents

- [Interpreter](#interpreter)

  - [Installing SOARE](#installing-soare)
  - [Compiling the Interpreter](#compiling-the-interpreter)
  - [Interpreter Commands](#interpreter-commands)
  - [Loading a File](#loading-a-file)

- [SOARE](#soare)

  - [Introduction](#introduction)
  - [Text and numbers](#text-and-numbers)
  - [Storing data](#storing-data)
  - [defining function](#defining-functions)
  - [Loops and conditional structures](#loops-and-conditional-structures)
  - [Arrays](#arrays)
  - [User inputs](#user-inputs)
  - [Shell and Reinterpret](#shell-and-reinterpret)
  - [Escape Sequence](#escape-sequence)
  - [Predefined variables](#predefined-variables)

---

## Interpreter

### Installing SOARE

To install SOARE, you can download the source code or the release from the [official repo](https://github.com/AntoineLandrieux/SOARE/).

**With git** :

```sh
git clone https://github.com/AntoineLandrieux/SOARE.git
```

On Windows, after [compiling the interpreter](#compiling-the-interpreter), you can run the `install.bat` file.

To launch SOARE:

```sh
# Linux
bin/soare
# Windows
%SOARE%\soare.exe
```

### Compiling the Interpreter

**Dependencies** :

- `ar`
- `g++`
- `gcc`
- `make`
- `windres` (for Windows only)

```sh
make
```

### Interpreter Commands

- `?run`

Executes the code written in the console

```txt
>>> write "Hello!"
>>> ?run
Hello!
>>> ?run
Hello!
>>>
```

- `?commit`

Executes the code written in the console once

```txt
>>> write "Hello!";
>>> ?commit
Hello!
>>> ?commit

>>>
```

- `?cancel`

Cancels the code written in the console

```txt
>>> write "Hello!";
>>> ?cancel
>>> ?commit

>>>
```

- `?clear`

Clears the console content

```txt
>>>
```

- `?exit`

Closes the interpreter

```txt
>>>?exit

Bye!

```

### Loading a File

To load a file, you can use the command:

```sh
soare "filename.soare"
```

---

## SOARE

### Introduction

Many examples in this manual, even those entered at the interactive prompt, include comments. Comments in SOARE begin with the character ?, and extend to the end of the physical line. A comment can appear at the start of a line or after whitespace or code, but not inside a string literal. Since comments are intended to clarify the code and are not interpreted by SOARE, they can be omitted when typing in examples.

**Some examples** :

```txt
? this is the first comment
let number = 1; ? and this is the second comment
             ? ... and now a third !
let text = "? This is not a comment because it's inside quotes.";
```

> [!NOTE]
>
> You can use ; at the end of a statement to indicate that it is complete.
>

### Text and Numbers

SOARE can manipulate text (represented by a string) as well as numbers. This includes characters like "!", words like "rabbit", names like "Paris", sentences like "I support you.", etc. "Yay!". They can be enclosed in single quotes ('...') or double quotes ("...") with the same result.

To display text, we use the write keyword:

```txt
write "Hello World!\n"; ? (1) A newline character or \n is a control character for Line feed
write 123.456;
```

>
> **(1)** See [Escape Sequence](#escape-sequence)
>

### Storing Data

A variable allows you to store a number or text. A variable is defined by a name and a value.

```txt
? let to create a new variable
let name = "Antoine"
let age = 15

? here the variable age already exists, so no need for the let  sign
age = age + 1

? the ',' sign with strings and numbers allows you to concatenate them
write "Hello ", name, " ", age, "yo";
```

### Defining Functions

We can create a function that writes the Fibonacci series:

```txt
fn fib(n)
  ? Print a Fibonacci series up to n.
  let a = 0;
  let b = 1;
  let next = b;
  
  while a < n do
    write a, '\n';
    a = b;
    b = next;
    next = a + b;
  end;
  
  return a;
end;

write "The last value is ", fib(100);
```

The `fn` keyword introduces a function definition. It must be followed by the function name and the list of formal parameters in parentheses and separated by a ;.

The `return` keyword exits the function, returning a value to the place where the function was called.

### Loops and Conditional Structures

**while** :

The while loop executes as long as the condition (here: a < 10) remains true. In SOARE, as in C, any non-zero integer value is true; zero is false. The test used in the example is a simple comparison. The standard comparison operators are written the same way as in C: < (less than), > (greater than), == (equal to), <= (less than or equal to), >= (greater than or equal to), and != (not equal to).

```txt
? Fibonacci series:
? the sum of two elements defines the next
let a = 0;
let b = 1;
let next = b;

while a < 100 do
  write a, '\n';
  a = b;
  b = next;
  next = a + b;
end;
```

**if** et **iferror** :

The most well-known type of statement is perhaps the if statement. For example:

```txt
try
  ? Include standard functions
  loadimport "script/math.soare";

  write "Enter a digit:";
  let x = 0;
  inputch x;

  if NaN(x) do
    write "\nNot a number"
  or x < 0 do
    write "\nNegative";
  or x == 0 do
    write "\nZero";
  else
    write "\nPositive";
  end;

iferror
  write "std.soare not found!"
end;
```

### Arrays

Strings or numbers are actually arrays of characters:

```txt
? For len() function
loadimport "script/std.soare";

let msg="Hello World!";

let index = 0;
? Returns the number of characters in the string
let size = len(msg);

while index < size do
  write msg[index], '\n';
  index = index + 1;
end;

? Negative number can be used to start with the end
write msg[0-1], '\n'; ? Write the last character of "msg", here "!"
write msg[0-2], '\n'; ? Write the one before last character of "msg", here "d"
```

### User Inputs

You can capture user input using `input`

```txt
let usr = "";

write "Enter your name: ";
input usr;

write "Hello ", usr, "!";
```

### Shell and Reinterpret

- `$` -> Run shell command
- `@` -> Reinterpret code

**Run shell command:**

```txt
? Clear terminal

? __PLATFORM__ : Get OS name
if __PLATFORM__ == "Windows" do
  $"cls";
else
  $"clear";
end;

$"echo Terminal cleared"
```

**Reinterpret code:**

```txt
let msg = "Hello";
@"write \"", msg, "\";";
```

### Escape Sequence

| Escape Sequence | Name               | Description                                                                            |
|-----------------|--------------------|----------------------------------------------------------------------------------------|
| \\a             | Alarm or Beep      | It is used to generate a bell sound.                                                   |
| \\b             | Backspace          | It is used to move the cursor one place backward.                                      |
| \\f             | Form Feed          | It is used to move the cursor to the start of the next logical page.                   |
| \\n             | New Line           | It moves the cursor to the start of the next line.                                     |
| \\r             | Carriage Return    | It moves the cursor to the start of the current line.                                  |
| \\t             | Horizontal Tab     | It inserts some whitespace to the left of the cursor and moves the cursor accordingly. |
| \\v             | Vertical Tab       | It is used to insert vertical space.                                                   |
| \\\\            | Backlash           | Use to insert backslash character.                                                     |
| \\`             | Single Quote       | It is used to display a single quotation mark.                                         |
| \\'             | Single Quote       | It is used to display a single quotation mark.                                         |
| \\"             | Double Quote       | It is used to display double quotation marks.                                          |
| \\ooo           | Octal Number       | It is used to represent an octal number.                                               |
| \\xhh           | Hexadecimal Number | It represents the hexadecimal number.                                                  |

### Predefined Variables

| name              | value                 |
|-------------------|-----------------------|
| \_\_SOARE\_\_     | `SOARE (MIT LICENSE)` |
| \_\_ERROR\_\_     | *errors*              |
| \_\_BUILD\_\_     | *build date*          |
| \_\_PLATFORM\_\_  | *current OS*          |

> SOARE Antoine LANDRIEUX <https://github.com/AntoineLandrieux/SOARE> (MIT LICENSE)
