#version 330 core
in vec4 gl_FragCoord;

out vec4 frag_color;

uniform float zoom;
uniform float frame_x;
uniform float frame_y;

#define MAX_ITERATIONS 50
 
// mainly code from:
// https://physicspython.wordpress.com/2020/02/16/visualizing-the-mandelbrot-set-using-opengl-part-1/

int get_iterations() {
    float real = ((gl_FragCoord.x / 600.0f - 0.5f) * zoom + frame_x) * 5.0;
    float imag = ((gl_FragCoord.y / 600.0f - 0.5f) * zoom + frame_y) * 5.0;
    // TODO: For julia set, you can change/vary these
    //float real = 0;
    //float imag = 0;
 
    int iterations = 0;
    float const_real = real;
    float const_imag = imag;
 
    while (iterations < MAX_ITERATIONS) {
        float temp_real = real;
        real = (real * real - imag * imag) + const_real;
        imag = (2.0f * temp_real * imag) + const_imag;
         
        float mag_sq = real * real + imag * imag;
         
        if (mag_sq > 4.0)
        break;
 
        ++iterations;
    }
    return iterations;
}
 
vec4 return_color() {
    int num_iterations = get_iterations();
    if (num_iterations == MAX_ITERATIONS) {
        gl_FragDepth = 0.0f;
        return vec4(0.0f, 0.0f, 0.0f, 1.0f);  // black default
    }
 
    float iterations = float(num_iterations) / MAX_ITERATIONS;  // scale on [0,1]
    return vec4(0.0f, iterations, 0.0f, 1.0f);
}
 
void main() {
    frag_color = return_color();
}