struct cell
{
	struct color fg;
	struct color bg;
	char c;
};

int cell_equals(struct cell c1, struct cell c2)
{
	return color_equals(c1.fg, c2.fg) && color_equals(c1.bg, c2.bg) && c1.c == c2.c;
}

