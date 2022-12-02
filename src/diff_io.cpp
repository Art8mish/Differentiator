 
#include "../include/diff.h"

int GetStartValues(double *point, int *diff_dgr, int *taylor_dgr)
{
    ERROR_CHECK(point      == NULL, ERROR_NULL_PTR);
    ERROR_CHECK(diff_dgr   == NULL, ERROR_NULL_PTR);
    ERROR_CHECK(taylor_dgr == NULL, ERROR_NULL_PTR);

    printf("Enter x value: ");
                                                                   
    bool correct_input = false;                                          
    while (!correct_input)                                               
    {                                                                    
        int scanf_ret = scanf(" %lf", point); 
        if (scanf_ret)                                   
            correct_input = true;                                        
        int ch = 0;                                                      
        while ((ch = getchar()) != '\n')                                 
            if (!isspace(ch))                                            
                correct_input = false;                                   
        if (!correct_input)                                              
            printf("I do not understand.\nEnter correct value..\n");   
    }    

    printf("Enter differentiation degree: ");   
    correct_input = false;
    while (!correct_input)                                               
    {                                                                    
        int scanf_ret = scanf(" %d", diff_dgr); 
        if (scanf_ret && *diff_dgr >= 0)                                  
            correct_input = true;                                        
        int ch = 0;                                                      
        while ((ch = getchar()) != '\n')                                 
            if (!isspace(ch))                                            
                correct_input = false;                                   
        if (!correct_input)                                              
            printf("I do not understand.\nEnter correct value..\n");   
    } 

    printf("Enter maximum Taylor degree: ");
    correct_input = false;   
    while (!correct_input)                                               
    {                                                                    
        int scanf_ret = scanf(" %d", taylor_dgr); 
        if (scanf_ret && *taylor_dgr >= 0)                                
            correct_input = true;                                        
        int ch = 0;                                                      
        while ((ch = getchar()) != '\n')                                 
            if (!isspace(ch))                                            
                correct_input = false;                                   
        if (!correct_input)                                              
            printf("I do not understand.\nEnter correct value..\n");   
    } 

    return SUCCESS;
}

int CreateMatanManual(struct Tree *expr, char var, diff_num_t point, 
                      unsigned int diff_dgr, unsigned int taylor_dgr)
{
    ERROR_CHECK(expr == NULL, ERROR_NULL_PTR);

    FILE *tex_f = fopen(TEX_FILE_PATH, "wb");
    ERROR_CHECK(tex_f == NULL, ERROR_OPENING_FILE);
    
    struct stat file_stat;
    int stat_err = stat(FOREWORD_FILE_PATH, &file_stat); 
    FILE_ERROR_CHECK(stat_err == -1, ERROR_STAT, tex_f);

    char *foreword = (char *) calloc(file_stat.st_size, sizeof(char));
    FILE_ERROR_CHECK(foreword == NULL, ERROR_CALLOC, tex_f);

    FILE *foreword_f = fopen(FOREWORD_FILE_PATH, "rb");
    FILE_ERROR_CHECK(foreword_f == NULL, ERROR_OPENING_FILE, tex_f);

    fread (foreword, sizeof(char), file_stat.st_size - 1, foreword_f);
    fwrite(foreword, sizeof(char), file_stat.st_size - 1, tex_f);

    free(foreword);
    fclose(foreword_f);

    fprintf(tex_f, "\n\\newpage \n \\section{Анализ функции} \n"
                   "Теперь можно перейти к анализу заданной функции: \n"
                   "\\begin{equation}\n f(%c) = ", var);
    int serialize_node_err = TexSerializeNode(tex_f, expr->root);
    ERROR_CHECK(serialize_node_err, ERROR_SERIALIZE_NODE);    
    fprintf(tex_f, "\\end{equation}\n");   

//значение f в точке
    TREEDUMP(expr, "first");
    
    printf("znach f v tochke...........................................\n");
    diff_num_t val_at_point = DIFF_NUM_PSN; 
    int count_val_at_point_err = CountValueAtPoint(expr->root, var, point, &val_at_point);
    ERROR_CHECK(count_val_at_point_err, ERROR_COUNT_VAL_AT_POINT);
    fprintf(tex_f, "Значение функции в точке $%c_0 = %g$:  $f(%c_0) = %g$ \\\\ \n", 
                    var, point, var, val_at_point);
    printf("val_at_point = %lf\n", val_at_point);

//касательная к f в точке
    fprintf(tex_f, "\\subsection{Касательная к графику функции в точке} \n ");

    TREEDUMP(expr, "scnd");
    printf("kas in tochka...........................................\n");
    diff_num_t val_at_null = DIFF_NUM_PSN;
    count_val_at_point_err = CountValueAtPoint(expr->root, var, 0, &val_at_null);
    ERROR_CHECK(count_val_at_point_err, ERROR_COUNT_VAL_AT_POINT);

    struct TreeNode *diff_expr_node = DifferentiateNode(expr->root, var);
    ERROR_CHECK(diff_expr_node == NULL, ERROR_TREE_NODE_COPY);

    diff_num_t diff_val_at_point = DIFF_NUM_PSN;
    count_val_at_point_err = CountValueAtPoint(diff_expr_node, var, point, &diff_val_at_point);
    ERROR_CHECK(count_val_at_point_err, ERROR_COUNT_VAL_AT_POINT);

    fprintf(tex_f, "Уравнение касательной к графику функции в точке $%c_0 = %g$:"
                   "\\[ y = %g \\cdot x + (%g)\\]\n", 
                    var, point, diff_val_at_point, val_at_null);

//Производная n-ой степени
    fprintf(tex_f, "\\subsection{Производная %d-ой степени} \n ", diff_dgr);
    fprintf(tex_f, "Из предисловия нетрудно заметить, что \n");
    printf("proizvodnaya.................................\n");
    struct TreeNode *diff_node = expr->root;
    ERROR_CHECK(diff_node == NULL, ERROR_DIFFERENTIATE_NODE);

    for (unsigned int i = 0; i < diff_dgr; i++)
    {
        struct TreeNode *extra_diff_node = DifferentiateNode(diff_node, var);
        ERROR_CHECK(extra_diff_node == NULL, ERROR_DIFFERENTIATE_NODE);

        if (i != 0)
        {
            int node_dtor_err = TreeNodeDtor(diff_node);
            ERROR_CHECK(node_dtor_err, ERROR_TREE_NODE_DTOR);
        }

        diff_node = SimplifyExpression(extra_diff_node);
        ERROR_CHECK(diff_node == NULL, ERROR_SIMPLIFY_EXPRESSION);

        if (i < MAX_PRINTED_DIFF_DGR)
        {
            fprintf(tex_f, "\\begin{eqnarray}\n f^{(%d)}(%c) = ", i+1, var);
            serialize_node_err = TexSerializeNode(tex_f, diff_node);
            ERROR_CHECK(serialize_node_err, ERROR_SERIALIZE_NODE); 
            fprintf(tex_f, "\\end{eqnarray}\\\\\n");
        }
    }

    if (diff_dgr > MAX_PRINTED_DIFF_DGR)
        fprintf(tex_f, "\\\\...Следующую часть простейших преобразований оставляем читателю...\\\\");

    fprintf(tex_f, "\\\\После небольшого количества элементарных преобразований получаем:\\\\ \n");

    fprintf(tex_f, "\\begin{eqnarray} \nf^{(%d)} = ", diff_dgr);

    serialize_node_err = TexSerializeNode(tex_f, diff_node);
    ERROR_CHECK(serialize_node_err, ERROR_SERIALIZE_NODE); 
    fprintf(tex_f, "\\end{eqnarray} \n");

//значение n-ой производной f в точке
    diff_val_at_point = DIFF_NUM_PSN;
    count_val_at_point_err = CountValueAtPoint(diff_node, var, point, &diff_val_at_point);
    ERROR_CHECK(count_val_at_point_err, ERROR_COUNT_VAL_AT_POINT);

    fprintf(tex_f, "\\\\В точке $%c_0 = %g$:  $f^{(%d)}(%c_0) = %g$\n", 
                    var, point, diff_dgr, var, diff_val_at_point);

//разложение в Ряд Тейлора
    fprintf(tex_f, "\\subsection{Разложение по формуле Тейлора} \n ");
    fprintf(tex_f, "Разложим функцию по формуле Тейлора в точке $%g$ до $x^{%d}$: \n",
                    point, taylor_dgr);
    int taylor_err = ExpandInTaylor(tex_f, expr->root, var, point, taylor_dgr);
    ERROR_CHECK(taylor_err, ERROR_EXPAND_TAYLOR);

//Вставить график
    fprintf(tex_f, "\\subsection{График функции} \n ");
    int make_graph_err = MakeGraphicFile(expr->root);
    ERROR_CHECK(make_graph_err, ERROR_MAKE_GRAPHIC_FILE);

    fprintf(tex_f, "\\includegraphics{%s}\\\\\n", GRAPHIC_FILE_PATH);


//Подсчет погрешности
    int add_tex_lab_err = AddTexLabError(tex_f);
    ERROR_CHECK(add_tex_lab_err, ERROR_ADD_TEX_LAB);

    fprintf(tex_f, "\\end{document}\n");
    fclose(tex_f);

    return SUCCESS;
}


int CountValueAtPoint(struct TreeNode *expr_node, char var, diff_num_t point,
                      diff_num_t *value)
{
    ERROR_CHECK(expr_node == NULL, ERROR_NULL_PTR);
    ERROR_CHECK(value     == NULL, ERROR_NULL_PTR);
    printf("entered count value\n");

    struct TreeNode *copy_expr_node = TreeNodeCopy(expr_node);
    ERROR_CHECK(copy_expr_node == NULL, ERROR_TREE_NODE_COPY);

    int frame_var_err = FrameVar(copy_expr_node, var, point);
    ERROR_CHECK(frame_var_err, ERROR_FRAME_VAR);
    printf("continue count value1..\n");

    copy_expr_node = SimplifyExpression(copy_expr_node);
    ERROR_CHECK(copy_expr_node == NULL, ERROR_SIMPLIFY_EXPRESSION);
    printf("continue count value2..\n");
    ERROR_CHECK(copy_expr_node->value->type_arg != TYPE_NUM, ERROR_FRAME_VAR);
    *value = copy_expr_node->value->diff_arg->num;

    printf("dtoring num_node:\n");
    int node_dtor_err = TreeNodeDtor(copy_expr_node);
    ERROR_CHECK(node_dtor_err, ERROR_TREE_NODE_DTOR);
    printf("finish count value\n");

    return SUCCESS;
} 

int TexSerializeNode(FILE *tex_f, const struct TreeNode *curr_node)
{
    ERROR_CHECK(curr_node == NULL, ERROR_NULL_PTR);
    ERROR_CHECK(tex_f    == NULL, ERROR_NULL_PTR);

    bool print_op = false;
    
    switch (curr_node->value->type_arg)
    {
        case TYPE_PSN : fprintf(tex_f, " TYPE_PSN ");
                        break;
        case TYPE_NUM : if (COMPARE_DOUBLE(curr_node->value->diff_arg->num, EXP))
                            fprintf(tex_f, "{e}");
                        else if (COMPARE_DOUBLE(curr_node->value->diff_arg->num, PI))
                            fprintf(tex_f, "{\\pi}");
                        else
                        {
                            if (curr_node->value->diff_arg->num < 0)
                                fprintf(tex_f, "{(%g)}", curr_node->value->diff_arg->num);
                            else
                                fprintf(tex_f, "{%g}", curr_node->value->diff_arg->num);
                        }
                        break;
        case TYPE_OP  :  
                switch (curr_node->value->diff_arg->op)
                {
                    case PSN_OP :   break;
                    case ADD_OP :   print_op = true;
                                    break;
                    case SUB_OP  :  print_op = true;
                                    break;
                    case MUL_OP  :  print_op = true;
                                    break;
                    case DIV_OP  :  fprintf(tex_f, " \\frac ");
                                    break;
                    case POW_OP  :  print_op = true;
                                    break;
                    case SIN_OP  :  fprintf(tex_f, "\\sin ");
                                    break;
                    case COS_OP  :  fprintf(tex_f, "\\cos ");
                                    break;
                    case TAN_OP  :  fprintf(tex_f, "\\tg ");
                                    break;
                    case ASIN_OP :  fprintf(tex_f, "\\arcsin ");
                                    break;
                    case ACOS_OP :  fprintf(tex_f, "\\arccos ");
                                    break;
                    case ATAN_OP :  fprintf(tex_f, "\\arctg ");
                                    break;
                    case SINH_OP :  fprintf(tex_f, "\\sh ");
                                    break;
                    case COSH_OP :  fprintf(tex_f, "\\ch ");
                                    break;
                    case TANH_OP :  fprintf(tex_f, "\\th ");
                                    break;
                    case LN_OP   :  fprintf(tex_f, "\\ln ");
                                    break;
                    default:        fprintf(tex_f, "ERROR_OP");
                                    break;
                }
                break;

        case TYPE_VAR : fprintf(tex_f, "%c", curr_node->value->diff_arg->var);
                        break;
            
        default :   //fprintf(tex_f, " TYPE_ERROR ");
                    break;
    }

    if (curr_node->left != NULL)
    {
        fprintf(tex_f, "{");
        if (curr_node->left->right != NULL)
            fprintf(tex_f, "(");

        int tex_serilize_node_err = TexSerializeNode(tex_f, curr_node->left);
        ERROR_CHECK(tex_serilize_node_err, ERROR_TEX_SERIALIZE_NODE);

        if (curr_node->left->right != NULL)
            fprintf(tex_f, ")");
        fprintf(tex_f, "}");
    }

    if (print_op)
    {
        switch (curr_node->value->diff_arg->op)
        {
            case ADD_OP :   fprintf(tex_f, " + ");
                            break;
            case SUB_OP  :  fprintf(tex_f, " - ");
                            break;
            case MUL_OP  :  fprintf(tex_f, " \\cdot ");
                            break;
            case POW_OP  :  fprintf(tex_f, " ^ ");
                            break;
            default      :  fprintf(tex_f, " error ");
                            break;
        }
    }

    
    if (curr_node->right != NULL)
    {
        fprintf(tex_f, "{");
        if (curr_node->right->right != NULL)
            fprintf(tex_f, "(");

        int tex_serilize_node_err = TexSerializeNode(tex_f, curr_node->right);
        ERROR_CHECK(tex_serilize_node_err, ERROR_TEX_SERIALIZE_NODE);

        if (curr_node->right->right != NULL)
            fprintf(tex_f, ")");
        fprintf(tex_f, "}");
    }


    return SUCCESS;
}

int ExpandInTaylor(FILE *tex_f, struct TreeNode *curr_node, 
                   char var, diff_num_t point, unsigned int degree)
{
    ERROR_CHECK(tex_f    == NULL, ERROR_NULL_PTR);
    ERROR_CHECK(curr_node == NULL, ERROR_NULL_PTR);

    fprintf(tex_f, "\\begin{eqnarray}\nf(%c) = ", var);

    int serialize_node_err = TexSerializeNode(tex_f, curr_node);
    ERROR_CHECK(serialize_node_err, ERROR_SERIALIZE_NODE);
    fprintf(tex_f, " = ");

    diff_num_t val_at_point = DIFF_NUM_PSN;
    int count_val_at_point_err = CountValueAtPoint(curr_node, var, point, &val_at_point);
    ERROR_CHECK(count_val_at_point_err, ERROR_COUNT_VAL_AT_POINT);

    fprintf(tex_f, "%g", val_at_point);

    struct TreeNode *diff_node = DifferentiateNode(curr_node, var);
    ERROR_CHECK(diff_node == NULL, ERROR_DIFFERENTIATE_NODE);

    int factorial = 1;
    for (unsigned int diff_degree = 0; diff_degree < degree; diff_degree++)
    {
        factorial *= (diff_degree + 1);
        
        struct TreeNode *extra_diff_node = diff_node;

        diff_num_t diff_val_at_point = DIFF_NUM_PSN;
        count_val_at_point_err = CountValueAtPoint(diff_node, var, point, &diff_val_at_point);
        ERROR_CHECK(count_val_at_point_err, ERROR_COUNT_VAL_AT_POINT);

        if (!COMPARE_DOUBLE(diff_val_at_point, 0))
        {
            if (diff_val_at_point < 0)
            {
                diff_val_at_point *= -1;
                fprintf(tex_f, " - ");
            }
            else
                fprintf(tex_f, " + ");
            fprintf(tex_f, "\\frac{%g}{%d}", diff_val_at_point, factorial);
            fprintf(tex_f, " \\cdot (x");
            if (!COMPARE_DOUBLE(point, 0))
                fprintf(tex_f, " - (%g)", point);
            fprintf(tex_f, ")^{%d}", diff_degree + 1);
        }

        diff_node = DifferentiateNode(extra_diff_node, var);
        ERROR_CHECK(diff_node == NULL, ERROR_DIFFERENTIATE_NODE);

        int node_dtor_err = TreeNodeDtor(extra_diff_node);
        ERROR_CHECK(node_dtor_err, ERROR_TREE_NODE_DTOR);

        if (diff_degree % MAX_TAYLOR_MEMBER_IN_LINE == 0)
            fprintf(tex_f, "\\nonumber\\\\ \n");
    }

    fprintf(tex_f, " + o((x");
    if (!COMPARE_DOUBLE(point, 0))
        fprintf(tex_f, " - (%g)", point);
    fprintf(tex_f, ")^{%d})", degree);

    
    fprintf(tex_f, "\\end{eqnarray}\n");

    return SUCCESS;
}


struct Tree *LabDeserialize(const char *input_file_name, char *main_var,
                            int *var_amount, struct DiffVar **vars)
{
    ERROR_CHECK(input_file_name == NULL, NULL);

    struct Tree *new_tree = TreeCtor();
    ERROR_CHECK(new_tree == NULL, NULL);

    struct WorkingField *onegin_context = CreateWorkingField(input_file_name);
    ERROR_CHECK(onegin_context == NULL, NULL);

    char *buf = onegin_context->chars_buffer;
    
    int n = 0;
    sscanf(buf, " %c %*c%n", main_var, &n);
    buf += n;

    printf("buf: %s\n", buf);
    new_tree->root = ReadExpression(buf);
    ERROR_CHECK(new_tree->root == NULL, NULL);
    
    while (*buf != '\0')
        buf++;
    buf++;
    
    sscanf(buf, " %*s = %d%n", var_amount, &n);
    buf += n;
    printf("var_amount = %d\n", *var_amount);
    SKIP_SPACE(buf, NULL);

    *vars = (struct DiffVar *) calloc(*var_amount, sizeof(struct DiffVar));
    ERROR_CHECK(*vars == NULL, NULL);

    for (int i = 0; i < *var_amount; i++)
    {
        char var   = 0;
        diff_num_t var_val = 0;
        diff_num_t var_err_val = 0; 
        printf("buf: %s\n", buf);
        sscanf(buf, " %c = %lf, %*s = %lf%n", &var, &var_val, &var_err_val, &n);
        buf += n;

        printf("var = %c, var_val = %g, var_err_val = %g\n", var, var_val, var_err_val);

        (*vars)[i].var     = var;
        (*vars)[i].point   = var_val;
        (*vars)[i].err_val = var_err_val;

        SKIP_SPACE(buf, new_tree);
    }

    return new_tree;    
}


int AddTexLabError(FILE *tex_f)
{
    char main_var = '\0';
    int var_amount = 0;
    struct DiffVar *vars = NULL;
    struct Tree *expr = LabDeserialize(LAB_DESERIALIZATION_PATH, &main_var,
                                       &var_amount, &vars);
    ERROR_CHECK(expr == NULL, ERROR_LAB_DESERIALIZE);

    diff_num_t result = 0;

    fprintf(tex_f, "\n\\newpage \n \\section{Подсчет погрешности} \n ");
    fprintf(tex_f, "Подсчитаем погрешность величины %c: \\\\\n", main_var);

    fprintf(tex_f, "\\[ %c = ", main_var);
    int serialize_node_err = TexSerializeNode(tex_f, expr->root);
    ERROR_CHECK(serialize_node_err, ERROR_SERIALIZE_NODE);    
    fprintf(tex_f, "\\]\n");
    
    fprintf(tex_f, "Для значений величин: \\\\");
    for (int i = 0; i < var_amount; i++)
        fprintf(tex_f, "$%c = %g$, $\\delta_{%c} = %g$\\\\", 
                        vars[i].var, vars[i].point, vars[i].var, vars[i].err_val);
    
    fprintf(tex_f, "\\[\\delta_{%c} = \\sqrt{", main_var);

    for (int i = 0; i < var_amount; i++)
    {
        if (i != 0)
            fprintf(tex_f, " + ");

        struct TreeNode *diff_expr_node = DifferentiateNode(expr->root, vars[i].var);
        ERROR_CHECK(diff_expr_node == NULL, ERROR_TREE_NODE_COPY);

        diff_num_t val_at_point = DIFF_NUM_PSN;
        int count_val_at_point_err = CountValueForManyVars(diff_expr_node, var_amount,
                                                           vars, &val_at_point);
        ERROR_CHECK(count_val_at_point_err, ERROR_COUNT_VAL_AT_POINT);

        fprintf(tex_f, "((%g) \\cdot %g)^2", val_at_point, vars[i].err_val);

        int node_dtor_err = TreeNodeDtor(diff_expr_node);
        ERROR_CHECK(node_dtor_err, ERROR_TREE_NODE_DTOR);

        result += pow(val_at_point * vars[i].err_val, 2);
    }

    result = sqrt(result);
    fprintf(tex_f, "} = %g \\]\n", result);

    free(vars);
    int tree_dtor_err = TreeDtor(expr);
    ERROR_CHECK(tree_dtor_err, ERROR_TREE_DTOR);

    return SUCCESS;
}

int CountValueForManyVars(struct TreeNode *expr_node, int var_amount,
                          struct DiffVar  *vars, diff_num_t *value)
{
    ERROR_CHECK(expr_node == NULL, ERROR_NULL_PTR);
    ERROR_CHECK(value     == NULL, ERROR_NULL_PTR);

    struct TreeNode *copy_expr_node = TreeNodeCopy(expr_node);
    ERROR_CHECK(copy_expr_node == NULL, ERROR_TREE_NODE_COPY);

    for (int i = 0; i < var_amount; i++)
    {
        int frame_var_err = FrameVar(copy_expr_node, vars[i].var, vars[i].point);
        ERROR_CHECK(frame_var_err, ERROR_FRAME_VAR);
    }
    
    copy_expr_node = SimplifyExpression(copy_expr_node);
    ERROR_CHECK(copy_expr_node == NULL, ERROR_SIMPLIFY_EXPRESSION);

    ERROR_CHECK(copy_expr_node->value->type_arg != TYPE_NUM, ERROR_FRAME_VAR);
    *value = copy_expr_node->value->diff_arg->num;

    int node_dtor_err = TreeNodeDtor(copy_expr_node);
    ERROR_CHECK(node_dtor_err, ERROR_TREE_NODE_DTOR);

    return SUCCESS;
}

int MakeGraphicFile(struct TreeNode *curr_node)
{
    ERROR_CHECK(curr_node == NULL, ERROR_NULL_PTR);

    FILE *gnu_f = fopen(GRAPHIC_GNU_FILE_PATH, "w");    
    ERROR_CHECK(gnu_f == NULL, ERROR_OPENING_FILE);

    fprintf(gnu_f, "reset\n");
    fprintf(gnu_f, "set terminal png size 400,300 enhanced font \"Arial,10\"\n");
    fprintf(gnu_f, "set output '%s'\n", GRAPHIC_FILE_PATH);
    fprintf(gnu_f, "plot ");
    int save_node_err = SerializeNode(gnu_f, curr_node);
    FILE_ERROR_CHECK(save_node_err, ERROR_SAVE_NODE, gnu_f);

    fclose(gnu_f);

    char command[MAX_COMMAND_LEN] = { 0 };
    snprintf(command, MAX_COMMAND_LEN, "gnuplot %s", GRAPHIC_GNU_FILE_PATH);
    system(command);

    return SUCCESS;
}
   

