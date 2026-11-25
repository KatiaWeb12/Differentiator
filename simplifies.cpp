static bool areEqualTrees(const Node* first, const Node* second);
static void replaceDiffNode(BinaryTree_t* tree, Node* node, Node* keep, Node* discard);
static void destroyDiffSubtree(BinaryTree_t* tree, Node* node);
static void writeSimplificationBefore(FILE* file, const Node* node);
static void writeSimplificationAfter(FILE* file, const Node* node);

void simplifyDiffTree(BinaryTree_t* tree, Node* node, FILE* file){

    assert(tree);
    assert(file);

    if(node == NULL) return;

    simplifyDiffTree(tree, node->left, file);
    simplifyDiffTree(tree, node->right, file);

    if(node->type != OP) return;

    Node* left = node->left;
    Node* right = node->right;

    if(left != NULL && right != NULL && left->type == MY_CONST && right->type == MY_CONST){

        double result = 0;
        bool canCalculate = true;

        switch((int)node->value){

            case ADD:
                result = left->value + right->value;
                break;

            case SUB:
                result = left->value - right->value;
                break;

            case MUL:
                result = left->value * right->value;
                break;

            case DIV:
                if((int)right->value == 0){
                    canCalculate = false;
                }
                else{
                    result = left->value / right->value;
                }
                break;

            default:
                canCalculate = false;
                break;
        }

        if(canCalculate){

            writeSimplificationBefore(file, node);

            destroyDiffSubtree(tree, left);
            destroyDiffSubtree(tree, right);

            node->type = MY_CONST;
            node->value = result;
            node->left = NULL;
            node->right = NULL;

            writeSimplificationAfter(file, node);

            return;
        }
    }

    if((int)node->value == MUL){

        // x * x = x^2

        if(areEqualTrees(left, right)){

            writeSimplificationBefore(file, node);

            destroyDiffSubtree(tree, right->right);

            node->value = POW;

            right->type = MY_CONST;
            right->value = 2;
            right->left = NULL;
            right->right = NULL;

            writeSimplificationAfter(file, node);

            return;
        }

        // 0 * x = 0

        if((left->type == MY_CONST && (int)left->value == 0) ||
           (right->type == MY_CONST && (int)right->value == 0)){

            writeSimplificationBefore(file, node);

            destroyDiffSubtree(tree, left);
            destroyDiffSubtree(tree, right);

            node->type = MY_CONST;
            node->value = 0;
            node->left = NULL;
            node->right = NULL;

            writeSimplificationAfter(file, node);

            return;
        }

        // 1 * x = x

        if(left->type == MY_CONST && (int)left->value == 1){

            writeSimplificationBefore(file, node);

            replaceDiffNode(tree, node, right, left);

            writeSimplificationAfter(file, node);

            return;
        }

        // x * 1 = x

        if(right->type == MY_CONST && (int)right->value == 1){

            writeSimplificationBefore(file, node);

            replaceDiffNode(tree, node, left, right);

            writeSimplificationAfter(file, node);

            return;
        }
    }

    if((int)node->value == ADD){

        // x + x = 2 * x

        if(areEqualTrees(left, right)){

            writeSimplificationBefore(file, node);

            destroyDiffSubtree(tree, left->left);
            destroyDiffSubtree(tree, left->right);

            node->value = MUL;

            left->type = MY_CONST;
            left->value = 2;
            left->left = NULL;
            left->right = NULL;

            writeSimplificationAfter(file, node);

            return;
        }

        // 0 + x = x

        if(left->type == MY_CONST && (int)left->value == 0){

            writeSimplificationBefore(file, node);

            replaceDiffNode(tree, node, right, left);

            writeSimplificationAfter(file, node);

            return;
        }

        if(right->type == MY_CONST && (int)right->value == 0){

            writeSimplificationBefore(file, node);

            replaceDiffNode(tree, node, left, right);

            writeSimplificationAfter(file, node);

            return;
        }
    }

    // x - 0 = x

    if((int)node->value == SUB && right->type == MY_CONST && (int)right->value == 0){

        writeSimplificationBefore(file, node);

        replaceDiffNode(tree, node, left, right);

        writeSimplificationAfter(file, node);

        return;
    }

    // x / 1 = x

    if((int)node->value == DIV && right->type == MY_CONST && (int)right->value == 1){

        writeSimplificationBefore(file, node);

        replaceDiffNode(tree, node, left, right);

        writeSimplificationAfter(file, node);

        return;
    }
    // x^1 = x

    if((int)node->value == POW && right->type == MY_CONST && (int)right->value == 1){

        writeSimplificationBefore(file, node);

        replaceDiffNode(tree, node, left, right);

        writeSimplificationAfter(file, node);

        return;
    }
}

static bool areEqualTrees(const Node* first, const Node* second){

    if(first == NULL || second == NULL) return first == second;

    if((int)first->type != (int)second->type) return false;

    if((int)first->value != (int)second->value) return false;

    return areEqualTrees(first->left, second->left) && areEqualTrees(first->right, second->right);
}

static void replaceDiffNode(BinaryTree_t* tree, Node* node, Node* keep, Node* discard){

    destroyDiffSubtree(tree, discard);

    node->type = keep->type;
    node->value = keep->value;

    node->left = keep->left;
    node->right = keep->right;

    if(node->left != NULL){
        node->left->parent = node;
    }

    if(node->right != NULL){
        node->right->parent = node;
    }

    free(keep);

    tree->dim--;
}

static void destroyDiffSubtree(BinaryTree_t* tree, Node* node){

    assert(tree);

    if(node == NULL) return;

    destroyDiffSubtree(tree, node->left);
    destroyDiffSubtree(tree, node->right);

    free(node);

    tree->dim--;
}

static void writeSimplificationBefore(FILE* file, const Node* node){

    assert(file);
    assert(node);

    fprintf(file, "\\[\n");

    writeTreeNode(file, node, 0, 0, false);

    fprintf(file, " \\longrightarrow ");
}

static void writeSimplificationAfter(FILE* file, const Node* node){

    assert(file);

    writeTreeNode(file, node, 0, 0, false);

    fprintf(file, "\n\\]\n\n");
}
