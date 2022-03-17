static TTF_Font *font = NULL;
static int font_height = 0;

bool open_font(char *filename, int size)
{
	font = TTF_OpenFont(filename, size);
	if (font == NULL) {
		return false;
	}
	font_height = TTF_FontHeight(font);
	return true;
}

bool get_rune_size(Uint16 rune, int *width, int *height)
{
	int minx, miny, maxx, maxy, advance;

	if (font == NULL) {
		return false;
	}

	if (TTF_GlyphMetrics(font, rune, &minx, &maxx, &miny, &maxy, &advance) == -1) {
		return false;
	}

	*width = advance;
	*height = font_height;

	return true;
}

SDL_Texture *get_text_texture(SDL_Renderer *renderer, char c, SDL_Color color)
{
	SDL_Texture *texture;
	SDL_Surface *surface;

	if (c <= 0) return 0;

	surface = TTF_RenderGlyph_Blended(font, c, color);
	if (surface == 0) {
		fprintf(stderr, "failed to render text: %d to surface: %s\n", c, TTF_GetError());
		return 0;
	}

	texture = SDL_CreateTextureFromSurface(renderer, surface);
	if (texture == 0) {
		fprintf(stderr, "failed to create texture from surface: %s\n", SDL_GetError());
	}

	SDL_FreeSurface(surface);

	return texture;
}


void
render_text(SDL_Renderer *renderer, SDL_Texture *texture, int x, int y)
{
	int w, h;
	SDL_QueryTexture(texture, 0, 0, &w, &h);
	SDL_Rect quad = {x, y, w, h};
	SDL_RenderCopyEx(renderer, texture, 0, &quad, 0, 0, 0);
}

void
render_char(SDL_Renderer *r, int *w, int *h, char c, int x, int y, SDL_Color color)
{
	SDL_Texture *texture;
	texture = get_text_texture(r, c, color);
	SDL_QueryTexture(texture, 0, 0, w, h);
	render_text(r, texture, x, y);
	SDL_DestroyTexture(texture);
}
