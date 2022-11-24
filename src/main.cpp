
#include "../include/diff.h"

int main(void)
{
    /*printf("Hello\n");
    const char *string = "262.2839";
    double num = 0;
    int err = ConvertStrToNum(string, &num);

    printf("err = %d, num = %lf\n", err, num);
    */

    printf("start\n");
    struct Tree *tree = TreeDeserialize(TREE_DESERIALIZATION_PATH);
    ERROR_CHECK(tree == NULL, 1);

    TREEDUMP(tree, "reading");
    int tree_deserial_err = TreeSerialize(tree);
    ERROR_CHECK(tree_deserial_err, 2);


    struct Tree *diff_tree = TreeCtor();
    ERROR_CHECK(diff_tree == NULL, 3);
    diff_tree->root = DifferentiateNode(tree->root);
    int err_revise = ReviseParentValue(diff_tree->root, NULL);
    ERROR_CHECK(err_revise, 4);

    TREEDUMP(diff_tree, "differ");
        tree_deserial_err = TreeSerialize(diff_tree);
    ERROR_CHECK(tree_deserial_err, 5);

    int tree_dtor_err = TreeDtor(tree);
    ERROR_CHECK(tree_dtor_err, 6);
        tree_dtor_err = TreeDtor(diff_tree);
    ERROR_CHECK(tree_dtor_err, 7);

    printf("SUCCEFULLY FINISHED\n");
    return SUCCESS;
}