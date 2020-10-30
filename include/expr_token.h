#pragma once
#include <optional>
#include "dictionary.h"

enum class expr_token_types
{
	number,
	brace,
	operation,
	function,
	variable,
	comma,
	
	unknown
};

struct expr_token
{
	expr_token_types type = expr_token_types::unknown;

	double number = 0;
	std::string str;
	operation_char function = operation_char::addition;
	
	explicit expr_token(const std::string& s);

	
	expr_token() = default;
	~expr_token() = default;

	expr_token(expr_token&& other) = default;
	expr_token(const expr_token& other) = default;

	expr_token& operator= (const expr_token& other) = default;
	expr_token& operator= (expr_token&& other) = default;
	
};

std::vector<expr_token> raw_make_tokens(const std::string& expression);
std::optional<std::vector<expr_token>> make_tokens(const std::string& expression);


std::vector<expr_token> insert_missing_multiplications(const std::vector<expr_token>& toks);
// std::vector<expr_token> process_unary_minuses(const std::vector<expr_token>& tokens);

std::ostream& operator <<(std::ostream& out, const expr_token& token);

std::ostream& operator <<(std::ostream& out, const std::vector<expr_token>& tokens);
