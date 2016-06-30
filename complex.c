#include "complex.h"

struct Complex pixelToComplex(int screenWidth, int screenHeight, int x, int y, double xOffset, double yOffset, double zoomLevel){
   struct Complex converted = {};
   double heightRatio = zoomLevel * ((double)screenHeight)/screenWidth;
   converted.re = (x * (heightRatio / screenHeight) - (heightRatio/2.0)) + xOffset;
   converted.im = (y * (zoomLevel / screenWidth) - (zoomLevel/2.0)) + yOffset;
   return converted;
}
