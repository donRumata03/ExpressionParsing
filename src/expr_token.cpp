#include "expr_token.h"

#include "parse_utils.h"
#include "validator.h"
#include <tokenizer.h>


expr_token::expr_token(const std::string& s)
{
	auto double_try = to_double(s);

	if (double_try)
	{
		type = expr_token_types::number;
		number = *double_try;
		return;
	}

	if (is_brace(s))
	{
		type = expr_token_types::brace;
		str = s;
		return;
	}

	if (is_primitive_operator(s))
	{
		type = expr_token_types::operation;
		function = string_operator_converter[s];
		return;
	}

	if (is_operation(s))
	{
		type = expr_token_types::function;
		function = string_operator_converter[s];
		return;
	}

	if (is_word(s))
	{
		type = expr_token_types::variable;
		str = s;
		return;
	}

	if (s == ",")
	{
		type = expr_token_types::comma;
		str = ",";
		return;
	}

	type = expr_token_types::unknown;
	str = s;
}

std::vector<expr_token> raw_make_tokens(const std::string& expression)
{
	std::vector<expr_token> res;
	for (auto& s : tokenize_full_expression(expression))
		res.emplace_back(s);

	res = insert_missing_multiplications(res);
	res = process_unary_minuses(res);

	return res;
}

std::optional< std::vector<expr_token> > make_tokens(const std::string& expression)
{
	auto res = raw_make_tokens(expression);
	
	auto arg_number_validation = validate_function_calls(res);

	bool have_no_call_error = !arg_number_validation;

	bool function_braces_are_ok = validate_functions(res);
	bool has_brace_balance = validate_braces(res);
	bool all_operators_have_args = validate_operator_args(res);
	
	bool ok = function_braces_are_ok && has_brace_balance && all_operators_have_args && have_no_call_error;

	if (ok)
		return res;

	return {};
}


std::vector<expr_token> insert_missing_multiplications(const std::vector<expr_token>& toks)
{
	if (toks.empty())
		return {};
	
	std::vector<expr_token> res = { toks[0] };
	
	for (long long i = 1; i < static_cast<long long>(toks.size()); i++) // Add "*" before i`th token
	{
		expr_token_types left_type = toks[i - 1].type, right_type = toks[i].type;
		const auto& left_token = toks[i - 1], right_token = toks[i];
		
		if (right_type != expr_token_types::operation && left_type != expr_token_types::operation && right_type != expr_token_types::comma && left_type != expr_token_types::comma)
		{
			if (right_type == expr_token_types::brace || left_type == expr_token_types::brace)
			{
				if (right_type == left_type) { // Both are braces
					if (!is_brace_opening[left_token.str] && is_brace_opening[right_token.str]) // Add "*" only if there is ")(" -> ") * ("
 					{
						res.emplace_back("*");
					}
				}
				
				// Only one is brace:
				else if (left_type == expr_token_types::brace) // Left is brace
				{
					if (!is_brace_opening[left_token.str]) // Left brace should be closing
					{
						// Second may be function, variable or number
						if (right_type != expr_token_types::number)
						{
							res.emplace_back("*");
						}
					}
				}
				else // Right is brace:
				{
					// Might be function; if function => don`t add anything
					if (left_type != expr_token_types::function && is_brace_opening[right_token.str])
					{
						res.emplace_back("*");
					}
					
				}
			}
			else // No braces
			{
				if (right_type == expr_token_types::number || left_type == expr_token_types::number)
				{
					if (left_type == expr_token_types::number && right_type != expr_token_types::number) // Only {num; not_num} is ok with number without braces
					{
						res.emplace_back("*");
					}
				}
				// Here we only have variables and functions
				// Fuction can`t be followed by anything but brace!
				// Here we have 2 things, so first is var => var + var of var + fun
				// So, insert "*" always!

				else		res.emplace_back("*");
			}
		}

		res.push_back(toks[i]);
	}

	return res;
}


std::ostream& operator<<(std::ostream& out, const expr_token& token)
{
	if (!token.str.empty())
	{
		out << token.str;
	}
	else if (token.type == expr_token_types::number)
	{
		out << token.number;
	}
	else // It`s function or operation
	{
		out << get_function_name[token.function];
	}
	return out;
}

std::ostream& operator<<(std::ostream& out, const std::vector<expr_token>& tokens)
{
	size_t pos = 0;
	for (auto& token : tokens)
	{
		if (pos != 0 && is_primitive_operator(tokens[pos].function)) out << " ";
		out << token;
		if (pos != tokens.size() && is_primitive_operator(tokens[pos].function)) out << " ";

		pos++;
	}
	return out;
}







