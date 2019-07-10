#!/bin/bash

# List paths with some git metadata.

# Source this file to add the gitls function to your shell.
# user@host:toolbox/git$ . gitls.sh

# user@host:toolbox/git$ gitls *
# gitb/    Nils Andgren  90c7c1b  5 days ago            [gitb] Adding interactive mode
# gitc.sh  Nils Andgren  464ae11  1 year, 5 months ago  Fixed spelling error.
# gupdate  Nils Andgren  821a15f  1 year, 2 months ago  [gupdate] Using --force-with-lease

_gitls() {
    TMPFILE="$(mktemp)"

	echo "File|Author|Revision|Date|Subject" > $TMPFILE

    # Loop the provided pats
    while (( "$#" )); do
        path=$1

        git_format="%an|%h|%cr|%Cgreen%<(50,trunc)%s%Creset"
        git_output=`git log -1 --pretty=format:"${git_format}" "${path}"`

        # Skip files not under version control
        if [[ ${git_output} = "" ]]; then
            shift
            continue
        fi

		# Append / to directories
        suffix=""
        if [[ -d $path ]]; then
            suffix="/"
        fi

        echo -e -n "${path}${suffix}|" >> $TMPFILE
        echo $git_output >> $TMPFILE

        shift
    done

    cat $TMPFILE | column -ts'|'
    rm "$TMPFILE"
}

gitls() {
    if [ $# -eq 0 ]; then
        _gitls *
    else
        _gitls $@
    fi
}
