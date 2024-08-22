#include "../include/RSWPuzzle.h"
#include "../include/util.h"
#include <iostream>
namespace vdf
{
RSWPuzzle::RSWPuzzle(
    const unsigned long _lambda,
    const unsigned long _t,
    const bytevec &_x) : lambda(_lambda), t(_t)
{
  x = BN_secure_new();
  T = BN_secure_new();

  BN_bin2bn(_x.data(), _x.size(), x);
  BN_dec2bn(&T, "1");
  BN_lshift(T, T, t);
}

RSWPuzzle::RSWPuzzle(
    const unsigned long _lambda,
    const unsigned long _t,
    const bytevec &_x,
    const bytevec &_N) : RSWPuzzle(_lambda, _t, _x)
{
  N = BN_secure_new();
  BN_bin2bn(_N.data(), _N.size(), N);

  BN_CTX *ctx = BN_CTX_secure_new();
  BN_CTX_start(ctx);
  BN_mod(x, x, N, ctx);
  BN_CTX_end(ctx);
  BN_CTX_free(ctx);
#ifdef _DEBUG
  std::cout << "RSWPuzzle x:\t" << print_bn(x) << std::endl;
  std::cout << "RSWPuzzle T:\t" << print_bn(T) << std::endl;
  std::cout << "RSWPuzzle N:\t" << print_bn(N) << std::endl;
#endif
}

RSWPuzzle::RSWPuzzle(
    const unsigned long _lambda,
    const unsigned long _t,
    const bytevec &_x,
    const unsigned long _lambdaRSW) : RSWPuzzle(_lambda, _t, _x)
{
  N = BN_secure_new();

  BN_CTX *ctx = BN_CTX_secure_new();
  BN_CTX_start(ctx);
  BIGNUM *p, *q;
  p = BN_CTX_get(ctx);
  q = BN_CTX_get(ctx);

  BN_generate_prime_ex(p, _lambdaRSW / 2, 1, NULL, NULL, NULL);
  BN_generate_prime_ex(q, _lambdaRSW / 2, 1, NULL, NULL, NULL);
  BN_mul(N, p, q, ctx);

  BN_mod(x, x, N, ctx);
#ifdef _DEBUG
  std::cout << "RSWPuzzle2 x:\t" << print_bn(x) << std::endl;
  std::cout << "RSWPuzzle2 T:\t" << print_bn(T) << std::endl;
  std::cout << "RSWPuzzle2 p:\t" << print_bn(p) << std::endl;
  std::cout << "RSWPuzzle2 q:\t" << print_bn(q) << std::endl;
  std::cout << "RSWPuzzle2 N:\t" << print_bn(N) << std::endl;
#endif
  BN_CTX_end(ctx);
  BN_CTX_free(ctx);
}

RSWPuzzle::RSWPuzzle(const RSWPuzzle &other) : t(other.t), lambda(other.lambda)
{
  N = BN_secure_new();
  x = BN_secure_new();
  T = BN_secure_new();
  BN_copy(N, other.N);
  BN_copy(x, other.x);
  BN_copy(T, other.T);
}

RSWPuzzle::~RSWPuzzle()
{
  BN_clear_free(T);
  BN_clear_free(x);
  BN_clear_free(N);
}

bytevec RSWPuzzle::get_N() const
{
  return bn2bytevec(N);
}

bytevec RSWPuzzle::get_x() const
{
  return bn2bytevec(x);
}

bytevec RSWPuzzle::get_T() const
{
  return bn2bytevec(T);
}

unsigned long RSWPuzzle::get_log2T() const
{
  return t;
}

unsigned long RSWPuzzle::get_lambda() const
{
  return lambda;
}
} // namespace vdf
