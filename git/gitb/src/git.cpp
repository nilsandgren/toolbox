// gitb - ncurses git util
// © 2018 Nils Andgren

// #include <stdio.h>
// #include <stdlib.h>
// #include <assert.h>
// #include <stdint.h>

#include <iostream>
#include <string>
#include <vector>

#include "git.h"


// Return git branches as a vector of strings. If possible, current_branch
// will be set to the index of the currently checked out branch, otherwise it
// will be set to -1.
std::vector<std::string>
get_git_branches(const git_command & command,
                 int & current_branch)
{
    current_branch = -1;
    std::vector<std::string> git_branches;
    std::string command_string = "git branch";
    if (command == git_command::k_checkout_remote)
    {
        command_string += " -r";
    }
    FILE * fp = popen(command_string.c_str(), "r");
    if (fp == NULL)
    {
        std::cerr << "Failed to run :" << command_string << std::endl;
        return git_branches;
    }

    char line[1024];
    size_t index = 0;
    while (fgets(line, sizeof(line)-1, fp) != NULL)
    {
        std::string tmp = line;
        if (tmp[0] == '*')
        {
            current_branch = index;
        }
        // trim first two characters and newline
        git_branches.push_back(std::string(tmp.begin() + 2, tmp.end() - 1));
        index++;
    }
    pclose(fp);
    return git_branches;
}

// Return git command string based on the command and the branch name
std::string
get_command_string(const git_command & command,
                   const std::string & branch)
{
    std::string command_string = "git ";
    if (command == git_command::k_checkout || command == git_command::k_checkout_remote)
        command_string += "checkout ";
    else if (command == git_command::k_delete)
        command_string += "branch -d ";
    else if (command == git_command::k_force_delete)
        command_string += "branch -D ";
    else
        std::cout << "  Unknown git command" << std::endl;

    std::string branch_copy = branch;
    // trim origin/ part of remote branches
    if (command == git_command::k_checkout_remote)
        if (branch_copy.compare(0, 7, "origin/") == 0)
            branch_copy = branch_copy.substr(7);

    command_string += branch_copy;

    return command_string;
}


