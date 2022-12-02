#ifndef OPTIMIZING_IO_H_INCLUDED
#define OPTIMIZING_IO_H_INCLUDED

#include "diff.h"

#define NODE_NUM(node)                       \
            node->value->diff_arg->num

#define NODE_OP(node)                        \
            node->value->diff_arg->op

#define NCTOR_UNAR_NUM(oper)                        \
            NCTOR(TYPE_NUM, NUM(oper##(NODE_NUM(right_node))), NULL, NULL)

#define NCTOR_OPER_NUM(oper)                        \
            NCTOR(TYPE_NUM, NUM(NODE_NUM(left_node) oper NODE_NUM(right_node)), NULL, NULL)

#define SIMPLIFY_ITER(code)                                                                 \
            struct TreeNode *new_node = code;                                               \
            int node_dtor_err = TreeNodeDtor(curr_node);                                    \
            ERROR_CHECK(node_dtor_err, NULL);                                               \
            return new_node


struct TreeNode *SimplifyExpression(struct TreeNode *curr_node);

struct TreeNode *OptimizeUnarFunc(struct TreeNode *curr_node, struct TreeNode *right_node);
struct TreeNode *OptimizeGeneral(struct TreeNode *curr_node, 
                                 struct TreeNode *left_node, struct TreeNode *right_node);
struct TreeNode *OptimizeSpecialCase(struct TreeNode *curr_node, 
                                     struct TreeNode *left_node, struct TreeNode *right_node);


#endif //OPTIMIZING_IO_H_INCLUDED