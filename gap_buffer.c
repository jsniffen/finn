struct gap_buffer
{
	char *buffer;
	int cursor;
	int end;
	int size;
};

int gb_new(struct gap_buffer *gb, int size)
{
	gb->buffer = (char *)malloc(size);
	if (gb->buffer) {
		gb->cursor = 0;
		gb->end = size;
		gb->size = size;
		return 1;
	} else {
		fprintf(stderr, "gb_new failed\n");
		return 0;
	}
}

void gb_free(struct gap_buffer *gb)
{
	free(gb->buffer);
	gb->buffer = 0;
	gb->cursor = 0;
	gb->end = 0;
	gb->size = 0;
}

void gb_insert(struct gap_buffer *gb, char c)
{
	if (gb->cursor < gb->end) {
		gb->buffer[(gb->cursor)++] = c;
	}
}

void gb_delete(struct gap_buffer *gb)
{
	if (gb->cursor) {
		--(gb->cursor);
	}
}

void gb_render(struct gap_buffer *gb)
{
	int y = 0;
	int x = 0;

	struct cell cell = {
		.bg = {
			.r = 0,
			.g = 0,
			.b = 0,
		},
		.fg = {
			.r = 255,
			.g = 255,
			.b = 255,
		},
		.c = ' ',
	};

	for (int i = 0; i < gb->size; ++i) {
		if (i == gb->cursor) {
			i = gb->end;
		} else {
			cell.c = gb->buffer[i];

			if (cell.c == '\n') {
				y += 1;
				x = 0;
			} else if (cell.c == ' ') {
				x += 1;
			} else {
				term_setcell(x, y, cell);
				x += 1;
			}
		}
	}
}
