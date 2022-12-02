#ifndef DIFF_IO_H_INCLUDED
#define DIFF_IO_H_INCLUDED

#include "diff.h"

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


int CreateMatanManual(struct Tree *expr, char var, diff_num_t point, 
                      unsigned int diff_dgr, unsigned int taylor_dgr);


int GetStartValues(double *point, int *diff_dgr, int *taylor_dgr);
int TexSerializeNode(FILE *tex_f, const struct TreeNode *curr_node);

struct Diff_elem_t *DiffOpCtor(DiffOp op);
struct Diff_elem_t *DiffNumCtor(diff_num_t num);
struct Diff_elem_t *DiffVarCtor(char var);
struct Diff_elem_t *DiffPsnArgCtor(void);

int ExpandInTaylor(FILE *file_o, struct TreeNode *curr_node, 
                   char var, diff_num_t point, unsigned int degree);

int AddTexLabError(FILE *tex_f);
struct Tree *LabDeserialize(const char *input_file_name, char *main_var,
                            int *var_amount, struct DiffVar **vars);

int MakeGraphicFile(struct TreeNode *curr_node);




#endif //DIFF_IO_H_INCLUDED