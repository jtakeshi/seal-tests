// g++ -pthread -I /usr/local/include -std=c++17 cachetest.cpp -o cachetest -L /usr/local/lib -lseal -O3
// #define BASELINE to only use one ciphertext

#ifndef _GNU_SOURCE
# define _GNU_SOURCE
#endif

#include <sys/sysinfo.h>
#include <unistd.h>

#include <cstddef>
#include <iostream>
#include <fstream>
#include <sstream>
#include <ctime>
#include <getopt.h>
#include <seal/seal.h>

#include "SEALContainer.h"

#define CLOCKS_PER_MS (CLOCKS_PER_SEC/1000)

static const size_t CACHE_SIZE = sysconf(_SC_LEVEL1_DCACHE_SIZE) + sysconf(_SC_LEVEL2_CACHE_SIZE) + sysconf(_SC_LEVEL3_CACHE_SIZE);

size_t ciphertext_bytes(const Ciphertext & c){
  return c.coeff_mod_count()*c.poly_modulus_degree()*c.size()*sizeof(uint64_t);
}

size_t num_ciphertexts_to_fill_cache(Ciphertext & c){
  size_t cbytes = ciphertext_bytes(c);
  size_t ret = CACHE_SIZE/cbytes;
  if(CACHE_SIZE % cbytes != 0){
    ret++;
  }
  return ret;
}

using namespace std;
using namespace seal;

int main(int argc, char ** argv){

  //Prevent output synch
  ios_base::sync_with_stdio(false);


  unsigned int num_iterations = 0;
  unsigned int runtime = 0;
  //bool default_powers = true;
  unsigned int poly_degree = 0;
  unsigned int sec_level = 0;
  char c;
  
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

  srand(time(NULL));

  //variables declaration
  
  //Setup SEAL
  SEALContainer sc(poly_degree, sec_level);
  
  //Get reference to Encryptor to obviate one indirection
  Evaluator & ev = sc.ev_ref();
  //Construct output stream
  ostringstream oss;
  //Prepare result
  //Ciphertext result;

  
  //Get ciphertext array
  vector<Ciphertext> ctext_vec(1);
  //Get plaintexts
  int x;
  srand(5);
  x = rand();
  Plaintext px = sc.encoder->encode(x);
  sc.encryptor->encrypt(px, ctext_vec[0]);
  size_t num_ct = num_ciphertexts_to_fill_cache(ctext_vec[0]);
  //Fill up cache
#ifndef BASELINE  
  ctext_vec.resize(num_ct);
  for(size_t i = 1; i < num_ct; i++){
    x = rand();
    px = sc.encoder->encode(x);
    sc.encryptor->encrypt(px, ctext_vec[i]);
  }
#endif

  //Run by iterations
  if(num_iterations){
    for(unsigned int i = 0; i < num_iterations; i++){
      unsigned int idx = i % ctext_vec.size();
      double start = clock();
      ev.negate_inplace(ctext_vec[idx]);
      //Get time in ms
      double duration = (clock() - start)/(double) CLOCKS_PER_MS;
      oss << duration << '\n';
    }
  }
  //Run by time
  else{
    double loop_start = clock();
    unsigned int i = 0;
    while ((clock() - loop_start)/(double) CLOCKS_PER_SEC <= runtime){
      unsigned int idx = i % ctext_vec.size();
      double start = clock();
      ev.negate_inplace(ctext_vec[idx]);
      //Get time in ms
      double duration = (clock() - start)/(double) CLOCKS_PER_MS;
      oss << duration << '\n';
    }
  }

  cout << oss.str() << flush;

  return 0;
}
