#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include "chip-8.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_timer.h>
#include <time.h>


emulated_platform chip8;

SDL_Window *window;
SDL_Texture *screen;
SDL_Renderer *render;


void errorMsg(const char *arg){

perror(arg);
exit(1);

}


void clean_SDL(){

SDL_DestroyTexture(screen);
SDL_DestroyRenderer(render);
SDL_DestroyWindow(window);
SDL_Quit();



}

void setupPlatform(emulated_platform chip8){

chip8.register_i = 0x0;

chip8.register_dt = 0x0;
chip8.register_st = 0x0;

chip8.register_pc = 0x200;
chip8.register_sp = 0x0;

memset(chip8.memory_space, 0x00, MEMORY_SIZE);
memset(chip8.stack, 0x00, 16);
memset(chip8.display_screen, 0x00, (SCREEN_HEIGHT * SCREEN_WIDTH));
memset(chip8.v_registers, 0x00, 16);
memset(chip8.keyboard, 0x00 , 16); 

memcpy(chip8.memory_space, font_set, 80 * sizeof(uint8_t));

}

void loadROM(char *file){

FILE *fp;


fp = fopen(file, "rb");

if(fp == NULL){

errorMsg("Error");

}

fseek(fp, 0, SEEK_END);

long size = ftell(fp);

fseek(fp, 0, SEEK_SET);


if(size > (MEMORY_SIZE - 512)){

errorMsg("Nedovoljno prostora");

}


fread(chip8.memory_space + 0x200, 1, size, fp);

fclose(fp);

}

void startFetching(){

uint8_t x, y, firstN, fourthN, key_pressed;



uint16_t opcode = ( chip8.memory_space[chip8.register_pc] << 8 ) | chip8.memory_space[chip8.register_pc + 1];

firstN = (opcode >> 12) & 0x0F;

x = ( opcode >> 8 ) & 0x0F;
y = ( opcode >> 4 ) & 0x0F;

fourthN = opcode & 0x0F;

uint8_t nn = opcode & 0xFF;
uint16_t nnn = opcode & 0xFFF;

chip8.register_pc+=2;

switch(opcode & 0xF000){

case 0x0000:

switch(opcode & 0x00FF){


case 0x00E0:
    flag = true;
    memset(chip8.display_screen, 0, SCREEN_HEIGHT * SCREEN_WIDTH);
    break;

case 0x00EE:
    chip8.register_sp--;
    chip8.register_pc = chip8.stack[chip8.register_sp];
    chip8.stack[chip8.register_sp] = 0x0;
    break;


}
break;

case 0x1000:
    chip8.register_pc = nnn;
    break;

case 0x2000:
    chip8.stack[chip8.register_sp] = chip8.register_pc;
    chip8.register_pc = nnn;
    chip8.register_sp++;
    break;

case 0x3000:
    if(chip8.v_registers[x] == nn) chip8.register_pc+=2;
    break;
    break;

case 0x4000:
    if(chip8.v_registers[x] != nn) chip8.register_pc+=2;
    break;

case 0x5000:
    if(chip8.v_registers[x] == chip8.v_registers[y]) chip8.register_pc+=2;
    break;

case 0x6000:
    chip8.v_registers[x] = nn;
    break;
    
case 0x7000:
    chip8.v_registers[x] += nn;

case 0x8000:
    switch(opcode & 0x000F){

    case 0x0000:
        chip8.v_registers[x] = chip8.v_registers[y];
        break;

    case 0x0001:
        chip8.v_registers[x] |= chip8.v_registers[y];
        break;
        
    case 0x0002:
        chip8.v_registers[x] &= chip8.v_registers[y];
        break;
    
    case 0x0003:
        chip8.v_registers[x] ^= chip8.v_registers[y];
        break;

    case 0x0004:{
        int i;
        i = (int)chip8.v_registers[x] + (int)chip8.v_registers[y];
        if(i > 255) 
        {

            chip8.v_registers[0xF] = 1;
        
        }
        else {
             chip8.v_registers[0xF] = 0;

        }
        chip8.v_registers[x] = i & 0xFF;
    }
    break;

    case 0x0005:{
    if( chip8.v_registers[x] > chip8.v_registers[y]){
        chip8.v_registers[0xF] = 1;
    }
    else {
        chip8.v_registers[0xF] = 0;
    }
    chip8.v_registers[x] -= chip8.v_registers[y];
    }
    break;


}

case 0x9000:
    if(chip8.v_registers[x] != chip8.v_registers[y]) chip8.register_pc += 2;
    break;

case 0xA000:
    chip8.register_i = nnn;
    break;

case 0xB000:
    chip8.register_pc = (nnn + chip8.v_registers[0]);
    break;

case 0xC000:{
    srand((unsigned int) time(NULL));
    uint8_t bajt = rand() % 256;
    chip8.v_registers[x] = (nn & bajt);
  
    }
      break;

case 0xD000:
    //kod
    break;

case 0xE000:{

switch(opcode & 0x00FF){

case 0x009E:
    if(chip8.keyboard[])
    //kod
    break;

case 0x00A1:
    //kod
    break;

}



}
break;

case 0xF000:{
        switch(opcode & 0x00FF){

            case 0x0007:
            chip8.v_registers[x] = chip8.register_dt;
            break;

            case 0x000A:
             key_pressed = 0;
            for(int i=0; i<16;i++){
                if( chip8.keyboard[i] ){
                    key_pressed = 1;
                    chip8.v_registers[x] = i;

                }
            }
            break;

            case 0x0015:
            chip8.register_dt = chip8.v_registers[x];
            break;

            case 0x0018:
            chip8.register_st = chip8.v_registers[x];
            break;


            case 0x001E:
            chip8.register_i += chip8.v_registers[x];
            break;

            case 0x0029:
            chip8.register_i = 5 * chip8.v_registers[x];
            break;

            case 0x0033:{
            uint8_t vX = chip8.v_registers[x];
                chip8.memory_space[chip8.register_i] = (vX - (vX % 100) ) / 100;
                vX -= chip8.memory_space[chip8.register_i] * 100;
                chip8.memory_space[chip8.register_i + 1] = (vX - (vX % 10)) / 10;
                vX -= chip8.memory_space[chip8.register_i] * 10;
                chip8.memory_space[chip8.register_i + 2] = vX;
            }
            
            break;
            
            case 0x0055:
            for(uint8_t i = 0; i < x; i++){
                chip8.memory_space[chip8.register_i + i] = chip8.v_registers[i];

            }
            break;

            case 0x0065:
            for(uint8_t i = 0; i < x; i++){
                chip8.v_registers[i] = chip8.memory_space[chip8.register_i + i];
            }
            break;
        
    }


}
    break;




}

}


int main(int argc, char *argv[]){

uint8_t stop = 0;

if(argc < 2){

    printf("Nije unet ROM program\n");
    return 0;
}

if( SDL_Init(SDL_INIT_EVERYTHING) != 0){

printf("Greska pri inicijalizaciji SDL-a %s\n", SDL_GetError());

}
SDL_Event event;

window = SDL_CreateWindow("CHIP8", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 320, 0);
render = SDL_CreateRenderer(window,-1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

SDL_RenderSetLogicalSize(render, 64, 32);
SDL_SetRenderDrawColor(render, 0, 0, 0, 255);
SDL_RenderClear(render);

screen = SDL_CreateTexture(render, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING,64,32);

setupPlatform(chip8);


loadROM(argv[1]);




while(!stop){

startFetching();

}


clean_SDL();
return 0;

}   