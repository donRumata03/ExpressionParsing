#pragma once

#include "expression_particle.h"
#include "parse_utils.h"
#include "validator.h"

inline void simple_expression_counting()
{
	char c[1000];
	std::cin.getline(c, 100000);
	std::string string = c;

	// std::vector<std::string> toks = tokenize_expression(res);

	try {
		auto tokens = make_tokens(string);
		if (!tokens && tokens->empty() || !get_variables(*tokens).empty())
		{
			std::cout << "WRONG" << std::endl;
		}
		else
		{
			expression_particle p(*tokens);
			std::cout << p.compute() << std::endl;
		}
		
	} catch(...)
	{
		std::cout << "Error!" << std::endl;
		std::cout << "WRONG" << std::endl;
	}

}


struct expression_compiling_result_struct
{
	std::string error_message;
	expression_particle particle;
	bool has_error_message = true;

	expression_compiling_result_struct() = default;
	expression_compiling_result_struct(std::string err) : error_message(std::move(err)), has_error_message(true) {}
	expression_compiling_result_struct(expression_particle _particle) : particle(std::move(_particle)), has_error_message(false) {}
};


inline expression_compiling_result_struct compile_expression(const std::string& expression) {
	
	auto tokens = raw_make_tokens(expression);

	if (tokens.empty())
	{
		return { "Do you really think that you can get something by passing a blank expression?!" };
	}
	// TODO! Replace all assertions by exceptions to catch them and send the messages to the user!!!

	auto strange_symbols_validation = validate_unknown_symbols(tokens);
	if (strange_symbols_validation)
	{
		// We`ve recieved a special string message, return it
		return *strange_symbols_validation;
	}
	
	auto arg_number_validation = validate_function_calls(tokens);
	if (arg_number_validation)
	{
		// We`ve recieved a special string message, return it
		return *arg_number_validation;
	}
	
	bool function_braces_are_ok = validate_functions(tokens);
	if (!function_braces_are_ok)
	{
		return { "Function calls are incorrect. Ensure, that there is an opening round brace after each built-in function!" };
	}
	
	bool has_brace_balance = validate_braces(tokens);
	if (!has_brace_balance)
		return { "I don`t even want to process further inspestions, beacuse the brace sequence is incorrect! :(" };
	
	bool all_operators_have_args = validate_operator_args(tokens);
	if (!all_operators_have_args)
		return { "Unfortunately, not all of the operators in your experssion have arguments or may be there are missing operators." };

	
	
	try {
		expression_particle p(tokens);
		return p;
	}
	catch (std::exception& e)
	{
		return { e.what() };
	}
}


inline std::unique_ptr<expression_tree> make_expression_tree(const std::string& expression) {
	auto compiled_particle = compile_expression(expression);

	if (compiled_particle.has_error_message){
		throw std::runtime_error(compiled_particle.error_message);
	}

	auto res = std::unique_ptr<expression_tree>(compiled_particle.particle.generate_computational_tree());
	res->optimize();

	return res;
}
