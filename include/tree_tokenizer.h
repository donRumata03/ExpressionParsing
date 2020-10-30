//
// Created by Vova on 11.06.2020.
//

#pragma once


#include "expression_tree.h"

using tree_tokens = std::vector<std::variant<expression_tree *, operation_char>>;

std::vector<tree_tokens> group_by_priority(std::vector<std::variant<expression_tree*, operation_char>> & expression, size_t priority);
tree_tokens apply_priority(tree_tokens& expression, size_t priority);

