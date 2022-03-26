#define INITIAL_GAP_SIZE 256

typedef struct
{
	uint64_t size;
	uint64_t gap;
	uint64_t gapsize;
	uint8_t *data;
} GapBuffer;

void gb_create(GapBuffer *gb, uint8_t *data, uint64_t size)
{
	// TODO(Julian): handle error
	gb->data = (uint8_t *)malloc(size + INITIAL_GAP_SIZE);
	memset(gb->data, 0, INITIAL_GAP_SIZE);
	memcpy(gb->data + INITIAL_GAP_SIZE, data, size);

	gb->gap = 0;
	gb->gapsize = INITIAL_GAP_SIZE;
	gb->size = size + INITIAL_GAP_SIZE;
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

void gb_render(GapBuffer *gb, SDL_Renderer *r, SDL_Rect pos, SDL_Color fg)
{

	int i, size, w, h, x, y;
	char rune;

	x = pos.x;
	y = pos.y;

	for (i = 0; i < gb->size; ++i) {
		if (i == gb->gap) {
			i += gb->gapsize-1;
			continue;
		}

		rune = gb->data[i];

		if (rune == '\n') {
			y += h;
			x = pos.x;
			continue;
		}

		if (x >= pos.x && x < pos.x + pos.w && y >= pos.y && y < pos.y + pos.h) {
			render_char(r, &w, &h, rune, x, y, fg);
		}

		x += w;
	}
}
