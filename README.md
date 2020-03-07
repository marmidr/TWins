# Brief

`TWins` library is designed to allow a developer to create graphical terminal applications on non-os platforms, like bare Cortex-M3.
It provides basic facilities required by interactive applications such as screen and cursor management, keyboard input, keymaps, color codes.
Additional it allows to desing terminal windows and it's controls in a convenient way as tree of `twins::Widget` structures.


## References

Implementation is based on examples:

* https://github.com/fidian/ansi/blob/master/ansi
* https://github.com/stark/Color-Scripts
* https://wiki.bash-hackers.org/scripting/terminalcodes
* http://0x80.pl/articles/terminals.html
* https://www.jedsoft.org/slang/

## Goals

### Primary goals

* text properties
    * foreground and background color codes
    * attributes (bold, inversion)
* operations
    * clear screen
    * go to home
    * go to location
* reading input
    * regular characters (a..z)
    * control codes (Up/Down, Del, Ctrl, Home, ...)

### Secondary goals

* support for drawing window with background and frame
* popup windows
* controls
    * panes
    * tabs
    * edit fields (text/number)
    * static labels
    * check boxes
    * radio group
    * button group

---

Hope I'll deal with it :)
