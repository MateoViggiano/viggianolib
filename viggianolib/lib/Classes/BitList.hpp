#pragma once
#include"../arrays.hpp"
namespace mpv{
    class BitList COUNT_IT{
        static constexpr unsigned char BITS[8]{
            0b00000001,
            0b00000010,
            0b00000100,
            0b00001000,
            0b00010000,
            0b00100000,
            0b01000000,
            0b10000000,
        };
        private:
            long maxSize=0;
            ssize_t bit_count=0;
            long size=0;
            unsigned char* bits=nullptr;
        public:
            BitList(){}
            BitList(long maxSize):maxSize(maxSize),bits(maxSize>0 ? new unsigned char[maxSize] : nullptr){}
            BitList(const BitList& other):maxSize(other.maxSize),bit_count(other.bit_count),size(other.size),bits(maxSize>0 ? new unsigned char[maxSize] : nullptr){
                dflt::memcopy(this->bits,other.bits,size);
            }
            BitList(BitList&& other):maxSize(other.maxSize),bit_count(other.bit_count),size(other.size),bits(other.bits){
                other.maxSize=0;other.bit_count=0;other.size=0;other.bits=nullptr;
            }
            BitList& operator=(const BitList& other){
                if(other.size>this->maxSize){
                    unsigned char* new_bits=new unsigned char[other.maxSize];
                    dflt::memcopy(new_bits,other.bits,other.size);
                    delete[] this->bits;
                    this->bits=new_bits;
                    this->maxSize=other.maxSize;
                }
                else
                    dflt::memcopy(this->bits,other.bits,other.size);
                this->size=other.size;
                this->bit_count=other.bit_count;
                return *this;
            }
            BitList& operator=(BitList&& other){
                delete[] this->bits;
                this->bit_count=other.bit_count;
                this->maxSize=other.maxSize;                
                this->bits=other.bits;
                this->size=other.size;
                other.bits=nullptr;
                other.bit_count=0;
                other.maxSize=0;
                other.size=0;
                return *this;
            }
            void append(bool bit){
                long bit_index=bit_count++%8;
                if(bit_index==0) size++;
                if(size>maxSize){
                    if(maxSize==0)
                        bits=new unsigned char[maxSize=20];
                    else{
                        unsigned char* new_bits=new unsigned char[maxSize=size*3];
                        dflt::memcopy(new_bits,bits,size);
                        delete[] bits;
                        bits=new_bits;                        
                    }
                }                
                if(bit==1)
                    bits[size-1] |= BITS[bit_index];
                else
                    bits[size-1] &= ~BITS[bit_index];
            }
            ~BitList(){
                delete[] bits;
            }
            inline long get_size()const{
                return size;
            }
            inline ssize_t get_bit_count()const{
                return bit_count;
            }
            inline unsigned char& operator[](long index)const{
                return bits[index];
            }
#if defined(_GLIBCXX_IOSTREAM) || defined(_IOSTREAM_)
            void print(){
                std::cout   <<"maxSize: "<<maxSize<<std::endl
                            <<"bit_count: "<<bit_count<<std::endl
                            <<"size: "<<size<<std::endl;
                std::cout<<'[';

                for(long i=0;i<size-1;i++){
                    bool byte[8]={};
                    unsigned char n=bits[i];
                    signed char I=0;
                    while(n>0){
                        byte[I++]=n%2;
                        n/=2;
                    }
                    I=0;
                    while(I<8){
                        std::cout<<byte[I++];
                    }
                    std::cout<<' ';
                }

                unsigned char remaining_bits = bit_count > 0 ? (unsigned char)(8-(size*8-bit_count)) : 0;
                
                for(unsigned char i=0,last_byte;i<remaining_bits;i++){
                    last_byte=bits[size-1];
                    last_byte&= BITS[i];
                    std::cout<<static_cast<bool>(last_byte);
                }std::cout<<']'<<(short)remaining_bits<<std::endl;
            }
#endif
    };
#if defined(_GLIBCXX_IOSTREAM) || defined(_IOSTREAM_)
    std::ostream& operator<<(std::ostream& stream,const BitList& bits){
        stream<<bits.get_bit_count();
        for(long i=0;i<bits.get_size();i++)
            stream<<bits[i];
        return stream;
    }
#endif
}