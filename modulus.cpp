
#include <cstddef>
#include <iostream>
#include <fstream>
#include <ctime>
#include <cassert>
#include <getopt.h>
#include <seal/seal.h>

#include "SEALContainer.h"

#define CLOCKS_PER_MS (CLOCKS_PER_SEC/1000)

using namespace std;
using namespace seal;

int main(int argc, char ** argv){

  //Setup SEAL
  SEALContainer sc;  

  //Set ciphertext moduli
  std::vector<SmallModulus> mods(4);
  uint64_t mod_val = 1 << 63;
  mods[0] = mods[1] = mods[2] = mods[3] = mod_val;
  sc.parms->set_coeff_modulus(mods);
  /*
  cout << "Moduli: " << endl;
  for(const auto & x : mods){
    cout << x.value() << endl;
  }
  */

  if(argc < 2){
    cout << "Require one integer argument!" << endl;
    return 0;
  }

  unsigned int num_iterations = atoi(argv[1]);
  srand(5);
  for(unsigned int i = 0; i < num_iterations; i++){
    unsigned int x = rand();
    int y = rand();
    cout << "x: " << x << " y: " << y;
    Plaintext px = sc.encoder->encode(x);
    Plaintext py = sc.encoder->encode(y);
    Ciphertext encx, ency;
    sc.encryptor->encrypt(px, encx);
    sc.encryptor->encrypt(py, ency);
    Evaluator & ev = sc.ev_ref();
    ev.multiply_inplace(encx, ency);
    Plaintext result;
    sc.decryptor->decrypt(encx, result);
    unsigned int ires = sc.encoder->decode_uint64(result);
    if(ires != x*y){
      cout << endl;
      cout << "Actual: " << x*y << endl;
      cout << "Expected: " << ires << endl;
      return 0;
    }
    else{
      cout << " x*y: " << x*y << endl;
    }
  }


  return 0;
}
