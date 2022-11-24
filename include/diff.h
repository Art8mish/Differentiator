#ifndef DIFF_H_INCLUDED
#define DIFF_H_INCLUDED

//#include "../onegin_functions/include/onegin_libraries.h"
#include "../tree_functions/include/tree.h"
#include "common.h"

typedef double diff_num_t;
const diff_num_t DIFF_NUM_PSN = 0;

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
    DGR_OP  = 5,
    SIN_OP  = 6,
    COS_OP  = 8,
    TG_OP   = 9,
    CTG_OP  = 10,
    ASIN_OP = 11,
    ACOS_OP = 12,
    ATG_OP  = 13,
    ACTG_OP = 14,
    LN_OP   = 15,
};

enum DiffVar
{
      PSN_VAR = 0,
        X_VAR = 1,
    CONST_VAR = 2,
};

struct Diff_elem_t
{
    DiffOp     op  = PSN_OP;
    diff_num_t num = DIFF_NUM_PSN;
    DiffVar    var = CONST_VAR;
};


struct DiffNode
{
    TypeDiffArg type_arg         = TYPE_PSN;
    struct Diff_elem_t *diff_arg = NULL;
};


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
};

const diff_num_t PI  = 3.1415926535;
const diff_num_t EXP = 2.7182818284;
const diff_num_t EPS = 1e-7;

struct TreeNode *DiffNodeCtor(TypeDiffArg type_value,     struct Diff_elem_t *value, 
                              struct TreeNode *left_node, struct TreeNode *right_node);

struct TreeNode *DifferentiateNode(struct TreeNode *curr_node);

int FindVar(struct TreeNode *curr_node, bool *is_const);
int ReviseParentValue(struct TreeNode *curr_node, struct TreeNode *parent_node);

struct Diff_elem_t *DiffOpCtor(DiffOp op);
struct Diff_elem_t *DiffNumCtor(diff_num_t num);
struct Diff_elem_t *DiffVarCtor(DiffVar var);
struct Diff_elem_t *DiffPsnArgCtor(void);

struct TreeNode *ReadExpression(const char *str);
struct TreeNode *GetAdd(const char **buf);
struct TreeNode *GetMul(const char **buf);
struct TreeNode *GetDgr(const char **buf);
struct TreeNode *GetUnarFunc(const char **buf);
struct TreeNode *GetBrackets(const char **buf);
struct TreeNode *GetArg(const char **buf);

#endif //DIFF_H_INCLUDED