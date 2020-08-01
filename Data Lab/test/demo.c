#include <stdio.h>
#include <stdlib.h>

void print_bin(int number);
void print_bin_simple(int number);

int main(void){
	int x = ~0x80000000 +1 ;
	print_bin(x);
	print_bin(0x80000000);
 	print_bin(0x7fffffff);
 	x= ~0 << 31;
 	print_bin(x);
 	print_bin(~0x7fffffff+1);
 	int min = ~0 << 31;
 	int max = ~0 ^ min;
 	print_bin(max);
    print_bin(0x35);
    print_bin(0x39);
    print_bin(0x31);
    print_bin(0x3f);
    print_bin(0x41);
  	print_bin(0x51);
    return 0;
}





void print_bin(int number){
    int bit = sizeof(int)*8;
    int i;
    for(i = bit - 1;i >= 0;i--){
        int bin = (number & (1 << i)) >> i;
        printf("%d", bin);
    }
    printf("\n");
}

// 二进制数格式输出，输出有效位，过滤掉前面的0位
void print_bin_simple(int number){
    int bit = sizeof(int)*8;
    int i;
    int status = 0;
    for(i = bit - 1;i >= 0;i--){
        int bin = (number & (1 << i)) >> i;
        if(bin == 1)
            status = 1;
        if(status == 1)
            printf("%d", bin);
    }
    printf("\n");
}