//SEALContainer.h
//Simple class to encapsulate the boilerplate around SEAL

#include <getopt.h>
#include <cstddef>
#include <memory>
#include <seal/seal.h>
using namespace seal;

class SEALContainer{

public:

//SEAL deletes default constructors, so we have to use indirection
EncryptionParameters * parms;
std::shared_ptr<seal::SEALContext> context;
KeyGenerator * keygen;
PublicKey * public_key;
SecretKey * secret_key;
Encryptor * encryptor;
Evaluator * evaluator;
Decryptor * decryptor;
IntegerEncoder * encoder;

SEALContainer(unsigned int poly_modulus_degree = 4096, unsigned int plain_modulus = 1024){
	//Scheme type is hardcoded - change later to allow flexibility?
	parms = new EncryptionParameters(scheme_type::BFV);
	parms->set_poly_modulus_degree(poly_modulus_degree);
  parms->set_coeff_modulus(CoeffModulus::BFVDefault(poly_modulus_degree));
  //Create context
  parms->set_plain_modulus(1024);
  context = SEALContext::Create(*parms);
  //Get key classes
  keygen = new KeyGenerator(context);
  public_key = new PublicKey(keygen->public_key());
  secret_key = new SecretKey(keygen->secret_key());
  //Get utility classes
  encryptor = new Encryptor(context, *public_key);
  evaluator = new Evaluator(context);
  decryptor = new Decryptor(context, *secret_key);
  encoder = new IntegerEncoder(context);
}

~SEALContainer(){
  delete parms;
  delete keygen;
  delete public_key;
  delete secret_key;
  delete encryptor;
  delete evaluator;
  delete decryptor;
  delete encoder;
}

//Returns a reference so the application can avoid one level of indirection
Evaluator & ev_ref(){
  return *evaluator;
}


};