//g++ -pthread -I /usr/local/include -std=c++17 dwr.cpp -o dwr -L /usr/local/lib -lseal -O3

#include <cstddef>
#include <iostream>
#include <fstream>
#include <ctime>
#include <cassert>
#include <getopt.h>
#include <gmp.h>
#include <gmpxx.h>
#include <seal/seal.h>

#include "SEALContainer.h"

#define CLOCKS_PER_MS (CLOCKS_PER_SEC/1000)

using namespace std;
using namespace seal;

int main(int argc, char ** argv){

  //Setup SEAL
	srand(time(NULL));
	unsigned int poly_mod = rand();
	unsigned int coeff_mod = rand();
  SEALContainer sc(atoi(argv[1]), atoi(argv[2]));  
  std::vector<SmallModulus> mods = sc.parms->coeff_modulus();
  for(auto & x : mods){
  	/*
  	x = 1 << 15;
  	*/
  	
  	cout << x.value() << ' ';
  	mpz_class xm = x.value();
  	assert(mpz_probab_prime_p(xm.get_mpz_t(), 15));
  	
  }
  cout << "coeff_mod size: " << mods.size() << endl;
  //sc.parms->set_coeff_modulus(mods);
 
  /*
  //Set ciphertext moduli
  std::vector<SmallModulus> mods(4);
  uint64_t mod_val = 1 << 63;
  mods[0] = mods[1] = mods[2] = mods[3] = mod_val;
  sc.parms->set_coeff_modulus(mods);
  */
  /*
  cout << "Moduli: " << endl;
  for(const auto & x : mods){
    cout << x.value() << endl;
  }
  */

  

  unsigned int num_iterations = 1;
  srand(time(NULL));
  for(unsigned int i = 0; i < num_iterations; i++){
    unsigned int x = rand();
    unsigned int y = rand();
    //cout << "x: " << x << " y: " << y;
    Plaintext px = sc.encoder->encode(x);
    Plaintext py = sc.encoder->encode(y);
    Ciphertext encx, ency;
    sc.encryptor->encrypt(px, encx);
    sc.encryptor->encrypt(py, ency);
    Evaluator & ev = sc.ev_ref();
    cout << "mod_count: " << encx.coeff_mod_count() << endl;
    cout << "poly degree: " << encx.poly_modulus_degree() << endl;
    /*
    for(unsigned int j = 0; j < 100; j++){
    	ev.multiply_inplace(encx, ency);
    }
    
    //Print ciphertext size
    cout << endl;
    cout << "K: " << encx.coeff_mod_count() << endl;
    cout << "N: " << encx.poly_modulus_degree() << endl;
    cout << "size: " << encx.size() << endl;
    cout << "Total capacity (64-bit words): " << encx.uint64_count_capacity() << endl;
    8?
    /*
    Plaintext result;
    sc.decryptor->decrypt(encx, result);
    unsigned int ires = sc.encoder->decode_uint64(result);
    */
    /*
    if(ires != x*y){
      cout << endl;
      cout << "Actual: " << x*y << endl;
      cout << "Expected: " << ires << endl;
      assert(ires == x*y);
      return 0;
    }
    else{
      cout << " x*y: " << x*y << endl;
    }
    */
  }


  return 0;
}
