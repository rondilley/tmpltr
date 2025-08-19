/*****
 *
 * Description: Parser Interface Implementation
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

/****
 *
 * includes
 *
 ****/

#include "parser_interface.h"
#include "parser.h"
#include "util.h"

/****
 *
 * Parser Interface Implementation
 *
 ****/

PRIVATE ParserInterface parser = {
    .type = PARSER_TYPE_LEGACY,
    .name = "parser",
    .init = initParser,
    .deinit = deInitParser,
    .parseLine = parseLine,
    .getParsedField = getParsedField,
    .getParsedFieldPtr = getParsedFieldPtr,
    .showCounts = showCounts,
    .supports_streaming = 0,
    .supports_zero_copy = 0,
    .supports_aggregation = 0
};


/****
 *
 * Interface Functions
 *
 ****/

ParserInterface* getParser(ParserType type)
{
    switch (type) {
        case PARSER_TYPE_LEGACY:
            return &parser;
        case PARSER_TYPE_FSM:
            return &parser; /* FSM disabled, fallback to legacy */
        default:
            return &parser; /* Default to legacy */
    }
}

void listParsers(void)
{
    fprintf(stderr, "Available parsers:\n");
    fprintf(stderr, "  parser - Template-based parser\n");
}

ParserType getParserTypeFromString(const char* name)
{
    if (name == NULL) {
        return PARSER_TYPE_LEGACY;
    }
    
    if (strcmp(name, "legacy") == 0) {
        return PARSER_TYPE_LEGACY;
    } else if (strcmp(name, "fsm") == 0) {
        fprintf(stderr, "WARNING: FSM parser disabled due to performance issues. Using parser.\n");
        return PARSER_TYPE_LEGACY;
    }
    
    return PARSER_TYPE_LEGACY; /* Default */
}

const char* getParserName(ParserType type)
{
    ParserInterface* iface = getParser(type);
    return iface ? iface->name : "unknown";
}

