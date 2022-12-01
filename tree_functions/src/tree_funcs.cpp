
#include "../include/tree.h"

#define VERIFICATION(tree)                                              \
            do                                                          \
            {                                                           \
                int verification_err = TreeVerification(tree);          \
                ERROR_CHECK(verification_err, ERROR_TREE_VERIFICATION); \
                if (tree->error != 0)                                   \
                    return ERROR_TREE;                                  \
            } while(false)

#define INSPECT_TREE(cond, err_code)                \
            do                                      \
            {                                       \
                if (cond)                           \
                    tree->error |= (err_code);      \
            } while(false) 
                        

struct Tree *TreeCtor(void)
{
    struct Tree *tree = (struct Tree *) calloc(1, sizeof(struct Tree));
    ERROR_CHECK(tree == NULL, NULL);

    tree->root = NULL;
    
    return tree;
}

int TreeDtor(struct Tree *tree)
{
    ERROR_CHECK(tree == NULL, ERROR_NULL_PTR);

    VERIFICATION(tree);

    int node_dtor_err = TreeNodeDtor(tree->root);
    ERROR_CHECK(node_dtor_err, ERROR_TREE_NODE_DTOR);

    free(tree);

    return SUCCESS;
}

int TreeNodeDtor(struct TreeNode *curr_node)
{
    if (curr_node == NULL)
        return SUCCESS;

    if (curr_node->left != NULL)
    {
        int node_dtor_err = TreeNodeDtor(curr_node->left);
        ERROR_CHECK(node_dtor_err, ERROR_TREE_NODE_DTOR);
    }

    if (curr_node->right != NULL)
    {
        int node_dtor_err = TreeNodeDtor(curr_node->right);
        ERROR_CHECK(node_dtor_err, ERROR_TREE_NODE_DTOR);
    }

    FREE_TREE_NODE_VALUE(curr_node);
    free(curr_node);

    return SUCCESS;
}

struct TreeNode *TreeNodeCtor(tree_elem_t arg, struct TreeNode *left_node, struct TreeNode *right_node)
{
    CHECK_TREE_ELEM(arg, NULL);

    struct TreeNode *new_node = (struct TreeNode *) calloc(1, sizeof(struct TreeNode));
    ERROR_CHECK(new_node == NULL, NULL);
    
    new_node->value  = arg;

    new_node->left = left_node;
    if (left_node != NULL)
        left_node->parent = new_node;

    new_node->right = right_node;
    if (right_node != NULL)
        right_node->parent = new_node;

    new_node->parent = NULL;

    return new_node;
}

struct TreeNode *TreeNodeCopy(struct TreeNode *curr_node)
{
    if (curr_node == NULL)
        return NULL;

    struct TreeNode *left_node = NULL;
    if (curr_node->left != NULL)
    {
        left_node = TreeNodeCopy(curr_node->left);
        ERROR_CHECK(left_node == NULL, NULL);
    }

    struct TreeNode *right_node = NULL;
    if (curr_node->right != NULL)
    {
        right_node = TreeNodeCopy(curr_node->right);
        ERROR_CHECK(right_node == NULL, NULL);
    }

    tree_elem_t val = NULL;
    CREATE_TREE_NODE_VALUE(val, NULL);
    COPY_TREE_VAl(val, curr_node->value);

    struct TreeNode *new_node = TreeNodeCtor(val, left_node, right_node);
    ERROR_CHECK(new_node == NULL, NULL);

    return new_node;
}

int TreeNodeTie(struct TreeNode *parent_node,
                struct TreeNode *curr_node, TreeTiePath tie_path)
{
    ERROR_CHECK(parent_node == NULL, ERROR_NULL_PTR);
    ERROR_CHECK(curr_node   == NULL, ERROR_NULL_PTR);

    int tree_node_dtor_err = 0;
    switch (tie_path)
    {
        case TREE_TIE_LEFT  : 
            {
                tree_node_dtor_err = TreeNodeDtor(parent_node->left);
                ERROR_CHECK(tree_node_dtor_err, ERROR_TREE_NODE_DTOR);

                parent_node->left = curr_node;
                curr_node->parent = parent_node;
                break;
            }

        case TREE_TIE_RIGHT : 
            {
                tree_node_dtor_err = TreeNodeDtor(parent_node->right);
                ERROR_CHECK(tree_node_dtor_err, ERROR_TREE_NODE_DTOR);

                parent_node->right = curr_node;
                curr_node->parent  = parent_node;
                break;
            }
    
        default:    return ERROR_WRONG_TREE_INSERT_PATH;
                    break;
    }

    return SUCCESS;
}

int TreeInsertLatin(struct TreeNode *parent_node, tree_elem_t arg)
{
    ERROR_CHECK(parent_node == NULL, ERROR_NULL_PTR);
    CHECK_TREE_ELEM(arg, ERROR_INCORRECT_TREE_ELEM);

    ERROR_CHECK(parent_node->left  == NULL && 
                parent_node->right != NULL, ERROR_TREE_INCORRECT_OP);

    if (parent_node->left != NULL && parent_node->right != NULL)
        return SUCCESS;    

    struct TreeNode *curr_node = TreeNodeCtor(arg, NULL, NULL);
    ERROR_CHECK(curr_node == NULL, ERROR_TREE_NODE_CTOR);
    curr_node->parent = parent_node;

    if (parent_node->left == NULL)
    {
        int tree_tie_err = TreeNodeTie(parent_node, curr_node, TREE_TIE_LEFT);
        ERROR_CHECK(tree_tie_err, ERROR_TREE_TIE);

        return SUCCESS;
    }

    if (parent_node->right == NULL)
    {
        int tree_tie_err = TreeNodeTie(parent_node, curr_node, TREE_TIE_RIGHT);
        ERROR_CHECK(tree_tie_err, ERROR_TREE_TIE);
    }

    return SUCCESS;
}

int TreeInsertArabic(struct TreeNode *parent_node, tree_elem_t arg)
{
    ERROR_CHECK(parent_node == NULL, ERROR_NULL_PTR);
    CHECK_TREE_ELEM(arg, ERROR_INCORRECT_TREE_ELEM);

    ERROR_CHECK(parent_node->left  != NULL && 
                parent_node->right == NULL, ERROR_TREE_INCORRECT_OP);

    if (parent_node->left != NULL && parent_node->right != NULL)
        return SUCCESS;

    struct TreeNode *curr_node = TreeNodeCtor(arg, NULL, NULL);
    ERROR_CHECK(curr_node == NULL, ERROR_TREE_NODE_CTOR);
    curr_node->parent = parent_node;

    if (parent_node->right == NULL)
    {
        int tree_tie_err = TreeNodeTie(parent_node, curr_node, TREE_TIE_LEFT);
        ERROR_CHECK(tree_tie_err, ERROR_TREE_TIE);

        return SUCCESS;
    }

    if (parent_node->left == NULL)
    {
        int tree_tie_err = TreeNodeTie(parent_node, curr_node, TREE_TIE_RIGHT);
        ERROR_CHECK(tree_tie_err, ERROR_TREE_TIE);
    }

    return SUCCESS;
}

int TreeInsert(struct Tree *tree, struct TreeNode *parent_node,
               tree_elem_t arg, TreeInsertPath insert_path)
{
    ERROR_CHECK(tree == NULL, ERROR_NULL_PTR);
    CHECK_TREE_ELEM(arg, ERROR_INCORRECT_TREE_ELEM);

    VERIFICATION(tree);

    struct TreeNode *curr_node = TreeNodeCtor(arg, NULL, NULL);
    ERROR_CHECK(curr_node == NULL, ERROR_TREE_NODE_CTOR);
    curr_node->parent = parent_node;

    switch (insert_path)
    {
        case TREE_INSERT_FIRST : tree->root = curr_node;
                                 break;

        case TREE_INSERT_LEFT  : ERROR_CHECK(parent_node       == NULL, ERROR_NULL_PTR);
                                 ERROR_CHECK(parent_node->left != NULL, ERROR_TREE_INCORRECT_OP);
                                 parent_node->left  = curr_node;
                                 break;

        case TREE_INSERT_RIGHT : ERROR_CHECK(parent_node == NULL, ERROR_NULL_PTR);
                                 ERROR_CHECK(parent_node->right != NULL, ERROR_TREE_INCORRECT_OP);
                                 parent_node->right = curr_node;
                                 break;
        
        default                : return ERROR_WRONG_TREE_INSERT_PATH;
                                 break;
    }

    VERIFICATION(tree);    

    return SUCCESS;
}

int TreeVerification(struct Tree *tree)
{
    ERROR_CHECK(tree == NULL, ERROR_NULL_PTR);

    int tree_check_err = TreeCheckError(tree);
    ERROR_CHECK(tree_check_err, ERROR_TREE_CHECK_ERROR);
    
    if (tree->error != 0)
        TREEDUMP(tree, "ERROR IN VERIFICATION");

    return SUCCESS;
}


int TreeCheckError(struct Tree *tree)
{
    ERROR_CHECK(tree == NULL, ERROR_NULL_PTR);

    //INCPECT_TREE(CONDITION, ERROR_CODE)

    return SUCCESS;
}

int TreeSerialize(const struct Tree *tree)
{
    ERROR_CHECK(tree == NULL, ERROR_NULL_PTR);

    FILE *tree_f = fopen(TREE_SERIALIZATION_PATH, "a");
    ERROR_CHECK(tree_f == NULL, ERROR_OPENING_FILE);

    if (tree->root != NULL)
    {
        int save_node_err = SerializeNode(tree_f, tree->root);
        FILE_ERROR_CHECK(save_node_err, ERROR_SAVE_NODE, tree_f);
    }

    fprintf(tree_f, "\n");
    int fclose_err = fclose(tree_f);
    ERROR_CHECK(fclose_err != 0, ERROR_CLOSING_FILE);

    return SUCCESS;
}

int SerializeNode(FILE *tree_f, const struct TreeNode *curr_node)
{
    ERROR_CHECK(curr_node == NULL, ERROR_NULL_PTR);
    ERROR_CHECK(tree_f    == NULL, ERROR_NULL_PTR);

    if (curr_node->left != NULL)
    {
        fprintf(tree_f, "(");
        int serialize_node_err = SerializeNode(tree_f, curr_node->left);
        ERROR_CHECK(serialize_node_err, ERROR_SERIALIZE_NODE);
        fprintf(tree_f, ")");
    }

    switch (curr_node->value->type_arg)
    {
        case TYPE_PSN : fprintf(tree_f, " TYPE_PSN ");
                        break;
        case TYPE_NUM : if (COMPARE_DOUBLE(curr_node->value->diff_arg->num, EXP))
                            fprintf(tree_f, "%g", EXP);
                        else if (COMPARE_DOUBLE(curr_node->value->diff_arg->num, PI))
                            fprintf(tree_f, "%g", PI);
                        else
                            fprintf(tree_f, "%g", curr_node->value->diff_arg->num);
                        break;
        case TYPE_OP  :  
                switch (curr_node->value->diff_arg->op)
                {
                    case PSN_OP :   break;
                    case ADD_OP :   fprintf(tree_f, " + ");
                                    break;
                    case SUB_OP  :  fprintf(tree_f, " - ");
                                    break;
                    case MUL_OP  :  fprintf(tree_f, " * ");
                                    break;
                    case DIV_OP  :  fprintf(tree_f, " / ");
                                    break;
                    case POW_OP  :  fprintf(tree_f, " ** ");
                                    break;
                    case SIN_OP  :  fprintf(tree_f, "sin ");
                                    break;
                    case COS_OP  :  fprintf(tree_f, "cos ");
                                    break;
                    case TAN_OP  :  fprintf(tree_f, "tan ");
                                    break;
                    case ASIN_OP :  fprintf(tree_f, "asin ");
                                    break;
                    case ACOS_OP :  fprintf(tree_f, "acos ");
                                    break;
                    case ATAN_OP :  fprintf(tree_f, "atan ");
                                    break;
                    case SINH_OP :  fprintf(tree_f, "sinh ");
                                    break;
                    case COSH_OP :  fprintf(tree_f, "cosh ");
                                    break;
                    case TANH_OP :  fprintf(tree_f, "tanh ");
                                    break;
                    case LN_OP   :  fprintf(tree_f, "log ");
                                    break;
                    default:        fprintf(tree_f, "ERROR_OP");
                                    break;
                }
                break;

        case TYPE_VAR : fprintf(tree_f, "%c", curr_node->value->diff_arg->var);
                        break;
            
        default :   //fprintf(tree_f, " TYPE_ERROR ");
                    break;
    }

    if (curr_node->right != NULL)
    {
        fprintf(tree_f, "(");
        int serialize_node_err = SerializeNode(tree_f, curr_node->right);
        ERROR_CHECK(serialize_node_err, ERROR_SERIALIZE_NODE);
        fprintf(tree_f, ")");
    }

    return SUCCESS;
}

struct Tree *TreeDeserialize(const char *input_file_name)
{
    ERROR_CHECK(input_file_name == NULL, NULL);

    struct Tree *new_tree = TreeCtor();
    ERROR_CHECK(new_tree == NULL, NULL);

    struct WorkingField *onegin_context = CreateWorkingField(input_file_name);
    ERROR_CHECK(onegin_context == NULL, NULL);

    char *buf = onegin_context->chars_buffer;

    /*
    struct DiffLexicalElem *lex_structs = LexicalAnalisis(buf);
    ERROR_CHECK(lex_structs == NULL, NULL);*/
   
    printf("buf = %s\n", buf);
    new_tree->root = ReadExpression(buf);
    ERROR_CHECK(new_tree->root == NULL, NULL);

    return new_tree;    
}
/*
struct DiffLexicalElem *LexicalAnalisis(char *buf)
{
    ERROR_CHECK(buf == NULL, NULL);

    unsigned int buf_pos = 0;

    struct DiffLexicalElem *lex_structs = (struct DiffLexicalElem *) 
                                            calloc(MAX_FORMULA_LENGTH, sizeof(struct DiffLexicalElem));
    ERROR_CHECK(lex_structs == NULL, NULL);

    int index = 0;
    bool is_oper_now = false;
    while (*buf != '\0')
    {
        if (*buf == '+' && !is_oper_now) 
        {
            is_oper_now = true;
            lex_structs[index].type_arg = TYPE_OP;
            lex_structs[index].arg.op   = ADD_OP;

            buf++;
            buf_pos++;
        }

        else if (*buf == '-' && !is_oper_now) 
        {
            is_oper_now = true;
            lex_structs[index].type_arg = TYPE_OP;
            lex_structs[index].arg.op   = SUB_OP;

            buf++;
            buf_pos++;
        }

        else if (*buf == '*' && !is_oper_now) 
        {
            LEX_ERROR(is_oper_now, NULL, buf_pos);
            is_oper_now = true;
            lex_structs[index].type_arg = TYPE_OP;
            lex_structs[index].arg.op   = MUL_OP;

            buf++;
            buf_pos++;
        }

        else if (*buf == '/' && !is_oper_now) 
        {
            LEX_ERROR(is_oper_now, NULL, buf_pos);
            is_oper_now = true;
            lex_structs[index].type_arg = TYPE_OP;
            lex_structs[index].arg.op   = DIV_OP;

            buf++;
            buf_pos++;
        }

        else if (*buf == '^' && !is_oper_now) 
        {
            LEX_ERROR(is_oper_now, NULL, buf_pos);
            is_oper_now = true;
            lex_structs[index].type_arg = TYPE_OP;
            lex_structs[index].arg.op   = POW_OP;

            buf++;
            buf_pos++;
        }

        else
        {
            size_t arg_len = GetLexicArg(&buf, &lex_structs[index]);
            LEX_ERROR(arg_len == 0, NULL, buf_pos);
            buf_pos += arg_len;
            is_oper_now = false;
        }
            
        index++;
    }

    return lex_structs; 
}

size_t GetLexicArg(const char **buf, struct DiffLexicalElem *lex_struct)
{
    printf("enter getarg\n");
    printf("buf = %s\n", *buf);

    size_t arg_len = 0;
    const char *s_old = *buf;

    if ((**buf >= '0' && **buf <= '9') || **buf == 'p' || **buf == 'e')
    {
        printf("enter num\n");
        lex_struct->type_arg = TYPE_NUM;
        diff_num_t num = DIFF_NUM_PSN;

        if (**buf == 'p')
        {
            (*buf)++;
            arg_len++;
            ERROR_CHECK(**buf != 'i', 0);
            num = PI;
            (*buf)++;
            arg_len++;
        }

        else if (**buf == 'e')
        {
            num = EXP;
            (*buf)++;
            arg_len++;
        }

        else
        {
            size_t num_length = ConvertStrToNum(buf, &num);
            ERROR_CHECK(num_length == 0, 0);
            arg_len += num_length;
        }

        printf("num = %lf\n", num);
        lex_struct->arg.num = num;
    }
    
    else if (**buf >= 'a' && **buf <= 'z' && 
             **buf != 's' && **buf != 'c' && **buf != 't' &&
             **buf != 'a' && **buf != 'l' && **buf != 'p' &&
             **buf != 'e')
    {
        printf("enter var\n");
        lex_struct->type_arg = TYPE_VAR;

        lex_struct->arg.var = **buf;

        (*buf)++;
        arg_len++;
    }

    ERROR_CHECK(*buf == s_old, 0);

    return arg_len;
}*/


size_t ConvertStrToNum(const char **string, double *num)
{
    ERROR_CHECK(string == NULL, 0);
    ERROR_CHECK(   num == NULL, 0);

    size_t num_length = 0;
    int sign = 1;
    if(**string == '-' || **string == '+')
    {
        if (**string == '-')
            sign = -1;
        (*string)++;
        num_length++;
    }

    *num = 0;
    while (**string >= '0' && **string <= '9')
    {
        *num = (*num) * 10 + **string - '0';
        (*string)++;
        num_length++;
    }

    if (**string == '.' || **string == ',')
    {
        (*string)++;
        num_length++;
        double degree = 0.1;
        while (**string >= '0' && **string <= '9')
        {
            *num += (**string - '0') * degree;
            (*string)++;
            num_length++;
            degree *= 0.1;
        }
    }
    
    *num *= sign;

    return num_length;
}


struct TreeNode *ReadExpression(const char *str)
{
    ERROR_CHECK(str == NULL, 0);
    printf("Enter get exp\n");
    printf("buf = %s\n", str);

    struct TreeNode *node = GetAdd(&str);
    ERROR_CHECK(*str != '\0', NULL);

    ERROR_CHECK(node == NULL, NULL);
    return node;
}

struct TreeNode *GetAdd(const char **buf)
{
    printf("Enter get add\n");
    printf("buf = %s\n", *buf);

    SKIP_SPACE_STR(*buf);

    struct TreeNode *node = GetMul(buf);

    while (**buf == '+' || **buf == '-')
    {
        int op = **buf;
        (*buf)++;

        struct TreeNode *node2 = GetMul(buf);

        struct TreeNode *parent_node = NULL; 
        if (op == '+')
            parent_node = DiffNodeCtor(TYPE_OP, DiffOpCtor(ADD_OP), node, node2);
        else
            parent_node = DiffNodeCtor(TYPE_OP, DiffOpCtor(SUB_OP), node, node2);

        node = parent_node;
    }

    SKIP_SPACE_STR(*buf);

    ERROR_CHECK(node == NULL, NULL);
    return node;
}

struct TreeNode *GetMul(const char **buf)
{
    printf("Enter get mul\n");
    printf("buf = %s\n", *buf);

    SKIP_SPACE_STR(*buf);

    struct TreeNode *node = GetPow(buf);

    while (**buf == '*' || **buf == '/')
    {
        int op = **buf;
        (*buf)++;

        struct TreeNode *node2 = GetPow(buf);

        struct TreeNode *parent_node = NULL; 
        if (op == '*')
            parent_node = DiffNodeCtor(TYPE_OP, DiffOpCtor(MUL_OP), node, node2);
        else
        {
            ERROR_CHECK(node2->value->type_arg == TYPE_NUM && 
                        node2->value->diff_arg->num - EPS <= 0  &&
                        node2->value->diff_arg->num + EPS >= 0, NULL);
            parent_node = DiffNodeCtor(TYPE_OP, DiffOpCtor(DIV_OP), node, node2);
        }

        node = parent_node;
    }

    SKIP_SPACE_STR(*buf);

    ERROR_CHECK(node == NULL, NULL);
    return node;
}

struct TreeNode *GetPow(const char **buf)
{
    printf("Enter get pow\n");
    printf("buf = %s\n", *buf);

    SKIP_SPACE_STR(*buf);

    struct TreeNode *node = GetUnarFunc(buf);

    while (**buf == '^')
    {
        (*buf)++;
        struct TreeNode *node2 = GetUnarFunc(buf);
        struct TreeNode *parent_node = DiffNodeCtor(TYPE_OP, DiffOpCtor(POW_OP), node, node2); 

        node = parent_node;
    }

    SKIP_SPACE_STR(*buf);

    ERROR_CHECK(node == NULL, NULL);
    return node;
}

struct TreeNode *GetUnarFunc(const char **buf)
{
    printf("Enter get unar\n");
    printf("buf = %s\n", *buf);

    SKIP_SPACE_STR(*buf);

    struct TreeNode *node = NULL;

    if (**buf == 's' || **buf == 'c' || **buf == 't' || **buf == 'a' || **buf == 'l')
    {
        int symb_count = 0;
        char op[MAX_ARG_LEN] = { 0 };
        sscanf((*buf), "%[a-z] %n", op, &symb_count);                     
        (*buf) += symb_count;

        struct TreeNode *node2 = GetBrackets(buf);

        if(strcmp(op, "sin") == 0)
            node = DiffNodeCtor(TYPE_OP, DiffOpCtor(SIN_OP), NULL, node2);

        else if(strcmp(op, "cos") == 0)
            node = DiffNodeCtor(TYPE_OP, DiffOpCtor(COS_OP), NULL, node2);

        else if(strcmp(op, "tg") == 0)
            node = DiffNodeCtor(TYPE_OP, DiffOpCtor(TAN_OP), NULL, node2);

        else if(strcmp(op, "arcsin") == 0)
            node = DiffNodeCtor(TYPE_OP, DiffOpCtor(ASIN_OP), NULL, node2);

        else if(strcmp(op, "arccos") == 0)
            node = DiffNodeCtor(TYPE_OP, DiffOpCtor(ACOS_OP), NULL, node2);

        else if(strcmp(op, "arctg") == 0)
            node = DiffNodeCtor(TYPE_OP, DiffOpCtor(ATAN_OP), NULL, node2);

        else if(strcmp(op, "sh") == 0)
            node = DiffNodeCtor(TYPE_OP, DiffOpCtor(SINH_OP), NULL, node2);

        else if(strcmp(op, "ch") == 0)
            node = DiffNodeCtor(TYPE_OP, DiffOpCtor(COSH_OP), NULL, node2);

        else if(strcmp(op, "th") == 0)
            node = DiffNodeCtor(TYPE_OP, DiffOpCtor(TANH_OP), NULL, node2);

        else if(strcmp(op, "ln") == 0)
        {
            ERROR_CHECK(node2->value->type_arg == TYPE_NUM && 
                        node2->value->diff_arg->num < EPS, NULL);
            node = DiffNodeCtor(TYPE_OP, DiffOpCtor(LN_OP), NULL, node2);
        }
    }

    else
        node = GetBrackets(buf);

    SKIP_SPACE_STR(*buf);

    ERROR_CHECK(node == NULL, NULL);
    return node;
}

struct TreeNode *GetBrackets(const char **buf)
{
    printf("Enter get brackets\n");
    printf("buf = %s\n", *buf);

    SKIP_SPACE_STR(*buf);

    struct TreeNode *node = NULL;

    if (**buf == '(')
    {
        (*buf)++;

        node = GetAdd(buf);

        ERROR_CHECK((**buf) != ')', 0);
        (*buf)++;
    }

    else
        node = GetArg(buf);

    SKIP_SPACE_STR(*buf);

    ERROR_CHECK(node == NULL, NULL);
    return node;
}

struct TreeNode *GetArg(const char **buf)
{
    printf("Enter get arg\n");
    printf("buf = %s\n", *buf);

    SKIP_SPACE_STR(*buf);

    tree_elem_t val = NULL;
    CREATE_TREE_NODE_VALUE(val, NULL);

    struct TreeNode *new_node = TreeNodeCtor(val, NULL, NULL);
    const char *s_old = *buf;
    if ((**buf >= '0' && **buf <= '9') || **buf == 'p' || **buf == 'e')
    {
        new_node->value->type_arg = TYPE_NUM;
        diff_num_t num = DIFF_NUM_PSN;

        if (**buf == 'p')
        {
            (*buf)++;
            ERROR_CHECK(**buf != 'i', NULL);
            num = PI;
            (*buf)++;
        }

        else if (**buf == 'e')
        {
            num = EXP;
            (*buf)++;
        }

        else
        {
            size_t num_length = ConvertStrToNum(buf, &num);
            ERROR_CHECK(num_length == 0, TREE_VAL_PSN);
        }

        new_node->value->diff_arg->num = num;
    }
    
    else if (**buf >= 'a' && **buf <= 'z' && 
             **buf != 's' && **buf != 'c' && **buf != 't' &&
             **buf != 'a' && **buf != 'l' && **buf != 'p' &&
             **buf != 'e')
    {
        new_node->value->type_arg = TYPE_VAR;

        new_node->value->diff_arg->var = **buf;

        (*buf)++;
    }

    ERROR_CHECK(*buf == s_old, TREE_VAL_PSN);

    SKIP_SPACE_STR(*buf);

    ERROR_CHECK(new_node == NULL, NULL);
    return new_node;
}