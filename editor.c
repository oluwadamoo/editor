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

void text_document_parse(TextDocument *doc, const char *input)
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
}

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

// static void text_document_print(const TextDocument *doc)
// {
//     printf("Parsed Document:\n");
//     for (size_t i = 0; i < doc->count; i++)
//     {
//         printf("Line %zu (%zu chars): \"%s\"\n",
//                doc->lines[i].line_num,
//                doc->lines[i].length,
//                doc->lines[i].text);
//     }
// }

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

// static char *get_document_text_on_index(const TextDocument *doc, int index)
// {
//     if (index < 0)
//         return "";

//     return doc->lines[index].text;
// }

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

void text_document_draw(TextDocument *doc, char character, size_t pos_x, size_t pos_y)
{
    char temp[2] = {character, '\0'};
    text_document_append(doc, temp, pos_x, pos_y);

    mvprintw(pos_y, 0, "%s", doc->lines[pos_y].text);
    move(pos_y, pos_x + 1);
}

Mode mode = NORMAL;

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

// void text_document_draw(const TextDocument *doc)
// {
//     clear();
//     for (size_t i = 0; i < doc->count; i++)
//     {
//         printw(i, 0, "%s", doc->lines[i].text);
//     }
// }
int main()
{
    int row, col;
    int ch;

    row = col = 0;

    TextDocument *doc = text_document_init();

    // char *text = "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Aenean euismod, nisi in aliquam blandit, mauris mi molestie tellus, sit amet feugiat nunc quam pharetra leo. Phasellus odio sem, accumsan et eleifend in, finibus feugiat dui. Donec id dolor non velit sollicitudin egestas. Phasellus quis tortor elit. Ut condimentum posuere diam, ac elementum ipsum malesuada ut. Nunc in diam eget sem scelerisque luctus. Duis accumsan, nisi eu vestibulum pretium, justo lorem pellentesque lectus, et dictum augue augue ornare dui. Maecenas at eros sapien. Duis vehicula ligula ullamcorper pharetra tempus.\n\n"

    //              "Curabitur fermentum,\n"
    //              "tellus eget dapibus elementum, dui ex imperdiet urna, eu pulvinar sapien lectus vitae justo.Morbi non tellus maximus, pretium dui sit amet, interdum mi.Nunc eget diam sit amet quam rutrum vestibulum.Pellentesque suscipit placerat libero, vel mollis sapien gravida id.Etiam ac nibh nisi.Nam eu eros ut lacus fermentum bibendum faucibus eget sapien.Vivamus bibendum pretium erat non ullamcorper.Nunc augue risus, ornare ut metus vel, iaculis congue urna.Cras mauris enim, maximus tincidunt mollis non, gravida vestibulum elit.Suspendisse bibendum ex nec dolor efficitur, vel ornare mauris pharetra.Donec interdum eros quis magna varius tempus.Mauris neque nibh, sagittis nec erat sed, hendrerit volutpat nisi.Nulla sapien arcu, pharetra non eleifend vitae, congue sed nibh.Curabitur placerat eget quam ut dictum.Curabitur in ex in elit faucibus venenatis id id velit.Suspendisse potenti.\n";

    initscr();
    set_escdelay(25);
    raw();
    keypad(stdscr, TRUE);
    noecho();

    // char *buf = malloc(sizeof(char) * 1024);
    // size_t buf_s = 0;

    getmaxyx(stdscr, row, col);
    mvprintw(row - 1, 0, "%s", stringify_mode());

    move(0, 0);

    ch = ' ';

    int x, y = 0;

    while (ch != ctrl('q') && QUIT != 1)
    {
        int boundary = get_document_boundary(doc);
        getyx(stdscr, y, x);

        mvprintw(row - 1, 0, "%s", stringify_mode());

        move(y, x);
        ch = getch();

        if (ch == KEY_UP)
        {
            int new_x = get_document_line_length(doc, y - 1);

            move(y - 1, new_x);
        }
        else if (ch == KEY_DOWN)
        {
            if (y < boundary)
            {
                int new_x = get_document_line_length(doc, y + 1);

                move(y + 1, new_x);
            }
        }
        else if (ch == KEY_LEFT)
        {
            move(y, x - 1);
        }
        else if (ch == KEY_RIGHT)
        {
            move(y, x + 1);
        }
        else
        {

            switch (mode)
            {
            case NORMAL:
                if (ch == 'i')
                {
                    mode = INSERT;
                }
                else if (ch == ctrl('s'))
                {
                    FILE *file = fopen("out.txt", "w");

                    char *str = text_document_to_string(doc);
                    fwrite(str, strlen(str), 1, file);
                    free(str);
                    fclose(file);
                    QUIT = 1;
                }
                break;
            case INSERT:
                if (ch == BACKSPACE)
                {
                    getyx(stdscr, y, x);

                    if (x == 0 && y > 0)
                    {
                        int prev_len = get_document_line_length(doc, y - 1);

                        text_document_backspace(doc, x, y);

                        clear();
                        for (size_t i = 0; i < doc->count; i++)
                        {
                            mvprintw(i, 0, "%s", doc->lines[i].text);
                        }
                        move(y - 1, prev_len);
                    }
                    else if (x > 0)
                    {
                        text_document_backspace(doc, x, y);

                        clear();
                        for (size_t i = 0; i < doc->count; i++)
                        {
                            mvprintw(i, 0, "%s", doc->lines[i].text);
                        }
                        move(y, x - 1);
                    }
                }
                else if (ch == ESCAPE)
                {
                    mode = NORMAL;
                }
                else if (ch == '\n' || ch == KEY_ENTER)
                {
                    char temp[2] = {ch, '\0'};
                    text_document_append(doc, temp, x, y);

                    y++;
                    x = 0;

                    clear();
                    for (size_t i = 0; i < doc->count; i++)
                    {
                        mvprintw(i, 0, "%s", doc->lines[i].text);
                    }
                    move(y, x);
                }
                else
                {
                    text_document_draw(doc, ch, x, y);
                }
                break;
            }
        }
    }

    refresh();
    endwin();

    text_document_free(doc);

    return 0;
}