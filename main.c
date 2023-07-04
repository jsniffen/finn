#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <signal.h>
#include <termios.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>

struct color
{
	unsigned char r;
	unsigned char g;
	unsigned char b;
};

int color_equals(struct color c1, struct color c2)
{
	return c1.r == c2.r && c1.g == c2.g && c1.b == c2.b;
}

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

struct termios orig_tios;
struct termios tios;

int term_init()
{
	int fd = open("/dev/tty", O_RDWR);
	if (fd != -1) {
		if (tcgetattr(fd, &orig_tios) == 0) {
			memcpy(&tios, &orig_tios, sizeof(struct termios));
			tios.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IXON);
			tios.c_oflag &= ~OPOST;
			tios.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
			tios.c_cflag &= ~(CSIZE | PARENB);
			tios.c_cflag |= CS8;
			tios.c_cc[VMIN] = 1;
			tios.c_cc[VTIME] = 0;

			if (tcsetattr(fd, TCSAFLUSH, &tios) == 0) {
				return 1;
			} else {
				fprintf(stderr, "tcsetattr failed");
			}
		} else {
			fprintf(stderr, "tcgetattr failed\n");
		}
	} else {
		fprintf(stderr, "failed to open dev/tty\n");
	}
	return 0;
}

void term_hide_cursor() {
	fprintf(stdout, "\033[?25l");
}

void term_show_cursor() {
	fprintf(stdout, "\033[?25h");
}

void term_move_cursor(int x, int y) {
	fprintf(stdout, "\033[%d;%dH", x, y);
}

void term_set_fg(struct color color) {
	fprintf(stdout, "\033[38;2;%d;%d;%dm", color.r, color.g, color.b);
}

void term_set_bg(struct color color) {
	fprintf(stdout, "\033[48;2;%d;%d;%dm", color.r, color.g, color.b);
}

int term_get_size(int *w, int *h)
{
	int fd = open("/dev/tty", O_RDWR);
	if (fd != -1) {
		struct winsize ws;
		if (ioctl(fd, TIOCGWINSZ, &ws) == 0) {
			*w = ws.ws_col;
			*h = ws.ws_row;
			return 1;
		} else {
			fprintf(stderr, "failed to get terminal size\n");
		}
	} else {
		fprintf(stderr, "failed to open /dev/tty\n");
	}
	return 0;
}

void term_reset() {
	fprintf(stdout, "\033[0m");
}

int term_close()
{
	int fd = open("/dev/tty", O_RDWR);
	if (fd != -1) {
		if (tcsetattr(fd, TCSAFLUSH, &orig_tios) == 0) {
			return 1;
		} else {
			fprintf(stderr, "tcsetattr failed");
		}
	} else {
		fprintf(stderr, "failed to open dev/tty\n");
	}
	return 0;
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

int main()
{
	if (term_init()) {
		term_hide_cursor();

		int w, h;
		if (term_get_size(&w, &h)) {
			struct cell *cells = (struct cell*)malloc(w*h*sizeof(struct cell));
			memset(cells, 0, w*h*sizeof(struct cell));

			int j;
			for (j = 50; j < 1000; ++j) {
				int i;
				for (i = 0; i < w*h; ++i) {
					cells[i].fg.r = j;
					cells[i].c = 'x';
				}
				term_render(cells, w*h);
			}
		}

		term_show_cursor();
		term_reset();
		term_close();
	}
}
