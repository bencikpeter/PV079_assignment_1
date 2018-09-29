//
//  main.cpp
//  Pseudorandom generator
//
//  Created by Peter Bencik on 21/09/2018.
//  Copyright © 2018 Peter Bencik. All rights reserved.
//

#include <iostream>
#include <string.h>
#include <iostream>
#include <fstream>
#include <vector>
#include "aescpp.h"

class Generator {
private:
    static const size_t seedSize = 16;
    static const size_t blockSize = seedSize;
    unsigned char seed[seedSize];
    unsigned char key[blockSize];
    unsigned char DT[blockSize];
    AESencrypt encryptor;
public:
    Generator(const unsigned char seed[16]){
        for (size_t i = 0; i < seedSize; ++i) {
            this->seed[i] = seed[i];
            key[i] = seed[seedSize-i-1];
            DT[i] = 0;
        }
        encryptor.key128(key);
    }
    
    //generates 128bits of pseudorandom data
    std::vector<unsigned char> generate(){
        
        //obtain nonce I
        unsigned char I[blockSize];
        encryptor.encrypt(DT, I);
        
        //xor nonce with current seed
        unsigned char xorVal[blockSize];
        for (int i = 0; i<blockSize; ++i) {
            xorVal[i] = I[i]^seed[i];
        }
        
        //obtain random value;
        unsigned char random[blockSize];
        encryptor.encrypt(xorVal, random);
        
        //derive new seed
        unsigned char xorSeed[blockSize];
        for (int i = 0; i<blockSize; ++i) {
            xorSeed[i] = random[i]^I[i];
        }
        encryptor.encrypt(xorSeed, seed);
        
        //fresh DT vector for next iteration
        incrementDT();
        
        return std::vector<unsigned char>(random, random + blockSize);
    }
    
private:
    //incremets date-time vector, when overflow, reset to zero
    void incrementDT(){
        for (int i = blockSize-1; i >=0; --i) {
            if (DT[i] == UCHAR_MAX) {
                DT[i] = 0;
                continue;
            }
            DT[i]+=1;
            break;
        }
    }
};

int main(int argc, const char * argv[]) {
    
    unsigned char uco[] = {4,3,3,8,0,7,4,3,3,8,0,7,4,3,3,8};
    
    //test uco
    //unsigned char uco[] = {2,3,2,8,8,6,2,3,2,8,8,6,2,3,2,8};
    
    Generator gen(uco);
    
    std::ofstream outfile ("F.bin",std::ios::out | std::ios::binary);
    
    for (int i = 0; i <7812500; ++i) {
        auto generated = gen.generate();
        outfile.write((char*)&generated[0], generated.size() * sizeof(unsigned char));
    }
    outfile.close();
    
    //for exercise 5, lenght of output should be 125000bytes - but why generatin 160000 is explained in discussion forum
    std::ofstream outfile2 ("ex5.bin",std::ios::out | std::ios::binary);
    
    for (int i = 0; i <10000; ++i) {
        auto generated = gen.generate();
        outfile2.write((char*)&generated[0], generated.size() * sizeof(unsigned char));
    }
    
    
    return 0;
}
