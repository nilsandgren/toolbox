# What is it?

vf, short for VimFind, executes find commands and opens the result in vim. vf
is very similar to its buddy vg, it just uses find instead of grep.

## Usage

Put vf in your path, e.g. in ~/bin/, then search for a pattern:

    vf <pattern>

Case insensitive file search for banana:

    vf banana


## Better vim integration

To make browsing of the list of matching files easier, add the following to
your .vimrc to open a match in a new tab with ,o

    nnoremap ,o <C-w>gF

