#include "Random.hh"

////////////////
// Random class
////////////////
// class to generate pseudo-random numbers


// constructor for class instance (has optional seed argument, with default value
Random::Random(unsigned long long seed){
  m_v = 4101842887655102017LL;
  m_w = 1;
  
  m_u = seed ^ m_v;
  int64();
  m_v = m_u;
  int64();
  m_w = m_v;
  int64();
}

// destructor for our class (no internal members that need to be deleted)
Random::~Random(){

}

// function returns a random 64 bit integer
unsigned long long Random::int64(){
  m_u = m_u * 2862933555777941757LL + 7046029254386353087LL;
  m_v ^= m_v >> 17;
  m_v ^= m_v << 31;
  m_v ^= m_v >> 8;
  m_w = 4294957665U*(m_w & 0xffffffff) + (m_w >> 32);
  unsigned long long x = m_u ^ (m_u << 21);
  x ^= x >> 35;
  x ^= x << 4;
  return (x + m_v) ^ m_w;
}

// function returns a random 32 bit integer
unsigned int Random::int32(){
  return (unsigned int)int64(); 
}

// function returns a random double between (0, 1) (uniform)
double Random::rand(){
  return 5.42101086242752217E-20 * int64();
}

// function returns a random integer (0 or 1) according to a Bernoulli distr.
int Random::Bernoulli(double p){
  if(p < 0. || p > 1.)
    return 1;
  
  double R = rand();

  if(R < p)
    return 1;
  else
    return 0;
}

// function returns a random double (0 to infty) according to an exponential distribution
double Random::Exponential(double beta){
  // make sure beta is consistent with an exponential
  if(beta <= 0.)
    beta = 1.;
  
  double R = rand();
  while(R <= 0.)
    R = rand();

  double X = -log(R)/beta;

  return X;
}

