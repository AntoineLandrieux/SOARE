
# SOARE documentation

---

## Table of Contents

- [Interpreter](#interpreter)

  - [Installing SOARE](#installing-soare)
  - [Compiling the Interpreter](#compiling-the-interpreter)
  - [Interpreter Commands](#interpreter-commands)
  - [Add Interpreter keywords/functions in C/C++](#add-interpreter-keywordsfunctions-in-cc)
  - [Loading a File](#loading-a-file)

- [SOARE](#soare)

  - [Introduction](#introduction)
  - [Text and numbers](#text-and-numbers)
  - [Storing data](#storing-data)
  - [defining function](#defining-functions)
  - [Loops and conditional structures](#loops-and-conditional-structures)
  - [Arrays](#arrays)
  - [User inputs](#user-inputs)
  - [Shell](#shell)
  - [Escape Sequence](#escape-sequence)

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

**Tools recommended for Windows**:

- [w64devkit](https://github.com/skeeto/w64devkit)

**Tools recommended for Linux**:

- `binutils`
- `build-essential`
- `make`

```sh
# Ubuntu/Debian
sudo apt update && sudo apt upgrade -y
sudo apt install binutils build-essential make -y
```

**Compilation**:

```sh
make
```

**Run executable**:

```sh
# Windows/Linux
make run

# Windows
.\bin\soare.exe
# Linux
./bin/soare
```

### Interpreter Commands

Executes the code written in the console

```txt
>>> write("Hello!");
Hello!
>>> 
```

**Editor**:

- `?editor`

Editor mode for multi-line input

```txt
>>> ?editor
... let msg = "hello";
... write(msg);
```

- `?run`

Execute the editor content

```txt
>>> ?editor
... let msg = "hello";
... write(msg);
... ?run
hello
>>>
```

- `?commit`

Execute the editor content (same as `?run`)

```txt
>>> ?editor
... let msg = "hello";
... write(msg);
... ?commit
hello
>>>
```

- `?cancel`

Cancels the code written in the editor mode

```txt
>>> ?editor
... let msg = "hello";
... write(msg);
... ?cancel
>>> 
```

- `?exit`

Closes the interpreter/editor mode

```soare
>>>?exit

Bye!

```

Closes the editor mode

```txt
>>> ?editor
... let msg = "hello";
... write(msg);
... ?exit
>>> 
```

### Add Interpreter keywords/functions in C/C++

> [!IMPORTANT]
> **See [core/Custom.c](../core/Custom.c)**
>

#### Functions

**Example**: Custom Function - Add Numbers with Variable Arguments

This example demonstrates how to implement a custom function that adds together an unknown number of integer arguments.

- Function Name:
  - char *int_add(soare_arguments_list args)
- Arguments:
  - args: A linked list of arguments passed to the function. Use soare_getarg(args, i) to retrieve the i-th argument as a string.
- Return Value:
  - Returns NULL (no value returned to SOARE).
  - If you want to return a value, allocate memory for the result string before returning.

**Implementation Steps**:

1. Initialize result accumulator.
2. Loop through arguments using soare_getarg.
3. Convert each argument from string to integer.
4. Add to result.
5. Print the result.
6. Return NULL (or a string if needed).

- Memory Management:
  - If returning a string, always allocate memory for it.
  - Example:

```c
char *ret = malloc(6 * sizeof(char));

if (!ret)
  return NULL;

strcpy(ret, "value");
return ret;
```

**Code**:

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

**Implement this function**: `soare_addfunction(<function name>, <function>)`

```c
soare_addfunction("int_add", int_add);
```

#### Keywords

**Example**: Custom Keyword - Clear screen

This example demonstrates how to implement a custom keyword.

- Function Name:
  - Example: `void clear(void)`

**Code**:

```c
void clear(void)
{
  // ANSI escape code to clear the screen
  printf("\033c\033[3J");
}
```

**Implement this keyword**: `soare_addkeyword(<keyword name>, <function>)`

```c
soare_addkeyword("clear", clear);
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

```soare
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

To display text, we use the `write` keyword:

```soare
write("Hello World!\n"); ? (1) A newline character or \n is a control character for Line feed
write(123.456);
```

To display errors, we use the `werr` keyword:

```soare
werr("Error: my error !");
```

>
> **(1)** See [Escape Sequence](#escape-sequence)
>

### Storing Data

A variable allows you to store a number or text. A variable is defined by a name and a value.

```soare
? let to create a new variable
let name = "Antoine"
let age = 15

? here the variable age already exists, so no need for the let  sign
age = age + 1

? the ',' sign with strings and numbers allows you to concatenate them
let hello_name = "Hello ", name;

? the write function support multiple-arguments with ';'
write(hello_name; " "; age; "yo");
```

### Defining Functions

We can create a function that writes the Fibonacci series:

```soare
fn fib(n)
  ? Print a Fibonacci series up to n.
  let a = 0;
  let b = 1;
  let next = b;

  while a < n do
    write(a; '\n');
    a = b;
    b = next;
    next = a + b;
  end;

  return a;
end;

write("The last value is "; fib(100));
```

The `fn` keyword introduces a function definition. It must be followed by the function name and the list of formal parameters in parentheses and separated by a ;.

The `return` keyword exits the function, returning a value to the place where the function was called.

### Loops and Conditional Structures

**while** :

The while loop executes as long as the condition (here: a < 10) remains true. In SOARE, as in C, any non-zero integer value is true; zero is false. The test used in the example is a simple comparison. The standard comparison operators are written the same way as in C: < (less than), > (greater than), == (equal to), <= (less than or equal to), >= (greater than or equal to), and != (not equal to).

```soare
? Fibonacci series:
? the sum of two elements defines the next
let a = 0;
let b = 1;
let next = b;

while a < 100 do
  write(a; '\n');
  a = b;
  b = next;
  next = a + b;
end;
```

**break** :

You can break a loop using `break`

```soare
let i = 0;

while 1 do

  if i > 10 do
    break;
  end;

  write(i; '\n');
  i = i + 1;

end;
```

**Result**:

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

**if** et **iferror** :

The most well-known type of statement is perhaps the if statement. For example:

```soare
try
  ? Include standard functions like NaN (Not a Number)
  loadimport "script/math.soare";

  let x = input("Enter a digit: ");

  if NaN(x) do
    write("\nNot a number");
  or x < 0 do
    write("\nNegative");
  or x == 0 do
    write("\nZero");
  else
    write("\nPositive");
  end;

iferror
  write("std.soare not found!")
end;
```

### Arrays

Strings or numbers are actually arrays of characters:

```soare
? For len() function
loadimport "script/std.soare";

let msg="Hello World!";

let index = 0;
? Returns the number of characters in the string
let size = len(msg);

while index < size do
  write(msg[index]; '\n');
  index = index + 1;
end;

? Negative number can be used to start with the end
write(msg[0-1]; '\n'); ? Write the last character of "msg", here "!"
write(msg[0-2]; '\n'); ? Write the one before last character of "msg", here "d"
```

### User Inputs

You can capture user input using `input`

```soare
let usr = input("Enter your name: ");
write("Hello "; usr; "!");
```

### Shell

- `system()` **Run shell command:**

```soare
system("echo hello");
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

### Predefined Functions

| name                  | function                        |
|-----------------------|---------------------------------|
| soareinfo()           | Show SOARE info                 |
| time()                | Show current timestamp          |
| random()              | Generate random number [0; 100] |
| system(...)           | Execute shell command           |
| input(...)            | User input, print text          |
| write(...)            | Print text                      |
| wer(...)              | Print error                     |
| ord(char)             | Get ASCII number from char      |
| chr(number)           | Get char from ASCII number      |

> SOARE Antoine LANDRIEUX <https://github.com/AntoineLandrieux/SOARE> (MIT LICENSE) ❤️
