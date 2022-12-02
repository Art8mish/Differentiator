
#include "../include/diff.h"

struct TreeNode *SimplifyExpression(struct TreeNode *curr_node)
{
    ERROR_CHECK(curr_node == NULL, NULL);
    printf("entered simplify expression, curr_node: %d\n", curr_node);

    struct TreeNode* prev_left  = curr_node->left;
    struct TreeNode* prev_right = curr_node->right;

    if (curr_node->value->type_arg == TYPE_NUM ||
        curr_node->value->type_arg == TYPE_VAR)
        return curr_node;
    
    struct TreeNode *left_node = NULL;
    if (curr_node->left != NULL)
    {
        left_node = SimplifyExpression(curr_node->left);
        ERROR_CHECK(left_node == NULL, NULL);
    }

    struct TreeNode *right_node = NULL;
    if (curr_node->right != NULL)
    {
        right_node = SimplifyExpression(curr_node->right);
        ERROR_CHECK(right_node == NULL, NULL);
    }

    curr_node->left  = left_node;
    curr_node->right = right_node;

    printf("type_arg = %d, op = %d\n", curr_node->value->type_arg, curr_node->value->diff_arg->op);
    
    ERROR_CHECK(curr_node->value->type_arg != TYPE_OP, NULL);

    if (left_node == NULL && right_node != NULL &&
        right_node->value->type_arg == TYPE_NUM)
    {
        return OptimizeUnarFunc(curr_node, right_node);
    }

    if (left_node != NULL && right_node != NULL &&
         left_node->value->type_arg == TYPE_NUM &&
        right_node->value->type_arg == TYPE_NUM)
    {
        return OptimizeGeneral(curr_node, left_node, right_node);
    }

    
    struct TreeNode *optimized_node = OptimizeSpecialCase(curr_node, left_node, right_node);
    if (optimized_node != NULL)
        return optimized_node;

    if (left_node != NULL)
    {
        int tree_tie_err = TreeNodeTie(curr_node, left_node, TREE_TIE_LEFT);
        ERROR_CHECK(tree_tie_err, NULL);
    }
        
    if (right_node != NULL)
    {
        int tree_tie_err = TreeNodeTie(curr_node, right_node, TREE_TIE_RIGHT);
        ERROR_CHECK(tree_tie_err, NULL);
    }

    
    if (left_node  != prev_left ||
        right_node != prev_right)
        return SimplifyExpression(curr_node);
    
    printf("do nothing\n");
    return curr_node;
}


struct TreeNode *OptimizeUnarFunc(struct TreeNode *curr_node, struct TreeNode *right_node)
{
    ERROR_CHECK(curr_node  == NULL, NULL);
    ERROR_CHECK(right_node == NULL, NULL);

    switch (curr_node->value->diff_arg->op)
        {
            case SIN_OP  :
                {  
                    SIMPLIFY_ITER(NCTOR(TYPE_NUM, NUM(sin(NODE_NUM(right_node))), NULL, NULL));
                }

            case COS_OP  :
                {  
                    SIMPLIFY_ITER(NCTOR(TYPE_NUM, NUM(cos(NODE_NUM(right_node))), NULL, NULL));
                }

            case TAN_OP  : 
                { 
                    SIMPLIFY_ITER(NCTOR(TYPE_NUM, NUM(tan(NODE_NUM(right_node))), NULL, NULL));
                }

            case ASIN_OP  :
                {  
                    SIMPLIFY_ITER(NCTOR(TYPE_NUM, NUM(asin(NODE_NUM(right_node))), NULL, NULL));
                }

            case ACOS_OP  : 
                { 
                    SIMPLIFY_ITER(NCTOR(TYPE_NUM, NUM(acos(NODE_NUM(right_node))), NULL, NULL));
                }

            case ATAN_OP  : 
                { 
                    SIMPLIFY_ITER(NCTOR(TYPE_NUM, NUM(atan(NODE_NUM(right_node))), NULL, NULL));
                }

            case SINH_OP  :
                {
                    SIMPLIFY_ITER(NCTOR(TYPE_NUM, NUM(sinh(NODE_NUM(right_node))), NULL, NULL));
                }

            case COSH_OP  : 
                { 
                    SIMPLIFY_ITER(NCTOR(TYPE_NUM, NUM(cosh(NODE_NUM(right_node))), NULL, NULL));
                }

            case TANH_OP  :
                {  
                    SIMPLIFY_ITER(NCTOR(TYPE_NUM, NUM(tanh(NODE_NUM(right_node))), NULL, NULL));
                }

            case LN_OP  :  
                {
                    SIMPLIFY_ITER(NCTOR(TYPE_NUM, NUM(log(NODE_NUM(right_node))), NULL, NULL));
                }

            default     :
                    break;
        }

    return NULL;
}

struct TreeNode *OptimizeGeneral(struct TreeNode *curr_node, 
                                 struct TreeNode *left_node, struct TreeNode *right_node)
{
    ERROR_CHECK(curr_node  == NULL, NULL);
    ERROR_CHECK(left_node  == NULL, NULL);
    ERROR_CHECK(right_node == NULL, NULL);

    switch (curr_node->value->diff_arg->op)
        {
            case ADD_OP : 
                {
                    SIMPLIFY_ITER(NCTOR_OPER_NUM(+));
                }

            case SUB_OP : 
                {
                    SIMPLIFY_ITER(NCTOR_OPER_NUM(-)); 
                }

            case MUL_OP : 
                {
                    SIMPLIFY_ITER(NCTOR_OPER_NUM(*));
                }

            case DIV_OP : 
                {
                    SIMPLIFY_ITER(NCTOR_OPER_NUM(/));
                }

            case POW_OP : 
                {
                    SIMPLIFY_ITER(NCTOR(TYPE_NUM, NUM(pow(NODE_NUM(left_node), 
                                                          NODE_NUM(right_node))), NULL, NULL));
                }
            default     : 
                    break;
        }

    return NULL;
}

struct TreeNode *OptimizeSpecialCase(struct TreeNode *curr_node, 
                                     struct TreeNode *left_node, struct TreeNode *right_node)
{
    ERROR_CHECK(curr_node  == NULL, NULL);

    // 0 * (..) | (..) * 0 | 0 / (..) = 0
    if (left_node != NULL && right_node != NULL &&
        (NODE_OP(curr_node) == MUL_OP || NODE_OP(curr_node) == DIV_OP) &&
        ((left_node->value->type_arg == TYPE_NUM && 
         COMPARE_DOUBLE(NODE_NUM(left_node),  0))   ||
        (right_node->value->type_arg == TYPE_NUM &&
         COMPARE_DOUBLE(NODE_NUM(right_node), 0))))
    {
        struct TreeNode *new_node = NCTOR(TYPE_NUM, NUM(0), NULL, NULL);
        int node_dtor_err = TreeNodeDtor(curr_node);
        ERROR_CHECK(node_dtor_err, NULL);
        return new_node;
    }

    // (..) ^ 0 = 1 (0^0 exist previosly) || 1 ^ (..)
    if (left_node != NULL && right_node != NULL  &&
         NODE_OP(curr_node) == POW_OP &&
        ((right_node->value->type_arg == TYPE_NUM &&
         COMPARE_DOUBLE(NODE_NUM(right_node), 0)) || 

        (left_node->value->type_arg   == TYPE_NUM &&
         COMPARE_DOUBLE(NODE_NUM(left_node),  1))))
    {
        struct TreeNode *new_node = NCTOR(TYPE_NUM, NUM(1), NULL, NULL);
        int node_dtor_err = TreeNodeDtor(curr_node);
        ERROR_CHECK(node_dtor_err, NULL);
        return new_node;
    }

    //(..) * 1 || (..) / 1 || (..) ^ 1 || (..) + 0 || (..) - 0 = (..)
    if (left_node != NULL && right_node != NULL &&
        (((NODE_OP(curr_node) == MUL_OP  || 
          NODE_OP(curr_node) == DIV_OP  ||
          NODE_OP(curr_node) == POW_OP)          &&
        (right_node->value->type_arg == TYPE_NUM &&
         COMPARE_DOUBLE(NODE_NUM(right_node), 1)))  || 
        ((NODE_OP(curr_node) == ADD_OP  ||
          NODE_OP(curr_node) == SUB_OP)          &&
        (right_node->value->type_arg == TYPE_NUM &&
         COMPARE_DOUBLE(NODE_NUM(right_node), 0)))))
    {
        struct TreeNode *new_node = NCOPY(curr_node->left);
        int node_dtor_err = TreeNodeDtor(curr_node);
        ERROR_CHECK(node_dtor_err, NULL);
        return new_node;
    }

    // 1 * (..) || 0 + (..)  = (..)
    if (left_node != NULL && right_node != NULL &&
        ((NODE_OP(curr_node) == MUL_OP &&
        left_node->value->type_arg == TYPE_NUM &&
        COMPARE_DOUBLE(NODE_NUM(left_node), 1)) ||
        (NODE_OP(curr_node) == ADD_OP &&
        left_node->value->type_arg == TYPE_NUM &&
        COMPARE_DOUBLE(NODE_NUM(left_node), 0))))
    {
        struct TreeNode *new_node = NCOPY(curr_node->right);
        int node_dtor_err = TreeNodeDtor(curr_node);
        ERROR_CHECK(node_dtor_err, NULL);
        return new_node;
    }

    return NULL;
}