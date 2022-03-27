//===============================================================
// housing.cpp
// 
// Purpose: Given a data file of houses and their information, this program 
// creates an array of House structs organizing these houses into rows and 
// columns by their lot code. Based on commands inputted by the user through 
// a query loop, it can then determine the availability of a house, find 
// any houses that match given conditions for price, color, and number of
// bedrooms, determine the number of neighbors surrounding a house, or rent
// an available house. The program will only end if a user types "q" into the
// query loop or if they are successful in renting a house.
//
// Written by: Vibha Kamath
//===============================================================

#include <iostream>
#include <fstream>

using namespace std;

const int ROWS = 50;
const int COLS = 20;

//===============================================================
//
//                             House
//
//     Each House struct records the information about one
//     house. 
//
//     Note that if the "no_house_here" member field is set
//     to true, then the other fields are not used. 
//
//===============================================================

struct House
{
    bool   no_house_here;  // true if there is NO house on this location
    int    id;             // id from the data file, converted to int
    string lot_code;       // lot_code string from the data file
    float price;           // price of the house
    int bedrooms;          // number of bedrooms
    string color;          // color of the house
    string availability;   // either holds "booked" or "available"
};

/*===============================================================
                   Required Function Prototypes
  ===============================================================*/

// - - - - - - - - - - - - - - - - - - - - - - - - - - -
//   Functions operating on a single house
// - - - - - - - - - - - - - - - - - - - - - - - - - - -
House read_one_house(ifstream& input_file);
void print_house(House h);

// - - - - - - - - - - - - - - - - - - - - - - - - - - -
//   Functions relating to the array of houses
// - - - - - - - - - - - - - - - - - - - - - - - - - - -
int col_from_lot(string lot);
int row_from_lot(string lot);
void fill_with_no_houses(int rows, int cols, House all_houses[ROWS][COLS]);
bool read_all_house_data(string filename, House all_houses[ROWS][COLS]);

// - - - - - - - - - - - - - - - - - - - - - - - - - - -
//   Key Functions/Commands
// - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool is_available(int house_id, House all_houses[ROWS][COLS]);
int matching_houses(float max_price, string desired_color,
                       int desired_bedrooms, House all_houses[ROWS][COLS],
                       House results[ROWS * COLS]); 
int num_neighbors(int house_id, House all_houses[ROWS][COLS]);
int rent_house(int house_id, House all_houses[ROWS][COLS]);

/*===============================================================
                    Additional Helper Functions */
void user_loop(House all_houses[ROWS][COLS], House results[ROWS * COLS]);
void print_is_available(House all_houses[ROWS][COLS]);
void print_matching_houses(House results[ROWS * COLS], 
    House all_houses[ROWS][COLS]);
void print_neighbors(House all_houses[ROWS][COLS]);
int check_surroundings(int rows, int cols, House all_houses[ROWS][COLS]);
bool is_occupied(int rows, int cols, House all_houses[ROWS][COLS]);
int print_rent_house(House all_houses[ROWS][COLS]);
// ===============================================================


//======================================================================
//                              main
//======================================================================

int main(int argc, char *argv[])
{
    if (argc != 2) {
        cout << "ERROR: Input file name not provided." << endl;
        return 1;
    }
    
    string filename = argv[1];
    
    House all_houses[ROWS][COLS];
    House results[ROWS * COLS];
    
    fill_with_no_houses(ROWS, COLS, all_houses);
    
    read_all_house_data(filename, all_houses);
    
    user_loop(all_houses, results);

    return 0;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - -
//   Functions operating on a single house
// - - - - - - - - - - - - - - - - - - - - - - - - - - -

// READ_ONE_HOUSE 
// Purpose: read one line of the input_file; return corresponding House
// Parameter: input file 

House read_one_house(ifstream& input_file)
{
    House h;
    
    h.no_house_here = false;
    input_file >> h.id;
    input_file >> h.lot_code;
    input_file >> h.price;
    input_file >> h.bedrooms;
    input_file >> h.color;
    input_file >> h.availability;

    return h;          
}

// PRINT_HOUSE
// Purpose: print information about a single house
// Parameter: a house struct

void print_house(House h)
{
    if (h.no_house_here) {
        cout << "No house" << endl;
    } else {
        cout << "Id: " << h.id << " " << 
        "Lot: " << h.lot_code << " " <<
        "Color: " << h.color << " " <<
        "Price: " << h.price << " " << 
        "Bedrooms: " << h.bedrooms << " " <<
        "Available: ";
        
        if (h.availability == "available") {
            cout << "Yes" << endl;
        } else {
            cout << "No" << endl;
        }
    } 
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - -
//   Functions relating to the array of houses
// - - - - - - - - - - - - - - - - - - - - - - - - - - -

// COL_FROM LOT and ROW_FROM_LOT
// Purposes: given a lot code, return the column and row (respectively)
// in the all_houses array
// Parameter: the lot code

int col_from_lot(string lot)
{
    int col = lot[0] - 65;
    
    return col;
}

int row_from_lot(string lot)
{
    int digits = 0;
    
    for (size_t i = 1; i < lot.length(); i++) {
            digits = (digits * 10) + (lot[i] - '0');
        }
   
    int row = digits - 1; 
    
    return row;
}

// FILL_WITH_NO_HOUSES
// Purpose: set the no_house_here marker as true for every position
// in the array
// Parameters: number of rows and columns, array of all houses

void fill_with_no_houses(int rows, int cols, House all_houses[ROWS][COLS])
{
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            all_houses[i][j].no_house_here = true;
        }
    }
}

// READ_ALL_HOUSE_DATA 
//  Purpose: given a file with a count n at the top
//  read the count and then n lines of house data
//  into appropriate positions in the array
//  Parameters: name of the input file, array of all houses 
//   return true if file was successfully read
//          false if file could not be opened

bool read_all_house_data(string filename, House all_houses[ROWS][COLS])
{
    ifstream input_file;
    
    input_file.open(filename);

    if (not input_file.is_open()) {
        cerr << "fileread.cpp: could not open file: "
             << filename << endl;
        return false; 
    }
    
    int num_houses;
    input_file >> num_houses;
    
    string line;
    
    for (int i = 0; i < num_houses; i++) {
        (getline(input_file, line));
        House h = read_one_house(input_file);
        string lot = h.lot_code;
        all_houses[row_from_lot(lot)][col_from_lot(lot)] = h;
    }
        return true; 
} 

// USER_LOOP 
// Purpose: prompts the user for a command — either calls the correct handler 
// function to execute that command and loops afterwards OR it returns
// Parameters: all_houses array, results array

void user_loop(House all_houses[ROWS][COLS], House results[ROWS * COLS])
{
    string command;
    cout << "query-> ";
    cin >> command;
    
    if (command == "a") {
        print_is_available(all_houses);
    } else if (command == "m") {
        print_matching_houses(results, all_houses);
    } else if (command == "n") {
        print_neighbors(all_houses);
    } else if (command == "q") {
        return;
    } else if (command == "r") {
        if (print_rent_house(all_houses) == 0) {
            return;
        }
    } else {
        cout << "Unknown command " << command << endl;
    }
    
    user_loop(all_houses, results);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - -
//   Functions for week 2 (ignore during week 1)
// - - - - - - - - - - - - - - - - - - - - - - - - - - -

// FIND_HOUSE_FROM_INT
// Purpose: given a house id, return the house struct in the array associated
// with that id, if any
// Parameter: a house id and the array of houses

House find_house_from_id(int id, House all_houses[ROWS][COLS]) 
{
    House house;
    house.no_house_here = true;
    
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            if (id == all_houses[i][j].id) {
                house = all_houses[i][j];
                return house;
            } 
        }
    }
    return house;
}

// PRINT_IS_AVAILABLE 
// Purpose: given the results of the is_available bool function, print the
// appropriate statement about house availability
// Parameter: array of all houses

void print_is_available(House all_houses[ROWS][COLS]) 
{
    int id;
    cin >> id;
    
    if ((is_available(id, all_houses))) {
        cout << "House " << id << " is available" << endl;
    } else {
        cout << "Sorry, House " << id << " is not available" << endl;
    }
}

// IS_AVAILABLE
// Purpose: Given an id, search the array of all houses and determine if there
// is an existing house with that id and if it is available 
// Parameters: array of all houses and house id
// Returns: true if house exists and is available, false if it doesn't exist 
// or is unavailable

bool is_available(int house_id, House all_houses[ROWS][COLS])
{
    if (find_house_from_id(house_id, all_houses).availability == "available") {
        return true;
    } else {
        return false;
    }
}

// PRINT_MATCHING_HOUSES
// Purpose: print a list of the houses in the results array by row order
// Parameter: array of all houses, results array 

void print_matching_houses(House results[ROWS * COLS], 
    House all_houses[ROWS][COLS]) 
{
    float price;
    string color;
    int bedrooms;
    cin >> price >> color >> bedrooms;
    
    cout << "Matching Houses:" << endl;
    int count = matching_houses(price, color, bedrooms, all_houses, results);
    
    for (int i = 0; i < count; i++) {
        print_house(results[i]);
    }
}

// MATCHING_HOUSES
// Purpose: searches through the array of all houses and adds any house that
// matches the price, color, and bedrooms condition to another results array 
// Parameters: a float max price, a string for desired color, an integer for 
// desired bedrooms, the array of all houses, and the array for results 
// Returns: an integer value for how many total houses match the conditions

int matching_houses(float max_price, string desired_color,
                    int desired_bedrooms, House all_houses[ROWS][COLS],
                    House results[ROWS * COLS])
{
    House house;
    int count = 0;
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            if (all_houses[i][j].price <= max_price and 
                all_houses[i][j].color == desired_color and 
                all_houses[i][j].bedrooms >= desired_bedrooms) {
                    house = all_houses[i][j];
                    results[count] = house;
                    count = count + 1;    
                   }
            }
    }
    return count;
}

// PRINT_NEIGHBORS
// Purpose: given the integer result of the num_neighbors function, print the 
// appropriate statement about how many neighbors exist
// Parameters: the array of all houses 

void print_neighbors(House all_houses[ROWS][COLS])
{
    int id;
    cin >> id;
    
    if (num_neighbors(id, all_houses) == 0) {
        cout << "You have no neighbors, practice your drums!" << endl;
    } else {
        cout << "You have " << num_neighbors(id, all_houses) << " neighbors!" 
        << endl;
    }
}

// NUM_NEIGHBORS
// Purpose: given a house ID, determine its array location and return the 
// amount of neighbors around that house 
// Parameters: the house id and the array of all houses 

int num_neighbors(int house_id, House all_houses[ROWS][COLS])
{
    House h = find_house_from_id(house_id, all_houses);
    int rows = row_from_lot(h.lot_code);
    int cols = col_from_lot(h.lot_code);
    
    int neighbors = check_surroundings(rows, cols, all_houses);
    
    return neighbors;  // REPLACE THIS WITH WHAT YOU REALLY WANT TO RETURN!
}

// CHECK_SURROUNDINGS
// Purpose: Determine the total number of neighbors surrounding a house
// Parameters: the row and column of the house's location and the array of all
// houses
// Returns: an integer value for the number of neighbors

int check_surroundings(int rows, int cols, House all_houses[ROWS][COLS])
{
    int neighbors = 0;
    if (is_occupied(rows, cols + 1, all_houses)) { // to the right 
        neighbors = neighbors + 1;
    }
    if (is_occupied(rows + 1, cols + 1, all_houses)) { // bottom right diag.
        neighbors = neighbors + 1;
    }
    if (is_occupied(rows + 1, cols, all_houses)) { // below
        neighbors = neighbors + 1;
    }
    if (is_occupied(rows + 1, cols - 1, all_houses)) { // bottom left diag.
        neighbors = neighbors + 1;
    }
    if (is_occupied(rows, cols - 1, all_houses)) { // left
        neighbors = neighbors + 1;
    }
    if (is_occupied(rows - 1, cols - 1, all_houses)) { // top left diag
        neighbors = neighbors + 1;
    }
    if (is_occupied(rows - 1, cols, all_houses)) { // above
        neighbors = neighbors + 1;
    }
    if (is_occupied(rows - 1, cols + 1, all_houses)) { // top right diag.
        neighbors = neighbors + 1;
    }
    return neighbors;
}

// IF_NEIGHBORS
// Purpose: determines whether a neighbor exists in a given array location
// Parameters: the row and column number of a house and the array of all houses
// Returns: true if the house is booked, false if it is available or if that
// array location does not exist

bool is_occupied(int rows, int cols, House all_houses[ROWS][COLS])
{
    // account for nonexistent array spaces
    if (rows < 0 or cols < 0 or rows >= ROWS or cols >= COLS ) { 
        return false;
    } else if (all_houses[rows][cols].availability == "booked") {
        return true;
    } else {
        return false;
    }
}

// PRINT RENT_HOUSE
// Purpose: per the results of the rent_house function, print the appropriate
// message as to whether the house can be rented or not 
// Parameters: the array of all houses 
// Returns: 1 if the house cannot be rented and 0 if it can

int print_rent_house(House all_houses[ROWS][COLS])
{
    int id;
    cin >> id;
    
    if (rent_house(id, all_houses) == 1) {
        cout << "Congrats, you rented a house! Hope your door knobs don't" <<
        " fall off" << endl;
        return 0;
    } else if (rent_house(id, all_houses) == 2) {
        cout << "Too late...I hear the dumpster behind hill has some spaces" << 
        " open" << endl;
        return 1;
    } else {
        cout << "You can't rent a house that doesn't exist, but good try" << 
        endl;
        return 1;
    }
}

// RENT_HOUSE
// Purpose: given a house id, search through the array of all houses to 
// determine if it is available to rent, and book it if it is available
// Parameters: a house id and the array of all houses 
// Returns: 1 if the the house is available, 2 if it is unavailable, and 0 if 
// it does not exist 

int rent_house(int house_id, House all_houses[ROWS][COLS])
{
    House h = find_house_from_id(house_id, all_houses);
    int retVal = 0;
    
    if (h.no_house_here == false) {
        if (h.availability == "available") {
            all_houses[row_from_lot(h.lot_code)]
            [col_from_lot(h.lot_code)].availability = "booked";
            retVal = 1;
        } else
            retVal = 2;
    }
    
    return retVal; 
}

// ============================================================
// TESTING CODE
// ============================================================

#include "housing_hooks.hpp"
