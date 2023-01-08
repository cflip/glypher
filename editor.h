#ifndef _EDITOR_H
#define _EDITOR_H

#include <time.h>
#include "buffer.h"

struct editor_state {
	int cursor_x, cursor_y;
	int cursor_display_x;
	int row_offset;
	int col_offset;
	int screen_rows;
	int screen_cols;
	int row_count;
	struct editor_row* rows;
	int dirty;
	char* filename;
	char status_message[80];
	time_t status_message_time;
	struct editor_syntax* syntax;
};

void init_editor(struct editor_state* editor);

void editor_set_status_message(struct editor_state* editor, const char* format, ...);
void editor_refresh_screen(struct editor_state* editor);
char* editor_prompt(struct editor_state* editor, char* prompt, void (*callback)(struct editor_state*, char*, int));

void editor_insert_char(struct editor_state* editor, int c);
void editor_insert_newline(struct editor_state* editor);
void editor_delete_char(struct editor_state* editor);
void editor_find(struct editor_state* editor);
void editor_scroll(struct editor_state* editor);
void editor_draw_rows(struct editor_state* editor, struct append_buffer* buffer);
void editor_draw_status_bar(struct editor_state* editor, struct append_buffer* buffer);
void editor_draw_message_bar(struct editor_state* editor, struct append_buffer* buffer);

#endif
