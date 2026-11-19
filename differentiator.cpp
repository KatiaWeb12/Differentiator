#include "differentiator.h"

Variables variables = {};

#include "binary-tree.cpp"
#include "graphics.cpp"

//DSL
#define _DL Diff(binaryTree, node->left)
#define _DR Diff(binaryTree, node->right)
#define _CL copyNode(binaryTree, node->left)
#define _CR copyNode(binaryTree, node->right)
#define _ADD(left, right) createNewDiffNode(binaryTree, OP, ADD, left, right)
#define _SUB(left, right) createNewDiffNode(binaryTree, OP, SUB, left, right)
#define _MUL(left, right) createNewDiffNode(binaryTree, OP, MUL, left, right)
#define _DIV(left, right) createNewDiffNode(binaryTree, OP, DIV, left, right)

int main(int argc, char* argv[]){

    struct files usedFiles = {};
    setFileNames(&usedFiles, MAX_PATH_LENGTH, argc, argv);

    BinaryTree_t binaryTree = {};
    initBinaryTree(&binaryTree);

    ErrorCode error = readBinaryTree(usedFiles.inputFile, &binaryTree);
    if(error != ERR_OK) return error;

    createDebugImage(&binaryTree, "binary-tree.png");

    BinaryTree_t diffTree = {};
    initBinaryTree(&diffTree);

    Node* diffTreeRoot = Diff(&diffTree, binaryTree.root);

    diffTree.root = diffTreeRoot;

    error = writeBinaryTree(&diffTree, usedFiles.resultFile);

    if(error != ERR_OK){
        destroyBinaryTree(&diffTree);
        destroyBinaryTree(&binaryTree);
        return error;
    }

    createDebugImage(&diffTree, "diff-binary-tree.png");

    destroyBinaryTree(&binaryTree);
    destroyBinaryTree(&diffTree);

    return 0;
}

Node* Diff(BinaryTree_t* binaryTree, Node* node){

    assert(binaryTree);

    switch(node->type){

        case MY_CONST: return createNewDiffNode(binaryTree, MY_CONST, 0, NULL, NULL);
        case VAR: return createNewDiffNode(binaryTree, MY_CONST, 1, NULL, NULL);
        case OP: switch((int)node->value){
            case ADD: return _ADD(_DL, _DR);
            case SUB: return _SUB(_DL, _DR);
            case MUL: return _ADD(_MUL(_DL, _CR), _MUL(_CL, _DR));
            case DIV: return _DIV(_SUB(_MUL(_DL, _CR), _MUL(_CL, _DR)), _MUL(_CR, _CR));
            default: return NULL;
        }
        default: return NULL;
    }
}

Node* copyNode(BinaryTree_t* binaryTree, const Node* node){

    assert(binaryTree);

    if (node == NULL) {
        return NULL;
    }

    Node* left = copyNode(binaryTree, node->left);

    Node* right = copyNode(binaryTree, node->right);

    Node* newNode = createNewDiffNode(binaryTree, node->type, node->value, left, right);

    if (newNode == NULL) {
        destroyTreeNode(left);
        destroyTreeNode(right);
        return NULL;
    }

    return newNode;
}

Node* createNewDiffNode(BinaryTree_t* binaryTree, NodeType type, double value, Node* left, Node* right){

    assert(binaryTree);

    Node* newNode = NULL;
    ErrorCode error = allocateMemoryForNode(&newNode);
    if(error != ERR_OK) return NULL;

    newNode->type = type;
    newNode->value = value;

    newNode->left = left;
    newNode->right = right;

    if (left != NULL) {
        left->parent = newNode;
    }

    if (right != NULL) {
        right->parent = newNode;
    }

    binaryTree->dim++;

    return newNode;
}
