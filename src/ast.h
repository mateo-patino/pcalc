#ifndef AST_H
#define AST_H

#include "token.h"

typedef enum {
    AST_OK,
    AST_INVALID_ARG,
    AST_INTEGER_OVERFLOW,
    AST_INTEGER_UNDERFLOW,
    AST_DIV_BY_ZERO,
    AST_SHIFT_COUNT_TOO_LARGE,
    AST_INVALID_EXPRESSION, /* Structurally incorrect inputs */
    AST_EXPECTED_OPERATOR,
    AST_UNKNOWN_OPERATION,
    AST_MALLOC_FAILURE
} ast_status;


/* AST node */
typedef struct ASTNode {
    const token_t *token;
    struct ASTNode *left;
    struct ASTNode *right;
} ASTNode;


/* 
* Initializes a parametrized ASTNode object. It returns the address of the new node, 
* which is allocated in HEAP memory. 
*/
ASTNode *init_ast_node(const token_t *tok, ASTNode *left, ASTNode *right);


/* AST object */
typedef struct {
    ASTNode *root;
} AST;


/*
* Builds an Abstract Syntax Tree (AST) given a tokens array 'tokens' and the number
* of tokens in the array 'tc' (token count).
*
* Returns a pointer to an ASTNode representing the root of the tree. NULL is returned
* upon error, and if 'status' is not NULL, a tokens_status code is written there to
* to describe the error.
*
* If an error occurs at any point throughout the AST building process, 'status' is the
* only way recursive calls can know something wrong happened and halt the creation of the
* AST.
*
* Always set 'status' to AST_OK before calling this function.
*/
ASTNode *create_ast_from_tokens(const token_t *tokens, size_t tc, ast_status *status);


/*
* Called by create_ast_from_tokens to perform the recursive routine and build the AST.
*
* If an error occurs, NULL is returned and the error is written to 'status' if not NULL.
*/
ASTNode *create_ast_helper(const token_t *tokens, int low, int high, ast_status *status);


/* 
* Traverses the AST recursively to evaluate it.
* Returns the result of the evaluation as a value_t type. 
*
* If evaluation fails, the error is written to 'status' if not NULL. The caller
* should always check 'status' before using the value returned by this function.
* If 'status' is not AST_OK, the value returned by evaluate_ast should be ignored.
*
* Always set 'status' to AST_OK before evaluating the AST.
*/
value_t evaluate_ast(const AST *ast, ast_status *status);


/* 
* Recursively evaluates an AST.
*/
value_t evaluate_ast_helper(const ASTNode *root, ast_status *status);


typedef struct {
    char depth;
    char precedence;
    int index;
} last_op_tuple;

/*
* Searches for the operator token that represents the last operation to be evaluated in
* the range [low, high] (inclusive).
*
* It returns the index of this operator token upon success and -1 if no token is found.
*
* It compares operators by recording 3 values about each one: depth, precedence, and index.
* Every operator token has a unique tuple of these 3 values.
*/
int find_last_operation(const token_t *tokens, int low, int high);


/*
* Returns the index of a single token of number type found in the range [low, high].
*
* It returns -1 if none were found or if more than one number token exists in the range (this signifies 
* that the AST building algorithm is wrong).
*/
int find_only_number(const token_t *tokens, int low, int high);


/* 
* Frees the ASTNode objects in an AST.
*/
void free_ast(AST *ast);


/* 
* Traverses the tree rooted at 'node' in a post-order fashion and calls free on each node.
*
* Note that only the memory allocated for the 3 pointers in the ASTNode struct is freed. The 
* token_t object pointed at by the 'token' member is NOT freed.
*/
void free_subtree(ASTNode *node);


/* 
* Returns true if an array of tokens has at least one operation token in the range 
* contaiing the indices [low, high].
*/
bool has_any_operations(const token_t *tokens, int low, int high);


/*
* Returns true if 'token' is a unary operator.
*/
bool is_unary_operator(const token_t *token);


/* 
* Prints an error message related to an operation described by 'code'.
* 'msg' is an optional message attached at the end of the error displayed.
*/
void print_ast_error(ast_status code, char *msg);


/*
* Returns 'left' | 'right'. There is no error check needed for bitwise OR.
*/
value_t op_bitwise_or(value_t left, value_t right, ast_status *status);

/*
* Returns 'left' ^ 'right'. There is no error check needed for bitwise XOR.
*/
value_t op_bitwise_xor(value_t left, value_t right, ast_status *status);

/*
* Returns 'left' & 'right'. There is no error check needed for bitwise AND.
*/
value_t op_bitwise_and(value_t left, value_t right, ast_status *status);


/*
* Returns 'left' << 'right' if 'right' is less than 'right' is less than the
* number of bits in value_t minus one.
*/
value_t op_bitwise_lshift(value_t left, value_t right, ast_status *status);

/*
* Returns 'left' >> 'right' if 'right' is less than 'right' is less than the
* number of bits in value_t minus one.
*/
value_t op_bitwise_rshift(value_t left, value_t right, ast_status *status);

/* 
* Returns 'left' + 'right' if the addition does not result in overflow.
*/
value_t op_add(value_t left, value_t right, ast_status *status);


/*
* Returns 'left' - 'right' if the substraction does not result in a negative number.
*/
value_t op_sub(value_t left, value_t right, ast_status *status);


/*
* Returns 'left' * 'right' if the multiplication does not result in overflow.
*/
value_t op_mul(value_t left, value_t right, ast_status *status);


/*
* Returns 'left' / 'right' if 'right' != 0.
*/
value_t op_div(value_t left, value_t right, ast_status *status);

#endif
