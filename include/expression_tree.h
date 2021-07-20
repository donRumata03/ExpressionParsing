//
// Created by Vova on 08.06.2020.
//

#pragma once

#include "expr_token.h"
#include <variant>
#include <ostream>
#include <chrono>


struct expression_tree
{
	enum class types {
		number,
		variable,
		operation
	};

	types type = types::number;

	double number = 0;
	std::string variable_name;
	operation_char m_operation = operation_char::addition;

	expression_tree* left = nullptr;
	expression_tree* right = nullptr;


	expression_tree() = default;

	explicit expression_tree (double number);
	explicit expression_tree (std::string variable_name);
	explicit expression_tree (operation_char m_operation, expression_tree *left, expression_tree *right);

	explicit expression_tree (operation_char m_operation, const std::string& variable_name, expression_tree *right);
	explicit expression_tree (operation_char m_operation, double number, expression_tree *right);
	explicit expression_tree (operation_char m_operation, expression_tree* left, const std::string& variable_name);
	explicit expression_tree (operation_char m_operation, expression_tree* left, double number);

	explicit expression_tree (const std::vector<std::variant<expression_tree *, operation_char>> &);


	expression_tree(expression_tree&& other) noexcept;
	expression_tree(const expression_tree& other);

	expression_tree& operator= (const expression_tree& other);
	expression_tree& operator= (expression_tree&& other) noexcept ;


	double compute(const std::unordered_map<std::string, double>& variable_values = {});

	void optimize();
	bool requires_variables() const;

	void reset_cache();
	[[nodiscard]] std::unordered_set<std::string> get_variables() const;

	[[nodiscard]] size_t count_node_number() const;
	[[nodiscard]] size_t count_operation_number() const;
	[[nodiscard]] double count_collective_size() const;
	// TODO: [[nodiscard]] std::chrono::nanoseconds estimate_computation_time() const;
	//  (Count really (what arguments to give?…) or just sum all operation times up (known from table ==> make table))

	[[nodiscard]] expression_tree * generate_derivative_tree(const std::string& diff_variable_name);


	friend std::ostream &operator<< (std::ostream &os, const expression_tree &tree);

	~expression_tree();

private:
	mutable bool has_temp = false;
	mutable bool requires_vars_temp = false;

	[[nodiscard]] std::vector<double> make_args(const std::unordered_map<std::string, double>& variable_values) const;
	expression_tree *multiply_by_arg_derivative (const std::string &diff_variable_name, expression_tree *derivative_expression) const;
};


std::ostream& operator<< (std::ostream& os, const expression_tree::types& type);
