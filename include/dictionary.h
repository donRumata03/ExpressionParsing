#pragma once

#include <algorithm>
#include <iostream>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include <unordered_set>
#include "_letter_utils.h"


////////////////////////////////////		Braces				///////////////////////////////////////////////////

enum class brace_types
{
	round,
	square,
	curly
};

inline std::unordered_map<std::string, brace_types> get_brace_type = {
	{ "(", brace_types::round },
	{ ")", brace_types::round },
	{ "[", brace_types::square },
	{ "]", brace_types::square },
	{ "{", brace_types::curly },
	{ "}", brace_types::curly },
};
inline bool is_brace(const std::string& s) { return get_brace_type.find(s) != get_brace_type.end(); }

inline std::unordered_map<std::string, bool> is_brace_opening = {
	{ "(", true },
	{ ")", false },
	{ "[", true },
	{ "]", false },
	{ "{", true },
	{ "}", false },
};

inline std::string get_brace(const brace_types& type, const bool opening)
{
	switch (type)
	{
	case brace_types::round:
		return opening ? "(" : ")";
	case brace_types::square:
		return opening ? "[" : "]";
	case brace_types::curly:
		return opening ? "{" : "}";
	}
	return {};
}


////////////////////////////////////////////					Operations			////////////////////////////////////////////////

enum class operation_char
{
	involution,
	rootation_2,
	rootation_3,

	multiplication,
	division,

	addition,
	substraction,

	absation,
	mination,
	maxation,
	sgntion,
	
	logariphmation,
	logariphmation_10,
	natural_logariphmation,
	exponentiation,
	rootation_n,

	sinusation,
	cosinusation,
	tangensation,
	cotangensation,

	arcsinusation,
	arccosinusation,
	arctangensation,
	arccotangensation,
};

std::ostream &operator<< (std::ostream &os, const operation_char& operation);


inline std::unordered_set<std::string> string_basic_operators = \
{
	"*",
	"**",
	"-",
	"–",
	"+",
	"/",
	"^",
};

inline std::unordered_set<operation_char> basic_operators = {
	operation_char::addition,
	operation_char::substraction,

	operation_char::multiplication,
	operation_char::division,
	
	operation_char::involution
};

inline bool is_primitive_operator(const std::string& s)
{	
	return string_basic_operators.find(s) != string_basic_operators.end();
}

inline bool is_primitive_operator(const operation_char& c)
{
	return basic_operators.find(c) != basic_operators.end();
}

extern std::unordered_map<operation_char, size_t> operator_priorities;
extern std::unordered_map<operation_char, size_t> operation_parameter_number;
extern std::unordered_map<std::string, operation_char> string_operator_converter;
extern std::unordered_map<operation_char, std::string> get_function_name;

inline bool is_operation(const std::string& s) { return string_operator_converter.find(s) != string_operator_converter.end(); }


double compute_function(const operation_char& c, const std::vector<double>& args);


///////////////////////////////////////				Constants				////////////////////////////

extern std::unordered_map<std::string, double> global_constants;

///////////////////////////////////////				tokenization				////////////////////////////

enum class token_types
{
    number,
    // built_in_function,
    // variable
    word,
    brace,
    operator_char,
    space,
    unknown
};

token_types get_char_token_type(char c);

inline bool is_multi_char(const token_types& type)
{
	return type == token_types::word || type == token_types::number;
}


inline bool is_word(const std::string& s)
{
	return std::all_of(s.begin(), s.end(), [](const char c){ return is_letter_(c) || c == '_'; });
}

