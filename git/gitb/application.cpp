// gitb - ncurses git util
// © 2018 Nils Andgren


#include "application.h"
#include "git.h"


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
    m_inner_window = derwin(m_main_window,
                            m_num_lines + 1,
                            longest_line + 2,
                            2, 1);
    set_form_sub(m_form, m_inner_window);
    post_form(m_form);
}


// Let the user select an item among the text fields
std::string
application::get_user_input(const git_command & command,
                            int current_branch_index)
{
    int field_num = 0;
    if (current_branch_index >= 0)
        field_num = current_branch_index;

    // Make a title with the git command and center it
    std::string title = " Select branch to ";
    title += to_string(command) + " ";
    int title_start = (m_width - title.length()) / 2;
    mvwprintw(m_main_window, 0, title_start, title.c_str());

    set_current_field(m_form, m_fields[field_num]);

    int user_input = 0;
    do
    {
        if (user_input == 'q')
            break;

        if (user_input == '\n')
            return std::string(field_buffer(current_field(m_form), 0));

        // Clear previous selected field
        set_field_fore(m_fields[field_num], A_NORMAL);
        mvwprintw(m_inner_window, field_num % m_num_lines, 0, " ");

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

            case KEY_NPAGE:
                if (field_num < m_num_fields - m_num_lines)
                    field_num += m_num_lines;
                else
                    field_num = m_num_fields - 1;
                set_current_field(m_form, m_fields[field_num]);
                break;

            case KEY_PPAGE:
                if (field_num >= m_num_lines)
                    field_num -= m_num_lines;
                else
                    field_num = 0;
                set_current_field(m_form, m_fields[field_num]);
                break;
        }

        // Mark current field
        set_field_fore(m_fields[field_num], A_BOLD);
        mvwprintw(m_inner_window, field_num % m_num_lines, 0, ">");

        wrefresh(m_inner_window);
    }
    while (user_input = wgetch(m_main_window));

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


// Return string representation of git command
std::string
application::to_string(const git_command & command)
{
    switch(command)
    {
        case git_command::k_checkout: return "CHECKOUT";
        case git_command::k_checkout_remote: return "CHECKOUT";
        case git_command::k_delete: return "DELETE";
        case git_command::k_force_delete: return "FORCE DELETE";
    }
}
