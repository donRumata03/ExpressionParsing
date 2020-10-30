#pragma once

#include "expr_token.h"

bool validate_functions(const std::vector<expr_token>& toks);
bool validate_braces(const std::vector<expr_token>& toks);
std::optional<std::string> validate_function_calls(const std::vector<expr_token>& toks);
bool validate_operator_args(const std::vector<expr_token>& toks);
std::optional<std::string> validate_unknown_symbols(const std::vector<expr_token>& toks);