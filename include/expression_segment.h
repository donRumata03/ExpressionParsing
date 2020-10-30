#pragma once
#include <utility>
#include <variant>
#include <vector>

#include "expr_token.h"
#include <cassert>

// using expression_segment = std::variant<std::vector<expr_token>, expr_token>;





struct expression_segment // Part of partially tokenized expression for example: 1 + 8 *10 + (89 / 4341 + 13) - 7 ==> 1 + 8 *10 + (...) - 7
{
	enum class types
	{
		operation,
		comma,
		
		// Statements:
		
		brace_content, // Function or simple braces
		number,
		variable,
	};

	[[nodiscard]] bool has_token() const;
	[[nodiscard]] bool is_function() const;
	
	static types get_type(const expr_token_types& token_type)
	{
		switch (token_type)
		{
			
		case expr_token_types::operation:
			return types::operation;
		case expr_token_types::variable:
			return types::variable;
		case expr_token_types::number:
			return types::number;
		case expr_token_types::comma:
			return types::comma;
		default:
			throw std::runtime_error("[[expression_segment::get_type]] : \"Bad token type!\"");
		}
	}

	types type = types::number;

	std::vector<expr_token> brace_content;
	expr_token token;

	expression_segment() = default;

	explicit expression_segment(std::vector<expr_token> tokens) : type(types::brace_content), brace_content(std::move(tokens)), token{} {}

	explicit expression_segment(const expr_token& tok)
	{
		token = tok;
		type = get_type(tok.type);
	}

	
	~expression_segment() = default;
	expression_segment(const expression_segment& other) = default;
	expression_segment(expression_segment&& other) = default;

	expression_segment& operator= (const expression_segment& other) = default;
	expression_segment& operator= (expression_segment&& other) = default;
	
	// [[nodiscard]] double compute_function(const std::unordered_set<std::string, double>& variables) const;
};


inline bool expression_segment::has_token() const
{
	return type != types::brace_content;
}

inline bool expression_segment::is_function() const
{
	assert(type == types::brace_content);
	return !brace_content.empty() && brace_content[0].type == expr_token_types::function;
}
