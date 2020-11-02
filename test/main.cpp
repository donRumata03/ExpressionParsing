#include <iostream>

#include "expression_parsing.h"
#include "benchmark.h"
#include "tests.h"
#include "encoding_test.h"
#include "tree_tests.h"

#ifdef WIN32
#include <windows.h>
#endif

int main() {
	// SetConsoleOutputCP(CP_UTF8);
	// SetConsoleCP(CP_UTF8);

#ifdef WIN32
	system(("chcp "s + std::to_string(CP_UTF8)).c_str());
#endif


	/// Tree tests:

	/*

	std::variant<double, expression_tree*> v = new expression_tree();
	std::cout << std::boolalpha << std::holds_alternative<expression_tree*>(v) << std::endl;

	*/

	// test_tree_constructors_and_operators();

	// tree_priority_applying_test();
	// test_tree_grouping();
	// tree_functionality_test(); /// Also benchmarking!

	test_tree_generation();
	// test_tree_computing();

	// tree_benchmark();
	// test_tree_leak();

	// measure_all_performance();

	/// Derivatives:
	// test_tree_derivative();



	/// Other tests:

	// test_encoding();



	return 0;

}
