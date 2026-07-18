#include "ast.h"
#include "token.h"

#include <stdlib.h>
#include <limits.h>
#include <stdio.h>
#include <inttypes.h>
#include <limits.h>



/*
* Unary operators only have one child. The other child will point to this sentinel object.
* This object shall never be dereferenced or inspected. 
*/
static ASTNode unary_child_sentinel;
#define UNARY_OPERATOR_CHILD (&unary_child_sentinel)


bool is_unary_operator(const token_t *token) {
    return token && token->type == OPERATOR && ((operator_t *)token->obj)->arity == 1;
}


ASTNode *init_ast_node(const token_t *tok, ASTNode *left, ASTNode *right) {
    ASTNode *node = malloc(sizeof(ASTNode));
    if (!node || !tok) {
        return NULL;
    }
    node->token = tok;
    /*
    * If the token is a unary operator, we set one child pointer to UNARY_OPERATOR_CHILD (a sentinel value)
    * depending on the operator's associativity. If the token is a binary operator or number, we simply set
    * the children pointers to the arguments provided by the caller.
    */
    if (is_unary_operator(tok)) {
        operation_type op = ((operator_t *)tok->obj)->op;
        switch (op_associativity[op]) {
            case ASSOC_LEFT:
                node->left = left;
                node->right = UNARY_OPERATOR_CHILD;
                break;
            case ASSOC_RIGHT:
                node->left = UNARY_OPERATOR_CHILD;
                node->right = right;
                break;
            default:
                break;
        }
        return node; 
    }

    node->left = left;
    node->right = right;

    return node;   
}

ASTNode *create_ast_from_tokens(const token_t *tokens, size_t tc, ast_status *status) {
    if (!tokens) {
        if (status) { *status = AST_INVALID_ARG; }
        return NULL;
    }
    return create_ast_helper(tokens, 0, tc - 1, status);
}


ASTNode *create_ast_helper(const token_t *tokens, int low, int high, ast_status *status) {
    if (status && *status != AST_OK) {
        return NULL;
    }

    ASTNode *new_node;

    /* Only one token in the range */
    if (low == high) {
        new_node = init_ast_node(tokens + low, NULL, NULL);
        goto RETURN_NEW_NODE;
    }
    /* Multiple tokens in the range but no more operations (only parens and number remain, so extract the number) */
    else if (!has_any_operations(tokens, low, high)) {
        /* A key invariant: no operations guarantees EXACTLY one number exists in [low, high]. If no numbers or more
        * than one exists in [low, high], the user's expression must be structurally invalid. In that case, 
        * find_only_number returns -1 to signal failure all the way up the recursion stack. */
        int on_index = find_only_number(tokens, low, high);
        if (on_index == -1) { 
            if (status) { *status = AST_INVALID_EXPRESSION; }
            return NULL;
        }
        new_node = init_ast_node(tokens + on_index, NULL, NULL);
        goto RETURN_NEW_NODE;
    }

    int lo_index = find_last_operation(tokens, low, high);
    ASTNode *left = create_ast_helper(tokens, low, lo_index - 1, status);
    if (!left || (status && *status != AST_OK)) {
        return NULL;
    }
    ASTNode *right = create_ast_helper(tokens, lo_index + 1, high, status);
    if (!right || (status && *status != AST_OK)) {
        return NULL;
    }
    new_node = init_ast_node(tokens+lo_index, left, right);

RETURN_NEW_NODE:
    if (!new_node) {
        if (status) { *status = AST_MALLOC_FAILURE; }
        return NULL;
    }
    return new_node;
}


value_t evaluate_ast(const AST *ast, ast_status *status) {
    if (!ast || !ast->root) {
        if (status) { *status = AST_INVALID_ARG; }
        return (value_t)0;
    }
    if (status) { *status = AST_OK; } /* Set status to ok before starting evaluation */
    return evaluate_ast_helper(ast->root, status);
}


value_t evaluate_ast_helper(const ASTNode *root, ast_status *status) {
    if (status && *status != AST_OK) {
        return (value_t)0;
    }

    if (!root || !root->token) {
        if (status) { *status = AST_INVALID_EXPRESSION; }
        return (value_t)0;
    }

    const token_t *tok = root->token;
    if (tok->type == NUMBER) {
        number_t *num = (number_t *)tok->obj;
        return num->value; 
    }

    value_t left = evaluate_ast_helper(root->left, status);
    if (status && *status != AST_OK) {
        return (value_t)0;
    }
    value_t right = evaluate_ast_helper(root->right, status);
    if (status && *status != AST_OK) {
        return (value_t)0;
    }

    if (tok->type != OPERATOR) {
        if (status) { *status = AST_EXPECTED_OPERATOR; }
        return (value_t)0;
    }

    operator_t *oper = (operator_t *)tok->obj;
    operation_type op = oper->op;

    value_t retval;
    switch (op) {
        case OR:
            retval = op_bitwise_or(left, right, status);
            if (status && *status != AST_OK) {
                return (value_t)0;
            }
            break;
        case XOR:
            retval = op_bitwise_xor(left, right, status);
            if (status && *status != AST_OK) {
                return (value_t)0;
            }
            break;
        case AND:
            retval = op_bitwise_and(left, right, status);
            if (status && *status != AST_OK) {
                return (value_t)0;
            }
            break;
        case LSHIFT:
            retval = op_bitwise_lshift(left, right, status);
            if (status && *status != AST_OK) {
                return (value_t)0;
            }
            break;
        case RSHIFT:
            retval = op_bitwise_rshift(left, right, status);
            if (status && *status != AST_OK) {
                return (value_t)0;
            }
            break;
        case ADD:
            retval = op_add(left, right, status);
            if (status && *status != AST_OK) {
                return (value_t)0;
            }
            break;
        case SUB:
            retval = op_sub(left, right, status);
            if (status && *status != AST_OK) {
                return (value_t)0;
            }
            break;
        case MUL:
            retval = op_mul(left, right, status);
            if (status && *status != AST_OK) {
                return (value_t)0;
            }
            break;
        case DIV:
            retval = op_div(left, right, status);
            if (status && *status != AST_OK) {
                return (value_t)0;
            }
            break;
        default:
            if (status) { *status = AST_UNKNOWN_OPERATION; }
            return (value_t)0;
    }
    return retval;
}


int find_last_operation(const token_t *tokens, int low, int high) {
    if (!tokens) {
        return -1;
    }
    token_type type;
    last_op_tuple last_op = { CHAR_MAX, CHAR_MAX, -1 };
    char curr_depth = 0;
    for (int i = low; i <= high; i++) {
        type = tokens[i].type;
        if (type == NUMBER) {
            continue;
        }
        else if (type == LPAREN) {
            curr_depth++;
        }
        else if (type == RPAREN) {
            curr_depth--;
        }
        else if (type == OPERATOR) {
            operator_t *oper = tokens[i].obj;

            /* Choose op with smallest depth */
            if (curr_depth < last_op.depth) {
                last_op.depth = curr_depth;
                last_op.precedence = oper->precedence;
                last_op.index = i;
            }
            else if (curr_depth == last_op.depth) {

                /* Choose op with lowest precedence if depth is equal */
                if (oper->precedence < last_op.precedence) {
                    last_op.precedence = oper->precedence;
                    last_op.index = i;
                }
                else if (oper->precedence == last_op.precedence) {
                    /*
                    * If same depth and precedence, choose the rightmost op if the operators
                    * are left-associative and the leftmost op if they are right-associative. 
                    * Note: Two operators with the same precedence MUST have equal associativity, 
                    * so we can check the assoc of either 'oper' or 'last_op' 
                    */
                    if (op_associativity[oper->op] == ASSOC_LEFT) {
                        if (i > last_op.index) {
                            last_op.index = i;
                        }
                    }
                    else if (op_associativity[oper->op] == ASSOC_RIGHT) {
                        if (i < last_op.index) {
                            last_op.index = i;
                        }
                    }
                }
            }
        }
    }
    return last_op.index;
}


int find_only_number(const token_t *tokens, int low, int high) {
    if (!tokens) {
        return -1;
    }
    int index = -1;
    for (int i = low; i <= high; i++) {
        if (tokens[i].type == NUMBER) {
            if (index != -1) {
                return -1;
            }
            index = i;
        }
    }
    return index;
}


void free_ast(AST *ast) {
    if (!ast || !ast->root) {
        return;
    }
    free_subtree(ast->root); 
}


void free_subtree(ASTNode *node) {
    if (node == NULL) {
        return;
    }
    free_subtree(node->left);
    free_subtree(node->right);
    free(node);
}


bool has_any_operations(const token_t *tokens, int low, int high) {
    if (!tokens) {
        return NULL;
    }
    for (int i = low; i <= high; i++) {
        if (tokens[i].type == OPERATOR) {
            return true;
        }
    }
    return false;
}


value_t op_bitwise_or(value_t left, value_t right, ast_status *status) {
    if (status) { *status = AST_OK; } /* OR will always succeed */
    return left | right;
}


value_t op_bitwise_xor(value_t left, value_t right, ast_status *status) {
    if (status) { *status = AST_OK; } /* XOR will always succeed */
    return left ^ right;
}


value_t op_bitwise_and(value_t left, value_t right, ast_status *status) {
    if (status) { *status = AST_OK; } /* AND will always succeed */
    return left & right;
}


value_t op_add(value_t left, value_t right, ast_status *status) {
    if (left > VALUE_T_MAX - right) {
        if (status) { *status = AST_INTEGER_OVERFLOW; }
        return (value_t)0;
    }
    return left + right;
}


value_t op_bitwise_lshift(value_t left, value_t right, ast_status *status) {
    if (right > CHAR_BIT * sizeof(value_t) - 1) {
        if (status) { *status = AST_SHIFT_COUNT_TOO_LARGE; }
        return (value_t)0;
    }
    return left << right;
}

value_t op_bitwise_rshift(value_t left, value_t right, ast_status *status) {
    if (right > CHAR_BIT * sizeof(value_t) - 1) {
        if (status) { *status = AST_SHIFT_COUNT_TOO_LARGE; }
        return (value_t)0;
    }
    return left >> right;
}


value_t op_sub(value_t left, value_t right, ast_status *status) {
    if (left < right) {
        if (status) { *status = AST_INTEGER_UNDERFLOW; }
        return (value_t)0;
    }
    return left - right;

}


value_t op_mul(value_t left, value_t right, ast_status *status) {
    if (right != 0 && left > VALUE_T_MAX / right) {
        if (status) { *status = AST_INTEGER_OVERFLOW; }
        return (value_t)0;
    }
    return left * right;
}


value_t op_div(value_t left, value_t right, ast_status *status) {
    if (right == 0) {
        if (status) { *status = AST_DIV_BY_ZERO; }
        return (value_t)0;
    }
    return left / right;
}


void print_ast_error(ast_status code, char *msg) {
    if (!msg) {
        msg = "";
    }
    switch (code) {
        case AST_OK:
            fprintf(stdout, "AST is valid. %s\n", msg);
            break;
        case AST_INVALID_ARG:
            fprintf(stderr, "Error: invalid argument. %s\n", msg);
            break;
        case AST_INTEGER_OVERFLOW:
            fprintf(stderr, "Error: integer overflow. %s\n", msg);
            break;
        case AST_INTEGER_UNDERFLOW:
            fprintf(stderr, "Error: integer underflow. %s\n", msg);
            break;
        case AST_DIV_BY_ZERO:
            fprintf(stderr, "Error: division by zero. %s\n", msg);
            break;
        case AST_SHIFT_COUNT_TOO_LARGE:
            int size = CHAR_BIT * sizeof(value_t);
            fprintf(stderr, "Error: shift count too large. Shift counts > %i are " 
                    "undefined for %i-bit unsigned integers.\n", size-1, size);
            break;
        case AST_INVALID_EXPRESSION:
            fprintf(stderr, "Error: invalid expression.%s\n", msg);
            break;
        case AST_EXPECTED_OPERATOR:
            fprintf(stderr, "Error: expected operator. %s\n", msg);
            break;
        case AST_UNKNOWN_OPERATION:
            fprintf(stderr, "Error: unknown operation. %s\n", msg);
            break;
        case AST_MALLOC_FAILURE:
            fprintf(stderr, "Error: malloc() failed. %s\n", msg);
            break;
        default:
            fprintf(stderr, "%i is not a registered AST status code. \n", code);
    } 
}
