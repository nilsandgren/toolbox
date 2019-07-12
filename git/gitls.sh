#!/bin/bash

# List paths with some git metadata.

# Source this file to add the gitls function to your shell.
# user@host:toolbox/git$ . gitls.sh

# user@host:toolbox/git$ gitls *
# gitb/    Nils Andgren  90c7c1b  5 days ago            [gitb] Adding interactive mode
# gitc.sh  Nils Andgren  464ae11  1 year, 5 months ago  Fixed spelling error.
# gupdate  Nils Andgren  821a15f  1 year, 2 months ago  [gupdate] Using --force-with-lease


_print_path() {
    # Print a path string with some formatting
    # $1 the path, $2 file to print to
    color_file="\e[97m"
    color_dir="\e[94m"
    reset="\e[0m"

    if [[ -d $1 ]]; then
        suffix="/"
        color=${color_dir}
    else
        suffix=""
        color=${color_file}
    fi
    echo -e -n "${color}${1}${suffix}${reset}" >> $2
}


_gitls() {
    TRACKED_FILES="$(mktemp)"
    UNTRACKED_FILES="$(mktemp)"
    no_untracked_files=1

    # Loop the provided paths
    while (( "$#" )); do
        path=$1

        git_format="%an|%C(dim)%h%C(reset)|%cr|%Cgreen%<(50,trunc)%s%Creset"
        git_output=`git log -1 --pretty=format:"${git_format}" "${path}"`

        # Skip files not under version control
        if [[ ${git_output} = "" ]]; then
            if [ $no_untracked_files -eq 1 ]; then
                no_untracked_files=0
                echo "" >> $UNTRACKED_FILES
                echo "Untracked files:" >> $UNTRACKED_FILES
            fi

            echo -n "  " >> $UNTRACKED_FILES
            _print_path "${path}" $UNTRACKED_FILES
            echo >> $UNTRACKED_FILES
            shift
            continue
        fi

        _print_path "${path}" $TRACKED_FILES
        echo -n "|" >> $TRACKED_FILES

        echo $git_output >> $TRACKED_FILES

        shift
    done

    cat $TRACKED_FILES | column -ts'|'
    rm "$TRACKED_FILES"
    cat $UNTRACKED_FILES
    rm "$UNTRACKED_FILES"
}

gitls() {
    if [ $# -eq 0 ]; then
        _gitls *
    else
        _gitls $@
    fi
}
