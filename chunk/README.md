# chunk
Read a chunk of data from an input file and print it to stdout.

## Usage

    chunk size@offset filename

## Example
Read one KB of data at position 123456 from file.dat and store in chunk.dat.

    chunk 1024@123456 file.dat > chunk.dat

