#ifndef DIFF_H_INCLUDED
#define DIFF_H_INCLUDED

//#include "../onegin_functions/include/onegin_libraries.h"
#include "../tree_functions/include/tree.h"
#include "common.h"
#include "math.h"

#define OP(op)                \
            DiffOpCtor(op)

#define NUM(num)              \
            DiffNumCtor(num)

#define VAR(var)              \
            DiffVarCtor(var)

#define NCTOR(type_value, value, left_node, right_node)                    \
            DiffNodeCtor(type_value, value, left_node, right_node)
            
#define NCOPY(curr_node)                                                   \
            TreeNodeCopy(curr_node)

enum DiffError
{
    ERROR_INCORRECT_DIFF_TYPE_ARG = 1,
    ERROR_INCORRECT_DIFF_OP       = 2,
    ERROR_DIFF_NODE_CTOR          = 3,
    ERROR_DIFF_PSN_ARG_CTOR       = 4,
    ERROR_NO_DOT_IN_ARG           = 5,
    ERROR_FIND_VAR                = 6,
    ERROR_REVISE_PARENT_VALUE     = 7,
    ERROR_SIMPLIFY_EXPRESSION     = 8,
    ERROR_DIFFERENTIATE_NODE      = 9,
    ERROR_REVISE_PARENT_NODE      = 10,
    ERROR_FRAME_VAR               = 11,
    ERROR_STAT                    = 12,
    ERROR_TEX_SERIALIZE_NODE      = 13,
    ERROR_COUNT_VAL_AT_POINT      = 14,
    ERROR_EXPAND_TAYLOR           = 15,
    ERROR_LAB_DESERIALIZE         = 16,
    ERROR_ADD_TEX_LAB             = 17,
    ERROR_MAKE_GRAPHIC_FILE       = 18,
    ERROR_GET_START_VALUES        = 19,
};


enum TypeDiffArg
{
    TYPE_PSN = 0,
    TYPE_NUM = 1,
    TYPE_OP  = 2,
    TYPE_VAR = 3,
};

enum DiffOp
{
    PSN_OP  = 0,
    ADD_OP  = 1,
    SUB_OP  = 2,
    MUL_OP  = 3,
    DIV_OP  = 4,
    POW_OP  = 5,
    SIN_OP  = 6,
    COS_OP  = 8,
    TAN_OP  = 9,
    ASIN_OP = 10,
    ACOS_OP = 11,
    ATAN_OP = 12,
    SINH_OP = 13,
    COSH_OP = 14,
    TANH_OP = 15,
    LN_OP   = 16,
};

typedef double diff_num_t;
const diff_num_t DIFF_NUM_PSN = 0;
const char PSN_VAR = '\0';

struct DiffNode
{
    TypeDiffArg type_arg         = TYPE_PSN;
    struct Diff_elem_t *diff_arg = NULL;
};

struct Diff_elem_t
{
    DiffOp     op  = PSN_OP;
    diff_num_t num = DIFF_NUM_PSN;
    char var = '\0';
};

struct DiffVar
{
    char var = '\0';
    diff_num_t point = 0;
    diff_num_t err_val = 0;
};

const diff_num_t PI  = 3.1415926535;
const diff_num_t EXP = 2.7182818284;
const diff_num_t EPS = 1e-7;


#define COMPARE_DOUBLE(value, num)                               \
            ((value) - EPS <= (num) && (value) + EPS >= (num))

#include "diff_io.h"
#include "parsing.h"
#include "optimizing.h"

struct TreeNode *DiffNodeCtor(TypeDiffArg type_value,     struct Diff_elem_t *value, 
                              struct TreeNode *left_node, struct TreeNode *right_node);

struct TreeNode *DifferentiateNode(struct TreeNode *curr_node, char var);

int FindVar(struct TreeNode *curr_node, char diff_var, bool *is_const);
int FrameVar(struct TreeNode *curr_node, char diff_var, diff_num_t value);
int ReviseParentValue(struct TreeNode *curr_node, struct TreeNode *parent_node);

int CountValueAtPoint(struct TreeNode *expr_node, char var, diff_num_t point,
                      diff_num_t *value);

int CountValueForManyVars(struct TreeNode *expr_node, int var_amount,
                          struct DiffVar  *vars, diff_num_t *value);

#endif //DIFF_H_INCLUDED