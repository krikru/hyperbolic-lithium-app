# lithium-app

This is an example application using the lithium game engine.

<img src="screenshot.png" alt="screenshot" width="400"/>

## Build
* Clone this repository. ```git clone https://github.com/filipfur/lithium-app.git```
* Create a new directory ```dep``` and clone lithium (https://github.com/filipfur/lithium.git).
* Clone the lithium dependencies by moving to directory ```dep/lithium/dep``` and running ```clonedependencies.bat```. (Make sure that you do this from the correct directory.)
* Install Visual Studio with C++ build tools (MSVC compiler).
* Install Visual Studio Code and extensions for C++ and CMake.
* Use VS or Code to compile the source code using the top-level CMakeLists.txt.

Note that some steps are platform dependent and differ if running on a unix system. The code itself is largely platform independent however, so it's completely feasible to build the software on those machines aswell.