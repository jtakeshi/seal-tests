// g++ -pthread -I /usr/local/include -std=c++17 multiply_2_batch.cpp -o newparams_mult -L /usr/local/lib -lseal -O3
//#define BASELINE to strip the actual computation
#include <cstddef>
#include <iostream>
#include <fstream>
#include <ctime>
#include <chrono>
#include <cassert>
#include <sstream>
#include <getopt.h>
#include <seal/seal.h>

#include "SEALContainer.h"

#define CLOCKS_PER_MS (CLOCKS_PER_SEC/1000)

#ifdef BASELINE
#define TIME_UNITS nanoseconds
#else
#define TIME_UNITS microseconds
#endif

using namespace std;
using namespace std::chrono;
using namespace seal;

const unsigned int NUM_LEVELS = 4;

int main(int argc, char ** argv){

  std::ostringstream oss;


  char c;
  unsigned int num_iterations = 0;
  unsigned int runtime = 0;
  //bool default_powers = true;
  unsigned int poly_degree = 0;
  unsigned int sec_level = 0;
  while((c = getopt(argc, argv, "n:t:d:s:")) != -1){
    switch(c){
      case 'n':{
        num_iterations = atoi(optarg);
        break;
      }
      case 't':{
        runtime = atoi(optarg);
        break;
      }
      case 'd':{
        poly_degree = atoi(optarg);
        break;
      }
      case 's':{
        sec_level = atoi(optarg);
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
  */

  //variables declaration
  int x,y;
  srand(5);
  x = rand();
  y = rand();

  //Setup SEAL
  SEALContainer sc(poly_degree, sec_level);
  //Remove this?
  /*
  if(!default_powers){
    sc.set_ctext_moduli(1<<15);
  }
  */
  //Get plaintexts
  Plaintext px = sc.encoder->encode(x);
  //Encrypt
  Ciphertext encx;
  sc.encryptor->encrypt(px, encx);

  high_resolution_clock::time_point start, end;

  //Run by iterations
  if(num_iterations){
    for(unsigned int i = 0; i < num_iterations; i++){
      //Reset every 4 levels
      start = high_resolution_clock::now();
#ifndef BASELINE      
      sc.decryptor->decrypt(encx, px);
#endif      
      //Get time in ms and ns
      end = high_resolution_clock::now();
      //double micro_duration = duration_cast<chrono::microseconds>(end-start).count();
      double nano_duration = duration_cast<chrono::nanoseconds>(end-start).count();
      //cout << "us " << micro_duration << endl;
      oss << nano_duration << '\n';
    }
  }
  //Run by time
  else{
    unsigned int i = 0;
    double loop_start = clock();
    while ((clock() - loop_start)/(double) CLOCKS_PER_SEC <= runtime){
      //Reset every 4 levels
      if(!(i%NUM_LEVELS)){
        sc.encryptor->encrypt(px, encx);
      }
      start = high_resolution_clock::now();
#ifdef BASELINE      
      ev.multiply_inplace(encx, ency);
      ev.relinearize_inplace(encx, relin_keys);
#endif      
      //Get time in ms
      end = high_resolution_clock::now();
      double duration = duration_cast<chrono::TIME_UNITS>(end-start).count();
      cout << duration << endl;
      i++;
    }
  }

  cout << oss.str() << endl;

  return 0;
}
