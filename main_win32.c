#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <synchapi.h>

#include "color.c"
#include "cell.c"
#include "term.c"
#include "gap_buffer.c"
#include "finn.c"

int term_init_os()
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

int term_close_os()
{
	return 1;
}

int term_read_os(char *buf, int len, int *read)
{
	if (ReadFile(GetStdHandle(STD_INPUT_HANDLE), buf, len, read, 0)) {
		return 1;
	} else {
		fprintf(stderr, "ReadFile failed\n");
		return 0;
	}
}

int main()
{
	run();
}
