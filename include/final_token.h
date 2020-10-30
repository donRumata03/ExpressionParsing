#pragma once

#include <vector>
#include "dictionary.h"
#include "expression_particle.h"

struct final_token
{
	enum class types
	{
		number,
		operation
	};
	types type = types::number;
	
	double number = 0;
	operation_char operation = operation_char::addition;

	final_token() = default;
	explicit final_token(const expression_particle& expr_part);
	explicit final_token(double num);
	explicit final_token(const expr_token& token);
	
};

final_token to_final_token(const expression_particle& token);
std::vector<final_token> to_final_tokens(const std::vector<expression_particle>& tokens);

std::vector<std::vector<final_token>> group_by_priority(const std::vector<final_token> &tokens, size_t priority);
std::vector<final_token> apply_priority(const std::vector<final_token>& tokens, const size_t priority);

double apply_all_priorities(const std::vector<final_token>& toks);
