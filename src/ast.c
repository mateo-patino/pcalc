#include "ast.h"
#include "token.h"

#include <stdlib.h>
#include <limits.h>
#include <stdio.h>



ASTNode *init_ast_node(const token_t *tok, ASTNode *left, ASTNode *right) {
    ASTNode *node = malloc(sizeof(ASTNode));
    if (!node) {
        return NULL;
    }
    node->token = tok;
    node->left = left;
    node->right = right;

    return node;
}


ASTNode *create_ast_from_tokens(const token_t *tokens, size_t tc) {
    if (!tokens) {
        return NULL;
    }
    return create_ast_helper(tokens, 0, tc - 1);
}


ASTNode *create_ast_helper(const token_t *tokens, int low, int high) {
    /* Only one token in the range */
    if (low == high) {
        return init_ast_node(tokens + low, NULL, NULL);
    }
    /* Multiple tokens in the range but no more operations (only parens and number remain, so extract the number) */
    else if (!has_any_operations(tokens, low, high)) {
        int on_index = find_only_number(tokens, low, high);

        /* DEBUG PURPOSES. TODO: remove after this invariant has been proven through testing */
        if (on_index == -1) { 
            fprintf(stderr, "\x1b[31m" "Not having any operations does not guarantee that EXACTLY ONE number token exists.\n" "\x1b[0m"); 
            return NULL;
        }
        /* DEBUG PURPOSES */

        return init_ast_node(tokens + on_index, NULL, NULL);
    }

    int lo_index = find_last_operation(tokens, low, high);
    ASTNode *left = create_ast_helper(tokens, low, lo_index - 1);
    ASTNode *right = create_ast_helper(tokens, lo_index + 1, high);

    return init_ast_node(tokens + lo_index, left, right);
}


value_t evaluate_ast(const AST *ast) {
    return evaluate_ast_helper(ast->root);
}


value_t evaluate_ast_helper(const ASTNode *root) {
    const token_t *tok = root->token;
    if (root->token->type == NUMBER) {
        number_t *num = (number_t *)tok->obj;
        return num->value; 
    }

    value_t left = evaluate_ast_helper(root->left);
    value_t right = evaluate_ast_helper(root->right);

    operator_t *oper = (operator_t *)tok->obj;
    operation_type op = oper->op;

    switch (op) {
        case ADD:
            return left + right;
        case SUB:
            return left - right;
        case MUL:
            return left * right;
        case DIV:
            return left / right;
        default:
            fprintf(stderr, "Unknown operation!\n");
            return 0;
    }
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


