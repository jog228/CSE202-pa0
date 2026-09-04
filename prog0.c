#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
// Union to store 4 bytes as an array of bytes, an unsigned, signed, or float number
union value{
    unsigned uval;
    int sval;
    float fval;
    unsigned char bytes[4];
};
// converts the ASCII hex character c to binary
// returns the hex value of c if c is a valid hex digit, -1 otherwise
char hexDigit(char c) {
    if(c >= '0' && c <= '9')
        return c - '0';
    if(c >= 'a' && c <= 'f')
        return c - 'a' + 10;
    if(c >= 'A' && c <= 'F')
        return c - 'A' + 10;
    return -1;
}
// reads 8 hex characters from string input and stores it in the union v
// returns -1 if the hexadecimal number is invalid, 0 otherwise
int read_hex(union value *v, char *input){
    if(strlen(input) != 8)
        return -1;
    for(int i = 0; i < 4; i++){
        char high = hexDigit(input[2*i]);
        char low = hexDigit(input[2*i + 1]);
        if(high == -1 || low == -1)
            return -1;
        v->bytes[3 - i] = (high << 4) | low; // 2 4 bit nibbles
    }
    return 0;
}

// returns true if x has any even bit equal to 1, 0 otherwise
int any_even_one(unsigned x){
    return (x & 0x55555555) != 0;
}
// returns a mask indicating the position of the left most one in x
int leftmost_one(unsigned x);
// returns x shifted n positions to the left with the n most significant bits of x 
// inserted at the right of x
unsigned rotate_left(unsigned x, int n);
// returns x shifted n positions to the right with the n least significant bits of x 
// inserted at the left of x
unsigned rotate_right(unsigned x, int n);
// returns x+y if no overflow occurs
// returns TMAX if a positive overflow occurs
// returns TMIN if a negative overflow occurs
int saturating_add(int x, int y);
// multiplies the binary representation of a float number f by 2
unsigned float_twice(unsigned f);
// divides the binary representation of a float number f by 2
unsigned float_half(unsigned f);

int main(int argc, char** argv){
    if(argc != 3 && argc != 4){
        printf("Invalid number of arguments");
        exit(0);
    }
    char *op = argv[1];
    union value v;
    if(strcmp(op, "even") == 0){
        if(read_hex(&v, argv[2]) == -1){
            printf("Invalid hex value");
            exit(0);
        }
        if(any_even_one(v.uval))
            printf("True");
        else
            printf("False");
    }
    else{
        printf("Invalid operation");
    }
    return 0;
}
