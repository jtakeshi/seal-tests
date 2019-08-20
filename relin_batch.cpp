
#include <cstddef>
#include <iostream>
#include <fstream>
#include <ctime>
#include "seal/seal.h"

using namespace std;
using namespace seal;

int main(int argc, char ** argv){
  if(argc != 2){
    cout << "Input one arguments to encrypt/relinearize" << endl;
    return 1;
  }
  //variables declaration
  int x,x_copy;
  
  //Print a header
  cout << "************************************************************" << endl;
  cout << "operation time (s) " << endl;
  cout << "************************************************************" << endl;
  
  //start batch running
  double loop_start = clock();
  while ((clock() - loop_start)/(double) CLOCKS_PER_SEC<=500){
  //Get input to subtract
  x = atoi(argv[1]);
  x_copy = x;
  //Construct scheme parameters
  EncryptionParameters parms(scheme_type::BFV);
  size_t poly_modulus_degree = 4096;
  parms.set_poly_modulus_degree(poly_modulus_degree);
  parms.set_coeff_modulus(CoeffModulus::BFVDefault(poly_modulus_degree));
  //Create context
  parms.set_plain_modulus(1024);
  auto context = SEALContext::Create(parms);
  //Get keys
  KeyGenerator keygen(context);
  PublicKey public_key = keygen.public_key();
  SecretKey secret_key = keygen.secret_key();
  //Get HE classes
  Encryptor encryptor(context, public_key);
  Evaluator evaluator(context);
  Decryptor decryptor(context, secret_key);
  //Get int. encoder
  IntegerEncoder encoder(context);
  //Construct plaintext
  Plaintext px = encoder.encode(x);
  //Get ciphertext
  Ciphertext encx;
  encryptor.encrypt(px, encx);
  //Do relinearization
  //begin watch
  double start = clock();
  
  RelinKeys relin_keys;
  relin_keys = keygen.relin_keys();
  
  evaluator.relinearize_inplace(encx, relin_keys);
  
  //stop watch
  double duration = (clock() - start)/(double) CLOCKS_PER_SEC;
  
  //Decrypt
  decryptor.decrypt(encx, px);
  //Decode result
  x = encoder.decode_int32(px);
  //Print operation time (s)
    cout << duration << endl;
  }
  //Print
  cout << "************************************************************" << endl;
  cout << x_copy << "=" << x << endl;
  cout << "total running time: " << (clock() - loop_start)/(double) CLOCKS_PER_SEC << "s" << endl;
  return 0;
  
}
