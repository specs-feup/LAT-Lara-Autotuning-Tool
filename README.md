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
 - `searchType` - Specify how to search for the best match with the given variables. The possibilities are _dependent_ and _independent_ or _custom_. _Dependent_ search will execute every combination of the variables whereas _independent_ search will execute each variable's variation while keeping every other variable to their default. See [Global constants](#global-constants) for more info. _Custom_ search will happen if you specify a value for the **searchGroups** variable;
 - `searchGroups` - A list of variable lists that will specify which variables to tune dependently. From 3 variables: `a`,`b` and `c`: `[a,b]` and `[a,c]` means that variables `a` and `b` will be tuned _dependently_ while keeping the value of `c`, and afterwars `a` and `c` will be tuned _dependently_ while keeping the value of `b`. In this example, `[a]`,`[b]`,`[c]` would be the same as _independent_ search and `[a,b,c]` would the same as _dependent_ search;
 - `numTests` - How many tests to perform for each variant;
 - `removeDuplicates` - If true, every variant generated will be tested to assert if it is unique, for large amounts of variants this becomes an expensive process as its time complexity is O^n, if you belive the number of duplicate variants is small and that the cost of creating and executing those variants is smaller than that of removing those duplicates then you should set it to false, its default value is true;
 - `clearCode` - Define whether the generated code variants will be deleted or not;
 - `verbose` - Turn verbose output on and off;
 - `abort` - Define if the tool will stop upon error from one of the variants;
 - `timeout` - Define the maximum amount of time the execution of the tool can take before stopping, in seconds. 0 means there is no timeout;
 - `outputPath` - The directory where the output code and results appear. The generated code will appear in that folder under the folder `/output` and the results under the folder `/results`. Inside the `/output` folder there will be a folder for each variant source code named accordingly to its index in execution: the first variant will be in the folder `/output/1` and so forth.

###### Variables, searchGroups and searchType behaviour
If the `searchType` is set to either _dependent_ or _independent_, the `searchGroups` will be created accordingly with the supplied variables; the content of `searchGroups` will be cleared immediately, but the groups generation is done when `tune()` is called, since more variables can be added in the meantime. 

If any `searchGroup` is supplied, the value of `searchType` will be set to _custom_ and when `tune()` is called those groups are the ones used for the variants generation. 

Essentially, either you choose _dependent_ or _independent_ search or you specify _custom_ `searchGroups`. You can do any action as many times as desired before calling `tune()` as only the last will be considered.

##### Defaults

|      property     |   default   |        accepted        |
|:-----------------:|:-----------:|:----------------------:|
|       `name`      |   required  |         string         |
|      `scope`      |   required  |        joinpoint       |
|     `measure`     |    scope    |        joinpoint       |
|    `variables`    |      []     |         LatVar         |
|   `searchGroups`  |      []     |     lists of Latvar    |
|    `searchType`   | independent |  LatConst.SEARCH_TYPE  |
|     `numTests`    |      1      |      unsigned int      |
| `removeDuplicate` |     true    |         boolean        |
|    `clearCode`    |     true    |         boolean        |
|     `verbose`     |     true    |         boolean        |
|      `abort`      |    false    |         boolean        |
|     `timeout`     |      0      |      unsigned int      |
|    `outputPath`   |  curent dir | absolute/relative path |

##### Methods

As for the **Lat** constructors you can specify as many parameters as you want, although you will only be able to invoke the other methods after `name` and `scope` are define, here are some common constructor forms:

 - `Lat()` - Empty constructor, this creates an empty **Lat** that cannot be used until the required parameters are set: `name` and `scope`;
 - `Lat(name, scope)` - Constructor, with the required parameters only;
 - `Lat(name, scope, variables, measure, searchType, numTests, removeDuplicates, clearCode, verbose, abort, timeout)` - Constructor, apart from the first two parameters (`name` and `scope`) all the other variables are optional, but must be supplied in this order;
 - `tune()` - Start the tuning of the code;
 - `toConfig()` - Save the current properties of the **Lat** instance to a `.json` file for future reuse, including the variables inside it;
 - `fromConfig(filename)` - Load a `.json` file into a Lat instance;
 - `countVariants()` - Returns an int with the number of variants the current configuration will execute, this is the value of running all possible combinations and duplicate instances may be accounted for;
 - `estimateTime()` - This function builds and executes the default src with the values and then returns the estimated amount of time in seconds for all the variants; 
 - `printResults()` - Prints the results after a `tune` tuning has happened; 
 - `showGraphs()` - Displays the results in user-friendly charts, these are automatically generated and sabed under the `outputPath/results`. 
 
 #### Setters
 Return a reference to the object so you can have chained calls:

 - `setName(name)`
 - `setScope(scope)`
 - `setMeasure(measure)`
 - `setVariables(variables)`
 - `addVariable(LatVar)`
 - `setSearchGroups(searchGroups)`
 - `addSearchGroup(searchGroup)` - a search group is a list of one or more LatVar variables that 
 - `setSearchType(searchType)` - only `LatConst.SEARCH_TYPE.INDEPENDENT` and `LatConst.SEARCH_TYPE.DEPENDENT` accepted. If you change this after you have set any `searchGroups`, the searchGroup specifications will be reset accordingly (see [properties](#properties)). If you have set any searchGroups, the value of searchType will be `LatConst.SEARCH_TYPE.CUSTOM`;
 - `setNumTests(numTests)`
 - `setRemoveDuplicates(removeDuplicates)`
 - `setClearCode(clearCode)`
 - `setVerbose(verbose)`
 - `setAbort(abort)`
 - `setTimeout(timeout)`
 - `setOutputPath(outputPath)`

---

### **LatVar** 
The parent class for representing a named variable. This works like a Java Interface or a C++ Virtual Class. 

##### Properties
 - `name` - the variable name in the scope specified, it must be unique.
 - `type` - [optional] the type of the variable in the code, this will be used when changing the ast so that there is no ambiguity when generating the variants. For instance, a char variable is not printed the same way a string is. Allowed values: `LatConst.OUTPUT_TYPE.`(`AUTO`, `STRING`, `CHAR`), see [Global constants](#global-constants). Default is `LatConst.OUTPUT_TYPE.AUTO`.

##### Methods
 - `getNext()` - private method used for obtaining the next value for the variable, it returns an object containing some useful information, which includes the next value for the variable, for example `{ type: LatConst.OUTPUT_TYPE.AUTO, value: 0, default: false }`;
 - `hasNext()` - private method used for checking if there are still elements to use;
 - `countElements()` - private method used to get the number of elements each LatVar produces;
 - `restart()` - private method that restarts the variable to its first element;
 - `setType(type)` - setter for type, it validates the supplied value, must be one of `OUTPUT_TYPE`, see [Global constants](#global-constants).

It is not to be invoked on its own, choose one of the following **accessible** sub-classes, that inherit its properties and methods:

---

### LatVarList
Used to define a variable that varies withtin a the values of a list;

##### Properties
 - `elements` - a list of values that this variable can assume. The default is an empty list;
 - `elementIndex` - The position of the next element to return, if `elementIndex > elements.size`. The default is 0.   

##### Methods
 - `LatVarList(name, elements, type)` - Constructor, elements is optional and can be changed through the property. 
    
---
    
### LatVarRange
Used to define a variable that has a starting value, a finish value, a step incremention and optionally a function to be called to get the next element from the previous. Apart from the `function` property it works like a `for` loop. 

##### Properties
 - `next` - The next value to be returned, this starts equal to `start` and goes until it becomes greater than `stop`, or smaller, if the step is negative; 
 - `start` - The initial value for the variable, **inclusive**; 
 - `stop` - The last value for the variable, **exclsuive**;
 - `step` - The increment to the current value to obtain the next. Default is 1;
 - `callback` - An optional function that generates the value of `next` and that receives the next element in the list. This is useful for mathematical expressions, returning non-numeric values, ...

##### Methods
 - `LatVarRange(name, start, stop, step, callback, type)` - Constructor, `step` and `callback` are optional.  

---
### LatVarOmp
Used to control the flow of an OpenMP variable or variables

**To be defined**


##### Properties
 - ` ` - 

##### Methods
 - ` ` - 

## System Specific Variables

Since the same code can have different behaviours according to the phisical system it is running on, there are a few variables that can be invoked in `Lat` that will be system dependent, such as: 

1. NUM_CPU_THREADS - Total number of hardware threads on the CPU (`LatConst.NUM_CPU_THREADS`)
2. L1_CACHE_LINESIZE - L1 cache line size in bytes (`LatConst.L1_CACHE_LINESIZE`)


## Global Constants

There are a few values that Lat requires that are grouped in the **LatConst** class, namely:

1. SEARCH_TYPE - An enum used to specify the search, values: 
   - `LatConst.SEARCH_TYPE.INDEPENDENT`
   - `LatConst.SEARCH_TYPE.DEPENDENT`
   - `LatConst.SEARCH_TYPE.CUSTOM`
2. OUTPUT_TYPE - An enum used to specify how to print the variable when generatin variants:
   - `LatConst.OUTPUT_TYPE.AUTO` - will detect int, string, omp, ...
   - `LatConst.OUTPUT_TYPE.CHAR` - force the output to be 'a' instead of "a"
   - `LatConst.OUTPUT_TYPE.STRING` - force the output to be "1" instead of 1


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



## Ideas

 1. Stop after a given user condition is met, example: enough variations have been run and there is one that is x% above average or another measure
 2. In LatVarList enable random access to the variables, if 1. is implemented as it does not make sense to use this when there is no ending condition
 3. How to support chars, does it need to insert the '' ?
 4. Identify irrelevant search Groups, example: `[[a, b], [a]]` - on the first search group, `a` will be run with the default value of `b` on the first iteration of this search group, meaning that the second search group, with `a` alone, will execute the same iteration (varying `a` and keeping all the other variables, in this case `b`)
 5. Create an index
 6. Create examples