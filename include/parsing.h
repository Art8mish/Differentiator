#ifndef PARSING_H_INCLUDED
#define PARSING_H_INCLUDED

#include "diff.h"

#define SKIP_SPACE(buf, success)                            \
            while(isspace(*(buf)) || *(buf) == '\0')        \
            {                                               \
                (buf)++;                                    \
                if (*(buf) == EOF)                          \
                    return success;                         \
            }

#define SKIP_SPACE_STR(buf)                                 \
            while(isspace(*(buf)))                          \
            {                                               \
                (buf)++;                                    \
                if (*(buf) == EOF)                          \
                    return NULL;                            \
            }

#define LEX_SKIP_SPACE(buf, buf_skip_len)                   \
            while(isspace(*(buf)))                          \
            {                                               \
                (buf)++;                                    \
                (buf_skip_len)++;                           \
                if (*(buf) == EOF)                          \
                    return NULL;                            \
            }

#define LEX_ERROR(cond, error, buf_pos)                     \
            do                                              \
            {                                               \
                SOFT_ASSERT(cond);                          \
                printf("ERROR in symb %d\n", buf_pos);      \
                if (cond)                                   \
                    return error;                           \
            } while(false)


struct DiffLexicalElem
{
    int type_arg              = TYPE_PSN;
    struct Diff_elem_t arg    = { };
    unsigned int symb_pos_num = 0;
};


struct TreeNode *ReadExpression(const char *str);
struct TreeNode *GetAdd(const char **buf);
struct TreeNode *GetMul(const char **buf);
struct TreeNode *GetPow(const char **buf);
struct TreeNode *GetUnarFunc(const char **buf);
struct TreeNode *GetBrackets(const char **buf);
struct TreeNode *GetArg(const char **buf);

size_t ConvertStrToNum(const char **string, double *num);


#endif