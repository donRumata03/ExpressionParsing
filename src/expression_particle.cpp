#include "expression_particle.h"

#include <optional>

#include "parse_utils.h"

#include "final_token.h"

// double expression_segment::compute_function(const std::unordered_set<std::string, double>& variables) const


/*

expression_particle::types expression_particle::get_type(const expression_segment::types& type)
{
	switch (type)
	{
	case expression_segment::types::operation:
		return types::operation;
	case expression_segment::types::number:
		return types::number;
	case expression_segment::types::variable:
		return types::variable;
	default:
		throw std::runtime_error("expression_particle: [Bad type]");
	}
}

*/

bool expression_particle::is_not_complex() const
{
	return type != types::brace_content;
}

expression_particle::expression_particle(const expression_segment& segment)
{
	assert(segment.type != expression_segment::types::brace_content);

	type = types::token;
	token = segment.token;
}

expression_particle::expression_particle(const std::string& s) : expression_particle(*make_tokens(s)) {
	
}

/*
expression_particle::expression_particle(const std::vector<std::string>& tokens)  // NOLINT(cppcoreguidelines-pro-type-member-init)
{
	std::vector<expr_token> res;
	res.reserve(tokens.size());
	for (auto& s : tokens) res.emplace_back(s);

	
	*this = expression_particle(res);
}

*/

expression_particle::expression_particle(const std::vector<expr_token>& tokens)
{
	// Here we only find braces
	std::vector<expression_segment> segments = select_brace_content(tokens);
	
	for(auto& segment : segments)
	{
		if (segment.has_token())
		{
			particles.emplace_back(segment);
		}
		else if (segment.is_function())
		{
			auto& new_function = particles.emplace_back();
			new_function.type = types::function;
			new_function.token = segment.brace_content[0];

			auto args = get_function_args(segment.brace_content, 0);
			new_function.particles.reserve(args.size());
			for (auto& arg : args)
			{
				new_function.particles.emplace_back(arg);
			}
		}
		else // Brace content...
		{
			assert(segment.brace_content.size() > 2 && "Bad size (may be it`s \"()\" or even worse)!");

			std::vector<expr_token> construct_content;
			construct_content.reserve(segment.brace_content.size() - 2);

			for (size_t index = 1; index < segment.brace_content.size() - 1; index ++)
			{
				construct_content.push_back(segment.brace_content[index]);
			}

			auto& new_brace_content = particles.emplace_back(construct_content);
			new_brace_content.type = types::brace_content;

		}
	}
}


/////////////////////////////////////////////////////////////////////////////////////////////


double expression_particle::compute(const std::unordered_map<std::string, double>& variables) const
{
	
	if (type == types::token) {
		assert(token.type == expr_token_types::number || token.type == expr_token_types::variable);

		if(token.type == expr_token_types::number)
		{
			return token.number;
		}
		// Otherwise: it`s a variable! Try to get it.
		auto response = get_variable(variables, token.str);

		if (!response) throw std::runtime_error(std::string("Don`t know the value of the variable \"") + token.str + "\"");

		return *response;
	}

	// Otherwise: count all the brace and function content (get the actual numbers)
	// After that apply the operators of a certain priority in order of the priority
	
	// Launch recursion: get only numbers and operators after this stage!
	// If it`s a brace content, simply get the value of it through recursion, if a function: recursively count all the arguments and invoke the function!
	// If it`s a number: get the number, variable => get the variable;


	// So, we have our particles. Let`s convert them

	
	if (type == types::function) { // Count all arguments
		std::vector<double> args;
		for (auto& particle : particles)
		{
			args.push_back(particle.compute(variables));
		}
		return compute_function(token.function, args);
	}

	// The type is definitely brace_content, so, we fill a new vector by raw numbers and operators

	std::vector<final_token> naked_numbers;
	naked_numbers.reserve(particles.size());

	for (auto& particle : particles)
	{
		if (particle.type == types::token) {
			if (particle.token.type == expr_token_types::operation)
				naked_numbers.emplace_back(particle.token);
			else
				naked_numbers.emplace_back(particle.compute(variables)); // It might be either number of variable
		}
		else {
			naked_numbers.emplace_back(particle.compute(variables));
		}
	}
	
	double answer = apply_all_priorities(naked_numbers);
	return answer;
}

/////////////////////////////////////////////////////////////////////////////////////////////

expression_tree* expression_particle::generate_computational_tree ()
{
	if (type == types::token){
		expr_token_types tt = token.type;

		assert(tt == expr_token_types::number || tt == expr_token_types::variable);

		if (tt == expr_token_types::number) {
			return new expression_tree(token.number);
		}

		// Otherwise: it`s a variable! (or a global constant!)
		if (global_constants.find(token.str) != global_constants.end()) return new expression_tree(global_constants[token.str]);

		return new expression_tree(token.str);
	}

	if (type == types::function) {
		assert(particles.size() == 1 || particles.size() == 2);

		expression_tree* left = nullptr;
		expression_tree* right = nullptr;

		// Anyway make first tree:
		left = particles[0].generate_computational_tree();

		// If required: make the second one:
		if (particles.size() == 2){
			right = particles[1].generate_computational_tree();
		}

		return new expression_tree(
			token.function,
			left,
			right
		);
	}

	// This is a brace content!
	std::vector<std::variant<expression_tree*, operation_char>> trees;
	trees.reserve(particles.size());

	for (auto& particle : particles) {
		if (particle.type == types::token && particle.token.type == expr_token_types::operation) {
			trees.emplace_back(particle.token.function);
		}
		else {
			trees.emplace_back(particle.generate_computational_tree());
		}
	}

	return new expression_tree(trees);
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

flat_expression_tree *expression_particle::generate_flat_expression_tree ()
{
	// TODO?
	return nullptr;
}


