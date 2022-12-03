
#include "../include/diff.h"

int main(void)
{
    struct Tree *tree = TreeDeserialize(TREE_DESERIALIZATION_PATH);
    ERROR_CHECK(tree == NULL, 1);

    int err = TreeSerialize(tree);
    ERROR_CHECK(err, 1);
    TREEDUMP(tree, "main_dump");

    double point = 0;
    int   diff_dgr = 0;
    int taylor_dgr = 0;

    err = GetStartValues(&point, &diff_dgr, &taylor_dgr);
    ERROR_CHECK(err, 1);

    err = CreateMatanManual(tree, 'x', point, diff_dgr, taylor_dgr);
    ERROR_CHECK(err, 1);

    err = TreeDtor(tree);
    ERROR_CHECK(err, 1);

    system("pdflatex io/metodichka.tex > patch");

    printf("SUCCEFULLY FINISHED\n");

    return 0;
}

