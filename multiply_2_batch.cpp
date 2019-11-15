// g++ -pthread -I /usr/local/include -std=c++17 multiply_2_batch.cpp -o newparams_mult -L /usr/local/lib -lseal -O3
//#define BASELINE to strip the actual computation
#include <cstddef>
#include <iostream>
#include <fstream>
#include <ctime>
#include <chrono>
#include <cassert>
#include <getopt.h>
#include <seal/seal.h>

#include "SEALContainer.h"

#define CLOCKS_PER_MS (CLOCKS_PER_SEC/1000)

#define BASELINE 0

using namespace std;
using namespace std::chrono;
using namespace seal;

const unsigned int NUM_LEVELS = 4;

int main(int argc, char ** argv){


  char c;
  unsigned int num_iterations = 0;
  unsigned int runtime = 0;
  bool default_powers = true;
  while((c = getopt(argc, argv, "n:t:p")) != -1){
    switch(c){
      case 'n':{
        num_iterations = atoi(optarg);
        break;
      }
      case 't':{
        runtime = atoi(optarg);
        break;
      }
      case 'p':{
        default_powers = false;
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
  SEALContainer sc;
  //Remove this?
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
  //Same for relin. keys
  auto relin_keys = sc.rlk();

  high_resolution_clock::time_point start, end;

  //Run by iterations
  if(num_iterations){
    for(unsigned int i = 0; i < num_iterations; i++){
      //Reset every 4 levels
      if(!(i%NUM_LEVELS)){
        sc.encryptor->encrypt(px, encx);
      }
      //assert(sc.decryptor->invariant_noise_budget(encx));
      cout << "Budget: " << sc.decryptor->invariant_noise_budget(encx) << endl;
      start = high_resolution_clock::now();
#ifdef BASELINE      
      ev.multiply_inplace(encx, ency);
      ev.relinearize_inplace(encx, relin_keys);
#endif      
      //Get time in ms
      end = high_resolution_clock::now();
      double duration = duration_cast<chrono::microseconds>(end-start).count();
      cout << duration << endl;
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
      double duration = duration_cast<chrono::microseconds>(end-start).count();
      cout << duration << endl;
      i++;
    }
  }

  return 0;
}
