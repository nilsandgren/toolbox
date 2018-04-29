//  An ncurses tool for switching between git branches
//
//  up/down arrow : move in branch list
//          enter : check out branch
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
//  Relies on "git branch" output having two leading characters
//  before the branch name


#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdint.h>
#include <ncurses.h>
#include <cursesf.h>
#include <form.h>

#include <iostream>
#include <string>
#include <vector>


// -- git and the command line --------------------------------------------

enum class git_command
{
    k_checkout,
    k_delete,
    k_force_delete
};

const char *
to_string(const git_command & command)
{
    switch(command)
    {
        case git_command::k_checkout: return "CHECKOUT";
        case git_command::k_delete: return "DELETE";
        case git_command::k_force_delete: return "FORCE DELETE";
    }
}

// Return git branches in current working directory as a vector of strings
std::vector<std::string>
get_git_branches()
{
    std::vector<std::string> git_branches;
    const char * command = "git branch";
    FILE * fp = popen(command, "r");
    if (fp == NULL)
    {
        std::cerr << "Failed to run :" << command << std::endl;
        return git_branches;
    }

    char line[1024];
    while (fgets(line, sizeof(line)-1, fp) != NULL)
    {
        // trim first two characters and newline
        std::string tmp = line;
        git_branches.push_back(std::string(tmp.begin() + 2, tmp.end() - 1));
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
    if (command == git_command::k_checkout)
        command_string += "checkout ";
    else if (command == git_command::k_delete)
        command_string += "branch -d ";
    else if (command == git_command::k_force_delete)
        command_string += "branch -D ";
    else
        std::cout << "  Unknown git command: " << to_string(command);

    command_string += branch;

    return command_string;
}


// -- ncurses application -------------------------------------------------

class application
{
    public:
        application();
        ~application();

        void add_text_fields(std::vector<std::string> & labels);
        std::string get_user_input(const git_command & command);

    private:
        void free_forms();
        void free_windows();
        static int get_longest_length(std::vector<std::string> & strings);

    private:
        int m_num_fields = 0;
        int m_num_lines = 16;

        int m_width = 0;
        int m_height = 0;

        FIELD ** m_fields = nullptr;
        FORM * m_form = nullptr;
        WINDOW * m_main_window = nullptr;
        WINDOW * m_inner_window = nullptr;
};


// Initialize ncurses
application::application()
{
    initscr();
    cbreak();
    noecho();
    set_escdelay(0);
    keypad(stdscr, TRUE);
    curs_set(0);
}


// Deallocate resources and leave ncurses
application::~application()
{
    free_forms();
    free_windows();
    endwin();
}


// Add text fields to the view
void
application::add_text_fields(std::vector<std::string> & labels)
{
    free_forms();
    free_windows();

    m_num_fields = labels.size();
    m_fields = (FIELD**) calloc(m_num_fields + 1, sizeof(FIELD *));
    if (!m_fields)
    {
        fprintf(stderr, "Could not allocate memory for text fields.\n");
        exit(1);
    }

    int row = 0;
    int col = 1;
    int longest_line = std::max(50, get_longest_length(labels));
    for (int i = 0; i < m_num_fields; i++, row++)
    {
        row = i % m_num_lines;
        FIELD * field = new_field(1, longest_line, row, col, 0, 0);
        set_new_page(field, row == 0);
        field_opts_on(field, O_VISIBLE);
        set_field_buffer(field, 0, labels[i].c_str());

        m_fields[i] = field;
    }

    m_fields[m_num_fields] = NULL;

    m_width = longest_line + 4;
    m_height = m_num_lines + 4;
    m_main_window = newwin(m_height, m_width, 1, 1);

    box(m_main_window, 0, 0);
    keypad(m_main_window, TRUE);

    m_form = new_form(m_fields);

    set_form_win(m_form, m_main_window);
    m_inner_window = derwin(m_main_window, m_num_lines + 2, longest_line + 2, 1, 1);
    set_form_sub(m_form, m_inner_window);
    post_form(m_form);
}


// Let the user select an item among the text fields
std::string
application::get_user_input(const git_command & command)
{
    int field_num = 0;

    // Make a title with the git command and center it
    std::string title = std::string(" Select branch to ") + to_string(command) + " ";
    int offset = (m_width - title.length()) / 2;
    mvwprintw(m_main_window, 0, offset, title.c_str());

    mvwprintw(m_inner_window, field_num, 0, ">");
    wrefresh(m_inner_window);
    while (int user_input = wgetch(m_main_window))
    {
        if (user_input == 'q')
            break;

        if (user_input == '\n')
            return std::string(field_buffer(current_field(m_form), 0));

        mvwprintw(m_inner_window, field_num, 0, " ");
        switch (user_input)
        {
            case KEY_DOWN:
                if (field_num < m_num_fields - 1)
                    field_num++;
                set_current_field(m_form, m_fields[field_num]);
                break;

            case KEY_UP:
                if (field_num >= 1)
                    field_num--;
                set_current_field(m_form, m_fields[field_num]);
                break;
        }
        mvwprintw(m_inner_window, field_num, 0, ">");
        wrefresh(m_inner_window);
    }
    return "";
}


// Deallocate form and fields
void
application::free_forms()
{
    if (m_fields)
    {
        unpost_form(m_form);
        free_form(m_form);
        for (int i = 0; i < m_num_fields; i++)
            free_field(m_fields[i]);
        free(m_fields);
        m_fields = 0;
        m_num_fields = 0;
    }
}


// Deallocate windows
void
application::free_windows()
{
    if (m_main_window)
    {
        delwin(m_main_window);
        m_main_window = nullptr;
    }

    if (m_inner_window)
    {
        delwin(m_inner_window);
        m_inner_window = nullptr;
    }
}


// Get the length of the longest string in a vector of strings
int
application::get_longest_length(std::vector<std::string> & strings)
{
    int longest = 0;
    for (const auto & string : strings)
        if (string.length() > longest)
            longest = string.length();
    return longest;
}


// -- main entry point ----------------------------------------------------

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
    std::cout << " usage: gitb [--help] [ --checkout | --delete | --force-delete]";
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

   -d
   --delete
         Select branch to delete

   -D
   --force-delete
         Select branch to force delete

  up/down arrow : move in branch list
          enter : perform git command
              q : exit
    )x";

    std::cout << help;
}

int
main(int argc, char * argv[])
{
    git_command command = git_command::k_checkout;

    if (argc == 2)
    {
        const char * argument = argv[1];
        if (std::string("-c") == argument || std::string("--checkout") == argument)
        {
            command = git_command::k_checkout;
        }
        else if (std::string("-d") == argument || std::string("--delete") == argument)
        {
            command = git_command::k_delete;
        }
        else if (std::string("-D") == argument || std::string("--force-delete") == argument)
        {
            command = git_command::k_force_delete;
        }
        else
        {
            if (std::string("-h") != argument && std::string("--help") != argument)
            {
                std::cout << std::endl << " unknown option: " << argument << std::endl;
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

    auto branches = get_git_branches();
    if (branches.empty())
        return 1;

    std::string selected_branch;
    // add a scope so ncurses does not eat the git output
    {
        application app;
        app.add_text_fields(branches);
        selected_branch = app.get_user_input(command);
    }

    if (selected_branch.length())
    {
        std::string command_string = get_command_string(command, selected_branch);
        system(command_string.c_str());
    }

    return 0;
}
