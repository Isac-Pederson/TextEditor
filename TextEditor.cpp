#include "TextEditor.h"
#include <ncurses.h>
#include <csignal>


TextEditor::TextEditor() {
    cursorIcon = '|';
    insertState = true;
    cursor = { 0, 0 };
    selection = { { 0, 0 }, { 0, 0 } };
    lines.push_back("");
    isLeftHighlighted = false;
    isRightHighlighted = false;
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
    int const right = 0;
    int const left = 1;
    for (int i = 0; i < lines.size(); i++) {
        //got highlight to semi work but need to make it to where it only works when SHIFT L || R Arrow Key is being pressed
        //and also only highlight the character the cursor passes
        if (i == cursor.row && isLeftHighlighted){
            highlightText(i, left);
        }
        else if (i == cursor.row && isRightHighlighted){
            highlightText(i, right);
        }
        else if (i == cursor.row) {
            mvprintw(i, 0, "%s", lines[i].substr(0, cursor.col).c_str());
            addch(cursorIcon);
            printw("%s",lines[i].substr(cursor.col).c_str());
        }
         else {
            mvprintw(i, 0, "%s", lines[i].c_str());
        }
    }
}

//store each character in array and remove the values when hitting delete. Change to empty when isHighlighted = false 

void TextEditor::highlightText(int i, int direction){
    int const right = 0;
    int const left = 1;
    switch (direction) {
    case right:
    // work on this shit
    
        // attron(A_REVERSE);
        // mvprintw(i, 0, "%s", lines[i].substr(lines[i][--cursor.col], cursor.col).c_str());
        // attroff(A_REVERSE);
        // addch(cursorIcon);
        // printw("%s", lines[i].substr(cursor.col).c_str());
        // break;
    case left:
        mvprintw(i, 0, "%s", lines[i].substr(0, cursor.col).c_str());
        addch(cursorIcon);
        attron(A_REVERSE);
        printw("%s", lines[i].substr(cursor.col, 1).c_str());
        attroff(A_REVERSE);
        break;
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
            printw("\nExiting...");
            break;
        }
        char temp[2];
            if(insertState){
                switch (input){
            case KEY_BACKSPACE:
                deleteCharacter();
                clear();
                break;
                case 24:
                    toggleCursorState();
                    break;
                case KEY_LEFT:
                    if(cursor.col > 0){
                        isLeftHighlighted = false;
                        isRightHighlighted = false;
                        moveCursor(cursor.row, cursor.col - 1);
                    }
                    break;
                case KEY_RIGHT:
                    isLeftHighlighted = false;
                    isRightHighlighted = false;
                    moveCursor(cursor.row, cursor.col + 1);
                    break;
                case KEY_DOWN:
                    isLeftHighlighted = false;
                    isRightHighlighted = false;
                    moveCursor(cursor.row+1, cursor.col);
                    break;
                case KEY_UP:
                    isLeftHighlighted = false;
                    isRightHighlighted = false;
                    moveCursor(cursor.row-1, cursor.col);
                    break;
                case KEY_ENTER:
                case 10:
                case 13:
                    clear();
                    newLine();
                    break;
                case KEY_SLEFT:
                    if(cursor.col > 0){
                        moveCursor(cursor.row, --cursor.col);
                        isLeftHighlighted = true;
                        isRightHighlighted = false;
                    }
                    break;
                case KEY_SRIGHT:
                    moveCursor(cursor.row, ++cursor.col);
                    isRightHighlighted = true;
                    isLeftHighlighted = false;
                    break;
                default:
                    insertCharacter(input);
                    break;
            }}
            else if (!insertState){
                switch(input){
                case 'h':
                    if(cursor.col > 0){
                    isLeftHighlighted = false;
                    isRightHighlighted = false;
                    moveCursor(cursor.row, cursor.col - 1);
                    clear();
                    }
                    break;
                case 'l':
                    isLeftHighlighted = false;
                    isRightHighlighted = false;
                    moveCursor(cursor.row, cursor.col + 1);
                    clear();
                    break;
                case 'j':
                    isLeftHighlighted = false;
                    isRightHighlighted = false;
                        moveCursor(cursor.row + 1, cursor.col);
                    clear();
                    break;
                case 'k':
                    isLeftHighlighted = false;
                    isRightHighlighted = false;
                        moveCursor(cursor.row - 1, cursor.col);
                    clear();
                    break;
                case 'i':
                    isLeftHighlighted = false;
                    isRightHighlighted = false;
                    toggleCursorState();
                    clear();
                    break;
                case 'x':
                    isLeftHighlighted = false;
                    isRightHighlighted = false;
                    deleteCharacter();
                    clear();
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
            // clear();
            refresh();
            display();
    }

        endwin();
}


//under CASE SHIFTLEFTARROW:
                    // attroff(A_REVERSE);
                    // refresh();

                    // for each character pressed from start position to cursor should be highlighted
                        // if (lines[cursor.row][cursor.col] == ++cursor.col){
                            // attron(A_REVERSE);
                        // }
                    
                    // temp[0] = { lines[cursor.row][cursor.col] };
                    // temp[1] = '\0';
                    // mvprintw(cursor.row, cursor.col +1, lines[cursor.row].substr(0, cursor.col).c_str());
                    // attroff(A_REVERSE);
                    // refresh();
                    
                    //ATTEMPT 2:
                        // for (int i = 0; i < lines.size(); i++) { // to view cursor
                        //     if (i == cursor.row){
                        //     mvprintw(i, 0, "%s", lines[i].substr(0, cursor.col).c_str());
                        //     addch(cursorIcon);
                        //     attron(A_REVERSE);
                        //     printw("%s", lines[i].substr(cursor.col).c_str());
                        //     attroff(A_REVERSE);
                        //     } else {
                        //         mvprintw(i, 0, "%s", lines[i].c_str());
                        //     }
                        // }
                        // refresh();
