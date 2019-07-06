# numbers
A linux command line tool for printing numbers in various formats

## Usage
When run without arguments, `numbers` will print the following help:

    user@host$ numbers
      Print numbers in various formats

      Usage
             numbers <prefix><number>

      Number prefix sets input format:

        b: binary
        o: octal
        d: decimal
        h: hexadecimal
       0x: hexadecimal
        r: Roman numerals

      Examples
             Read binary input:
             numbers b1001110

             Read hexadecimal input:
             numbers h09fa37
             numbers 0x20

             Read Roman numeral input:
             numbers rMLXVI

             Without prefix, decimal format is assumed:
             numbers 157


When given a number, `numbers` displays the data in the following forms:

  * Binary
  * Octal
  * Decimal
  * Hexadecimal
  * Time (GMT and local)
  * ASCII character (if printable)
  * Roman numerals (if value <= 10000)


An example of reading a binary string:

    user@host$ numbers b101110

      input: bin

      bin: 101110
      oct: 56
      dec: 46
      hex: 2e
      rom: XLVI
      asc: '.'

      gmt time: 1970-01-01 00:00:46
      loc time: 1970-01-01 01:00:46

An example of reading decimal data:

    user@host$ numbers 1506001993

      input: dec

      bin: 1011001110000111100010001001001
      oct: 13160742111
      dec: 1506001993
      hex: 59c3c449

      gmt time: 2017-09-21 13:53:13
      loc time: 2017-09-21 15:53:13
