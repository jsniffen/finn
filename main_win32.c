#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

#include "color.c"
#include "cell.c"
#include "term.c"

int term_init()
{
	DWORD mode;

	HANDLE in = GetStdHandle(STD_INPUT_HANDLE);
	HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);

	if (GetConsoleMode(out, &mode)) {
		mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
		if (!SetConsoleMode(out, mode)) {
			fprintf(stderr, "failed to set stdout console mode\n");
			return 0;
		}
	} else {
		fprintf(stderr, "failed to get stdout console mode\n");
		return 0;
	}

	if (GetConsoleMode(in, &mode)) {
		mode |= ENABLE_VIRTUAL_TERMINAL_INPUT;
		mode &= ~(ENABLE_LINE_INPUT|ENABLE_ECHO_INPUT);
		if (!SetConsoleMode(in, mode)) {
			fprintf(stderr, "failed to set stdin console mode\n");
			return 0;
		}
	} else {
		fprintf(stderr, "failed to get stdin console mode\n");
		return 0;
	}

	return 1;
}

int term_get_size(int *w, int *h) {
	CONSOLE_SCREEN_BUFFER_INFO info;
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &info);
	*w = info.srWindow.Right+1;
	*h = info.srWindow.Bottom-info.srWindow.Top+1;

	return 1;
}

int term_close()
{
	return 1;
}

int main()
{
	if (term_init()) {
		term_hide_cursor();

		int w, h;
		if (term_get_size(&w, &h)) {
			struct cell *cells = (struct cell*)malloc(w*h*sizeof(struct cell));
			if (cells) {
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
			} else {
				fprintf(stderr, "malloc failed\n");
			}
		}

		term_show_cursor();
		term_reset();
		term_close();
	} else {
		fprintf(stderr, "term_init failed\n");
	}
}
