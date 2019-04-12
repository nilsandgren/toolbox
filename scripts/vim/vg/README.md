# What is it?

vg is short for VimGrep, a little script that greps for stuff and opens the
matches in vim. When inside a git repository, "git grep" is used.


## Usage

Put vg in your path, e.g. in ~/bin/, then search for a pattern:

    vg <pattern>
    vg <grep options>

If the search pattern is found the list of matches is opened in vim. Use gF to
jump to the file:line match under the cursor. Use :q to get back to the list
of matching lines.


### Examples:

Recursive grep for the word 'banana' in all files and folders:

    vg banana

Grep for the exact word 'banana', and exclude binary files:

    vg -rnwI banana *


## Better vim integration

To make browsing of the list of matching files easier, add the following to
your .vimrc to open a match in a new tab with ,o

    nnoremap ,o <C-w>gF

