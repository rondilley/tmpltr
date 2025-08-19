/*****
 *
 * Description: Parser Interface - Common interface for multiple parsers
 * 
 * Copyright (c) 2008-2023, Ron Dilley
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

#ifndef PARSER_INTERFACE_DOT_H
#define PARSER_INTERFACE_DOT_H

/****
 *
 * includes
 *
 ****/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "../include/sysdep.h"

#ifndef __SYSDEP_H__
#error something is messed up
#endif

#include "../include/common.h"

/****
 *
 * defines
 *
 ****/

typedef enum {
    PARSER_TYPE_LEGACY = 0,
    PARSER_TYPE_FSM = 1
} ParserType;

/****
 *
 * Parser Interface Structure
 *
 ****/

typedef struct {
    ParserType type;
    const char* name;
    
    /* Core parser functions */
    void (*init)(void);
    void (*deinit)(void);
    int (*parseLine)(char *line);
    int (*getParsedField)(char *oBuf, int oBufLen, const unsigned int fieldNum);
    const char* (*getParsedFieldPtr)(const unsigned int fieldNum);
    void (*showCounts)(void);
    
    /* Parser-specific capabilities */
    int supports_streaming;
    int supports_zero_copy;
    int supports_aggregation;
} ParserInterface;

/****
 *
 * functions
 *
 ****/

/* Get parser interface by type */
ParserInterface* getParser(ParserType type);

/* List available parsers */
void listParsers(void);

/* Get parser type from string name */
ParserType getParserTypeFromString(const char* name);

/* Get parser name from type */
const char* getParserName(ParserType type);

#endif /* PARSER_INTERFACE_DOT_H */