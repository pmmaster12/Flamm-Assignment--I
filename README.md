# AUTHOR : BT21CSE059 TARUN GUPTA 
# Android Assignment Set 1


## Questions Overview

### Q1: LRU Cache Implementation
A Least Recently Used (LRU) cache implementation in C++ that provides O(1) time complexity for both get and put operations.

Key features:
- O(1) time complexity for all operations
- Uses a combination of hash map and doubly linked list
- Handles cache eviction based on least recently used policy
- Thread-safe implementation

Build and run:
```bash
g++ -std=c++17 Q1.cpp -o lru_cache
./lru_cache
```

### Q2: Custom HashMap Implementation
A simplified version of the HashMap data structure with basic collision handling.

Features:
- Dynamic resizing
- Basic collision resolution using chaining
- Generic key-value pair support
- Load factor monitoring

Build and run:
```bash
g++ -std=c++17 Q2.cpp -o hashmap
./hashmap
```

### Q4: Solar System Visualization
A 3D visualization of our solar system using OpenGL, featuring realistic orbital mechanics and lighting effects.

#### Features
- Realistic 3D rendering of celestial bodies
- Dynamic lighting and shading
- Interactive camera controls
- Smooth orbital animations
- Custom shader effects for the sun and planets

#### Dependencies
- OpenGL 3.3+
- GLFW3
- GLEW
- GLM

#### Building the Project
1. Install dependencies:
   ```bash
   # Ubuntu/Debian
   sudo apt-get install build-essential libgl1-mesa-dev libglu1-mesa-dev libglfw3-dev libglew-dev
   # Build
   g++ -o solar_system main.cpp -lGL -lGLU -lglfw -lGLEW -std=c++17
   # Run
   ./solar_system





#### Controls
- Left Mouse Button + Drag: Rotate camera
- Mouse Wheel: Zoom in/out
- ESC: Exit application

#### Implementation Details
The solar system visualization uses modern OpenGL techniques:
- Vertex and fragment shaders for rendering
- Phong lighting model for realistic materials
- Perspective projection for 3D depth
- Efficient sphere mesh generation
- Proper memory management for OpenGL resources

## Project Structure
```
.
├── Q1.cpp              # LRU Cache implementation
├── Q2.cpp              # HashMap implementation
├── Q4/                 # Solar System visualization
│   ├── main.cpp        # Main OpenGL application 
└── README.md          # This file
```

## Development Notes
- All C++ code follows modern C++17 standards
- OpenGL code uses core profile (3.3+)
- Memory management follows RAII principles
- Error handling is implemented throughout

## Contributing
Feel free to submit issues and enhancement requests!

