//
// Created by Vova on 08.06.2020.
//

#include <expression_tree.h>
#include <cassert>
#include <utility>
#include <parse_utils.h>
#include <set>

#include "tree_tokenizer.h"



double expression_tree::compute (const std::unordered_map<std::string, double>& variable_values)
{
	using namespace std::string_literals;


	if (type == types::number) return number;

	else if (type == types::variable) {
		auto res = get_variable(variable_values, variable_name);
		if (!res) throw std::runtime_error("Unknown variable: "s + variable_name);

		return *res;
	}

	std::vector<double> function_args = make_args(variable_values);

	return compute_function(m_operation, function_args);
}

bool expression_tree::requires_variables () const
{
	if (has_temp) return requires_vars_temp;

	if (type == types::variable) {
		has_temp = true;
		requires_vars_temp = true;

		return true;
	}
	if (type == types::number) {
		has_temp = true;
		requires_vars_temp = false;

		return false;
	}

	bool res = false;
	if (left && left->requires_variables()) res = true;
	else if (right && right->requires_variables()) res = true;


	requires_vars_temp = res;
	has_temp = true;

	return res;
}

void expression_tree::optimize ()
{
	if (requires_variables()) {
		if (type == types::variable) return;

		// Type is operation => launch optimization where possible:

		if (left) left->optimize();
		if (right) right->optimize();

		return;
	}

	if (type == types::number) return;

	/// And here the optimization happens:

	number = compute_function(m_operation, make_args({}));

	delete left;
	delete right;

	left = nullptr;
	right = nullptr;

	type = types::number;
}

expression_tree::~expression_tree ()
{
	delete left;
	delete right;
/*
	left = nullptr;
	right = nullptr;
*/

}

std::vector<double> expression_tree::make_args (const std::unordered_map<std::string, double>& variable_values) const
{
	assert(left || right);

	std::vector<double> res;

	if (left) res.push_back(left->compute(variable_values));
	if (right) res.push_back(right->compute(variable_values));

	return res;
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

expression_tree::expression_tree (double number) : number(number)
{ type = types::number; }

expression_tree::expression_tree (std::string variable_name) : variable_name(std::move(variable_name))
{ type = types::variable; }

expression_tree::expression_tree (operation_char m_operation, expression_tree *left,
                                  expression_tree *right) : m_operation(m_operation), left(left),
                                                            right(right)
{ type = types::operation; }

expression_tree::expression_tree (operation_char m_operation, double number, expression_tree *right) :
				m_operation(m_operation),
				left(new expression_tree(number)), right(right),
				type(types::operation)
{}

expression_tree::expression_tree (operation_char m_operation, const std::string& variable_name, expression_tree *right) :
		m_operation(m_operation),
		left(new expression_tree(variable_name)), right(right),
		type(types::operation)
{}

expression_tree::expression_tree (operation_char m_operation, expression_tree *left, double number) : expression_tree(m_operation, left, new expression_tree(number))
{}

expression_tree::expression_tree (operation_char m_operation, expression_tree *left, const std::string& variable_name) : expression_tree(m_operation, left, new expression_tree(variable_name))
{}



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

expression_tree::expression_tree (const std::vector<std::variant<expression_tree*, operation_char>> & expression)
{
	std::set<size_t> priorities_set;
	for(auto& t : expression)
	{
		if (std::holds_alternative<operation_char>(t))
			priorities_set.insert(operator_priorities[std::get<operation_char>(t)]);
	}

	std::vector<size_t> priorities;
	for (auto& v : priorities_set) priorities.push_back(v);
	std::reverse(priorities.begin(), priorities.end());

	auto this_toks = expression;

	for (auto& p : priorities)
	{
		this_toks = apply_priority(this_toks, p);
	}


	expression_tree& this_tree = *std::get<expression_tree*>(this_toks[0]);

	*this = std::move(this_tree);
}

/////////////////////////////////////////////////////////////////////////////////////////////////

expression_tree::expression_tree (expression_tree &&other) noexcept :
												type(other.type), m_operation(other.m_operation), number(other.number), variable_name(std::move(other.variable_name)),
												has_temp(other.has_temp), requires_vars_temp(other.requires_vars_temp),
												left(other.left), right(other.right)
{
	other.left = nullptr;
	other.right = nullptr;

	// std::cout << "Performed move constructor!" << std::endl;
}

expression_tree::expression_tree (const expression_tree &other) : type(other.type), m_operation(other.m_operation), number(other.number),
																	variable_name(other.variable_name),
                                                                  has_temp(other.has_temp), requires_vars_temp(other.requires_vars_temp)
{
	if (other.left) left = new expression_tree(*other.left);
	if (other.right) right = new expression_tree(*other.right);

	// std::cout << "Performed copy constructor!" << std::endl;
}

expression_tree &expression_tree::operator= (const expression_tree &other)
{
	if (this != &other) {
		variable_name = other.variable_name;
		requires_vars_temp = other.requires_vars_temp;
		has_temp = other.has_temp;
		number = other.number;
		m_operation = other.m_operation;

		type = other.type;


		delete left;
		delete right;

		if (other.left) left = new expression_tree(*other.left);
		if (other.right) right = new expression_tree(*other.right);

		// std::cout << "Performed copy operator!" << std::endl;
	}

	return *this;
}

expression_tree &expression_tree::operator= (expression_tree &&other) noexcept {
	if (this != &other) {
		variable_name = std::move(other.variable_name);
		requires_vars_temp = other.requires_vars_temp;
		has_temp = other.has_temp;
		number = other.number;
		m_operation = other.m_operation;

		type = other.type;


		delete left;
		delete right;

		left = other.left;
		right = other.right;

		other.left = nullptr;
		other.right = nullptr;

		// std::cout << "Performed move operator!" << std::endl;
	}

	return *this;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////

std::ostream &operator<< (std::ostream &os, const expression_tree &tree)
{
	os << std::boolalpha << "Expression tree: { " << "type: " << tree.type << "; number: " << tree.number << "; variable_name: " << tree.variable_name
	   << "; m_operation: " << tree.m_operation << ";";
	if (tree.left) os << " left: { " << *tree.left << " }; ";
	if (tree.right) os << " right: { " << *tree.right << " }; ";
	os << " }";
	return os;
}

expression_tree * expression_tree::generate_derivative_tree (const std::string& diff_variable_name)
{
	optimize();

	if (type == types::variable) {
		return (variable_name == diff_variable_name) ? new expression_tree(1) : new expression_tree(0);
	}
	if (type == types::number) {
		return new expression_tree(0);
	}

	// Type is an operation

	#define require_one_argument() \
		if (!(bool(right) ^ bool(left))) \
			throw std::runtime_error("There should exist exactly one of the left and right nodes!"); \
		expression_tree *the_ptr = nullptr; \
		if (left) the_ptr = left; \
		if (right) the_ptr = right;

	switch (m_operation) {
		case operation_char::addition:
			return new expression_tree(operation_char::addition, left->generate_derivative_tree(diff_variable_name), right->generate_derivative_tree(diff_variable_name));

		case operation_char::substraction:
			return new expression_tree(operation_char::substraction, left->generate_derivative_tree(diff_variable_name), right->generate_derivative_tree(diff_variable_name));

		case operation_char::multiplication: {
			assert(right && left);

			auto *p1 = new expression_tree(operation_char::multiplication, new expression_tree(*left),
			                               right->generate_derivative_tree(diff_variable_name));
			auto *p2 = new expression_tree(operation_char::multiplication,
			                               left->generate_derivative_tree(diff_variable_name),
			                               new expression_tree(*right));
			return new expression_tree(operation_char::addition, p1, p2);
		}

		case operation_char::division: {
			auto *denominator_copy = new expression_tree(*right);
			auto *second_denominator_copy = new expression_tree(*right);

			auto *numerator_deriv = left->generate_derivative_tree(diff_variable_name);
			auto *denominator_deriv = right->generate_derivative_tree(diff_variable_name);

			auto* p1 = new expression_tree(operation_char::multiplication, numerator_deriv, new expression_tree(*right));
			auto* p2 = new expression_tree(operation_char::multiplication, denominator_deriv, new expression_tree(*left));
			auto* res_numerator = new expression_tree(operation_char::substraction, p1, p2);

			auto* res_denominator = new expression_tree(operation_char::multiplication, denominator_copy, second_denominator_copy);

			return new expression_tree(operation_char::division, res_numerator, res_denominator);
		}

		case operation_char::absation:
		{
			// abs(x)' = x / abs(x)
			/// abs(x)' = sgn(x)
/*

			// if ((!right && !left) || (right && left))
			if (!(bool(right) ^ bool(left)))
				throw std::runtime_error("There should exist exactly one of the left and right nodes!");

			expression_tree* the_copy = nullptr;
			if (left) the_copy = new expression_tree(*left);
			if (right) the_copy = new expression_tree(*right);


			auto* sgn_tree = new expression_tree(operation_char::division,
					the_copy,
					new expression_tree(*this));
					// new expression_tree(operation_char::absation, new expression_tree(*the_copy), nullptr)); // Or simply new expression_tree(*this);

			return new expression_tree(operation_char::multiplication, the_copy->generate_derivative_tree(diff_variable_name), sgn_tree);
*/
			require_one_argument()

			return multiply_by_arg_derivative(diff_variable_name,
					new expression_tree(operation_char::sgntion, new expression_tree(*the_ptr), nullptr));
		}

		case operation_char::mination: {
			/// min(a, b) = (a + b - abs(a - b)) / 2


			auto eq_tree = std::make_unique<expression_tree>(operation_char::division,
			                                                 new expression_tree(
									operation_char::substraction,
									new expression_tree(operation_char::addition, new expression_tree(*left), new expression_tree(*right)),
									new expression_tree(
											operation_char::absation,
											new expression_tree(operation_char::substraction, new expression_tree(*left), new expression_tree(*right)),
											nullptr)
											), 2);

			return eq_tree->generate_derivative_tree(diff_variable_name);
		}

		case operation_char::maxation: {
			/// max(a, b) = (a + b + abs(a - b)) / 2

			auto eq_tree = std::make_unique<expression_tree>(operation_char::division,
			                                                 new expression_tree(
					                                                 operation_char::addition,
					                                                 new expression_tree(operation_char::addition, new expression_tree(*left), new expression_tree(*right)),
					                                                 new expression_tree(
							                                                 operation_char::absation,
							                                                 new expression_tree(operation_char::substraction, new expression_tree(*left), new expression_tree(*right)),
							                                                 nullptr)
			                                                 ), 2);

			return eq_tree->generate_derivative_tree(diff_variable_name);
		}

		case operation_char::sgntion: {
			return new expression_tree(0);
		}


		case operation_char::involution: {
			// Check if the exponent is an integer constant

			if (right->type == types::number) {
				auto exponent = static_cast<long long>(std::round(right->compute({})));

				if (double(exponent) == right->compute({})) {


					return new expression_tree(operation_char::multiplication,
					                           left->generate_derivative_tree(diff_variable_name),
					                           new expression_tree(operation_char::multiplication, double(exponent),
					                                               new expression_tree(operation_char::involution,
					                                                                   new expression_tree(*left),
					                                                                   new expression_tree(
							                                                                   operation_char::substraction,
							                                                                   new expression_tree(
									                                                                   double(exponent)),
							                                                                   1.))));
				}
			}

			auto log_tree = std::make_unique<expression_tree>(operation_char::multiplication, new expression_tree(*right),
					new expression_tree(operation_char::natural_logariphmation, new expression_tree(*left), nullptr));

			return new expression_tree(operation_char::multiplication, new expression_tree(*this), log_tree->generate_derivative_tree(diff_variable_name));
		}

		case operation_char::rootation_2: {
			// sqrt(x)' = 1 / (2 * sqrt(x))
			require_one_argument()

			return multiply_by_arg_derivative(diff_variable_name, new expression_tree(operation_char::division, 1.,
					new expression_tree(operation_char::multiplication, 2., new expression_tree(operation_char::rootation_2,
							new expression_tree(*the_ptr), nullptr))
			));
		}

		case operation_char::rootation_3: {
			// cbrt(x)' = 1 / (3 * cbrt(x^2))
			require_one_argument()

			return multiply_by_arg_derivative(diff_variable_name, new expression_tree(operation_char::division, 1.,
			                                    new expression_tree(operation_char::multiplication, 3., new expression_tree(operation_char::rootation_3,
			                                        new expression_tree(operation_char::involution, new expression_tree(*the_ptr), 2.), nullptr))
			));
		}

		case operation_char::logariphmation: {
			// log(x, y) = ln(y) / ln(x)

			auto eqv_tree = std::make_unique<expression_tree>(operation_char::division,
					new expression_tree(operation_char::natural_logariphmation, new expression_tree(*right), nullptr),
					new expression_tree(operation_char::natural_logariphmation, new expression_tree(*left), nullptr)
			);

			return eqv_tree->generate_derivative_tree(diff_variable_name);
		}

		case operation_char::natural_logariphmation: {
			// ln(x)' = 1 / x

			require_one_argument()

			return multiply_by_arg_derivative(diff_variable_name, new expression_tree(operation_char::division, 1., new expression_tree(*the_ptr)));
		}

		case operation_char::logariphmation_10: {
			// ln(x)' = 1 / x

			require_one_argument()

			auto eq_tree = new expression_tree(operation_char::logariphmation, 10., new expression_tree(*the_ptr));

			return eq_tree->generate_derivative_tree(diff_variable_name);
		}

		case operation_char::exponentiation: {
			require_one_argument()

			return multiply_by_arg_derivative(diff_variable_name, new expression_tree(*this));
		}

		case operation_char::rootation_n: {
			// require_one_argument()

			auto eq_tree = std::make_unique<expression_tree>(
					operation_char::involution, new expression_tree(*left), new expression_tree(
							operation_char::division, 1., new expression_tree(*right))
			);

			return eq_tree->generate_derivative_tree(diff_variable_name);
		}

		////////////////////////////////////////////////////////////////////////////////////////////////////////
		//////////////////////////  		TRIGONOMETRY			////////////////////////////////////////////
		////////////////////////////////////////////////////////////////////////////////////////////////////////

		case operation_char::sinusation: {
			require_one_argument()

			return new expression_tree(operation_char::multiplication,
					the_ptr->generate_derivative_tree(diff_variable_name),
					new expression_tree(operation_char::cosinusation, new expression_tree(*the_ptr), nullptr));
		}

		case operation_char::cosinusation: {
			require_one_argument()

			return multiply_by_arg_derivative(diff_variable_name,
			                           new expression_tree(operation_char::substraction,
			                                0.,
				                            new expression_tree(operation_char::sinusation, new expression_tree(*the_ptr), nullptr))
											);
		}

		case operation_char::tangensation: {
			require_one_argument()

			return multiply_by_arg_derivative(diff_variable_name, new expression_tree(operation_char::division,
					1.,
					new expression_tree(operation_char::involution,
							new expression_tree(operation_char::cosinusation, new expression_tree(*the_ptr), nullptr),
							2.)
					));
		}

		case operation_char::cotangensation: {
			require_one_argument()

			return multiply_by_arg_derivative(diff_variable_name, new expression_tree(operation_char::substraction, 0., new expression_tree(operation_char::division,
			                           1.,
			                           new expression_tree(operation_char::involution,
			                                               new expression_tree(operation_char::sinusation, new expression_tree(*the_ptr), nullptr),
			                                               2.)
			)));
		}

		case operation_char::arcsinusation: {
			require_one_argument()

			return multiply_by_arg_derivative(diff_variable_name, new expression_tree(operation_char::division,
			                           1.,
			                           new expression_tree(operation_char::rootation_2,
			                                               new expression_tree(operation_char::substraction, 1, new expression_tree(operation_char::involution, new expression_tree(*the_ptr), 2)),
			                                               nullptr)
			));
		}

		case operation_char::arccosinusation: {
			require_one_argument()

			return multiply_by_arg_derivative(diff_variable_name, new expression_tree(operation_char::substraction, 0., new expression_tree(operation_char::division,
			                                                                          1.,
			                                                                          new expression_tree(operation_char::rootation_2,
			                                                                                              new expression_tree(operation_char::substraction, 1, new expression_tree(operation_char::involution, new expression_tree(*the_ptr), 2)),
			                                                                                              nullptr)
			)));
		}

		case operation_char::arctangensation: {
			require_one_argument()
			return multiply_by_arg_derivative(diff_variable_name, new expression_tree(operation_char::division, 1.,
					new expression_tree(operation_char::addition, 1, new expression_tree(operation_char::involution, new expression_tree(*the_ptr), 2))
			));
		}

		case operation_char::arccotangensation: {
			require_one_argument()
			return multiply_by_arg_derivative(diff_variable_name,
					new expression_tree(operation_char::substraction, 0.,
							new expression_tree(operation_char::division, 1.,
			                new expression_tree(operation_char::addition, 1, new expression_tree(operation_char::involution, new expression_tree(*the_ptr), 2))
			)));
		}

		default:
			std::cerr << "Unknown operation!" << std::endl;
			return nullptr;
	}

#undef require_one_argument
}

expression_tree *expression_tree::multiply_by_arg_derivative (const std::string &diff_variable_name, expression_tree *derivative_expression) const
{
	if (!(bool(right) ^ bool(left)))
			throw std::runtime_error("There should exist exactly one of the left and right nodes!");
	expression_tree *the_ptr = nullptr;
	if (left) the_ptr = left;
	if (right) the_ptr = right;

	auto res = new expression_tree(operation_char::multiplication, the_ptr->generate_derivative_tree(diff_variable_name), derivative_expression);
	res->optimize();
	return res;
}

std::unordered_set<std::string> expression_tree::get_variables () const
{
	if (type == types::number) return std::unordered_set<std::string>();
	if (type == types::variable) return { variable_name };

	std::unordered_set<std::string> res;

	if (right) res.merge(right->get_variables());
	if (left) res.merge(left->get_variables());

	return res;
}

size_t expression_tree::count_node_number () const
{
	if (type != types::operation) return 1;

	size_t res = 1;
	if (left) res += left->count_node_number();
	if (right) res += right->count_node_number();

	return res;
}

size_t expression_tree::count_operation_number () const
{
	if (type != types::operation) return 0;

	size_t res = 1;
	if (left) res += left->count_operation_number();
	if (right) res += right->count_operation_number();

	return res;
}

void expression_tree::reset_cache ()
{
	has_temp = false;
}

std::ostream &operator<< (std::ostream &os, const expression_tree::types &type)
{
	switch (type)
	{
		case expression_tree::types::number:
			os << "expression_tree::types::number";
			break;
		case expression_tree::types::operation:
			os << "expression_tree::types::operation";
			break;
		case expression_tree::types::variable:
			os << "expression_tree::types::variable";
			break;
	}

	return os;
}
