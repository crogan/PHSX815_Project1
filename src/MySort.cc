#include "MySort.hh"

////////////////
// MySort class
////////////////
// class to sort vector<double> objects in different ways

// constructor for class instance (has optional random seed argument, with default value)
MySort::MySort(unsigned long long seed){
  m_random = new Random(seed);
}

// destructor for our class (need to delete m_random to clean up memory)
MySort::~MySort(){
  if(m_random)
    delete m_random;
}

void MySort::SWAP(int index0, int index1, vector<double>& array){
  double temp = array[index0];
  array[index0] = array[index1];
  array[index1] = temp;
}

// sorts array using bubble sort
void MySort::BubbleSort(vector<double>& array){
  int N = array.size();
  
  for(int i = 0; i < N; i++){
    // Create a flag that will allow the function to
    // terminate early if there's nothing left to sort
    bool already_sorted = true;
    
    // Start looking at each item of the list one by one,
    // comparing it with its adjacent value. With each
    // iteration, the portion of the array that you look at
    // shrinks because the remaining items have already been
    // sorted.
    for(int j = 0; j < N-i-1; j++){
      if(array[j] > array[j + 1]){
	// If the item you're looking at is greater than its
	// adjacent value, then swap them
	SWAP(j, j+1, array);


	// Since you had to swap two elements,
	// set the `already_sorted` flag to `False` so the
	// algorithm doesn't finish prematurely
	already_sorted = false;
      }
    }

    // If there were no swaps during the last iteration,
    // the array is already sorted, and you can terminate
    if(already_sorted)
      break;
  }
}
  
// sorts array using insertion sort
void MySort::InsertionSort(vector<double>& array){
  int N = array.size();
  
  // Loop from the second element of the array until
  // the last element
  for(int i = 1; i < N; i++){
    // This is the element we want to position in its
    // correct place
    double key_item = array[i];

    // Initialize the variable that will be used to
    // find the correct position of the element referenced
    // by `key_item`
    int j = i - 1;

    // Run through the list of items (the left
    // portion of the array) and find the correct position
    // of the element referenced by `key_item`. Do this only
    // if `key_item` is smaller than its adjacent values.
    while(j >= 0 && array[j] > key_item){
      // Shift the value one position to the left
      // and reposition j to point to the next element
      // (from right to left)
      array[j + 1] = array[j];
      j -= 1;
    }

    // When you finish shifting the elements, you can position
    // `key_item` in its correct location
    array[j + 1] = key_item;
  }
}

// sorts array using quicksort
void MySort::QuickSort(vector<double>& array){
  int N = array.size();
  
  // If the input array contains fewer than two elements,
  // then return it as the result of the function
  if(N < 2)
    return;
  
  vector<double> low;
  vector<double> same;
  vector<double> high;
 
  // Select your `pivot` element randomly
  double pivot = array[int(m_random->rand()*N)];

  for(int i = 0; i < N; i++){
    // Elements that are smaller than the `pivot` go to
    // the `low` list. Elements that are larger than
    // `pivot` go to the `high` list. Elements that are
    // equal to `pivot` go to the `same` list.
    if(array[i] < pivot){
      low.push_back(array[i]);
    } else if(array[i] == pivot){
      same.push_back(array[i]);
    } else {
      high.push_back(array[i]);
    }
  }

  // The final result combines the sorted `low` list
  // with the `same` list and the sorted `high` list
  QuickSort(low);
  QuickSort(high);
  // recreate sorted array from pieces
  array.clear();
  for(auto val : low)
    array.push_back(val);
  for(auto val : same)
    array.push_back(val);
  for(auto val : high)
    array.push_back(val);
}

// sorts array C++ stdlib sort
void MySort::DefaultSort(vector<double>& array){
  sort(array.begin(),array.end());
}
