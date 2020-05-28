/*******************************************************************************
 * @brief   TWins - ANSI terminal escape code macros
 * @author  Mariusz Midor
 *          https://bitbucket.org/mmidor/twins
 * @note    This file is based on terminal_codes.h (c) NXP
 *          and https://en.m.wikipedia.org/wiki/ANSI_escape_code
 ******************************************************************************/

#pragma once

// -----------------------------------------------------------------------------

/**
 *  @brief ANSI terminal special escape code macros.
 *
 *  ANSI terminal compatible escape sequences. These escape sequences are designed to be concatenated with existing
 *  strings to modify their display on a compatible terminal application.
 *
 *  @code.c
 *      printf("Normal String, "
 *             ESC_ATTR_BOLD "Bold String, "
 *             ESC_UNDERLINE_ON "Bold and Underlined String, "
 *             ESC_RESET ESC_FG_BLUE ESC_BG_YELLOW "Normal Blue-on-Yellow String");
 *  @endcode
 */

// -----------------------------------------------------------------------------

#define ANSI_CSI(x)     "\033" "["  x  // Control Sequence Introducer
#define ANSI_OSC(x)     "\033" "]"  x  // Operating System Command
#define ANSI_ST(x)      "\033" "\\" x  // String Terminator

/*******************************************************************************
 * @name Text Display Modifier Escape Sequences
 */

//@{

#define ESC_BOLD                        ANSI_CSI("1m")
#define ESC_FAINT                       ANSI_CSI("2m")
#define ESC_NORMAL                      ANSI_CSI("22m")

// if not italics, may be same as inverse
#define ESC_ITALICS_ON                  ANSI_CSI("3m")
#define ESC_ITALICS_OFF                 ANSI_CSI("23m")

#define ESC_UNDERLINE_ON                ANSI_CSI("4m")
#define ESC_UNDERLINE_OFF               ANSI_CSI("24m")

// if not blinks, the bg color may be lighter
#define ESC_BLINK                       ANSI_CSI("5m")
#define ESC_BLINK_OFF                   ANSI_CSI("25m")

#define ESC_INVERSE_ON                  ANSI_CSI("7m")
#define ESC_INVERSE_OFF                 ANSI_CSI("27m")

#define ESC_INVISIBLE_ON                ANSI_CSI("8m")
#define ESC_INVISIBLE_OFF               ANSI_CSI("28m")

#define ESC_STRIKETHROUGH_ON            ANSI_CSI("9m")
#define ESC_STRIKETHROUGH_OFF           ANSI_CSI("29m")

#define ESC_FONT(id)                    ANSI_CSI("1" #id "m") // id: 1..9
#define ESC_FONT_DEFAULT                ANSI_CSI("10m")

#define ESC_ATTRIBUTES_DEFAULT          ANSI_CSI("10;22;23;24;25;27;28;29m")

// '\u001B]8;;https://github.com\u0007Click\u001B]8;;\u0007'
#define ESC_LINK(url, capt)             ANSI_OSC("8;;") url "\u0007" capt ANSI_OSC("8;;\u0007")

//@}

/*******************************************************************************
 * @name Text Color Control Sequences
 */

//@{

#define ESC_FG_BLACK                    ANSI_CSI("30m")
#define ESC_FG_BLACK_INTENSE            ANSI_CSI("90m")
#define ESC_FG_RED                      ANSI_CSI("31m")
#define ESC_FG_RED_INTENSE              ANSI_CSI("91m")
#define ESC_FG_GREEN                    ANSI_CSI("32m")
#define ESC_FG_GREEN_INTENSE            ANSI_CSI("92m")
#define ESC_FG_YELLOW                   ANSI_CSI("33m")
#define ESC_FG_YELLOW_INTENSE           ANSI_CSI("93m")
#define ESC_FG_BLUE                     ANSI_CSI("34m")
#define ESC_FG_BLUE_INTENSE             ANSI_CSI("94m")
#define ESC_FG_MAGENTA                  ANSI_CSI("35m")
#define ESC_FG_MAGENTA_INTENSE          ANSI_CSI("95m")
#define ESC_FG_CYAN                     ANSI_CSI("36m")
#define ESC_FG_CYAN_INTENSE             ANSI_CSI("96m")
#define ESC_FG_WHITE                    ANSI_CSI("37m")
#define ESC_FG_WHITE_INTENSE            ANSI_CSI("97m")

#define ESC_FG_RGB(r,g,b)               ANSI_CSI("38;2;" #r ";" #g ";" #b "m")  // r,g,b: 0..255
#define ESC_FG_COLOR(n)                 ANSI_CSI("38;5;" #n "m")                // n: 1..255
#define ESC_FG_DEFAULT                  ANSI_CSI("39m")


#define ESC_BG_BLACK                    ANSI_CSI("40m")
#define ESC_BG_BLACK_INTENSE            ANSI_CSI("100m")
#define ESC_BG_RED                      ANSI_CSI("41m")
#define ESC_BG_RED_INTENSE              ANSI_CSI("101m")
#define ESC_BG_GREEN                    ANSI_CSI("42m")
#define ESC_BG_GREEN_INTENSE            ANSI_CSI("102m")
#define ESC_BG_YELLOW                   ANSI_CSI("43m")
#define ESC_BG_YELLOW_INTENSE           ANSI_CSI("103m")
#define ESC_BG_BLUE                     ANSI_CSI("44m")
#define ESC_BG_BLUE_INTENSE             ANSI_CSI("104m")
#define ESC_BG_MAGENTA                  ANSI_CSI("45m")
#define ESC_BG_MAGENTA_INTENSE          ANSI_CSI("105m")
#define ESC_BG_CYAN                     ANSI_CSI("46m")
#define ESC_BG_CYAN_INTENSE             ANSI_CSI("106m")
#define ESC_BG_WHITE                    ANSI_CSI("47m")
#define ESC_BG_WHITE_INTENSE            ANSI_CSI("107m")

#define ESC_BG_RGB(r,g,b)               ANSI_CSI("48;2;" #r ";" #g ";" #b "m")  // r,g,b: 0..255
#define ESC_BG_COLOR(n)                 ANSI_CSI("48;5;" #n "m")                // n: 1..255
#define ESC_BG_DEFAULT                  ANSI_CSI("49m")

/** Put Foreground and Background colors to their defaults */
#define ESC_COLORS_DEFAULT              ANSI_CSI("0m")

//@}

 /*******************************************************************************
  * @name Cursor Positioning Control Sequences
  */

//@{

#define ESC_CURSOR_POS_SAVE             ANSI_CSI("s")
#define ESC_CURSOR_POS_RESTORE          ANSI_CSI("u")

#define ESC_CURSOR_HIDE                 ANSI_CSI("?25l")
#define ESC_CURSOR_SHOW                 ANSI_CSI("?25h")

#define ESC_CURSOR_HOME                 ANSI_CSI("H")
#define ESC_CURSOR_COLUMN(col)          ANSI_CSI(#col "G")
#define ESC_CURSOR_GOTO(row, col)       ANSI_CSI(#row ";" #col "H")
#define ESC_CURSOR_UP(lines)            ANSI_CSI(#lines "A")
#define ESC_CURSOR_DOWN(lines)          ANSI_CSI(#lines "B")
#define ESC_CURSOR_FORWARD(columns)     ANSI_CSI(#columns "C")
#define ESC_CURSOR_BACKWARD(columns)    ANSI_CSI(#columns "D")

#define ESC_CURSOR_COLUMN_FMT           ESC_CURSOR_COLUMN(%u)
#define ESC_CURSOR_GOTO_FMT             ESC_CURSOR_GOTO(%u, %u)
#define ESC_CURSOR_UP_FMT               ESC_CURSOR_UP(%u)
#define ESC_CURSOR_DOWN_FMT             ESC_CURSOR_DOWN(%u)
#define ESC_CURSOR_FORWARD_FMT          ESC_CURSOR_FORWARD(%u)
#define ESC_CURSOR_BACKWARD_FMT         ESC_CURSOR_BACKWARD(%u)

//@}

/*******************************************************************************
 * @name Line control
 */

//@{

/** @brief Erases the current line, returning the cursor to the far left. */
#define ESC_LINE_ERASE_ALL              ANSI_CSI("2K")
#define ESC_LINE_ERASE_RIGHT            ANSI_CSI("0K")
#define ESC_LINE_ERASE_LEFT             ANSI_CSI("1K")

/** @brief Insert line */
#define ESC_LINE_INSERT(n)              ANSI_CSI(#n "L")
#define ESC_LINE_INSERT_FMT             ESC_LINE_INSERT(%u)

//@}

/*******************************************************************************
 * @name Screen Control Sequences
 */

 //@{

/** @brief Erases the entire display, returning the cursor to the top left. */
#define ESC_SCREEN_ERASE_ALL            ANSI_CSI("2J")
#define ESC_SCREEN_ERASE_BELOW          ANSI_CSI("0J")
#define ESC_SCREEN_ERASE_ABOVE          ANSI_CSI("1J")

/** */
#define ESC_SCREEN_SAVE                 ANSI_CSI("?47h")
#define ESC_SCREEN_RESTORE              ANSI_CSI("?47l")

/** @brief Reverse/normal video mode (BG <--> FG) */
#define ESC_SCREEN_REVERSE_ON           ANSI_CSI("?5h")
#define ESC_SCREEN_REVERSE_OFF          ANSI_CSI("?5l")

/** @brief Scrool screen  */
#define ESC_SCREEN_SCROLL_UP(n)         ANSI_CSI(#n "S")
#define ESC_SCREEN_SCROLL_DOWN(n)       ANSI_CSI(#n "T")

#define ESC_SCREEN_SCROLL_UP_FMT        ESC_SCREEN_SCROLL_UP(%u)
#define ESC_SCREEN_SCROLL_DOWN_FMT      ESC_SCREEN_SCROLL_DOWN(%u)

//@}

/*******************************************************************************
 * @name Mouse Control Sequences
 *       https://www.systutorials.com/docs/linux/man/4-console_codes/#lbAF
 */

//@{

// Mode1: only click
#define ESC_MOUSE_REPORTING_M1_ON       ANSI_CSI("?9h")
#define ESC_MOUSE_REPORTING_M1_OFF      ANSI_CSI("?9l")

// Mode2: click + release + wheel + Ctrl/Alt/Shift
#define ESC_MOUSE_REPORTING_M2_ON       ANSI_CSI("?1000h")
#define ESC_MOUSE_REPORTING_M2_OFF      ANSI_CSI("?1000l")

//@}

/*******************************************************************************
 * @name Miscellaneous Control Sequences
 */

//@{

/** @brief Terminal reset - clear the screen and scroll buffer */
#define ESC_TERM_RESET                  ANSI_CSI("c")

/** @brief Repeat last character \p n times - not fully supported */
#define ESC_REPEAT_LAST_CHAR(n)         ANSI_CSI(#n "b")
#define ESC_REPEAT_LAST_CHAR_FMT        ESC_REPEAT_LAST_CHAR(%u)

/** @brief Character encoding */
#define ESC_ENCODING_ISO8858_1          ANSI_ESC("%@")
#define ESC_ENCODING_UTF8               ANSI_ESC("%G")

/** @brief Terminal properties reporting */
#define ESC_REPORT_WINDOW_CHARS         ANSI_CSI("18t")
#define ESC_REPORT_SCREEN_CHARS         ANSI_CSI("19t")
#define ESC_REPORT_CAPABILITIES         ANSI_CSI("c")


// bash: blink screen until key pressed
// { while true; do printf \\e[?5h; sleep 0.3; printf \\e[?5l; read -s -n1 -t1 && break; done; }

//@}
