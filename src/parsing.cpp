
#include "../include/diff.h"

struct TreeNode *ReadExpression(const char *str)
{
    ERROR_CHECK(str == NULL, 0);
    
    struct TreeNode *node = GetAdd(&str);
    ERROR_CHECK(*str != '\0', NULL);

    ERROR_CHECK(node == NULL, NULL);
    return node;
}

struct TreeNode *GetAdd(const char **buf)
{
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