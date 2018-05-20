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

        void add_text_fields(std::vector<std::string> & labels);
        std::string get_user_input(const git_command & command,
                                   int current_branch);

    private:
        void free_forms();
        void free_windows();
        static int get_longest_length(std::vector<std::string> & strings);
        std::string to_string(const git_command & command);

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
