const char* TEX_SETTINGS = "\\documentclass[12pt,a4paper]{extreport}\n"
                            "\\usepackage[T2A]{fontenc}\n"
                            "\\usepackage[utf8]{inputenc}\n"
                            "\\usepackage[russian]{babel}\n"
                            "\\usepackage{amsmath}\n"
                            "\\usepackage{amssymb}\n"
                            "\\usepackage{geometry}\n"
                            "\\usepackage{setspace}\n"
                            "\\geometry{\n"
                            "left=30mm,\n"
                            "right=15mm,\n"
                            "top=20mm,\n"
                            "bottom=20mm\n}\n"
                            "\\onehalfspacing\n"
                            "\\title{\\textbf{Дифференциатор}}\n"
                            "\\author{Екатерина Краева}\n"
                            "\\date{\\today}\n\n"
                            "\\newenvironment{task}[1]\n{\n"
                            "\\par\\noindent\n"
                            "\\textbf{Задача}\n"
                            "\\par\\medskip\n}\n{\n"
                            "\\par\\bigskip\n}\n"
                            "\\newenvironment{solution}\n{\n"
                            "\\par\\noindent\n"
                            "\\textbf{Решение}\n"
                            "\\par\\medskip}\n{\n"
                            "\\par\\bigskip\n}\n\n";

ErrorCode formTexReportFile(files* usedFiles, BinaryTree_t* binaryTree, BinaryTree_t* diffTree){

    assert(usedFiles);
    assert(binaryTree);
    assert(diffTree);

    FILE* diffFileReport = fopen(usedFiles->resultFile, "w");
    if (diffFileReport == NULL) {
        error_debug debugLogInfo = {};
        ERROR_STRUCT_FORMAT(&debugLogInfo, __func__, __LINE__, "File-problem", ERR_FILE_NOT_FOUND);
        printErrorIntoConsole(&debugLogInfo);
        printf("Cannot open file: %s\n", usedFiles->resultFile);

        return ERR_FILE_NOT_FOUND;
    }

    fprintf(diffFileReport, "%s", TEX_SETTINGS);

    fprintf(diffFileReport, "\\begin{document}\n\n");

    fprintf(diffFileReport, "\\begin{task}{1}\n");

    ErrorCode error = writeBinaryTree(binaryTree, diffFileReport, false);
    if(error != ERR_OK) return error;

    fprintf(diffFileReport, "\\end{task}\n\n");

    fprintf(diffFileReport, "\\begin{solution}\n");

    diffTree->root = Diff(diffTree, binaryTree->root, diffFileReport);

    simplifyDiffTree(diffTree, diffTree->root, diffFileReport);

    fprintf(diffFileReport, "Итоговый ответ: \\\\ \n");

    error = writeBinaryTree(diffTree, diffFileReport, true);
    if(error != ERR_OK){
        destroyBinaryTree(diffTree);
        destroyBinaryTree(binaryTree);
        return error;
    }

    fprintf(diffFileReport, "\\end{solution}\n\n");

    fprintf(diffFileReport, "\\end{document}");

    FCLOSE(diffFileReport);

    return ERR_OK;
}
