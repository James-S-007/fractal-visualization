# C++ Fractal Visualization with OpenGL and SFML

## Dependencies
| Package    | Install Cmd                      |
| ---------- | -------------------------------- |
| SFML 2.5.1 | sudo apt-get install libsfml-dev |
| GLEW 2.1.0 | sudo apt-get install libglew-dev |
| OpenGL 3.3 | update graphics card drivers     |

## Build
```bash
mkdir build && cd build
cmake ..
cmake --build .
./Fractal_Visualization
```

## Navigating
| Input | Function |
| ----- | -------- |
| Mouse Scroll Wheel | Zoom |
| Left Mouse Click Drag | Pan |
| r | Reset zoom and frame to origin |
| q | Quit program |
