#pragma once
#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
/* 
this function came from:
https://stackoverflow.com/questions/53033971/how-to-get-the-color-of-a-specific-pixel-from-sdl-surface 
*/
Uint32 getpixel(SDL_Surface *surface, int x, int y)
{
    int bpp = surface->format->BytesPerPixel;
    /* Here p is the address to the pixel we want to retrieve */
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

switch (bpp)
{
    case 1:
        return *p;
        break;

    case 2:
        return *(Uint16 *)p;
        break;

    case 3:
        if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
            return p[0] << 16 | p[1] << 8 | p[2];
        else
            return p[0] | p[1] << 8 | p[2] << 16;
            break;

        case 4:
            return *(Uint32 *)p;
            break;

        default:
            return 0;       /* shouldn't happen, but avoids warnings */
      }
}
/* 
this function came from:
https://stackoverflow.com/questions/6852055/how-can-i-modify-pixels-using-sdl
*/
void PutPixel24_nolock(SDL_Surface * surface, int x, int y, Uint32 color)
{
    Uint8 * pixel = (Uint8*)surface->pixels;
    pixel += (y * surface->pitch) + (x * sizeof(Uint8) * 3);
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    pixel[0] = (color >> 24) & 0xFF;
    pixel[1] = (color >> 16) & 0xFF;
    pixel[2] = (color >> 8) & 0xFF;
#else
    pixel[0] = color & 0xFF;
    pixel[1] = (color >> 8) & 0xFF;
    pixel[2] = (color >> 16) & 0xFF;
#endif
}
void PutPixel24(SDL_Surface * surface, int x, int y, Uint32 color)
{
    if( SDL_MUSTLOCK(surface) )
        SDL_LockSurface(surface);
    PutPixel24_nolock(surface, x, y, color);
    if( SDL_MUSTLOCK(surface) )
        SDL_LockSurface(surface);
}

void set_pixel_colour(SDL_Surface* game_map,int x, int y, int value)
{
	/*change the colour of a pixel, just changes all rgb values the same*/
    if(x < game_map->w && y < game_map->h && x > 0 && y > 0){
	    PutPixel24(game_map, x, y, (Uint32)0xFF000000 | value | (value << 16) | (value << 8));
    }
}

int get_pixel_darkness(SDL_Surface* game_map, int x, int y)
{
	/*return how dark a pixel is*/
    if(x < game_map->w && y < game_map->h && x > 0 && y > 0){
        SDL_Color rgb;
        Uint32 data = getpixel(game_map, x, y);
        SDL_GetRGB(data, game_map->format, &rgb.r, &rgb.g, &rgb.b);
        return (int)(rgb.b+rgb.r+rgb.g)/3;
    }
    return 0;
}
