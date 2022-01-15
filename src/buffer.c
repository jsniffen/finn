#define INITIAL_GAP_SIZE 256

typedef struct Buffer Buffer;

struct Buffer
{
	int size;
	int gap;
	int gapsize;
	char *data;
	SDL_RWops *file;
};

void bmovegap(Buffer *b, int i);

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

	b->gap = 0;
	b->gapsize = INITIAL_GAP_SIZE;

	read = SDL_RWread(b->file, b->data+b->gapsize, 1, size);
	if (read < size) {
		fprintf(stderr, "failed to read the entire file: %s\n", SDL_GetError());
		return -1;
	}

	return 0;
}

int bclose(Buffer *b)
{
	b->size = 0;
	b->gap = 0;
	b->gapsize = 0;
	if (SDL_RWclose(b->file) < 0) {
		fprintf(stderr, "failed to close file: %s\n", SDL_GetError());
	}
	free(b->data);
	return 0;
}

void
binsert(Buffer *b, char c)
{
	if (b->gapsize <= 0) {
		// TODO(Julian): handle resize.
		return;
	}

	b->data[b->gap++] = c;
	--b->gapsize;
}

void
bremove(Buffer *b)
{
	if (b->gap == 0) {
		fprintf(stderr, "cannot backspace when the gap is at 0\n");
		return;
	}

	--b->gap;
	++b->gapsize;
}

void
bmoveu(Buffer *b)
{
	return;
}

void
bmoved(Buffer *b)
{
	return;
}

void
bmover(Buffer *b)
{
	bmovegap(b, b->gap+1);
}

void
bmovel(Buffer *b)
{
	bmovegap(b, b->gap-1);
}

void
bmovegap(Buffer *b, int i)
{
	if (i < 0 || i > b->size - b->gapsize) {
		fprintf(stderr, "invalid gap placement: %d\n", i);
		return;
	}

	if (b->gap == i) return;

	char *dst, *src;
	int diff;

	src = b->data + i;

	if (i < b->gap) {
		diff = b->gap - i;
		dst = src + b->gapsize;
		memmove(dst, src, diff);
	} else {
		diff = i - b->gap;
		dst = src - b->gapsize;

		//TODO(Julian): There is a bug here...
		memmove(dst, src, b->gapsize);
	}

	b->gap = i;
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
		if (i == b->gap) {
			i = b->gap+b->gapsize-1;

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
