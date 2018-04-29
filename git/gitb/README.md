# gitb

`gitb` is an ncurses tool for switching between, or deleting,
your local git branches using the arrow keys on your keyboard.

Example output:

    ┌──────────── Select branch to CHECKOUT ─────────────┐
    │ bug-314-account-cleanup                            │
    │>bug-461-remove-all-entries                         │
    │ master                                             │
    │ update-all-user-credentials                        │
    │                                                    │
    │                                                    │
    └────────────────────────────────────────────────────┘


If no command line arguments are given, the default action
is to do a checkout of the selected branch name. 
Use the up and down arrows to move in the list of branches.
Press enter to check out the currently selected branch, or
press q to abort.

Description of command line arguments:

    -c
    --checkout
          Select branch to check out
          This is the default action

    -d
    --delete
          Select branch to delete

    -D
    --force-delete
          Select branch to force delete


`gitb` is relying on that the output from "git branch" is
having two leading characters before the branch name.

 © 2018 Nils Andgren

