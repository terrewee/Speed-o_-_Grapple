#include "BrickPi3.h" // for BrickPi3
#include <stdio.h>      // for printf
#include <unistd.h>     // for usleep
#include <signal.h>     // for catching exit signals
#include <vector>

using std::vector;
using std::cout;
using std::cin;
using std::endl;

BrickPi3 BP;

struct coordinates{
  int x;
  int y;
};

void exit_signal_handler(int signo);

// vector<vector<bool>> getGrid(){
//     vector<vector<bool>> grid= {{}};
//     int x = 0;
//     int y = 0;
    
//     cout << "Please give the relative x coordinate of the object to be found." < <endl;
//     cin >> x;
//     cout << "Please give the relative y coordinate of the object to be found." < <endl;
//     cin >> y;
    
//     grid = makeGrid(x, y);
    
//     return grid;
// }

vector<vector<bool>> makeGrid(coordinates userInput) {
    //Makes a grid from x and y input.
    vector<vector<bool>> grid = { {} };

    for (int i = 0; i < y + 2; i++){
        vector<bool> tempRow = {};

        for (int j = 0; j < x + 2; j++){
            tempRow.push_back(true);
        }

        grid.push_back(tempRow);
    }

    //Prints the grid to test it, this can be deleted later on.
    // for (size_t k = 0; k < grid.size(); k++) {
    //     for (size_t l = 0; l < grid[k].size(); l++) {
    //         cout << grid[k][l];
    //     }
    //     cout << endl;
    // }

    return grid;
}

vector<vector<bool>> getGrid(){
  vector<vector<bool>> grid= {{}};
  coordinates userInput;
    
  cout << "Please give the relative x coordinate of the object to be found." < <endl;
  cin >> userInput.x;
  cout << "Please give the relative y coordinate of the object to be found." < <endl;
  cin >> userInput.y;
    
  grid = makeGrid(coordinates);
    
  return grid;
}




int main(){
    return 0;
}

// Signal handler that will be called when Ctrl+C is pressed to stop the program
void exit_signal_handler(int signo){
  if(signo == SIGINT){
    BP.reset_all();    // Reset everything so there are no run-away motors
    exit(-2);
  }
}