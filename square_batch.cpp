
#include <cstddef>
#include <iostream>
#include <fstream>
#include <ctime>
#include "seal/seal.h"

using namespace std;
using namespace seal;

int main(int argc, char ** argv){
  if(argc != 2){
    cout << "Input one argument to square" << endl;
    return 1;
  }
  //variables declaration
  int x,r;
  
  //Print a header
  cout << "************************************************************" << endl;
  cout << "operation time (s) " << endl;
  cout << "************************************************************" << endl;
  
  //start batch running
  double loop_start = clock();
  while ((clock() - loop_start)/(double) CLOCKS_PER_SEC<=500){
  //Get input to square
  x = atoi(argv[1]);
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
  //Construct plaintexts
  Plaintext px = encoder.encode(x);
  //Get ciphertexts
  Ciphertext encx, ency, encr;
  encryptor.encrypt(px, encx);
  //Do square
  //begin watch
  double start = clock();
  
  evaluator.square(encx, encr);
  
  //stop watch
  double duration = (clock() - start)/(double) CLOCKS_PER_SEC;
  
  //Decrypt
  Plaintext pr;
  decryptor.decrypt(encr, pr);
  //Decode result
  r = encoder.decode_int32(pr);
  //Print operation time (s)
    cout << duration << endl;
  }
  //Print
  cout << "************************************************************" << endl;
  cout << x <<  "^2 = " << r << endl;
  cout << "total running time: " << (clock() - loop_start)/(double) CLOCKS_PER_SEC << "s" << endl;
  return 0;
  
}
