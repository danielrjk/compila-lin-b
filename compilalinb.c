/* Daniel Rodrigues Jacinto Kielmanowicz - 2310703 - 3WC */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "compilalinb.h"

/*
FORMATOS POSSÍVEIS
func	::=	cmd '\n' | cmd '\n' func
cmd	::=	att | desvio | ret
att	::=	varp '<=' expr
expr	::=	varpc op varpc
varp	::=	'v' num | 'p' num
varpc	::=	varp | '$' snum
op	::=	'+' | '-' | '*'
ret	::=	'ret'
desvio	::=	'if' varp num
num	::=	digito | digito num
snum	::=	[-] num
digito	::=	0' | '1' | '2' | '3' | '4' | '5' | '6' | '7'| '8' | '9'
*/


funcp compilaLinB(FILE *f, unsigned char codigo[])
{
  int c;
  int idx0, idx1, idx2, num;
  int reg_index_0, reg_index_1, reg_index_2;
  char var0, var1, var2, op, c0;
  unsigned char *inicio_codigo = codigo;
  int size = 0;
  int line_addresses[50] = {0};
  int current_line = 0;

  // Enumeração dos opcodes para instruções MOV
  enum
  {
    MOVREG2REG = 0x89, // Opcode para mov reg, reg
    MOVNUM2REG = 0xC7, // Opcode para mov $num, reg
    RET = 0xc3,        // Opcode para ret
    ADDREG2REG = 0x01, // Opcode para add reg, reg
    ADDNUM2REG = 0x83, // Opcode para add $num, reg
    SUBREG2REG = 0x29,
    SUBNUM2REG = 0x83,
    MULREG2REG1 = 0x0F,
    MULREG2REG2 = 0xAF,
    MULNUM2REG = 0x6B,
    CMPREG2REG = 0x39,
    CMPNUM2REG = 0x83,
    JMP = 0xe9,
    JNE1 = 0x0f,
    JNE2 = 0x85,
  };

  // Mapeamento dos registradores para índices
  enum
  {
    V1 = 0, // %rax
    V2,     // %rbx
    V3,     // %rcx
    V4,     // %rdx
    P1,     // %rdi
    P2      // %rsi
  };

  // Matriz 2D contendo os códigos de máquina em formato hexadecimal
  const unsigned char reg_matrix[6][6] = {
      {0x00, 0xc3, 0xc1, 0xc2, 0xc7, 0xc6}, // v1 -> %rax
      {0xd8, 0x00, 0xd9, 0xda, 0xdf, 0xde}, // v2 -> %rbx
      {0xc8, 0xcb, 0x00, 0xca, 0xcf, 0xce}, // v3 -> %rcx
      {0xd0, 0xd3, 0xd1, 0x00, 0xd7, 0xd6}, // v4 -> %rdx
      {0xf8, 0xfb, 0xf9, 0xfa, 0x00, 0xfe}, // p1 -> %rdi
      {0xf0, 0xf3, 0xf1, 0xf2, 0xf7, 0x00}  // p2 -> %rsi
  };
  // Mapeamento de valores numéricos para registradores
  const unsigned char num_to_reg[6] = {0xc0, 0xc3, 0xc1, 0xc2, 0xc7, 0xc6};

  // Mapeamento de valores numéricos para registradores para sub
  const unsigned char num_to_reg_sub[6] = {0xe8, 0xeb, 0xe9, 0xea, 0xef, 0xee};

  // Matriz 2D contendo os códigos de máquina em formato hexadecimal para imul
  const unsigned char imul_reg_matrix[6][6] = {
      {0x00, 0xd8, 0xc8, 0xd0, 0xf8, 0xf0}, // v1 -> %rax
      {0xc3, 0x00, 0xcb, 0xd3, 0xfb, 0xf3}, // v2 -> %rbx
      {0xc1, 0xd9, 0x00, 0xd1, 0xf9, 0xf1}, // v3 -> %rcx
      {0xc2, 0xda, 0xca, 0x00, 0xfa, 0xf2}, // v4 -> %rdx
      {0xc7, 0xdf, 0xcf, 0xd7, 0x00, 0xf7}, // p1 -> %rdi
      {0xc6, 0xde, 0xce, 0xd6, 0xfe, 0x00}  // p2 -> %rsi
  };
  // Mapeamento de valores numéricos para registradores para imul
  const unsigned char num_to_reg_imul[6] = {0xc0, 0xdb, 0xc9, 0xd2, 0xff, 0xf6};

  // Mapeamento de valores numéricos para registradores para cmp
  const unsigned char num_to_reg_cmp[6] = {0xf8, 0xfb, 0xf9, 0xfa, 0xff, 0xfe};

  while ((c = fgetc(f)) != EOF)
  {
    switch (c)
    {
    case 'r':
      if (fscanf(f, "et%c", &c0) != 1)
      {
        exit(1);
      }
      *codigo++ = RET;
      size++;
      break;
    case 'v':
    case 'p':
      var0 = c;
      if (fscanf(f, "%d <= %c%d %c %c%d", &idx0, &var1, &idx1, &op, &var2, &idx2) != 6)
      {
        exit(1);
      }
      if (var1 == '$' && var2 == '$')
      {
        switch (op)
        {
        case '+':
          num = idx1 + idx2;
          break;
        case '-':
          num = idx1 - idx2;
          break;
        case '*':
          num = idx1 * idx2;
          break;
        }
        *codigo++ = 0x48;
        *codigo++ = MOVNUM2REG;
        size += 2;

        reg_index_0 = 0;
        if (var0 == 'p')
        {
          reg_index_0 += 4;
        }
        reg_index_0 += idx0 - 1;

        *codigo++ = num_to_reg[reg_index_0];
        size++;

        unsigned char bytes[4];
        bytes[0] = (num >> 0) & 0xFF;
        bytes[1] = (num >> 8) & 0xFF;
        bytes[2] = (num >> 16) & 0xFF;
        bytes[3] = (num >> 24) & 0xFF;

        for (int i = 0; i < 4; i++)
        {
          *codigo++ = bytes[i];
          size++;
        }
      }
      else
      {
        if (var1 == 'v' || var1 == 'p')
        {
          reg_index_0 = 0;
          if (var0 == 'p')
          {
            reg_index_0 += 4;
          }
          reg_index_0 += idx0 - 1;

          reg_index_1 = 0;
          if (var1 == 'p')
          {
            reg_index_1 += 4;
          }
          reg_index_1 += idx1 - 1;

          if (!(idx0 == idx1 && var0 == var1))
          {
            *codigo++ = 0x48;
            *codigo++ = MOVREG2REG;
            *codigo++ = reg_matrix[reg_index_1][reg_index_0];
            size += 3;
          }

          if (var2 == 'v' || var2 == 'p')
          {
            reg_index_2 = 0;
            if (var2 == 'p')
            {
              reg_index_2 += 4;
            }
            reg_index_2 += idx2 - 1;

            switch (op)
            {
            case '+':
              *codigo++ = 0x48;
              *codigo++ = ADDREG2REG;
              *codigo++ = reg_matrix[reg_index_2][reg_index_0];
              size += 3;
              break;
            case '-':
              *codigo++ = 0x48;
              *codigo++ = SUBREG2REG;
              *codigo++ = reg_matrix[reg_index_2][reg_index_0];
              size += 3;
              break;
            case '*':
              *codigo++ = 0x48;
              *codigo++ = MULREG2REG1;
              *codigo++ = MULREG2REG2;
              *codigo++ = imul_reg_matrix[reg_index_2][reg_index_0];
              size += 4;
              break;
            }
          }
          else if (var2 == '$')
          {
            switch (op)
            {
            case '+':
              *codigo++ = 0x48;
              *codigo++ = ADDNUM2REG;
              *codigo++ = num_to_reg[reg_index_0];
              size += 3;
              num = idx2;
              *codigo++ = num;
              size++;
              break;
            case '-':
              *codigo++ = 0x48;
              *codigo++ = SUBNUM2REG;
              *codigo++ = num_to_reg_sub[reg_index_0];
              size += 3;
              num = idx2;
              *codigo++ = num;
              size++;
              break;
            case '*':
              *codigo++ = 0x48;
              *codigo++ = MULNUM2REG;
              *codigo++ = num_to_reg_imul[reg_index_0];
              size += 3;
              num = idx2;
              *codigo++ = num;
              size++;
              break;
            }
          }
        }
      }

      break;
    case 'i':
      if (fscanf(f, "f %c%d %d", &var0, &idx0, &num) != 3)
      {
        exit(1);
      }

      // 1. Determine o registrador da variável
      reg_index_0 = 0;
      if (var0 == 'p')
      {
        reg_index_0 += 4;
      }
      reg_index_0 += idx0 - 1;

      *codigo++ = 0x48;
      *codigo++ = CMPNUM2REG;
      *codigo++ = num_to_reg_cmp[reg_index_0];
      *codigo++ = 0x00; // Comparar com zero
      size += 4;

      *codigo++ = JNE1;
      *codigo++ = JNE2;
      *codigo++ = num; // linha para pular
      *codigo++ = 0x00;
      *codigo++ = 0x00;
      *codigo++ = 0x00; 
      size += 6;

      break;
    }

    line_addresses[current_line + 1] = size;
    current_line++;
    fscanf(f, " ");
  }

  for (int i = 0; i < current_line; i++)
  {
    int current_line_address = line_addresses[i];

    if (inicio_codigo[current_line_address + 4] == JNE1 &&
        inicio_codigo[current_line_address + 5] == JNE2)
    {
      int jne_offset_position = current_line_address + 6;
      int line_to_jump = inicio_codigo[jne_offset_position];
      int target_addres = line_addresses[line_to_jump];
      int offset = target_addres - (current_line_address + 16);

      inicio_codigo[jne_offset_position] = offset & 0xFF;
      inicio_codigo[jne_offset_position + 1] = (offset >> 8) & 0xFF;
      inicio_codigo[jne_offset_position + 2] = (offset >> 16) & 0xFF;
      inicio_codigo[jne_offset_position + 3] = (offset >> 24) & 0xFF;
    }
  }

  return (funcp)inicio_codigo;
};