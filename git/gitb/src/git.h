#pragma once

// gitb - ncurses git util
// © 2018 Nils Andgren


#include <stdlib.h>
#include <string>
#include <vector>


enum class git_command
{
    k_checkout,
    k_checkout_remote,
    k_delete,
    k_force_delete
};


// Return git branches as a vector of strings. If possible, current_branch
// will be set to the index of the currently checked out branch, otherwise it
// will be set to -1.
std::vector<std::string>
get_git_branches(const git_command & command, int & current_branch);


// Return git command string based on the command and the branch name
std::string
get_command_string(const git_command & command, const std::string & branch);
