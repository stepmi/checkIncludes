# checkIncludes
A tool to check if #includes in C++ source files are required to compile the files

## What is it

checkIncludes is a command line tool that checks a given C++ project for #includes that are no longer needed.<br> 
Throughout the life of a source file, the #include section often becomes bigger, especially with refactoring - checkIncludes addresses this problem.

## How does it work

checkIncludes relies on the build chain for the project. For every compile-file (such as cpp, cxx, c, etc) in the project it iterates and removes all #includes and checks if the file builds without the #include. If it does build without errors, the user is informed that the #include may be removed. The program doesn't modify any files, instead it works with temporary copies of the files.

## Caveat

Just because a certain compile-file builds without errors, it may not create the expected results, e.g. export information may be lost, linking may fail, etc. So the results of checkIncludes have to be seen as mere hints, to check if this particular #include is still needed.

Since checkIncludes tries to build each compile-file multiple times, it is very slow although it uses a multithreaded approach.

The usefulness of the results depends a lot on the #include structure. This is especially important for projects that use precompiled headers. checkIncludes assumes, that all compile-files build without including the precompiled header file. That means each compile-file is expected to include everything it uses. If this isn't the case, the tool can be used, but the results are not very meaningful.

Although checkIncludes can be build cross-platform, it only works with MSBuild for now.

## How to build

Use the attached Visual Studio Solution or create your own project with CMake. checkIncludes doesn't rely on any external libraries. You just need a compiler that supports C++17.

## How to run

Just call checkIncludes from your terminal and supply a path to the project file as parameter.<br>
checkIncludes has to find the build tool, so for MSBuild that means, you have to add the path to MSBuild.exe to your Windows path variable.


### Command line options

Option | Explanation
-------| -----------
-h     | print help text
-p:X   | platform for build process, where X is x64, x86, etc
-c:X   | configuration for build process, where X is Debug, Release, etc
-r     | files require precompiled header file to compile. Results will not be very meaningful when using this option.
-log   | show compiler log
-o:X   | check only this file, where X is a c/cpp file. Files have to be specified exactly as in the project file.
-i:X   | ignore file, where X may be a c/cpp file or a header<br>if X is a c/cpp file it is not checked<br>if X is a header, it isn't checked if this header can be removed			

### examples:

> checkIncludes  ../checkIncludes/checkIncludes.vcxproj

Checks all files of the project with default platform and configuration.

> checkIncludes -p:x64 -c:Release -i:src\tools\filesystem.cpp -i:string ../checkIncludes/checkIncludes.vcxproj

Checks all files of the project, except tools/filesystem.cpp. Use platform x64 and configuration Release. #include "string" is not tested at all.

> checkIncludes -o:src\tools\filesystem.cpp -o:src\tools\strings.cpp ../checkIncludes/checkIncludes.vcxproj

Checks only tools/filesystem.cpp and tools/strings.cpp.


## Development

checkIncludes has been tested with a variety of MsBuild projects. It is still in alpha-state. But it will not modify the original files in any way, so it can be used without safety concerns.<br>

### Next steps

* Only test #includes that are used with the current compile-process at all. Ignore #includes that are disabled by the preprocessor.
* Improve tolerance for file names. Use case-insensitive compare and handle different types of slashes.
* Improve logging options
* Further testing
* CMake projects should add source files into folders via source_group, to make the project structure more clear. Until now I didn't find a solution for that.
* Add build chains other than MSBuild. Although the source code is open for that, my knowledge of other build processes is very limited. So contributions for this task are very welcome.

## Credits

checkIncludes uses TinyXml2 to parse and edit MSBuild project files.<br>
https://github.com/leethomason/tinyxml2
