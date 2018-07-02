# LAT - LARA Autotuning Tool 
### (Ongoing project)

## What is Lat

**Lat** is a tool for code autotuning, built using [LARA](https://web.fe.up.pt/~specs/projects/lara/doku.php) (a Javascript based language that supports source code transformations) and applied using the [Clava](https://github.com/specs-feup/clava/tree/master/ClavaWeaver) tool . It is designed for C/C++. With it, coders may test multilple **variants** of their code dynamically and then choose the best variant to ship their code. 

**Lat** mimics [Intel Software Autotuning Tool (ISAT)](https://software.intel.com/en-us/articles/intel-software-autotuning-tool) behaviour for testing multiple instances of the same code. However, it is build entirely on LARA. Because of this it is platform agnostic and can be more flexible and easier to expand, both in functionalities and languages (not only C and C++).

### For details and instructions visit the :notebook: [Lat Wiki](https://github.com/specs-feup/LAT-Lara-Autotuning-Tool/wiki) 

### Folder Structure:

 - :file_folder: [aspects](https://github.com/specs-feup/LAT-Lara-Autotuning-Tool/tree/master/aspects) - contains LARA aspects that Lat uses to manipulate the source code AST; 
 - :file_folder: [examples](https://github.com/specs-feup/LAT-Lara-Autotuning-Tool/tree/master/examples) - a list of [ISAT examples](https://software.intel.com/en-us/articles/download-intel-software-autotuning-tool) implemented with LAT; 
 - :file_folder: [lat](https://github.com/specs-feup/LAT-Lara-Autotuning-Tool/tree/master/lat) - the source code of Lat, implementation of all the Javascript classes and core functions;
 - :file_folder: [templates](https://github.com/specs-feup/LAT-Lara-Autotuning-Tool/tree/master/templates) - html, css and js (extensionless) files that Lat uses to produce the LatReport;
 - :file_folder: [tests](https://github.com/specs-feup/LAT-Lara-Autotuning-Tool/tree/master/tests) - LARA aspect files that contain some tests for lat methods and properties
 
### [Presentation on Lat](https://docs.google.com/presentation/d/16jLotjWUA6WJFUu2lA5GW7NaVbV6wX7JJwPlvxN6Czc/edit?usp=sharing)
