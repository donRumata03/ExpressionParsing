#include "dictionary.h"
#include <cmath>
#include "_letter_utils.h"


////////////////////////////////////////////					Operations			////////////////////////////////////////////////

std::unordered_map<operation_char, size_t> operator_priorities = {
	{operation_char::addition, 1 },
	{operation_char::substraction, 1 },
	{operation_char::multiplication, 2 },
	{operation_char::division, 2 },
	{operation_char::involution, 3 }
};

std::unordered_map<std::string, operation_char> string_operator_converter = {
	{ "+", operation_char::addition },
	{ "-", operation_char::substraction },
	{ "–", operation_char::substraction },

	{ "*", operation_char::multiplication },
	{ "/", operation_char::division },

	{ "^", operation_char::involution },
	{ "**", operation_char::involution },

	{ "exp", operation_char::exponentiation },
	{ "log", operation_char::logariphmation },
	{ "lg", operation_char::logariphmation_10 },
	{ "ln", operation_char::natural_logariphmation },

	{ "sqrt", operation_char::rootation_2 },
	{ "cbrt", operation_char::rootation_3 },
	{ "root", operation_char::rootation_n },

	{ "abs", operation_char::absation },
	{ "min", operation_char::mination },
	{ "max", operation_char::maxation },
	{ "sgn", operation_char::sgntion  },
	{ "sign", operation_char::sgntion  },

	{ "sin", operation_char::sinusation },
	{ "cos", operation_char::cosinusation },
	{ "tg", operation_char::tangensation },
	{ "tan", operation_char::tangensation },
	{ "ctg", operation_char::cotangensation },
	{ "cot", operation_char::cotangensation },

	{ "arcsin", operation_char::arcsinusation },
	{ "arccos", operation_char::arccosinusation },
	{ "arctg", operation_char::arctangensation },
	{ "arctan", operation_char::arctangensation },
	{ "arcctg", operation_char::arccotangensation },
	{ "arccot", operation_char::arccotangensation },
};

std::unordered_map<operation_char, size_t> operation_parameter_number {
	{ operation_char::addition, 2 },
	{ operation_char::substraction, 2 },
	
	{ operation_char::multiplication, 2 },
	{ operation_char::division, 2 },
	
	{ operation_char::involution , 2 },

	{ operation_char::rootation_2, 1 },
	{ operation_char::rootation_3, 1 },
	{ operation_char::rootation_n, 2 },

	{ operation_char::exponentiation, 1 },
	{ operation_char::logariphmation, 2 },
	{ operation_char::logariphmation_10, 1 },
	{ operation_char::natural_logariphmation, 1 },

	{ operation_char::absation, 1 },
	{ operation_char::mination, 2 }, 
	{ operation_char::maxation, 2 },
	{ operation_char::sgntion, 1 },


	{ operation_char::sinusation, 1 },
	{ operation_char::cosinusation, 1 },
	{ operation_char::tangensation, 1 },
	{ operation_char::cotangensation, 1 },

	{ operation_char::arcsinusation, 1 },
	{ operation_char::arccosinusation, 1 },
	{ operation_char::arctangensation, 1 },
	{ operation_char::arccotangensation, 1 },
};

std::unordered_map<operation_char, std::string> get_function_name {
	{ operation_char::addition, "Addition" },
	{ operation_char::substraction, "Substraction" },

	{ operation_char::multiplication, "Multiplication" },
	{ operation_char::division, "Division" },

	{ operation_char::involution , "Power" },

	{ operation_char::exponentiation, "Exponent" },
	{ operation_char::logariphmation, "Logariphm" },
	{ operation_char::logariphmation_10, "Base 10 Logariphm" },
	{ operation_char::natural_logariphmation, "Natural Logariphm" },

	{ operation_char::rootation_2, "Square root" },
	{ operation_char::rootation_3, "Cube root" },
	{ operation_char::rootation_n, "N_th root" },

	{ operation_char::absation, "Absolute value" },
	{ operation_char::mination, "Minimum" },
	{ operation_char::maxation, "Maximim" },
	{ operation_char::sgntion, "Signum" },

	{ operation_char::sinusation, "Sine" },
	{ operation_char::cosinusation, "Cosine" },
	{ operation_char::tangensation, "Tangent" },
	{ operation_char::cotangensation, "Cotangent" },

	{ operation_char::arcsinusation, "Inverse sine" },
	{ operation_char::arccosinusation, "Inverse cosine" },
	{ operation_char::arctangensation, "Inverse tangent" },
	{ operation_char::arccotangensation, "Inverse cotangent" },
};

double compute_function(const operation_char& c, const std::vector<double>& args)
{
	size_t have_size = args.size();
	size_t required_size = operation_parameter_number[c];

	if (have_size != required_size) {
		std::string beg = "Bad number of arguments: have: ";
		throw std::length_error(beg + std::to_string(have_size) + "; should be: " + std::to_string(required_size));
	}
		
	switch (c)
	{
	case operation_char::addition:
		return args[0] + args[1];
	case operation_char::substraction:
		return args[0] - args[1];

	case operation_char::multiplication:
		return args[0] * args[1];
	case operation_char::division:
		if (args[1] == 0) throw std::runtime_error("Can`t divide by zero :(");
		return args[0] / args[1];

	case operation_char::absation:
		return abs(args[0]);
	case operation_char::maxation:
		return std::max(args[0], args[1]);
	case operation_char::mination:
		return std::min(args[0], args[1]);
	case operation_char::sgntion:
		return ((args[0] == 0) ? (0) : ((args[0] > 0) ? (1) : (-1)));

	case operation_char::involution:
		if (std::round(args[1]) != args[1] && args[0] < 0)
			throw std::range_error("First number should be bigger than 0 to perform non-integer power operation!");

		return pow(args[0], args[1]);
		
	case operation_char::rootation_2:
		if (args[0] < 0)
			throw std::range_error("Number should be bigger or equal to 0 to perform sqrt operation!");
		return sqrt(args[0]);

	case operation_char::rootation_3:
		return cbrt(args[0]);
	
	case operation_char::rootation_n:
		if (std::round(args[1]) != args[1] && args[0] < 0)
			throw std::range_error("First number should be bigger than 0 to perform rootation_n operation!");
		return pow(args[0], 1 / args[1]);

	case operation_char::exponentiation:
		return exp(args[0]);
	case operation_char::logariphmation:
		if (args[0] <= 0)
			throw std::range_error("First number should be bigger than 0 to perform logarithm operation!");
		if (args[1] <= 0)
			throw std::range_error("Second number should be bigger than 0 to perform logarithm operation!");
		if (args[0] == 1)
			throw std::range_error("First number should not be equal to 1 to perform logariphm operation!");

		
		return log(args[1]) / log(args[0]);

	case operation_char::logariphmation_10:
		if (args[0] <= 0)
			throw std::range_error("The number should be greater than 0 to perform base 10 logarithm operation!");

		return log10(args[0]);

	case operation_char::natural_logariphmation:
		if (args[0] <= 0)
					throw std::range_error("The number should be greater than 0 to perform natural logarithm operation!");

		return log(args[0]);

	case operation_char::sinusation:
		return sin(args[0]);
	case operation_char::cosinusation:
		return cos(args[0]);
	case operation_char::tangensation:
		return tan(args[0]);
	case operation_char::cotangensation:
		return 1 / tan(args[0]);

	case operation_char::arcsinusation:
		return asin(args[0]);
	case operation_char::arccosinusation:
		return acos(args[0]);
	case operation_char::arctangensation:
		return atan(args[0]);
	case operation_char::arccotangensation:
		return atan(1 / args[0]);
	}

	throw std::runtime_error("Unknown operation!");
}

std::ostream &operator<< (std::ostream &os, const operation_char &operation)
{
	os << get_function_name[operation];
	return os;
}


///////////////////////////////////////				Constants				////////////////////////////
std::unordered_map<std::string, double> global_constants = {
	{ "pi", 3.14159265358979323846 },
	{ "e", 2.718281828459045235360287471352 },
	{ "Fi", 1.61803398874989484820 },
	{ "fi", 0.61803398874989484820 },

	{"inf", HUGE_VAL },
	{"Inf", HUGE_VAL },
	{"INF", HUGE_VAL },
};

///////////////////////////////////////				tokenization				////////////////////////////


token_types get_char_token_type(const char c)
{
	if (string_basic_operators.find({ c }) != string_basic_operators.end())
	{
		return token_types::operator_char;
	}

	if (is_space_(c))
	{
		return token_types::space;
	}

	if (is_letter_(c) || c == '_')
	{
		return token_types::word;
	}

	if (is_digit_(c) || c == '.')
	{
		return token_types::number;
	}

	if (get_brace_type.find({ c }) != get_brace_type.end())
	{
		return token_types::brace;
	}

	return token_types::unknown;
}



