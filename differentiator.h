#include <TXLib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <assert.h>
#include <stdlib.h>
#include <cctype> // для isspace
#include <locale.h>
#include <string.h>

#include "../universal-features/error.h"
#include "../universal-features/colors.h"
#include "../universal-features/useful-functions.h"

const size_t MAX_VARIABLES_COUNT = 100;
const size_t MAX_PROGRAM_CAPACITY = 1000;

const char* diffFileName = "differentiator.txt";
const char* diffReportFileName = "differentiator-report.txt";

enum NodeType{
    MY_CONST = 1,
    VAR = 2,
    OP = 3
};

enum Operation{
    ADD = '+',
    SUB = '-',
    MUL = '*',
    DIV = '/',
};

struct Variables{
    size_t variablesCount;
    char* varArr[MAX_VARIABLES_COUNT];
};

struct Node{
    NodeType type;
    double value;
    Node* left;
    Node* right;
    Node* parent;
};

struct BinaryTree_t{
    Node* root;
    size_t dim;
};

//Binary Tree
void initBinaryTree(BinaryTree_t* binaryTree);
ErrorCode readBinaryTree(const char* fileName, BinaryTree_t* binaryTree);
void destroyBinaryTree(BinaryTree_t* binaryTree);
ErrorCode verifyBinaryTree(const BinaryTree_t* binaryTree, const char* function, int line);
ErrorCode writeBinaryTree(BinaryTree_t* binaryTree, const char* fileName);

//Graphics
ErrorCode createDebugImage(BinaryTree_t* binaryTree, const char* fileName);

//Diff
Node* createNewDiffNode(BinaryTree_t* binaryTree, NodeType type, double value, Node* left, Node* right);
Node* copyNode(BinaryTree_t* binaryTree, const Node* node);
Node* Diff(BinaryTree_t* binaryTree, Node* node);


#define VERIFY_OK(binaryTree, function)                                                 \
    do {                                                                                \
        if (verifyBinaryTree((binaryTree), (function), __LINE__) != ERR_OK) {           \
            abort();                                                                    \
        }                                                                               \
    } while (0)
