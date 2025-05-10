// editor.h

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>

#define ctrl(x) ((x) & 0x1f)
#define BACKSPACE 263
#define ESCAPE 27

#define INITIALSIZE 20

typedef enum
{
    NORMAL,
    INSERT
} Mode;

struct text_line
{
    char *text;
    size_t length;
    size_t line_num;
};

typedef struct text_line TextLine;

struct text_document
{
    TextLine *lines;
    size_t count;
    size_t capacity;
};
typedef struct text_document TextDocument;

struct position
{
    int x;
    int y;
};
typedef struct position Position;
TextDocument *text_document_init();
// void text_document_parse(TextDocument *doc, const char *input);

void text_document_free(TextDocument *doc);
int get_document_line_length(const TextDocument *doc, int index);
int get_document_boundary(const TextDocument *doc);
void text_document_append(TextDocument *doc, const char *input, size_t pos_x, size_t pos_y);
void text_document_backspace(TextDocument *doc, size_t x, size_t y);
void text_document_draw(TextDocument *doc, int character, size_t *pos_x, size_t *pos_y, int *scroll_offset, int row);
char *text_document_to_string(const TextDocument *doc);
void on_arrow_key_press(const TextDocument *doc, int key, int *scroll_offset, int row, size_t *y, size_t *x);