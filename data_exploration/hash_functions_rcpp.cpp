#include <Rcpp.h>
#include <cstdint>  // pour uint32_t

// Hash function
uint32_t hash(uint32_t val) {
  val *= 9;
  val ^= val << 17;
  val ^= val << 5;
  val += val << 19;
  val &= 0x7fffffff;
  val ^= val >> 3;
  val *= 9;
  val &= 0x7fffffff;
  return val;
}

// Inverted hash function
uint32_t inverted_hash(uint32_t val) {
  val *= 954437177;
  val &= 0x7fffffff;
  val ^= val >> 3;
  val ^= val >> 6;
  val ^= val >> 12;
  val ^= val >> 24;
  val *= 2146959361;
  val ^= val << 5;
  val ^= val << 10;
  val ^= val << 20;
  val ^= val << 17;
  val &= 0x7fffffff;
  val *= 954437177;
  val &= 0x7fffffff;
  return val;
}

// Expose the functions to R using Rcpp
// [[Rcpp::export]]
uint32_t hash_R(uint32_t val) {
  return hash(val);
}

// [[Rcpp::export]]
uint32_t inverted_hash_R(uint32_t val) {
  return inverted_hash(val);
}
