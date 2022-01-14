#define INITIAL_GAP_SIZE 256

typedef struct Buffer Buffer;

struct Buffer
{
	int size;
	int gap_start;
	int gap_end;
	char *data;
	SDL_RWops *file;
};

int bopen(Buffer *b, char *filename)
{
	int64_t size;
	size_t read;

	b->file = SDL_RWFromFile(filename, "r+b");
	if (b->file == 0) {
		fprintf(stderr, "failed to open file: %s\n", SDL_GetError());
		return -1;
	}

	size = SDL_RWsize(b->file);
	if (size == -1) {
		fprintf(stderr, "failed to get file size: %s\n", SDL_GetError());
		return -1;
	}

	b->size = INITIAL_GAP_SIZE + size;
	b->data = (char *)malloc(b->size);
	memset(b->data, 0, b->size);

	b->gap_start = 0;
	b->gap_end = 255;

	read = SDL_RWread(b->file, b->data+b->gap_end, 1, size);
	if (read < size) {
		fprintf(stderr, "failed to read the entire file: %s\n", SDL_GetError());
		return -1;
	}

	return 0;
}

int bclose(Buffer *b)
{
	b->size = 0;
	b->gap_start = 0;
	b->gap_end = 0;
	if (SDL_RWclose(b->file) < 0) {
		fprintf(stderr, "failed to close file: %s\n", SDL_GetError());
	}
	free(b->data);
	return 0;
}

int binsert(Buffer *b, char c)
{
	if (b->gap_end - b->gap_start <= 0) {
		// TODO(Julian): handle resize.
		return 1;
	}

	b->data[b->gap_start++] = c;
	return 0;
}

int bremove(Buffer *b)
{
	if (b->gap_start == 0) {
		fprintf(stderr, "cannot backspace when the gap_start is 0\n");
		return 1;
	}

	--b->gap_start;
	return 0;
}

void
bmoveu(Buffer *b)
{
	// TODO(Julian): Buggy implementation. Fix this...
	if (b->gap_start <= 0) return;

	int scanl, distl, gap_size, diff;
	gap_size = b->gap_end - b->gap_start;

	scanl = b->gap_start-1;
	while (scanl > 0 && b->data[scanl] != '\n') --scanl;
	distl = b->gap_start-scanl;

	--scanl;
	while (scanl > 0 && b->data[scanl] != '\n') --scanl;

	if (distl-scanl > 2)  {
		scanl += distl;
	} else {
		scanl -= 2;
	}

	diff = b->gap_start-scanl;
	fprintf(stdout, "%d\n", diff);

	memcpy(b->data + scanl + gap_size, b->data + scanl, diff);
	b->gap_start = scanl;
	b->gap_end = scanl + gap_size;
}

void
bmoved(Buffer *b)
{
	return;
}

void
bmover(Buffer *b)
{
	if (b->gap_end == b->size) return;

	b->data[b->gap_start++] = b->data[b->gap_end++];
}

void
bmovel(Buffer *b)
{
	if (b->gap_start <= 0) return;

	b->data[--b->gap_start] = b->data[b->gap_end--];
}

int bmove(Buffer *b, int i)
{
	if (i < 0 || i > b->size - INITIAL_GAP_SIZE) {
		fprintf(stderr, "invalid gap placement: %d\n", i);
		return 1;
	}

	if (b->gap_start == i) return 0;

	if (b->gap_start < i) {
		// copy backwards
	} else {
		// copy forwards
	}

	// TODO(Julian): handle moving gap Buffer.

	return 0;
}

int brender(Buffer *b, SDL_Renderer *r, TTF_Font *f)
{
	int i, size, w, h, x, y;
	char c;
	SDL_Texture *texture;
	SDL_Color color = {0, 0, 0};

	w = 0;
	h = 0;
	x = 0;
	y = 0;
	for (i = 0; i < b->size; ++i) {
		if (i == b->gap_start) {
			i = b->gap_end-1;

			SDL_Rect rect = {x, y, 2, 30};
			SDL_SetRenderDrawColor(r, 0, 0, 0, SDL_ALPHA_OPAQUE);
			SDL_RenderFillRect(r, &rect);

			continue;
		}

		c = b->data[i];

		if (c == '\n') {
			y += h;
			x = 0;
			continue;
		}

		render_char(r, f, &w, &h, c, x, y);

		x += w;
	}
	return 0;
}
