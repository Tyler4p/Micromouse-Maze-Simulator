# Micromouse-Maze-Simulator
Graphical maze simulator created for IEEE at UCLA's Micromouse program

![MazeSimulator](https://user-images.githubusercontent.com/13712871/108049026-9ceab000-6ffc-11eb-9b62-fb95c4ec89df.png)

## Introduction
Micromice are essentially mini maze-solving robots. A large part of developing the intelligence powering a micromouse is creating smart maze solving algorithms. This maze simulator aims to aid in the development and testing of maze solving algorithms by providing a graphical visualization of these algorithms in action.

This repository contains two tools:
- Simulator: graphic visualizer for maze solving algorithms
- Editor: tool for creating and editing maze files for use by the simulator

There are additionally a handful of ready made maze files (available in the Mazes folder) for use in the simulator. Most of these are based off real maze layouts used in past Micromouse competitions.

## Installation
The maze simulator and editor are written in C and use the C-based SDL2 library for graphics. Since writing and debugging different maze solving algorithms inherently requires frequent compilation, it is recommended that the maze simulator be used on a system that enables easy C-compiling and execution of graphical applications (aka not Windows). Since the simulator and editor were developed on a Linux system, this section will cover how to either get set up if you are already on a Linux-based machine or how to set up a Linux virtual machine for development.

### Method 1: Linux
If you are already on a Linux-based system that can run graphical applications, setup is quite simple. The first step is to install the required SDL2 libraries by running the following in the terminal:

```
sudo apt-get install libsdl2-dev
sudo apt-get install libsdl2-image-dev
'''


