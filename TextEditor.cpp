#include "TextEditor.h"
#include <ncurses.h>
#include <csignal>


TextEditor::TextEditor() {
    cursorIcon = '|';
    insertState = true;
    cursor = { 0, 0 };
    selection = { { 0, 0 }, { 0, 0 } };
}



void TextEditor::moveCursor(int row, int col)
{
// can't be negative and must be less than the files line count
   if (row >= 0 && row < lines.size()){
    
       cursor.row = row; //set cursor row to the moved row since it was valid
       cursor.col = std::min(col, static_cast<int>(lines[row].size())); //set min col. Either inputed or the line length
   } 
}

//when toggling what mode, set the cursor variable from | to  ▮

void TextEditor::insertCharacter(char c){
    lines[cursor.row].insert(cursor.col, 1, c);
    mvaddch(cursor.row, cursor.col++, c);
};



void TextEditor::display(){
    system("clear");
    for (int i = 0; i < lines.size(); i++) { // to view cursor
        if (i == cursor.row){
            std::cout << lines[i].substr(0, cursor.col) << cursorIcon << lines[i].substr(cursor.col) << std::endl;
        }
        else{
            std::cout << lines[i] << std::endl;
        }
    }
}

//on enter press, lines.push_back() so you can move cursor up and down
//make line go all the way to the left, idk how I will achieve this



void TextEditor::toggleCursorState(){
    if(insertState){ //go to normal state
        cursorIcon = '_';
        insertState = false;
    } else if (!insertState) { // go to insertState
        cursorIcon = '|';
        insertState = true;
    }
    mvaddch(cursor.row, cursor.col, cursorIcon);
    refresh();
}

void TextEditor::deleteCharacter(){
    if(lines[cursor.row].length() > 0 && cursor.col > 0){
        lines[cursor.row].erase(--cursor.col, 1);
    }
}

void signalHandler(int signal){}

void TextEditor::run(){
    bool currentInstance = this;
    lines.push_back("");
    initscr(); // Initialize ncurses
    cbreak(); // Disable line buffering
    noecho();
    timeout(-1);
    keypad(stdscr, TRUE);
    curs_set(0);
    signal(SIGINT, signalHandler);

    int input;
    while(true){
        display();
        input = getch();
        if(input ==  27){
            printf("\nExiting...");
            break;
        }
            if(insertState){
                switch (input){
            case KEY_BACKSPACE:
                deleteCharacter();
                break;
                case 24:
                    toggleCursorState();
                    break;
                case KEY_LEFT:
                    if(cursor.col > 0){
                        moveCursor(cursor.row, cursor.col - 1);
                    }
                    break;
                case KEY_RIGHT:
                        moveCursor(cursor.row, cursor.col + 1);
                    break;
                case KEY_DOWN:
                        moveCursor(cursor.row+1, cursor.col);
                    break;
                case KEY_UP:
                        moveCursor(cursor.row-1, cursor.col);
                    break;
                case KEY_ENTER:
                case 10:
                case 13:
                    lines.push_back("");
                    cursor = {cursor.row +1 ,0 };
                    lines.push_back("");
                    break;

                default:
                    insertCharacter(input);
                    break;
            }}
            else if (!insertState){
                switch(input){
                case 'h':
                    if(cursor.col > 0){
                    moveCursor(cursor.row, cursor.col - 1);
                    }
                    break;
                case 'l':
                    moveCursor(cursor.row, cursor.col + 1);
                    break;
                case 'j':
                    printw("%d",cursor.row);
                    // if (cursor.row - 1 < lines.size()) {
                        moveCursor(cursor.row + 1, cursor.col);
                    // }
                    break;
                case 'k':
                        moveCursor(cursor.row - 1, cursor.col);
                    break;
                case 'i':
                    toggleCursorState();
                    break;
                case 'x':
                    deleteCharacter();
                    break;
                // case 'u':
                //     undo();
                //     break;
                // case 'r':
                //     redo();
                //     break;
                case 24:
                    toggleCursorState();
                    break;
        }}
        refresh();
    }

        endwin();
}
