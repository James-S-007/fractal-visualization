/* 
Author: Jack Crandell & James Springer
Class: ECE 4122
Last Date Modified: 12/07/21
 
Description: Mandelbrot implementation via OpenMP
*/

#pragma once

namespace omp {

    // Calculates mandelbrot set via multi-threading and calls necessary OpenGL functions
    void display(int width, int height, float zoom, float frame_x, float frame_y);

    // Calculates mandelbrot set via multi-threading
    void mandelbrotSet(int width, int height, float zoom, float frame_x, float frame_y);

    // Calculates number of iterations for a specific pixel
    int getIterations(int i, int j, int width, int height, float zoom, float frame_x, float frame_y);

    // Draws a single pixel in matrix
    void drawPoint(int i, int j, int height, int color);

    // Sets color of a single pixel in matrix
    void setColor(int color);

}  // namespace omp
