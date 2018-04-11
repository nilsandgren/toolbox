//  An ncurses tool for switching between git branches
//
//  up/down arrow : move in branch list
//          enter : check out branch
//              q : exit
//  
//  ┌─────────────────────────────┐
//  │ bug-314-account-cleanup     │
//  │ bug-461-remove-all-entries  │
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


typedef uint8_t  U08;
typedef uint16_t U16;
typedef int16_t  S16;
typedef uint32_t U32;
typedef int32_t  S32;


// -- shell command interaction -------------------------------------------

// Get git branches in current working directory as a vector of strings
void
get_git_branches(std::vector<std::string> & git_branches)
{
    FILE *fp;
    char line[1035];

    const char * command = "git branch";
    fp = popen(command, "r");
    if (fp == NULL)
    {
        std::cerr << "Failed to run :" << command << std::endl;
        return;
    }

    while (fgets(line, sizeof(line)-1, fp) != NULL)
    {
        // trim first two characters and newline
        std::string tmp = line;
        git_branches.push_back(std::string(tmp.begin() + 2, tmp.end() - 1));
    }
    pclose(fp);
    return;
}


// -- ncurses application -------------------------------------------------

class application
{
    public:
        application();
        ~application();

        void add_text_fields(std::vector<std::string> & labels);
        std::string get_user_input();

    private:
        void free_forms();
        void free_windows();
        int  get_longest_length(std::vector<std::string> & strings);

    private:
        int num_fields;
        int num_lines;

        FIELD ** fields;
        FORM * form;
        WINDOW * main_window;
        WINDOW * inner_window;
};


// Initialize ncurses
application::application()
    : num_fields(0)
    , num_lines(16)
    , form(nullptr)
    , fields(nullptr)
    , main_window(nullptr)
    , inner_window(nullptr)
{
    initscr();
    cbreak();
    noecho();
    set_escdelay(0);
    keypad(stdscr, TRUE);
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

    num_fields = labels.size();
    fields = (FIELD**) calloc(num_fields + 1, sizeof(FIELD *));
    if (!fields)
    {
        fprintf(stderr, "Could not allocate memory for text fields.\n");
        exit(1);
    }

    int row = 0;
    int col = 1;
    int longest_line = get_longest_length(labels);
    for (int i = 0; i < num_fields; i++, row++)
    {
        row = i % num_lines;
        FIELD * field = new_field(1, longest_line, row, col, 0, 0);
        set_new_page(field, row == 0);

        field_opts_on(field, O_VISIBLE);
        field_opts_off(field, O_EDIT);
        field_opts_on(field, O_ACTIVE);
        set_field_buffer(field, 0, labels[i].c_str());

        fields[i] = field;
    }

    fields[num_fields] = NULL;

    main_window = newwin(num_lines + 4, longest_line + 4, 1, 1);
    box(main_window, 0, 0);
    keypad(main_window, TRUE);

    form = new_form(fields);

    set_form_win(form, main_window);
    inner_window = derwin(main_window, num_lines + 2, longest_line + 2, 1, 1);
    set_form_sub(form, inner_window);
    post_form(form);
}


// Let the user select an item among the text fields
std::string
application::get_user_input()
{
    int field_num = 0;
    while (int user_input = wgetch(main_window))
    {
        if (user_input == 'q')
        {
            break;
        }

        if (user_input == '\n')
        {
            return std::string(field_buffer(current_field(form), 0));
        }

        switch (user_input)
        {
            case KEY_DOWN:
                if (field_num < num_fields - 1)
                {
                    field_num++;
                }
                set_current_field(form, fields[field_num]);
                break;

            case KEY_UP:
                if (field_num >= 1)
                {
                    field_num--;
                }
                set_current_field(form, fields[field_num]);
                break;
        }
    }
    return "";
}


// Deallocate form and fields
void
application::free_forms()
{
    if (fields)
    {
        unpost_form(form);
        free_form(form);
        for (int i = 0; i < num_fields; i++)
        {
            free_field(fields[i]);
        }
        free(fields);
        fields = 0;
        num_fields = 0;
    }
}


// Deallocate windows
void
application::free_windows()
{
    if (main_window)
    {
        delwin(main_window);
    }

    if (inner_window)
    {
        delwin(inner_window);
    }
}


// Get the length of the longest string in a vector of strings
int
application::get_longest_length(std::vector<std::string> & strings)
{
    int longest = 0;
    for (const auto & string : strings)
    {
        if (string.length() > longest)
        {
            longest = string.length();
        }
    }
    return longest;
}


// -- main entry point ----------------------------------------------------

int
main(int argc, char * argv[])
{
    std::vector<std::string> branches;
    get_git_branches(branches);
    if (branches.empty())
    {
        return 1;
    }

    std::string selected_branch;
    // add a scope so ncurses does not eat the git output
    {
        application app;
        app.add_text_fields(branches);
        selected_branch = app.get_user_input();
    }

    std::string command = "git checkout " + selected_branch;
    system(command.c_str());

    return 0;
}
