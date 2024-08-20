#include "TextEditor.h"
#include <ncurses.h>
#include <csignal>


TextEditor::TextEditor() {
    cursorIcon = '|';
    insertState = true;
    cursor = { 0, 0 };
    selection = { { 0, 0 }, { 0, 0 } };
    lines.push_back("");
    isHighlighted = false;
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
    for (int i = 0; i < lines.size(); i++) {
        //got highlight to semi work but need to make it to where it only works when SHIFT L || R Arrow Key is being pressed
        //and also only highlight the character the cursor passes
        if (i == cursor.row && isHighlighted){
            mvprintw(i, 0, "%s", lines[i].substr(0, cursor.col).c_str());
            addch(cursorIcon);
            attron(A_REVERSE);
            printw("%s", lines[i].substr(cursor.col).c_str());
            attroff(A_REVERSE);
        }
        else if (i == cursor.row){
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
                    clear();
                    newLine();
                    break;
                case KEY_SLEFT:
                    if(cursor.col > 0){
                        moveCursor(cursor.row, --cursor.col);
                        isHighlighted = true;
                    }
                    break;
                case KEY_SRIGHT:
                    isHighlighted = false;
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
