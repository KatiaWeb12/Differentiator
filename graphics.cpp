static ErrorCode formPicture(BinaryTree_t* binaryTree);
static void recordGraphicBlock(Node* node, FILE* file);
static const char* getNodeColor(const NodeType type);
static const char* getOpNodeValueForPicture(const double value);
static const char* getNodeForPicture(const NodeType type);

const char* GRAPHIC_SETTINGS = "digraph BinaryTree {\n"
        "graph ["
        "bgcolor=\"#F0F7FF\", "
        "rankdir=TB, "
        "splines=polyline, "
        "nodesep=0.35, "
        "ranksep=0.65, "
        "pad=0.3, "
        "];\n"

        "node ["
        "shape=Mrecord, "
        "style=filled, "
        "color=\"#481569\", "
        "fontcolor=\"#60062c\", "
        "fontname=\"Arial\", "
        "fontsize=16, "
        "margin=\"0.18,0.10\""
        "];\n";


ErrorCode createDebugImage(BinaryTree_t* binaryTree, const char* fileName){

    assert(binaryTree);

    ErrorCode error = formPicture(binaryTree);
    if (error != ERR_OK) return error;

    char command[MAX_PROGRAM_CAPACITY] = "";

    sprintf(command, "dot graph.txt -Tpng -o \"%s\"", fileName);

    system(command);

    deletingResultFile("graph.txt");

    return ERR_OK;
}

static ErrorCode formPicture(BinaryTree_t* binaryTree){

    assert(binaryTree);

    const char fileName[] = "graph.txt";

    FILE* graphicCodeFile = fopen(fileName, "w");
    if (graphicCodeFile == NULL) {
        error_debug debugLogInfo = {};
        ERROR_STRUCT_FORMAT(&debugLogInfo, __func__, __LINE__, "File-problem", ERR_FILE_NOT_FOUND);
        printErrorIntoConsole(&debugLogInfo);
        printf("Cannot open graphic code file: %s\n", fileName);

        return ERR_FILE_NOT_FOUND;
    }

    fprintf(graphicCodeFile, "%s", GRAPHIC_SETTINGS);

    recordGraphicBlock(binaryTree->root, graphicCodeFile);

    fprintf(graphicCodeFile, "\n}\n");

    FCLOSE(graphicCodeFile);

    return ERR_OK;
}

static const char* getNodeColor(const NodeType type){

    switch(type){

        case MY_CONST: return "#feb7b9";
        case VAR: return "#d583f5";
        case OP: return "#8beae0";
        default: return NULL;
    }

    return NULL;
}

static const char* getOpNodeValueForPicture(const double value){

    switch((int)value){

        case ADD: return "+";
        case SUB: return "-";
        case MUL: return "*";
        case DIV: return "\\";
        default: return NULL;
    }

    return NULL;
}

static const char* getNodeForPicture(const NodeType type){

    switch(type){

        case MY_CONST: return "CONST";
        case VAR: return "VAR";
        case OP: return "OP";
        default: return NULL;
    }

    return NULL;
}

static void recordGraphicBlock(Node* node, FILE* file){

    assert(file);

    if (node == NULL) return;

    const char* color = getNodeColor(node->type);
    assert(color);

    const char* typeValue = getNodeForPicture(node->type);
    assert(typeValue);

    if(node->type == OP){
        fprintf(file,
            "node%p [fillcolor=\"%s\", label = \"{type = %s|value = %s}\"];\n",
            (const void*)node, color, typeValue, getOpNodeValueForPicture(node->value));
    }
    else{
        fprintf(file,
            "node%p [fillcolor=\"%s\", label = \"{type = %s|value = %g}\"];\n",
            (const void*)node, color, typeValue, node->value);
    }

    if(node->left != NULL){
        fprintf(file,
            "\tnode%p -> node%p "
            "[color=\"#1947bd\"];\n",
            (const void*)node,
            (const void*)node->left);
    }

    if(node->right != NULL){
        fprintf(file,
            "\tnode%p -> node%p "
            "[color=\"#1947bd\"];\n",
            (const void*)node,
            (const void*)node->right);
    }

    recordGraphicBlock(node->left, file);
    recordGraphicBlock(node->right, file);

}
