<h1>FractAll - Mandelbrot renderer</h1>

The mandelbrot set is a popular fractal based on a set of complex numbers.
For info about it, please visit wikipedia.
https://en.wikipedia.org/wiki/Mandelbrot_set

<b>Build the program in Linux terminal:</b>
gcc *.c *.h -O0 -g3 -Wall -lSDL2 -lm -o FractAll

<b>Run it:</b>
./FractAll

Or you can use the IDE of your choice.

<h2>Usage</h2>
'+' : zoom in

'-' : zoom out

'q' : increase the max iteration for 1 pixel

'w' : decrease the max iteration for 1 pixel

arrows : moving the center

left mouse click : make the center position

![Mandelbrot 0](http://i.imgur.com/HPrjmCJ.jpg)

![Mandelbrot 1](http://i.imgur.com/k2FtoFf.png)
