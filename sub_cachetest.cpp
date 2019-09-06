
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
  //Get ciphertext array
  Ciphertext * ctexts = new Ciphertext[num_ciphertexts];
  for(unsigned int j = 0; j < num_ciphertexts; j++){
  	Plaintext pt = sc.encoder->encode(rand());
  	sc.encryptor->encrypt(pt, ctexts[j]);
  }
  //Get reference to Encryptor to obviate one indirection
  Evaluator & ev = sc.ev_ref();
  //Construct output stream
  ostringstream oss;
  //Prepare result
  Ciphertext result;

  //Run by iterations
  if(num_iterations){
    for(unsigned int i = 0; i < num_iterations; i++){
      unsigned int idx1 = rand() % num_ciphertexts;
      unsigned int idx2 = rand() % num_ciphertexts;
      double start = clock();
      ev.sub(ctexts[idx1], ctexts[idx2], result);
      //Get time in ms
      double duration = (clock() - start)/(double) CLOCKS_PER_MS;
      //Clear up memory used by result
      result.release();
      oss << duration << '\n';
    }
  }
  //Run by time
  else{
    double loop_start = clock();
    while ((clock() - loop_start)/(double) CLOCKS_PER_SEC <= runtime){
      unsigned int idx1 = rand() % num_ciphertexts;
      unsigned int idx2 = rand() % num_ciphertexts;
      double start = clock();
      ev.sub(ctexts[idx1], ctexts[idx2], result);
      //Get time in ms
      double duration = (clock() - start)/(double) CLOCKS_PER_MS;
      result.release();
      oss << duration << '\n';
    }
  }

  cout << oss.str() << flush;

  delete[] ctexts;

  return 0;
}
