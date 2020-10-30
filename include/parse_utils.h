#pragma once

#include <cassert>
#include <stack>

#include "dictionary.h"
#include "expr_token.h"
#include "expression_segment.h"



bool isOnlyDouble(const std::string& str);
std::optional<double> to_double(const std::string& str);

std::optional<double> get_variable(const std::unordered_map<std::string, double>& variables, const std::string& variable_name);

std::vector<std::string> get_variables(const std::vector<expr_token>& data);

std::pair<size_t, size_t> get_brace_positions(const std::vector<expr_token>& data, size_t position = 0, const std::string& brace_type = "(");
std::vector<expr_token> get_brace_contents(const std::vector<expr_token>& data, size_t position = 0, const std::string& brace_type = "(");

std::vector<std::vector<expr_token>> get_function_args(const std::vector<expr_token>& toks, size_t function_pos);

std::vector<expression_segment> select_brace_content(const std::vector<expr_token>& toks);


std::vector<expr_token> rasterize_brace_content(const std::vector<expression_segment>& brace_content);
std::vector<expr_token> process_unary_minuses(const std::vector<expr_token>& tokens);
