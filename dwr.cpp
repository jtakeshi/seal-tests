
#include <cstddef>
#include <iostream>
#include <fstream>
#include <ctime>
#include <stdexcept>
#include <getopt.h>
#include <seal/seal.h>

#include "SEALContainer.h"

#define CLOCKS_PER_MS (CLOCKS_PER_SEC/1000)

using namespace std;
using namespace seal;
using namespace seal::util;

int main(int argc, char ** argv){


  char c;
  unsigned int num_iterations = 0;
  unsigned int runtime = 0;
  //bool default_powers = true;
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
      /*
      case 'p':{
        default_powers = false;
        break;
      }
      */
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

  uint64_t x, y;
  srand(5);
  //Run by iterations
  if(num_iterations){
    for(unsigned int i = 0; i < num_iterations; i++){
      while((x = rand()) < 0){}
      while((y = rand()) <= 0){}
      double start = clock();	
      try{
      	uint64_t result = divide_round_up(x, y);
      }
      catch(std::out_of_range & ex){
      	i--;
      	continue;
      }
      //Get time in ms
      double duration = (clock() - start)/(double) CLOCKS_PER_MS;
      cout << duration << endl;
    }
  }
  //Run by time
  else{
    double loop_start = clock();
    while ((clock() - loop_start)/(double) CLOCKS_PER_SEC <= runtime){
      while((x = rand()) < 0){}
      while((y = rand()) <= 0){}	
      double start = clock();
      try{
      	uint64_t result = divide_round_up(x, y);
      }
      catch(std::out_of_range & ex){
      	continue;
      }
      //Get time in ms
      double duration = (clock() - start)/(double) CLOCKS_PER_MS;
      cout << duration << endl;
    }
  }

  return 0;
}
