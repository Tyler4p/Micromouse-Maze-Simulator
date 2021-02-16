# Mazes
The files in this folder contain mazes that can be loaded by the maze simulator and maze editor. Many of them are based on real maze layouts from past Micromouse competitions. 

## File Format
Each maze file has the following format:

- Top line: name of maze (can be at most 64 characters long)
- Second line: size of maze (refers to side width in number of cells, must be between 2 and 16)
- Remainder of file: maze wall data

Though currently a bit buggy, the idea with the maze data is that it physically resembles the maze using x's where there should be walls, including the border walls, and blank spaces where there should be empty space in the maze. There are 2n+1 lines of maze wall data, where n is the size of the maze.
