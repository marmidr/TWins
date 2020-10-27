# TWins in brief

[![Coverage Status](https://coveralls.io/repos/bitbucket/mmidor/twins/badge.svg?branch=master)](https://coveralls.io/bitbucket/mmidor/twins?branch=master) 

`TWins` is a C++ library designed to allow a developer to create graphical terminal applications on non-os platforms, like bare Cortex-M3.
It provides basic facilities required by interactive applications such as screen and cursor management, keyboard input, keymaps, color codes.
Additional it allows to define terminal windows and it's widgets in a convenient way as const tree of `twins::Widget` structures.

![example 1](doc/sshot5.png)
![example 2](doc/sshot6.png)

Wikipedia: [reference color tables for different terminals](https://en.m.wikipedia.org/wiki/ANSI_escape_code)

## References

Implementation is based on examples:

* https://github.com/fidian/ansi/blob/master/ansi
* https://github.com/stark/Color-Scripts
* https://wiki.bash-hackers.org/scripting/terminalcodes
* http://0x80.pl/articles/terminals.html
* https://www.jedsoft.org/slang/
* https://www.systutorials.com/docs/linux/man/4-console_codes/#lbAF

## Primary goals

- text properties
    - [x] foreground and background color codes
    - [x] attributes (bold, inversion)
- operations
    - [x] clear screen
    - [x] go to home
    - [x] go to location
- reading input
    - [x] regular characters (a..z)
    - [x] control codes (Up/Down, Del, Ctrl, Home, ...)
- [x] buffered terminal output
- [x] platform abstraction layer (PAL) to ease porting

## Secondary goals

- widgets (controls) to implement
    - [x] window
    - [x] panel
    - [x] static label / led
    - [x] check box
    - [x] edit field (text/number)
    - [x] radio button
    - [x] page control
    - [x] progress bar
    - [x] list box
    - [x] combo box
    - [x] scrollable text box
    - [x] custom widget base
    - [ ] scrollbar
    - [ ] horizontal page control
    - [x] popup windows
- navigation
    - [x] widgets navigation by Tab/Esc key
    - [x] render focused widget state
    - [x] blinking cursor instead of inversed colors
    - [x] select widget by mouse
- notifications
    - [x] notify event per widget type (button clicked, checkbox toggled)
- [x] color theme for window
- [x] keyboard handler returns if key was handled by active widget
- [x] support for mouse click
- [ ] password input


## Prerequisites

```bash
sudo apt install g++ cmake cmake-curses-gui
```

## How to build

Project is CMake-based and contains two targets: *TWinsDemo* and *TWinsUT*.  
Tests are enabled by default, Demo has to be enabled in commandline or in `ccmake`.

```bash
mkdir build && cd build
cmake -DTWINS_BUILD_DEMO=ON -DTWINS_THEME_DIR="../demo/inc/" ..
make -j
ctest -V
```

#### Run demo:

```bash
./bin/TWinsDemo
```

If you have `gcovr` installed, after running tests you can generate test coverage HTML report

```bash
make twins_cov_only
```

---
