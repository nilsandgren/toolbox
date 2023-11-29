#!/bin/bash

###############################################
# tag a set of directories with ctags
###############################################

set -e

tagfile="vim-tags"

start_dir="$PWD"
source_root="$HOME/git/"
excludes="--exclude=.git \
          --exclude=build \
          --exclude=install \
          --exclude=objs \
          --exclude=env \
          --exclude=*.js \
          --exclude=NDI*/* \
          --exclude=*.tgz \
          --exclude=*.tar.gz*"

function clean_tagfile
{
    cd "$source_root"
    rm -f $tagfile
    cd "$start_dir"
}

function tag_source_directories
{
    directories=("directory1"
                 "directory2")

    # find out longest path for nice printing
    max_len=0
    for directory in ${directories[*]}
    do
        length=${#directory}
        if [ $length -gt $max_len ]; then
            max_len=$length
        fi
    done


    cd "$source_root"
    for directory in ${directories[*]}
    do
        # print directory name and pad with dots for alignment
        printf "%s" $directory
        num_dots=$(( $max_len - ${#directory} + 2))
        while [ $num_dots -gt 0 ]; do
            echo -n "."
            num_dots=$(( $num_dots - 1 ))
        done

        if [ ! -e "$directory" ]; then
            echo ": Not found"
            continue
        fi

        # lets time the tagging
        start_time=$SECONDS
        ctags -a -R $excludes --links=no --c++-kinds=+p --fields=+iaS --extras=+q -f ${tagfile} ${directory}

        duration=$(( SECONDS - start_time ))
        printf ": Done (%2d sec)\n" $duration
    done
    cd "$start_dir"
}

# entry point
global_start=$SECONDS

clean_tagfile

tag_source_directories $@

global_duration=$(( SECONDS - global_start ))
printf "\nAll the things tagged in %d seconds\n\n" $global_duration
