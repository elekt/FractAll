/**
 * Fractal drawing application created by Elekes Tamás.
 * Features:
 * Usage:
*/

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <SDL2/SDL.h>
#include <unistd.h>

#define WIDTH 768
#define HEIGHT 768

int MAX_ITER = 128;

typedef struct Complex {
    double im;
    double re;
} Complex;

typedef struct Color {
    int R;
    int G;
    int B;
} Color;


Complex pixelToComplex(int x, int y, double xOffset, double yOffset, double zoomLevel){
   Complex converted = {};
   double heightRatio = zoomLevel * ((double)HEIGHT)/WIDTH;
   converted.re = (x * (heightRatio / HEIGHT) - (heightRatio/2.0)) + xOffset;
   converted.im = (y * (zoomLevel / WIDTH) - (zoomLevel/2.0)) + yOffset;
   return converted;
}

/**
 * Mandelbrot set is the set of values of c in the complex plane for which the orbit of 0 under iteration of the quadratic map
 * zn + 1 = zn^2 + c
 * remains bounded.
 */
Color** countMandelbrot(double zoomLevel, double xOffset, double yOffset){
    int i, j;
    Color** colors = (Color**) malloc(HEIGHT * sizeof(Color*));
    for(i=0; i < HEIGHT; ++i){
    	colors[i] = (Color*) malloc(WIDTH * sizeof(Color));
    }

	#pragma omp parallel shared(colors) private(i, j)
    {

		#pragma omp sections nowait
    	{

			for(i = 0; i < HEIGHT; ++i){
				for(j = 0; j < WIDTH; ++j){
					Complex c = pixelToComplex(i, j, xOffset, yOffset, zoomLevel);

					int iter = 0;
					Complex z = { 0.0, 0.0 };

					#pragma omp section
					while(iter < MAX_ITER && z.re*z.re - z.im*z.im < 4.0){
						double tmp = z.re*z.re - z.im*z.im + c.re;
						z.im = 2.0 * z.re * z.im + c.im;
						z.re = tmp;
						++iter;
					}
					colors[i][j] = *(Color*) malloc(sizeof(Color));
					colors[i][j].R = 255 * (double)iter / MAX_ITER;
					colors[i][j].G = 255 * (double)iter / MAX_ITER;
					colors[i][j].B = 255 * (double)iter / MAX_ITER;
				}
			}
		}
    }
    return colors;
}

void drawMandelbrot(SDL_Surface* screenSurface, Color** mandelbrot) {
	int i, j;
	for (i = 0; i < HEIGHT; ++i) {
		for (j = 0; j < WIDTH; ++j) {
			((Uint32*) screenSurface->pixels)[(i * screenSurface->w) + j] =
					SDL_MapRGB(screenSurface->format, mandelbrot[i][j].R,
							mandelbrot[i][j].G, mandelbrot[i][j].B);
		}
	}
	((Uint32*) screenSurface->pixels)[(HEIGHT/2 * screenSurface->w) + WIDTH/2] =
						SDL_MapRGB(screenSurface->format, 255,0,0);
}

int main(){
	double zoomLevel = 4.0;
	double xOffset = 0.0;
	double yOffset = 0.0;
	Color** mandelbrot = countMandelbrot(zoomLevel, xOffset, yOffset);

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

			drawMandelbrot(screenSurface, mandelbrot);
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
						printf("max iter: %d\n", MAX_ITER);
					} else if(event.key.keysym.sym == SDLK_w){
						MAX_ITER /= 2;
						printf("max iter: %d\n", MAX_ITER);
					}
					mandelbrot = countMandelbrot(zoomLevel, xOffset, yOffset);
					drawMandelbrot(screenSurface, mandelbrot);
					SDL_UpdateWindowSurface( window );
					// printf("zoom: %f x: %f y= %f\n", zoomLevel, xOffset, yOffset);
				} else if(event.type == SDL_KEYUP){
					isKeyDown = 0;
				} else if(event.type == SDL_MOUSEBUTTONDOWN && !isKeyDown){
					isKeyDown = 1;
					Complex center = pixelToComplex(event.button.y, event.button.x, xOffset, yOffset, zoomLevel);
					xOffset = center.re;
					yOffset = center.im;
					printf("zoom: %f x: %f y= %f\n", zoomLevel, xOffset, yOffset);
					mandelbrot = countMandelbrot(zoomLevel, xOffset, yOffset);
					drawMandelbrot(screenSurface, mandelbrot);
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
	SDL_DestroyWindow( window );

	//Quit SDL subsystems
	SDL_Quit();

	return 0;


}
