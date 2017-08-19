# LAT - LARA Autotuning Tool

## Context

**LAT** is a tool for code autotuning, built using LARA (an AOP language). It is designed for C/C++. With it, coders may test multilple **variants** of their code dynamically and then choose the best variant to ship their code. (Maybe add some more information about clava and links to these mentions)

## Classes

### **Lat**
The main class for the tool, this will represent a region to autotune and all the associated configurations and hold all the variables and methods to invoke.

##### Properties

 - `name` - the name of the region to be tuned (useful for debug and results section);
 - `scope` - a **joinpoint** defining where the variables to be tuned are and where their values will be tuned;
 - `measure` - a **joinpoint** that specifies the region for the timing to happen, if none is specified then the scope will be used;
 - `variables` - a list of **LatVar** that specifies the actual variables in the code and the values they will assume;
 - `searchType` - Specify how to search for the best match with the given variables. The possibilities are _dependent_ and _independent_ or _custom_. _Dependent_ search will execute every combination of the variables whereas _independent_ search will execute each variable's variation while keeping every other variable to their default. _Custom_ search will happen if you specify a value for the **searchGroups** variable;
 - `searchGroups` - A list of variable lists that will specify which variables to tune dependently. From 3 variables: `a`,`b` and `c`: `[a,b]` and `[a,c]` means that variables `a` and `b` will be tuned _dependently_ while keeping the value of `c`, and afterwars `a` and `c` will be tuned _dependently_ while keeping the value of `b`. In this example `[a]`,`[b]`,`[c]` would be the same as _independent_ search and `[a,b,c]` would the same as _dependent_ search;
 - `numTests` - How many tests to perform for each variant;
 - `compiler` - (To be defined whether this will be mentioned, since it is automatically detected);
 - `clearCode` - Define whether the generated code variants will be deleted or not;
 - `verbose` - Turn verbose output on and off;
 - `abort` - Define if the tool will stop upon error from one of the variants;
 - `timeout` - Define the maximum amount of time the execution of the tool can take before stopping, in seconds. 0 means there is no timeout.

##### Defaults

|    property    |   default   |        accepted        |
|:--------------:|:-----------:|:----------------------:|
|     `name`     |   required  |                        |
|     `scope`    |   required  |        joinpoint       |
|    `measure`   |    scope    |        joinpoint       |
|   `variables`  |      []     |         LatVar         |
| `searchGroups` |      []     |     lists of Latvar    |
|  `searchType`  | independent | dependent, independent |
|   `numTests`   |      1      |      unsigned int      |
|   `compiler`   |     auto    |          none          |
|   `clearCode`  |     true    |         boolean        |
|    `verbose`   |     true    |         boolean        |
|     `abort`    |    false    |         boolean        |
|    `timeout`   |      0      |      unsigned int      |

##### Methods

 - `Lat(name, scope)` - Constructor, with the required parameters only;
 - `Lat(name, scope, variables, measure, searchType, numTests, clearCode, verbose, abort, timeout)` - Constructor, apart from the first two parameters (`name` and `scope`) all the other variables are optional, but must be supplied in this order;
 - `tune()` - Start the tuning of the code;
 - `toConfig()` - Save the current properties of the **Lat** instance to a `.json` file for future reuse, including the variables inside it;
 - `fromConfig(filename)` - Load a `.json` file into a Lat instance;
 - `countVariants()` - Returns an int with the number of variants the current configuration will execute;
 - `estimateTime()` - This function builds and executes the default src with the values and then returns the estimated amount of time in seconds for all the variants. 


### **LatVar** 
The parent class for representing a named variable. This works like a Java Interface or a C++ Virtual Class. 

##### Properties
 - `name` - the variable name in the scope specified, it must be unique.

##### Methods
 - `getNext()` - private method used for obtaining the next value for the variable
 - `hasNext()` - private method used for checking if there are still elements to use.
 - `restart()` - private method that restarts the variable to its first element

It is not to be invoked on its own, choose one of the following **accessible** sub-classes, that inherit its properties and methods:

### LatVarList
Used to define a variable that varies withtin a the values of a list;

##### Properties
 - `elements` - a list of values that this variable can assume. The default is an empty list;
 - `elementIndex` - The position of the next element to return, if `elementIndex > elements.size`. The default is 0.   

##### Methods
 - `LatVarList(name, elements)` - Constructor, elements is optional and can be changed through the property. 
    
### LatVarRange
Used to define a variable that has a starting value, a finish value, a step incremention and optionally a function to be called to get the next element from the previous. Apart from the `function` property it works like a `for` loop. 

##### Properties
 - `next` - The next value to be returned, this starts equal to `start` and goes until it becomes greater than `end` or smaller if the step is negative; 
 - `start` - The initial value for the variable; 
 - `end` - The last value for the variable;
 - `step` - The increment to the current value to obtain the next. Default is 1;
 - `function` - An optional function that receives the value of `next` and can generate another value from that. This is useful for mathematical expressions, returning non-numeric values, ...

##### Methods
 - `LatVarRange(start, end, step, function)` - Constructor, `step` and `function` are optional.  

---
### LatVarOmp
Used to control the flow of an OpenMP variable or variables

**To be defined**


##### Properties
 - ` ` - 

##### Methods
 - ` ` - 
---

## System Specific Variables

Since the same code can have different behaviours according to the phisical system it is running on, there are a few variables that can be invoked in `Lat` that will be system dependent, such as: 

1. NUM_CPU_THREADS - Total number of hardware threads on the CPU (`LatConst.NUM_CPU_THREADS`)
2. L1_CACHE_LINESIZE - L1 cache line size in bytes (`LatConst.L1_CACHE_LINESIZE`)


## Global Constants

There are a few values that Lat requires that are grouped in the **LatConst** class, namely:

1. SEARCH_TYPE - Ane enum used to specify the search type (`LatConst.SEARCH_TYPE.INDEPENDENT`, `LatConst.SEARCH_TYPE.DEPENDENT`)


## Examples

## Instructions

## Design

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



