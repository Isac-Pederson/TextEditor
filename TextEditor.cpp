#include "TextEditor.h"
#include <ncurses.h>


TextEditor::TextEditor() {
    cursorIcon = '|';
    insertState = true;
    cursor = { 0, 0 };
    selection = { { 0, 0 }, { 0, 0 } };
    lines.push_back("");
    isSelecting = false;
    selectionDirection = RIGHT;
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

    if(isSelecting){
        deleteSelection();
    }

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


void TextEditor::deleteSelection(){
    int startCol = std::min(selection.start.col, selection.end.col);
    int endCol = std::max(selection.start.col, selection.end.col);
    lines[cursor.row].erase(startCol,endCol - startCol);
    cursor.col = startCol;
    isSelecting = false;
}

void TextEditor::display(){
    clear();
    for (int i = 0; i < lines.size(); i++) {
        if (i == cursor.row && isSelecting){
            highlightText(i);
        }
        else if (i == cursor.row) {
            mvprintw(i, 0, "%s", lines[i].c_str());
            move(i, cursor.col);
            addch(cursorIcon);
        } else {
            mvprintw(i, 0, "%s", lines[i].c_str());
        }
    }
    refresh();
}

//store each character in array and remove the values when hitting delete. Change to empty when isHighlighted = false 

void TextEditor::highlightText(int row){
    int startCol = std::min(selection.start.col, selection.end.col);
    int endCol = std::max(selection.start.col, selection.end.col);

    startCol = std::max(0, startCol);
    endCol = std::min(static_cast<int>(lines[row].length()), endCol);

    mvprintw(row, 0, "%s", lines[row].substr(0, startCol).c_str());

    attron(A_REVERSE);
    printw("%s", lines[row].substr(startCol, endCol-startCol).c_str());
    attroff(A_REVERSE);

    printw("%s", lines[row].substr(endCol).c_str());

    move(row, cursor.col);
    addch(cursorIcon);
}

void TextEditor::toggleCursorState(){
    insertState = !insertState;
    cursorIcon = insertState ? '|' : '_';
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

void TextEditor::startSelection(Direction dir){
    if(!isSelecting){
        isSelecting = true;
        selectionDirection = dir;
        selection.start = cursor;
        selection.end = cursor;
    }
    updateSelection(dir);
}

void TextEditor::updateSelection(Direction dir){
    if(dir == LEFT && cursor.col > 0){
        cursor.col--;
    }else if(dir == RIGHT && cursor.col < lines[cursor.row].length()){
        cursor.col++;
    }
    selection.end = cursor;
}

void TextEditor::newLine(){
    cursor.row++;
    lines.insert(lines.begin() + cursor.row, "");
    cursor.col = 0;
    move(cursor.row, cursor.col);
}

void TextEditor::moveCursorLeft(){
    if(isSelecting){
        updateSelection(LEFT);
    }else if(cursor.col > 0){
        cursor.col--;
    }
}

void TextEditor::moveCursorRight(){
    if(isSelecting){
        updateSelection(RIGHT);
    }else if(cursor.col < lines[cursor.row].length()){
       cursor.col++;
    }
}

void TextEditor::moveCursorUp(){
    if (cursor.row > 0){
        cursor.row--;
        cursor.col = std::min(cursor.col, static_cast<int>(lines[cursor.row].length()));
    }
}

void TextEditor::moveCursorDown(){
    if (cursor.row < lines.size() -1 ){
        cursor.row++;
        cursor.col = std::min(cursor.col, static_cast<int>(lines[cursor.row].length()));
    }
}




void TextEditor::run(){
    initscr(); // Initialize ncurses
    cbreak(); // Disable line buffering
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);

    int input;
    while(true){
        display();
        input = getch();


        if(input ==  27){
            printw("\nExiting...");
            break;
        }

            if(insertState){
                if(input == KEY_BACKSPACE){
                    if(isSelecting){
                        deleteSelection();
                    }
                    deleteCharacter();
                }else if(input == 24){//toggle CTRL+X
                    toggleCursorState();
                }else if(input == KEY_LEFT){
                    moveCursorLeft();
                }else if(input == KEY_RIGHT){
                    moveCursorRight();
                }else if(input == KEY_DOWN){
                    moveCursorDown();
                }else if(input == KEY_UP){
                    moveCursorUp();
                }else if (input == KEY_ENTER || input == 10 || input == 13){
                    newLine();
                }else if (input == KEY_SLEFT){
                    startSelection(LEFT);
                }else if (input == KEY_SRIGHT){
                    startSelection(RIGHT);
                }else{
                    insertCharacter(static_cast<char>(input));
                }
            }else{
                if(input == 'h'){
                    moveCursorLeft();
                }else if(input == 24){//toggle CTRL+X
                    toggleCursorState();
                }else if(input == 'l'){
                    moveCursorRight();
                }else if(input == 'j'){
                    moveCursorDown();
                }else if(input == 'k'){
                    moveCursorUp();
                }else if(input == 'x'){
                    deleteCharacter();
                }else if(input == 'u'){
                    // redo();
                }else if(input == 'k'){
                    // undo();
            }
            }
    }
            refresh();
            endwin();
}


