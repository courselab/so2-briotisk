/*
 *    SPDX-FileCopyrightText: 2021 Monaco F. J. <monaco@usp.br>
 *    SPDX-FileCopyrightText: 2024 briotisk <briotisk@gmail.com>
 *    SPDX-FileCopyrightText: 2024 FelipeCecato <fececato31@gmail.com>
 *   
 *    SPDX-License-Identifier: GPL-3.0-or-later
 *
 *  This file is a derivative work from SYSeg (https://gitlab.com/monaco/syseg)
 *  and contains modifications carried out by the following author(s):
 *  briotisk <briotisk@gmail.com>
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define ERRO_ALOCACAO printf("Erro ao alocar memoria!\n");
#define ERRO    printf("Falha no processamento do arquivo.\n");
#define ERRO_ARQUIVO    printf("Erro na abertura do arquivo.\n\n");
#define LINE_MAX_LENGHT 64
#define moval   0xb0
#define movah   0xb4
#define intop   0xcd
#define hltop   0xebf4
#define jmpop   0x00fd
#define word    0xaa55

void trim(char *line) {

    char *line_start = line;
    char *line_end = line + strlen(line) - 1;

    while (*line_start == ' ' || *line_start == '\t') 
        line_start++;
    
    while (line_end > line_start && (*line_end == ' ' || *line_end == '\t')) 
        line_end--;

    *(line_end + 1) = '\0';

    strcpy(line, line_start);

}

void remove_comments(char *line) {

    while (*line != '\0' && *line != '#' && *line != '\n')
        line++;

    if(*line == '#' || *line == '\n')
        *line = '\0';    

}

int main(int argc, char const *argv[]) {

    //aloca espaço para a variável que vai salvar o nome do arquivo de entrada
    char* source_filename = malloc(sizeof(char)*strlen(argv[1]));
    if(source_filename == NULL) {//imprime uma mensagem de erro caso a alocação falhe
        ERRO_ALOCACAO
        return -1; 
    }

    //copia o nome do arquivo e abre em modo leitura
    strcpy(source_filename, argv[1]);
    FILE *source_file;
    source_file = fopen(source_filename, "r");
    if(source_file == NULL){//imprime uma mensagem de erro caso a abertura do arquivo falhe
        ERRO_ARQUIVO
        return -1;
    }

    //aloca espaço para a variável que vai salvar o nome do arquivo de saída
    source_filename = strtok(source_filename, ".");
    size_t output_filename_length = strlen(source_filename) + 5;
    char* output_filename = malloc(sizeof(char)*output_filename_length);
    strcpy(output_filename, source_filename);
    strcat(output_filename, ".bin");
    if(output_filename == NULL) {//imprime uma mensagem de erro caso a alocação falhe
        ERRO_ALOCACAO
        return -1; 
    }

    //cria o arquivo de saída 
    FILE *output_file;
    output_file = fopen(output_filename, "w");
    if(output_file == NULL){//imprime uma mensagem de erro caso a abertura do arquivo falhe
        ERRO_ARQUIVO
        return -1;
    }

    //aloca espaço para a variável que vai salvar uma linha lida do arquivo de entrada
    char* line = malloc(sizeof(char)*LINE_MAX_LENGHT);
    if(line == NULL) {//imprime uma mensagem de erro caso a alocação falhe
        ERRO_ALOCACAO
        return -1; 
    }
    const char *line_ptr = line;

    uint16_t opcode = 0;
    char *instruction;

    while (fgets(line, LINE_MAX_LENGHT, source_file)){

        remove_comments(line);
        trim(line);
        if(strlen(line) > 1){

            instruction = strtok(line, " ");

            if(!strcmp(instruction, "mov")){

                
                instruction = strtok(NULL, ",");
                char *arg_mov = instruction;

                instruction = strtok(NULL, " ");
                if(!strcmp(instruction, "\%al")){

                    arg_mov += 2;//ignora '$'
                    char c = *arg_mov;
                    opcode = (c << 8) | moval;

                }else {

                    arg_mov++;//ignora '$'
                    opcode = (uint8_t) strtol(arg_mov, NULL, 16);
                    opcode = (opcode << 8) | movah;

                }

                fwrite(&opcode, sizeof(uint16_t), 1, output_file);

            }else if(!strcmp(instruction, "hlt")) { 

                opcode = hltop;
                fwrite(&opcode, sizeof(uint16_t), 1, output_file);

            }else if(!strcmp(instruction, "int")) { 

                instruction = strtok(NULL, " ");
                instruction++;//igrora o '$1
                opcode = (uint8_t) strtol(instruction, NULL, 16);
                opcode = (opcode << 8) | intop;
                fwrite(&opcode, sizeof(uint16_t), 1, output_file);
                
            }else if(!strcmp(instruction, "jmp")) { 

                opcode = jmpop;
                fwrite(&opcode, sizeof(uint16_t), 1, output_file);

            }else if(!strcmp(instruction, ".fill")) { 

                int pos = ftell(output_file);
                unsigned char *buffer = (unsigned char *)calloc(510 - pos, sizeof(unsigned char));
                if (buffer == NULL) {
                    ERRO_ALOCACAO
                    return 1;
                }
                fwrite(buffer, sizeof(unsigned char), 510 - pos, output_file);

            }else if(!strcmp(instruction, ".word")) { 

                opcode = word;
                fwrite(&opcode, sizeof(uint16_t), 1, output_file);
            
            }
    
        }
        
    }

    //libera memória 
    free(source_filename);
    free(output_filename);
    free(line);

    //fecha o arquivo
    fclose(output_file);
    fclose(source_file);
    return 0;

}