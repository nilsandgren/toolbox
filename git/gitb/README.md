# gitb

`gitb` is an ncurses tool for switching between your local
git branches using the arrow keys on your keyboard.

Example output:

    ┌─────────────────────────────┐
    │ bug-314-account-cleanup     │
    │>bug-461-remove-all-entries  │
    │ master                      │
    │ update-all-user-credentials │
    │                             │
    │                             │
    └─────────────────────────────┘


Use the up and down arrows to move in the list of branches.
Press enter to check out the currently selected branch, or
press q to abort.

`gitb` is relying on that the output from "git branch" is
having two leading characters before the branch name.
