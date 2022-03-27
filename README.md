# Housing Project

This was completed as a project for the CS11 class this semester. Given a data file of houses and their information (included as housing.bigtestdata), this program creates an array of House structs organizing these houses into rows and columns by their lot code. Columns are labeled by letter and rows are assigned by number, beginning with 1. For example, the house with lot code B2 is in the second column and second row in the array. 

Through a query loop, the user then has the ability to run various commands utilizing the housing array. 

### **1. House Availability ("a")**
  * User input: `a HOUSE_ID`
  * This command determines whether a house with that given ID exists in the array and if it does, checks whether it is available. 
  * Output: `House HOUSE_ID is available`  or  `Sorry, House HOUSE_ID is not available`

### **2. Find matching houses ("m")**
  * User input: `m PRICE COLOR BEDROOMS`
  * This command searches the housing array and prints all matching housing equal to or less than the price given, that match the given color, and have at least the amount of given bedrooms.
  * Output: 
  
  ```
  Matching Houses:
  Id: id# Lot: lot# Color: color Price: price Bedrooms: bedroom# Available: availability
  ```

### **3. Neighbors ("n")**
  * User input: `n HOUSE_ID`
  * This command will determine how many neighbors the house with the given ID has. Houses that are booked and directly adjacent to the given house are       counted, so that is the number of rented houses whose lots touch the lot of the given house (vertical, horizontal, OR diagonal). If a house does not       exist, there are no neighbors there. 
  * Output: `You have no neighbors, practice your drums!`  or  `You have #neighbors neighbors!`

### **4. Rent House ("r")**
  * User input: `r HOUSE_ID`
  * This command will inform the user if the house they wish to rent is booked or does not exist. If it does exist and can be rented, the user is             informed and the query loop will terminate. 
  * Output:
      * If the house is unavailable: `Too late...I hear the dumpster behind hill has some spaces open`
      * If the house does not exist: `You can't rent a house that doesn't exist, but good try`
      * If the house can be rented: `Congrats, you rented a house! Hope your door knobs don't fall off`

The user can also terminate the query loop at anytime by typing 'q'.

The structure of this program involves four main key functions that execute these four main commands, with a helper function for each that prints the appropriate messages depending on what those key functions return. The void function that maintains the query loop calls upon these helper functions to execute the main commands. 
