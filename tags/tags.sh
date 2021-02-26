#!/bin/bash

###############################################
# tag a set of directories with ctags
###############################################

set -e

tagfile="tags-edgeware"

start_dir="$PWD"
source_root="/home/nilsa/git/"
excludes="--exclude=.git --exclude=build --exclude=objs --exclude=env"

function clean_tagfile
{
    cd "$source_root"
    rm -f $tagfile
    cd "$start_dir"
}

function tag_edgeware_repo
{
    directories=("monorepo/test"
                 "monorepo/products/esb3002"
                 "monorepo/products/esb3003"
                 "monorepo/src/drm-gw"
                 "monorepo/src/esb2001"
                 "monorepo/src/esb3008"
                 "monorepo/src/repackaging"
                 "monorepo/src/sw-liveingest"
                 "monorepo/src/sw-streamer"
                 "monorepo/src/platform/drivers"
                 "monorepo/src/platform/include"
                 "monorepo/src/platform/firmware"
                 "monorepo/src/platform/test"
                 "monorepo/src/components"
                 "monorepo/src/convoy"
                 "monorepo/shared/arcade"
                 "monorepo/src/repackaging/build/app/software-repackager/openresty-prefix"
                 "monorepo/src/repackaging/build/app/software-repackager/3rd-party/nginx"
                 "monorepo/src/repackaging/build/lib/3rd-party/bento4/src/bento4_download"
                 "monorepo/src/repackaging/build/lib/3rd-party/easylogging/easyloggingpp/src/easyloggingpp/src"
                 "monorepo/utils/media-tools"
                 "repackaging-poc/repack_poc"
                 "cloud-api/SDK/sunshine"
                 "ew-aas/tasks"
                 "ew-aas/src"
                 "bento"
                 "orc")

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
        if [ ! -e "$directory" ]; then
            printf 'skipping missing file:\n'
            printf '  "%s"\n' $directory
            continue
        fi

        # print directory name and pad with dots for alignment
        printf "%s" $directory
        num_dots=$(( $max_len - ${#directory} + 2))
        while [ $num_dots -gt 0 ]; do
            echo -n "."
            num_dots=$(( $num_dots - 1 ))
        done

        # lets time the tagging
        start_time=$SECONDS
        ctags -a -R $excludes --links=no --c++-kinds=+p --fields=+iaS --extra=+q -f ${tagfile} ${directory}

        duration=$(( SECONDS - start_time ))
        printf "done (%2d sec)\n" $duration
    done
    cd "$start_dir"
}

# entry point
global_start=$SECONDS

clean_tagfile

# tag edgeware source repository
tag_edgeware_repo

global_duration=$(( SECONDS - global_start ))
printf "\nall the things tagged in %d seconds\n\n" $global_duration
