#define INITIAL_GAP_SIZE 256
#define INITIAL_LINES_SIZE 256

typedef struct
{
	uint64_t size;
	uint64_t gap;
	uint64_t gapsize;
	uint32_t lines[INITIAL_LINES_SIZE];
	uint32_t nlines;
	uint8_t *data;
} GapBuffer;

void find_lines(GapBuffer *gb)
{
	gb->nlines = 0;

	int i;
	char c;

	for (i = 0; i < gb->size; ++i) {
		if (i == gb->gap) {
			i += gb->gapsize;
		}

		c = gb->data[i];
		if (c == '\n') {
			gb->lines[gb->nlines++] = i;
			++i;
		}
	}
}

void gb_create(GapBuffer *gb, uint8_t *data, uint64_t size)
{
	// TODO(Julian): handle error
	gb->data = (uint8_t *)malloc(size + INITIAL_GAP_SIZE);
	memset(gb->data, 0, INITIAL_GAP_SIZE);
	memcpy(gb->data + INITIAL_GAP_SIZE, data, size);

	memset(gb->lines, 0, INITIAL_LINES_SIZE);

	gb->gap = 0;
	gb->gapsize = INITIAL_GAP_SIZE;
	gb->size = size + INITIAL_GAP_SIZE;

	find_lines(gb);
	printf("%d\n", gb->nlines);
}

void gb_delete(GapBuffer *gb)
{
	gb->size = 0;
	gb->gap = 0;
	gb->gapsize = 0;

	// TODO(Julian): handle error
	free(gb->data);
	gb->data = NULL;
}

void gb_insert(GapBuffer *gb, uint8_t rune)
{
	// TODO(Julian): handle resize.
	SDL_assert(gb->gapsize > 1);

	gb->data[gb->gap++] = rune;
	--gb->gapsize;
}

void gb_movegap(GapBuffer *gb, uint64_t gap)
{
	if (gap < 0 || gap > gb->size - gb->gapsize) {
		fprintf(stderr, "invalid gap placement: %lld\n", gap);
		return;
	}

	if (gb->gap == gap) return;

	uint8_t *dst, *src;
	int32_t diff;

	if (gap < gb->gap) {
		diff = gb->gap - gap;
		src = gb->data + gap;
		dst = src + gb->gapsize;
	} else {
		diff = gap - gb->gap;
		dst = gb->data + gb->gap;
		src = dst + gb->gapsize;
	}

	memmove(dst, src, diff);
	gb->gap = gap;
}

void gb_remove(GapBuffer *gb)
{
	if (gb->gap == 0) {
		fprintf(stderr, "cannot backspace when the gap is at 0\n");
		return;
	}

	--gb->gap;
	++gb->gapsize;
}

void gb_render(GapBuffer *gb, SDL_Renderer *r, MouseInput mouse, SDL_Rect pos, SDL_Color fg, bool active)
{
	int i, size, w, h, x, y;
	char rune;

	x = pos.x;
	y = pos.y;

	for (i = 0; i < gb->size; ++i) {
		if (i == gb->gap) {
			if (active) {
				render_cursor(r, x, y);
			}

			i += gb->gapsize-1;
			continue;
		}

		rune = gb->data[i];

		if (rune == '\n') {
			y += h;
			x = pos.x;
			continue;
		}

		get_rune_size(rune, &w, &h);

		SDL_Color color = {0, 0, 0, 0};
		SDL_Rect rect = {x, y, w, h};

		if (SDL_PointInRect(&mouse.position, &rect) && lclick(mouse)) {

			color.r = 255;

			int gap = i;

			if (mouse.position.x-x > w/2) {
				gap += 1;
			}

			if (gap < gb->gap) {
				gb_movegap(gb, gap);
			} else {
				gb_movegap(gb, gap-gb->gapsize);
			}
		} 

		if (x >= pos.x && x < pos.x + pos.w && y >= pos.y && y < pos.y + pos.h) {
			render_char(r, &w, &h, rune, x, y, fg);
		}

		x += w;
	}
}
