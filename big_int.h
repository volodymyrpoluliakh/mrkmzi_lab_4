#ifndef BIG_INT_H
#define BIG_INT_H


#include <stdint.h>
#include <string>


class big_int{
private:
   uint64_t* num;
   uint64_t size;
   uint64_t sign;
   bool fewArr (uint64_t* b, uint64_t bSize);
   uint64_t* addArr (uint64_t* b, uint64_t bSize);
   uint64_t* subArr (uint64_t* b, uint64_t bSize);
   uint64_t* mulArr (uint64_t* b, uint64_t bSize);
   uint64_t* divArr (uint64_t* b, uint64_t bSize);
   void andArr (uint64_t* b, uint64_t bSize);
   void mod2pow (uint64_t p);
   void updateSize ();
public:
   big_int();
   big_int(std::string str);
   big_int(uint64_t inNum);
   big_int(uint64_t* inArr, uint64_t inSize ,uint64_t inSign);
   big_int(big_int* a);
   ~big_int();
   bool equal(big_int* b);
   bool few(big_int* b);
   void add(big_int* b);
   void add(uint64_t b);
   bool addF(uint64_t b);
   void sub(big_int* b);
   void sub(uint64_t b);
   void mul(big_int* b);
   void mulMod(big_int* b, big_int* mod, big_int* mu);
   big_int* div(big_int* b);
//   big_int* divFast(big_int* b);
   uint64_t divFast0(big_int* b);
   uint64_t divFast1(big_int* b);
   uint64_t divFast2(big_int* b);
   uint64_t div(uint64_t b);
   uint64_t getRes(uint64_t b);
   void shl(int sSize);
   void shr(int sSize);
   void red(big_int* mod, big_int* mu);
   big_int* gcd(big_int* b);
   big_int* inverse(big_int* mod);
   void pow(big_int* p);
   void powMod(big_int* p, big_int* mod, big_int* mu);
   std::string show();
   std::string showNumHex();
   std::string showNumDec();
   uint64_t getSizeBits();
   big_int* getMu();
   uint8_t getByte();
   uint64_t getQWord();
   bool isZero();
   bool isEven();
};

#endif // BIG_INT_H
