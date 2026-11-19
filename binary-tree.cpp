static ErrorCode createTreeNode(BinaryTree_t* binaryTree,
                                Node** currentNode,
                                NodeType type,
                                double value,
                                Node** newNode);
static ErrorCode allocateMemoryForNode(Node** treeNode);
static ErrorCode formBinaryTree(BinaryTree_t* binaryTree, char* treeText);
static ErrorCode writeTreeNode(FILE* file, const Node* node);
static void destroyTreeNode(Node* node);
static ErrorCode formAfterOpenedBracket(BinaryTree_t* binaryTree, char** current, Node** currentNode);
static ErrorCode verifyNode(const Node* node,
                       const Node* parent,
                       const BinaryTree_t* binaryTree,
                       size_t* count,
                       const char* function,
                       int line);


void initBinaryTree(BinaryTree_t* binaryTree){

    assert(binaryTree);

    binaryTree->root = NULL;
    binaryTree->dim = 0;
}

ErrorCode readBinaryTree(const char* fileName, BinaryTree_t* binaryTree){

    assert(fileName);

    FILE* file = fopen(fileName, "r");
    if (file == NULL) {
        error_debug debugLogInfo = {};
        ERROR_STRUCT_FORMAT(&debugLogInfo, __func__, __LINE__, "File-problem", ERR_FILE_NOT_FOUND);
        printErrorIntoConsole(&debugLogInfo);
        printf("Cannot open file: %s\n", fileName);

        return ERR_FILE_NOT_FOUND;
    }

    char treeText[MAX_PROGRAM_CAPACITY] = "";

    size_t textRead = fread(treeText, 1, MAX_PROGRAM_CAPACITY, file);
    treeText[textRead] = '\0';

    FCLOSE(file);

    return formBinaryTree(binaryTree, treeText);

}

static ErrorCode formBinaryTree(BinaryTree_t* binaryTree, char* treeText){

    assert(binaryTree);
    assert(treeText);

    Node* currentNode = NULL;
    char* current = treeText;

    while(*current != '\0'){

        if (isspace(*current)) {

            current++;
            continue;
        }
        else if(*current == '('){

            ErrorCode error = formAfterOpenedBracket(binaryTree, &current, &currentNode);
            if(error != ERR_OK) return error;
        }

        else if(*current == ')'){

            if(currentNode == NULL){
                error_debug debugLogInfo = {};
                ERROR_STRUCT_FORMAT(&debugLogInfo,
                                    __func__,
                                    __LINE__,
                                "Incorrect input-data. Invalid brackets sequence. Extra ')'",
                                ERR_INVALID_ARGUMENT);
                printErrorIntoConsole(&debugLogInfo);

                return ERR_INVALID_ARGUMENT;
            }

            currentNode = currentNode->parent;
            current++;

        }
        else{
            error_debug debugLogInfo = {};
            ERROR_STRUCT_FORMAT(&debugLogInfo,
                                __func__,
                                __LINE__,
                            "Incorrect input-data. Invalid symbol in file.",
                            ERR_INVALID_ARGUMENT);
            printErrorIntoConsole(&debugLogInfo);

            return ERR_INVALID_ARGUMENT;
        }

    }

    return ERR_OK;
}

static ErrorCode formAfterOpenedBracket(BinaryTree_t* binaryTree, char** current, Node** currentNode){

    assert(binaryTree);
    assert(current);
    assert(currentNode);

    (*current)++;

    while (isspace(**current)) (*current)++;

    char* start = *current;

    while (**current != '\0' && !isspace(**current) && **current != '(' && **current != ')') (*current)++;

    if (start == *current){

        error_debug debugLogInfo = {};
        ERROR_STRUCT_FORMAT(&debugLogInfo,
                            __func__,
                            __LINE__,
                            "Incorrect input-data",
                            ERR_INVALID_ARGUMENT);
        printErrorIntoConsole(&debugLogInfo);

        return ERR_INVALID_ARGUMENT;
    }

    char delimiter = **current;

    **current = '\0';

    size_t length = strlen(start);

    NodeType type = MY_CONST;
    double value = 0;

    int readCount = 0;

    if (sscanf(start, "%lf%n", &value, &readCount) == 1 && readCount == (int)length) {

        type = MY_CONST;
    }
    else if (length == 1 && (*start == '+' || *start == '-' || *start == '*' || *start == '/')){

        type = OP;
        value = *start;
    }
    else{
        type = VAR;

        size_t index = 0;

        while (index < variables.variablesCount && strcmp(variables.varArr[index], start) != 0) index++;

        if(index == variables.variablesCount){ //variable not found

            char* memory = strdup(start);
            if(memory == NULL){
                error_debug debugLogInfo = {};
                ERROR_STRUCT_FORMAT(&debugLogInfo,
                                    __func__,
                                    __LINE__,
                                "Incorrect input-data. Invalid symbol in file.",
                                ERR_OUT_OF_MEMORY);
                printErrorIntoConsole(&debugLogInfo);

                return ERR_OUT_OF_MEMORY;
            }

            variables.varArr[index] = memory;
            variables.variablesCount++;

        }
        value = (double)index;
    }

    Node* newNode = NULL;
    ErrorCode error = createTreeNode(binaryTree, currentNode, type, value, &newNode);
    if (error != ERR_OK) return error;

    if (delimiter == ')') {
        *currentNode = (*currentNode)->parent;
    }

    if (delimiter != '\0') {
        (*current)++;
    }

    return ERR_OK;

}

static ErrorCode createTreeNode(BinaryTree_t* binaryTree,
                                Node** currentNode,
                                NodeType type,
                                double value,
                                Node** newNode){

    assert(currentNode);
    assert(binaryTree);
    assert(newNode);

    ErrorCode error = allocateMemoryForNode(newNode);
    if(error != ERR_OK) return error;

    (*newNode)->type = type;
    (*newNode)->value = value;

    if(*currentNode == NULL) binaryTree->root = *newNode;

    else{
        (*newNode)->parent = *currentNode;

        if((*newNode)->parent->left == NULL){

            (*newNode)->parent->left = *newNode;
        }
        else if((*newNode)->parent->right == NULL){

            (*newNode)->parent->right = *newNode;
        }

    }

    *currentNode = *newNode;
    binaryTree->dim++;

    return ERR_OK;
}

static ErrorCode allocateMemoryForNode(Node** treeNode){

    Node* memory = (Node*)calloc(1, sizeof(Node));
    if (memory == NULL){
        error_debug debugLogInfo = {};
        ERROR_STRUCT_FORMAT(&debugLogInfo, __func__, __LINE__, "ERR_OUT_OF_MEMORY", ERR_OUT_OF_MEMORY);
        printErrorIntoConsole(&debugLogInfo);
        return ERR_OUT_OF_MEMORY;
    }

    *treeNode = memory;

    return ERR_OK;
}

ErrorCode writeBinaryTree(BinaryTree_t* binaryTree, const char* fileName){

    assert(binaryTree);
    assert(fileName);

    VERIFY_OK(binaryTree, __func__);

    FILE* file = fopen(fileName, "w");
    if (file == NULL) {
        error_debug debugLogInfo = {};
        ERROR_STRUCT_FORMAT(&debugLogInfo, __func__, __LINE__, "File-problem", ERR_FILE_NOT_FOUND);
        printErrorIntoConsole(&debugLogInfo);
        printf("Cannot open file: %s\n", fileName);

        return ERR_FILE_NOT_FOUND;
    }

    ErrorCode error  = writeTreeNode(file, binaryTree->root);
    if(error != ERR_OK) return error;

    FCLOSE(file);

    return ERR_OK;
}

static ErrorCode writeTreeNode(FILE* file, const Node* node){

    assert(file);
    assert(node);

    fprintf(file, "(");

    if (node->type == MY_CONST){

        fprintf(file, "%g", node->value);
    }

    else if (node->type == VAR){

        size_t index = (size_t)node->value;

        if (index >= variables.variablesCount){
            error_debug debugLogInfo = {};
            ERROR_STRUCT_FORMAT(&debugLogInfo, __func__, __LINE__, "", ERR_FILE_NOT_FOUND);
            printErrorIntoConsole(&debugLogInfo);
            return ERR_INVALID_ARGUMENT;
        }

        fprintf(file, "%s", variables.varArr[index]);
    }

    else if (node->type == OP){

        fprintf(file, "%c", (char)node->value);

        if (node->left == NULL || node->right == NULL){
            error_debug debugLogInfo = {};
            ERROR_STRUCT_FORMAT(&debugLogInfo, __func__, __LINE__, "", ERR_FILE_NOT_FOUND);
            printErrorIntoConsole(&debugLogInfo);
            return ERR_INVALID_ARGUMENT;
        }

        fprintf(file, " ");

        ErrorCode error = writeTreeNode(file, node->left);
        if (error != ERR_OK) return error;

        fprintf(file, " ");

        error = writeTreeNode(file, node->right);
        if (error != ERR_OK) return error;
    }

    else{
        error_debug debugLogInfo = {};
        ERROR_STRUCT_FORMAT(&debugLogInfo, __func__, __LINE__, "Incorrect type of Node", ERR_FILE_NOT_FOUND);
        printErrorIntoConsole(&debugLogInfo);
        return ERR_INVALID_ARGUMENT;
    }

    fprintf(file, ")");

    return ERR_OK;
}

void destroyBinaryTree(BinaryTree_t* binaryTree){

    assert(binaryTree);

    VERIFY_OK(binaryTree, __func__);

    destroyTreeNode(binaryTree->root);

    binaryTree->root = NULL;
    binaryTree->dim = 0;
}

static void destroyTreeNode(Node* node){

    if (node == NULL) return;

    destroyTreeNode(node->left);
    destroyTreeNode(node->right);

    free(node);
}

ErrorCode verifyBinaryTree(const BinaryTree_t* binaryTree, const char* function, int line){

    if (binaryTree == NULL){
        error_debug debugLogInfo = {};
        ERROR_STRUCT_FORMAT(&debugLogInfo, function, line, "NULL binaryTree", ERR_INVALID_ARGUMENT);
        printErrorIntoConsole(&debugLogInfo);

        return ERR_INVALID_ARGUMENT;
    }

    if (binaryTree->root == NULL) {
        error_debug debugLogInfo = {};
        ERROR_STRUCT_FORMAT(&debugLogInfo, function, line, "NULL binaryTree->root", ERR_INVALID_ARGUMENT);
        printErrorIntoConsole(&debugLogInfo);

        return ERR_INVALID_ARGUMENT;
    }

    if (binaryTree->dim == 0) {
        error_debug debugLogInfo = {};
        ERROR_STRUCT_FORMAT(&debugLogInfo, function, line, "DIM = 0", ERR_INVALID_ARGUMENT);
        printErrorIntoConsole(&debugLogInfo);

        return ERR_INVALID_ARGUMENT;
    }

    if (binaryTree->root->parent != NULL){
        error_debug debugLogInfo = {};
        ERROR_STRUCT_FORMAT(&debugLogInfo, function, line, "INVALID root parent", ERR_INVALID_ARGUMENT);
        printErrorIntoConsole(&debugLogInfo);

        return ERR_INVALID_ARGUMENT;
    };

    size_t count = 0;

    return verifyNode(binaryTree->root, NULL, binaryTree, &count, function, line);
}

static ErrorCode verifyNode(const Node* node, const Node* parent, const BinaryTree_t* binaryTree,
                            size_t* count, const char* function, int line){

    if (node == NULL) return ERR_OK;

    if (*count >= binaryTree->dim){
        error_debug debugLogInfo = {};
        ERROR_STRUCT_FORMAT(&debugLogInfo, function, line, "INVALID DIM", ERR_INVALID_ARGUMENT);
        printErrorIntoConsole(&debugLogInfo);

        return ERR_INVALID_ARGUMENT;
    };

    (*count)++;

    if (node->parent != parent){
        error_debug debugLogInfo = {};
        ERROR_STRUCT_FORMAT(&debugLogInfo, function, line, "NULL node", ERR_INVALID_ARGUMENT);
        printErrorIntoConsole(&debugLogInfo);

        return ERR_INVALID_ARGUMENT;
    };

    if (verifyNode(node->left, node, binaryTree, count, function, line) != ERR_OK){
        error_debug debugLogInfo = {};
        ERROR_STRUCT_FORMAT(&debugLogInfo, function, line, "Invalid node", ERR_INVALID_ARGUMENT);
        printErrorIntoConsole(&debugLogInfo);

        return ERR_INVALID_ARGUMENT;
    }

    if (verifyNode(node->right, node, binaryTree, count, function, line) != ERR_OK){
        error_debug debugLogInfo = {};
        ERROR_STRUCT_FORMAT(&debugLogInfo, function, line, "Invalid node", ERR_INVALID_ARGUMENT);
        printErrorIntoConsole(&debugLogInfo);

        return ERR_INVALID_ARGUMENT;
    }

    return ERR_OK;
}
