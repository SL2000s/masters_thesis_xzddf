/*
  Benchmarking successful decryptions of a bootstrapping method.
  Inspired by /openfhe-development/src/binfhe/examples/pke/boolean-ap-pke.cpp
 */

#include "binfhecontext.h"

using namespace lbcrypto;

int main() {

    BINFHE_METHOD       method = XZDDF;
    BINFHE_PARAMSET     paramset = STD128;
    int                 nr_tests = 100;
    int                 nr_operation_iterations = 10;
    int                 test_i;
    int                 i;
    int                 count_succ = 0;
    
    for (test_i = 0; test_i < nr_tests; ++test_i) {
        std::cout << " Test iteration nr: " << test_i << std::endl;

        auto cc = BinFHEContext();
        cc.GenerateBinFHEContext(paramset, method);
        auto sk = cc.KeyGen();
        cc.BTKeyGen(sk, PUB_ENCRYPT);
        auto pk = cc.GetPublicKey();
        
        auto ct1 = cc.Encrypt(pk, 1);
        auto ct2 = cc.Encrypt(pk, 0);    
        for (i = 0; i < nr_operation_iterations; ++i) {
            ct2 = cc.EvalBinGate(OR, ct1, ct2);
            ct2 = cc.EvalBinGate(AND, ct1, ct2);
            ct2 = cc.EvalNOT(ct2);   // no bootstrapping
        }
        
        LWEPlaintext result;
        cc.Decrypt(sk, ct2, &result);
        std::cout << "  Decrypted message: " << result << std::endl;

        if (result == 0) {
            ++count_succ;
        }

    }
    std::cout << "Succeded in " << count_succ << "/" << nr_tests << " tests." << std::endl;
    
    return 0;
}
