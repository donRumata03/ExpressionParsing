#include "final_token.h"

#include <set>

final_token::final_token(const expression_particle& expr_part)
{
	*this = to_final_token(expr_part);
}

final_token::final_token(const double num)
{
	type = types::number;
	number = num;
}

final_token::final_token(const expr_token& token)
{
	assert(token.type == expr_token_types::operation || token.type == expr_token_types::number);

	if (token.type == expr_token_types::operation) {
		type = types::operation;
		operation = token.function;
	}
	else
	{
		type = types::number;
		number = token.number;
	}
}

final_token to_final_token(const expression_particle& token)
{
	assert(token.is_not_complex() && (token.token.type == expr_token_types::number || token.token.type == expr_token_types::operation));
	auto tt = token.token;
	if (tt.type == expr_token_types::number)
	{
		final_token f;
		f.number = tt.number;
		f.type = final_token::types::number;
		return f;
	}
	if (tt.type == expr_token_types::operation)
	{
		final_token f = {};
		f.operation = tt.function;
		f.type = final_token::types::operation;
		return f;
	}

	return {};
}


std::vector<final_token> to_final_tokens(const std::vector<expression_particle>& tokens)
{
	std::vector<final_token> res;
	res.reserve(tokens.size());
	for (auto& tok : tokens) res.emplace_back(tok);
	return res;
}

std::vector<std::vector<final_token>> group_by_priority(const std::vector<final_token>& tokens, const size_t priority)
{
	std::vector<std::vector<final_token>> res;
	// res.reserve(std::count_if(tokens.begin(), tokens.end(), [p](){}));
	res.push_back({ tokens[0] });

	if (tokens.size() == 1) return res;
	
	bool in_expression = tokens[1].type == final_token::types::operation && operator_priorities[tokens[1].operation] == priority;
	
	for (size_t index = 1; index < tokens.size() - 1; index ++)
	{
		auto& token = tokens[index];
		auto& next_token = tokens[index + 1];

		if (!in_expression) {
			// If this one is not an operator, the next one definetely is, so don`t even check
			if (token.type == final_token::types::operation || operator_priorities[next_token.operation] != priority)
			{
				res.back().push_back(token);
			}
			else
			{
				// So, this is a number and the next one is an operator with reuired priority => create a new container with required stuff
				res.push_back({ token });
				in_expression = true;
			}
		}
		else
		{
			if (token.type == final_token::types::operation && operator_priorities[token.operation] != priority)
			{
				res.push_back({ token });
				in_expression = false;
			}
			else
			{
				res.back().push_back(token);
			}
		}
	}
	res.back().push_back(tokens.back());
	
	return res; // TODO: return pair of data and descriptor!
}


std::vector<final_token> apply_priority(const std::vector<final_token>& tokens, const size_t priority)
{
	std::vector<final_token> res;
	res.reserve(tokens.size());
	
	auto tok_sets = group_by_priority(tokens, priority);

	for (auto& ts : tok_sets)
	{
		// Determine the type:
		bool to_compute = std::any_of(ts.begin(), ts.end(), [&](const final_token& tok) { return tok.type == final_token::types::operation && operator_priorities[tok.operation] == priority; });
		if (to_compute)
		{
			// TODO! Have left association for "^" and "**" !!!
			double number = ts[0].number;
			for(size_t index = 1; index < ts.size(); index += 2)
			{
				number = compute_function(ts[index].operation, { number, ts[index + 1].number });
			}
			res.emplace_back(number);
		}
		else
		{
			for(auto &token : ts)
			{
				res.emplace_back(token);
			}
		}
	}
	
	/*
	double last_number = tokens[0].number;
	operation_char last_operator = operation_char::addition;

	bool has_number = false;
	bool has_operator = false;

	size_t index = 0;
	for (auto& token : tokens)
	{
		if (!has_number)
		{
			if (token.type ==  final_token::types::number)
				last_number = token.number;
			has_number = true;
		}
		else
		{
			if (token.type == final_token::types::operation)
				last_operator = token.operation;
			
		}
		index++;
	}
	*/
	return res;
}

double apply_all_priorities(const std::vector<final_token>& toks)
{
	std::set<size_t> priorities_set;
	for(auto& t : toks)
	{
		if (t.type == final_token::types::operation)
			priorities_set.insert(operator_priorities[t.operation]);
	}

	auto& this_toks = const_cast<std::vector<final_token>&>(toks);

	std::vector<size_t> priorities;
	for (auto& v : priorities_set) priorities.push_back(v);
	std::reverse(priorities.begin(), priorities.end());
	
	for (auto& p : priorities)
	{
		this_toks = apply_priority(this_toks, p);
	}

	return this_toks[0].number;
}

