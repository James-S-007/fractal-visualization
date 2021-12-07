/* 
Author: James Springer
Class: ECE 4122
Last Date Modified: 12/07/21 
 
Description: OpenGL fragment shader for Julia set
*/

#version 330 core
in vec4 gl_FragCoord;

out vec4 frag_color;

uniform float zoom;
uniform float frame_x;
uniform float frame_y;
uniform int width;   // width of window
uniform int height;  // height of window

#define MAX_ITERATIONS 1000
#define MAX_MAG 4.0


int calcIterations()
{
    float x = ((gl_FragCoord.x / float(width) - 0.5f) * zoom + frame_x) * 5.0;
    float y = ((gl_FragCoord.y / float(height) - 0.5f) * zoom + frame_y) * 5.0;
 
    int iterations = 0;
    float xc = .355534;
    float yc = -0.337292;
 
    while (iterations < MAX_ITERATIONS)
    {
        float x_temp = x;
        x = (x * x - y * y) + xc;
        y = (2.0f * x_temp * y) + yc;
         
        float mag_sq = x * x + y * y;
         
        if (mag_sq > MAX_MAG)
        {
            return iterations;
        }
 
        ++iterations;
    }

    return iterations;
}
 
vec4 calcColor(int iterations)
{
    if (iterations == MAX_ITERATIONS)
    {
        gl_FragDepth = 0.0f;
        return vec4(0.0f, 0.0f, 0.0f, 1.0f);  // black default
    }
 
    float color_scale = float(iterations) / MAX_ITERATIONS;  // scale on [0,1]
    return vec4(color_scale, 0.0f, 0.0f, 1.0f);
}
 
void main()
{
    int iterations = calcIterations();
    frag_color = calcColor(iterations);
}
