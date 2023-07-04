void run()
{
	if (term_init()) {
		term_hide_cursor();

		int i;
		for (i = 0; i < 1000; ++i) {
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
				.c  = 'x',
			};

			term_setcell(i, i, cell);
			term_render();
		}

		term_show_cursor();
		term_reset();
		term_close();
	} else {
		fprintf(stderr, "term_init failed\n");
	}
}
