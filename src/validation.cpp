#include "validator.h"
#include <stack>
#include "parse_utils.h"

bool validate_functions(const std::vector<expr_token>& toks)
{
	for (long long index = 0; index < static_cast<long long>(toks.size()) - 1; index++)
	{
		auto& left = toks[index];
		auto& right = toks[index + 1];

		if (left.type == expr_token_types::function && right.str != "(") return false;
	}
	
	return true;
}

bool validate_braces(const std::vector<expr_token>& toks)
{
	std::stack<std::string> s;

	for (auto& tok : toks)
	{
		if (tok.type == expr_token_types::brace)
		{
			bool is_opening = is_brace_opening[tok.str];
			if (s.empty())
			{
				s.push(tok.str);
				if (!is_opening)
					return false;
				
				continue;
			}
			// So, here is something on the top of stack:
			std::string prev = s.top();

			// bool prev_opening = is_brace_opening[prev];

			bool same_types = get_brace_type[tok.str] == get_brace_type[prev];
			
			if (same_types)
			{
				if (is_opening)
				{
					s.push(tok.str);
				}
				else
				{
					s.pop();
				}
			}
			else
			{
				if (is_opening)
				{
					s.push(tok.str);
				}
				else
				{
					return false;
				}
			}
		}
	}
	
	return s.empty();
}

std::optional<std::string> validate_function_calls(const std::vector<expr_token>& toks)
{
	/*
	auto segments = select_brace_content(toks);
	for (auto& segment : segments)
	{
		if (segment.type ==	expression_segment::types::brace_content && !segment.brace_content.empty() && segment.brace_content[0].type == expr_token_types::function)
		{
			// Select brace content of function:
			
		}
	}
	*/
	size_t pos = 0;
	for (auto& token : toks)
	{
		if (token.type == expr_token_types::function)
		{
			// Get brace content of the function:
			auto brace_content = get_brace_contents(toks, pos);
			// We don`t need all commas, only the ones, that are not nested.

			auto args = get_function_args(toks, pos);

			if (args.size() != operation_parameter_number[token.function])
			{
				std::string message_start = "There is an incorrect number of arguments in function \"";
				return message_start + get_function_name[token.function] + "\": required: " + std::to_string(operation_parameter_number[token.function]) + "; have: " + std::to_string(args.size()) + "!";
			}
			/*
			auto sel_content = select_brace_content(brace_content);
			size_t args_found = 0;
			for (auto& unit : sel_content)
			{
				if (unit.type == expression_segment::types::comma && unit.token.str == ",")
				{
					args_found++;
				}
			}
			if (args_found != operation_parameter_number[token.function]) return false;
			*/
		}
		pos++;
	}

	return {};
}

bool validate_operator_args(const std::vector<expr_token>& toks)
{
	auto selected = select_brace_content(toks);
	size_t size = selected.size();
	size_t pos = 0;
	for (auto& segment : selected)
	{
		if (segment.type == expression_segment::types::operation) // So, there should be non-operations around it; moreover, they shouldn`t be iat the beginig or ending
		{
			if (pos == 0 || selected[pos - 1].type == expression_segment::types::operation)
			{
				return false;
			}
			if (pos == size - 1 || selected[pos + 1].type == expression_segment::types::operation)
			{
				return false;
			}
		}
		else
		{
			if (pos != 0 && selected[pos - 1].type != expression_segment::types::operation)
			{
				return false;
			}
			if (pos != size - 1 && selected[pos + 1].type != expression_segment::types::operation)
			{
				return false;
			}
		}
		pos++;
	}
	
	// Now launch recursion:

	for (auto& segment : selected)
	{
		if(segment.type == expression_segment::types::brace_content)
		{
			if (segment.brace_content.size() == 2)
			{
				return false; // Braces can`t be empty!
				
				// throw std::runtime_error("Braces can`t be empty!");
			}
			
			if (segment.brace_content.front().type == expr_token_types::function) // It`s a function call!
			{
				auto args = get_function_args(segment.brace_content, 0);
				for (auto& arg : args)
				{
					if (!validate_operator_args(arg)) return false;
				}
			}
			else // It`s a simple brace expression
			{
				std::vector<expr_token> to_validate;
				to_validate.reserve(segment.brace_content.size() - 2);

				for(size_t p = 1; p < segment.brace_content.size() - 1; p++)
				{
					to_validate.emplace_back(segment.brace_content[p]);
				}

				//TodO: add ranges as args to prevent copying!!!
				// (not necessary...)

				if (!validate_operator_args(to_validate))
				{
					return false;
				}
			}
		}
	}

	
	return true;
}

std::optional<std::string> validate_unknown_symbols(const std::vector<expr_token>& toks)
{
	for (auto& s : toks)
	{
		if (s.type == expr_token_types::unknown)
		{
			return "There is an unknown symbol in your expression! Here it is: \"" + s.str + "\"!";
		}
	}
	return {};
}


