# gitb

`gitb` is an ncurses tool for checking out local or remote git
branches using the arrow keys on your keyboard. I also allows
you to delete local branches.

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

If the command line argument -i (short for --interactive)
is provided, checkout, delete, and force delete can be
performed on the currently selected branch by pressing
c, d, or D respectively.

The branch list can be filtered to reduce the number of displayed
items by pressing slash (/) and entering a search string.


Description of command line arguments:

    -c
    --checkout
          Select branch to check out
          This is the default action

    -r
    --checkout-remote
          Select remote branch to check out

    -d
    --delete
          Select branch to delete

    -D
    --force-delete
          Select branch to force delete

    -i
    --interactive
          Select (c)checkout, (d)elete, or force (D)elete
          when navigating the branches.
          Only for local branches.

    up/down arrow : previous/next branch
     page up/down : previous/next page of branches
            enter : perform git command
            slash : edit filter text
                q : exit

`gitb` is relying on that the output from "git branch" is
having two leading characters before the branch name.

 © 2018 Nils Andgren

