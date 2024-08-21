#ifndef TEXTEDITOR_H
#define TEXTEDITOR_H

#include <iostream>
#include <vector>
#include <string>
#include <stack>

struct Cursor
{
    int row;
    int col;
};

struct Selection{
    Cursor start;
    Cursor end;
};

class TextEditor{
    private:
        std::vector<std::string> lines;
        Cursor cursor;
        Selection selection;
        std::stack<std::vector<std::string>> undoStack;
        std::stack<std::vector<std::string>> redoStack;
        bool insertState;
        bool isLeftHighlighted;
        bool isRightHighlighted;
        char cursorIcon;
        void saveStateForUndo();


    public:
        TextEditor();
        void highlightText(int i, int direction1);
        void newLine();
        void moveCursor(int row, int col);
        void insertCharacter(char c);
        void deleteCharacter();
        void selectText(int startRow, int startCol, int endRow, int endCol);
        void undo();
        void redo();
        void display();
        void run();
        void toggleCursorState();
};
#endif
