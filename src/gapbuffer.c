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
	b->data = (uint8_t *)malloc(size + INITIAL_GAP_SIZE);
	memset(b->data, 0, INITIAL_GAP_SIZE);
	memcpy(b->data + INITIAL_GAP_SIZE, data, size);

	gb->gap = 0;
	gb->gapsize = INITIAL_GAP_SIZE;
	gb->size = size;
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
	SDL_assert(b->gapsize > 1);

	b->data[b->gap++] = rune;
	--b->gapsize;
}

void gb_movegap(GapBuffer *gb, uint64_t gap)
{
	if (gap < 0 || gap > b->size - b->gapsize) {
		fprintf(stderr, "invalid gap placement: %d\n", i);
		return;
	}

	if (b->gap == gap) return;

	uint8_t *dst, *src;
	int32_t diff;

	if (gap < b->gap) {
		diff = b->gap - gap;
		src = b->data + gap;
		dst = src + b->gapsize;
	} else {
		diff = gap - b->gap;
		dst = b->data + b->gap;
		src = dst + b->gapsize;
	}

	memmove(dst, src, diff);
	b->gap = gap;
}

