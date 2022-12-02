
#include "../include/diff.h"

int main(void)
{
    struct Tree *tree = TreeDeserialize(TREE_DESERIALIZATION_PATH);
    ERROR_CHECK(tree == NULL, 1);

    int tree_deserial_err = TreeSerialize(tree);
    ERROR_CHECK(tree_deserial_err, 1);


    double point = 0;
    int   diff_dgr = 0;
    int taylor_dgr = 0;

    int get_start_val_err = GetStartValues(&point, &diff_dgr, &taylor_dgr);
    ERROR_CHECK(get_start_val_err, ERROR_GET_START_VALUES);

    /*struct TreeNode *extra_diff_node = DifferentiateNode(tree->root, 'x');
    ERROR_CHECK(extra_diff_node == NULL, ERROR_DIFFERENTIATE_NODE);

    NODEDUMP(extra_diff_node, "scnd");

    tree->root = SimplifyExpression(extra_diff_node);
    ERROR_CHECK(tree->root == NULL, ERROR_SIMPLIFY_EXPRESSION);*/

    TREEDUMP(tree, "scnd");

    printf("point = %lf, diff_dgr = %d, taylor_dgr = %d\n", point, diff_dgr, taylor_dgr);
    int err = CreateMatanManual(tree, 'x', point, diff_dgr, taylor_dgr);
    ERROR_CHECK(err, 10);

    int tree_dtor_err = TreeDtor(tree);
    ERROR_CHECK(tree_dtor_err, 1);

    system("pdflatex io/metodichka.tex > patch");

    printf("SUCCEFULLY FINISHED\n");
    return SUCCESS;
}

