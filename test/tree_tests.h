//
// Created by Vova on 08.06.2020.
//

#pragma once

#include <expression_parsing.h>

#include <functional>

#include "Timer.h"

#include "coloured_printer.h"
#include "_print_stl.h"

using namespace std::string_literals;

inline void tree_priority_applying_test() {
	std::vector<std::variant<expression_tree*, operation_char>> tokens;
	tokens.emplace_back(new expression_tree(2));
	tokens.emplace_back(operation_char::addition);
	tokens.emplace_back(new expression_tree(3));
	tokens.emplace_back(operation_char::involution);
	tokens.emplace_back(new expression_tree(4));
	tokens.emplace_back(operation_char::involution);
	tokens.emplace_back(new expression_tree(1));
	tokens.emplace_back(operation_char::division);
	tokens.emplace_back(new expression_tree(6));
	tokens.emplace_back(operation_char::substraction);
	tokens.emplace_back(new expression_tree(7));

	std::cout << "Tokens: {" << std::endl;

	for (auto & token : tokens) {
		std::cout << "    ";
		if (std::holds_alternative<operation_char>(token)) std::cout << std::get<operation_char>(token) << std::endl;
		else std::cout << *std::get<expression_tree*>(token) << std::endl;
	}

	std::cout << "}\n" << std::endl;

	auto involved = apply_priority(tokens, 3);

	std::cout << "Involved Tokens ("s + std::to_string(involved.size()) << ") : {" << std::endl;

	for (auto & token : involved) {
		std::cout << "    ";
		if (std::holds_alternative<operation_char>(token)) std::cout << std::get<operation_char>(token) << std::endl;
		else std::cout << *std::get<expression_tree*>(token) << std::endl;
	}
}

inline void test_tree_grouping(){
	std::vector<std::variant<expression_tree*, operation_char>> tokens;
	tokens.emplace_back(new expression_tree(2));
	tokens.emplace_back(operation_char::addition);
	tokens.emplace_back(new expression_tree(3));
	tokens.emplace_back(operation_char::involution);
	tokens.emplace_back(new expression_tree(4));
	tokens.emplace_back(operation_char::involution);
	tokens.emplace_back(new expression_tree(1));
	tokens.emplace_back(operation_char::division);
	tokens.emplace_back(new expression_tree(6));
	tokens.emplace_back(operation_char::substraction);
	tokens.emplace_back(new expression_tree(7));


	std::cout << "Tokens: {" << std::endl;

	for (auto & token : tokens) {
		std::cout << "    ";
		if (std::holds_alternative<operation_char>(token)) std::cout << std::get<operation_char>(token) << std::endl;
		else std::cout << *std::get<expression_tree*>(token) << std::endl;
	}

	std::cout << "}\n" << std::endl;

	std::cout << "____________________________________________________________\n";

	std::cout << "Going to group:" << std::endl;
	auto grouped = group_by_priority(tokens, 3);
	std::cout << "Performed grouping!" << std::endl;

	for (auto & group : grouped) {
		std::cout << "{" << std::endl;

		for (auto &token : group) {
			std::cout << "    ";
			if (std::holds_alternative<operation_char>(token))
				std::cout << std::get<operation_char>(token) << std::endl;
			else std::cout << *std::get<expression_tree *>(token) << std::endl;
		}

		std::cout << "}\n";
	}

}



inline void tree_functionality_test() {
	std::vector<std::variant<expression_tree*, operation_char>> tokens;
	tokens.emplace_back(new expression_tree(2));
	tokens.emplace_back(operation_char::addition);
	tokens.emplace_back(new expression_tree(3));
	tokens.emplace_back(operation_char::involution);
	tokens.emplace_back(new expression_tree(4));
	tokens.emplace_back(operation_char::involution);
	tokens.emplace_back(new expression_tree(1));
	tokens.emplace_back(operation_char::division);
	tokens.emplace_back(new expression_tree(6));
	tokens.emplace_back(operation_char::substraction);
	tokens.emplace_back(new expression_tree(7));

	std::string human_interpretation = "2 + 3^4^1 / 6 - 7";

	std::cout << "Tokens: {" << std::endl;

	for (auto & token : tokens) {
		std::cout << "    ";
		if (std::holds_alternative<operation_char>(token)) std::cout << std::get<operation_char>(token) << std::endl;
		else std::cout << *std::get<expression_tree*>(token) << std::endl;
	}

	std::cout << "}\n" << std::endl;

	std::cout << "____________________________________________________________\n";

	expression_tree all_tree(tokens);
	std::cout << all_tree << std::endl;

	std::cout << "Result is: " << all_tree.compute({}) << std::endl;
	std::cout << "Right result is: " << compile_expression(human_interpretation).particle.compute() << std::endl;

	/// Benchmark tree (compilation) and particle (interpretation):

	std::cout << "_____________________________________________________" << std::endl;
	std::cout << "Benchmarking trees and particles: " << std::endl;

	size_t bench_num = 10000000;

	{
		Timer give_me_a_name("Computing prepared tree");
		for (size_t i = 0; i < bench_num; ++i) {
			double result = all_tree.compute({});
		}
	}
	std::cout << "________________________________" << std::endl;

	auto particle = compile_expression(human_interpretation).particle;

	{
		Timer give_me_a_name("No trees!");
		for (size_t i = 0; i < bench_num; ++i) {
			double result = particle.compute();
		}
	}
	std::cout << "_________________________________" << std::endl;


	{
		Timer give_me_a_name("Computing new-created tree");
		for (size_t i = 0; i < bench_num; ++i) {
			auto *new_tree = particle.generate_computational_tree();
			double answer = new_tree->compute();
		}
	}

	std::cout << "_____________________________________________________" << std::endl;
}


//////////////////////////////////////////////////////////////////////
//////////////////////            Working tests:      ////////////////
//////////////////////////////////////////////////////////////////////


inline void test_tree_generation(){
	// auto particle = compile_expression("0.5 + (cos(sin(abs(x^2 - y^2)))^2 - 0.5) / (1 + 0.001(x^2 + y^2))^2").particle;

	std::string expression = "привет + 1";

	for (auto c : expression) std::cout << int(c) << std::endl;

	auto particle = compile_expression(expression).particle;
	auto* tree = particle.generate_computational_tree();

	std::unordered_map<std::string, double> vars = { { "привет", 3.5 } };

	std::cout << "Answer is: " << tree->compute(vars) << std::endl;
	std::cout << "Right answer is: " << particle.compute( vars ) << std::endl;
}

inline void test_tree_computing(){

	auto* one = new expression_tree {
			1
	};

	auto two = new expression_tree {
			2
	};

	auto three = new expression_tree {
			3
	};

	auto one_plus_two = new expression_tree (
		operation_char::addition,
		one,
		two
	);

	auto three_mul_three = expression_tree (
			operation_char::multiplication,
			one_plus_two,
			three
	);

	std::cout << "Result is: " << three_mul_three.compute({}) << std::endl;
	std::cout << "Requires variables: " << std::boolalpha << three_mul_three.requires_variables() << std::endl;

	try {
		three_mul_three.optimize();
	}
	catch(std::exception& e) {
		std::cout << e.what() << std::endl;
	}

	std::cout << "Result is: " << three_mul_three.compute({}) << std::endl;
	std::cout << "Requires variables: " << std::boolalpha << three_mul_three.requires_variables() << std::endl;
}


inline void test_tree_constructors_and_operators(){
	auto* one = new expression_tree {
			1
	};

	auto two = new expression_tree {
			2
	};

	auto three = new expression_tree {
			3
	};

	auto one_plus_two = new expression_tree (
			operation_char::addition,
			one,
			two
	);

	auto three_mul_three = new expression_tree (
			operation_char::multiplication,
			one_plus_two,
			three
	);

	auto* moved = new expression_tree(std::move(*three_mul_three));

	std::cout << "Going to copy!" << std::endl;

	auto* copied = new expression_tree(*moved);

	std::cout << "______________________________________" << std::endl;

	auto new_tree = new expression_tree();

	*new_tree = *copied;

	auto new_new_tree = new expression_tree();
	*new_new_tree = std::move(*new_tree);

	std::cout << *new_new_tree << std::endl;
}


///////////////////////////////////////////////////////////////////////////
/// Benchmark tree ("compilation") and particle ("interpretation"):	 //////
///////////////////////////////////////////////////////////////////////////
inline void tree_benchmark() {

	std::string expression = "0.5 + (cos(sin(abs(x^2 - y^2)))^2 - 0.5) / (1 + 0.001(x^2 + y^2))^2";
	auto particle = compile_expression(expression).particle;
	auto* tree = particle.generate_computational_tree();

	std::unordered_map<std::string, double> vars = { { "x", 3.5 }, { "y", 5.3 } };


	std::cout << "_____________________________________________________" << std::endl;
	std::cout << "Benchmarking trees and particles: " << std::endl;

	size_t bench_num = 1e+6;

	{
		Timer give_me_a_name("Computing prepared tree");
		for (size_t i = 0; i < bench_num; ++i) {
			double result = tree->compute(vars);
		}
	}
	std::cout << "________________________________" << std::endl;


	{
		Timer give_me_a_name("No trees! (prepared particle)");
		for (size_t i = 0; i < bench_num; ++i) {
			double result = particle.compute(vars);
		}
	}
	std::cout << "_________________________________" << std::endl;


	{
		Timer give_me_a_name("Creating tree from prepared particle");
		for (size_t i = 0; i < bench_num; ++i) {
			auto *new_tree = particle.generate_computational_tree();
		}
	}
	std::cout << "_________________________________" << std::endl;


	{
		Timer give_me_a_name("Creating particle");
		for (size_t i = 0; i < bench_num; ++i) {
			auto new_particle = compile_expression(expression).particle;
		}
	}


	std::cout << "_____________________________________________________" << std::endl;
}


inline void test_tree_leak(){
	std::string expression = "0.5 + (cos(sin(abs(x^2 - y^2)))^2 - 0.5) / (1 + 0.001(x^2 + y^2))^2";
	auto particle = compile_expression(expression).particle;

	std::unordered_map<std::string, double> vars = { { "x", 3.5 }, { "y", 5.3 } };

	size_t epochs = 5e+5;

	/// Raw pointer
	for (size_t i = 0; i < epochs; ++i) {
		auto* tree = particle.generate_computational_tree();
		{ auto new_tree = std::move(*tree); }
		delete tree;
	}

	/// Smart pointer
	for (size_t j = 0; j < epochs; ++j) {
		auto ptr = std::unique_ptr<expression_tree>(particle.generate_computational_tree());
	}
}

///////////////////////////
/// 	Derivatives		///
///////////////////////////

inline void test_one_derivative(const std::string& expression, const std::unordered_map<std::string, double>& variable_values, double expected_result){
	std::unique_ptr<expression_tree> tree(compile_expression(expression).particle.generate_computational_tree());
	std::unique_ptr<expression_tree> derivative_tree(tree->generate_derivative_tree("x"));

	double res = derivative_tree->compute(variable_values);

	bool correct = (res == expected_result && res == 0) || abs((res - expected_result) / (res + expected_result)) < 0.001;


	// RED : "\x1B[31m", "\033[0m"
	// GREEN: "\033[92m"

	std::cout << "The derivative of " << expression << " at " << variable_values << " is: " << res << ", while it should be: " << expected_result << "\t\t";
	if (correct) colored_print("Test passed", console_colors::green);
	else colored_print("Test failed", console_colors::red);

	std::cout << "______________________________________________________________" << std::endl;

	// std::cout << (correct ? "Test passed" : "Test failed") << std::endl;
}

inline void test_tree_derivative ()
{
	test_one_derivative("10 / (x * x) + y + 332", { { "x", 3 } }, -0.740741); /// Should be "-20/x^3" -> -0.740741
	test_one_derivative("abs(10 / (x * x) + y + 332)", {{ "x", 3 }, {"y", -10000000}}, 0.740741);

	test_one_derivative("sin(x)", {{"x", 10}}, -0.839072);
	test_one_derivative("abs(sin(x))", {{"x", 10}}, 0.839072);
	test_one_derivative("cos(x)", {{"x", 10}}, 0.54402111088);

	test_one_derivative("arcsin(sin(x))", {{"x", 0.36534}}, 1);
	test_one_derivative("arccos(cos(x))", {{"x", 0.36534}}, 1);

	test_one_derivative("tg(cos(x * x))", {{"x", global_constants["pi"] - 8}}, -9.72545);

	test_one_derivative("min(x, 10)", {{"x", 5}}, 1);

	test_one_derivative("min(cos(x^2), sin(x - 1.321))", {{"x", 10.432}}, -18.8618);


	test_one_derivative("x^2", {{"x", 10}}, 20);


	test_one_derivative("x^2", {{"x", -10}}, -20);


	test_one_derivative("x^(1 + 1)", {{"x", -10}}, -20);

	test_one_derivative("abs(x)", { { "x", 0 } }, 0);
	test_one_derivative("abs(x)", { { "x", 100 } }, 1);
	test_one_derivative("abs(x)", { { "x", -100000 } }, -1);



	test_one_derivative("min(cos(x^2), sin(x - 1.321))", {{"x", -1.490313279879031}}, 2.372400315076007978);
	test_one_derivative("min(cos(x^2), sin(x - 1.321))", {{"x", global_constants["pi"] - 8}}, -9.7081733392747962880451);


	test_one_derivative("root(x, 5)", {{"x", global_constants["pi"] + 8}}, 0.029071816898814604346469895586267860738921);

	colored_print("________________________________________________________________", console_colors::bold, console_colors::yellow);
	colored_print("|||||||||||\t\tSerious tests:\t\t|||||||||||", console_colors::bold, console_colors::yellow);
	colored_print("________________________________________________________________", console_colors::bold, console_colors::yellow);

	// std::cout << "" << std::endl;

	test_one_derivative("0.5 + (cos(sin(abs(x^2 - y^2)))^2 - 0.5) / (1 + 0.001(x^2 + y^2))^2",
			{{"x", global_constants["pi"] + 8}, {"y", 1.876868686}}, -5.53287);

	test_one_derivative("0.5 + (cos(sin(abs(x^2 - y^2)))^2 - 0.5) / (1 + 0.001(x^2 + y^2))^2",
	                    {{"x", 0.23245345}, {"y", 1.876868686}}, 0.261537);



	test_one_derivative("0-cos(x)cos(y)(exp(0-((x-pi)^2 + (y-pi)^2)))",
	                    {{"x", 3.5}, {"y", 2.5}}, 0.477113);

	test_one_derivative("0-cos(x)cos(y)(exp(0-((x-pi)^2 + (y-pi)^2)))",
	                    {{"x", 2.46}, {"y", 5}}, 0.009521);

	test_one_derivative("arcctg(ctg(-cos(x)cos(y)(exp(-arccos(((x-pi)^2) + sin(y-pi)^2)))))",
	                    {{"x", 4}, {"y", 6}}, 0.610344);

	test_one_derivative("-20e^((0-0.2)sqrt(0.5(x^2 + y^2))) Ц e^(0.5(cos(2pi*x) + cos(2pi*y))) + e + 20",
	                    {{"x", 2.5}, {"y", 1}}, 1.794492);



	// std::cout << !(true ^ true) << " " << !(true ^ false) << " " << !(false ^ true) << " " << !(false ^ false) << " " << std::endl;
/*

	std::string simple_expression = "x^2 + sqrt(x) + y - 10";

	std::unique_ptr<expression_tree> simple_tree(compile_expression(simple_expression).particle.generate_computational_tree());
	std::unique_ptr<expression_tree> simple_derivative_tree(simple_tree->generate_derivative_tree("x")); /// Should be 2x + 1 / 2sqrt(x)

	std::cout << simple_derivative_tree->compute({{ "x", 3 }}) << std::endl; /// Should be 2 * 3 + 1 / 2sqrt(3) ? 6,288675
*/

}

double virtual_caller(const std::function<double()>& func) {
	return func();
}

inline void measure_all_performance(){

	/// counting tree, virtual call

	size_t iterations = 5e+4;


	auto benchmark_operation = [iterations](const auto& operation, const std::string& message){
			Timer timer;
			for (size_t i = 0; i < iterations; ++i) operation();
			double iteration_time_mks = timer.get_time("mks") / iterations;
			std::cout << message + " took " << iteration_time_mks << " microseconds!" << std::endl;
	};


	benchmark_operation([](){auto tree = make_expression_tree("arcctg(ctg(-cos(x)cos(y)(exp(-arccos(((x-pi)^2) + sin(y-pi)^2)))))");}, "Creating an expression tree from scratch");


	auto prepared_tree = make_expression_tree("arcctg(ctg(-cos(x)cos(y)(exp(-arccos(((x-pi)^2) + sin(y-pi)^2)))))");
	benchmark_operation([&prepared_tree](){auto derivative_tree = prepared_tree->generate_derivative_tree("x");}, "Creating derivative tree");


	auto prepared_derivative_tree = prepared_tree->generate_derivative_tree("x");
	std::unordered_map<std::string, double> args = {{"x", 4}, {"y", 6}};

	benchmark_operation([&prepared_derivative_tree, &args](){double answer = prepared_derivative_tree->compute(args);}, "Counting derivative tree result");
	benchmark_operation([&prepared_tree, &args](){double answer = prepared_tree->compute(args);}, "Counting tree result");
	benchmark_operation([&prepared_derivative_tree, &args](){double answer = virtual_caller([&](){ return prepared_derivative_tree->compute(args);});}, "Virtual counting tree result");

	auto prepared_particle = compile_expression("arcctg(ctg(-cos(x)cos(y)(exp(-arccos(((x-pi)^2) + sin(y-pi)^2)))))").particle;
	benchmark_operation([&prepared_particle, &args](){double answer = prepared_particle.compute(args);}, "Computing particle");

}

