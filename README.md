# LAT - LARA Autotuning Tool
# See [Lat Wiki](https://github.com/specs-feup/LAT-Lara-Autotuning-Tool/wiki)
## Context

**LAT** is a tool for code autotuning, built using LARA (an AOP language). It is designed for C/C++. With it, coders may test multilple **variants** of their code dynamically and then choose the best variant to ship their code. (Maybe add some more information about clava and links to these mentions)
## What is Lat

**Lat** is a tool for code autotuning, built using [LARA](https://web.fe.up.pt/~specs/projects/lara/doku.php) (a Javascript based language that support source code transformations) and applied using the [Clava](https://github.com/specs-feup/clava/tree/master/ClavaWeaver) tool . It is designed for C/C++. With it, coders may test multilple **variants** of their code dynamically and then choose the best variant to ship their code. 

**Lat** mimics [Intel Software Autotuning Tool (ISAT)](https://software.intel.com/en-us/articles/intel-software-autotuning-tool) behaviour for testing multiple instances of the same code. However, it is build entirely on LARA. Because of this it is platform agnostic and can be more flexible and easier to expand, both in functionalities and languages (not only C and C++).

## How to begin
Start by looking at our [Examples](https://github.com/specs-feup/LAT-Lara-Autotuning-Tool/wiki/Examples), once you get a grip of how Lat works, you can read the [Setup Guide](https://github.com/specs-feup/LAT-Lara-Autotuning-Tool/wiki/Setup) and then check the [API](https://github.com/specs-feup/LAT-Lara-Autotuning-Tool/wiki/Lat) reference. 

## Design Flow

1. The user specifies the Lat variable and its configurations;
2. The user runs the autotuning tool with `latInstance.tune();`
3. For each variant points 4 to 8 are executed:
4. Insert the new variables value accordingly into the code;
5. Insert the timer instructions in the code;
6. Compile and execute the code;
7. Read the time output from the execution;
8. Save the values to the results file;
9. Clear the generated code (if this config is enabled);
10. Display the results.
