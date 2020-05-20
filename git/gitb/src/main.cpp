//  gitb - An ncurses tool for checking out and deleting git branches
//
//  up/down arrow : move in branch list
//          enter : perform action on branch
//              q : exit
//
//  ┌─────────────────────────────┐
//  │ bug-314-account-cleanup     │
//  │>bug-461-remove-all-entries  │
//  │ master                      │
//  │ update-all-user-credentials │
//  │                             │
//  │                             │
//  └─────────────────────────────┘
//
//  Actions are checkout, delete and force-delete. See gitb -h for details.
//
//  Relies on "git branch" output having two leading characters
//  before the branch name
//
// © 2018 Nils Andgren


#include <stdlib.h>
#include <iostream>
#include <string>

#include "application.h"
#include "git.h"


void display_credits();
void display_usage();
void display_help();
void parse_args(int argc, char * argv[], git_command & command);


int
main(int argc, char * argv[])
{
    // Parse the command line and determine the git command
    git_command command;
    parse_args(argc, argv, command);

    // Get the available git branches
    int current_branch_index = 0;
    auto branches = get_git_branches(command, current_branch_index);
    if (branches.empty())
        return 1;

    std::string selected_branch;
    // Add a scope so ncurses does not eat the git output
    {
        application app;
        app.add_text_labels(branches, current_branch_index);

        while (true)
        {
            app.get_user_input(command);
            if (app.get_state() == application::k_quit)
                return 0;
            if (app.get_state() == application::k_done)
            {
                selected_branch = app.get_selected_label();
                break;
            }
        }
    }

    if (selected_branch == "master")
    {
        if (command == git_command::k_delete ||
            command == git_command::k_force_delete)
        {
            std::cout << "Will not delete master for you" << std::endl;
            return 0;
        }
    }

    if (selected_branch.length())
    {
        // Get the complete command to execute
        std::string command_string = get_command_string(command, selected_branch);
        // Execute command on selected branch
        system(command_string.c_str());
    }

    return 0;
}


void
display_credits()
{
    std::cout << std::endl;
    std::cout << " © 2018 Nils Andgren" << std::endl;
    std::cout << std::endl;
}


void
display_usage()
{
    std::cout << " usage: gitb [--help] [ --checkout | --checkout-remote | --delete | --force-delete | --interactive ]";
    std::cout << std::endl;
}


void
display_help()
{
    std::string help = R"x(
 gitb - ncurses git branch util

 Check out or delete git branches

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
         when navigating the branches. Only for local branches.

   The branch list can be filtered to reduce the number of displayed
   items by pressing slash (/) and entering a search string.

   up/down arrow : previous/next branch
    page up/down : previous/next page of branches
           enter : perform git command
           slash : edit filter text
               q : exit
    )x";

    std::cout << help;
}


void parse_args(int argc, char * argv[], git_command & command)
{
    command = git_command::k_checkout;

    if (argc == 2)
    {
        const char * argument = argv[1];
        if (std::string("-c") == argument ||
            std::string("--checkout") == argument)
        {
            command = git_command::k_checkout;
        }
        else if (std::string("-r") == argument ||
                 std::string("--checkout-remote") == argument)
        {
            command = git_command::k_checkout_remote;
        }
        else if (std::string("-d") == argument ||
                 std::string("--delete") == argument)
        {
            command = git_command::k_delete;
        }
        else if (std::string("-D") == argument ||
                 std::string("--force-delete") == argument)
        {
            command = git_command::k_force_delete;
        }
        else if (std::string("-i") == argument ||
                 std::string("--interactive") == argument)
        {
            command = git_command::k_interactive;
        }
        else
        {
            if (std::string("-h") != argument &&
                std::string("--help") != argument)
            {
                std::cout << std::endl << " unknown option: "
                          << argument << std::endl;
                display_usage();
            }
            else
            {
                display_help();
            }

            display_credits();
            exit(1);
        }
    }
    else if (argc > 2)
    {
        std::cout << std::endl << " too many arguments"<< std::endl;
        display_usage();
        display_credits();
        exit(1);
    }

}
