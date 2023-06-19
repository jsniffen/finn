
int term_init();
int term_get_size(int *w, int *h);
int term_close();

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
	fprintf(stdout, "\033[%d;%dH", x, y);
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

void term_render(struct cell *cells, int length)
{
	int i;
	struct color fg;
	struct color bg;

	for (i = 0; i < length; ++i) {
		struct cell cell = cells[i];

		if (!color_equals(fg, cell.fg)) {
			term_set_fg(cell.fg);
			fg = cell.fg;
		}

		if (!color_equals(bg, cell.bg)) {
			term_set_bg(cell.bg);
			bg = cell.bg;
		}

		fprintf(stdout, "%c", cell.c);
	}
}
