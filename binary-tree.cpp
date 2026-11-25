static ErrorCode createTreeNode(BinaryTree_t* binaryTree,
                                Node** currentNode,
                                NodeType type,
                                double value,
                                Node** newNode);
static ErrorCode allocateMemoryForNode(Node** treeNode);
static ErrorCode formBinaryTree(BinaryTree_t* binaryTree, char* treeText);
static void destroyTreeNode(Node* node);
static ErrorCode formAfterOpenedBracket(BinaryTree_t* binaryTree, char** current, Node** currentNode);
static ErrorCode verifyNode(const Node* node,
                       const Node* parent,
                       const BinaryTree_t* binaryTree,
                       size_t* count,
                       const char* function,
                       int line);
static const char* getTexInstruction(const double value);
static bool isOpUnary(const double value);
static bool operationRecognition(const char* start, NodeType* type, double* value);


void initBinaryTree(BinaryTree_t* binaryTree){

    assert(binaryTree);

    binaryTree->root = NULL;
    binaryTree->dim = 0;
}

ErrorCode readBinaryTree(const char* fileName, BinaryTree_t* binaryTree){

    assert(fileName);
    assert(binaryTree);

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
    else if(!operationRecognition(start, &type, &value)){

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

    if (delimiter != '\0'){
        (*current)++;
    }

    return ERR_OK;

}

static bool operationRecognition(const char* start, NodeType* type, double* value){

    if (strcmp(start, "sin") == 0){

        *type = OP;
        *value = SIN;

        return true;
    }
    else if (strcmp(start, "cos") == 0){

        *type = OP;
        *value = COS;

        return true;
    }
    else if (strcmp(start, "tg") == 0){

        *type = OP;
        *value = TG;

        return true;
    }
    else if (strcmp(start, "ctg") == 0){

        *type = OP;
        *value = CTG;

        return true;
    }
    else if (strcmp(start, "ln") == 0){

        *type = OP;
        *value = LN;

        return true;
    }
    else if (strcmp(start, "exp") == 0){

        *type = OP;
        *value = EXP;

        return true;
    }
    else if(strcmp(start, "^") == 0){

        *type = OP;
        *value = POW;

        return true;
    }
    else if(strcmp(start, "log") == 0){

        *type = OP;
        *value = LOG;

        return true;
    }
    else if(strcmp(start, "arcsin") == 0){

        *type = OP;
        *value = ARCSIN;

        return true;
    }
    else if(strcmp(start, "arccos") == 0){

        *type = OP;
        *value = ARCCOS;

        return true;
    }
    else if(strcmp(start, "arctg") == 0){

        *type = OP;
        *value = ARCTG;

        return true;
    }
    else if(strcmp(start, "arcctg") == 0){

        *type = OP;
        *value = ARCCTG;

        return true;
    }

    return false;
}

static bool isOpUnary(const double value){

    switch((int)value){

        case SIN:
        case COS:
        case TG:
        case CTG:
        case LN:
        case EXP:

        case ARCSIN:
        case ARCCOS:
        case ARCTG:
        case ARCCTG:

            return true;

        default:
            return false;
    }
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

        Node* parent = *currentNode;
        bool isUnary = parent->type == OP && isOpUnary(parent->value);

        if(isUnary && parent->left != NULL){

            error_debug debugLogInfo = {};
            ERROR_STRUCT_FORMAT(&debugLogInfo, __func__, __LINE__,
                                "Not NULL Left node in unary-op-node",
                                ERR_INVALID_ARGUMENT);
            printErrorIntoConsole(&debugLogInfo);
            return ERR_INVALID_ARGUMENT;
        }
        else if(parent->left == NULL){

            parent->left = *newNode;
        }

        else if(parent->right == NULL && !isUnary){

            parent->right = *newNode;
        }
        else{
            error_debug debugLogInfo = {};
            ERROR_STRUCT_FORMAT(&debugLogInfo, __func__, __LINE__,
                                "Incorrect node-type",
                                ERR_INVALID_ARGUMENT);
            printErrorIntoConsole(&debugLogInfo);
            return ERR_INVALID_ARGUMENT;
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

ErrorCode writeBinaryTree(BinaryTree_t* binaryTree, FILE* file, const bool isAnswer){

    assert(binaryTree);
    assert(file);

    VERIFY_OK(binaryTree, __func__);

    fprintf(file, "\\[\n");

    if(isAnswer) fprintf(file, "\\boxed{\n");

    ErrorCode error  = writeTreeNode(file, binaryTree->root, 0, 0, false);
    if(error != ERR_OK) return error;

    if(isAnswer) fprintf(file, "\n}\n");

    fprintf(file, "\\]\n");

    return ERR_OK;
}

ErrorCode writeTreeNode(FILE* file, const Node* node, int parentPrecedence, int parentOperation, bool isRightChild){

    assert(file);
    assert(node);

    if(node->type == MY_CONST){

        fprintf(file, "%g", node->value);
        return ERR_OK;
    }

    if(node->type == VAR){

        size_t index = (size_t)node->value;

        if(index >= variables.variablesCount){
            error_debug debugLogInfo = {};
            ERROR_STRUCT_FORMAT(&debugLogInfo, __func__, __LINE__,
                                "Incorrect node-type",
                                ERR_INVALID_ARGUMENT);
            printErrorIntoConsole(&debugLogInfo);
            return ERR_INVALID_ARGUMENT;
        }

        fprintf(file, "%s", variables.varArr[index]);
        return ERR_OK;
    }

    if(node->type != OP){
        error_debug debugLogInfo = {};
        ERROR_STRUCT_FORMAT(&debugLogInfo, __func__, __LINE__,
                            "Incorrect node-type",
                            ERR_INVALID_ARGUMENT);
        printErrorIntoConsole(&debugLogInfo);
        return ERR_INVALID_ARGUMENT;
    }

    const char* functionName = getTexInstruction(node->value);

    if(functionName == NULL){
        error_debug debugLogInfo = {};
        ERROR_STRUCT_FORMAT(&debugLogInfo, __func__, __LINE__,
                            "Incorrect node-type",
                            ERR_INVALID_ARGUMENT);
        printErrorIntoConsole(&debugLogInfo);
        return ERR_INVALID_ARGUMENT;
    }

    if(isOpUnary(node->value)){

        if(node->left == NULL || node->right != NULL){
            error_debug debugLogInfo = {};
            ERROR_STRUCT_FORMAT(&debugLogInfo, __func__, __LINE__,
                                "Incorrect node-type",
                                ERR_INVALID_ARGUMENT);
            printErrorIntoConsole(&debugLogInfo);
            return ERR_INVALID_ARGUMENT;
        }

        fprintf(file, "%s(", functionName);

        ErrorCode error = writeTreeNode(file, node->left, 0, 0, false);
        if(error != ERR_OK) return error;

        fprintf(file, ")");

        return ERR_OK;
    }

    if(node->left == NULL || node->right == NULL){
        error_debug debugLogInfo = {};
        ERROR_STRUCT_FORMAT(&debugLogInfo, __func__, __LINE__,
                            "Incorrect node-type",
                            ERR_INVALID_ARGUMENT);
        printErrorIntoConsole(&debugLogInfo);
        return ERR_INVALID_ARGUMENT;
    }

    int precedence = getOperationPrecedence(node);

    bool needParentheses = precedence < parentPrecedence;
    int operation = (int)node->value;

    if(isRightChild && precedence == parentPrecedence){

        // a - (b + c)
        // a - (b - c)
        if(parentOperation == SUB && (operation == ADD || operation == SUB)){

            needParentheses = true;
        }

        // a * (b / c)
        if(parentOperation == MUL && operation == DIV){

            needParentheses = true;
        }

        // a / (b * c)
        // a / (b / c)
        if(parentOperation == DIV && (operation == MUL || operation == DIV)){

            needParentheses = true;
        }
    }

    if(needParentheses) fprintf(file, "(");

    if((int)node->value == DIV){

        fprintf(file, "\\frac{");

        ErrorCode error = writeTreeNode(file, node->left, 0, 0, false);
        if(error != ERR_OK) return error;

        fprintf(file, "}{");

        error = writeTreeNode(file, node->right, 0, 0, false);
        if(error != ERR_OK) return error;

        fprintf(file, "}");
    }

    else if((int)node->value == MUL){

        // -1 * x = -x

        if(node->left->type == MY_CONST && (int)node->left->value == -1){

            fprintf(file, "-");

            ErrorCode error = writeTreeNode(file, node->right, precedence, MUL, true);
            if(error != ERR_OK) return error;
        }
        else if(node->right->type == MY_CONST && (int)node->right->value == -1){

            fprintf(file, "-");

            ErrorCode error = writeTreeNode(file, node->left, precedence, MUL, false);
            if(error != ERR_OK) return error;
        }
        else{

            ErrorCode error = writeTreeNode(file, node->left, precedence, MUL, false);
            if(error != ERR_OK) return error;

            fprintf(file, " \\cdot ");

            error = writeTreeNode(file, node->right, precedence, MUL, true);
            if(error != ERR_OK) return error;
        }
    }

    else if((int)node->value == POW){

        if(node->left == NULL || node->right == NULL){
            error_debug debugLogInfo = {};
            ERROR_STRUCT_FORMAT(&debugLogInfo, __func__, __LINE__,
                                "Incorrect node-type",
                                ERR_INVALID_ARGUMENT);
            printErrorIntoConsole(&debugLogInfo);
            return ERR_INVALID_ARGUMENT;
        }

        bool needPowerParentheses = node->left->type == OP;

        fprintf(file, "{");

        if(needPowerParentheses) fprintf(file, "(");

        ErrorCode error = writeTreeNode(file, node->left, 0, 0, false);
        if(error != ERR_OK) return error;

        if(needPowerParentheses) fprintf(file, ")");

        fprintf(file, "}^{");

        error = writeTreeNode(file, node->right, 0, 0, false);
        if(error != ERR_OK) return error;

        fprintf(file, "}");

        return ERR_OK;
    }

    else if((int)node->value == LOG){

        if(node->left == NULL || node->right == NULL){
            error_debug debugLogInfo = {};
            ERROR_STRUCT_FORMAT(&debugLogInfo, __func__, __LINE__,
                                "Incorrect node-type",
                                ERR_INVALID_ARGUMENT);
            printErrorIntoConsole(&debugLogInfo);
            return ERR_INVALID_ARGUMENT;
        }

        fprintf(file, "\\log_{");

        ErrorCode error = writeTreeNode(file, node->left, 0, 0, false);
        if(error != ERR_OK) return error;

        fprintf(file, "}(");

        error = writeTreeNode(file, node->right, 0, 0, false);
        if(error != ERR_OK) return error;

        fprintf(file, ")");

        return ERR_OK;
    }
    else{

        ErrorCode error = writeTreeNode(file, node->left, precedence, (int)node->value, false);
        if(error != ERR_OK) return error;

        fprintf(file, " %s ", functionName);

        error = writeTreeNode(file, node->right, precedence, (int)node->value, true);
        if(error != ERR_OK) return error;
    }

    if(needParentheses) fprintf(file, ")");

    return ERR_OK;
}

static const char* getTexInstruction(const double value){

    switch((int)value){

        case ADD: return "+";
        case SUB: return "-";
        case MUL: return "\\cdot";
        case DIV: return "\\over";
        case SIN: return "\\sin";
        case COS: return "\\cos";
        case TG:  return "\\tan";
        case CTG: return "\\cot";
        case LN:  return "\\ln";
        case EXP: return "\\exp";
        case POW: return "^";
        case LOG: return "\\log";

        case ARCSIN: return "\\arcsin";
        case ARCCOS: return "\\arccos";
        case ARCTG:  return "\\arctan";
        case ARCCTG: return "\\operatorname{arcctg}";
        default: return NULL;
    }

    return NULL;
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
