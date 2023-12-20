//==================================================================================
// BSD 2-Clause License
//
// Copyright (c) 2014-2022, NJIT, Duality Technologies Inc. and other contributors
//
// All rights reserved.
//
// Author TPOC: contact@openfhe.org
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice, this
//    list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//==================================================================================

#include "rgsw-acc-xzddf.h"

#include <string>

namespace lbcrypto {

// Key generation as described in Section 4 of https://eprint.iacr.org/2023/1564
RingGSWACCKey RingGSWAccumulatorXZDDF::KeyGenAcc(const std::shared_ptr<RingGSWCryptoParams>& params,
                                              const NativePoly& skNTT, ConstLWEPrivateKey& LWEsk) const {
    if (!skNTT.InverseExists()) {
        OPENFHE_THROW(math_error, "GSW secret key (polynomial) has no inverse");
    }
    auto q = params->Getq().ConvertToInt();
    auto N = params->GetN();
    if ((N % q) != 0)
        OPENFHE_THROW(config_error, "q does not divide N");
    auto sv = LWEsk->GetElement();
    uint32_t n = sv.GetLength();
    
    RingGSWACCKey ek = std::make_shared<RingGSWACCKeyImpl>(1, 1, (n+1) + (q-1));
    const auto skNTTinv = skNTT.MultiplicativeInverse();                            // will be on evaluation form

    // evk_0
    auto s0 = sv[0].ConvertToInt<int32_t>();
    auto xExpS0 = GetXPower(params, s0);
    xExpS0.SetFormat(Format::EVALUATION);
    (*ek)[0][0][0] = NTRUencrypt(params, skNTTinv, xExpS0 * skNTTinv);

    // evk_i, i=1..(n-1)
#pragma omp parallel for num_threads(OpenFHEParallelControls.GetThreadLimit(n))
    for (uint32_t i = 1; i < n; ++i) {
        auto s = sv[i].ConvertToInt<int32_t>();
        (*ek)[0][0][i] = NTRUencrypt(params, skNTTinv, GetXPower(params, s));
    }

    // evk_n
    auto negSum = -sv[0].ConvertToInt<int32_t>();
    for (uint32_t i = 1; i < n; ++i) {
        negSum -= sv[i].ConvertToInt<int32_t>();
    }
    (*ek)[0][0][n] = NTRUencrypt(params, skNTTinv, GetXPower(params, negSum));

    // ksk_j
#pragma omp parallel for num_threads(OpenFHEParallelControls.GetThreadLimit(q))
    for (uint32_t i = 1; i < q; ++i) {
        auto autotrans = skNTT.AutomorphismTransform((2*N/q)*i + 1);
        autotrans.SetFormat(Format::EVALUATION);
        (*ek)[0][0][n+i] = NTRUencrypt(params, skNTTinv, autotrans*skNTTinv);
    }

    return ek;
}

// BREval as described in Section 4.1 of https://eprint.iacr.org/2023/1564.pdf
void RingGSWAccumulatorXZDDF::EvalAcc(const std::shared_ptr<RingGSWCryptoParams>& params, ConstRingGSWACCKey& ek,
                                   RLWECiphertext& acc, const NativeVector& v) const {

    const int64_t delta = params->GetQ().ConvertToInt() >> 3;               // The delta in https://eprint.iacr.org/2023/1564.pdf (move to RGSW params?)

    auto N = params->GetN();
    auto q       = params->Getq().ConvertToInt();
    uint32_t n = v.GetLength() - 1;                     // -1: since last element is the LWE b (the rest is a)

    NativeInteger wNow = ( (2*2*N - 2*(N/q)*v[0].ConvertToInt()) + 1) % (2*N);  // w_i = (-(2*N/q)*a_i+1)

    NativeInteger wNext;

    auto b = v[n].ConvertToInt();

    auto accPol = GetRotPol(params, 2*(N/q)*wNow.ModInverse(2*N).ConvertToInt());
    auto xPower0 = GetXPower(params, 2*(N/q)*b*wNow.ModInverse(2*N).ConvertToInt());
    accPol.SetFormat(Format::EVALUATION);
    xPower0.SetFormat(Format::EVALUATION);
    accPol = accPol * xPower0;
    accPol = accPol.Times(delta);   

    for (uint32_t i = 0; i < n; ++i) {
        auto ev = (*ek)[0][0][i]->GetElements()[0];
        
        accPol = ExternProd(params, accPol, ev);

        if (i < n-1) {
            wNext = ((2*2*N - 2*(N/q)*v[i+1].ConvertToInt()) + 1) % (2*N);
        }
        else {
            wNext = 1;
        }
        
        auto wProd = (wNow * wNext.ModInverse(2*N)).ConvertToInt() % (2*N);
        if (wProd != 1) {
            int kskInd = (wProd-1)*q/(2*N) + n;
            auto ksk = (*ek)[0][0][kskInd]->GetElements()[0];
            accPol = ExternProd(params, accPol.AutomorphismTransform(wProd), ksk);
        }

        wNow = wNext;
    }
    auto ev = (*ek)[0][0][n]->GetElements()[0];
    accPol = ExternProd(params, accPol, ev);

    acc->GetElements()[0] = accPol;
}

// Vector NTRU encryption as described in Section 3 of https://eprint.iacr.org/2023/1564.pdf
// skNTTinv corresponds to the inverse of the secret key f
RingGSWEvalKey RingGSWAccumulatorXZDDF::NTRUencrypt(const std::shared_ptr<RingGSWCryptoParams>& params,
                                        const NativePoly& skNTTinv, NativePoly m) const {
    if (skNTTinv.GetFormat() != Format::EVALUATION)
        OPENFHE_THROW(config_error, "skNTT does not have the format EVALUATION");

    const int64_t tau = 1;                                  // The tau in https://eprint.iacr.org/2023/1564.pdf (move to RGSW params?)

    m.SetFormat(Format::EVALUATION);
    const auto& powersG = params->GetGPower();
    auto d = params->GetDigitsG();
    std::vector<NativePoly> noisePols(d, NativePoly(params->GetDgg(), params->GetPolyParams(), Format::EVALUATION));
    RingGSWEvalKeyImpl result(1, d);
    for (uint32_t i = 0; i < d; ++i) {
        result[0][i] = noisePols[i].Times(tau);
        result[0][i] *= skNTTinv;
        result[0][i] += m.Times(powersG[i]);
    }
    return std::make_shared<RingGSWEvalKeyImpl>(result);
}

// Potential optimization: precompute X powers
NativePoly RingGSWAccumulatorXZDDF::GetXPower(const std::shared_ptr<RingGSWCryptoParams>& params,
                                              const int64_t exponent, const Format format) const {    

    const auto& polyParams = params->GetPolyParams();
    auto cycOrd = polyParams->GetCyclotomicOrder();
    uint32_t N = params->GetN();

    int64_t mm = (((exponent % cycOrd) + cycOrd) % cycOrd);
    int sign = 1;
    if (mm >= N) {
        mm -= N;
        sign = -1;
    }

    NativePoly mPol(polyParams, Format::COEFFICIENT, true);
    mPol[mm].SetValue(NativeInteger("1"));
    mPol = mPol.Times(sign);
    mPol.SetFormat(format);

    return mPol;
}

// Potential optimization: calculate vector instead of adding many polynomials with +=
NativePoly RingGSWAccumulatorXZDDF::GetRotPol(const std::shared_ptr<RingGSWCryptoParams>& params, const uint32_t expTransform) const {
    NativePoly r(params->GetPolyParams(), Format::COEFFICIENT, true);
    auto q = params->Getq().ConvertToInt();
    auto qFourth = q>>2;
//#pragma omp parallel for num_threads(OpenFHEParallelControls.GetThreadLimit(qFourth))       // Is it sure that no data race will happen?
    for (uint32_t i = 0; i < qFourth; ++i) {        
        r += GetXPower(params, -i*expTransform).Times(-1);
    }
//#pragma omp parallel for num_threads(OpenFHEParallelControls.GetThreadLimit(qFourth))       // Is it sure that no data race will happen?
    for (uint32_t i = qFourth; i < (q >> 1); ++i) {        
        r += GetXPower(params, -i*expTransform);
    }
    return r;
}

// External product as described in Section 3 of https://eprint.iacr.org/2023/1564.pdf
NativePoly RingGSWAccumulatorXZDDF::ExternProd(const std::shared_ptr<RingGSWCryptoParams>& params, NativePoly c1, std::vector<PolyImpl<NativeVector>> c2) const {
    NativePoly extProd(params->GetPolyParams(), Format::EVALUATION, true);

    int baseBits = 0;
    uint32_t baseGtmp = params->GetBaseG();
    while (baseGtmp >>= 1) ++baseBits;                          // Potential optimization: move baseBits to params
    
    auto bitdecom = c1.BaseDecompose(baseBits, true);

    for (unsigned int i = 0; i < bitdecom.size(); ++i) {
        auto p1 = bitdecom[i];
        auto p2 = c2[i];
        p1.SetFormat(Format::EVALUATION);
        p2.SetFormat(Format::EVALUATION);
        auto prod = p1 * p2;
        extProd += prod;
    }
    return extProd;
}

};  // namespace lbcrypto