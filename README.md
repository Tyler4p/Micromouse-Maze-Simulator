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
The maze simulator and editor are written in C and use the C-based SDL2 library for graphics. Since writing and debugging different maze solving algorithms inherently requires frequent compilation, it is recommended that the maze simulator be used on a system that enables easy C-compiling and execution of graphical applications (aka not Windows). Since the simulator and editor were developed on a Linux system, this section will cover how to either get set up if you are already on a Linux-based machine or how to set up a Linux virtual machine for development. If you have an alternate way to run the maze simulator that is perfectly fine, just be wary it is currently untested on other platforms.

### Method 1: Linux
If you are already on a Linux-based system that can run graphical applications, setup is quite simple. The first step is to install the required SDL2 libraries by running the following in the terminal:

```
sudo apt-get install libsdl2-dev
sudo apt-get install libsdl2-image-dev
```

The only remaining step is to download the actual project files! You can either download and extract them manually, or you can navigate to the location you would like to keep the maze simulator files and run the following command:

```
git clone https://github.com/Tyler4p/Micromouse-Maze-Simulator.git
```

### Method 2: Virtual Machine
If you do not already have a Linux-based system, virtual machines make it very easy to run an emulation of one. Furthermore, depending on which virtual machine software you are using, whole preconfigured machine/OS snapshots can easily be imported.

For this guide we will be using VirtualBox. Other virtual machine programs should be fine, though keep in mind not all virtual machine software support importing .ova files, which make the rest of the install very easy. The first step is to [download VirtualBox](https://www.virtualbox.org/) and install it for your system.

Once VirtualBox is installed, there are two options: manually install and configure Ubuntu (or another Linux-based OS), or import a virtualization file which will automatically install Ubuntu with all the required libraries installed. 

### Method 3: MacOS
If you have homebrew installed, it is easy to install the required SDL2 libraries via the terminal:
```
brew install sdl2
brew install sdl2_image
```
If you don't have homebrew installed, install it from [this link] (https://brew.sh)

Once these libraries are installed, download the project files just like for Linux systems.
s
#### Import Virtual Machine (Recommended)
For the later option, simply [download this .ova file](https://drive.google.com/file/d/1RxYdXluyUCeAe3fLh7dhtgh74nuaI9yY/view?usp=sharing) and run it--doing so will launch VirtualBox and you will be prompted to import the virtualization file. Once imported, you should see MM_Ubuntu in the list of virtual machines. Select it and click Settings > Shared Folders and click the add shared folder button. In the window that pops up, click the dropdown for the folder path field and click other. This will allow you to select a folder on your machine that will be mounted to the virtual machine, which will make it easy to edit your maze simulator code with your editor of choice running on your main OS. Be sure to select the Auto-mount option too.

Once you've set up a shared folder, you can now start the virtual machine. Once it boots up, open the file manager--here you should see a mounted drive named according to the name of the shared folder you specified. Enter it and navigate to a location where you want to keep the maze simulator development files, and then run the git clone command mentioned under Method 2. Alternatively, you can download the files in this repository as a zip folder and unzip it in the desired location.

#### Manual Setup
If you so chose, or you are unable to get the .ova import working, you can manually set up and configure a Linux virtual machine. [This tutorial](https://www.youtube.com/watch?v=x5MhydijWmc) is very good and goes through setting up an Ubunutu virtual machine in VirtualBox. Once you have the OS operational, the guest additions added, and a shared folder set up, you can follow the steps described in Method 1 to install the SDL2 libraries and project files to your virtual machine. Again, it is recommended that you clone the github repository somewhere in the shared folder so both the virtual machine and your main OS can access and edit the files.

## Using the Simulator
Once everything has been set up, using the simulator is easy! First, navigate to the Simulation folder which contains the simulation source files. Right click within the folder and select open with terminal (if you are on Ubuntu). You can also manually navigate to the folder location within your terminal. Use the following command to run the simulator:

```
./simulation <filename.txt>
```

The file name argument is used to specify which maze the simulator should load. All mazes are assumed to be in the Mazes folder adjacent to the Simulator folder. For example, to load the maze contained in the aamc_2015.txt file, I would run
```
./simulation aamc_2015.txt
```
If the filename field is left blank, the default 4x4 maze will be loaded.

The following can be used to control the simulation:
- s: start/pause the simulation
- d: toggle darkmode
- i: display credits
- esc: exit the program

#### Notes
We are still working out a few kinks with the GUI, so there are a few bugs. To avoid some errors, avoid resizing the window while the simulation is running.

### Creating Maze Solving Algorithms
Aside from possibly tweaking some settings variables in main.c, the intention is that none of the files aside from solver.c and solver.h should be modified. The rest of the files are driver code for the simulator--the actual maze traversal logic gets implemented in solver.c. Each maze solving algorithm function should return the next move the mouse should take when called, which can either be FORWARD, LEFT, RIGHT, or IDLE. The only functions that should be used by the mouse are getLeftReading, getFrontReading, and getRightReading, which represent the wall detecting sensors on a physical micromouse.

To create a new maze solving algorithm function, declare it in solver.h and then implement it in solver.c. Additionally, you will need to make sure that solver(), a function forwarding function at the top of solver.c, calls your new maze solving function.

Every time you make changes to any of the source files, you will need to recompile the simulator. To do so, run the following commands through the terminal from inside the Simulator folder:
```
make clean
make
```
This will clean out the old executable file and compile a new one. Once you have recompiled, you can run the new executable exactly as described earlier.

### Using the Editor
1) Enter a file name for the maze file. If a file with the same name exists, it will attempt to open that file. If no such file exists, then a file will be created. Note: it is recommended to add the ".txt" extension to your file name.
2) Enter a name for the maze.
3) Enter a maze size. This must be an integer between 2 and 16.

Now the GUI should open up, and you are free to click on wall locations to toggle whether or not a wall is present there.

To save your work, press "s" on your keyboard.
