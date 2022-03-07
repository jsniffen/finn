#define INITIAL_GAP_SIZE 256

typedef struct Buffer Buffer;

struct Buffer
{
	int size;
	int gap;
	int gapsize;
	char *data;
	SDL_RWops *file;

	char *filename;
};

void bmovegap(Buffer *b, int i);
void bmovegapxy(Buffer *b, SDL_Renderer *r, TTF_Font *f, int point_x, int point_y);

// TODO(Julian): fix error handling and file cleanup.
int bopen(Buffer *b, char *filename)
{
	int64_t size;
	size_t read;

	SDL_RWops *file = NULL;

	b->filename = filename;

	file = SDL_RWFromFile(filename, "r+b");
	if (file == 0) {
		fprintf(stderr, "failed to open file: %s\n", SDL_GetError());
		return -1;
	}

	size = SDL_RWsize(file);
	if (size == -1) {
		fprintf(stderr, "failed to get file size: %s\n", SDL_GetError());
		return -1;
	}

	b->size = INITIAL_GAP_SIZE + size;
	b->data = (char *)malloc(b->size);
	memset(b->data, 0, b->size);

	b->gap = 0;
	b->gapsize = INITIAL_GAP_SIZE;

	read = SDL_RWread(file, b->data+b->gapsize, 1, size);
	if (read < size) {
		fprintf(stderr, "failed to read the entire file: %s\n", SDL_GetError());
		return -1;
	}

	SDL_RWclose(file);
	return 0;
}

int bclose(Buffer *b)
{
	b->size = 0;
	b->gap = 0;
	b->gapsize = 0;
	free(b->data);
	return 0;
}

void bwrite(Buffer *b)
{
	SDL_RWops *newfile = SDL_RWFromFile(b->filename, "w+b");
	if (newfile) {
		const int lsize = b->gap;
		const int rsize = b->size - lsize - b->gapsize;

		const char *lptr = b->data;
		const char *rptr = b->data + lsize + b->gapsize;

		SDL_RWwrite(newfile, lptr, lsize, 1);
		SDL_RWwrite(newfile, rptr, rsize, 1);

		SDL_RWclose(newfile);
	} else {
		fprintf(stderr, "Error opening newfile\n");
	}
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
	int dleft, dprevline;

	dleft = 0;
	while (dleft < b->gap && b->data[b->gap-dleft-1] != '\n') ++dleft;

	// we are on the first line.
	if (dleft == b->gap) return;

	dprevline = 0;
	while (dleft+dprevline+1 < b->gap && b->data[b->gap-dleft-dprevline-2] != '\n') ++dprevline;

	if (dprevline > dleft) {
		bmovegap(b, b->gap-dprevline-1);
	} else {
		bmovegap(b, b->gap-dleft-1);
	}
}

void
bmoved(Buffer *b)
{
	// TODO(Julian): Fix buggy implementation.
	int i, dleft, dright;

	dleft = 0;
	while (dleft < b->gap && b->data[b->gap-dleft-1] != '\n') ++dleft;

	dright = 0;
	while (dright < b->size - b->gapsize - b->gap && b->data[b->gap + b->gapsize + dright] != '\n') ++dright;

	bmovegap(b, b->gap+dleft+dright+1);

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

void bmovegapxy(Buffer *b, SDL_Renderer *r, TTF_Font *f, int point_x, int point_y)
{
	SDL_Point point = {point_x, point_y};

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
		}

		c = b->data[i];

		if (c == '\n') {
			y += h;
			x = 0;
			continue;
		}

		render_char(r, f, &w, &h, c, x, y);

		SDL_Rect rect = {x, y, w, h};
		if (SDL_PointInRect(&point, &rect)) {
			int new_i = i;
			if (new_i > b->gap) {
				new_i -= b->gapsize;
			}

			float mid = (rect.w/2);
			if (point.x-rect.x > mid) {
				++new_i;
			}
			// printf("%c, point(%d, %d), rect(%d, %d, %d, %d)\n", c, point.x, point.y, rect.x, rect.y, rect.w, rect.h);
			// printf("%f, %d\n", mid, point.x-rect.x);

			if (new_i == b->gap) {
				return;
			}

			bmovegap(b, new_i);
		}

		x += w;
	}
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

	if (i < b->gap) {
		diff = b->gap - i;
		src = b->data + i;
		dst = src + b->gapsize;
	} else {
		diff = i - b->gap;
		dst = b->data + b->gap;
		src = dst + b->gapsize;
	}

	memmove(dst, src, diff);
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
