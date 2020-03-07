/*******************************************************************************
 * @brief   TWins - ANSI terminal escape code macros
 * @author  Mariusz Midor
 *          https://bitbucket.org/mmidor/twins
 * @note    This file is based on terminal_codes.h (c) NXP
 ******************************************************************************/

#pragma once

// -----------------------------------------------------------------------------

/**
 *  @brief ANSI terminal special escape code macros.
 *
 *  ANSI terminal compatible escape sequences. These escape sequences are designed to be concatenated with existing
 *  strings to modify their display on a compatible terminal application.
 *
 *  @code
 *      printf("Normal String, "
 *             ESC_BOLD_ON "Bold String, "
 *             ESC_UNDERLINE_ON "Bold and Underlined String, "
 *             ESC_RESET ESC_FG_BLUE ESC_BG_YELLOW "Normal Blue-on-Yellow String");
 *  @endcode
 */

// -----------------------------------------------------------------------------

#define ANSI_ESC(x)     "\033" x
#define ANSI_CSI(x)     ANSI_ESC("[" x)
#define ANSI_OSC(x)     ANSI_ESC("]" x)
#define ANSI_ST(x)      ANSI_ESC("\\" x)

/*******************************************************************************
 * @name Text Display Modifier Escape Sequences
 */

//@{

#define ESC_BOLD_ON                     ANSI_CSI("1m")
#define ESC_BOLD_OFF                    ANSI_CSI("22m")

#define ESC_FAINT_ON                    ANSI_CSI("2m")
#define ESC_FAINT_OFF                   ANSI_CSI("22m")

// if not italics, may be same as inverse
#define ESC_ITALICS_ON                  ANSI_CSI("3m")
#define ESC_ITALICS_OFF                 ANSI_CSI("23m")

#define ESC_UNDERLINE_ON                ANSI_CSI("4m")
#define ESC_UNDERLINE_OFF               ANSI_CSI("24m")

// if not blinks, the bg color may be lighter
#define ESC_BLINK_ON                    ANSI_CSI("5m")
#define ESC_BLINK_OFF                   ANSI_CSI("25m")

#define ESC_INVERSE_ON                  ANSI_CSI("7m")
#define ESC_INVERSE_OFF                 ANSI_CSI("27m")

#define ESC_INVISIBLE_ON                ANSI_CSI("8m")
#define ESC_INVISIBLE_OFF               ANSI_CSI("28m")

#define ESC_STRIKETHROUGH_ON            ANSI_CSI("9m")
#define ESC_STRIKETHROUGH_OFF           ANSI_CSI("29m")


//@}

/*******************************************************************************
 * @name Text Colour Control Sequences
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

#define ESC_FG_DEFAULT                  ANSI_CSI("39m")
#define ESC_FG_RGB(r,g,b)               ANSI_CSI("38;2;" #r ";" #g ";" #b ";m")


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

#define ESC_BG_DEFAULT                  ANSI_CSI("49m")
#define ESC_BG_RGB(r,g,b)               ANSI_CSI("48;2;" #r ";" #g ";" #b ";m")

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
 * @name Miscellaneous Control Sequences
 */

//@{

/** Resets any escape sequence modifiers back to their defaults. */
#define ESC_RESET                       ANSI_CSI("0m")
/** Erases the entire display, returning the cursor to the top left. */
#define ESC_ERASE_DISPLAY_BELOW         ANSI_CSI("0J")
#define ESC_ERASE_DISPLAY_ABOVE         ANSI_CSI("1J")
#define ESC_ERASE_DISPLAY_ALL           ANSI_CSI("2J")

/** Erases the current line, returning the cursor to the far left. */
#define ESC_ERASE_LINE_RIGHT            ANSI_CSI("0K")
#define ESC_ERASE_LINE_LEFT             ANSI_CSI("1K")
#define ESC_ERASE_LINE_ALL              ANSI_CSI("2K")

#define ESC_SCREEN_SAVE                 ANSI_CSI("?47h")
#define ESC_SCREEN_RESTORE              ANSI_CSI("?47l")

//@}

#define ESC_BELL                        "\007"

// TODO: add keyboard codes
