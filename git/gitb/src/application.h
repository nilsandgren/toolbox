#pragma once

// gitb - ncurses git util
// © 2018 Nils Andgren

#include <ncurses.h>
#include <form.h>

#include <string>
#include <vector>

#include "git.h"

class application
{
    public:
        application();
        ~application();

        // Current state of the application
        enum state
        {
            k_navigating,  // moving about
            k_filtering,   // edit filter string
            k_done,        // branch selected
            k_quit         // early exit
        };
        state get_state() const;

        // Add text fields for the user to select from
        void add_text_fields(std::vector<std::string> & labels);

        // Let the user select a text field and return the text.
        // An empty string is returned if the user quits the form.
        // If the command is git_command::k_interactive, the command can be
        // changed by the user while interacting with the form.
        std::string get_user_input(git_command & command,
                                   int& current_branch);

    private:
        void free_forms();
        void free_windows();
        static int get_longest_length(std::vector<std::string> & strings);
        std::string to_string(const git_command & command);
        void change_interactive_command(git_command& command, int& user_input);
        void change_state(int user_input);
        void navigate(int user_input, int& field_num);
        void update_filter(int user_input);
        void filter_labels(std::vector<std::string>& dst,
                           std::vector<std::string>& src);
        void select_field(int field_num);
        void deselect_field(int field_num);
        void display_header(const git_command& command);
        void display_footer();

    private:
        int m_num_fields = 0;
        int m_num_lines = 16;

        int m_width = 0;
        int m_height = 0;

        state m_state = k_navigating;
        std::string m_filter = "";

        FIELD ** m_fields = nullptr;
        FORM * m_form = nullptr;
        WINDOW * m_main_window = nullptr;
        WINDOW * m_inner_window = nullptr;
};
