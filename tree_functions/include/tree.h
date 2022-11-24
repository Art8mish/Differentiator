#ifndef TREE_H_INCLUDED
#define TREE_H_INCLUDED

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <windows.h>
#include <locale.h>
#include <ctype.h>

#include "../../onegin_functions/include/onegin_libraries.h"
#include "common.h"

#define CREATE_TREE_NODE_VALUE(val, err)                                                             \
            do                                                                                          \
            {                                                                                           \
                (val) = (struct DiffNode *) calloc(1, sizeof(struct DiffNode));                     \
                ERROR_CHECK((val) == NULL, err);                                                    \
                (val)->type_arg = TYPE_PSN;                                                       \
                (val)->diff_arg = DiffPsnArgCtor();                                               \
                ERROR_CHECK((val)->diff_arg == NULL, err);                                        \
            } while (false)

#define COPY_TREE_VAl(val, copying_val)                                   \
        do                                                                \
        {                                                                 \
            val->type_arg      = copying_val->type_arg;                   \
            val->diff_arg->op  = copying_val->diff_arg->op;               \
            val->diff_arg->num = copying_val->diff_arg->num;              \
            val->diff_arg->var = copying_val->diff_arg->var;              \
        }while  (false) 

#define FREE_TREE_NODE_VALUE(curr_node)                                  \
            do                                                           \
            {                                                            \
                if (curr_node->value != NULL)                            \
                {                                                        \
                    free((void *)curr_node->value->diff_arg);            \
                    free((void *)curr_node->value);                      \
                }                                                        \
            } while (false)

#define CHECK_TREE_ELEM(elem, error)                                     \
            do                                                           \
            {                                                            \
                ERROR_CHECK(elem == NULL, error);                        \
            } while (false)          

#define COMPARE_TREE_ELEM(frst_value, scnd_value)                        \
            strcmp(frst_value, scnd_value) == 0

#define TREEDUMP(tree, comment)                                           \
            TreeDump(tree, comment, true, __FILE__, __LINE__, __FUNCTION__)

#include "../../include/diff.h"
typedef struct DiffNode *tree_elem_t;
#define TREE_VAL_PSN NULL
#define TREE_SPECIFIER "%p"

#define READABLE_SYMB "%[a-zA-Z1234567890 -+*/^]"
#define MAIN_VARIABLE 'x'

static const char *const           TREE_GRAPH_PATH = "tree_functions/tree_graph.gv";
static const char *const            TREE_DUMP_PATH = "tree_functions/io/tree_dump.txt";
static const char *const             TREE_HTM_PATH = "tree_functions/io/tree_graphs.htm"; 
static const char *const   TREE_SERIALIZATION_PATH = "io/tree_serial.txt";
static const char *const TREE_DESERIALIZATION_PATH = "io/tree_deserial.txt";

const size_t MAX_TREE_STR_SIZE = 101;
const size_t MAX_COMMAND_SIZE  = 101;
const size_t MAX_ARG_LEN       = 20;

#ifndef PTR_LIST_H_INCLUDED
enum ListError
{
    SUCCESS                   = 0,
    ERROR_CALLOC              = 3,
    ERROR_NULL_PTR            = 4,
    ERROR_OPENING_FILE        = 9,
    ERROR_CLOSING_FILE        = 10,
    ERROR_CREATE_GRAPH        = 15,  
    ERROR_ADD_GRAPH           = 16,
};
#endif

enum TreeError
{
    ERROR_TREE                   = 1,
    ERROR_CREATE_NODES           = 2,
    ERROR_TREE_VERIFICATION      = 5,
    ERROR_NODE_DTOR              = 6,
    ERROR_TREE_INCORRECT_OP      = 8,
    ERROR_TREE_CHECK_ERROR       = 11,
    ERROR_SAVE_NODE              = 12,
    ERROR_TREE_INSERT            = 13,
    ERROR_TREE_REMOVE            = 14,
    ERROR_READ_NODE              = 15,
    ERROR_SYNTAX                 = 16,
    ERROR_WRONG_TREE_INSERT_PATH = 17,
    ERROR_READ_VALUE             = 18,
    ERROR_INCORRECT_TREE_ELEM    = 19,
    ERROR_TREE_NODE_CTOR         = 20,
    ERROR_TREE_TIE               = 21,
    ERROR_CONVERT_STR_TO_NUM     = 22,
    
};

enum TreeInsertPath
{
    TREE_INSERT_FIRST = 0,
    TREE_INSERT_LEFT  = 1,
    TREE_INSERT_RIGHT = 2,
};

enum TreeErrorMask
{
    //for TreeVerification 
};

struct TreeNode
{
    tree_elem_t value       = TREE_VAL_PSN;
    struct TreeNode *left   = NULL;
    struct TreeNode *right  = NULL;
    struct TreeNode *parent = NULL;
};

struct Tree
{
    struct TreeNode *root = NULL;

    int error = 0;
};

struct Tree *TreeCtor(void);
int TreeDtor(struct Tree *tree);
int TreeNodeDtor(struct Tree *tree, struct TreeNode *curr_node);
struct TreeNode *TreeNodeCtor(tree_elem_t arg, struct TreeNode *left_node, struct TreeNode *right_node);

struct TreeNode *TreeNodeCopy(struct TreeNode *curr_node);

int TreeNodeTie(struct Tree *tree, struct TreeNode *parent_node,
                struct TreeNode *curr_node, int insert_path);

int TreeInsertLatin(struct Tree *tree, struct TreeNode *parent_node, tree_elem_t arg);
int TreeInsertArabic(struct Tree *tree, struct TreeNode *parent_node, tree_elem_t arg);

int TreeInsert(struct Tree *tree, struct TreeNode *parent_node,
               tree_elem_t arg, TreeInsertPath insert_path);
    
int TreeRemove(struct Tree *tree, struct TreeNode *node_ptr);
    
int TreeVerification(struct Tree *tree);
int TreeCheckError(struct Tree *tree);

int TreeDump(const struct Tree *tree, const char *comment = NULL, bool debug = false,
             const char *file_name = NULL, int line_num = 0, const char *func_name = NULL);
int CreateTreeGraph(const struct Tree *tree);
int SaveTreeGraphPng(void);
int AddTreeGraphPng(int graph_counter);
int CreateTreeNodes(const struct TreeNode *curr_node, FILE *graph_f);

int TreeSerialize(const struct Tree *tree);
int SerializeNode(const struct TreeNode *curr_node, FILE *tree_f);

struct Tree *TreeDeserialize(const char *input_file_name);
int DeserializeNode(struct Tree *new_tree, struct TreeNode **parent_node, 
                    char **buf, int recur_level);

//int ReadValue(tree_elem_t *pt_val, char **buf);
int ConvertStrToNum(const char **string, double *num);


#endif //TREE_H_INCLUDED