enum event_type
{
	event_symbol,
	event_char,
};

enum key_symbol
{
	key_up,
	key_down,
	key_left,
	key_right,
	key_backspace,
};

struct term_event
{
	enum event_type type;
	union
	{
		enum key_symbol symbol;
		char code;
	};
};

static int term_width;
static int term_height;

static struct cell *term_backbuffer;
static struct cell *term_frontbuffer;

int term_init_os();
int term_get_size(int *w, int *h);
int term_close_os();
int term_read_os();

int term_init()
{
	if (term_init_os()) {
		if (term_get_size(&term_width, &term_height)) {
			size_t size = sizeof(struct cell)*term_width*term_height;

			term_backbuffer = (struct cell *)malloc(size);
			memset(term_backbuffer, 0, size);

			term_frontbuffer = (struct cell *)malloc(size);
			memset(term_frontbuffer, 0, size);

			return 1;
		} else {
			fprintf(stderr, "term_get_size failed\n");
		}
	} else {
		fprintf(stderr, "term_init_os failed\n");
	}
	return 0;
}

int term_close()
{
	if (term_close_os()) {
		return 1;
	} else {
		fprintf(stderr, "term_close_os failed\n");
		return 0;
	}
}

int term_get_event(struct term_event *e)
{
	char buf[4];
	int read;
	if (term_read_os(buf, 4, &read)) {
		if (read == 1) {
			if (buf[0] == 127) {
				e->type = event_symbol;
				e->symbol = key_backspace;
			} else {
				e->type = event_char;
				e->code = buf[0];
			}
		} else if (read == 3) {
			if (buf[0] == '\033' && buf[1] == '[') {
				e->type = event_symbol;
				if (buf[2] == 'A') {
					e->symbol = key_up;
				} else if (buf[2] == 'B') {
					e->symbol = key_down;
				} else if (buf[2] == 'C') {
					e->symbol = key_right;
				} else if (buf[2] == 'D') {
					e->symbol = key_left;
				}
			}
		}
		return 1;
	} else {
		fprintf(stderr, "term_read_os failed\n");
		return 0;
	}
}

void term_hide_cursor()
{
	fprintf(stdout, "\033[?25l");
}

void term_show_cursor()
{
	fprintf(stdout, "\033[?25h");
}

void term_move_cursor(int x, int y)
{
	fprintf(stdout, "\033[%d;%dH", y+1, x+1);
}

void term_set_fg(struct color color)
{
	fprintf(stdout, "\033[38;2;%d;%d;%dm", color.r, color.g, color.b);
}

void term_set_bg(struct color color)
{
	fprintf(stdout, "\033[48;2;%d;%d;%dm", color.r, color.g, color.b);
}

void term_reset()
{
	fprintf(stdout, "\033[0m");
}

int term_setcell(int x, int y, struct cell c)
{
	if (x < 0 || y < 0 || x > term_width-1 || y > term_height-1) {
		return 0;
	}
	term_backbuffer[term_width*y + x] = c;
	return 1;
}

void term_render()
{
	int y;
	int x;

	struct cell *frontbuffer = term_frontbuffer;
	struct cell *backbuffer = term_backbuffer;

	for (y = 0; y < term_height; ++y) {
		for (x = 0; x < term_width; ++x) {
			if (!cell_equals(*frontbuffer, *backbuffer)) {
				term_move_cursor(x, y);

				struct cell cell = *backbuffer;

				term_set_fg(cell.fg);
				term_set_bg(cell.bg);
				fprintf(stdout, "%c", cell.c);
			}

			++frontbuffer;
			++backbuffer;
		}
	}
}

int term_get_height()
{
	return term_height;
}

int term_get_width()
{
	return term_width;
}
