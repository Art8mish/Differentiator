
#include "../include/diff.h"

#define OP(op)                \
            DiffOpCtor(op)

#define NUM(num)              \
            DiffNumCtor(num)

#define VAR(var)              \
            DiffVarCtor(var)

#define NCTOR(type_value, value, left_node, right_node)                    \
            DiffNodeCtor(type_value, value, left_node, right_node)

#define DIFFN(curr_node)                                                   \
            DifferentiateNode(curr_node, var)

            
#define NCOPY(curr_node)                                                   \
            TreeNodeCopy(curr_node)


struct TreeNode *DiffNodeCtor(TypeDiffArg type_value,     struct Diff_elem_t *value, 
                              struct TreeNode *left_node, struct TreeNode *right_node)
{
    ERROR_CHECK(value == NULL, NULL);

    struct DiffNode *diff_node = (struct DiffNode *) calloc(1, sizeof(struct DiffNode));
    ERROR_CHECK(diff_node == NULL, NULL);

    diff_node->type_arg = type_value;
    diff_node->diff_arg = value;

    struct TreeNode *curr_node = TreeNodeCtor(diff_node, left_node, right_node);
    ERROR_CHECK(curr_node == NULL, NULL);

    return curr_node;
}


struct TreeNode *DifferentiateNode(struct TreeNode *curr_node, char var)
{
    ERROR_CHECK(curr_node == NULL, NULL);


    switch (curr_node->value->type_arg)
    {
    case TYPE_NUM : 
        {
            return NCTOR(TYPE_NUM, NUM(0), NULL, NULL);
            break;
        }

    case TYPE_VAR : 
        {
            if (curr_node->value->diff_arg->var == var)
            {
                return NCTOR(TYPE_NUM, NUM(1), NULL, NULL);
                break;
            }
            else
            {
                return NCTOR(TYPE_NUM, NUM(0), NULL, NULL);
                break;
            }
        }

    case TYPE_OP  : 
        {
            switch (curr_node->value->diff_arg->op)
            {        
                case PSN_OP :
                    {
                        return NULL;
                        break;
                    }    
                case ADD_OP : 
                    {   
                        return NCTOR(TYPE_OP, OP(ADD_OP),
                                    DIFFN(curr_node->left),
                                    DIFFN(curr_node->right));
                        break;
                    }

                case SUB_OP : 
                    {
                        return NCTOR(TYPE_OP, OP(SUB_OP),
                                    DIFFN(curr_node->left),
                                    DIFFN(curr_node->right));
                        break;
                    }

                case MUL_OP : 
                    {
                        return NCTOR(TYPE_OP, OP(ADD_OP), 
                                    NCTOR(TYPE_OP, OP(MUL_OP),
                                            DIFFN(curr_node->left), 
                                            NCOPY(curr_node->right)), 
                                    NCTOR(TYPE_OP, OP(MUL_OP),
                                            NCOPY(curr_node->left), 
                                            DIFFN(curr_node->right)));
                        break;
                    }

                case DIV_OP : 
                    {
                        return NCTOR(TYPE_OP, OP(DIV_OP), 
                                    NCTOR(TYPE_OP, OP(SUB_OP),
                                            NCTOR(TYPE_OP, OP(MUL_OP),
                                                DIFFN(curr_node->left), 
                                                NCOPY(curr_node->right)), 
                                            NCTOR(TYPE_OP, OP(MUL_OP),
                                                NCOPY(curr_node->left), 
                                                DIFFN(curr_node->right))),

                                    NCTOR(TYPE_OP, OP(POW_OP),
                                            NCOPY(curr_node->right), 
                                            NCTOR(TYPE_NUM, NUM(2), NULL, NULL)));
                        break;
                    }
                    
                case POW_OP :
                    {   
                        bool  left_is_const = true;
                        bool right_is_const = true;
                        int find_var_err = FindVar(curr_node->left, var, &left_is_const);
                        ERROR_CHECK(find_var_err, NULL);
                            find_var_err = FindVar(curr_node->right, var, &right_is_const);
                        ERROR_CHECK(find_var_err, NULL);  
                        printf("left = %d, right = %d\n", left_is_const, right_is_const);

                        if (!left_is_const && !right_is_const)
                            return  DIFFN(NCTOR(TYPE_OP, OP(POW_OP),
                                                NCTOR(TYPE_NUM, NUM(EXP), NULL, NULL),
                                                NCTOR(TYPE_OP, OP(MUL_OP),
                                                    NCTOR(TYPE_OP, OP(LN_OP),
                                                        NULL, NCOPY(curr_node->left)),
                                                    NCOPY(curr_node->right))));

                        else if (left_is_const && right_is_const)
                            return  NCTOR(TYPE_NUM, NUM(0), NULL, NULL);

                        else if (left_is_const)
                            return NCTOR(TYPE_OP, OP(MUL_OP),
                                        NCTOR(TYPE_OP, OP(MUL_OP),
                                            NCOPY(curr_node),
                                            NCTOR(TYPE_OP, OP(LN_OP),
                                                NULL, NCOPY(curr_node->left))),
                                        DIFFN(curr_node->right));
                        
                        else 
                            return NCTOR(TYPE_OP, OP(MUL_OP),
                                        NCTOR(TYPE_OP, OP(MUL_OP),
                                            NCOPY(curr_node->right),
                                            NCTOR(TYPE_OP, OP(POW_OP),
                                                NCOPY(curr_node->left), 
                                                NCTOR(TYPE_OP, OP(SUB_OP),
                                                    NCOPY(curr_node->right),
                                                    NCTOR(TYPE_NUM, NUM(1), NULL, NULL)))),
                                        DIFFN(curr_node->left));
                        break;
                    }

                case SIN_OP : 
                    {
                        return NCTOR(TYPE_OP, OP(MUL_OP),
                                    NCTOR(TYPE_OP, OP(COS_OP),
                                        NULL, NCOPY(curr_node->right)),
                                    DIFFN(curr_node->right));
                        break;
                    }

                case COS_OP : 
                    {
                        return NCTOR(TYPE_OP, OP(MUL_OP),
                                    NCTOR(TYPE_OP, OP(MUL_OP),
                                        NCTOR(TYPE_NUM, NUM(-1), NULL, NULL),
                                        NCTOR(TYPE_OP, OP(SIN_OP),
                                            NULL, NCOPY(curr_node->right))),
                                    DIFFN(curr_node->right));
                        break;
                    }

                case TAN_OP  : 
                    {
                        return NCTOR(TYPE_OP, OP(MUL_OP),
                                    NCTOR(TYPE_OP, OP(DIV_OP),
                                        NCTOR(TYPE_NUM, NUM(1), NULL, NULL),
                                        NCTOR(TYPE_OP, OP(POW_OP),
                                                NCTOR(TYPE_OP, OP(COS_OP),
                                                    NULL, NCOPY(curr_node->right)),
                                                NCTOR(TYPE_NUM, NUM(2), NULL, NULL))),
                                    DIFFN(curr_node->right));
                        break;
                    }

                case ASIN_OP: 
                    {
                        return NCTOR(TYPE_OP, OP(MUL_OP),
                                    NCTOR(TYPE_OP, OP(DIV_OP),
                                        NCTOR(TYPE_NUM, NUM(1), NULL, NULL),
                                        NCTOR(TYPE_OP, OP(POW_OP),
                                                NCTOR(TYPE_OP, OP(SUB_OP),
                                                    NCTOR(TYPE_NUM, NUM(1), NULL, NULL),
                                                    NCTOR(TYPE_OP, OP(POW_OP),
                                                        NCOPY(curr_node->right),
                                                        NCTOR(TYPE_NUM, NUM(2), NULL, NULL))),
                                                NCTOR(TYPE_NUM, NUM(0.5), NULL, NULL))),
                                    DIFFN(curr_node->right));
                        break;
                    }

                case ACOS_OP: 
                    {
                        return NCTOR(TYPE_OP, OP(MUL_OP),
                                    NCTOR(TYPE_OP, OP(DIV_OP),
                                        NCTOR(TYPE_NUM, NUM(-1), NULL, NULL),
                                        NCTOR(TYPE_OP, OP(POW_OP),
                                                NCTOR(TYPE_OP, OP(SUB_OP),
                                                    NCTOR(TYPE_NUM, NUM(1), NULL, NULL),
                                                    NCTOR(TYPE_OP, OP(POW_OP),
                                                        NCOPY(curr_node->right),
                                                        NCTOR(TYPE_NUM, NUM(2), NULL, NULL))),
                                                NCTOR(TYPE_NUM, NUM(0.5), NULL, NULL))),
                                    DIFFN(curr_node->right));
                        break;
                    }

                case ATAN_OP : 
                    {   
                        return NCTOR(TYPE_OP, OP(MUL_OP),
                                    NCTOR(TYPE_OP, OP(DIV_OP),
                                        NCTOR(TYPE_NUM, NUM(1), NULL, NULL),
                                        NCTOR(TYPE_OP, OP(ADD_OP),
                                            NCTOR(TYPE_NUM, NUM(1), NULL, NULL),
                                            NCTOR(TYPE_OP, OP(POW_OP),
                                                NCOPY(curr_node->right),
                                                NCTOR(TYPE_NUM, NUM(2), NULL, NULL)))),
                                    DIFFN(curr_node->right));
                        break;
                    }

                case SINH_OP : 
                    {
                        return NCTOR(TYPE_OP, OP(MUL_OP),
                                    NCTOR(TYPE_OP, OP(COSH_OP),
                                        NULL, NCOPY(curr_node->right)),
                                    DIFFN(curr_node->right));
                        break;
                    }

                case COSH_OP : 
                    {
                        return NCTOR(TYPE_OP, OP(MUL_OP),
                                    NCTOR(TYPE_OP, OP(SINH_OP),
                                        NULL, NCOPY(curr_node->right)),
                                    DIFFN(curr_node->right));
                        break;
                    }

                case TANH_OP  : 
                    {
                        return NCTOR(TYPE_OP, OP(MUL_OP),
                                    NCTOR(TYPE_OP, OP(DIV_OP),
                                        NCTOR(TYPE_NUM, NUM(1), NULL, NULL),
                                        NCTOR(TYPE_OP, OP(POW_OP),
                                                NCTOR(TYPE_OP, OP(COSH_OP),
                                                    NULL, NCOPY(curr_node->right)),
                                                NCTOR(TYPE_NUM, NUM(2), NULL, NULL))),
                                    DIFFN(curr_node->right));
                        break;
                    }

                case LN_OP  : 
                    {
                        return NCTOR(TYPE_OP, OP(MUL_OP),
                                    NCTOR(TYPE_OP, OP(DIV_OP),
                                        NCTOR(TYPE_NUM, NUM(1), NULL, NULL),
                                        NCOPY(curr_node->right)),
                                    DIFFN(curr_node->right));
                        break;
                    }

                default     : 
                    {   
                        return NULL;
                        break;
                    }
            }
            break;
        }

    case TYPE_PSN : 
        {
            return NULL;
            break;
        }
    
    default:        
        {
            return NULL;
            break;
        }
    }

    return NULL;
}

int FindVar(struct TreeNode *curr_node, char diff_var, bool *is_const)
{
    ERROR_CHECK(curr_node == NULL, ERROR_NULL_PTR); 

    if (curr_node->value->type_arg == TYPE_VAR && 
        curr_node->value->diff_arg->var == diff_var)
    {
        *is_const = false;
        return SUCCESS;        
    }

    if (curr_node->left != NULL)
    {
        int find_var_err = FindVar(curr_node->left, diff_var, is_const);
        ERROR_CHECK(find_var_err, ERROR_FIND_VAR);
    }

    if (curr_node->right != NULL)
    {
        int find_var_err = FindVar(curr_node->right, diff_var, is_const);
        ERROR_CHECK(find_var_err, ERROR_FIND_VAR);
    }

    return SUCCESS;       
}

int FrameVar(struct TreeNode *curr_node, char diff_var, diff_num_t value)
{
    ERROR_CHECK(curr_node == NULL, ERROR_NULL_PTR); 

    if (curr_node->value->type_arg == TYPE_VAR && 
        curr_node->value->diff_arg->var == diff_var)
    {
        curr_node->value->diff_arg->var = PSN_VAR;
        curr_node->value->diff_arg->num = value;
        curr_node->value->type_arg = TYPE_NUM;

        return SUCCESS;        
    }

    if (curr_node->left != NULL)
    {
        int find_var_err = FrameVar(curr_node->left, diff_var, value);
        ERROR_CHECK(find_var_err, ERROR_FIND_VAR);
    }

    if (curr_node->right != NULL)
    {
        int find_var_err = FrameVar(curr_node->right, diff_var, value);
        ERROR_CHECK(find_var_err, ERROR_FIND_VAR);
    }

    return SUCCESS;       
}


int ReviseParentValue(struct TreeNode *curr_node, struct TreeNode *parent_node)
{
    ERROR_CHECK(curr_node == NULL, ERROR_NULL_PTR);

    curr_node->parent = parent_node;

    if (curr_node->left != NULL)
    {
        int revise_par_val = ReviseParentValue(curr_node->left, curr_node);
        ERROR_CHECK(revise_par_val, ERROR_REVISE_PARENT_VALUE);
    }

    if (curr_node->right != NULL)
    {
        int revise_par_val = ReviseParentValue(curr_node->right, curr_node);
        ERROR_CHECK(revise_par_val, ERROR_REVISE_PARENT_VALUE);
    }

    return SUCCESS;
}


struct Diff_elem_t *DiffOpCtor(DiffOp op)
{
    struct Diff_elem_t *value = (struct Diff_elem_t *) calloc(1, sizeof(struct Diff_elem_t));
    ERROR_CHECK(value == NULL, NULL);

    value->op  = op;
    value->num = DIFF_NUM_PSN;
    value->var = PSN_VAR;

    return value;
}

struct Diff_elem_t *DiffNumCtor(diff_num_t num)
{
    struct Diff_elem_t *value = (struct Diff_elem_t *) calloc(1, sizeof(struct Diff_elem_t));
    ERROR_CHECK(value == NULL, NULL);

    value->op  = PSN_OP;
    value->num = num;
    value->var = PSN_VAR;

    return value;
}

struct Diff_elem_t *DiffVarCtor(char var)
{
    struct Diff_elem_t *value = (struct Diff_elem_t *) calloc(1, sizeof(struct Diff_elem_t));
    ERROR_CHECK(value == NULL, NULL);

    value->op  = PSN_OP;
    value->num = DIFF_NUM_PSN;
    value->var = var;

    return value;
}

struct Diff_elem_t *DiffPsnArgCtor(void)
{
    struct Diff_elem_t *value = (struct Diff_elem_t *) calloc(1, sizeof(struct Diff_elem_t));
    ERROR_CHECK(value == NULL, NULL);

    value->op  = PSN_OP;
    value->num = DIFF_NUM_PSN;
    value->var = PSN_VAR;

    return value;
}


#define NODE_NUM(node)                       \
            node->value->diff_arg->num

#define NODE_OP(node)                        \
            node->value->diff_arg->op

#define NCTOR_UNAR_NUM(oper)                        \
            NCTOR(TYPE_NUM, NUM(oper##(NODE_NUM(right_node))), NULL, NULL)

#define NCTOR_OPER_NUM(oper)                        \
            NCTOR(TYPE_NUM, NUM(NODE_NUM(left_node) oper NODE_NUM(right_node)), NULL, NULL)

#define SIMPLIFY_ITER(code)                                      \
            struct TreeNode *new_node = code;                    \
            int node_dtor_err = TreeNodeDtor(curr_node);         \
            ERROR_CHECK(node_dtor_err, NULL);                    \
            return new_node




struct TreeNode *SimplifyExpression(struct TreeNode *curr_node)
{
    ERROR_CHECK(curr_node == NULL, NULL);

    struct TreeNode* prev_left = curr_node->left;
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
    
    ERROR_CHECK(curr_node->value->type_arg != TYPE_OP, NULL);

    if (left_node == NULL && right_node != NULL &&
        right_node->value->type_arg == TYPE_NUM)
    {
        switch (curr_node->value->diff_arg->op)
        {
            case SIN_OP  :  
                {
                    SIMPLIFY_ITER(NCTOR(TYPE_NUM, NUM(sin(NODE_NUM(right_node))), NULL, NULL));
                    break;
                }

            case COS_OP  :  
                {
                    SIMPLIFY_ITER(NCTOR(TYPE_NUM, NUM(cos(NODE_NUM(right_node))), NULL, NULL));
                    break;
                }

            case TAN_OP  :  
                {
                    SIMPLIFY_ITER(NCTOR(TYPE_NUM, NUM(tan(NODE_NUM(right_node))), NULL, NULL));
                    break;
                }

            case ASIN_OP  :  
                {
                    SIMPLIFY_ITER(NCTOR(TYPE_NUM, NUM(asin(NODE_NUM(right_node))), NULL, NULL));
                    break;
                }

            case ACOS_OP  :  
                {
                    SIMPLIFY_ITER(NCTOR(TYPE_NUM, NUM(acos(NODE_NUM(right_node))), NULL, NULL));
                    break;
                }

            case ATAN_OP  :  
                {
                    SIMPLIFY_ITER(NCTOR(TYPE_NUM, NUM(atan(NODE_NUM(right_node))), NULL, NULL));
                    break;
                }

            case SINH_OP  :  
                {
                    SIMPLIFY_ITER(NCTOR(TYPE_NUM, NUM(sinh(NODE_NUM(right_node))), NULL, NULL));
                    break;
                }

            case COSH_OP  :  
                {
                    SIMPLIFY_ITER(NCTOR(TYPE_NUM, NUM(cosh(NODE_NUM(right_node))), NULL, NULL));
                    break;
                }

            case TANH_OP  :  
                {
                    SIMPLIFY_ITER(NCTOR(TYPE_NUM, NUM(tanh(NODE_NUM(right_node))), NULL, NULL));
                    break;
                }

            case LN_OP  :  
                {
                    SIMPLIFY_ITER(NCTOR(TYPE_NUM, NUM(log(NODE_NUM(right_node))), NULL, NULL));
                    break;
                }
            default     :
                    return NULL;
                    break;
        }
    }

    if (left_node != NULL && right_node != NULL &&
         left_node->value->type_arg == TYPE_NUM &&
        right_node->value->type_arg == TYPE_NUM)
    {
        switch (curr_node->value->diff_arg->op)
        {
            case ADD_OP : 
                {
                    SIMPLIFY_ITER(NCTOR_OPER_NUM(+));
                    break;
                }

            case SUB_OP : 
                {
                    SIMPLIFY_ITER(NCTOR_OPER_NUM(-));
                    break;
                } 

            case MUL_OP : 
                {
                    SIMPLIFY_ITER(NCTOR_OPER_NUM(*));
                    break;
                }

            case DIV_OP : 
                {
                    SIMPLIFY_ITER(NCTOR_OPER_NUM(/));
                    break;
                }

            case POW_OP : 
                {
                    SIMPLIFY_ITER(NCTOR(TYPE_NUM, NUM(pow(NODE_NUM(left_node), 
                                                          NODE_NUM(right_node))), NULL, NULL));
                    break;
                }
            default : return NULL;
                    break;
        }
    }

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

    curr_node->left  = left_node;
    curr_node->right = right_node;

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

    if (curr_node->left  != prev_left ||
           curr_node->right != prev_right)
        return SimplifyExpression(curr_node);
    
    return curr_node;
}
