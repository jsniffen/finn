void run()
{
	if (term_init()) {
		term_hide_cursor();

		struct term_event e;
		char c;
		int i;

		while (1) {

			if (term_get_event(&e)) {
				if (e.key_code == key_left) {
					c = 'h';
				} else {
					c = 'l';
				}
			}

			for (i = 0; i < 10; ++i) {
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
					.c  = c,
				};

				term_setcell(i, i, cell);
				term_render();
			}
		}


		term_show_cursor();
		term_reset();
		term_close();
	} else {
		fprintf(stderr, "term_init failed\n");
	}
}
