# rbfx game engine template

## How to start game development

### Coding

1. Clone this repository using `git clone`
2. Run `git submodule update --init --recursive --depth 1` inside of it to fetch all dependencies
3. Make sure the rbfx submodule is up to date: `pushd rbfx ; git pull ; popd`
4. Use your favorite IDE to start coding!

### Scene editing

1. Clone the rbfx repository using `git clone https://github.com/rbfx/rbfx.git`
2. Configure and compile using cmake
3. `cd` into the `bin` directory inside your build directory and run `Editor`
4. Choose the `Project` folder inside your game repository

### Enabling and disabling rbfx components

You can enable/disable rbfx components in the `CMakeLists.txt`. The default settings are just fine for most small games.

## Executable Distribution

The only build outputs needed for public distribution are: `Project` and `Game(.exe)`
