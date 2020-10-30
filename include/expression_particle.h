#pragma once
#include <optional>
#include <string>
#include <vector>

#include "tokenizer.h"
#include "expr_token.h"
#include "expression_segment.h"

#include "expression_tree.h"
#include "flat_expression_tree.h"


struct expression_particle
{
	enum class types
	{
		token,					// Something simple
		function,				// Built-in function, this type of particle also contains some other particles (according to the number of function arguments)
		brace_content			// Simple brace content (its "particles" contain a set of expression_segments)
	};
	// types get_type(const expression_segment::types& type);

	[[nodiscard]] bool is_not_complex() const;
	
	types type = types::brace_content;
	expr_token token;

	std::vector<expression_particle> particles; // Could be an expression, but if it`s a function, it`s a list of arguments

	expression_particle() = default;
	explicit expression_particle(const expression_segment& segment);
	
	explicit expression_particle(const std::string& s);
	// explicit expression_particle(const std::vector<std::string>& tokens);
	
	explicit expression_particle(const std::vector<expr_token>& tokens);

	[[nodiscard]] double compute(const std::unordered_map<std::string, double>& variables = {}) const;

	expression_tree* generate_computational_tree ();
	flat_expression_tree* generate_flat_expression_tree();
};
