/*****
 *
 * Description: Character Classification Lookup Tables
 * 
 * Copyright (c) 2025, Ron Dilley
 * All rights reserved.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 ****/

#include "../include/common.h"

/****
 *
 * Fast character classification lookup table
 *
 * DESCRIPTION:
 *   Pre-computed lookup table for fast character classification without
 *   function call overhead. Each byte value maps to classification bits
 *   that can be tested using bitwise operations for optimal performance.
 *
 * USAGE:
 *   Use FAST_ISALPHA(c), FAST_ISDIGIT(c), etc. macros defined in common.h
 *   These macros perform: char_class_table[(unsigned char)(c)] & CHAR_ALPHA
 *
 * CLASSIFICATION BITS:
 *   CHAR_ALPHA  (0x01) - Alphabetic character (a-z, A-Z)
 *   CHAR_DIGIT  (0x02) - Digit character (0-9)
 *   CHAR_ALNUM  (0x03) - Alpha or numeric (combination of above)
 *   CHAR_XDIGIT (0x04) - Hexadecimal digit (0-9, a-f, A-F)
 *   CHAR_PUNCT  (0x08) - Punctuation character
 *   CHAR_SPACE  (0x10) - Whitespace character
 *   CHAR_CNTRL  (0x20) - Control character
 *   CHAR_PRINT  (0x40) - Printable character
 *
 * PERFORMANCE:
 *   Provides O(1) character classification vs. function call overhead
 *   Critical optimization for parser performance with millions of operations
 *
 * PORTABILITY:
 *   Based on ASCII character set (0-127), extended ASCII treated as non-classified
 *
 ****/

/* Character classification lookup table - one entry per byte value */
const unsigned char char_class_table[256] = {
    /* 0x00-0x0F */
    CHAR_CNTRL, CHAR_CNTRL, CHAR_CNTRL, CHAR_CNTRL,
    CHAR_CNTRL, CHAR_CNTRL, CHAR_CNTRL, CHAR_CNTRL,
    CHAR_CNTRL, CHAR_CNTRL | CHAR_SPACE, CHAR_CNTRL | CHAR_SPACE, CHAR_CNTRL,
    CHAR_CNTRL, CHAR_CNTRL | CHAR_SPACE, CHAR_CNTRL, CHAR_CNTRL,
    
    /* 0x10-0x1F */
    CHAR_CNTRL, CHAR_CNTRL, CHAR_CNTRL, CHAR_CNTRL,
    CHAR_CNTRL, CHAR_CNTRL, CHAR_CNTRL, CHAR_CNTRL,
    CHAR_CNTRL, CHAR_CNTRL, CHAR_CNTRL, CHAR_CNTRL,
    CHAR_CNTRL, CHAR_CNTRL, CHAR_CNTRL, CHAR_CNTRL,
    
    /* 0x20-0x2F */
    CHAR_SPACE | CHAR_PRINT,  /* space */
    CHAR_PUNCT | CHAR_PRINT,  /* ! */
    CHAR_PUNCT | CHAR_PRINT,  /* " */
    CHAR_PUNCT | CHAR_PRINT,  /* # */
    CHAR_PUNCT | CHAR_PRINT,  /* $ */
    CHAR_PUNCT | CHAR_PRINT,  /* % */
    CHAR_PUNCT | CHAR_PRINT,  /* & */
    CHAR_PUNCT | CHAR_PRINT,  /* ' */
    CHAR_PUNCT | CHAR_PRINT,  /* ( */
    CHAR_PUNCT | CHAR_PRINT,  /* ) */
    CHAR_PUNCT | CHAR_PRINT,  /* * */
    CHAR_PUNCT | CHAR_PRINT,  /* + */
    CHAR_PUNCT | CHAR_PRINT,  /* , */
    CHAR_PUNCT | CHAR_PRINT,  /* - */
    CHAR_PUNCT | CHAR_PRINT,  /* . */
    CHAR_PUNCT | CHAR_PRINT,  /* / */
    
    /* 0x30-0x3F */
    CHAR_DIGIT | CHAR_XDIGIT | CHAR_PRINT,  /* 0 */
    CHAR_DIGIT | CHAR_XDIGIT | CHAR_PRINT,  /* 1 */
    CHAR_DIGIT | CHAR_XDIGIT | CHAR_PRINT,  /* 2 */
    CHAR_DIGIT | CHAR_XDIGIT | CHAR_PRINT,  /* 3 */
    CHAR_DIGIT | CHAR_XDIGIT | CHAR_PRINT,  /* 4 */
    CHAR_DIGIT | CHAR_XDIGIT | CHAR_PRINT,  /* 5 */
    CHAR_DIGIT | CHAR_XDIGIT | CHAR_PRINT,  /* 6 */
    CHAR_DIGIT | CHAR_XDIGIT | CHAR_PRINT,  /* 7 */
    CHAR_DIGIT | CHAR_XDIGIT | CHAR_PRINT,  /* 8 */
    CHAR_DIGIT | CHAR_XDIGIT | CHAR_PRINT,  /* 9 */
    CHAR_PUNCT | CHAR_PRINT,  /* : */
    CHAR_PUNCT | CHAR_PRINT,  /* ; */
    CHAR_PUNCT | CHAR_PRINT,  /* < */
    CHAR_PUNCT | CHAR_PRINT,  /* = */
    CHAR_PUNCT | CHAR_PRINT,  /* > */
    CHAR_PUNCT | CHAR_PRINT,  /* ? */
    
    /* 0x40-0x4F */
    CHAR_PUNCT | CHAR_PRINT,  /* @ */
    CHAR_ALPHA | CHAR_XDIGIT | CHAR_PRINT,  /* A */
    CHAR_ALPHA | CHAR_XDIGIT | CHAR_PRINT,  /* B */
    CHAR_ALPHA | CHAR_XDIGIT | CHAR_PRINT,  /* C */
    CHAR_ALPHA | CHAR_XDIGIT | CHAR_PRINT,  /* D */
    CHAR_ALPHA | CHAR_XDIGIT | CHAR_PRINT,  /* E */
    CHAR_ALPHA | CHAR_XDIGIT | CHAR_PRINT,  /* F */
    CHAR_ALPHA | CHAR_PRINT,  /* G */
    CHAR_ALPHA | CHAR_PRINT,  /* H */
    CHAR_ALPHA | CHAR_PRINT,  /* I */
    CHAR_ALPHA | CHAR_PRINT,  /* J */
    CHAR_ALPHA | CHAR_PRINT,  /* K */
    CHAR_ALPHA | CHAR_PRINT,  /* L */
    CHAR_ALPHA | CHAR_PRINT,  /* M */
    CHAR_ALPHA | CHAR_PRINT,  /* N */
    CHAR_ALPHA | CHAR_PRINT,  /* O */
    
    /* 0x50-0x5F */
    CHAR_ALPHA | CHAR_PRINT,  /* P */
    CHAR_ALPHA | CHAR_PRINT,  /* Q */
    CHAR_ALPHA | CHAR_PRINT,  /* R */
    CHAR_ALPHA | CHAR_PRINT,  /* S */
    CHAR_ALPHA | CHAR_PRINT,  /* T */
    CHAR_ALPHA | CHAR_PRINT,  /* U */
    CHAR_ALPHA | CHAR_PRINT,  /* V */
    CHAR_ALPHA | CHAR_PRINT,  /* W */
    CHAR_ALPHA | CHAR_PRINT,  /* X */
    CHAR_ALPHA | CHAR_PRINT,  /* Y */
    CHAR_ALPHA | CHAR_PRINT,  /* Z */
    CHAR_PUNCT | CHAR_PRINT,  /* [ */
    CHAR_PUNCT | CHAR_PRINT,  /* \ */
    CHAR_PUNCT | CHAR_PRINT,  /* ] */
    CHAR_PUNCT | CHAR_PRINT,  /* ^ */
    CHAR_PUNCT | CHAR_PRINT,  /* _ */
    
    /* 0x60-0x6F */
    CHAR_PUNCT | CHAR_PRINT,  /* ` */
    CHAR_ALPHA | CHAR_XDIGIT | CHAR_PRINT,  /* a */
    CHAR_ALPHA | CHAR_XDIGIT | CHAR_PRINT,  /* b */
    CHAR_ALPHA | CHAR_XDIGIT | CHAR_PRINT,  /* c */
    CHAR_ALPHA | CHAR_XDIGIT | CHAR_PRINT,  /* d */
    CHAR_ALPHA | CHAR_XDIGIT | CHAR_PRINT,  /* e */
    CHAR_ALPHA | CHAR_XDIGIT | CHAR_PRINT,  /* f */
    CHAR_ALPHA | CHAR_PRINT,  /* g */
    CHAR_ALPHA | CHAR_PRINT,  /* h */
    CHAR_ALPHA | CHAR_PRINT,  /* i */
    CHAR_ALPHA | CHAR_PRINT,  /* j */
    CHAR_ALPHA | CHAR_PRINT,  /* k */
    CHAR_ALPHA | CHAR_PRINT,  /* l */
    CHAR_ALPHA | CHAR_PRINT,  /* m */
    CHAR_ALPHA | CHAR_PRINT,  /* n */
    CHAR_ALPHA | CHAR_PRINT,  /* o */
    
    /* 0x70-0x7F */
    CHAR_ALPHA | CHAR_PRINT,  /* p */
    CHAR_ALPHA | CHAR_PRINT,  /* q */
    CHAR_ALPHA | CHAR_PRINT,  /* r */
    CHAR_ALPHA | CHAR_PRINT,  /* s */
    CHAR_ALPHA | CHAR_PRINT,  /* t */
    CHAR_ALPHA | CHAR_PRINT,  /* u */
    CHAR_ALPHA | CHAR_PRINT,  /* v */
    CHAR_ALPHA | CHAR_PRINT,  /* w */
    CHAR_ALPHA | CHAR_PRINT,  /* x */
    CHAR_ALPHA | CHAR_PRINT,  /* y */
    CHAR_ALPHA | CHAR_PRINT,  /* z */
    CHAR_PUNCT | CHAR_PRINT,  /* { */
    CHAR_PUNCT | CHAR_PRINT,  /* | */
    CHAR_PUNCT | CHAR_PRINT,  /* } */
    CHAR_PUNCT | CHAR_PRINT,  /* ~ */
    CHAR_CNTRL,               /* DEL */
    
    /* 0x80-0xFF - Extended ASCII/UTF-8 */
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};