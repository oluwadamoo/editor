// editor.c

#include "editor.h"

TextDocument *text_document_init()

{

    TextDocument *doc = malloc(sizeof(TextDocument));

    if (!doc)

        return NULL;

    doc->lines = malloc(sizeof(TextLine) * INITIALSIZE);

    if (!doc->lines)

    {

        free(doc);

        return NULL;
    }

    doc->count = 0;

    doc->capacity = INITIALSIZE;

    return doc;
}

/*void text_document_parse(TextDocument *doc, const char *input)

{



    for (size_t i = 0; i < doc->count; i++)

    {

        free(doc->lines[i].text);

   }

   doc->count = 0;



   const char *start = input;

   const char *end = input;



   size_t line_num = 0;



   while (*end != '\0')

   {

       if (*end == '\n')

       {

           size_t line_len = end - start;



           char *line_text = malloc(line_len + 1);

           strncpy(line_text, start, line_len);

           line_text[line_len] = '\0';



           if (doc->count >= doc->capacity)

           {

               doc->capacity *= 2;

               doc->lines = realloc(doc->lines, doc->capacity * sizeof(TextLine));

           }



           doc->lines[doc->count].text = line_text;

           doc->lines[doc->count].length = line_len;

           doc->lines[doc->count].line_num = line_num;



           doc->count++;

           line_num++;



           start = end + 1;

       }



       end++;

   }



   if (start < end)

   {

       size_t line_len = end - start;

       char *line_text = malloc(line_len + 1);

       strncpy(line_text, start, line_len);

       line_text[line_len] = '\0';



       if (doc->count >= doc->capacity)

       {

           doc->capacity *= 2;

           doc->lines = realloc(doc->lines, doc->capacity * sizeof(TextLine));

       }



       doc->lines[doc->count].text = line_text;

       doc->lines[doc->count].length = line_len;

       doc->lines[doc->count].line_num = line_num;

       doc->count++;

   }

}*/

void text_document_free(TextDocument *doc)

{

    if (!doc)

        return;

    for (size_t i = 0; i < doc->count; i++)

    {

        free(doc->lines[i].text);
    }

    free(doc->lines);

    free(doc);
}

int get_document_line_length(const TextDocument *doc, int index)

{

    if (index < 0)

        return 0;

    return strlen(doc->lines[index].text);
}

int get_document_boundary(const TextDocument *doc)

{

    int index = 0;

    for (size_t i = 0; i < doc->count; i++)

    {

        index = doc->lines[i].line_num;
    }

    return index;
}

void text_document_append(TextDocument *doc, const char *input, size_t pos_x, size_t pos_y)

{

    if (!doc || !input)

        return;

    if (doc->count == 0)

    {

        if (doc->capacity == 0)

        {

            doc->capacity = INITIALSIZE;

            doc->lines = malloc(sizeof(TextLine) * doc->capacity);
        }

        doc->lines[0].text = strdup("");

        doc->lines[0].length = 0;

        doc->lines[0].line_num = 0;

        doc->count = 1;

        pos_y = 0;
    }

    if (pos_y >= doc->count)

        pos_y = doc->count - 1;

    TextLine *line = &doc->lines[pos_y];

    size_t line_len = line->length;

    if (pos_x > line_len)

        pos_x = line_len;

    size_t input_len = strlen(input);

    size_t new_len = line_len + input_len;

    char *new_text = malloc(new_len + 1);

    if (!new_text)

        return;

    if (line_len > 0)

    {

        if (pos_x > 0)

        {

            strncpy(new_text, line->text, pos_x);
        }

        strcpy(new_text + pos_x, input);

        if (pos_x < line_len)

        {

            strcpy(new_text + pos_x + input_len, line->text + pos_x);
        }
    }

    else

    {

        strcpy(new_text, input);
    }

    free(line->text);

    line->text = new_text;

    line->length = new_len;

    const char *nl = strchr(input, '\n');

    if (nl)

    {

        size_t nl_pos = pos_x + (nl - input);

        char *first_part = strndup(new_text, nl_pos);

        char *second_part = strdup(new_text + nl_pos + 1);

        if (doc->count >= doc->capacity)

        {

            doc->capacity *= 2;

            doc->lines = realloc(doc->lines, doc->capacity * sizeof(TextLine));
        }

        for (size_t i = doc->count; i > pos_y + 1; i--)

        {

            doc->lines[i] = doc->lines[i - 1];

            doc->lines[i].line_num++;
        }

        free(line->text);

        line->text = first_part;

        line->length = nl_pos;

        doc->lines[pos_y + 1].text = second_part;

        doc->lines[pos_y + 1].length = strlen(second_part);

        doc->lines[pos_y + 1].line_num = pos_y + 1;

        doc->count++;
    }
}

void text_document_backspace(TextDocument *doc, size_t x, size_t y)

{

    if (y >= doc->count)

        return;

    TextLine *line = &doc->lines[y];

    if (x > 0 && x <= line->length)

    {

        memmove(&line->text[x - 1], &line->text[x], line->length - x + 1);

        line->length--;

        line->text = realloc(line->text, line->length + 1);
    }

    else if (x == 0 && y > 0)

    {

        TextLine *prev = &doc->lines[y - 1];

        TextLine *curr = &doc->lines[y];

        size_t prev_len = prev->length;

        size_t curr_len = curr->length;

        prev->text = realloc(prev->text, prev_len + curr_len + 1);

        strcpy(prev->text + prev_len, curr->text);

        prev->length = prev_len + curr_len;

        free(curr->text);

        for (size_t i = y; i < doc->count - 1; i++)

        {

            doc->lines[i] = doc->lines[i + 1];

            doc->lines[i].line_num = i;
        }

        doc->count--;
    }
}

void text_document_draw(TextDocument *doc, int character, size_t *pos_x, size_t *pos_y, int *scroll_offset, int row)

{

    int y = *pos_y;

    int x = *pos_x;

    char temp[2] = {character, '\0'};

    text_document_append(doc, temp, x, y);

    if (character == '\n' || character == KEY_ENTER)

    {

        y++;

        x = 0;

        clear();

        for (size_t i = 0; i < doc->count; i++)

        {

            mvprintw(i, 0, "%s", doc->lines[i].text);
        }

        move(y, x);

        on_arrow_key_press(doc, KEY_DOWN, scroll_offset, row, pos_y, pos_x);
    }

    else

    {

        mvprintw(y, 0, "%s", doc->lines[y].text);

        move(y, x + 1);
    }
}

char *text_document_to_string(const TextDocument *doc)

{

    size_t total_length = 1;

    for (size_t i = 0; i < doc->count; i++)

    {

        total_length += doc->lines[i].length;

        total_length += 1;
    }

    char *result = malloc(total_length);

    if (!result)

        return NULL;

    result[0] = '\0';

    for (size_t i = 0; i < doc->count; i++)

    {

        strcat(result, doc->lines[i].text);

        if (i < doc->count - 1)

            strcat(result, "\n");
    }

    return result;
}

void text_document_read_file(TextDocument *doc, const char *filename)

{

    FILE *input_file;

    char *line = NULL;

    size_t len = 0;

    long int read;

    int line_number = 0;

    input_file = fopen(filename, "r+");

    if (input_file == NULL)

    {

        perror("Error opening file");

        return;
    }

    while ((read = getline(&line, &len, input_file)) != -1)

    {

        if (doc->count >= doc->capacity)

        {

            doc->capacity *= 2;

            doc->lines = realloc(doc->lines, doc->capacity * sizeof(TextLine));
        }

        doc->lines[line_number].text = strdup(line);

        doc->lines[line_number].length = strlen(doc->lines[line_number].text);

        doc->lines[line_number].line_num = line_number;

        doc->count++;

        line_number++;
    }

    free(line);

    fclose(input_file);
}

Mode mode = INSERT;

int QUIT = 0;

char *stringify_mode()

{

    switch (mode)

    {

    case NORMAL:

        return "NORMAL";

        break;

    case INSERT:

        return "INSERT";

        break;

    default:

        return "NORMAL";

        break;
    }
}

void on_arrow_key_press(const TextDocument *doc, int key, int *scroll_offset, int row, size_t *y, size_t *x)

{

    switch (key)

    {

    case KEY_UP:

        if (*y > 0)

        {

            (*y)--;
        }

        else if (*scroll_offset > 0)

        {

            (*scroll_offset)--;
        }

        break;

    case KEY_DOWN:

        if (*y < doc->count - 1)

        {

            (*y)++;

            if (*y - *scroll_offset >= (size_t)row - 1)

            {

                (*scroll_offset)++;
            }
        }

        break;

    case KEY_LEFT:

        if (*x > 0)

            (*x)--;

        break;

    case KEY_RIGHT:

        (*x)++;

        break;

    default:

        break;
    }
}

void prompt_filename(char *buffer, int max_len, int row)

{

    echo();

    curs_set(1);

    mvprintw(row - 1, 0, "Enter filename to save: ");

    clrtoeol();

    move(row - 1, 25);

    getnstr(buffer, max_len - 1);

    noecho();

    curs_set(0);
}

int main(int argc, char *argv[])

{

    char *filename = NULL;

    int row, col;

    int ch;

    row = col = 0;

    TextDocument *doc = text_document_init();

    if (argc > 1)

    {

        filename = argv[1];

        text_document_read_file(doc, filename);
    }

    initscr();

    set_escdelay(25);

    raw();

    keypad(stdscr, TRUE);

    noecho();

    if (has_colors())

    {

        start_color();

        use_default_colors();

        init_pair(1, COLOR_BLACK, COLOR_WHITE);
    }

    getmaxyx(stdscr, row, col);

    size_t x = 0, y = 0;

    int scroll_offset = 0;

    ch = ' ';

    while (ch != ctrl('q') && QUIT != 1)

    {

        clear();

        attron(COLOR_PAIR(1));

        mvprintw(0, 0, "Editor - Press Ctrl+S to save, Ctrl+Q to quit ");

        attroff(COLOR_PAIR(1));

        attron(A_NORMAL);

        for (size_t i = scroll_offset; i < doc->count && i - scroll_offset < (size_t)row - 1; i++)

        {

            mvprintw(i - scroll_offset + 1, 0, "%s", doc->lines[i].text);
        }

        attroff(A_NORMAL);

        move(y - scroll_offset + 1, x);

        refresh();

        ch = getch();

        if (ch == KEY_UP || ch == KEY_DOWN || ch == KEY_LEFT || ch == KEY_RIGHT)

            on_arrow_key_press(doc, ch, &scroll_offset, row, &y, &x);

        else

        {

            if (ch == ctrl('s'))

            {

                if (!filename)

                {

                    char temp_filename[256] = {0};

                    prompt_filename(temp_filename, sizeof(temp_filename), row);

                    if (strlen(temp_filename) == 0)

                        break;

                    filename = strdup(temp_filename);
                }

                FILE *file = fopen(filename, "w");

                if (!file)

                {

                    mvprintw(row - 1, 0, "Error saving file!");

                    clrtoeol();

                    refresh();

                    napms(1500);
                }

                else

                {

                    char *str = text_document_to_string(doc);

                    fwrite(str, strlen(str), 1, file);

                    free(str);

                    fclose(file);

                    QUIT = 1;
                }
            }

            else if (ch == BACKSPACE)

            {

                if (x == 0 && y > 0)

                {

                    int prev_len = get_document_line_length(doc, y - 1);

                    text_document_backspace(doc, x, y);

                    y--;

                    x = prev_len;
                }

                else if (x > 0)

                {

                    text_document_backspace(doc, x, y);

                    x--;
                }
            }

            else if (ch == ESCAPE)

            {

                mode = NORMAL;
            }

            else

            {

                text_document_draw(doc, ch, &x, &y, &scroll_offset, row);

                x++;
            }
        }

        int line_len = get_document_line_length(doc, y);

        if (x > (size_t)line_len)

            x = line_len;
    }

    endwin();

    text_document_free(doc);

    return 0;
}
