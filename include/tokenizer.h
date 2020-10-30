#pragma once
#include "dictionary.h"

inline std::vector<std::string> cong_stars(const std::vector<std::string>& toks)
{
	std::vector<std::string> res;

	bool flag = false;
	for (size_t index = 0; index < toks.size(); index++)
	{
		if (index == toks.size() - 1 || toks[index] != "*")
		{
			res.push_back(toks[index]);
		}
		else
		{
			
			if (toks[index + 1] == "*")
			{
				res.emplace_back("**");
				index += 1;
			}
			else
			{
				res.emplace_back("*");
			}
		}
	}

	return res;
}


inline std::vector<std::string> tokenize_expression(const std::string& expression)
{
	std::vector<std::string> res;
	token_types prev_token_type = get_char_token_type(expression[0]);

	std::string buff_string;
	
	for (char c : expression)
	{
		token_types this_type = get_char_token_type(c);

		if (this_type != prev_token_type && !buff_string.empty())
		{
			res.push_back(buff_string);
			buff_string.clear();
		}
		
		
		if (is_multi_char(this_type)) {
			buff_string.push_back(c);
		}
		else if (this_type != token_types::space)
		{
			res.push_back({ c });
		}

		
		prev_token_type = this_type;
	}
	if (!buff_string.empty())
		res.push_back(buff_string);

	return res;
}


inline std::vector<std::string> tokenize_full_expression(const std::string& expression)
{
	return cong_stars(tokenize_expression(expression));
}


inline void test_tokenizing()
{
	std::string s = "1.55 + 10.3223 - .1 + 10.*100";


	std::vector<std::string> toks = tokenize_expression(s);

	
	for (auto& val : toks)
	{
		std::cout << val << std::endl;
	}

	return;

	
	std::string s2 = "3**3 + 1 * 89*-** ***2*****";
	std::vector<std::string> toks2 = tokenize_expression(s2);

	auto v = cong_stars(toks2);

	for (auto& vv : v)
		std::cout << vv << std::endl;
}
