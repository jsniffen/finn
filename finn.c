char insert_string[6] = {'I', 'N', 'S', 'E', 'R', 'T'};
char normal_string[6] = {'N', 'O', 'R', 'M', 'A', 'L'};
char visual_string[6] = {'V', 'I', 'S', 'U', 'A', 'L'};

enum finn_mode
{
	mode_insert,
	mode_normal,
	mode_visual,
};

static enum finn_mode finn_mode;

void render_status_bar()
{
	int y = term_get_height()-1;

	struct cell cell = {
		.bg = {
			.r = 0,
			.g = 0,
			.b = 255,
		},
		.fg = {
			.r = 255,
			.g = 255,
			.b = 255,
		},
		.c = ' ',
	};

	for (int x = 0; x < term_get_width(); ++x) {
		if (x < 6) {
			if (finn_mode == mode_insert) {
				cell.c = insert_string[x];
			} else if (finn_mode == mode_normal) {
				cell.c = normal_string[x];
			} else if (finn_mode == mode_visual) {
				cell.c = visual_string[x];
			}
		} else {
			cell.c = ' ';
		}
		term_setcell(x, y, cell);
	}
}

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
					finn_mode = mode_insert;
				} else if (e.key_code == key_up) {
					finn_mode = mode_normal;
				} else if (e.key_code == key_right) {
					finn_mode = mode_visual;
				}
			}

			render_status_bar();

			term_render();
		}


		term_show_cursor();
		term_reset();
		term_close();
	} else {
		fprintf(stderr, "term_init failed\n");
	}
}
