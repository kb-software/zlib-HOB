<!-- # zlib-HOB
 modified HOB zLib-source
 -->

# The HOB Precompiler PRECOMP
```
        Contents
             Overview 2
         2
             List of commands 3
             1.1. Commands for the Precompiler: 4
               1.1.1. SET 4
               1.1.2. DEFT 4
               1.1.3. Creating text conditionally 4
               1.1.4. CANCEL: Canceling the output 5
               1.1.5. INCLUDE: Including another file 5
             1.2. Commands for text output 6
               1.2.1. Outputting a Percent Character 6
               1.2.2. TAB Text-Tabulator. 6
               1.2.3. INT: Outputting Integer Numbers 6
               1.2.4. TEXT Outputting a text variable 7
               1.2.5. HEXA output of a hexa constant 7
               1.2.6. Commands for console dialogues 8
         8
             2.1. Variables 8
             2.2. Rules for Expressions 8
         10
             3.1. Job with INCLUDE and request of the system variable $COPYC: 10
```
## Overview
        
The **HOB Precompiler** program PRECOMP runs under OS/2 or Win32 and offers a variable method of text processing. With the help of PRECOMP you can create variable COBOL-, ASSEMBLER- or other program sources or any other text files.
        
PRECOMP is also used to create the HOB Java programs, because Java has no precompiler build into the javac compiler. To start PRECOMP enter in the OS/2 or Win32 command line:

´PRECOMP prefile targfile parameters´

* **prefile**: is the name of the file that is to be precompiled This parameter is required.
* **targfile**: is the name of the output file. This parameter is required.
* **parameters**: When you run the precompiler, additional parameters can be entered. This parameter is not required. The user cannot use parameters with blanks inbetween, so use : in commands.

### Commands for the precompiler
All commands which are interpreted by PRECOMP start with the control character '%'. This control character has been chosen, because it rarely occurs in text files.

There are two type of commands:
```
$ Commands for the precompiler
$ Commands for text output
```
Command for text output can be staggered in as many levels as you like.

###List of commands
        
The following commands are recognized by PRECOMP:

`%%` Is replaced by a `%`.
`%SET`Setting a precompiler variable
`%DEFT` Setting a precompiler text variable
`%INT` Output of an integer number
`%TEXT` Output of a text variable
`%HEXA`Output of a hexa constant
`%CONT` Continuing the current line in the next line
`%TAB` Tabulator, set to the desired column
`%INCLUDE` Insert of another file
`%CANCEL` Cancelling the text creation
`%VARPUSH` Saving the current list of variables Not implemented.
`%VARPOP` Repeating the last list of variables Not implemented.
`%DISP` Display on console
`%ACC` Reading the console
`%IF` Starts creating text conditionally
`%IFT` Output only, if condition is true
`%IFF` Output only, if condition is false
`%IFTF` Output, if condition is true or false
`%CEND CONDITION END`, End of an IF condition
`%IIF` IMMEDIATE IF, creating text conditionally in thisline
`%MACRO` defines a macro
`%MEND` end of macro definition
`%MEXIT` leave macro
`%MSET` defines a numeric variable inside a macro
`%MDEFT` defines a text variable inside a macro
`%RPT` repeat next part
`%RPTN` repeat next part n times
`%REND` end of part to be repeated
`%REXIT` leave part to be repeated

### Commands for the Precompiler:
#### SET

`%SET <variable>=<expression>;` The `SET` command defines a precompiler variable and assigns a value. The value may be calculated as an expression. With `%SET` a global numeric variable is defined. Examples:

`%SET I=5;` The variable `'I'` gets the value `'5'`. `%SET I1=I*I;`

The variable `I1` get the value of the product of the variables `I` and `I`.

You have to define the variable `I` before.

#### DEFT
         
`%DEFT <Variable>:<Text>` The command `DEFT` defines a precompiler text variable and assigns a text to it. The precompiler interprets the complete text after the colon. The text may have a maximum length of 80 characters. Blancs at the end are not implemented. The text may consist of two ore more other texts. With %DEFT a global text variable is defined. EXAMPLES:

`%DEFT TI:5-ABCD` The text variable `TI` is defined as `5-ABCD`. `%DEFT TI:%INT:I;  %TEXT:TI;` Assumption: `I` was initialized as `12`, `TI` was initialized as `5-ABCD`. The text variable `TI` is initialized as `12 5-ABCD`.

### Creating text conditionally

The following commands control the creation of conditioned texts.

```cobol
%IF
%IFT
%IFF
%IFTF
%CEND
%IIF
```
The `IF` command tests a condition: The following text will only be output, if the condition is true. At the same time a current `IF` condition is opened. The `CEND` (`CONDITION END`) command closes the current IF condition. The text, that follows after the `CEND` command is output in any case. After `IFF (IF FALSE)` the text will only be output, if the last IF condition is false.

After `IFT (IF TRUE)` the text will only be output, if the last `IF` condition is `true`.

After `IFTF` (IF TRUE OR FALSE) the text will always be output (resp. block other `IF`).

`IF` conditions can extend as many levels as you like. To every `IF` a `CEND` has to be assigned.

**IIF (IMMEDIATE IF)** refers only to the current line. The text after the condition is only output, if the condition is true. After `IIF` no `CEND` follows.

The following conditions can be tested:

- `DEF (VAR)`: It is considered, whether a variable is defined. The condition is true, if the variable is defined.
- `NDF (VAR)`: It is considered, whether a variable is defined. The condition is true, if the variable is not defined.
- `EQ (EXPR)`: The specified expression is compared with Zero. The condition is true, if the expression is `0`. The specified expression is compared with Zero. The condition is true, if the expression is not `0`.
- `GT (EXPR)`: The specified expression is compared with Zero. The condition is true, if the expression is greater than `0`.
- `GE (EXPR)`: The specified expression is compared with Zero. The condition is true, if the expression is greater or equal `0`.
- `LT (EXPR)`: The specified expression is compared with Zero. The condition is true, if the expression is less than `0`.
- `LE (EXPR)`: The specified expression is compared with Zero. The condition is true, if the expression is less or equal than `0`.

Examples:

```cobol
%IF EQ I-5;
%DISP:THE VARIABLE I HAS THE VALUE 5
%CEND;
```

The text `'THE VARIABLE I HAS THE VALUE 5'` will be output, if `I` is `5`.

### CANCEL: Canceling the output

`%CANCEL;` With the help of the CANCEL command the output of the current text can be stopped.

### INCLUDE: Including another PC file

`%INCLUDE <Sle entry>;` With the help of the `INCLUDE` command another PC file can be included.

For `INCLUDE` the system variable `$COPYC (Copy-Count = number of copies)` is dealed. In the first entry, the value is Zero, then 1 ...

Example (Host):

`%INCLUDE V.SLXX.TESTTEXT` in the SLE file `SLXX` the entry `TESTTEXT` is searched and inserted.

## Commands for text output
### Outputting a Percent Character

`%%`: The `%` character is defined as control character. Therefore outputting a `%` requires two consecutive `%` characters.

Example:

The text `10 %%` is output as `10 %`.

### TAB Text-Tabulator.

`%TAB(<Variable>);`: The command TAB is effective as text tabulator. Text that follows will be output from the column specified. The free space is filled with blancs.

The tab can only be moved to the right, not to the left. Text, that was already output, can not be overwritten.

Examples:

`%TAB(40);`: The character that follows the; is output in column `40`.
`%TAB(I);`:  The character that follows the; is output in column `I`.

### INT: Outputting Integer Numbers
```cobol
%INT:<variable>;
%INT(<Format>):<Variable>;
```

The INT command can insert an INTEGER number in the text. In case a negative number is output, the minus character is output in front of the remaining digits.

The number can be output with a format. The number in front of the colon then determines, how many digits of the number will be output. If the number is smaller, the Zeroes will be used for the left digits. If the number is larger, the left digits are cut off.

Examples:

```cobol
%SET I=15;
I=%INT:I;
Output: I=15
%SET I=-21;
I=%INT:I;
Output: I=-21
%SET I=15;
I=%INT(4):I;
Output: I=0015
%SET I=-21;
I=%INT(4):I;
Output: I=-021
%SET I=15;
I=%INT(1):I;
Output: I=5
%SET I=-21;
I=%INT(1):I;
Output: I=1
```

### TEXT Outputting a text variable

```cobol
%TEXT:<variable>;
%TEXT(<begin>,<length>):<variable>;
```         

With the help of the `TEXT` command a text variable can be inserted into the current text.

A text variable can be output in a certain format. Then only a part of the text is output.

The format is entered after the `TEXT` command in brackets. The format st consists of a text position (that means 1 `position = one`), comma and length. If no length is entered (end bracket after the comma), the length of the original length of the text is used (resp. shortened).

If a greater length is entered, the rest is filled with blancs (on the right side).

Examples:

`%TEXT:TEXTVAR1;`: The text variable `TEXTVAR1` is inserted in the text.
`%TEXT(5,):TEXTVAR1;`: The text variable `TEXTVAR1` is inserted from the 5  position (shortened by the positioned 1 to 4).
```cobol
%TEXT(5,2):TEXTVAR1;
     th      th
```
The 5 and 6 position of the text variable `TEXTVAR1` are inserted.

### HEXA output of a hexa constant

`%HEXA:<Hexa-Konstante>;` : With the `HEXA` command a hexa constant can be inserted inthe text. The characters of the hexa constant will be entered ashalfbyte with the values 0 to 9 and A to F.

Each two characters of the input will become onecharacter of the output.

If `PRECOMP` runs of S/390, input is **EBCDIC**, if **PRECOMP** runs on **PCs (OS/2 and Win32)**, input is **ASCII / ANSI**.

Examples:

`%HEXA:4142;`: In the text Hexa 4142 is inserted (that means ASCII AB). `%HEXA:00010203;` In the Text Hexa `00010203` is inserted. (This character is not printable).



### Commands for console dialogues
#### The command DISP - Display on console.

`%DISP:<Text>`: Text that follows after the `DISP` command and the colon will be displayed on the console. This text can also be created by the precompiler.

Examples:
`%DISP:ABCDE - THIS IS A CONSOLE OUTPUT`

The text `'ABCDE - THIS IS A CONSOLE OUTPUT '` will be displayed on the system console.

```
%SET I=5;
%DISP:THIS VARIABLE HAS VALUE %INT:I;
```
The text `' THIS VARIABLE HAS VALUE  5 '` will be displayed on the system console.
         
#### The command ACC - Input from the console.

`%ACC: <Variable> <Format>;`  The desired variable will be read from the console. Hereby the format will be considered. After the format character follows the number of characters to be read.

The following formats are possible:

* **N** - numeric
* **A** - alphanumeric (Text)

Examples:

`%ACC:VARN1 N5;`. The console wants to have a five-character numeric alinput. If the variable is to have less than 5 characters, Zeroes have to be filled left. The input must not be greater.

`Input: 00123` - the variable `VARN1` gets the value `123`.
`%ACC:VART1 A20;`: The maximum length of text from the console is 20characters. If the input is greater, an error message appears and the input can be repeated.

`Input: --- TEST ---` The text variable `VART1` contains the text `--- TEST ---`

## Macros

The **HOB precompiler** can process macros, that means `linesof` text that are repeatedly used can be included and parameters can be set in.

Example of a macro:

```cobol
%macro:mac1(mint:mparam1,mtext:mparam2,mquote:mparam3);
...
%mend;
```
`mint` defines a parameter for the macro as an integer.
`mtext` defines a parameter for the macro as a text variable.
`mquote` defines a parameter for the macro as a text variable, that can contain spaces or quotes.
         
This macro can be called by: `%mac1( 1, Text, 'Example of a quoted string' );`

When the macro is called in that way, the numeric `varible mparam1` will contain `1`, the text variable `mparam2` will contain `'Text'` and the text variable `mparam 3` will contain `'Example of a quoted string'`.

These variables can be used normally in the macro body.
`mint` defines a parameter for the macro as an integer.
`mtext` defines a parameter for the macro as a text variable.
`mquote` defines a parameter for the macro as a text variable, that can contain spaces or quotes.

A macro can also be left by the precompiler-command `%MEXIT;`.
In the macro body, the user can user `%MSET` and `%MDEFT` to define variables, that are only valid during the processing of this macro (or nested macros / repeated parts).

After the macro is left, these variables are undefined again or have the value they hade before. `PRECOMP` has a set of variables that are global and has an additional set of variables for each macro that has been called.

## Repeated Parts

The user can repeat parts multiple times by the commands `RPT` and `RPTN`.

In this way, loops can be build.

A repeated part starts with `RPT;` or `RPTN nnn;` (`nnn` is a numeric expression).

The command `%REND` marks the end of a repeated area.

With the command `%REXIT`, a repeated part can be left.
A part started with `RPT` has to be left by `REXIT`.

A part started with `RPTN` nnn is repeated nnn times, if nnn is greater than zero.

If `nnn` is zero, the part to be repeated is left out. If `nnn` is less than zero, there will be an error message.

1.5 Conditional, Macro and Repeated parts / rules which comes first Macros or Repeated parts can contain `IF` statements. These `IF` statements have less priority that `%MACRO` or `%RPT(N)`, so the `IF` statements are processed when the macro or repeated part is called, not when it is defined.

`%MACRO` and `%RPT(N)` have the same priority, that means whichever comes first is handled before till it is left.

Macros and repeated parts can be nested.

# Other Rules
## Variables

Variable names can have any length, but only the first 8characters are used for the recognition of the variable name. Variable name shave to begin with a character (A to Z, no special characters) or the character `$` (system variables). The remaining characters may be digits (0 to9).

The following variables have a special meaning:

```cobol
$COPYC      copy count, see %INCLUDE
$VARPC      not implemented, for future use
$DATE       the current date, format DDMMYY
$INPFILN    file name of the current input file
$INPFCOU    statement number in the current input file
```

## The Length Attribute for Variables

With `L'VAR` there can be the length ot a set variable usedin an expression.

The length of a numeric variable is the number of digitsused plus sign if minus.

The length of a text variable does not contains trailing blanks.

Only if a text variable has been defined by `MQUOTE`,trailing blanks are part of the contents of the variable.

## Rules for Expressions

Expressions (marked with `EXPR`) are calculated accordingto the rules of the Polish Notation (normal algebra). Any number of bracketsis allowed.

Multiplication and division are preferred to addition andsubtraction, else from the left to the right.

It is calculated with complete numbers exclusively, thatmeans all numbers are rounded down. Numbers can have a minus sign before the digits. Two following calculation signs are not allowed.

If after a calculation sign a negative number follows,the negative number has to be written in brackets.
 Examples:
```
I is 5, J is 6
I+2*J
is 5+(2*6) = 17
(I+2)*J
```
is calculated: `(5+2)*6 = 42`
```
14/I
```
is calculated: `14/5 = 2`
```
14/I*J
```
is calculated: `(14/5)*6 = 2*6 = 12`
```
14/(I*J)
```
is calculated: `14/(5*6) = 14/30 = 0`
```
14/(-5)*J
```
is calculated: `(14/(-5))*6 = -2*6 = -12`

# Examples
## Job with INCLUDE and request of the system variable
`$COPYC:` Our first example is a job, which can be calledseparately or as `INCLUDE` inside another job.

The Power cards and the job cards at the beginning and atthe end may only be called, if the job runs separately. The job in theentry `SLJOB1.GOV1242`
was defined as follows:

```COBOL
%IF EQ $COPYC;
* $$ JOB JNM=GOV1242,CLASS=5
* $$ LST DISP=H,CLASS=Z
// JOB  GOV1242
%IFF;
*  JOB GOV1242
%CEND;
// EXEC PROC=GEORAN
// EXEC PROC=SPACEMGR
// DLBL IJSYSUC,'A.UC1',,VSAM
// EXTENT ,SYSWK1
// DLBL CAT2,'A.UC2'
// EXTENT ,SYSWK2
// DLBL CAT3,'A.UC3'
// EXTENT ,SYSWK3
// ASSGN SYS031,DISK,VOL=SYSWK1,SHR
// DLBL VXARST,'V-ARST',,VSAM
// EXTENT SYS031,SYSWK1
// ASSGN SYS032,DISK,VOL=SYSWK3,SHR
// DLBL VXARTP,'V-ARTP',,VSAM,CAT=CAT3
// EXTENT SYS032,SYSWK3
// ASSGN SYS033,DISK,VOL=SYSWK2,SHR
// DLBL VXASET,'V-ASET',,VSAM,CAT=CAT2
// EXTENT SYS033,SYSWK2
// ASSGN SYS034,DISK,VOL=SYSWK2,SHR
// DLBL VXAULK,'V-AULK',,VSAM,CAT=CAT2
// EXTENT SYS034,SYSWK2
// ASSGN SYS035,DISK,VOL=SYSWK2,SHR
// DLBL VXAULI,'V-AULI',,VSAM,CAT=CAT2
// EXTENT SYS035,SYSWK2
// EXEC V1242,SIZE=128K
%IF EQ $COPYC;
/&
* $$ EOJ
%CEND;
```

The job which copies:
```cobol
* $$ JOB JNM=MONAT-W,CLASS=5
* $$ LST DISP=H,CLASS=Z
// JOB  MONAT-W
  .....
%INCLUDE V.SLJOB1.GOV1242
/&
* $$ EOJ
```


In the job `GOV1242` the Power- and the job cards are only output, if the variable `$COPYC` is zero. This is only in case of the job was called separately.

```
30.12.97  KB
14.07.98  KB
```
