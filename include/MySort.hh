#ifndef MySort_HH
#define MySort_HH

#include <cmath>
#include <vector>
#include "Random.hh"

using std::vector;

////////////////
// MySort class
////////////////
// class to sort vector<double> objects in different ways
class MySort {
  
public:
  // constructor for class instance (has optional random seed argument, with default value)
  MySort(unsigned long long seed = 5555);

  // destructor for our class (no internal members that need to be deleted)
  virtual ~MySort();

  // sorts array using bubble sort
  void BubbleSort(vector<double>& array);
  
  // sorts array using insertion sort
  void InsertionSort(vector<double>& array);

  // sorts array using quicksort
  void QuickSort(vector<double>& array);

  // sorts array C++ stdlib sort
  void DefaultSort(vector<double>& array);

private:
  // private member variables stored inside class instance (has a random number generator built in)
  Random* m_random;

  void SWAP(int index0, int index1, vector<double>& array);

};

#endif

