#include "big_int.h"
#include <stdint.h>
#include <wchar.h>
#include <iostream>

using namespace std;


big_int::big_int(){
   num = new uint64_t[1];
   num[0] = 0;
   sign = 0;
   size = 1;
}

big_int::big_int (std::string str){
    uint64_t strSize = 0;
    while(((str[strSize] > 47) && (str[strSize] < 58)) || ((str[strSize] > 64) && (str[strSize] < 71))){
        strSize += 1;
    }
    uint64_t arrSize = ((strSize - 1) / 16) + 1;
    num = new uint64_t[arrSize];
    uint8_t temp;
    for (int i = 0; i < (arrSize - 1); i++){
        num[i] = 0;
        for (int j = 0; j < 16; j++){
            temp = str[strSize - ((16 * i) + j) - 1];
            if (temp < 58){
                num[i] |= ((uint64_t)(temp - 48) << (4 * j));
            } else {
                num[i] |= ((uint64_t)(temp - 55) << (4 * j));
            }
        }
    }
    num[arrSize - 1] = 0;
    for (int i = 0; i < (strSize - 16 * (arrSize - 1)); i++){
        temp = str[(strSize - 16 * (arrSize - 1)) - i - 1];
        if (temp < 58){
            num[arrSize - 1] |= ((uint64_t)(temp - 48) << (4 * i));
        } else {
            num[arrSize - 1] |= ((uint64_t)(temp - 55) << (4 * i));
        }
    }
    sign = 0;
    size = arrSize;
    updateSize ();
}

big_int::big_int (uint64_t inNum){
    num = new uint64_t[1];
    num[0] = inNum;
    size = 1;
    sign = 0;
}

big_int::big_int (uint64_t* inArr, uint64_t inSize, uint64_t inSign) {
    size = inSize;
    num = new uint64_t[size];
    for (int i = 0; i < size; i++){
        num[i] = inArr[i];
    }
    sign = inSign;
    updateSize ();
}

big_int::big_int (big_int* b){
    num = new uint64_t[b -> size];
    for (int i = 0; i < b -> size; i++){
        num[i] = b -> num[i];
    }
    size = b -> size;
    sign = b -> sign;
    return;
}

big_int::~big_int (){
    delete num;
}

bool big_int::equal (big_int* b){
    if (size != b -> size){
        updateSize ();
        b -> updateSize ();
        if (size != b -> size){
            return false;
        }
    }
    for (int i = 0; i < size; i++){
        if (num[i] != b -> num[i]){
            return false;
        }
    }
    return true;
}

bool big_int::few (big_int* b){
    if (sign){
        if (b -> sign){
            return !(fewArr (b -> num, b -> size));
        } else {
            return true;
        }
    } else {
        if (b -> sign){
            return false;
        } else {
            return fewArr (b -> num, b -> size);
        }
    }
}

void big_int::add (big_int* b){
    uint64_t* res;
    if (sign){
        if (b -> sign){
            if (size < b -> size){
                res = b -> addArr (num, size);
                size = b -> size + 1;
            } else {
                res = this -> addArr (b -> num, b -> size);
                size += 1;
            }
        } else {
            if (b -> fewArr (num, size)){
                res = this -> subArr (b -> num, b -> size);
            } else {
                res = b -> subArr (num, size);
                size = b -> size;
                sign = 0;
            }
        }
    } else {
        if (b -> sign){
            if (this -> fewArr (b -> num, b -> size)){
                res = b -> subArr (num, size);
                size = b -> size;
                sign = 1;
            } else {
                res = this -> subArr (b -> num, b -> size);
            }
        } else {
            if (size < b -> size){
                res = b -> addArr (num, size);
                size = b -> size + 1;
            } else {
                res = this -> addArr (b -> num, b -> size);
                size += 1;
            }
        }
    }
    delete num;
    num = res;
    while ((num[size - 1] == 0) && (size > 1)){
        size -= 1;
    }
    return;
}

void big_int::add (uint64_t b){
    bool carry = 0;
    __asm__ __volatile__(
        "mov %0, %%rax\n\t"
        "add %1, %%rax\n\t"
        "jnc LOOP%=\n\t"
        "movq $1, %2\n\t"
        "LOOP%=:"
        :
        : "m" (num[0]), "m" (b), "m" (carry)
        : "%rax"
    );
    if (carry){
        uint64_t* bArr = new uint64_t[1];
        bArr[0] = b;
        big_int* bNum = new big_int(bArr, 1, 0);
        delete bArr;
        this -> add (bNum);
        delete bNum;
    } else {
        num[0] += b;
    }
    return;
}

bool big_int::addF (uint64_t b){
    uint8_t carry = 0;
    __asm__ __volatile__(
        "mov %0, %%rax\n\t"
        "add %1, %%rax\n\t"
        "jnc LOOP%=\n\t"
        "movq $1, %2\n\t"
        "LOOP%=:"
        :
        : "m" (num[0]), "m" (b), "m" (carry)
        : "%rax"
    );
    if (carry){
        uint64_t* bArr = new uint64_t[1];
        bArr[0] = b;
        big_int* bNum = new big_int(bArr, 1, 0);
        delete bArr;
        this -> add (bNum);
        delete bNum;
        return true;
    }
    num[0] += b;
    return false;
}

void big_int::sub (big_int* b){
    uint64_t* res;
    if (sign){
        if (b -> sign){
            if (b -> fewArr (num, size)){
                res = this -> subArr (b -> num, b -> size);
            } else {
                res = b -> subArr (num, size);
                size = b -> size;
                sign = 0;
            }
        } else {
            if (size < b -> size){
                res = b -> addArr (num, size);
                size = b -> size + 1;
            } else {
                res = this -> addArr (b -> num, b -> size);
                size += 1;
            }
        }
    } else {
        if (b -> sign){
            if (size < b -> size){
                res = b -> addArr (num, size);
                size = b -> size + 1;
            } else {
                res = this -> addArr (b -> num, b -> size);
                size += 1;
            }
        } else {
            if (this -> fewArr (b -> num, b -> size)){
                res = b -> subArr (num, size);
                size = b -> size;
                sign = 1;
            } else {
                res = this -> subArr (b -> num, b -> size);
            }
        }
    }
    delete num;
    num = res;
    while ((num[size - 1] == 0) && (size > 1)){
        size -= 1;
    }
    return;
}

void big_int::sub (uint64_t b){
    bool carry = 0;
    __asm__ __volatile__(
        "mov %0, %%rax\n\t"
        "sub %1, %%rax\n\t"
        "jnc LOOP%=\n\t"
        "movq $1, %2\n\t"
        "LOOP%=:"
        :
        : "m" (num[0]), "m" (b), "m" (carry)
        : "%rax"
    );
    if (carry){
        uint64_t* bArr = new uint64_t[1];
        bArr[0] = b;
        big_int* bNum = new big_int(bArr, 1, 0);
        delete bArr;
        this -> sub (bNum);
        delete bNum;
    } else {
        num[0] -= b;
    }
    return;
}

void big_int::mul (big_int* b){
    if (sign ^ b -> sign){
        sign = 1;
    } else {
        sign = 0;
    }
    uint64_t* res;
    res = this -> mulArr (b -> num, b -> size);
    delete num;
    num = res;
    size += b -> size;
    while ((num[size - 1] == 0) && (size > 1)){
        size -= 1;
    }
    return;
}

void big_int::mulMod (big_int* b, big_int* mod, big_int* mu){
    if (sign || (getSizeBits () >= mod -> getSizeBits ())){
        red(mod, mu);
    }
    big_int* originalB = 0;
    if (b -> sign || (b -> getSizeBits () >= mod -> getSizeBits ())){
        originalB = b;
        b = new big_int(originalB);
        b -> red (mod, mu);
    }
    uint64_t* newNum = mulArr (b -> num, b -> size);
    delete num;
    num = newNum;
    size += b -> size;
    updateSize ();
    red(mod, mu);
    if (originalB != 0){
        delete b;
        b = originalB;
    }
    return;
}

big_int* big_int::div (big_int* b){
    if (sign ^ b -> sign){
        sign = 1;
    } else {
        sign = 0;
    }
    while (((b -> size - 1) == 0) && (b -> size > 1)){
        b -> size -= 1;
    }
    if (this -> fewArr(b -> num, b -> size)){
        big_int* res = new big_int(num, size, sign);
        num[0] = 0;
        size = 1;
        return res;
    }
    uint64_t* qArr = this -> divArr(b -> num, b -> size);
    big_int* q = new big_int(qArr, size - b -> size + 1, sign);
    q -> mul(b);
    big_int* r = new big_int (this);
    r -> sub(q);
    delete q;
    delete num;
    num = qArr;
    size = size - b -> size + 1;
    while ((num[size - 1] == 0) && (size > 1)){
        size -= 1;
    }
    return r;
}

//big_int* big_int::divFast (big_int* b){
//	uint64_t qhq = fabiDivStep2 (num, size, b -> num, b -> size);
//	big_int* q = new big_int (qhq);
//	q -> shl (64 * (size - b -> size));
//	return q;
//}



uint64_t big_int::div (uint64_t b){
    uint64_t r;
    __asm__ __volatile__(
        "mov %0, %%r8\n\t"
        "mov %1, %%rbx\n\t"
        "mov %2, %%rcx\n\t"
        "xor %%rdx, %%rdx\n\t"
        "LOOP%=:\n\t"
        "mov -8(%%r8, %%rcx, 8), %%rax\n\t"
        "div %%rbx\n\t"
        "mov %%rax, -8(%%r8, %%rcx, 8)\n\t"
        "dec %%rcx\n\t"
        "jnz LOOP%=\n\t"
        "mov %%rdx, %3"
        :
        : "m" (num), "m" (b), "m" (size), "m" (r)
        : "%rax", "%rbx", "%rcx", "%rdx", "%r8"
    );
    return r;
}

uint64_t big_int::getRes (uint64_t b){
    uint64_t r;
    __asm__ __volatile__(
        "mov %0, %%r8\n\t"
        "mov %1, %%rbx\n\t"
        "mov %2, %%rcx\n\t"
        "xor %%rdx, %%rdx\n\t"
        "LOOP%=:\n\t"
        "mov -8(%%r8, %%rcx, 8), %%rax\n\t"
        "div %%rbx\n\t"
        "dec %%rcx\n\t"
        "jnz LOOP%=\n\t"
        "mov %%rdx, %3"
        :
        : "m" (num), "m" (b), "m" (size), "m" (r)
        : "%rax", "%rbx", "%rcx", "%rdx", "%r8"
    );
    return r;
}

void big_int::shl (int sSize){
    if (sSize % 64){
        uint64_t arrSize = getSizeBits () + sSize;
        arrSize = ((arrSize - 1) / 64) + 1;
        if (arrSize == size){
            for (int i = 0; i < size - 1; i++){
                num[size - i - 1] = (num[size - i - 1] << sSize) | (num[size - i - 2] >> (64 - sSize));
            }
            num[0] = num[0] << sSize;
            return;
        }
        uint64_t sb = sSize % 64;
        uint64_t sw = sSize / 64;
        uint64_t* res = new uint64_t[arrSize + 1];
        for (int i = 0; i <= arrSize; i++){
            res[i] = 0;
        }
        for (int i = 0; i < size; i++){
            res[size - i + sw] |= num[size - i - 1] >> (64 - sb);
        }
        for (int i = 0; i < size; i++){
            res[size - i + sw - 1] |= num[size - i - 1] << sb;
        }
        delete num;
        num = res;
        size = arrSize;
    } else {
        sSize /= 64;
        uint64_t* res = new uint64_t[size + sSize];
        for (int i = 0; i < sSize; i++){
            res[i] = 0;
        }
        for (int i = 0; i < size; i++){
            res[i + sSize] = num[i];
        }
        delete num;
        num = res;
        size += sSize;
    }
    return;
}

void big_int::shr (int sSize){
    if (getSizeBits () <= sSize){
        num[0] = 0;
        size = 1;
        return;
    }
    if (sSize % 64){
        uint64_t arrSize = getSizeBits () - sSize;
        arrSize = ((arrSize - 1) / 64) + 1;
        uint64_t sb = sSize % 64;
        uint64_t sw = sSize / 64;
        for (int i = 0; i < arrSize; i++){
            if ((i + sw + 1) < size){
                num[i] = (num[i + sw + 1] << (64 - sb)) | (num[i + sw] >> sb);
            } else {
                num[i] = num[i + sw] >> sb;
                break;
            }
        }
        size = arrSize;
        if ((size > 1) && (num[size - 1] == 0)){
            size -= 1;
        }
    } else {
        sSize /= 64;
        for (int i = 0; i < (size - sSize); i++){
            num[i] = num[i + sSize];
        }
        size -= sSize;
        if (size == 0){
            size = 1;
        }
    }
    return;
}

void big_int::red (big_int* mod, big_int* mu) {
    if (size < mod -> size){
        if (sign){
            add (mod);
        }
        return;
    }
    uint64_t numSize = getSizeBits ();
    uint64_t modSize = mod -> getSizeBits ();
    if (numSize < modSize){
        if (sign){
            add (mod);
        }
        return;
    }
    if ((numSize - modSize) < 2){
        if (fewArr (mod -> num, mod -> size)){
            if (sign){
                add (mod);
            }
            return;
        }
        big_int* dMod = new big_int(mod);
        dMod -> shl (1);
        if (fewArr (dMod -> num, dMod -> size)){
            if (sign){
                add (dMod);
            } else {
                sub (mod);
            }
            delete dMod;
            return;
        }
        delete dMod;
    }
    big_int* q = new big_int(this);
    q -> shr (modSize - 1);
    q -> mul (mu);
    q -> shr (modSize + 1);
    q -> mul (mod);
    q -> mod2pow (modSize + 1);
    mod2pow (modSize + 1);
    sub (q);
    delete q;
    if (sign){
        big_int* m = new big_int (1);
        m -> shl (modSize + 1);
        add (m);
        delete m;
    }
    while (!fewArr (mod -> num, mod -> size)){
        sub (mod);
    }
    return;
}

big_int* big_int::gcd (big_int* b){
    if (isZero () || b -> isZero ()){
        big_int* res = new big_int();
        return res;
    }
    big_int* x = new big_int(this);
    big_int* y = new big_int(b);
    if (x -> few (y)){
        big_int* swap = x;
        x = y;
        y = swap;
    }
    big_int* g = new big_int(1);
    while (x -> isEven () && y -> isEven ()){
        x -> shr (1);
        y -> shr (1);
        g -> shl (1);
    }
    big_int* t;
    while (!x -> isZero ()){
        while (x -> isEven ()){
            x -> shr (1);
        }
        while (y -> isEven ()){
            y -> shr (1);
        }
        t = new big_int(x);
        t -> sub (y);
        t -> sign = 0;
        t -> shr (1);
        if (x -> few(y)){
            delete y;
            y = t;
        } else {
            delete x;
            x = t;
        }
    }
    y -> mul (g);
    delete x;
    delete g;
    return y;
}

big_int* big_int::inverse (big_int* mod){
    big_int* x = new big_int (mod);
    big_int* y = new big_int (this);
    big_int* g = new big_int (1);
    while (x -> isEven () && y -> isEven ()){
        x -> shr (1);
        y -> shr (1);
        g -> shl (1);
    }
    big_int* u = new big_int (x);
    big_int* v = new big_int (y);
    big_int* a = new big_int (1);
    big_int* b = new big_int ();
    big_int* c = new big_int ();
    big_int* d = new big_int (1);
    do {
        while (u -> isEven ()){
            u -> shr (1);
            if (a -> isEven () && b -> isEven ()){
                a -> shr (1);
                b -> shr (1);
            } else {
                a -> add (y);
                a -> shr (1);
                b -> sub (x);
                b -> shr (1);
            }
        }
        while (v -> isEven ()){
            v -> shr (1);
            if (c -> isEven () && d -> isEven ()){
                c -> shr (1);
                d -> shr (1);
            } else {
                c -> add (y);
                c -> shr (1);
                d -> sub (x);
                d -> shr (1);
            }
        }
        if (u -> few (v)){
            v -> sub (u);
            c -> sub (a);
            d -> sub (b);
        } else {
            u -> sub (v);
            a -> sub (c);
            b -> sub (d);
        }
    } while (!u -> isZero ());
    v -> mul (g);
    if (d -> sign){
        d -> add (mod);
    }
    delete x;
    delete y;
    delete g;
    delete a;
    delete b;
    delete c;
    delete u;
    if (v -> getSizeBits () == 1){
        delete v;
        return d;
    } else {
        delete v;
        delete d;
        return new big_int ();
    }
}

void big_int::pow (big_int* p){
    big_int* t = new big_int(this);
    num[0] = 1;
    size = 1;
    if (p -> isEven ()){
        sign = 0;
    }
    for (int i = 0; i < p -> size; i++){
        for (int j = 0; j < 64; j++){
            if ((p -> num[i] >> j) & 1){
                mul (t);
            }
            t -> mul (t);
        }
    }
    delete t;
    return;
}

void big_int::powMod (big_int* p, big_int* mod, big_int* mu){
    red (mod, mu);
    big_int* t = new big_int(this);
    num[0] = 1;
    size = 1;
    sign = 0;
    for (int i = 0; i < p -> size; i++){
        for (int j = 0; j < 64; j++){
            if ((p -> num[i] >> j) & 1){
                mul (t);
                red (mod, mu);
            }
            t -> mul (t);
            t -> red (mod, mu);
        }
    }
    delete t;
    return;
}

std::string big_int::show (){
    std::string res;
    if (sign){
        res = "- ";
    }
    uint64_t t;
    for (int i = 0; i < size; i++){
        for (int j = 0; j < 16; j++){
            t = (num[size - i - 1] >> (60 - (4 * j))) & 0xF;
            if (t < 10){
                res += (char)(t + 48);
            } else {
                res += (char)(t + 55);
            }
        }
        res += " ";
    }
    res += "\n";
    return res;
}

std::string big_int::showNumHex (){
    updateSize ();
    std::string res;
    if ((size == 1) && (num[0] == 0)){
        res = "0\n";
        return res;
    }
    if (sign){
        res = "-";
    }
    uint64_t t;
    bool begin = true;
    for (int j = 0; j < 16; j++){
        t = (num[size - 1] >> (60 - (4 * j))) & 0xF;
        if (begin && (t == 0)){
            continue;
        }
        if (t < 10){
            res += (char)(t + 48);
        } else {
            res += (char)(t + 55);
        }
        begin = false;
    }
    for (int i = 1; i < size; i++){
        for (int j = 0; j < 16; j++){
            t = (num[size - i - 1] >> (60 - (4 * j))) & 0xF;
            if (t < 10){
                res += (char)(t + 48);
            } else {
                res += (char)(t + 55);
            }
        }
    }
    res += "\n";
    return res;
}

std::string big_int::showNumDec (){
    updateSize ();
    std::string res;
    if ((size == 1) && (num[0] == 0)){
        res = "0\n";
        return res;
    }
    std::string temp;
    uint8_t cur;
    big_int* d = new big_int(this);
    while (!d -> isZero ()){
        cur = (uint8_t)(d -> div (10));
        temp += (char)(cur + 48);
    }
    if (sign){
        res = "-";
    }
    int strSize = temp.size () - 1;
    for (int i = strSize; i >= 0; i--){
        res += temp[i];
    }
    res += "\n";
    return res;
}

uint64_t big_int::getSizeBits (){
    if (size == 0) return 0;
    size -= 1;
    while ((num[size] == 0) && (size != 0)){
        size -= 1;
    }
    uint64_t res = 0;
    while (((num[size] >> res) != 0) && (res < 64)){
        res += 1;
    }
    res += 64 * size;
    size += 1;
    return res;
}

big_int* big_int::getMu (){
    std::string s = "1";
    big_int* res = new big_int(s);
    res -> shl (2 * getSizeBits ());
    res -> div (this);
    return res;
}

uint8_t big_int::getByte (){
    return (uint8_t)num[0];
}

uint64_t big_int::getQWord (){
    return num[0];
}

bool big_int::fewArr (uint64_t* b, uint64_t bSize){
    if (size < bSize){
        return true;
    }
    if (size == bSize){
        if (num[size - 1] < b[bSize - 1]){
            return true;
        } else {
            for (int i = (size - 1); i > 0; i--){
                if (num[i] == b[i]){
                    if (num[i - 1] < b[i - 1]){
                        return true;
                    }
                } else {
                    break;
                }
            }
        }
    }
    return false;
}

uint64_t* big_int::addArr (uint64_t* b, uint64_t bSize){
    uint64_t* res = new uint64_t[size + 1];
    __asm__ __volatile__(
        "mov %0, %%r8\n\t"
        "mov %1, %%r9\n\t"
        "mov %2, %%r10\n\t"
        "mov %4, %%rcx\n\t"
        "mov $1, %%r11\n\t"
        "xor %%rbx, %%rbx\n\t"
        "ML%=:\n\t"
        "xor %%rdx, %%rdx\n\t"
        "mov (%%r9), %%rax\n\t"
        "add (%%r10), %%rax\n\t"
        "cmovc %%r11, %%rdx\n\t"
        "add %%rbx, %%rax\n\t"
        "cmovc %%r11, %%rdx\n\t"
        "mov %%rax, (%%r8)\n\t"
        "mov %%rdx, %%rbx\n\t"
        "add $8, %%r8\n\t"
        "add $8, %%r9\n\t"
        "add $8, %%r10\n\t"
        "dec %%rcx\n\t"
        "jnz ML%=\n\t"
        "mov %3, %%rcx\n\t"
        "sub %4, %%rcx\n\t"
        "jz EL%=\n\t"
        "AL%=:\n\t"
        "xor %%rdx, %%rdx\n\t"
        "mov (%%r9), %%rax\n\t"
        "add %%rbx, %%rax\n\t"
        "cmovc %%r11, %%rdx\n\t"
        "mov %%rax, (%%r8)\n\t"
        "mov %%rdx, %%rbx\n\t"
        "add $8, %%r8\n\t"
        "add $8, %%r9\n\t"
        "dec %%rcx\n\t"
        "jnz AL%=\n\t"
        "EL%=:\n\t"
        "mov %0, %%r8\n\t"
        "mov %3, %%rcx\n\t"
        "mov %%rbx, (%%r8, %%rcx, 8)"
        :
        : "m" (res), "m" (num), "m" (b), "m" (size), "m" (bSize)
        : "%rax", "%rbx", "%rcx", "%rdx", "%r8", "%r9", "%r10", "%r11"
    );
    return res;
}

uint64_t* big_int::subArr (uint64_t* b, uint64_t bSize){
    uint64_t* res = new uint64_t[size];
    __asm__ __volatile__(
        "mov %0, %%r8\n\t"
        "mov %1, %%r9\n\t"
        "mov %2, %%r10\n\t"
        "mov %4, %%rcx\n\t"
        "mov $1, %%r11\n\t"
        "xor %%rbx, %%rbx\n\t"
        "ML%=:\n\t"
        "xor %%rdx, %%rdx\n\t"
        "mov (%%r9), %%rax\n\t"
        "sub (%%r10), %%rax\n\t"
        "cmovc %%r11, %%rdx\n\t"
        "sub %%rbx, %%rax\n\t"
        "cmovc %%r11, %%rdx\n\t"
        "mov %%rax, (%%r8)\n\t"
        "mov %%rdx, %%rbx\n\t"
        "add $8, %%r8\n\t"
        "add $8, %%r9\n\t"
        "add $8, %%r10\n\t"
        "dec %%rcx\n\t"
        "jnz ML%=\n\t"
        "mov %3, %%rcx\n\t"
        "sub %4, %%rcx\n\t"
        "jz EL%=\n\t"
        "AL%=:\n\t"
        "xor %%rdx, %%rdx\n\t"
        "mov (%%r9), %%rax\n\t"
        "sub %%rbx, %%rax\n\t"
        "cmovc %%r11, %%rdx\n\t"
        "mov %%rax, (%%r8)\n\t"
        "mov %%rdx, %%rbx\n\t"
        "add $8, %%r8\n\t"
        "add $8, %%r9\n\t"
        "dec %%rcx\n\t"
        "jnz AL%=\n\t"
        "EL%=:\n\t"
        :
        : "m" (res), "m" (num), "m" (b), "m" (size), "m" (bSize)
        : "%rax", "%rbx", "%rcx", "%rdx", "%r8", "%r9", "%r10", "%r11"
    );
    return res;
}

uint64_t* big_int::mulArr (uint64_t* b, uint64_t bSize){
    uint64_t* res = new uint64_t[size + bSize];
    for (int i = 0; i < (size + bSize); i++){
        res[i] = 0;
    }
    for (int i = 0; i < (bSize); i++){
        __asm__ __volatile__ (
            "lea %0, %%r8\n\t"
            "mov %1, %%r9\n\t"
            "mov %3, %%r10\n\t"
            "mov %2, %%r11\n\t"
            "xor %%rcx, %%rcx\n\t"
            "xor %%rbx, %%rbx\n\t"
            "ML%=:\n\t"
            "mov (%%r9, %%rcx, 8), %%rax\n\t"
            "mul %%r10\n\t"
            "add %%rbx, %%rax\n\t"
            "jnc FL%=\n\t"
            "inc %%rdx\n\t"
            "FL%=:\n\t"
            "add (%%r8, %%rcx, 8), %%rax\n\t"
            "jnc SL%=\n\t"
            "inc %%rdx\n\t"
            "SL%=:\n\t"
            "mov %%rax, (%%r8, %%rcx, 8)\n\t"
            "mov %%rdx, %%rbx\n\t"
            "inc %%rcx\n\t"
            "cmp %%rcx, %%r11\n\t"
            "jne ML%=\n\t"
            "mov %%rbx, (%%r8, %%rcx, 8)"
            :
            : "m" (res[i]), "m" (num), "m" (size), "m" (b[i]), "m" (i)
            : "%rax", "%rbx", "%rcx", "%rdx", "%r8", "%r9", "%r10", "%r11"
        );
    }
    return res;
}

uint64_t* big_int::divArr (uint64_t* b, uint64_t bSize){
    uint64_t rSize = size - bSize + 1;
    uint64_t* res = new uint64_t[rSize];
    for (int i = 0; i < (rSize); i++){
        res[i] = 0;
    }
    uint64_t* cmp = new uint64_t[size + 1];
    for (int i = 0; i < (rSize); i++){
        for (int j = 0; j < 64; j++){
            res[rSize - i - 1] |= (uint64_t)1 << (63 - j);
            for (int k = 0; k <= size; k++){
                cmp[k] = 0;
            }
            for (int k = 0; k < bSize; k++){
                __asm__ __volatile__ (
                    "lea %0, %%r8\n\t"
                    "mov %1, %%r9\n\t"
                    "mov %3, %%r10\n\t"
                    "mov %2, %%r11\n\t"
                    "xor %%rcx, %%rcx\n\t"
                    "xor %%rbx, %%rbx\n\t"
                    "ML%=:\n\t"
                    "mov (%%r9, %%rcx, 8), %%rax\n\t"
                    "mul %%r10\n\t"
                    "add %%rbx, %%rax\n\t"
                    "jnc FL%=\n\t"
                    "inc %%rdx\n\t"
                    "FL%=:\n\t"
                    "add (%%r8, %%rcx, 8), %%rax\n\t"
                    "jnc SL%=\n\t"
                    "inc %%rdx\n\t"
                    "SL%=:\n\t"
                    "mov %%rax, (%%r8, %%rcx, 8)\n\t"
                    "mov %%rdx, %%rbx\n\t"
                    "inc %%rcx\n\t"
                    "cmp %%rcx, %%r11\n\t"
                    "jne ML%=\n\t"
                    "mov %%rbx, (%%r8, %%rcx, 8)"
                    :
                    : "m" (cmp[k]), "m" (res), "m" (rSize), "m" (b[k]), "m" (k)
                    : "%rax", "%rbx", "%rcx", "%rdx", "%r8", "%r9", "%r10", "%r11"
                );
            }
            if (cmp[size]){
                res[rSize - i - 1] ^= (uint64_t)1 << (63 - j);
            } else if (this -> fewArr(cmp, size)){
                res[rSize - i - 1] ^= (uint64_t)1 << (63 - j);
            }
        }
    }
    return res;
}

//uint64_t* big_int::divArrFast (uint64_t* b, uint64_t bSize){
//	uint64_t* qArr = new uint64_t[size - bSize];
//	for (int i = 0; i < size - bSize; i++){
//		qArr[i] = 0;
//	}
//
//}

void big_int::andArr (uint64_t* b, uint64_t bSize){
    if (size > bSize){
        size = bSize;
    }
    for (int i = 0; i < size; i++){
        num[i] &= b[i];
    }
    return;
}

void big_int::mod2pow (uint64_t p){
    if (p == 0){
        num[0] = 0;
        size = 1;
        return;
    }
    uint64_t numSize = getSizeBits ();
    if (p >= numSize){
        return;
    }
    uint64_t pb = p % 64;
    uint64_t pw = p / 64;
    if (pb == 0){
        size = pw;
        return;
    }
    num[pw] &= UINT64_MAX >> (64 - pb);
    size = pw + 1;
    updateSize ();
    return;
}

void big_int::updateSize (){
    if (size == 0){
        delete num;
        num = new uint64_t[1];
        num[0] = 0;
        size = 1;
        sign = 0;
    } else {
        while (size > 1){
            if (num[size - 1] == 0){
                size -= 1;
            } else {
                break;
            }
        }
    }
    return;
}

bool big_int::isZero (){
    if (size == 1){
        if (num[0] == 0){
            return true;
        }
    } else {
        updateSize ();
        if (size == 1){
            if (num[0] == 0){
                return true;
            }
        }
    }
    return false;
}

bool big_int::isEven (){
    if ((num[0] & 1) == 0){
        return true;
    }
    return false;
}

