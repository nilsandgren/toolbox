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

application::state
application::get_state() const
{
    return m_state;
}

// Copy strings matching current filter
void
application::filter_labels(std::vector<std::string>& dst,
                           std::vector<std::string>& src)
{
    for (const auto& item : src)
    {
        if (item.find(m_filter) != std::string::npos)
        {
            dst.push_back(item);
        }
    }
}

// Add text fields to the view
void
application::add_text_labels(std::vector<std::string> & labels,
                             int selected_index)
{
    m_labels = labels;
    m_field_index = selected_index;
}

void
application::create_main_window()
{
    free_forms();
    free_windows();

    // only include labels matching m_filter
    std::vector<std::string> filtered;
    filter_labels(filtered, m_labels);

    m_num_fields = filtered.size();
    m_fields = (FIELD**) calloc(m_num_fields + 1, sizeof(FIELD *));
    if (!m_fields)
    {
        fprintf(stderr, "Could not allocate memory for text fields.\n");
        exit(1);
    }

    int row = 0;
    int col = 1;
    int longest_line = std::max(50, get_longest_length(m_labels));
    for (int i = 0; i < m_num_fields; i++, row++)
    {
        row = i % m_num_lines;
        FIELD * field = new_field(1, longest_line, row, col, 0, 0);
        set_new_page(field, row == 0);
        field_opts_on(field, O_VISIBLE);
        set_field_buffer(field, 0, filtered[i].c_str());

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

// If command is k_interactive, see if user pressed c, d or D
// (checkout, delete, force delete). Change the command
// accordingly and act as if user pressed enter.
void
application::change_interactive_command(git_command& command,
                                        int& user_input)
{
    switch (user_input)
    {
        case '\n':
        case 'c':
        {
            command = git_command::k_checkout;
            user_input = '\n';
            break;
        }
        case 'd':
        {
            command = git_command::k_delete;
            user_input = '\n';
            break;
        }
        case 'D':
        {
            command = git_command::k_force_delete;
            user_input = '\n';
            break;
        }
        default: break;
    }
}

// Change application state based on user input
void
application::change_state(int user_input)
{
    if (m_state == k_filtering)
    {
        // Break out of filter editing state
        if (user_input == '\n' ||
            user_input == KEY_UP ||
            user_input == KEY_DOWN ||
            (user_input == KEY_BACKSPACE &&
             m_filter.length() == 0))
        m_state = k_navigating;
    }
    else if (user_input == '/' ||
             (user_input == KEY_BACKSPACE &&
              m_filter.length() != 0))
    {
        // Go into filter editing state
        m_state = k_filtering;
    }
    else if (user_input == 'q')
    {
        m_state = k_quit;
    }
}

void
application::navigate(int user_input, int& field_num)
{
    if (!m_num_fields)
        return;

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
}

void
application::update_filter(int user_input)
{
    if (user_input <= 0xff && isprint(int(user_input)))
    {
        m_filter.push_back(char(user_input));
    }
    else if (user_input == KEY_BACKSPACE && m_filter.length())
    {
        m_filter.pop_back();
    }
}

void
application::select_field(int field_num)
{
    if (field_num >= m_num_fields)
        return;
    set_field_fore(m_fields[field_num], A_BOLD);
    mvwprintw(m_inner_window, field_num % m_num_lines, 0, ">");
}

void
application::deselect_field(int field_num)
{
    if (field_num >= m_num_fields)
        return;
    set_field_fore(m_fields[field_num], A_NORMAL);
    mvwprintw(m_inner_window, field_num % m_num_lines, 0, " ");
}

void
application::display_header(const git_command& command)
{
    // Make a title with the git command and center it
    std::string title = " Select branch to ";
    title += to_string(command) + " ";
    int title_start = (m_width - title.length()) / 2;
    mvwprintw(m_main_window, 0, title_start, title.c_str());

}

void
application::display_footer()
{
    if (m_state != k_filtering && m_filter.length() == 0)
        return;

    // Display filter text in lower left corner
    int y = m_height - 1;
    mvwprintw(m_main_window, y, 3, "/");
    mvwprintw(m_main_window, y, 4, m_filter.c_str());
    mvwprintw(m_main_window, y, 4 + m_filter.length(), " ");
}

void
application::get_user_input(git_command & command)
{
    // Very lazy. Recreate ncurses window from current state since filtering
    // changes the list of labels displayed. Ideally only the list of FIELDs
    // should be modified.
    create_main_window();

    int field_num = 0;
    if (m_field_index >= 0)
        field_num = m_field_index;

    display_header(command);
    display_footer();

    if (m_num_fields > field_num)
        set_current_field(m_form, m_fields[field_num]);

    int user_input = 0;
    do
    {
        state prev_state = m_state;
        change_state(user_input);

        if (m_state != prev_state)
        {
            m_field_index = 0;
            break;
        }

        if (m_state == k_navigating)
        {
            if (command == git_command::k_interactive)
                change_interactive_command(command, user_input);

            if (user_input == '\n')
            {
                m_state = k_done;
                std::string tmp(field_buffer(current_field(m_form), 0));
                m_selected_label = tmp.substr(0, tmp.find(" "));
                return;
            }
        }

        // Clear previous selected field
        deselect_field(field_num);

        if (m_state == k_navigating)
        {
            navigate(user_input, field_num);
        }
        else if (m_state == k_filtering)
        {
            std::string prev_filter = m_filter;
            update_filter(user_input);
            if (m_filter != prev_filter)
            {
                m_field_index = 0;
                break;
            }
        }

        // Mark current field
        select_field(field_num);

        wrefresh(m_inner_window);
    }
    while (user_input = wgetch(m_main_window));

    m_selected_label = "";
}

std::string
application::get_selected_label() const
{
    return m_selected_label;
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
        case git_command::k_interactive: return "INTERACT WITH (c|d|D)";
    }
}
