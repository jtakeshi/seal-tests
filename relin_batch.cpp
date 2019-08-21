
#include <cstddef>
#include <iostream>
#include <fstream>
#include <ctime>
#include <getopt.h>
#include <seal/seal.h>

#include "SEALContainer.h"

#define CLOCKS_PER_MS (CLOCKS_PER_SEC/1000)

using namespace std;
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

  int index = optind;
  if(argc-index != 1){
    cout << "Require exactly one integer argument!" << endl;
    return 0;
  }

  //variables declaration
  int x,x_copy;
  x = atoi(argv[index]);

  //Setup SEAL
  SEALContainer sc;
  //Get plaintexts
  Plaintext px = sc.encoder->encode(x);
  //Encrypt
  Ciphertext encx, encr;
  sc.encryptor->encrypt(px, encx);
  //Get reference to Encryptor to obviate one indirection
  Evaluator & ev = sc.ev_ref();
  //Create relinearization keys
  RelinKeys relin_keys;
  relin_keys = sc.keygen->relin_keys();


  //Run by iterations
  if(num_iterations){
    for(unsigned int i = 0; i < num_iterations; i++){
      double start = clock();
      ev.square(encx, encr);
      //Get time in ms
      double duration = (clock() - start)/(double) CLOCKS_PER_MS;
      cout << duration << endl;
    }
  }
  //Run by time
  else{
    double loop_start = clock();
    while ((clock() - loop_start)/(double) CLOCKS_PER_SEC <= runtime){
      double start = clock();
      ev.square(encx, encr);
      //Get time in ms
      double duration = (clock() - start)/(double) CLOCKS_PER_MS;
      cout << duration << endl;
    }
  }

  return 0;
}
