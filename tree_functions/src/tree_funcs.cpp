
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

#define SKIP_SPACE(buf)                                     \
            while(isspace(*(buf)) || *(buf) == '\0')        \
            {                                               \
                (buf)++;                                    \
                if (*(buf) == EOF)                          \
                    return SUCCESS;                         \
            }

#define SKIP_SPACE_STR(buf)                                 \
            while(isspace(*(buf)))                          \
            {                                               \
                (buf)++;                                    \
                if (*(buf) == EOF)                          \
                    return NULL;                            \
            }
                        

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
    
        default:              return ERROR_WRONG_TREE_INSERT_PATH;
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
        if (curr_node->left->right != NULL && 
            curr_node->left->left  != NULL)
            fprintf(tree_f, "(");

        int save_node_err = SerializeNode(tree_f, curr_node->left);
        ERROR_CHECK(save_node_err, ERROR_SAVE_NODE);

        if (curr_node->left->right != NULL && 
            curr_node->left->left  != NULL)
            fprintf(tree_f, ")");
    }

    switch (curr_node->value->type_arg)
    {
        case TYPE_PSN : fprintf(tree_f, " TYPE_PSN ");
                        break;
        case TYPE_NUM : if (COMPARE_DOUBLE(curr_node->value->diff_arg->num, EXP))
                            fprintf(tree_f, "e");
                        else if (COMPARE_DOUBLE(curr_node->value->diff_arg->num, PI))
                            fprintf(tree_f, "pi");
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
                    case POW_OP  :  fprintf(tree_f, " ^ ");
                                    break;
                    case SIN_OP  :  fprintf(tree_f, "sin ");
                                    break;
                    case COS_OP  :  fprintf(tree_f, "cos ");
                                    break;
                    case TAN_OP  :  fprintf(tree_f, "tg ");
                                    break;
                    case ASIN_OP :  fprintf(tree_f, "arcsin ");
                                    break;
                    case ACOS_OP :  fprintf(tree_f, "arccos ");
                                    break;
                    case ATAN_OP :  fprintf(tree_f, "arctg ");
                                    break;
                    case SINH_OP :  fprintf(tree_f, "sh ");
                                    break;
                    case COSH_OP :  fprintf(tree_f, "ch ");
                                    break;
                    case TANH_OP :  fprintf(tree_f, "th ");
                                    break;
                    case LN_OP   :  fprintf(tree_f, "ln ");
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
        if (curr_node->right->right != NULL && 
            curr_node->right->left  != NULL)
            fprintf(tree_f, "(");

        int save_node_err = SerializeNode(tree_f, curr_node->right);
        ERROR_CHECK(save_node_err, ERROR_SAVE_NODE);

        if (curr_node->right->right != NULL && 
            curr_node->right->left  != NULL)
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
   
    printf("buf = %s\n", buf);
    new_tree->root = ReadExpression(buf);
    ERROR_CHECK(new_tree->root == NULL, NULL);

    return new_tree;    
}
/*
int DeserializeNode(struct Tree *new_tree, struct TreeNode **parent_node, 
                    char **buf, int recur_level)
{
    ERROR_CHECK( new_tree    == NULL, ERROR_NULL_PTR);
    ERROR_CHECK( parent_node == NULL, ERROR_NULL_PTR);
    ERROR_CHECK( buf == NULL, ERROR_NULL_PTR);
    ERROR_CHECK(*buf == NULL, ERROR_NULL_PTR);
    
    tree_elem_t *pt_val = NULL;

    SKIP_SPACE(*buf);
    if ((**buf) == ')')
    {
        (*buf)++;
        return SUCCESS;
    }

    struct Diff_elem_t *main_value = DiffPsnArgCtor();
    ERROR_CHECK(main_value, ERROR_DIFF_PSN_ARG_CTOR);

    struct TreeNode *main_node = DiffNodeCtor(TYPE_OP, main_value, NULL, NULL);
    ERROR_CHECK(main_node == NULL, ERROR_DIFF_NODE_CTOR);

    if ((**buf) == '(')
    {
        (*buf)++;
        int read_node_err = DeserializeNode(new_tree, &main_node->left, buf, ++recur_level);
        ERROR_CHECK(read_node_err, ERROR_READ_NODE);
    }

    //main part
    //read left arg
    int read_val_err = ReadValue(pt_val, buf);
    ERROR_CHECK(read_val_err, ERROR_READ_VALUE);

    if ((*pt_val)->type_arg == TYPE_OP)
        return ERROR_SYNTAX;//should be changed when doing solo argument funcs

    struct TreeNode *left_node = TreeNodeCtor((*pt_val), NULL, NULL);
    ERROR_CHECK(left_node == NULL, ERROR_TREE_NODE_CTOR);

    main_node->left = left_node;

    //insert check right argument for solo arg funcs (sin cos)

    //read op
    SKIP_SPACE(*buf)
        read_val_err = ReadValue(pt_val, buf);
    ERROR_CHECK(read_val_err, ERROR_READ_VALUE);

    if ((*pt_val)->type_arg != TYPE_OP)
        return ERROR_SYNTAX;

    main_node->value = *pt_val;

    //read right arg
    SKIP_SPACE(*buf)
    if ((**buf) == '(')
    {
        (*buf)++;
        int read_node_err = DeserializeNode(new_tree, &main_node->right, buf, ++recur_level);
        ERROR_CHECK(read_node_err, ERROR_READ_NODE);
    }

    else if ((**buf) == EOF)
        return ERROR_SYNTAX;

    read_val_err = ReadValue(pt_val, buf);
    ERROR_CHECK(read_val_err, ERROR_READ_VALUE);

    if ((*pt_val)->type_arg == TYPE_OP)
        return ERROR_SYNTAX;//should be changed when doing solo argument funcs

    struct TreeNode *right_node = TreeNodeCtor((*pt_val), NULL, NULL);
    ERROR_CHECK(right_node == NULL, ERROR_TREE_NODE_CTOR);

    main_node->right = right_node;

    return SUCCESS;
}*/




int ConvertStrToNum(const char **string, double *num)
{
    ERROR_CHECK(string == NULL, ERROR_NULL_PTR);
    ERROR_CHECK(   num == NULL, ERROR_NULL_PTR);

    int sign = 1;
    if(**string == '-' || **string == '+')
    {
        if (**string == '-')
            sign = -1;
        (*string)++;
    }

    *num = 0;
    while (**string >= '0' && **string <= '9')
    {
        *num = (*num) * 10 + **string - '0';
        (*string)++;
    }

    if (**string == '.' || **string == ',')
    {
        (*string)++;
        double degree = 0.1;
        while (**string >= '0' && **string <= '9')
        {
            *num += (**string - '0') * degree;
            (*string)++;
            degree *= 0.1;
        }
    }
    
    *num *= sign;

    return SUCCESS;
}




struct TreeNode *ReadExpression(const char *str)
{
    ERROR_CHECK(str == NULL, 0);
    printf("enter readexp\n");
    printf("buf = %s\n", str);

    struct TreeNode *node = GetAdd(&str);
    ERROR_CHECK(*str != '\0', NULL);


    printf("finish reading expression\n");
    ERROR_CHECK(node == NULL, NULL);
    return node;
}

struct TreeNode *GetAdd(const char **buf)
{
    SKIP_SPACE_STR(*buf);
    printf("enter add\n");
    printf("buf = %s\n", *buf);

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
    printf("enter mul\n");
    printf("buf = %s\n", *buf);

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
    printf("enter dgr\n");
    printf("buf = %s\n", *buf);

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
    printf("enter unar\n");
    printf("buf = %s\n", *buf);

    struct TreeNode *node = NULL;

    if (**buf == 's' || **buf == 'c' || **buf == 't' || **buf == 'a' || **buf == 'l')
    {
        printf("enter unar op\n");
        int symb_count = 0;
        char op[MAX_ARG_LEN] = { 0 };
        sscanf((*buf), "%[a-z] %n", op, &symb_count);
        printf("op = %s\n", op);                       
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
    printf("enter brackets\n");
    printf("buf = %s\n", *buf);

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
    printf("enter getarg\n");
    printf("buf = %s\n", *buf);

    tree_elem_t val = NULL;
    CREATE_TREE_NODE_VALUE(val, NULL);

    struct TreeNode *new_node = TreeNodeCtor(val, NULL, NULL);
    const char *s_old = *buf;
    if ((**buf >= '0' && **buf <= '9') || **buf == 'p' || **buf == 'e')
    {
        printf("enter num\n");
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
            int cnvrt_str_to_num_err = ConvertStrToNum(buf, &num);
            ERROR_CHECK(cnvrt_str_to_num_err, TREE_VAL_PSN);
        }

        printf("num = %lf\n", num);
        new_node->value->diff_arg->num = num;
    }
    
    else if (**buf >= 'a' && **buf <= 'z' && 
             **buf != 's' && **buf != 'c' && **buf != 't' &&
             **buf != 'a' && **buf != 'l' && **buf != 'p' &&
             **buf != 'e')
    {
        printf("enter var\n");
        new_node->value->type_arg = TYPE_VAR;

        new_node->value->diff_arg->var = **buf;

        (*buf)++;
    }

    ERROR_CHECK(*buf == s_old, TREE_VAL_PSN);

    SKIP_SPACE_STR(*buf);
    printf("buf = %s\n", *buf);

    ERROR_CHECK(new_node == NULL, NULL);
    return new_node;
}
/*
int ReadValue(char **buf, tree_elem_t *pt_val)
{
    ERROR_CHECK( buf  == NULL, ERROR_NULL_PTR);
    ERROR_CHECK(*buf  == NULL, ERROR_NULL_PTR);

    char *value = (char *) calloc(MAX_ARG_LEN, sizeof(char));
    int symb_count = 0;

    CREATE_TREE_NODE_VALUE(pt_val, ERROR_CALLOC);

    SKIP_SPACE((*buf));
    //(*buf)++;

    //while(**buf >= '0' || **buf <= '9')
    sscanf((*buf), READABLE_SYMB "%n", value,  &symb_count);
    (*buf) += symb_count;

    //num
    if(isdigit(value[0]))
    {
        (*pt_val)->type_arg = TYPE_NUM;
        int convert_num_err = ConvertStrToNum((const char **)&value, &(*pt_val)->diff_arg->num);
        ERROR_CHECK(convert_num_err, ERROR_CONVERT_STR_TO_NUM);
    }

    //var
    if(isalpha(value[0]) && value[1] == '\0')
    {
        (*pt_val)->type_arg = TYPE_VAR;
        if (value[0] == MAIN_VARIABLE)
            (*pt_val)->diff_arg->var = X_VAR;

        else
            (*pt_val)->diff_arg->var = CONST_VAR;
    }

    //operation
    else
    {
        int diff_op = PSN_OP;
        (*pt_val)->type_arg = TYPE_OP;
        if (value[0] == '+')
            diff_op = ADD_OP;
        if (value[0] == '-')
            diff_op = SUB_OP;
        if (value[0] == '*')
            diff_op = MUL_OP;
        if (value[0] == '/')
            diff_op = DIV_OP;
        if (value[0] == '^')
            diff_op = EXP_OP;
        if (strncmp(value, "sin", 3) == 0)
            diff_op = SIN_OP;
        if (strncmp(value, "cos", 3) == 0)
            diff_op = COS_OP;

    }

    if(value[0] == '-' || value[0] == '+')

    free(value);
    return SUCCESS;
}*/