#include "TextEditor.h"
#include <ncurses.h>
#include <csignal>


TextEditor::TextEditor() {
    cursorIcon = '|';
    insertState = true;
    cursor = { 0, 0 };
    selection = { { 0, 0 }, { 0, 0 } };
    lines.push_back("");
}

void TextEditor::moveCursor(int row, int col)
{
// can't be negative and must be less than the files line count
   if (row >= 0 && row < lines.size()){
    
       cursor.row = row; //set cursor row to the moved row since it was valid
       cursor.col = std::min(col, static_cast<int>(lines[row].size())); //set min col. Either inputed or the line length
   } 
}

void TextEditor::insertCharacter(char c){
    lines[cursor.row].insert(cursor.col, 1, c);

    if(cursor.col >= COLS){
    
        cursor.row++;
        cursor.col = 0;

        if (cursor.row >= lines.size()){
            lines.push_back("");
        }
        mvaddch(cursor.row, cursor.col, c);
    }else{
        mvaddch(cursor.row, cursor.col++, c);
    }
}



void TextEditor::display(){
    for (int i = 0; i < lines.size(); i++) { // to view cursor
        if (i == cursor.row){
            mvprintw(i, 0, "%s", lines[i].substr(0, cursor.col).c_str());
            addch(cursorIcon);
            printw("%s",lines[i].substr(cursor.col).c_str());
        }
        else{
            mvprintw(i, 0, "%s", lines[i].c_str());
        }
    }
}


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
    if(cursor.row != 0 && lines[cursor.row].length() == 0){
        cursor.row--;
        moveCursor(cursor.row, lines[cursor.row].length());
    }
    if(lines[cursor.row].length() > 0 && cursor.col > 0){
        lines[cursor.row].erase(--cursor.col, 1);
    }
}

void TextEditor::newLine(){
    cursor.row++;
    lines.insert(lines.begin() + cursor.row, "");
    cursor.col = 0;
    move(cursor.row, cursor.col);
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
                    newLine();
                    break;
                case KEY_SLEFT:
                    newLine();
                    break;
                case KEY_SRIGHT:
                    break;
                default:
                    insertCharacter(input);
                    printf("%d",cursor.row);
                    clear();
                    display();
                    refresh();
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
                        moveCursor(cursor.row + 1, cursor.col);
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
            clear();
            refresh();
            display();
    }

        endwin();
}
