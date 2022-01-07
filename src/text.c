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


void render_text(SDL_Renderer *renderer, SDL_Texture *texture, int x, int y)
{
	int w, h;
	SDL_QueryTexture(texture, 0, 0, &w, &h);
	SDL_Rect quad = {x, y, w, h};
	SDL_RenderCopyEx(renderer, texture, 0, &quad, 0, 0, 0);
}

void render_file(SDL_Renderer *renderer, TTF_Font *font, SDL_RWops *file, int x, int y)
{
	int i, size, w, h;
	SDL_Texture *texture;
	char buf[1024];
	SDL_Color color = {0, 0, 0};

	SDL_RWseek(file, 0, RW_SEEK_SET);
	
	size = SDL_RWsize(file);
	SDL_RWread(file, buf, sizeof(buf), 1);

	for (i = 0; i < size; ++i) {
		if (buf[i] == '\n') {
			y += h;
			x = 0;
			continue;
		}
		
		texture = get_text_texture(renderer, font, buf[i], color);
		SDL_QueryTexture(texture, 0, 0, &w, &h);
		render_text(renderer, texture, x, y);
		SDL_DestroyTexture(texture);

		x += w;
	}
}
