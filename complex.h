/*
 * complex
 *
 *  Created on: Jun 30, 2016
 *      Author: redmachine
 */

#ifndef COMPLEX_H
#define COMPLEX_H

typedef struct Complex {
    double im;
    double re;
}Complex;

struct Complex pixelToComplex(int screenWidth, int sreenHeight, int x, int y, double xOffset, double yOffset, double zoomLevel);

#endif /* COMPLEX_H */
