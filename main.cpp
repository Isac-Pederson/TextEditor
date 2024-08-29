#include "TextEditor.h"
#include <iostream>
#include <fstream>


int main(int argc, char *argv[]){
    TextEditor editor;

    if(argc > 1){
        editor.loadFile(argv[1]);
    };

    editor.run();

    return 0;
}
