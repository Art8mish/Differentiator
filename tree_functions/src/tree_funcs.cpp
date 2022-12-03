
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
    
        default: 
            {
                return ERROR_WRONG_TREE_INSERT_PATH;
                break;
            }
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
   
    new_tree->root = ReadExpression(buf);
    ERROR_CHECK(new_tree->root == NULL, NULL);

    return new_tree;    
}
