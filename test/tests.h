//
// Created by Vova on 23.05.2020.
//

#pragma once

// #include <bits/stdc++.h>
#include <scenery.h>
#include <filesystem>

inline void test_computing(){
	std::string expression = "0.5 + (cos(sin(abs(x^2 - y^2)))^2 - 0.5) / (1 + 0.001(x^2 + y^2))^2";

	auto compiled = compile_expression(expression);

	if (compiled.has_error_message) {
		std::cout << "Has error message! " << compiled.error_message << std::endl;
		return;
	}
	auto particle = compiled.particle;

	double value = particle.compute({
			                                {"x", -2.64332 },
			                                {"y", 1.4352312 }
	                                });

	std::cout << value << std::endl;
}


