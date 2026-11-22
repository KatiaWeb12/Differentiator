#include "differentiator.h"

Variables variables = {};

#include "binary-tree.cpp"
#include "graphics.cpp"
#include "tex-settings.cpp"
#include "simplifies.cpp"

#include "DSL.cpp"

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

    formTexReportFile(&usedFiles, &binaryTree, &diffTree);

    createDebugImage(&diffTree, "diff-binary-tree.png");

    destroyBinaryTree(&binaryTree);
    destroyBinaryTree(&diffTree);

    return 0;
}

Node* Diff(BinaryTree_t* binaryTree, Node* node, FILE* file){

    assert(binaryTree);

    Node* result = NULL;

    switch(node->type){

        case MY_CONST: {
            result = _C(0);
            break;
        }
        case VAR: {
            result = _C(1);
            break;
        }
        case OP: {
            switch((int)node->value){

                case ADD: {
                    result = _ADD(_DL, _DR);
                    break;
                }
                case SUB: {
                    result = _SUB(_DL, _DR);
                    break;
                }
                case MUL: {
                    result = _ADD(_MUL(_DL, _CR), _MUL(_CL, _DR));
                    break;
                }
                case DIV: {
                    result = _DIV(_SUB(_MUL(_DL, _CR), _MUL(_CL, _DR)), _MUL(_CR, _CR));
                    break;
                }
                case SIN: {
                    result = _MUL(_COS(_CL), _DL);
                    break;
                }
                case COS: {
                    result = _MUL(_MUL(_C(-1), _SIN(_CL)), _DL);
                    break;
                }
                case TG: {
                    result = _DIV(_DL, _MUL(_COS(_CL), _COS(_CL)));
                    break;
                }
                case CTG: {
                    result = _DIV(_MUL(_C(-1), _DL), _MUL(_SIN(_CL), _SIN(_CL)));
                    break;
                }
                case LN: {
                    result = _DIV(_DL, _CL);
                    break;
                }
                case EXP: {
                    result = _MUL(_EXP(_CL), _DL);
                    break;
                }
                case POW: {
                    if(node->right->type == MY_CONST){

                        result = _MUL(_MUL(_CR, _POW(_CL, _SUB(_CR, _C(1)))), _DL);
                    }
                    else{

                        result = _MUL(
                                    _EXP(_MUL(_LN(_CL), _CR)),
                                    _ADD(
                                        _MUL(_DIV(_DL, _CL), _CR),
                                        _MUL(_LN(_CL), _DR)
                                    )
                                );
                    }
                    break;
                }
                case LOG: {
                    result = _DIV(
                        _SUB(
                            _MUL(_DIV(_DR, _CR), _LN(_CL)),
                            _MUL(_DIV(_DL, _CL), _LN(_CR))
                        ),
                        _MUL(_LN(_CL), _LN(_CL))
                    );

                    break;
                }
                case ARCSIN: {

                    result = _DIV(_DL, _POW(_SUB(_C(1), _POW(_CL, _C(2))), _C(0.5)));

                    break;
                }
                case ARCCOS: {

                    result = _DIV(_MUL(_C(-1), _DL), _POW(_SUB(_C(1), _POW(_CL, _C(2))),_C(0.5)));

                    break;
                }
                case ARCTG: {

                    result = _DIV(_DL, _ADD(_C(1), _POW(_CL, _C(2))));

                    break;
                }
                case ARCCTG: {

                    result = _DIV(_MUL(_C(-1), _DL), _ADD(_C(1), _POW(_CL, _C(2))));

                    break;
                }
                default: {
                    result = NULL;
                    break;
                }
            }
            break;
        }
        default: {
            result = NULL;
            break;
        }
    }

    if(result != NULL && node->type == OP){

        ErrorCode error = writeDiffStep(file, node, result);
        if(error != ERR_OK) return NULL;
    }

    return result;
}

ErrorCode writeDiffStep(FILE* file, const Node* original, const Node* difElem){

    assert(file);
    assert(original);
    assert(difElem);

    fprintf(file, "\\[\n(");

    ErrorCode error = writeTreeNode(file, original, 0, 0, false);
    if(error != ERR_OK){
        printf("Cannot write expression to TeX\n");
        return error;
    }

    fprintf(file, ")' = ");

    error = writeTreeNode(file, difElem, 0, 0, false);
    if(error != ERR_OK){
        printf("Cannot write expression to TeX\n");
        return error;
    }

    fprintf(file, "\n\\]\n\n");

    return ERR_OK;
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

int getOperationPrecedence(const Node* node){

    if(node == NULL || node->type != OP) return 10;

    switch((int)node->value){

        case ADD:
        case SUB: return 1;

        case MUL:
        case DIV: return 2;

        case POW: return 3;

        default: return 4;
    }
}
