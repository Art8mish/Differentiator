 
#include "../include/diff.h"

int CreateMatanManual(struct Tree *expr, char var, diff_num_t point, 
                      unsigned int diff_dgr, unsigned int taylor_dgr)
{
    ERROR_CHECK(expr == NULL, ERROR_NULL_PTR);

    FILE *tex_f = fopen(TEX_FILE_PATH, "w");
    ERROR_CHECK(tex_f == NULL, ERROR_OPENING_FILE);
    /*
    fprintf(tex_f, "\\documentclass[a4paper]{article}\n"
                   "\\usepackage{amsmath}\n"
                   "\\usepackage[unicode, pdftex]{hyperref} % гиперссылки\n"
                   "\\usepackage[usenames]{color} %цвета\n"
                   "\\usepackage{mathtext}\n"
                   "\\usepackage[T2A]{fontenc}\n"
                   "\\usepackage[utf8]{inputenc}\n"
                   "\\usepackage[english, russian]{babel}\n"
                   "\\usepackage{amsfonts}\n"
                   "\\usepackage{float} % для нормального расположения картинок и таблиц\n"
                   "\\usepackage[left=15mm, top=15mm, right=15mm, bottom=15mm, nohead, footskip=10mm]{geometry} % настройки полей документа\n"
                   "\\usepackage{graphicx} % работа с картинками \n"
                   "\\usepackage{wrapfig}\n"
                   "\\usepackage{placeins} % работа с картинками (их корректная вставка в текст)\n"
                   "\\usepackage{misccorr} % в заголовках появляется точка, но при ссылке на них ее нет\n"
                   "\\usepackage{indentfirst} % красная строка у первой строки раздела\n"
                   "\\DeclareMathSymbol{,}{\\mathord}{letters}{\"3B} % убирает пробел после запятой в формулах\n"
                   "\\DeclareSymbolFont{lsymb}{U}{euex}{m}{n} % Определяем свой шрифт \"lsumb\"\n"
                   "\\DeclareMathSymbol{\\intop}{\\mathop}{lsymb}{\"52}% Определяем интегралы\n"
                   "\\DeclareMathSymbol{\\ointop}{\\mathop}{lsymb}{\"48}\n"
                   "\\DeclareMathSymbol{\\smallint}{\\mathop}{lsymb}{\"52}\n"
                   "\\usepackage{mathrsfs}%Красивая ЭДС с завитушками\n"
                   "\\DeclareSymbolFont{rsfs}{U}{rsfs}{m}{n} \\DeclareSymbolFontAlphabet{\\mathscr}{rsfs}\n"
                   "\\DeclareMathSymbol{\\EDS}{\\mathord}{rsfs}{`E}\n"
                   "\\usepackage{amssymb} %русские неравенства: \\leqslant \\geqslant\n"
                   "\\usepackage[12pt]{extsizes} %12-ый размер шрифта\n"
                   "\\usepackage{setspace}\n"
                   "\\usepackage{cmap}\n"			
                   "\\usepackage{mathtext}\n"			
                   "\\usepackage[T2A]{fontenc}\n"		
                   "\\usepackage[utf8]{inputenc}\n"	
                   "\\usepackage[english,russian]{babel}\n"
                   "\\usepackage{multirow}\n"
                   "\\usepackage{graphicx}\n"
                   "\\usepackage{wrapfig}\n"
                   "\\usepackage{tabularx}\n"
                   "\\usepackage{float}\n"
                   "\\usepackage{longtable}\n"
                   "\\usepackage{hyperref}\n"
                   "\\hypersetup{colorlinks=true,urlcolor=blue}\n"
                   "\\usepackage[rgb]{xcolor}\n"
                   "\\usepackage{amsmath,amsfonts,amssymb,amsthm,mathtools}\n"
                   "\\usepackage{icomma}\n"
                   "\\mathtoolsset{showonlyrefs=true}\n"
                   "\\usepackage{euscript}\n"
                   "\\usepackage{mathrsfs}\n"
                   "\\DeclareMathOperator{\\sgn}{\\mathop{sgn}}\n"
                   "\\newcommand*{\\hm}[1]{#1\\nobreak\\discretionary{}\n"
                   "    {\\hbox{$\\mathsurround=0pt #1$}}{}\n"
                   "\\title{\\textbf{Методичка по МатАНАЛизу}}\n"
                   "\\author{Шептяков Артём}\n"
                   "\\date{\\today}\n"
                   "\\begin{document} \n \\maketitle \n \\newpage\n\n");

    fprintf(tex_f, "\\section{Аннотация}\n");
    fprintf(tex_f, "");*/

    struct stat file_stat;
    int stat_err = stat(FOREWORD_FILE_PATH, &file_stat); 
    ERROR_CHECK(stat_err == -1, ERROR_STAT);
    
    char *foreword = (char *) calloc(file_stat.st_size, sizeof(char));
    ERROR_CHECK(foreword == NULL, ERROR_CALLOC);

    FILE *foreword_f = fopen(FOREWORD_FILE_PATH, "r");
    fread(foreword, sizeof(char), file_stat.st_size, foreword_f);
    fclose(foreword_f);

    fwrite(foreword, sizeof(char), file_stat.st_size, tex_f);
    free(foreword);

    fprintf(tex_f, "\\newpage \n \\section{Анализ функции} \n"
                   "Теперь можно перейти к анализу заданной функции: \n"
                   "\\[");
    int serialize_node_err = SerializeNode(tex_f, expr->root);
        ERROR_CHECK(serialize_node_err, ERROR_SERIALIZE_NODE);    

    fprintf(tex_f, "\\]");          

    fprintf(tex_f, "\\subsection{Производная %d степени} \n ",diff_dgr);
    fprintf(tex_f, "\\[");
    int serialize_node_err = SerializeNode(tex_f, expr->root);
        ERROR_CHECK(serialize_node_err, ERROR_SERIALIZE_NODE); 

    fprintf(tex_f, " = ");

    struct Tree *diff_tree = TreeCtor();
    ERROR_CHECK(diff_tree == NULL, ERROR_TREE_CTOR);
    diff_tree->root = DifferentiateNode(tree->root, 'x');
    int err_revise = ReviseParentValue(diff_tree->root, NULL);
    ERROR_CHECK(err_revise, 4);

    fprintf(tex_f, "\\]"); 



    fclose(tex_f);
    return SUCCESS;
}


int ExpandInTaylor(FILE *file_o, struct TreeNode *curr_node, 
                   char var, diff_num_t point, unsigned int degree)
{
    ERROR_CHECK(file_o    == NULL, ERROR_NULL_PTR);
    ERROR_CHECK(curr_node == NULL, ERROR_NULL_PTR);

    int serialize_node_err = SerializeNode(file_o, curr_node);
    ERROR_CHECK(serialize_node_err, ERROR_SERIALIZE_NODE);

    fprintf(file_o, " = \n =");

    for (unsigned int diff_degree = 0; diff_degree < degree; diff_degree++)
    {
        for(int i = 0; i < diff_degree; i++)
        {
            curr_node = DifferentiateNode(curr_node, var);
            ERROR_CHECK(curr_node == NULL, ERROR_DIFFERENTIATE_NODE);
            int err_revise = ReviseParentValue(curr_node, NULL);
            ERROR_CHECK(err_revise, ERROR_REVISE_PARENT_NODE);
        }
        
        struct TreeNode *copy_node = TreeNodeCopy(curr_node);
        int frame_var_err = FrameVar(copy_node, var, point);
        ERROR_CHECK(frame_var_err, ERROR_FRAME_VAR);
        copy_node = SimplifyExpression(copy_node);
        ERROR_CHECK(copy_node == NULL, ERROR_SIMPLIFY_EXPRESSION);

        int serialize_node_err = SerializeNode(file_o, copy_node);
        ERROR_CHECK(serialize_node_err, ERROR_SERIALIZE_NODE);
        //..................
    }
}
