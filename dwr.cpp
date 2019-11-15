// g++ -std=c++17 dwr.cpp -o dwr -O3
//#define BASELINE to strip the actual computation
#include <cstddef>
#include <iostream>
#include <fstream>
#include <ctime>
#include <chrono>
#include <random>
#include <stdexcept>
#include <getopt.h>

#define CLOCKS_PER_MS (CLOCKS_PER_SEC/1000)

using namespace std;
using namespace std::chrono;

//Shamelessly taken from https://stackoverflow.com/questions/2422712/rounding-integer-division-instead-of-truncating

#define DWR(dividend, divisor) ((dividend + (divisor / 2)) / divisor)

inline unsigned int round_closest(unsigned int dividend, unsigned int divisor)
{
    return (dividend + (divisor / 2)) / divisor;
}

int main(int argc, char ** argv){


  char c;
  unsigned int num_iterations = 0;
  unsigned int runtime = 0;
  //bool default_powers = true;
  while((c = getopt(argc, argv, "n:t:")) != -1){
    switch(c){
      case 'n':{
        num_iterations = atoi(optarg);
        break;
      }
      case 't':{
        runtime = atoi(optarg);
        break;
      }
    }
  }

  if((num_iterations && runtime) || 
    (!num_iterations && !runtime)){
    cout << "Specify exactly one of -n (iterations) or -t (time)" << endl;
    return 0;
  }

  /*
  int index = optind;
  if(argc-index != 2){
    cout << "Require exactly two integer arguments!" << endl;
    return 0;
  }

  //variables declaration
  int x,y,x_copy;
  x = atoi(argv[index++]);
  y = atoi(argv[index]);
  */

  /*
  //Setup SEAL
  SEALContainer sc;
  if(!default_powers){
    sc.set_ctext_moduli(1<<15);
  }
  //Get plaintexts
  Plaintext px = sc.encoder->encode(x);
  Plaintext py = sc.encoder->encode(y);
  //Encrypt
  Ciphertext encx, ency, encr;
  sc.encryptor->encrypt(px, encx);
  sc.encryptor->encrypt(py, ency);
  //Get reference to Encryptor to obviate one indirection
  Evaluator & ev = sc.ev_ref();
  */

  //Have to do all this to get rand. uint64s
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<uint64_t> dis(
    std::numeric_limits<uint64_t>::min(),
   std::numeric_limits<uint64_t>::max());

  high_resolution_clock::time_point start, end;

  uint64_t x, y;
  //Run by iterations
  if(num_iterations){
    for(unsigned int i = 0; i < num_iterations; i++){
      x = dis(gen);
      y = dis(gen);
      start = high_resolution_clock::now();	
#ifndef BASELINE
     	uint64_t result = DWR(x, y);
#endif      
      //Get time in ns
      end = high_resolution_clock::now();
      double duration = duration_cast<chrono::nanoseconds>(end-start).count();
      cout << duration << endl;
    }
  }
  //Run by time
  else{
    double loop_start = clock();
    while ((clock() - loop_start)/(double) CLOCKS_PER_SEC <= runtime){
      x = dis(gen);
      y = dis(gen);	
      start = high_resolution_clock::now(); 
#ifdef BASELINE
      uint64_t result = DWR(x, y);
#endif      
      //Get time in ns
      end = high_resolution_clock::now();
      double duration = duration_cast<chrono::nanoseconds>(end-start).count();
      cout << duration << endl;
    }
  }

  return 0;
}
