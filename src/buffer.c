#define INITIAL_GAP_SIZE 256

typedef struct Buffer Buffer;

struct Buffer
{
	bool dirty;
	int size;
	int gap;
	int gapsize;
	char *data;
	SDL_RWops *file;

	char *filename;
};

void bmovegap(Buffer *b, int i);

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

	b->dirty = false;
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

		b->dirty = false;
	} else {
		fprintf(stderr, "Error opening newfile\n");
	}
}

void binsert(Buffer *b, char c)
{
	b->dirty = true;

	// TODO(Julian): handle resize.
	SDL_assert(b->gapsize > 1);
	if (b->gapsize <= 0) {
		return;
	}

	b->data[b->gap++] = c;
	--b->gapsize;
}

void binserttext(Buffer *b, char *text)
{
	// TODO(Julian): handle: resize
	char c;
	while ((c = *text++) != '\0') {
		binsert(b, c);
	}
}

void
bremove(Buffer *b)
{
	b->dirty = true;

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

int brender(Buffer *b, SDL_Renderer *r)
{
	int mousex, mousey;
	int state = SDL_GetMouseState(&mousex, &mousey);
	int i, size, w, h, x, y;
	char c;
	SDL_Texture *texture;
	SDL_Color color = {0, 0, 0};

	SDL_Point mouse = {mousex, mousey};

	w = h = x = y = 0;

	int font_height = get_font_height();

	SDL_Rect save_button_rect = {x, y, font_height, font_height};

	if (SDL_PointInRect(&mouse, &save_button_rect) && state & SDL_BUTTON_LMASK) {
		bwrite(b);
	}

	if (b->dirty) {
		SDL_SetRenderDrawColor(r, 255, 0, 0, 255);
	} else {
		SDL_SetRenderDrawColor(r, 0, 0, 255, 255);
	}
	SDL_RenderFillRect(r, &save_button_rect);
	x += font_height;

	char *filename = b->filename;
	while ((c = *filename++) != '\0') {
		SDL_Color color = {0, 0, 0};
		render_char(r, &w, &h, c, x, y, color);
		x += w;
	}
	x = 0;

	y = font_height;
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

		get_rune_size(c, &w, &h);

		SDL_Color color = {0, 0, 0, 0};
		SDL_Rect rect = {x, y, w, h};

		if (SDL_PointInRect(&mouse, &rect) && state & SDL_BUTTON_LMASK) {
			color.r = 255;

			int gap = i;

			if (mousex-x > w/2) {
				gap += 1;
			}

			if (gap < b->gap) {
				bmovegap(b, gap);
			} else {
				bmovegap(b, gap-b->gapsize);
			}
		} 

		render_char(r, &w, &h, c, x, y, color);

		x += w;
	}
	return 0;
}
