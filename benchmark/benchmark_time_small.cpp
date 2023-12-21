/*
  Benchmarking execution times of a bootstrapping method.
  Inspired by /openfhe-development/src/binfhe/examples/pke/boolean-ap-pke.cpp
 */


#include "binfhecontext.h"

using namespace lbcrypto;

int main() {
    BINFHE_METHOD       method = XZDDF;
    BINFHE_PARAMSET     paramset = STD128;
    int                 nr_operation_iterations = 10;
    int                 i;
    TimeVar             t;
    double              elapsed_time;

    auto cc = BinFHEContext();
    cc.GenerateBinFHEContext(paramset, method);
    auto sk = cc.KeyGen();

    std::cout << "Generating the bootstrapping keys..." << std::endl;
    TIC(t);
    cc.BTKeyGen(sk, PUB_ENCRYPT);
    elapsed_time = TOC(t);
    std::cout << "Generated the bootstrapping keys in " << elapsed_time << " ms." << std::endl;

    auto pk = cc.GetPublicKey();
   
    auto ct1 = cc.Encrypt(pk, 1);
    auto ct2 = cc.Encrypt(pk, 0);
    std::cout << "Starting evaluating binary operations homomorphically..." << std::endl;
    TIC(t);
    for (i = 0; i < nr_operation_iterations; i++) {
        ct2 = cc.EvalBinGate(OR, ct1, ct2);
        ct2 = cc.EvalBinGate(AND, ct1, ct2);
        ct2 = cc.EvalNOT(ct2);   // no bootstrapping
    }
    elapsed_time = TOC(t);
    std::cout << "Performed " << nr_operation_iterations << " iterations in " << elapsed_time << " ms." << std::endl;
    
    LWEPlaintext result;
    cc.Decrypt(sk, ct2, &result);
    std::cout << "Decrypted message: " << result << std::endl;

    return 0;
}
