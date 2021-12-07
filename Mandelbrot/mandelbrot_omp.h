#pragma once

namespace omp {

    void display(int width, int height, float zoom, float frame_x, float frame_y);
    void mandelbrot_set(int width, int height, float zoom, float frame_x, float frame_y);
    int get_iterations(int i, int j, int width, int height, float zoom, float frame_x, float frame_y);
    void draw_point(int i, int j, int height, int color);
    void set_color(int color);

}  // namespace omp
