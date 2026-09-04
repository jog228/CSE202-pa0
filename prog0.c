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
int leftmost_one(unsigned x){
    x |= (x >> 1);
    x |= (x >> 2);
    x |= (x >> 4);
    x |= (x >> 8);
    x |= (x >> 16);
    return x ^ (x >> 1);
}
// returns x shifted n positions to the left with the n most significant bits of x 
// inserted at the right of x
unsigned rotate_left(unsigned x, int n){
    return (x << n) | (x >> (32 - n));
}
// returns x shifted n positions to the right with the n least significant bits of x 
// inserted at the left of x
unsigned rotate_right(unsigned x, int n){
    return (x >> n) | (x << (32 - n));
}
// returns x+y if no overflow occurs
// returns TMAX if a positive overflow occurs
// returns TMIN if a negative overflow occurs
int saturating_add(int x, int y){
    int sum = x + y;
    int sign_x = x >> 31;
    int sign_y = y >> 31;
    int sign_sum = sum >> 31;
    int pos_overflow = (!sign_x) && (!sign_y) && sign_sum;
    int neg_overflow = sign_x && sign_y && (!sign_sum);
    if(pos_overflow)
        return INT_MAX;
    if(neg_overflow)
        return INT_MIN;
    return sum;
}
// multiplies the binary representation of a float number f by 2
unsigned float_twice(unsigned f){
    unsigned sign = f & 0x80000000;
    unsigned exp = (f >> 23) & 0xFF;
    unsigned mantissa = f & 0x7FFFFF;
    // infinity or NaN
    if(exp == 0xFF)
        return f;
    if(exp == 0){
        // denormalized
        mantissa <<= 1;
    }
    else{
        // normal
        exp++;
        if(exp == 0xFF)
            mantissa = 0;
    }
    return sign | (exp << 23) | mantissa;
}
// divides the binary representation of a float number f by 2
unsigned float_half(unsigned f){
    unsigned sign = f & 0x80000000;
    unsigned exp = (f >> 23) & 0xFF;
    unsigned mantissa = f & 0x7FFFFF;
    // infinity or NaN
    if(exp == 0xFF)
        return f;
    if(exp == 0){
        // denormalized
        mantissa >>= 1;
    }
    else if (exp == 1){
        mantissa = (mantissa | 0x800000) >> 1;
        exp = 0;
    }
    else{
        exp--;
    }
    return sign | (exp << 23) | mantissa;
}

int main(int argc, char** argv){
    if(argc != 3 && argc != 4){
        printf("Invalid number of arguments\n");
        exit(0);
    }
    char *op = argv[1];
    union value v;
    if(strcmp(op, "even") == 0){
        if(read_hex(&v, argv[2]) == -1){
            printf("Invalid hex value\n");
            exit(0);
        }
        if(any_even_one(v.uval))
            printf("True\n");
        else
            printf("False\n");
    }
    else if(strcmp(op, "lrotate") == 0){
        if(argc != 4){
            printf("Invalid number of arguments\n");
            exit(0);
        }
        if(read_hex(&v, argv[2]) == -1){
            printf("Invalid hex value\n");
            exit(0);
        }
        int n = atoi(argv[3]);
        if(n < 0 || n > 31){
            printf("Invalid number of shift positions\n");
            exit(0);
        }
        printf("%08x\n", rotate_left(v.uval, n));
    }
    else if(strcmp(op, "rrotate") == 0){
        if(argc != 4){
            printf("Invalid number of arguments\n");
            exit(0);
        }
        if(read_hex(&v, argv[2]) == -1){
            printf("Invalid hex value\n");
            exit(0);
        }
        int n = atoi(argv[3]);
        if(n < 0 || n > 31){
            printf("Invalid number of shift positions\n");
            exit(0);
        }
        printf("%08x\n", rotate_right(v.uval, n));
    }
    else if(strcmp(op, "left") == 0){
        if(read_hex(&v, argv[2]) == -1){
            printf("Invalid hex value\n");
            exit(0);
        }
        printf("%08x\n", leftmost_one(v.uval));
    }
    else if(strcmp(op, "saturate") == 0){
        if(argc != 4){
            printf("Invalid number of arguments\n");
            exit(0);
        }
        union value vx, vy;
        if(read_hex(&vx, argv[2]) == -1 || read_hex(&vy, argv[3]) == -1){
            printf("Invalid hex value\n");
            exit(0);
        }
        int result = saturating_add(vx.sval, vy.sval);
        printf("%08x %d\n", result, result);
    }
    else if(strcmp(op, "twice") == 0){
        if(read_hex(&v, argv[2]) == -1){
            printf("Invalid hex vaue\n");
            exit(0);
        }
        unsigned result = float_twice(v.uval);
        union value vr;
        vr.uval = result;
        printf("%08x %e\n", result, vr.fval);
    }
    else if(strcmp(op, "half") == 0){
        if(read_hex(&v, argv[2]) == -1){
            printf("Invalid hex value\n");
            exit(0);
        }
        unsigned result = float_half(v.uval);
        union value vr;
        vr.uval = result;
        printf("%08x %e\n", result, vr.fval);
    }
    else{
        printf("Invalid operation\n");
    }
    return 0;
}
