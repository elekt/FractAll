/**
The MIT License (MIT)

Copyright (c) 2016 Elekes Tamás

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <SDL2/SDL.h>
#include <unistd.h>

#include "complex.h"
#include "fractal_function.h"
#include "color.h"

#define WIDTH 768
#define HEIGHT 768

int MAX_ITER = 256;

struct Color** countFractal(fractalFun fun, double zoomLevel, double xOffset, double yOffset){
    int i, j;
    struct  Color** colors = (struct Color**) malloc(HEIGHT * sizeof(Color*));
    for(i=0; i < HEIGHT; ++i){
    	colors[i] = (struct Color*) malloc(WIDTH * sizeof(struct Color));
    }

	for(i = 0; i < HEIGHT; ++i){
		for(j = 0; j < WIDTH; ++j){
			struct Complex c = pixelToComplex(WIDTH, HEIGHT, i, j, xOffset, yOffset, zoomLevel);

			int iter = fun(c, MAX_ITER);

			double colorRatio = (1 - (double)iter / MAX_ITER);
			colors[i][j] = *(struct Color*) malloc(sizeof(struct Color));
			colors[i][j].R = 255 * colorRatio;
			colors[i][j].G = 255 * 0.8 * colorRatio;
			colors[i][j].B = 255 * 0.8 * colorRatio;

			// drawing with only grey colors
//			colors[i][j].R = iter % MAX_ITER;
//			colors[i][j].G = iter % MAX_ITER;
//			colors[i][j].B = iter % MAX_ITER;
		}
    }
    return colors;
}

void drawFractal(SDL_Surface* screenSurface, struct Color** fractal) {
	int i, j;
	for (i = 0; i < HEIGHT; ++i) {
		for (j = 0; j < WIDTH; ++j) {
			((Uint32*) screenSurface->pixels)[(i * screenSurface->w) + j] =
					SDL_MapRGB(screenSurface->format, fractal[i][j].R,
							fractal[i][j].G, fractal[i][j].B);
		}
	}
	((Uint32*) screenSurface->pixels)[(HEIGHT/2 * screenSurface->w) + WIDTH/2] =
						SDL_MapRGB(screenSurface->format, 255,0,0);
}

int main(){
	double zoomLevel = 4.0;
	double xOffset = 0.0;
	double yOffset = 0.0;
	fractalFun fractalFunction = mandelbrot;
	struct Color** mandelbrotColors = countFractal(fractalFunction, zoomLevel, xOffset, yOffset);

	SDL_Window* window = NULL;
	SDL_Surface* screenSurface = NULL;

	if( SDL_Init( SDL_INIT_VIDEO ) < 0 ) {
		printf( "SDL could not initialize! SDL_Error: %s\n", SDL_GetError() );
	} else {
		//Create window
		window = SDL_CreateWindow( "FractAll", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN );
		if( window == NULL ){
			printf( "Window could not be created! SDL_Error: %s\n", SDL_GetError() );
		} else {
			screenSurface = SDL_GetWindowSurface( window );

			drawFractal(screenSurface, mandelbrotColors);
			SDL_UpdateWindowSurface( window );

			int isKeyDown = 0;
			double deltaOffset = 0.2;

			SDL_Event event;
			while(1) {
				SDL_PollEvent(&event);
				if(event.type == SDL_KEYDOWN && !isKeyDown){
					isKeyDown = 1;
					if(event.key.keysym.sym == SDLK_ESCAPE){
						exit(0);
					} else if(event.key.keysym.sym == SDLK_PLUS || event.key.keysym.sym == SDLK_KP_PLUS){
						zoomLevel -= zoomLevel/10.0;
						deltaOffset /= 1.05;
					} else if(event.key.keysym.sym == SDLK_MINUS || event.key.keysym.sym == SDLK_KP_MINUS){
						zoomLevel += zoomLevel/10.0;
						deltaOffset *= 1.05;
					} else if(event.key.keysym.sym == SDLK_UP){
						xOffset -= deltaOffset;
					} else if(event.key.keysym.sym == SDLK_DOWN){
						xOffset += deltaOffset;
					} else if(event.key.keysym.sym == SDLK_RIGHT){
						yOffset += deltaOffset;
					} else if(event.key.keysym.sym == SDLK_LEFT){
						yOffset -= deltaOffset;
					} else if(event.key.keysym.sym == SDLK_q){
						MAX_ITER *= 2;
					} else if(event.key.keysym.sym == SDLK_w && MAX_ITER > 1){
						MAX_ITER /= 2;
					}
					mandelbrotColors = countFractal(fractalFunction, zoomLevel, xOffset, yOffset);
					drawFractal(screenSurface, mandelbrotColors);
					SDL_UpdateWindowSurface( window );
				} else if(event.type == SDL_KEYUP){
					isKeyDown = 0;
				} else if(event.type == SDL_MOUSEBUTTONDOWN && !isKeyDown){
					isKeyDown = 1;
					struct Complex center = pixelToComplex(WIDTH, HEIGHT, event.button.y, event.button.x, xOffset, yOffset, zoomLevel);
					xOffset = center.re;
					yOffset = center.im;
					mandelbrotColors = countFractal(fractalFunction, zoomLevel, xOffset, yOffset);
					drawFractal(screenSurface, mandelbrotColors);
					SDL_UpdateWindowSurface( window );
				} else if(event.type == SDL_MOUSEBUTTONUP){
					isKeyDown = 0;
				} else if(event.type == SDL_QUIT){
					exit(0);
				}
			}
		}
	}

	//Destroy window
	SDL_DestroyWindow(window);

	//Quit SDL subsystems
	SDL_Quit();

	return 0;


}
