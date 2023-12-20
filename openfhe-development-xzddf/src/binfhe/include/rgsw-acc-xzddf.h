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

#ifndef _RGSW_ACC_XZDDF_H_
#define _RGSW_ACC_ZDDF_H_

#include "rgsw-acc.h"

#include <memory>

namespace lbcrypto {

/**
 * @brief Ring GSW accumulator schemes described in
 * https://eprint.iacr.org/2023/1564
 */
class RingGSWAccumulatorXZDDF final : public RingGSWAccumulator {
public:
    RingGSWAccumulatorXZDDF() = default;

    /**
   * Key generation for internal Ring GSW as described in https://eprint.iacr.org/2023/1564.pdf
   *
   * @param params a shared pointer to RingGSW scheme parameters
   * @param skNTT secret key polynomial in the EVALUATION representation
   * @param LWEsk the secret key
   * @return a shared pointer to the resulting keys
   */
    RingGSWACCKey KeyGenAcc(const std::shared_ptr<RingGSWCryptoParams>& params, const NativePoly& skNTT,
                            ConstLWEPrivateKey& LWEsk) const override;

    /**
   * Main accumulator function used in bootstrapping - XZDDF variant
   *
   * @param params a shared pointer to RingGSW scheme parameters
   * @param ek the accumulator key
   * @param acc previous value of the accumulator
   * @param v value of length n+1 to update the accumulator with -- first n elements should be LWE a, last element should be LWE b
   */
    void EvalAcc(const std::shared_ptr<RingGSWCryptoParams>& params, ConstRingGSWACCKey& ek, RLWECiphertext& acc,
                 const NativeVector& v) const override;

private:
    /**
   * Vector NTRU encryption of a message as described in https://eprint.iacr.org/2023/1564.pdf
   *
   * @param params a shared pointer to RingGSW scheme parameters
   * @param skNTTinv inverse of the secret key polynomial in the EVALUATION representation
   * @param m a plaintext
   * @return a NTRU encryption on format EVALUATION
   */
    RingGSWEvalKey NTRUencrypt(const std::shared_ptr<RingGSWCryptoParams>& params, const NativePoly& skNTTinv,
                                   NativePoly m) const;

    /**
   * Get a power of X
   *
   * @param params a shared pointer to RingGSW scheme parameters
   * @param exponent the exponent of X
   * @param format the format that the output polynomial will have (default is COEFFICIENT)
   * @return a power of X
   */
    NativePoly GetXPower(const std::shared_ptr<RingGSWCryptoParams>& params, const int64_t exponent, const Format format=Format::COEFFICIENT) const;

    /**
   * Computes a modified version of the rotation polynomial in https://eprint.iacr.org/2023/1564.pdf
   *
   * @param params a shared pointer to RingGSW scheme parameters
   * @param expTransform an exponent to put inside the rotation polynomial: r(X^expTransform) (usually expTransform=2N/q)
   * @return a rotation polynomial
   */   
    NativePoly GetRotPol(const std::shared_ptr<RingGSWCryptoParams>& params, const uint32_t expTransform=1) const;

    /**
   * Computes the external product between a Ring GSW Polynomial and a vector NTRU ciphertext as described in https://eprint.iacr.org/2023/1564.pdf
   *
   * @param params a shared pointer to RingGSW scheme parameters
   * @param c1 a ring GSW polynomial
   * @param c2 a vector NTRU ciphertext
   * @return an external product on format EVALUATION
   */   
    NativePoly ExternProd(const std::shared_ptr<RingGSWCryptoParams> &params, NativePoly c1, std::vector<PolyImpl<NativeVector>> c2) const;
};

}  // namespace lbcrypto

#endif  // _RGSW_ACC_XZDDF_H_
