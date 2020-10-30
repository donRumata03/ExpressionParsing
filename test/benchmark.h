//
// Created by Vova on 23.05.2020.
//

#pragma once

#include <thread>
#include "Timer.h"

inline void make_benchmark(size_t iterations = 1000000)
{
	// Benchmark of expression counting!

	// For Release mode: 1000000 iterations were computed in 7.75317 s
	// For Debug mode: 1000000 iterations were computed in 44.2836 s
	// The difference is 44.2836 / 7.75317 = 5.7 times!

	std::string expression = "0.5 + (cos(sin(abs(x^2 - y^2)))^2 - 0.5) / (1 + 0.001(x^2 + y^2))^2";

	auto particle = compile_expression(expression).particle;

	Timer t("Counting expressions");

	for(size_t index = 0; index < iterations; index++){
		double value = particle.compute({
			{"x", -2.64332 },
			{"y", 1.4352312 }
		});
	}

	// Timer destructor is called here!

}

inline void async_test()
{
	std::string expression = "0.5 + (cos(sin(abs(x^2 - y^2)))^2 - 0.5) / (1 + 0.001(x^2 + y^2))^2";

	auto compiled = compile_expression(expression);

	if (compiled.has_error_message) {
		std::cout << "Has error message: " << compiled.error_message << std::endl;
		// std::terminate();
		throw std::runtime_error("Error while compiling expression");
	}
	auto particle = compiled.particle;

	std::vector<std::thread> threads;

	for(size_t thread_index = 0; thread_index < 10; thread_index++) {
		threads.emplace_back(
				[&]() {
					for (size_t index = 0; index < 300000; index++) {
						double value = particle.compute({
								                                {"x", -2.64332},
								                                {"y", 1.4352312}
						                                });
					}
					std::cout << "Finished!" << std::endl;
				}
		);
	}

	for(auto& thread : threads) thread.join();
}


