/*
  Benchmarking execution times of multiple bootstrapping methods.
  Inspired by /openfhe-development/src/binfhe/examples/pke/boolean-ap-pke.cpp
 */

#include "binfhecontext.h"

#define EXTENSIVE_PRINT (0)
#if EXTENSIVE_PRINT
#  define PRINT(x) std::cout << x << std::endl
#  define PRINT2(x1, x2) std::cout << x1 << x2 << std::endl
#  define PRINT3(x1, x2, x3) std::cout << x1 << x2 << x3 << std::endl
#  define PRINT4(x1, x2, x3, x4) std::cout << x1 << x2 << x3 << x4 << std::endl
#else
#  define PRINT(x) do {} while (0)
#  define PRINT2(x1, x2) do {} while (0)
#  define PRINT3(x1, x2, x3) do {} while (0)
#  define PRINT4(x1, x2, x3, x4) do {} while (0)
#endif

using namespace lbcrypto;


void print_array(double a[], int sz) {
    std::cout << "[" << a[0];
    for (int i=1; i < sz; ++i) {
        std::cout << ", " << a[i];
    }
    std::cout << "]" << std::endl;
}

double average_array(double a[], int sz) {
    double out = 0;
    for (int i=0; i < sz; ++i) {
        out += a[i];
    }
    return out/sz;
}

int main() {

    int             nr_tests = 100;
    int             nr_operation_iterations = 10;
    BINFHE_PARAMSET paramset;
    BINFHE_METHOD   method;
    uint32_t        test_i;
    int             i;
    int             j;
    TimeVar         t;
    LWECiphertext   ct_res;
    LWEPlaintext    decr;
    double          elapsed_time;
    double          times_gen[nr_tests];
    double          times_or[nr_tests];
    double          times_and[nr_tests];
    double          times_or_and_not[nr_tests];

    BINFHE_PARAMSET paramsets[] = {STD128, STD128, STD128_LMKCDEY, STD128, STD192, STD192, STD192, STD192};
    std::vector<BINFHE_METHOD> methods = {AP, GINX, LMKCDEY, XZDDF, AP, GINX, LMKCDEY, XZDDF};
    
    for (test_i=0; test_i < methods.size(); ++test_i) {
        method = methods[test_i];
        paramset = paramsets[test_i];
    
        for (i = 0; i < nr_tests; ++i) {
            PRINT2("Iteration: ", i);

            auto cc = BinFHEContext();
            cc.GenerateBinFHEContext(paramset, method); 
            auto sk = cc.KeyGen();

            PRINT("  Generating the bootstrapping keys...");
            TIC(t);
            cc.BTKeyGen(sk, PUB_ENCRYPT);
            elapsed_time = TOC(t);
            times_gen[i] = elapsed_time;
            PRINT3("    Generated the bootstrapping keys in ", elapsed_time, " ms.");

            auto pk = cc.GetPublicKey();
            
            auto ct1 = cc.Encrypt(pk, 1);
            auto ct2 = cc.Encrypt(pk, 0);

            PRINT("  Evaluating OR operation...");
            TIC(t);
            ct_res = cc.EvalBinGate(OR, ct1, ct2);
            elapsed_time = TOC(t);
            times_or[i] = elapsed_time;
            cc.Decrypt(sk, ct_res, &decr);
            PRINT4("    Performed OR in: ", elapsed_time, " ms.        Decryption: ", decr);

            PRINT("  Evaluating AND operation...");
            TIC(t);
            ct_res = cc.EvalBinGate(AND, ct1, ct2);
            elapsed_time = TOC(t);
            times_and[i] = elapsed_time;
            cc.Decrypt(sk, ct_res, &decr);
            PRINT4("    Performed AND in: ", elapsed_time, " ms.        Decryption: ", decr);

            PRINT("  Evaluating OR, AND, NOT operation...");
            TIC(t);
            for (j = 0; j < nr_operation_iterations; ++j) {
                ct2 = cc.EvalBinGate(OR, ct1, ct2);
                ct2 = cc.EvalBinGate(AND, ct1, ct2);
                ct2 = cc.EvalNOT(ct2);                  // no bootstrapping
            }
            ct_res = ct2;
            elapsed_time = TOC(t);
            times_or_and_not[i] = elapsed_time;
            cc.Decrypt(sk, ct_res, &decr);
            PRINT4("    Performed OR, AND, NOT in: ", elapsed_time, " ms.        Decryption: ", decr);
        }

        std::cout << std::endl << "======== Results of test nr. " << test_i << " ========" << std::endl << std::endl;

        std::cout << "Times, KeyGen: " << std::endl;
        print_array(times_gen, nr_tests);
        std::cout << "Times, OR: " << std::endl;
        print_array(times_or, nr_tests);
        std::cout << "Times, AND: " << std::endl;
        print_array(times_and, nr_tests);
        std::cout << "Times, 10(AND, OR, NOT): " << std::endl;
        print_array(times_or_and_not, nr_tests);

        std::cout << "Average time, KeyGen: " << average_array(times_gen, nr_tests) << " ms." << std::endl;
        std::cout << "Average time, OR: " << average_array(times_or, nr_tests) << " ms." << std::endl;
        std::cout << "Average time, AND: " << average_array(times_and, nr_tests) << " ms." << std::endl;
        std::cout << "Average time, 10(AND, OR, NOT): " << average_array(times_or_and_not, nr_tests) << " ms." << std::endl;    

        std::cout << std::endl << "================" << std::endl << std::endl;
    }
       
    return 0;
}