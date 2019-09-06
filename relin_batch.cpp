
#include <cstddef>
#include <iostream>
#include <fstream>
#include <sstream>
#include <ctime>
#include <getopt.h>
#include <seal/seal.h>

#include "SEALContainer.h"

#define CLOCKS_PER_MS (CLOCKS_PER_SEC/1000)

using namespace std;
using namespace seal;

int main(int argc, char ** argv){

  //Prevent output synch
  ios_base::sync_with_stdio(false);


  char c;
  unsigned int num_iterations = 0;
  unsigned int runtime = 0;
  unsigned int num_ciphertexts = 0;
  while((c = getopt(argc, argv, "n:t:c:")) != -1){
    switch(c){
      case 'n':{
        num_iterations = atoi(optarg);
        break;
      }
      case 't':{
        runtime = atoi(optarg);
        break;
      }
      case 'c':{
      	num_ciphertexts = atoi(optarg);
      	break;
      }
    }
  }

  if((num_iterations && runtime) || 
    (!num_iterations && !runtime)){
    cout << "Specify exactly one of -n (iterations) or -t (time)" << endl;
    return 0;
  }
  if(!num_ciphertexts){
  	cout << "Specify a nonzero number of ciphertexts" << endl;
  	return 0;
  }

  srand(time(NULL));

  //variables declaration

  //Setup SEAL
  SEALContainer sc;
  //Get reference to Encryptor to obviate one indirection
  Evaluator & ev = sc.ev_ref();
  //Get relinearization keys
  RelinKeys relin_keys = sc.keygen->relin_keys();
  //Construct output stream
  ostringstream oss;
  //Get a ciphertext and do some operations on it
  Plaintext pt1 = sc.encoder->encode(rand());
  Plaintext pt2 = sc.encoder->encode(rand());
  Ciphertext ct1, ct2;
  sc.encryptor->encrypt(pt1, ct1);
  sc.encryptor->encrypt(pt2, ct2);
  //In a real application, reserve extra memory beforehand with Ciphertext::reserve
  ev.multiply_inplace(ct1, ct2);

  //Run by iterations
  if(num_iterations){
    for(unsigned int i = 0; i < num_iterations; i++){
      Ciphertext cpy = ct1;
      //Time relinearization
      double start = clock();
      ev.relinearize_inplace(cpy, relin_keys);
      //Get time in ms
      double duration = (clock() - start)/(double) CLOCKS_PER_MS;
      oss << duration << '\n';
    }
  }
  //Run by time
  else{
    double loop_start = clock();
    while ((clock() - loop_start)/(double) CLOCKS_PER_SEC <= runtime){
      Ciphertext cpy = ct1;
      //Time relinearization
      double start = clock();
      ev.relinearize_inplace(ct1, relin_keys);
      //Get time in ms
      double duration = (clock() - start)/(double) CLOCKS_PER_MS;
      oss << duration << '\n';
    }
  }

  cout << oss.str() << flush;

  return 0;
}
