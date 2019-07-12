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
    TRACKED_DIRS="$(mktemp)"
    UNTRACKED_FILES="$(mktemp)"
    UNTRACKED_DIRS="$(mktemp)"
    no_untracked_files=1

    # Loop the provided paths
    while (( "$#" )); do
        path=$1

        git_format="%an|%C(dim)%h%C(reset)|%cr|%Cgreen%<(50,trunc)%s%Creset"
        git_output=`git log -1 --pretty=format:"${git_format}" "${path}"`

        # Files not under version control
        if [[ ${git_output} = "" ]]; then
            no_untracked_files=0
            if [[ -d $path ]]; then
                output=$UNTRACKED_DIRS
            else
                output=$UNTRACKED_FILES
            fi

            echo -n "  " >> $output
            _print_path "${path}" $output
            echo >> $output
            shift
            continue
        fi

        if [[ -d $path ]]; then
            output=$TRACKED_DIRS
        else
            output=$TRACKED_FILES
        fi

        _print_path "${path}" $output
        echo -n "|" >> $output

        echo $git_output >> $output

        shift
    done

    cat $TRACKED_DIRS $TRACKED_FILES | column -ts'|'
    rm "$TRACKED_FILES"
    rm "$TRACKED_DIRS"
    if [ $no_untracked_files -eq 0 ]; then
        echo ""
        echo "Untracked files:"
        cat $UNTRACKED_DIRS
        cat $UNTRACKED_FILES
    fi
    rm "$UNTRACKED_DIRS"
    rm "$UNTRACKED_FILES"
}

gitls() {
    if [ $# -eq 0 ]; then
        _gitls *
    else
        _gitls $@
    fi
}
