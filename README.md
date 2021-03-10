# DANAS/VC!
### My Fantasy Computer
it is

### Installation and Building
First of all, make sure you have `g++` and `cmake` installed:
```bash
sudo apt-get install g++ cmake
```

Download these dependencies:
- SDL2
- Lua

#### For Linux (Debian / Ubuntu)
```bash
sudo apt-get install libsdl2-dev liblua5.3-dev
```

Then type in Terminal:
```bash
cmake -G "Unix Makefiles" ./build
cd build
make
```

That's it