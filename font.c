#include "font.h"

#include <assert.h>
#include <limits.h>
#include <stdlib.h>

#include <zlib.h>

#include "error.h"

/* Create a texture atlas containing all of the glyphs in a font. */
SDL_Texture *font_create_texture(SDL_Renderer *renderer, PSFFont *font)
{
	SDL_Texture *result;

	const int bytes_per_row = (font->bytes_per_glyph / font->height);
	const int texture_width = font->num_glyphs * font->width;
	SDL_Surface *surface = SDL_CreateRGBSurface(0, texture_width, font->height, 32, 0, 0, 0, 0);

	for (int glyph_idx = 0; glyph_idx < font->num_glyphs; glyph_idx++) {
		for (int y = 0; y < font->height; y++) {
			for (int x = 0; x < font->width; x++) {
				/*
				 * TODO: Make it possible for the texture atlas to have more
				 * than one row.
				 */
				int xp = x + glyph_idx * font->width;
				int yp = y;

				int bit_idx = 7 - (x % 8);
				int row_byte_idx = (glyph_idx * font->bytes_per_glyph) + y * bytes_per_row + (x / 8);
				char current_bit = font->glyph_data[row_byte_idx] & (1 << bit_idx);
				((Uint32*)(surface->pixels))[xp + yp * surface->w] = current_bit ? 0xffffffff : 0;
			}
		}
	}

	result = SDL_CreateTextureFromSurface(renderer, surface);
	if (result == NULL)
		fatal_error("Failed to create texture: %s\n", SDL_GetError());

	SDL_FreeSurface(surface);

	printf("Created font texture atlas of size %dx%d\n", surface->w, surface->h);
	return result;
}

PSFFont font_load(const char *filename)
{
	PSFFont font;

	gzFile file = gzopen(filename, "rb");
	if (!file)
		fatal_error("Failed to open font from '%s'\n", filename);

	gzread(file, &font.magic, 4);
	if (font.magic != PSF_MAGIC_NUMBER)
		fatal_error("Font header mismatch! '%s' has magic value %x.\n", filename, font.magic);

	gzread(file, &font.version, 4);
	gzread(file, &font.header_size, 4);
	gzread(file, &font.flags, 4);
	gzread(file, &font.num_glyphs, 4);
	gzread(file, &font.bytes_per_glyph, 4);
	gzread(file, &font.height, 4);
	gzread(file, &font.width, 4);

	size_t glyph_buffer_size = font.num_glyphs * font.bytes_per_glyph;
	font.glyph_data = malloc(glyph_buffer_size);
	gzseek(file, font.header_size, SEEK_SET);
	gzread(file, font.glyph_data, font.bytes_per_glyph * font.num_glyphs);

	font.unicode_desc = NULL;
	if (font.flags == PSF_FLAG_UNICODE) {
		/* Store the file's unicode information in a buffer. */
		char *desc = malloc(UNICODE_TABLE_SIZE);
		gzread(file, desc, UNICODE_TABLE_SIZE);

		/* Create a buffer in our object to map character codes to glyphs */
		font.unicode_desc = calloc(USHRT_MAX, 2);

		int glyph_index = 0;
		unsigned char letter = 0;
		for (int i = 0; i < UNICODE_TABLE_SIZE; i++) {
			unsigned char uc = desc[i];
			if (uc == 0xff) {
				font.unicode_desc[letter] = glyph_index;
				glyph_index++;
			} else {
				letter = uc;
			}
		}
	}

	gzclose(file);

	printf("Loaded a font with %d glyphs of size %dx%d (%d bytes per glyph)\n", 
			font.num_glyphs, font.width, font.height, font.bytes_per_glyph);
	return font;
}

void font_destroy(PSFFont *font)
{
	free(font->glyph_data);
	free(font->unicode_desc);
}
