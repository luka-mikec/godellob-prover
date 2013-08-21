godellob-prover
===============

Automatic theorem prover for Gödel-Löb system. 

Requires newer C++ compiler (godellob-prover is written in C++11 standard).

Unixoids: download as zip, extract, run g++ *.cpp -std=c++11 -o bin 

Windows: same as Unixoids, but might require downloading MinGW first (gcc for Windows) or any other C++11-compliant compiler (MSVC etc.)

(compatible with Emscripten so it can be run in a browser, old demo: [here](https://www.ffri.hr/~lmikec/gl), and in case you ever wondered about GL's theoremhood while in the move, it's adjusted to work with Google's NDK for Android, just use g++ from 4.8 and -std=c++11)

[More info in Croatian language](https://github.com/luka-mikec/godellob-prover/wiki)

![Screenshot godellob-prover](/res/shot.png "Screenshot")
