# Brief

`TWins` library is meant to support terminal emulator ASCII codes 
for small ARM devices, like Cortex-M3.

## References

Implementation is based on examples:

* https://github.com/fidian/ansi/blob/master/ansi
* https://github.com/stark/Color-Scripts
* https://wiki.bash-hackers.org/scripting/terminalcodes

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
  * regular text -> echo
  * commands (backspace, up, down, tab)

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
