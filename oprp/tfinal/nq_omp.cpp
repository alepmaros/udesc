/*
 * n-queens problem using only openmp
 * idea/base code from: https://hbfs.wordpress.com/2012/03/27/openmp-and-the-n-queens-problem/
 */

#include <sys/time.h>
#include <time.h>    // gettimeofday
#include <stdint.h>  // uint64_t
#include <cstdlib>   // atoi
#include <iostream>
#include <algorithm> // min, max
#include <iomanip>  // setw, setprecision, etc.


#define NUM_THREADS 8

///////////////////////////////////////

const int maxdim = 32;

///////////////////////////////////////
inline uint64_t now() 
 {
  struct timeval tv;
  gettimeofday(&tv,0);
  return tv.tv_sec*1000000ull+tv.tv_usec;
 }

///////////////////////////////////////
inline bool test(int k, int j,
                 uint64_t diag45,
                 uint64_t diag135,
                 uint64_t cols)  
 {
  return ( (cols    & (1ull << j))
         + (diag135 & (1ull << (j+k))) 
         + (diag45  & (1ull << (32+j-k))) )==0;
 }

///////////////////////////////////////
uint64_t solve( int niv, int dx, 
                uint64_t diag45,
                uint64_t diag135,
                uint64_t cols)
 {
  uint64_t solutions=0;
  if (niv)
   {
    for (int i=0; i<dx; i++)
     if (test(niv,i, diag45, diag135, cols))
       solutions+=solve ( niv-1, dx, 
                          diag45 | (1ull << (32+i-niv)), 
                          diag135 | (1ull << (niv+i)), 
                          cols | (1ull << i ));
   }
  else
   for (int i=0; i<dx; i++)
    solutions += test(0,i, diag45, diag135, cols);

  return solutions;
 }

uint64_t meta_solve( int dx )
 {
  uint64_t solutions=0;

  int niv=dx-1;

  #pragma omp parallel for reduction(+:solutions) schedule(guided) num_threads(NUM_THREADS)
  for (int i=0;i<dx;i++)
   solutions+=solve(niv-1,dx,
                    (1ull << (32+i-niv)),  // diag45
                    (1ull << (niv+i)),     // diag135
                    (1ull << i )           // cols
                    );

  return solutions;
 }

///////////////////////////////////////
int main(int argc, const char * argv[])
 {
  if (argc >= 2 )
   {
    int dx = std::max(1,std::min(maxdim,std::atoi(argv[1])));

    uint64_t start = now();

    uint64_t solutions = meta_solve(dx);

    uint64_t stop = now();

    std::cout << dx << ' '
              << solutions << ' ' 
              << std::setiosflags(std::ios::fixed)
              << std::setprecision(6)
              << (stop-start)/(double)1000000.0 
              << std::endl;

    return 0;
   }
  else 
   {
    std::cerr << "./nqo <num>" << std::endl;
    return 1;
   }
 }

