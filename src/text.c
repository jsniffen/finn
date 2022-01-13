TTF_Font *open_font(char *filename, int size)
{
	TTF_Font *font;

	font = TTF_OpenFont(filename, size);
	if (font == 0) {
		fprintf(stderr, "failed to open font: %s\n", SDL_GetError());
	}

	return font;
}

SDL_Texture *get_text_texture(SDL_Renderer *renderer, TTF_Font *font, char c, SDL_Color color)
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
render_char(SDL_Renderer *r, TTF_Font *f, int *w, int *h, char c, int x, int y)
{
	SDL_Texture *texture;
	SDL_Color color = {0, 0, 0, 0};
	texture = get_text_texture(r, f, c, color);
	SDL_QueryTexture(texture, 0, 0, w, h);
	render_text(r, texture, x, y);
	SDL_DestroyTexture(texture);
}
