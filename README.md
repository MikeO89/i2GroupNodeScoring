@mainpage i2 Group Tech Test

# i2 Group Tech Test
Author: Mike Orr

## Specification

Welcome to our technical interview process! 

In this interview, we'd like to assess your skills in C++. We expect the exercise to take roughly 2-3 hours.

You'll be asked to submit some code for an application that tackles three specific questions using the data provided in _data.json_ in this repo.

### Question 1
    
For a given node in the graph, its **weighted degree** is the sum of the weights of all links connected to the node.

Write a program to compute the **weighted degree** of each node of the graph, and order the nodes by their weighted degree. The program should print its output to stdout in the following format
  
highest scoring node name:          score  
second highest scoring node name:   score  
.  
.  
.  
  
### Question 2
  
How do you think the method of question 1 performs as a definition of importance for the nodes in the graph? (What does it do well? What are its shortcomings?)
  
### Question 3
  
Based on your answer to question 2, and your own research, determine a better method of assigning an "importance score" to each node in the graph, and write a program to implement it (again printing its output to stdout, in the same format as question 1). Give a brief description of how the chosen method works, and if it has a name, say what it is.

### Evaluation Criteria
Your submission will be evaluated based on the following criteria:
* **Functionality:** Does your code accomplish the tasks specified in a functional manner?
* **Code Quality:** Is your code well-structured, readable, and maintainable? Are best practices followed?

### Additional Information
* All of the necessary data for the tasks can be found in _data.json_ in this repo, so you do not need to worry about data generation.
* You are free to use any libraries or tools you prefer to complete the tasks.
* Feel free to reach out to us if you have any questions or need clarification during the interview process.

We look forward to reviewing your submission and assessing your skills in C++ development. Good luck with your interview, and we appreciate your interest in joining our team!

# Solution
Author: Mike Orr

## Intro
Initially, I would like to thank you for the opportunity to complete the technical test: I thoroughly enjoyed completing the task, and I look forward to receiving your feedback.

CMake has been utilised for fast project setup and cross platform capabilities.
In CMakeLists.txt you will see that I have created three components to the solution:

1. i2Lib - a dynamic library containing the logic and definitions for building the virtual representation of the Nodes, linking them together, and computing the weighted degree, from a JSON input file.

2. i2TechTest - an executable that utilises the logic within i2Lib to load, sort, and display the weighted nodes in descending order.

3. i2UnitTest - an executable using the GTest framework to test i2Lib.

The main decision to abstract the logic to a library was made due to re-using the implementations between i2TechTest, i2UnitTest, and provide future-proofing for use in any future projects. The resultant executables are also smaller.

## Environment Setup

### VCPKG Setup (Windows)

VCPKG is the chosen package manager for this project.

1. Clone the vcpkg public repository and run the bootstrap script

```command
> git clone https://github.com/microsoft/vcpkg.git C:/vcpkg
> cd C:/vcpkg
> .\bootstrap-vcpkg.bat
```

2. Create system environment variable VCPKG_ROOT, set the value to 'C:\vcpkg', and add the VCPKG_ROOT variable to the system PATH variable.

### Doxygen Setup (Windows)

Doxygen is used for documentation generation.
Unforunately Doxygen is not currently available as a package with vcpkg, so you will need to manually [download and install Doxygen from the official website](https://www.doxygen.nl/download.html).
Add the Doxygen bin path ('C:\Program Files\doxygen\bin') to your system PATH variable, so Doxygen can be found (may be automatically added by the installer).

Doxygen uses a Graphviz component called 'dot', so [download and install Graphviz](https://graphviz.gitlab.io/download/) - be sure to select the install option to add GraphViz to the system PATH variable, otherwise add it manually.

## Question #1

> For a given node in the graph, its **weighted degree** is the sum of the weights of all links connected to the node.
>
> Write a program to compute the **weighted degree** of each node of the graph, and order the nodes by their weighted degree. The program should print its output to stdout in the following format
>  
> highest scoring node name:          score  
> second highest scoring node name:   score  
> .  
> .  
> .

**Building - Debug**

```command
> cd <project_root_dir>
> cmake -S . -B build --preset "x64-debug"
> cmake --build build --config Debug
```

**Building & Installing - Release**

```command
> cd <project_root_dir>
> cmake -S . -B build --preset "x64-release"
> cmake --build build --config Release
> cmake --install build --prefix <path_to_install>
```

**Running Tests (Post-build)**

```command
> cd build
> ctest
```

**Running Tech Test Solution (Post-install)**

```command
> cd <install_path>/bin
> i2TechTest.exe --process ../resources/data.json
```

To see the help menu simply do not pass arguments when running, or pass -h/--help.

**Application Output**

```command
Valjean: 158
Marius: 104
Enjolras: 91
Courfeyrac: 84
Cosette: 68
Combeferre: 68
Bossuet: 66
Thenardier: 61
Gavroche: 56
Javert: 47
Fantine: 47
Joly: 43
Bahorel: 39
Feuilly: 38
Mme.Thenardier: 34
Myriel: 31
Gillenormand: 29
Babet: 27
Tholomyes: 26
Favourite: 26
Dahlia: 25
Gueulemer: 25
Blacheville: 25
Fameuil: 24
Listolier: 24
Zephine: 24
Mlle.Gillenormand: 23
Claquesous: 20
Prouvaire: 19
Mme.Magloire: 19
Eponine: 19
Mlle.Baptistine: 17
Mabeuf: 16
Grantaire: 16
Fauchelevent: 14
Champmathieu: 14
Judge: 14
Brujon: 13
Montparnasse: 12
Bamatabois: 11
Brevet: 11
Chenildieu: 11
Cochepaille: 11
Simplice: 8
Mme.Hucheloup: 7
Woman2: 5
Lt.Gillenormand: 5
Anzelma: 5
Child2: 5
Child1: 5
MotherInnocent: 4
Toussaint: 4
Woman1: 3
MotherPlutarch: 3
Mme.Burgon: 3
Marguerite: 3
Pontmercy: 3
Perpetue: 3
Gribier: 2
Magnon: 2
Mme.Pontmercy: 2
Count: 2
BaronessT: 2
Jondrette: 1
Scaufflaire: 1
Mlle.Vaubois: 1
Champtercier: 1
Geborand: 1
CountessdeLo: 1
Mme.deR: 1
Napoleon: 1
Labarre: 1
Boulatruelle: 1
Isabeau: 1
Gervais: 1
OldMan: 1
Cravatte: 1
```
### Notes

1. To open the CMake project in Visual Studio: Start Visual Studio -> continue without code -> File -> Open -> CMake -> CMakeLists.txt
2. Doxygen documentation is generated during the build process and is installed to the install directory - see ../docs/html/index.html

## Question #2

> How do you think the method of question 1 performs as a definition of importance for the nodes in the graph? (What does it do well? What are its shortcomings?)

This is my first time weighing graph nodes, so I spent some time researching online to enable me to answer this question. The summary of my findings:

**Strengths**

1. Local Importance - Nodes with a high weighted degree have many strong connections, making them influential in their immediate neighbourhood.
2. Computational Efficiency - Calculating weighted degree is straightforward, requiring only a single pass through the node’s edges, making it efficient for large graphs.
3. Intuitive Interpretation - A high weighted degree suggests that a node has many strong interactions, which is useful in networks like social graphs (e.g., influence based on direct interactions).

**Shortcomings**

1. Ignores Global Influence: A node with a high weighted degree might not be truly important if it is only connected to a small, isolated cluster rather than playing a central role in the entire graph.
2. No Consideration of Connectivity to Important Nodes: A node connected to many weakly connected nodes may have a high weighted degree, but it might not be as important as a node connected to a few highly central nodes.
3. Fails in Certain Network Types: In scale-free networks (e.g., social networks, the internet), centrality measures like PageRank are often better indicators of true importance.
4. Does not Differentiate Between Bridge and Hub Nodes: Nodes that serve as "bridges" between communities (high betweenness centrality) might have a lower weighted degree but be crucial for overall network connectivity.

While weighted degree is a good local importance metric, it does not fully capture global influence. A better metric should account for not just the number and weight of connections, but also what the node is connected to.
  

## Question #3

> Based on your answer to question 2, and your own research, determine a better method of assigning an "importance score" to each node in the graph, and write a program to implement it (again printing its output to stdout, in the same format as question 1). Give a brief description of how the chosen method works, and if it has a name, say what it is.

A better method of assigning an importance score would be Google's PageRank formula as it caters for global importance rather than just direct neighbours.
PageRank works by recursively updating each node's rank. Each iteration the new rank is calculated based on the current calculations for all nodes - the ranking continues to run until the rank is smaller than a specified tolerance.
A dampening factor (constant) is used to prevent nodes with fewer links from being penalised too much.

Rather than writing a separate program to implement PageRank, I have integrated the ranking algorithm within I2::NodeLoader as standalone function computePageRank. Run the executable in the CLI for instructions on running the application - page ranking is not run by default, so to include run command ```> i2GroupTechTest.exe --process <path_to_data_json> --rank```

**Running PageRank ([See Question #1 for building/installing](#question-1))**

```command
> cd <install_path>/bin
> i2TechTest.exe --process ../resources/data.json --rank
```

**Application Output**

```command
Mlle.Baptistine: 1000.00
Myriel: 1000.00
Dahlia: 1000.00
Bamatabois: 1000.00
Bahorel: 1000.00
Tholomyes: 1000.00
Judge: 1000.00
Child2: 1000.00
Favourite: 1000.00
Courfeyrac: 1000.00
Cosette: 1000.00
Child1: 1000.00
Feuilly: 1000.00
Prouvaire: 1000.00
Javert: 1000.00
Fauchelevent: 1000.00
Zephine: 1000.00
Combeferre: 1000.00
Enjolras: 1000.00
Mme.Thenardier: 1000.00
Mabeuf: 1000.00
Thenardier: 1000.00
Chenildieu: 1000.00
Eponine: 1000.00
Gillenormand: 1000.00
Mme.Magloire: 1000.00
Blacheville: 1000.00
Mlle.Gillenormand: 1000.00
Fantine: 1000.00
Marius: 1000.00
Valjean: 1000.00
Fameuil: 1000.00
Brevet: 1000.00
Cochepaille: 1000.00
Gavroche: 1000.00
Listolier: 1000.00
Champmathieu: 1000.00
Claquesous: 1000.00
Gueulemer: 1000.00
Babet: 1000.00
Joly: 1000.00
Grantaire: 1000.00
Bossuet: 1000.00
Brujon: 952.29
Montparnasse: 868.28
MotherInnocent: 661.11
Lt.Gillenormand: 486.30
Mme.Hucheloup: 452.74
Simplice: 441.91
Gribier: 425.00
Anzelma: 338.07
Perpetue: 244.45
MotherPlutarch: 231.82
Toussaint: 228.16
Magnon: 198.70
Woman2: 198.11
Count: 170.00
Pontmercy: 169.93
Mme.Pontmercy: 169.58
BaronessT: 166.17
Marguerite: 136.95
Mlle.Vaubois: 121.43
Mme.Burgon: 120.96
Woman1: 97.22
CountessdeLo: 85.00
Geborand: 85.00
Cravatte: 85.00
Napoleon: 85.00
OldMan: 85.00
Champtercier: 85.00
Boulatruelle: 53.13
Jondrette: 51.41
Labarre: 23.61
Scaufflaire: 23.61
Gervais: 23.61
Isabeau: 23.61
Mme.deR: 23.61
```