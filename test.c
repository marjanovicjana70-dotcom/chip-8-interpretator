#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
int main(){
srand(time(NULL));

uint8_t random = rand() % 256;

uint8_t nekibr = 10;


uint8_t odabir = random & nekibr;

uint8_t br1 = 20;
uint8_t br2 = 10;

if(br1 + br2 == 30){

    br1 = -br2;
}
printf("%hu - broj\n", br1);



}