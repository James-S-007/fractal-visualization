# C++ Fractal Visualization with OpenGL and SFML

## Dependencies
| Package    | Install Cmd                       |
| ---------- | --------------------------------- |
| SFML 2.5.1 | sudo apt-get install libsfml-dev  |
| GLEW 2.1.0 | sudo apt-get install libglew-dev  |
| GLUT 2.8   | sudo apt-get install freeglut3-dev|
| OpenGL 3.3 | update graphics card drivers      |

## Build
```bash
mkdir build && cd build
cmake ..
cmake --build .
./Fractal_Visualization
```

## Navigating

### Fractal Select Menu

| Input | Function |
| ----- | -------- |
| 1 | Mandelbrot fractal with shaders |
| 2 | Julia fractal with shaders |
| 3 | Mandelbrot fractal with OpenMP |

### Fractal Visualizer

| Input | Function |
| ----- | -------- |
| Mouse Scroll Wheel | Zoom |
| Left Mouse Click Drag | Pan |
| r | Reset zoom and frame to origin |
| esc | Go to fractal select menu |
