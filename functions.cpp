#include <functions.h>
#include <big_int.h>
#include <time.h>
#include <iomanip>

bool testMillerRaben(big_int* p){
    if(p->isEven()) return false;

    big_int* d = new big_int(p);
    d->sub((uint64_t)1);
    big_int* pMinusOne = new big_int(d);
    uint64_t s = 0;

    while (d->isEven()){
        d->shr(1);
        s++;
    }

    srand(time(NULL));

    int k = 15;
    uint64_t lenP = p->getSizeBits();
    big_int* mu = p->getMu();
    big_int* one = new big_int((uint64_t)1);
    bool isPseudoPrime = 0;
    big_int* two = new big_int ((uint64_t) 2);
    while (k != -1) {
        isPseudoPrime = 0;
        big_int* testPrime = generateL89(lenP, 0);

        big_int* testPrime1 = new big_int(testPrime);
        if (!(testPrime1->gcd(p))->equal(one)) {
            delete testPrime;
            delete testPrime1;
            delete two;
            delete mu;
            delete one;
            delete pMinusOne;
            delete d;
            return false;
        }
        delete testPrime1;

        testPrime1 = new big_int(testPrime);
        testPrime1->powMod(d, p, mu);
        if(testPrime1->equal(pMinusOne) || testPrime1->equal(one)) {
            k--;
            delete testPrime1;
            delete testPrime;
            continue;
        }

        for(int i = 1; i < s; i++){
            testPrime1->powMod(two, p, mu);

            if(testPrime1->equal(pMinusOne)) {
                isPseudoPrime = 1;
                break;
            }
            if(testPrime1->equal(one)) {
                delete testPrime;
                delete testPrime1;
                delete two;
                delete mu;
                delete one;
                delete pMinusOne;
                delete d;
                return false;
            }
        }
        if(isPseudoPrime){
            delete testPrime;
            delete testPrime1;
            k--;
            continue;
        } else {
            delete testPrime;
            delete testPrime1;
            delete two;
            delete mu;
            delete one;
            delete pMinusOne;
            delete d;
            return false;
        }
    }
    return true;
}

big_int* generateL89(uint64_t size, bool isP){
    uint64_t* state = new uint64_t[2];
    state[0] = 0;
    state[1] = 0;

    for(int i = 0; i < 2; i++){
        for(int j = 0; j < 8; j++){
            uint64_t rd = getNumOfPrt();
            state[i] = (state[i] << 8) | (rd & 0xFF);
        }
    }

    uint64_t arrLen = 0;
    if (size % 64 == 0){
        arrLen = (size / 64);
    }
    else {
        arrLen = (size / 64) + 1;
    }
    uint64_t* l89 = new uint64_t[arrLen];

    uint64_t buf;
    for(int i = 0; i < 1000; i++){
        buf = (((state[0] >> 23) ^ (state[1] >> 37)) & 1);
        state[0] = (state[0] << 1) | (state[1] >> 63);
        state[1] = (state[1] << 1) | buf;
    }

    uint64_t len = size / 64;
    for(int i = 0; i < len; i++){
        for(int j = 0; j < 64; j++ ){
            buf = (((state[0] >> 23) ^ (state[1] >> 37)) & 1);
            state[0] = (state[0] << 1) | (state[1] >> 63);
            state[1] = (state[1] << 1) | buf;
        }
        l89[i] = state[0];
    }

    if(size % 64 != 0){
        l89[arrLen - 1] = 0;
        buf = 0;
        uint64_t lastDigit{};
        for(int i = 0; i < size % 64; i++){
            buf = (((state[0] >> 23) ^ (state[1] >> 37)) & 1);
            state[0] = (state[0] << 1) | (state[1] >> 63);
            state[1] = (state[1] << 1) | buf;
            lastDigit = (lastDigit << 1) | buf;
        }
        l89[arrLen - 1] = lastDigit;
    }

    if (isP){
        l89[arrLen - 1] = l89[arrLen - 1] | ((uint64_t) 1 << ((size % 64) - 1));
        l89[0] = l89[0] | 1;
    }
    big_int* intL89 = new big_int(l89, arrLen, 0);

    delete [] state;
    delete [] l89;

    return intL89;
}

uint64_t getNumOfPrt(){
   uint64_t res;
   asm __volatile__(
    "xor %%rax, %%rax\n\t"
    "rdtsc\n\t"
    "shl $32, %%rdx\n\t"
    "or %%rdx, %%rax\n\t"
    "mov %%rax, %0"
    : "=r" (res)
    :
    : "rdx"
   );
   return res;
}

big_int* generateTestPrime(uint64_t size, big_int* cand){

    while (true) {
        big_int* testPrime = new big_int(generateL89(size, 0));
        if(testPrime->isEven() || testPrime->isZero() || !testPrime->few(cand)){
            delete testPrime;
            continue;
        } else {
            return testPrime;
        }
    }
}

big_int* generatePrime(uint64_t size){
    big_int* p = new big_int (generateL89(size, 1));

    bool res = testMillerRaben(p);
    while(true){
        res = testMillerRaben(p);
        if (res){
            break;
        } else{
            delete p;
            p = new big_int (generateL89(size, 1));
        }
    }
//    cout << endl << p->showNumHex();
    return p;
}

big_int* generateP(uint64_t size){
    big_int* pHatch = generatePrime(size - 1);
    pHatch->shl(1);
    big_int* p = new big_int (pHatch);
    p->add((uint64_t) 1);
    big_int* i = new big_int ((uint64_t) 1);
    uint64_t one = 1;

    bool res = testMillerRaben(p);
    while (true){
        res = testMillerRaben(p);
        if (res){
            break;
        } else {
            delete p;
            i->add(one);
            p = new big_int(i);
            p->mul(pHatch);
            p->add(one);
        }
    }
    delete i;
    delete pHatch;

    return p;
}

big_int*** generateKeyPair(uint64_t size){
    big_int** publicKey = new big_int* [2];
    big_int** secretKey = new big_int* [3];

    secretKey[1] = generateP(size/2);
    secretKey[2] = generateP(size/2 + 1);

    publicKey[0] = new big_int(secretKey[1]);
    publicKey[0]->mul(secretKey[2]);
    publicKey[1] = new big_int((uint64_t) 65537);

    big_int* fi1 = new big_int(secretKey[1]);
    big_int* fi2 = new big_int(secretKey[2]);
    fi1->sub((uint64_t) 1);
    fi2->sub((uint64_t) 1);
    fi1->mul(fi2);
    secretKey[0] = publicKey[1]->inverse(fi1);

    big_int*** keyPair = new big_int** [2];
    keyPair[0] = secretKey;
    keyPair[1] = publicKey;

    return keyPair;
}

big_int*** generateSpecialKeyPair(uint64_t size){
    big_int* p = generateP(size/2);
    big_int* q = generateP(size/2);
    big_int* n = new big_int(q);
    n->mul(p);

    while (n->getSizeBits() <= size) {
        delete n;
        delete q;
        q = generateP(size/2);
        n->mul(p);
    }

    big_int* p1 = generateP(size/2);
    big_int* q1 = generateP(size/2);
    big_int* n1 = new big_int(q1);
    n1->mul(p1);

    while (n1->getSizeBits() <= size && n1->few(n)) {
        delete n1;
        delete q1;
        q1 = generateP(size/2);
        n1 = new big_int(q1);
        n1->mul(p1);
    }

    big_int*** pqp1q1 = new big_int** [2];
    pqp1q1[0] = new big_int* [2];
    pqp1q1[1] = new big_int* [2];
    pqp1q1[0][0] = p;
    pqp1q1[0][1] = q;
    pqp1q1[1][0] = p1;
    pqp1q1[1][1] = q1;

    return pqp1q1;
}

big_int* encrypt(big_int *M, big_int *e, big_int *n){
    big_int* C = new big_int(M);
    big_int* mu = n->getMu();

    C->powMod(e, n, mu);

    return C;
}

big_int* decrypt(big_int *C, big_int* n, big_int *d){
    big_int* M = new big_int(C);
    big_int* mu = n->getMu();

    M->powMod(d, n, mu);

    delete d;
    delete n;
    delete mu;

    return M;
}

big_int** sign(big_int *M, big_int *d, big_int *n){
    big_int** signPair = new big_int* [2];
    signPair[0] = new big_int(M);
    big_int* mu = n->getMu();
    M->powMod(d, n, mu);
    signPair[1] = new big_int(M);

    return signPair;
}

bool verify(big_int *M, big_int *S, big_int *e, big_int *n){
    big_int* mu = n->getMu();
    S->powMod(e, n, mu);

    if (S->equal(M)){
        return true;
    }
    else {
        return false;
    }
}

big_int** sendKey(big_int *d, big_int *n1, big_int *k, big_int *n){
    big_int* e1 = new big_int((uint64_t) 65537);
    while (true){
        if (!n1->few(n)) break;
        delete n;
        delete d;
        big_int*** keyPair = generateKeyPair(n1->getSizeBits()-5);
        n = keyPair[1][0];
        d = keyPair[0][0];
    }

    big_int* mu = n->getMu();
    big_int* mu1 = n1->getMu();

    big_int* S = new big_int(k);
    S->powMod(d, n, mu);
    big_int* S1 = new big_int(S);
    S1->powMod(e1, n1, mu1);
    k->powMod(e1, n1, mu1);


    big_int** ans = new big_int* [3];
    ans[0] = k;
    ans[1] = S1;
    ans[2] = n;

    return ans;
}

bool receiveKey(big_int *d1, big_int *n1, big_int *k1, big_int *S1, big_int *n, big_int *e){
    big_int* mu = n->getMu();
    big_int* mu1 = n1->getMu();
    cout << "k1 : " << k1->showNumHex() << endl;
    cout << "S1 : " << S1->showNumHex() << endl;
    k1->powMod(d1, n1, mu1);

    S1->powMod(d1, n1, mu1);
    cout << "S : " << S1->showNumHex() << endl;
    S1->powMod(e, n, mu);

    cout << "k : " << k1->showNumHex() << endl;
    if (S1->equal(k1)){
        return true;
    }
    else {
        return false;
    }
}
