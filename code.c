#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

void crash(int a1, const char *a2)
{
  printf("Machine CRASHED due to: %s\n", a2);
  sys_exit(a1, 1);
}

int sys_open(int a1, const char *a2, int a3)
{
  return open(a2, a3);
}

ssize_t sys_read(int a1, int a2, void *a3, size_t a4)
{
  return read(a2, a3, a4);
}

ssize_t sys_write(int a1, int a2, const void *a3, size_t a4)
{
  return write(a2, a3, a4);
}

void sys_exit(int a1, int a2)
{
  exit(a2);
}

int sys_sleep(int a1, int a2)
{
  return sleep(a2);
}

int *describe_register(char a1)
{
  switch ( a1 )
  {
    case 16:
      return aAbcdsif;
    case 32:
      return &aAbcdsif[1];
    case 2:
      return &aAbcdsif[2];
    case 1:
      return &aAbcdsif[3];
    case 8:
      return &aAbcdsif[4];
    case 4:
      return &aAbcdsif[5];
    case 64:
      return &aAbcdsif[6];
  }
  if ( a1 )
    return (int *)"?";
  return (int *)"NONE";
}

const char *describe_instruction(char a1)
{
  if ( (a1 & 1) != 0 )
    return "imm";
  if ( (a1 & 2) != 0 )
    return "add";
  if ( (a1 & 8) != 0 )
    return "stk";
  if ( (a1 & 0x40) != 0 )
    return "stm";
  if ( a1 < 0 )
    return "ldm";
  if ( (a1 & 0x20) != 0 )
    return "cmp";
  if ( (a1 & 0x10) != 0 )
    return "jmp";
  if ( (a1 & 4) != 0 )
    return "sys";
  return "???";
}

_BYTE *describe_flags(char a1)
{

  v2 = 0;
  if ( (a1 & 2) != 0 )
  {
    flag_description[0] = 76;
    v2 = 1;
  }
  if ( (a1 & 4) != 0 )
    flag_description[v2++] = 71;
  if ( (a1 & 1) != 0 )
    flag_description[v2++] = 69;
  if ( (a1 & 0x10) != 0 )
    flag_description[v2++] = 78;
  if ( (a1 & 8) != 0 )
    flag_description[v2++] = 90;
  if ( !a1 )
    flag_description[v2++] = 42;
  flag_description[v2] = 0;
  return flag_description;
}

int read_register(int *a1, char a2)
{
  switch ( a2 )
  {
    case 16:
      return a1[1024];
    case 32:
      return a1[1025];
    case 2:
      return a1[1026];
    case 1:
      return a1[1027];
    case 8:
      return a1[1028];
    case 4:
      return a1[1029];
  }
  if ( a2 != 64 )
    crash((int)a1, "unknown register");
  return a1[1030];
}

_BYTE *write_register(_BYTE *a1, char a2, char a3)
{

  switch ( a2 )
  {
    case 16:
      result = a1;
      a1[1024] = a3;
      break;
    case 32:
      result = a1;
      a1[1025] = a3;
      break;
    case 2:
      result = a1;
      a1[1026] = a3;
      break;
    case 1:
      result = a1;
      a1[1027] = a3;
      break;
    case 8:
      result = a1;
      a1[1028] = a3;
      break;
    case 4:
      result = a1;
      a1[1029] = a3;
      break;
    case 64:
      result = a1;
      a1[1030] = a3;
      break;
    default:
      crash((int)a1, "unknown register");
  }
  return result;
}

int read_memory(int a1, int a2)
{
  return *(int *)(a1 + a2 + 768);
}

int write_memory(int a1, int a2, char a3)
{

  result = a2;
  *(_BYTE *)(a1 + a2 + 768) = a3;
  return result;
}

_BYTE *interpret_imm(_BYTE *a1, int a2)
{

  v2 = describe_register(SBYTE1(a2));
  printf("[s] IMM %s = %#hhx\n", (const char *)v2, BYTE2(a2));
  return write_register(a1, SBYTE1(a2), SBYTE2(a2));
}

_BYTE *interpret_add(int *a1, int a2)
{

  v2 = describe_register(SBYTE2(a2));
  v3 = describe_register(SBYTE1(a2));
  printf("[s] ADD %s %s\n", (const char *)v3, (const char *)v2);
  LOBYTE(v2) = read_register(a1, SBYTE1(a2));
  v4 = read_register(a1, SBYTE2(a2));
  return write_register(a1, SBYTE1(a2), (int)v2 + v4);
}

int *interpret_stk(int *a1, int a2)
{

  v2 = describe_register(SBYTE2(a2));
  v3 = describe_register(SBYTE1(a2));
  printf("[s] STK %s %s\n", (const char *)v3, (const char *)v2);
  if ( BYTE2(a2) )
  {
    v4 = describe_register(SBYTE2(a2));
    printf("[s] ... pushing %s\n", (const char *)v4);
    ++a1[1028];
    v5 = read_register(a1, SBYTE2(a2));
    write_memory((int)a1, a1[1028], v5);
  }
  result = (int *)BYTE1(a2);
  if ( BYTE1(a2) )
  {
    v7 = describe_register(SBYTE1(a2));
    printf("[s] ... popping %s\n", (const char *)v7);
    memory = read_memory((int)a1, a1[1028]);
    write_register(a1, SBYTE1(a2), memory);
    result = a1;
    --a1[1028];
  }
  return result;
}

int interpret_stm(int *a1, int a2)
{

  v2 = describe_register(SBYTE2(a2));
  v3 = describe_register(SBYTE1(a2));
  printf("[s] STM *%s = %s\n", (const char *)v3, (const char *)v2);
  LOBYTE(v2) = read_register(a1, SBYTE2(a2));
  v4 = read_register(a1, SBYTE1(a2));
  return write_memory((int)a1, v4, (char)v2);
}

_BYTE *interpret_ldm(int *a1, int a2)
{

  v2 = describe_register(SBYTE2(a2));
  v3 = describe_register(SBYTE1(a2));
  printf("[s] LDM %s = *%s\n", (const char *)v3, (const char *)v2);
  v4 = read_register(a1, SBYTE2(a2));
  memory = read_memory((int)a1, v4);
  return write_register(a1, SBYTE1(a2), memory);
}

int *interpret_cmp(int *a1, int a2)
{

  v2 = describe_register(SBYTE2(a2));
  v3 = describe_register(SBYTE1(a2));
  printf("[s] CMP %s %s\n", (const char *)v3, (const char *)v2);
  v5 = read_register(a1, SBYTE1(a2));
  v6 = read_register(a1, SBYTE2(a2));
  a1[1030] = 0;
  if ( v5 < v6 )
    a1[1030] |= 2u;
  if ( v5 > v6 )
    a1[1030] |= 4u;
  if ( v5 == v6 )
    a1[1030] |= 1u;
  result = (int *)v5;
  if ( v5 != v6 )
  {
    result = a1;
    a1[1030] |= 0x10u;
  }
  if ( !v5 && !v6 )
  {
    result = a1;
    a1[1030] |= 8u;
  }
  return result;
}

int interpret_jmp(int *a1, int a2)
{

  v2 = describe_register(SBYTE2(a2));
  v3 = describe_flags(SBYTE1(a2));
  printf("[j] JMP %s %s\n", v3, (const char *)v2);
  if ( BYTE1(a2) && (BYTE1(a2) & a1[1030]) == 0 )
    return puts("[j] ... NOT TAKEN");
  puts("[j] ... TAKEN");
  result = read_register(a1, SBYTE2(a2));
  a1[1029] = result;
  return result;
}

int interpret_sys(int *a1, int a2)
{

  v2 = describe_register(SBYTE2(a2));
  printf("[s] SYS %#hhx %s\n", BYTE1(a2), (const char *)v2);
  if ( (a2 & 0x1000) != 0 )
  {
    puts("[s] ... open");
    v3 = sys_open((int)a1, (const char *)&a1[a1[1024] + 768], a1[1025]);
    write_register(a1, SBYTE2(a2), v3);
  }
  if ( (a2 & 0x100) != 0 )
  {
    puts("[s] ... read_code");
    v4 = a1[1026];
    if ( 3LL * (256 - a1[1025]) <= v4 )
      v4 = 3LL * (256 - a1[1025]);
    v5 = sys_read((int)a1, a1[1024], &a1[3 * a1[1025]], v4);
    write_register(a1, SBYTE2(a2), v5);
  }
  if ( (a2 & 0x400) != 0 )
  {
    puts("[s] ... read_memory");
    v6 = a1[1026];
    if ( 256 - a1[1025] <= v6 )
      LOBYTE(v6) = -a1[1025];
    v7 = sys_read((int)a1, a1[1024], &a1[a1[1025] + 768], (int)v6);
    write_register(a1, SBYTE2(a2), v7);
  }
  if ( (a2 & 0x200) != 0 )
  {
    puts("[s] ... write");
    v8 = a1[1026];
    if ( 256 - a1[1025] <= v8 )
      LOBYTE(v8) = -a1[1025];
    v9 = sys_write((int)a1, a1[1024], &a1[a1[1025] + 768], (int)v8);
    write_register(a1, SBYTE2(a2), v9);
  }
  if ( (a2 & 0x800) != 0 )
  {
    puts("[s] ... sleep");
    v10 = sys_sleep((int)a1, a1[1024]);
    write_register(a1, SBYTE2(a2), v10);
  }
  if ( (a2 & 0x2000) != 0 )
  {
    puts("[s] ... exit");
    sys_exit((int)a1, a1[1024]);
  }
  result = BYTE2(a2);
  if ( BYTE2(a2) )
  {
    v12 = (int)read_register(a1, SBYTE2(a2));
    v13 = describe_register(SBYTE2(a2));
    return printf("[s] ... return value (in register %s): %#hhx\n", (const char *)v13, v12);
  }
  return result;
}

int interpret_instruction(int *a1, int a2)
{

  printf(
    "[V] a:%#hhx b:%#hhx c:%#hhx d:%#hhx s:%#hhx i:%#hhx f:%#hhx\n",
    a1[1024],
    a1[1025],
    a1[1026],
    a1[1027],
    a1[1028],
    a1[1029],
    a1[1030]);
  printf("[I] op:%#hhx arg1:%#hhx arg2:%#hhx\n", (int)a2, BYTE1(a2), BYTE2(a2));
  if ( (a2 & 1) != 0 )
    interpret_imm(a1, a2);
  if ( (a2 & 2) != 0 )
    interpret_add(a1, a2);
  if ( (a2 & 8) != 0 )
    interpret_stk(a1, a2);
  if ( (a2 & 0x40) != 0 )
    interpret_stm(a1, a2);
  if ( (a2 & 0x80u) != 0 )
    interpret_ldm(a1, a2);
  if ( (a2 & 0x20) != 0 )
    interpret_cmp(a1, a2);
  if ( (a2 & 0x10) != 0 )
    interpret_jmp(a1, a2);
  result = a2 & 4;
  if ( (a2 & 4) != 0 )
    return interpret_sys(a1, a2);
  return result;
}

void interpreter_loop(int *a1)
{

  while ( 1 )
  {
    v1 = a1[1029];
    a1[1029] = v1 + 1;
    interpret_instruction(a1, a1[3 * v1] | (a1[3 * v1 + 2] << 16));
  }
}

int main(int argc, const char **argv, const char **envp)
{
  memset(dest, 0, sizeof(dest));
  memcpy(dest, &vm_code, vm_code_length);
  interpreter_loop(dest);
}

