#pragma once

// ---------------------------------------------------------------------------|
// Boost Includes
// ---------------------------------------------------------------------------|
#include <boost/variant/recursive_variant.hpp>
#include <boost/fusion/include/adapt_struct.hpp>

// ---------------------------------------------------------------------------|
// Standard Includes
// ---------------------------------------------------------------------------|
#include <string>
#include <vector>

// ---------------------------------------------------------------------------|
/**
 * An example spirit parser for JSON Syntax:
 *    JsonObject 	= 	"{" member ("," member)* "}"
 *    JsonMember 	= 	string ":" value
 *    string 	   = 	'"' character* '"'
 *    value 	   = 	string | number | object | array | "true" | "false" | "null"
 *    number 	   = 	integer | real
 *    JsonArray 	= 	"[" value ("," value)* "]"
 *    character 	= 	[a-zA-Z]
 */
