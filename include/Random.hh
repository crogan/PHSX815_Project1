#ifndef Random_HH
#define Random_HH

#include <cmath>

////////////////
// Random class
////////////////
// class to generate pseudo-random numbers according to different distributions
class Random {
  
public:
  // constructor for class instance (has optional seed argument, with default value
  Random(unsigned long long seed = 5555);

  // destructor for our class (no internal members that need to be deleted)
  virtual ~Random();

  // function returns a random 64 bit integer
  unsigned long long int64();

  // function returns a random 32 bit integer
  unsigned int int32();

  // function returns a random double between (0, 1) (uniform)
  double rand();

  // function returns a random integer (0 or 1) according to a Bernoulli distr.
  int Bernoulli(double p = 0.5);

  // function returns a random double (0 to infty) according to an exponential distribution
  double Exponential(double beta = 1.);

private:
  // private member variables stored inside class instance (random number
  // generator has an internal memory)
  unsigned long long m_seed;
  unsigned long long m_u;
  unsigned long long m_v;
  unsigned long long m_w;

};

#endif

