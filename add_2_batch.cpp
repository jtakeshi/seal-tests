
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

int main(int argc, char ** argv){


  char c;
  unsigned int num_iterations = 0;
  unsigned int runtime = 0;
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
  */

  //variables declaration
  int x,y;
  srand(5);
  x = rand();
  y = rand();

  //Setup SEAL
  SEALContainer sc;
  //Get plaintexts
  Plaintext px = sc.encoder->encode(x);
  Plaintext py = sc.encoder->encode(y);
  //Encrypt
  Ciphertext encx, ency;
  sc.encryptor->encrypt(px, encx);
  sc.encryptor->encrypt(py, ency);
  //Get reference to Encryptor to obviate one indirection
  Evaluator & ev = sc.ev_ref();

  high_resolution_clock::time_point start, end;

  //Run by iterations
  if(num_iterations){
    for(unsigned int i = 0; i < num_iterations; i++){
      start = high_resolution_clock::now();
      ev.add_inplace(encx, ency);
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
      start = high_resolution_clock::now();
      ev.add_inplace(encx, ency);
      //Get time in ns
      end = high_resolution_clock::now();
      double duration = duration_cast<chrono::nanoseconds>(end-start).count();
      cout << duration << endl;
    }
  }

  return 0;
}
