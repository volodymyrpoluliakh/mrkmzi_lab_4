#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <iostream>
#include <string>
#include "big_int.h"

using namespace std;

big_int* generateL89(uint64_t size, bool isP);
big_int* generatePrimeNumber(int minSize);
bool testMillerRaben(big_int* p);
bool determineTest(big_int* cand);
big_int* generateTestPrime(uint64_t size);
big_int* generatePrime(uint64_t size);
big_int* generateP(uint64_t size);
big_int** generateTwoKeyPairs(uint64_t size);
uint64_t getNumOfPrt();

big_int* encrypt(big_int* M, big_int* e, big_int* n);
big_int* decrypt(big_int* C, big_int *n, big_int* d);
big_int*** generateKeyPair(uint64_t size);
big_int*** generateSpecialKeyPair(uint64_t size);
big_int** sign(big_int* M, big_int* d, big_int* n);
bool verify(big_int* M, big_int* S, big_int* e, big_int* n);
bool receiveKey(big_int* d1, big_int* n1, big_int* k1, big_int* S1, big_int* n, big_int* e);
big_int** sendKey(big_int* d, big_int* n1, big_int* k, big_int* n);

#endif // FUNCTIONS_H
