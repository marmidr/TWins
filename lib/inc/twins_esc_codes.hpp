/*******************************************************************************
 * @brief   TWins - ANSI terminal escape code macros
 * @author  Mariusz Midor
 *          https://bitbucket.org/marmidr/twins
 *          https://github.com/marmidr/twins
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

#define ANSI_CSI(x)     "\e" "["  x  // Control Sequence Introducer
#define ANSI_OSC(x)     "\e" "]"  x  // Operating System Command
#define ANSI_ST(x)      "\e" "\\" x  // String Terminator

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

#define ESC_OVERLINE_ON                 ANSI_CSI("53m")
#define ESC_OVERLINE_OFF                ANSI_CSI("55m")

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
#define ESC_LINK(url, caption)          ANSI_OSC("8;;") url "\u0007" caption ANSI_OSC("8;;\u0007")
#define ESC_LINK_FMT                    ESC_LINK("%s", "%s")

//@}

/*******************************************************************************
 * @name Text Color Control Sequences
 */

// 4/8/24-bit ANSI colors
// https://en.wikipedia.org/wiki/ANSI_escape_code

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
#define ESC_FG_COLOR(n)                 ANSI_CSI("38;5;" #n "m")                // n: 1..255, 232..255=black->white
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
#define ESC_BG_COLOR(n)                 ANSI_CSI("48;5;" #n "m")                // n: 1..255, 232..255=black->white
#define ESC_BG_DEFAULT                  ANSI_CSI("49m")

/** Put Foreground and Background colors to their defaults */
#define ESC_COLORS_DEFAULT              ANSI_CSI("0m")

//@}


// -----------------------------------------------------------------------------

// WEB colors
// https://en.wikipedia.org/wiki/Web_colors


// RE find:
// ([^ ]+)\s+([0-9A-F]+)\s+([0-9A-F]+)\s+([0-9A-F]+)\s+([0-9A-F]+)\s+([0-9A-F]+)\s+([0-9A-F]+)
// RE replace with:
// #define ESC_FG_$1 \t\t ESC_FG_RGB($5, $6, $7)\n#define ESC_BG_$1 \t\t ESC_BG_RGB($5, $6, $7)

// Pink colors
#define ESC_FG_Pink                   ESC_FG_RGB(255, 192, 203)
#define ESC_BG_Pink                   ESC_BG_RGB(255, 192, 203)
#define ESC_FG_LightPink              ESC_FG_RGB(255, 182, 193)
#define ESC_BG_LightPink              ESC_BG_RGB(255, 182, 193)
#define ESC_FG_HotPink                ESC_FG_RGB(255, 105, 180)
#define ESC_BG_HotPink                ESC_BG_RGB(255, 105, 180)
#define ESC_FG_DeepPink               ESC_FG_RGB(255, 20, 147)
#define ESC_BG_DeepPink               ESC_BG_RGB(255, 20, 147)
#define ESC_FG_PaleVioletRed          ESC_FG_RGB(219, 112, 147)
#define ESC_BG_PaleVioletRed          ESC_BG_RGB(219, 112, 147)
#define ESC_FG_MediumVioletRed        ESC_FG_RGB(199, 21, 133)
#define ESC_BG_MediumVioletRed        ESC_BG_RGB(199, 21, 133)

// Red colors
#define ESC_FG_LightSalmon            ESC_FG_RGB(255, 160, 122)
#define ESC_BG_LightSalmon            ESC_BG_RGB(255, 160, 122)
#define ESC_FG_Salmon                 ESC_FG_RGB(250, 128, 114)
#define ESC_BG_Salmon                 ESC_BG_RGB(250, 128, 114)
#define ESC_FG_DarkSalmon             ESC_FG_RGB(233, 150, 122)
#define ESC_BG_DarkSalmon             ESC_BG_RGB(233, 150, 122)
#define ESC_FG_LightCoral             ESC_FG_RGB(240, 128, 128)
#define ESC_BG_LightCoral             ESC_BG_RGB(240, 128, 128)
#define ESC_FG_IndianRed              ESC_FG_RGB(205, 92, 92)
#define ESC_BG_IndianRed              ESC_BG_RGB(205, 92, 92)
#define ESC_FG_Crimson                ESC_FG_RGB(220, 20, 60)
#define ESC_BG_Crimson                ESC_BG_RGB(220, 20, 60)
#define ESC_FG_Firebrick              ESC_FG_RGB(178, 34, 34)
#define ESC_BG_Firebrick              ESC_BG_RGB(178, 34, 34)
#define ESC_FG_DarkRed                ESC_FG_RGB(139, 0, 0)
#define ESC_BG_DarkRed                ESC_BG_RGB(139, 0, 0)
#define ESC_FG_Red                    ESC_FG_RGB(255, 0, 0)
#define ESC_BG_Red                    ESC_BG_RGB(255, 0, 0)

// Orange colors
#define ESC_FG_OrangeRed              ESC_FG_RGB(255, 69, 0)
#define ESC_BG_OrangeRed              ESC_BG_RGB(255, 69, 0)
#define ESC_FG_Tomato                 ESC_FG_RGB(255, 99, 71)
#define ESC_BG_Tomato                 ESC_BG_RGB(255, 99, 71)
#define ESC_FG_Coral                  ESC_FG_RGB(255, 127, 80)
#define ESC_BG_Coral                  ESC_BG_RGB(255, 127, 80)
#define ESC_FG_DarkOrange             ESC_FG_RGB(255, 140, 0)
#define ESC_BG_DarkOrange             ESC_BG_RGB(255, 140, 0)
#define ESC_FG_Orange                 ESC_FG_RGB(255, 165, 0)
#define ESC_BG_Orange                 ESC_BG_RGB(255, 165, 0)

// Yellow colors
#define ESC_FG_Yellow                 ESC_FG_RGB(255, 255, 0)
#define ESC_BG_Yellow                 ESC_BG_RGB(255, 255, 0)
#define ESC_FG_LightYellow            ESC_FG_RGB(255, 255, 224)
#define ESC_BG_LightYellow            ESC_BG_RGB(255, 255, 224)
#define ESC_FG_LemonChiffon           ESC_FG_RGB(255, 250, 205)
#define ESC_BG_LemonChiffon           ESC_BG_RGB(255, 250, 205)
#define ESC_FG_LightGoldenrodYellow   ESC_FG_RGB(250, 250, 210)
#define ESC_BG_LightGoldenrodYellow   ESC_BG_RGB(250, 250, 210)
#define ESC_FG_PapayaWhip             ESC_FG_RGB(255, 239, 213)
#define ESC_BG_PapayaWhip             ESC_BG_RGB(255, 239, 213)
#define ESC_FG_Moccasin               ESC_FG_RGB(255, 228, 181)
#define ESC_BG_Moccasin               ESC_BG_RGB(255, 228, 181)
#define ESC_FG_PeachPuff              ESC_FG_RGB(255, 218, 185)
#define ESC_BG_PeachPuff              ESC_BG_RGB(255, 218, 185)
#define ESC_FG_PaleGoldenrod          ESC_FG_RGB(238, 232, 170)
#define ESC_BG_PaleGoldenrod          ESC_BG_RGB(238, 232, 170)
#define ESC_FG_Khaki                  ESC_FG_RGB(240, 230, 140)
#define ESC_BG_Khaki                  ESC_BG_RGB(240, 230, 140)
#define ESC_FG_DarkKhaki              ESC_FG_RGB(189, 183, 107)
#define ESC_BG_DarkKhaki              ESC_BG_RGB(189, 183, 107)
#define ESC_FG_Gold                   ESC_FG_RGB(255, 215, 0)
#define ESC_BG_Gold                   ESC_BG_RGB(255, 215, 0)

// Brown colors
#define ESC_FG_Cornsilk               ESC_FG_RGB(255, 248, 220)
#define ESC_BG_Cornsilk               ESC_BG_RGB(255, 248, 220)
#define ESC_FG_BlanchedAlmond         ESC_FG_RGB(255, 235, 205)
#define ESC_BG_BlanchedAlmond         ESC_BG_RGB(255, 235, 205)
#define ESC_FG_Bisque                 ESC_FG_RGB(255, 228, 196)
#define ESC_BG_Bisque                 ESC_BG_RGB(255, 228, 196)
#define ESC_FG_NavajoWhite            ESC_FG_RGB(255, 222, 173)
#define ESC_BG_NavajoWhite            ESC_BG_RGB(255, 222, 173)
#define ESC_FG_Wheat                  ESC_FG_RGB(245, 222, 179)
#define ESC_BG_Wheat                  ESC_BG_RGB(245, 222, 179)
#define ESC_FG_Burlywood              ESC_FG_RGB(222, 184, 135)
#define ESC_BG_Burlywood              ESC_BG_RGB(222, 184, 135)
#define ESC_FG_Tan                    ESC_FG_RGB(210, 180, 140)
#define ESC_BG_Tan                    ESC_BG_RGB(210, 180, 140)
#define ESC_FG_RosyBrown              ESC_FG_RGB(188, 143, 143)
#define ESC_BG_RosyBrown              ESC_BG_RGB(188, 143, 143)
#define ESC_FG_SandyBrown             ESC_FG_RGB(244, 164, 96)
#define ESC_BG_SandyBrown             ESC_BG_RGB(244, 164, 96)
#define ESC_FG_Goldenrod              ESC_FG_RGB(218, 165, 32)
#define ESC_BG_Goldenrod              ESC_BG_RGB(218, 165, 32)
#define ESC_FG_DarkGoldenrod          ESC_FG_RGB(184, 134, 11)
#define ESC_BG_DarkGoldenrod          ESC_BG_RGB(184, 134, 11)
#define ESC_FG_Peru                   ESC_FG_RGB(205, 133, 63)
#define ESC_BG_Peru                   ESC_BG_RGB(205, 133, 63)
#define ESC_FG_Chocolate              ESC_FG_RGB(210, 105, 30)
#define ESC_BG_Chocolate              ESC_BG_RGB(210, 105, 30)
#define ESC_FG_SaddleBrown            ESC_FG_RGB(139, 69, 19)
#define ESC_BG_SaddleBrown            ESC_BG_RGB(139, 69, 19)
#define ESC_FG_Sienna                 ESC_FG_RGB(160, 82, 45)
#define ESC_BG_Sienna                 ESC_BG_RGB(160, 82, 45)
#define ESC_FG_Brown                  ESC_FG_RGB(165, 42, 42)
#define ESC_BG_Brown                  ESC_BG_RGB(165, 42, 42)
#define ESC_FG_Maroon                 ESC_FG_RGB(128, 0, 0)
#define ESC_BG_Maroon                 ESC_BG_RGB(128, 0, 0)

// Green colors
#define ESC_FG_DarkOliveGreen         ESC_FG_RGB(85, 107, 47)
#define ESC_BG_DarkOliveGreen         ESC_BG_RGB(85, 107, 47)
#define ESC_FG_Olive                  ESC_FG_RGB(128, 128, 0)
#define ESC_BG_Olive                  ESC_BG_RGB(128, 128, 0)
#define ESC_FG_OliveDrab              ESC_FG_RGB(107, 142, 35)
#define ESC_BG_OliveDrab              ESC_BG_RGB(107, 142, 35)
#define ESC_FG_YellowGreen            ESC_FG_RGB(154, 205, 50)
#define ESC_BG_YellowGreen            ESC_BG_RGB(154, 205, 50)
#define ESC_FG_LimeGreen              ESC_FG_RGB(50, 205, 50)
#define ESC_BG_LimeGreen              ESC_BG_RGB(50, 205, 50)
#define ESC_FG_Lime                   ESC_FG_RGB(0, 255, 0)
#define ESC_BG_Lime                   ESC_BG_RGB(0, 255, 0)
#define ESC_FG_LawnGreen              ESC_FG_RGB(124, 252, 0)
#define ESC_BG_LawnGreen              ESC_BG_RGB(124, 252, 0)
#define ESC_FG_Chartreuse             ESC_FG_RGB(127, 255, 0)
#define ESC_BG_Chartreuse             ESC_BG_RGB(127, 255, 0)
#define ESC_FG_GreenYellow            ESC_FG_RGB(173, 255, 47)
#define ESC_BG_GreenYellow            ESC_BG_RGB(173, 255, 47)
#define ESC_FG_SpringGreen            ESC_FG_RGB(0, 255, 127)
#define ESC_BG_SpringGreen            ESC_BG_RGB(0, 255, 127)
#define ESC_FG_MediumSpringGreen      ESC_FG_RGB(0, 250, 154)
#define ESC_BG_MediumSpringGreen      ESC_BG_RGB(0, 250, 154)
#define ESC_FG_LightGreen             ESC_FG_RGB(144, 238, 144)
#define ESC_BG_LightGreen             ESC_BG_RGB(144, 238, 144)
#define ESC_FG_PaleGreen              ESC_FG_RGB(152, 251, 152)
#define ESC_BG_PaleGreen              ESC_BG_RGB(152, 251, 152)
#define ESC_FG_DarkSeaGreen           ESC_FG_RGB(143, 188, 143)
#define ESC_BG_DarkSeaGreen           ESC_BG_RGB(143, 188, 143)
#define ESC_FG_MediumAquamarine       ESC_FG_RGB(102, 205, 170)
#define ESC_BG_MediumAquamarine       ESC_BG_RGB(102, 205, 170)
#define ESC_FG_MediumSeaGreen         ESC_FG_RGB(60, 179, 113)
#define ESC_BG_MediumSeaGreen         ESC_BG_RGB(60, 179, 113)
#define ESC_FG_SeaGreen               ESC_FG_RGB(46, 139, 87)
#define ESC_BG_SeaGreen               ESC_BG_RGB(46, 139, 87)
#define ESC_FG_ForestGreen            ESC_FG_RGB(34, 139, 34)
#define ESC_BG_ForestGreen            ESC_BG_RGB(34, 139, 34)
#define ESC_FG_Green                  ESC_FG_RGB(0, 128, 0)
#define ESC_BG_Green                  ESC_BG_RGB(0, 128, 0)
#define ESC_FG_DarkGreen              ESC_FG_RGB(0, 100, 0)
#define ESC_BG_DarkGreen              ESC_BG_RGB(0, 100, 0)

// Cyan colors
#define ESC_FG_Aqua                   ESC_FG_RGB(0, 255, 255)
#define ESC_BG_Aqua                   ESC_BG_RGB(0, 255, 255)
#define ESC_FG_Cyan                   ESC_FG_RGB(0, 255, 255)
#define ESC_BG_Cyan                   ESC_BG_RGB(0, 255, 255)
#define ESC_FG_LightCyan              ESC_FG_RGB(224, 255, 255)
#define ESC_BG_LightCyan              ESC_BG_RGB(224, 255, 255)
#define ESC_FG_PaleTurquoise          ESC_FG_RGB(175, 238, 238)
#define ESC_BG_PaleTurquoise          ESC_BG_RGB(175, 238, 238)
#define ESC_FG_Aquamarine             ESC_FG_RGB(127, 255, 212)
#define ESC_BG_Aquamarine             ESC_BG_RGB(127, 255, 212)
#define ESC_FG_Turquoise              ESC_FG_RGB(64, 224, 208)
#define ESC_BG_Turquoise              ESC_BG_RGB(64, 224, 208)
#define ESC_FG_MediumTurquoise        ESC_FG_RGB(72, 209, 204)
#define ESC_BG_MediumTurquoise        ESC_BG_RGB(72, 209, 204)
#define ESC_FG_DarkTurquoise          ESC_FG_RGB(0, 206, 209)
#define ESC_BG_DarkTurquoise          ESC_BG_RGB(0, 206, 209)
#define ESC_FG_LightSeaGreen          ESC_FG_RGB(32, 178, 170)
#define ESC_BG_LightSeaGreen          ESC_BG_RGB(32, 178, 170)
#define ESC_FG_CadetBlue              ESC_FG_RGB(95, 158, 160)
#define ESC_BG_CadetBlue              ESC_BG_RGB(95, 158, 160)
#define ESC_FG_DarkCyan               ESC_FG_RGB(0, 139, 139)
#define ESC_BG_DarkCyan               ESC_BG_RGB(0, 139, 139)
#define ESC_FG_Teal                   ESC_FG_RGB(0, 128, 128)
#define ESC_BG_Teal                   ESC_BG_RGB(0, 128, 128)

// Blue colors
#define ESC_FG_LightSteelBlue         ESC_FG_RGB(176, 196, 222)
#define ESC_BG_LightSteelBlue         ESC_BG_RGB(176, 196, 222)
#define ESC_FG_PowderBlue             ESC_FG_RGB(176, 224, 230)
#define ESC_BG_PowderBlue             ESC_BG_RGB(176, 224, 230)
#define ESC_FG_LightBlue              ESC_FG_RGB(173, 216, 230)
#define ESC_BG_LightBlue              ESC_BG_RGB(173, 216, 230)
#define ESC_FG_SkyBlue                ESC_FG_RGB(135, 206, 235)
#define ESC_BG_SkyBlue                ESC_BG_RGB(135, 206, 235)
#define ESC_FG_LightSkyBlue           ESC_FG_RGB(135, 206, 250)
#define ESC_BG_LightSkyBlue           ESC_BG_RGB(135, 206, 250)
#define ESC_FG_DeepSkyBlue            ESC_FG_RGB(0, 191, 255)
#define ESC_BG_DeepSkyBlue            ESC_BG_RGB(0, 191, 255)
#define ESC_FG_DodgerBlue             ESC_FG_RGB(30, 144, 255)
#define ESC_BG_DodgerBlue             ESC_BG_RGB(30, 144, 255)
#define ESC_FG_CornflowerBlue         ESC_FG_RGB(100, 149, 237)
#define ESC_BG_CornflowerBlue         ESC_BG_RGB(100, 149, 237)
#define ESC_FG_SteelBlue              ESC_FG_RGB(70, 130, 180)
#define ESC_BG_SteelBlue              ESC_BG_RGB(70, 130, 180)
#define ESC_FG_RoyalBlue              ESC_FG_RGB(65, 105, 225)
#define ESC_BG_RoyalBlue              ESC_BG_RGB(65, 105, 225)
#define ESC_FG_Blue                   ESC_FG_RGB(0, 0, 255)
#define ESC_BG_Blue                   ESC_BG_RGB(0, 0, 255)
#define ESC_FG_MediumBlue             ESC_FG_RGB(0, 0, 205)
#define ESC_BG_MediumBlue             ESC_BG_RGB(0, 0, 205)
#define ESC_FG_DarkBlue               ESC_FG_RGB(0, 0, 139)
#define ESC_BG_DarkBlue               ESC_BG_RGB(0, 0, 139)
#define ESC_FG_Navy                   ESC_FG_RGB(0, 0, 128)
#define ESC_BG_Navy                   ESC_BG_RGB(0, 0, 128)
#define ESC_FG_MidnightBlue           ESC_FG_RGB(25, 25, 112)
#define ESC_BG_MidnightBlue           ESC_BG_RGB(25, 25, 112)

// Purple, violet, and magenta colors
#define ESC_FG_Lavender               ESC_FG_RGB(230, 230, 250)
#define ESC_BG_Lavender               ESC_BG_RGB(230, 230, 250)
#define ESC_FG_Thistle                ESC_FG_RGB(216, 191, 216)
#define ESC_BG_Thistle                ESC_BG_RGB(216, 191, 216)
#define ESC_FG_Plum                   ESC_FG_RGB(221, 160, 221)
#define ESC_BG_Plum                   ESC_BG_RGB(221, 160, 221)
#define ESC_FG_Violet                 ESC_FG_RGB(238, 130, 238)
#define ESC_BG_Violet                 ESC_BG_RGB(238, 130, 238)
#define ESC_FG_Orchid                 ESC_FG_RGB(218, 112, 214)
#define ESC_BG_Orchid                 ESC_BG_RGB(218, 112, 214)
#define ESC_FG_Fuchsia                ESC_FG_RGB(255, 0, 255)
#define ESC_BG_Fuchsia                ESC_BG_RGB(255, 0, 255)
#define ESC_FG_Magenta                ESC_FG_RGB(255, 0, 255)
#define ESC_BG_Magenta                ESC_BG_RGB(255, 0, 255)
#define ESC_FG_MediumOrchid           ESC_FG_RGB(186, 85, 211)
#define ESC_BG_MediumOrchid           ESC_BG_RGB(186, 85, 211)
#define ESC_FG_MediumPurple           ESC_FG_RGB(147, 112, 219)
#define ESC_BG_MediumPurple           ESC_BG_RGB(147, 112, 219)
#define ESC_FG_BlueViolet             ESC_FG_RGB(138, 43, 226)
#define ESC_BG_BlueViolet             ESC_BG_RGB(138, 43, 226)
#define ESC_FG_DarkViolet             ESC_FG_RGB(148, 0, 211)
#define ESC_BG_DarkViolet             ESC_BG_RGB(148, 0, 211)
#define ESC_FG_DarkOrchid             ESC_FG_RGB(153, 50, 204)
#define ESC_BG_DarkOrchid             ESC_BG_RGB(153, 50, 204)
#define ESC_FG_DarkMagenta            ESC_FG_RGB(139, 0, 139)
#define ESC_BG_DarkMagenta            ESC_BG_RGB(139, 0, 139)
#define ESC_FG_Purple                 ESC_FG_RGB(128, 0, 128)
#define ESC_BG_Purple                 ESC_BG_RGB(128, 0, 128)
#define ESC_FG_Indigo                 ESC_FG_RGB(75, 0, 130)
#define ESC_BG_Indigo                 ESC_BG_RGB(75, 0, 130)
#define ESC_FG_DarkSlateBlue          ESC_FG_RGB(72, 61, 139)
#define ESC_BG_DarkSlateBlue          ESC_BG_RGB(72, 61, 139)
#define ESC_FG_SlateBlue              ESC_FG_RGB(106, 90, 205)
#define ESC_BG_SlateBlue              ESC_BG_RGB(106, 90, 205)
#define ESC_FG_MediumSlateBlue        ESC_FG_RGB(123, 104, 238)
#define ESC_BG_MediumSlateBlue        ESC_BG_RGB(123, 104, 238)

// Gray and black colors
#define ESC_FG_Gainsboro              ESC_FG_RGB(220, 220, 220)
#define ESC_BG_Gainsboro              ESC_BG_RGB(220, 220, 220)
#define ESC_FG_LightGray              ESC_FG_RGB(211, 211, 211)
#define ESC_BG_LightGray              ESC_BG_RGB(211, 211, 211)
#define ESC_FG_Silver                 ESC_FG_RGB(192, 192, 192)
#define ESC_BG_Silver                 ESC_BG_RGB(192, 192, 192)
#define ESC_FG_DarkGray               ESC_FG_RGB(169, 169, 169)
#define ESC_BG_DarkGray               ESC_BG_RGB(169, 169, 169)
#define ESC_FG_Gray                   ESC_FG_RGB(128, 128, 128)
#define ESC_BG_Gray                   ESC_BG_RGB(128, 128, 128)
#define ESC_FG_DimGray                ESC_FG_RGB(105, 105, 105)
#define ESC_BG_DimGray                ESC_BG_RGB(105, 105, 105)
#define ESC_FG_LightSlateGray         ESC_FG_RGB(119, 136, 153)
#define ESC_BG_LightSlateGray         ESC_BG_RGB(119, 136, 153)
#define ESC_FG_SlateGray              ESC_FG_RGB(112, 128, 144)
#define ESC_BG_SlateGray              ESC_BG_RGB(112, 128, 144)
#define ESC_FG_DarkSlateGray          ESC_FG_RGB(47, 79, 79)
#define ESC_BG_DarkSlateGray          ESC_BG_RGB(47, 79, 79)
#define ESC_FG_Black                  ESC_FG_RGB(0, 0, 0)
#define ESC_BG_Black                  ESC_BG_RGB(0, 0, 0)

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

/** @brief Delete line */
#define ESC_LINE_DELETE(n)              ANSI_CSI(#n "M")
#define ESC_LINE_DELETE_FMT             ESC_LINE_DELETE(%u)

//@}

/*******************************************************************************
 * @name Character control
 */

//@{

/** @brief Repeat last character \p n times - not fully supported */
#define ESC_CHAR_REPEAT_LAST(n)         ANSI_CSI(#n "b")
#define ESC_CHAR_REPEAT_LAST_FMT        ESC_CHAR_REPEAT_LAST(%u)

/** @brief Erase \p n characters (replace with space) */
#define ESC_CHAR_ERASE(n)               ANSI_CSI(#n "X")
#define ESC_CHAR_ERASE_FMT              ESC_CHAR_ERASE(%u)

/** @brief Delete \p n characters */
#define ESC_CHAR_DELETE(n)              ANSI_CSI(#n "P")
#define ESC_CHAR_DELETE_FMT             ESC_CHAR_DELETE(%u)

/** @brief Insert \p n characters */
#define ESC_CHAR_INSERT(n)              ANSI_CSI(#n "@")
#define ESC_CHAR_INSERT_FMT             ESC_CHAR_INSERT(%u)

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
#define ESC_TERM_RESET                  "\033" "c"

/** @brief */
#define ESC_BELL                        "\007"

/** @brief Character encoding */
#define ESC_ENCODING_ISO8858_1          ANSI_ESC("%@")
#define ESC_ENCODING_UTF8               ANSI_ESC("%G")

/** @brief Terminal properties reporting */
#define ESC_REPORT_WINDOW_CHARS         ANSI_CSI("18t")
#define ESC_REPORT_SCREEN_CHARS         ANSI_CSI("19t")
#define ESC_REPORT_CAPABILITIES         ANSI_CSI("c")

/** @brief Maximum ESC sequence length (including null) */
#define ESC_SEQ_MAX_LENGTH              8

// bash: blink screen until key pressed
// { while true; do printf \\e[?5h; sleep 0.3; printf \\e[?5l; read -s -n1 -t1 && break; done; }

//@}
