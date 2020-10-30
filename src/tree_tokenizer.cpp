//
// Created by Vova on 11.06.2020.
//

#include "tree_tokenizer.h"

/*
void clear_tokens(tree_tokens& tokens) {
	for (auto& token : tokens) {
		if (std::holds_alternative<expression_tree*>(token)) {
			auto* tree = ;
		}
	}
}
*/

tree_tokens apply_priority (std::vector<std::variant<expression_tree *, operation_char>> &expression, const size_t priority)
{
	auto grouped = group_by_priority(expression, priority);



	tree_tokens res;
	res.reserve(expression.size());

	for (auto group : grouped) {
		// Determine the type:
		bool to_compute = std::any_of(group.begin(), group.end(), [&](const auto& tok) {
			return std::holds_alternative<operation_char>(tok) && operator_priorities[std::get<operation_char>(tok)] == priority;
		});

		if (to_compute)
		{
			// TODO! Have left association for "^" and "**" !!!
			auto* this_tree = std::get<expression_tree*>(group[0]);
			for(size_t index = 1; index < group.size(); index += 2)
			{
				auto current_operation = std::get<operation_char>(group[index]);
				this_tree = new expression_tree(current_operation, this_tree, std::get<expression_tree*>(group[index + 1]));
			}
			res.emplace_back(this_tree);
		}
		else
		{
			for(auto &token : group)
			{
				res.emplace_back(token);
			}
		}
	}

	return res;
}

std::vector<tree_tokens> group_by_priority (std::vector<std::variant<expression_tree *, operation_char>> &expression, size_t priority)
{
	std::vector<tree_tokens> res;

	res.push_back({ expression[0] });

	if (expression.size() == 1) return res;

	bool in_expression = std::holds_alternative<operation_char>(expression[1]) && operator_priorities[std::get<operation_char>(expression[1])] == priority;

	for (size_t index = 1; index < expression.size() - 1; ++index) {
		auto& token = expression[index];
		auto& next_token = expression[index + 1];
		if (!in_expression) {
			// If this one is not an operator, the next one definetely is, so don`t even check
			if (std::holds_alternative<operation_char>(token) || operator_priorities[std::get<operation_char>(next_token)] != priority)
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
			if (std::holds_alternative<operation_char>(token) && operator_priorities[std::get<operation_char>(token)] != priority)
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
	res.back().emplace_back(expression.back());

	return res;
}

