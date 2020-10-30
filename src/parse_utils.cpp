#include "parse_utils.h"
#include "expr_token.h"


bool isOnlyDouble(const std::string& str) // Locale shouldn`t be russian
{
	std::string s = str;

	// std::replace(s.begin(), s.end(), '.', ',');
	
	char* endptr = nullptr;
	strtod(s.c_str(), &endptr);

	return *endptr == '\0' && endptr != s;
}

std::optional<double> to_double(const std::string& str) // Locale shouldn`t be russian
{
	std::string s = str;

	// std::replace(s.begin(), s.end(), '.', ',');
	
	auto sstr = s.c_str();
	char* endptr = nullptr;
	double res = strtod(sstr, &endptr);
	bool success = *endptr == '\0' && endptr != sstr;
	return success ? res : std::optional<double>{};
}

std::optional<double> get_variable(const std::unordered_map<std::string, double>& variables, const std::string& variable_name)
{
	if (global_constants.find(variable_name) != global_constants.end()) // It`s a constant!
	{
		return global_constants[variable_name];
	}
	if (variables.find(variable_name) != variables.end())
	{
		return variables.find(variable_name)->second;
	}
	return {};
}

std::vector<std::string> get_variables(const std::vector<expr_token>& data)
{
	std::vector<std::string> all_vars;
	for (auto& el : data)
	{
		if (el.type == expr_token_types::variable) all_vars.push_back(el.str);
	}

	std::unordered_set<std::string> taken_var_set;
	std::vector<std::string> res;

	for (auto& el : all_vars) {
		if (taken_var_set.find(el) == taken_var_set.end())
		{
			res.push_back(el);
			taken_var_set.insert(el);
		}
	}

	return res;
}


std::pair<size_t, size_t> get_brace_positions(const std::vector<expr_token>& data, size_t position, const std::string& brace_type)
{
	std::string opening = get_brace(get_brace_type[brace_type], true);
	std::string closing = get_brace(get_brace_type[brace_type], false);
	while (position != data.size() && data[position].str != opening) position++;
	if (position == data.size()) return { std::string::npos, std::string::npos };

	size_t start_pos = position;

	size_t nesting_counter = 1;
	position++;

	while (nesting_counter != 0 && position < data.size())
	{
		expr_token t = data[position];
		if (t.str == opening)
		{
			nesting_counter++;
		}
		else if (t.str == closing)
		{
			nesting_counter--;
		}
		position++;
	}

	if (position == data.size() && data.back().str != closing)
	{
		position = std::string::npos;
	}

	
	return { start_pos, position };
}


std::vector<expr_token> get_brace_contents(const std::vector<expr_token>& data, const size_t position, const std::string& brace_type)
{
	std::vector<expr_token> res;
	auto ranges_ = get_brace_positions(data, position, brace_type);
	
	if (ranges_.second == std::string::npos || ranges_.first == std::string::npos) return {};

	for (size_t pos = ranges_.first; pos < ranges_.second; pos++)
	{
		res.push_back(data[pos]);
	}

	return res;
}

std::vector<std::vector<expr_token>> get_function_args(const std::vector<expr_token>& toks, const size_t function_pos)
{
	auto brace_content = get_brace_contents(toks, function_pos);

	// We are searching for commas without nesting:
	std::vector<size_t> good_comma_positions;

	long long nesting_counter = -1;
	size_t this_pos = 0;
	for (auto & tok : brace_content)
	{
		if (tok.type == expr_token_types::brace)
		{
			if (is_brace_opening[tok.str])
			{
				nesting_counter++;
			}
			else
			{
				nesting_counter--;
			}
		}
		else
		{
			if (nesting_counter == 0 && tok.type == expr_token_types::comma)
			{
				good_comma_positions.push_back(this_pos);
			}
		}
		this_pos++;
	}
	
	good_comma_positions.push_back(brace_content.size() - 1);
	std::vector<std::vector<expr_token>> res;
	size_t prev_pos = 1;
	for(auto & pos : good_comma_positions)
	{
		auto& this_cont = res.emplace_back();
		for (size_t index = prev_pos; index < pos; index++)
		{
			this_cont.push_back(brace_content[index]);
		}
		
		prev_pos = pos + 1;
	}

	return res;
}




std::vector<expression_segment> select_brace_content(const std::vector<expr_token>& toks)
{
	size_t pos = 0;

	std::vector<expression_segment> res;
	
	while(pos < toks.size())
	{
		auto this_token = toks[pos];
		bool is_opening_brace = this_token.type == expr_token_types::brace && is_brace_opening[this_token.str];
		if (is_opening_brace || this_token.type == expr_token_types::function)  // Need to select brace content:
		{
			auto ranges = get_brace_positions(toks, pos, this_token.str);
			size_t end_index = ranges.second;
			
			auto& new_segment = res.emplace_back();
			new_segment.type = expression_segment::types::brace_content;
			new_segment.brace_content.reserve(end_index - pos + 1);
			
			for (; pos < end_index; pos++)
			{
				new_segment.brace_content.push_back(toks[pos]);
			}
		}
		else // Simply add this token and increment position:
		{
			res.emplace_back(this_token);
			pos++;
		}
	}

	return res;
}

std::vector<expr_token> rasterize_brace_content(const std::vector<expression_segment>& brace_content)
{
	// TODO
	return {};
}


std::vector<expr_token> process_unary_minuses(const std::vector<expr_token>& tokens)
{
	// auto brace_content = select_brace_content(tokens);

	std::vector<expr_token> res;

	for(size_t index = 0; index < tokens.size(); index++)
	{
		auto this_token = tokens.at(index);
		auto this_type = this_token.type;
		if (this_type != expr_token_types::operation || this_token.function != operation_char::substraction)
		{
			res.push_back(this_token);
			continue;
		}
		

		// Detect unary minuses by 2 requirements 

		bool req_prev;
		bool req_next;

		if (index == 0) req_prev = true;
		else
		{
			auto prev_token = tokens[index - 1];
			auto prev_type = prev_token.type;

			if(prev_type == expr_token_types::number || prev_type == expr_token_types::variable)
			{
				req_prev = false;
			}
			else if (prev_type == expr_token_types::brace)
			{
				// req_prev = is_brace_opening[prev_token.str];

				if (is_brace_opening[prev_token.str])
				{
					req_prev = true;
				}
				else
				{
					req_prev = false;
				}
			}
			else
			{
				req_prev = true;
			}
		}

		if (index == tokens.size() - 1) req_next = false; // It`s incorrect, to be honest...
		else
		{
			
			
			auto next_token = tokens[index + 1];
			auto next_type = next_token.type;
			
			if (next_type == expr_token_types::variable || next_type == expr_token_types::function || next_type == expr_token_types::number)
			{
				req_next = true;
			}
			else if (next_type == expr_token_types::brace) {
				req_next = is_brace_opening[next_token.str];
			}
			else
			{
				req_next = false;
			}
		}

		
		if (req_next && req_prev) // It`s an unary minus!
		{
			res.emplace_back("(");
			res.emplace_back("-1");
			res.emplace_back(")");
			res.emplace_back("*");
		}
		else
		{
			res.push_back(this_token);
		}
	}

	
	// Don`t believe comments below! We don`t need recursion here!!!

	// Check for this unary minus
	// And launch recursion!

	// TODO: add braces around the operand of the unary minus in order to prevent bugs with power operation!
	// Do it only if this is AFTER power operation, because:
	// -3^2 = -(3^2)
	// but
	// 3^-2 = 3^(-2)
	

	return res;
}


