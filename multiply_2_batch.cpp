
#include <cstddef>
#include <iostream>
#include <fstream>
#include <ctime>
#include <chrono>
#include <getopt.h>
#include <seal/seal.h>

#include "SEALContainer.h"

#define CLOCKS_PER_MS (CLOCKS_PER_SEC/1000)

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
  auto relin_keys = sc.rlk_ref();

  high_resolution_clock::time_point start, end;

  //Run by iterations
  if(num_iterations){
    for(unsigned int i = 0; i < num_iterations; i++){
      //Reset every 4 levels
      if(!(i%NUM_LEVELS)){
        sc.encryptor->encrypt(px, encx);
      }
      start = high_resolution_clock::now();
      ev.multiply_inplace(encx, ency);
      ev.relinearize_inplace(encx, relin_keys);
      //Get time in ns
      end = high_resolution_clock::now();
      double duration = duration_cast<chrono::nanoseconds>(end-start).count();
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
      ev.multiply_inplace(encx, ency);
      ev.relinearize_inplace(encx, relin_keys);
      //Get time in ns
      end = high_resolution_clock::now();
      double duration = duration_cast<chrono::nanoseconds>(end-start).count();
      cout << duration << endl;
      i++;
    }
  }

  return 0;
}
