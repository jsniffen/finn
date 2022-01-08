#define INITIAL_GAP_SIZE 256

typedef struct buffer {
	int size;
	int gap_start;
	int gap_end;
	char *data;
	SDL_RWops *file;
} buffer;

// create a new buffer. return -1 on error, 0 on success.
int open_buffer(buffer *b, char *filename)
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

int close_buffer(buffer *b)
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

int insert_buffer(buffer *b, char c)
{
	if (b->gap_end - b->gap_start <= 0) {
		// TODO(Julian): handle resize.
		return 1;
	}

	b->data[b->gap_start++] = c;
	return 0;
}

int backspace_buffer(buffer *b)
{
	if (b->gap_start == 0) {
		fprintf(stderr, "cannot backspace when the gap_start is 0\n");
		return 1;
	}
	
	--b->gap_start;
	return 0;
}

int move_gap(buffer *b, int i)
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

	// TODO(Julian): handle moving gap buffer.

	return 0;
}

int render_buffer(SDL_Renderer *r, buffer *b)
{
	return 0;
}
