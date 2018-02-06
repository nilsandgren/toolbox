#!/bin/bash

# Do git checkout by branch number on the command line.
# Source this file to add the gitc function to your shell.
#
#     user@host~$ gitc
#
#       1: ticket-141-improve-logging
#       2: performance-tests
#       3: web-server-security-fix
#       4: chocolate-test
#
#       Enter a branch number: 4
#
#     Switched to branch 'chocolate-test'

gitc() {
    # Get local branches as an array
    # Remove white space and that '*'
    BRANCHES=(`git branch | tr -d "\*" | tr -d " "`)
    INDEX=1

    # Display list of branches
    echo
    for BRANCH in "${BRANCHES[@]}"; do
        printf "%3d: %s\n" "$INDEX" "$BRANCH"
        INDEX=$((INDEX + 1))
    done
    echo

    printf "  Enter a branch number: "
    read USER_SELECTION

    # Sanity check
    NUMBER_REGEX='^[0-9]+$'
    MIN_INDEX=1
    MAX_INDEX=${#BRANCHES[@]}
    if ! [[ $USER_SELECTION =~ $NUMBER_REGEX ]] ||
         [[ $USER_SELECTION -lt $MIN_INDEX ]] ||
         [[ $USER_SELECTION -gt $MAX_INDEX ]] ; then
       echo "  Invalid index" >&2
       return
    fi

    # Checkout the branch
    echo
    git checkout "${BRANCHES[$((USER_SELECTION - 1))]}"
    echo
}
