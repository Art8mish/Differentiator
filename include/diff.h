#ifndef DIFF_H_INCLUDED
#define DIFF_H_INCLUDED

//#include "../onegin_functions/include/onegin_libraries.h"
#include "../tree_functions/include/tree.h"
#include "common.h"
#include "math.h"

#define SKIP_SPACE(buf, success)                            \
            while(isspace(*(buf)) || *(buf) == '\0')        \
            {                                               \
                (buf)++;                                    \
                if (*(buf) == EOF)                          \
                    return success;                         \
            }

#define SKIP_SPACE_STR(buf)                                 \
            while(isspace(*(buf)))                          \
            {                                               \
                (buf)++;                                    \
                if (*(buf) == EOF)                          \
                    return NULL;                            \
            }

#define LEX_SKIP_SPACE(buf, buf_skip_len)                   \
            while(isspace(*(buf)))                          \
            {                                               \
                (buf)++;                                    \
                (buf_skip_len)++;                           \
                if (*(buf) == EOF)                          \
                    return NULL;                            \
            }

#define LEX_ERROR(cond, error, buf_pos)                     \
            do                                              \
            {                                               \
                SOFT_ASSERT(cond);                          \
                printf("ERROR in symb %d\n", buf_pos);      \
                if (cond)                                   \
                    return error;                           \
            } while(false)


typedef double diff_num_t;
const diff_num_t DIFF_NUM_PSN = 0;
const char PSN_VAR = '\0';

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

struct DiffVar
{
    char var = '\0';
    diff_num_t point = 0;
    diff_num_t err_val = 0;
};


struct Diff_elem_t
{
    DiffOp     op  = PSN_OP;
    diff_num_t num = DIFF_NUM_PSN;
    char var = '\0';
};


struct DiffNode
{
    TypeDiffArg type_arg         = TYPE_PSN;
    struct Diff_elem_t *diff_arg = NULL;
};


struct DiffLexicalElem
{
    int type_arg              = TYPE_PSN;
    struct Diff_elem_t arg    = { };
    unsigned int symb_pos_num = 0;
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

const diff_num_t PI  = 3.1415926535;
const diff_num_t EXP = 2.7182818284;
const diff_num_t EPS = 1e-7;

static const char *const             TEX_FILE_PATH = "io/metodichka.tex";
static const char *const        FOREWORD_FILE_PATH = "io/foreword.tex";
static const char *const  LAB_DESERIALIZATION_PATH = "io/lab_deserial.txt";
static const char *const     GRAPHIC_GNU_FILE_PATH = "io/graphic.gnuplot";
static const char *const         GRAPHIC_FILE_PATH = "graphic.png";

const int MAX_FORMULA_LENGTH        = 150;

const int MAX_PRINTED_DIFF_DGR      = 2;
const int MAX_COMMAND_LEN           = 30;
const int MAX_SYMB_IN_LINE          = 30;
const int MAX_TAYLOR_MEMBER_IN_LINE = 3;

#define COMPARE_DOUBLE(value, num)                               \
            ((value) - EPS <= (num) && (value) + EPS >= (num))

struct TreeNode *DiffNodeCtor(TypeDiffArg type_value,     struct Diff_elem_t *value, 
                              struct TreeNode *left_node, struct TreeNode *right_node);

struct TreeNode *DifferentiateNode(struct TreeNode *curr_node, char var);

int FindVar(struct TreeNode *curr_node, char diff_var, bool *is_const);
int FrameVar(struct TreeNode *curr_node, char diff_var, diff_num_t value);
int ReviseParentValue(struct TreeNode *curr_node, struct TreeNode *parent_node);

struct Diff_elem_t *DiffOpCtor(DiffOp op);
struct Diff_elem_t *DiffNumCtor(diff_num_t num);
struct Diff_elem_t *DiffVarCtor(char var);
struct Diff_elem_t *DiffPsnArgCtor(void);

struct TreeNode *ReadExpression(const char *str);
struct TreeNode *GetAdd(const char **buf);
struct TreeNode *GetMul(const char **buf);
struct TreeNode *GetPow(const char **buf);
struct TreeNode *GetUnarFunc(const char **buf);
struct TreeNode *GetBrackets(const char **buf);
struct TreeNode *GetArg(const char **buf);

int CreateMatanManual(struct Tree *expr, char var, diff_num_t point, 
                      unsigned int diff_dgr, unsigned int taylor_dgr);

int CountValueAtPoint(struct TreeNode *expr_node, char var, diff_num_t point,
                      diff_num_t *value);
struct TreeNode *SimplifyExpression(struct TreeNode *curr_node);

int TexSerializeNode(FILE *tex_f, const struct TreeNode *curr_node);
int ExpandInTaylor(FILE *file_o, struct TreeNode *curr_node, 
                   char var, diff_num_t point, unsigned int degree);

int AddTexLabError(FILE *tex_f);
struct Tree *LabDeserialize(const char *input_file_name, char *main_var,
                            int *var_amount, struct DiffVar **vars);
int CountValueForManyVars(struct TreeNode *expr_node, int var_amount,
                          struct DiffVar  *vars, diff_num_t *value);

int MakeGraphicFile(struct TreeNode *curr_node);

int GetStartValues(double *point, int *diff_dgr, int *taylor_dgr);

#endif //DIFF_H_INCLUDED