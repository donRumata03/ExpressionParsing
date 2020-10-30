//
// Created by Vova on 24.05.2020.
//

#pragma once

#include <string>

#include <iostream>
#include <fstream>

std::string readFile(const std::string& filename) {
	std::ifstream in(filename, std::ios::in | std::ios::binary);
	if (in)
	{
		std::string contents;

		in.seekg(0, std::ios::end);
		contents.resize(static_cast<const unsigned int>(in.tellg()));
		in.seekg(0, std::ios::beg);
		in.read(&contents[0], contents.size());
		in.close();
		return contents;
	}
	// print_red("Can`t find file!!! \n");
	std::cout << "( " << filename << " )" << std::endl;
	return "-1";
}




inline void test_encoding()
{
	/// File is in cp1251!

	std::string expression = "10 – 1 - 3";

	"-";
    "–";

	std::cout << compile_expression(expression).particle.compute() << std::endl;

	/*

	std::string r = readFile(R"(D:\Literature_data\All_books\Толстой Лев\Война и мир Том 3 и 4\text.txt)");

	std::string eng = "Hello!";
	std::string rus = "Привет";

	std::cout << eng.size() << " " << rus.size() << std::endl;

	std::cout << rus[0] << rus[1] << std::endl;

	std::cout << r << std::endl;

	 */
}