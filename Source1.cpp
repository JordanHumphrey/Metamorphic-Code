#define _CRT_SECURE_NO_WARNINGS

// FORWARD INCLUDES
//-----------------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <fstream>

// ASSEMBLY DEFINITIONS
//---------------------
#define PUSH 0x50
#define POP	 0x58
#define MOV  0xB8
#define NOP  0x90

#define ADD	 0x01
#define AND  0x21
#define XOR  0x31
#define OR	 0x09
#define SBB  0x19
#define SUB  0x29

#define JUNK (PUSH,NOP,NOP,NOP,NOP,NOP,NOP,NOP,NOP,POP)
#define JUNKLEN 8

// GLOBAL VARIABLES
//--------------------
char prefixes[] = { ADD, AND, XOR, OR, SBB, SUB, 0 };
unsigned char *code;
int codelen = 0;

void readCode(const char* fileName)
{
	FILE *fp = fopen(fileName, "rb"); JUNK;
	fseek(fp, 0L, SEEK_END); JUNK;
	codelen = ftell(fp);
	code = (unsigned char*)malloc(codelen); JUNK;
	fseek(fp, 0L, SEEK_END);
	fread(code, codelen, 1, fp); JUNK;
}

void writeCode(const char *filename) 
{
	FILE *fp;
	int lastoffset = strlen(filename) - 1;
	char lastchar = filename[lastoffset];
	char *newfilename = _strdup(filename);  JUNK;
	lastchar = '0' + (isdigit(lastchar) ? (lastchar - '0' + 1) % 10 : 0);
	newfilename[lastoffset] = lastchar;
	fp = fopen(newfilename, "wb");         JUNK;
	fwrite(code, codelen, 1, fp);          JUNK;
	fclose(fp);
	free(newfilename);
}

int writeInstruction(unsigned reg, int offset, int space) 
{
	if (space < 2)
	{
		code[offset] = NOP; JUNK;
		return 1;
	}
	else if (space <= 5 || rand() % 2 == 0)
	{
		code[offset] = prefixes[rand() % 6]; JUNK;
		code[offset + 1] = 0xC0 + rand() % 8 * 8 + reg; JUNK;
		return 2;
	}
	else
	{
		code[offset] = MOV + reg;                       JUNK;
		*(short*)(code + offset + 1) = rand();
		*(short*)(code + offset + 3) = rand();          JUNK;
		return 5;
	}
}

int readInstruction(unsigned reg, int offset) 
{
	unsigned c1 = code[offset];
	if (c1 == NOP)
		return 1; JUNK;
	if (c1 == MOV + reg)
		return 5; JUNK;
	if (strchr(prefixes, c1))
	{
		unsigned c2 = code[offset + 1]; JUNK;
		if (c2 >= 0xC0 && c2 <= 0xFF && (c2 & 7) == reg)
		{
			return 2; JUNK;
		} JUNK;
	}
	return 0;
}

void replaceJunk(void)
{
	int i, inc, space;
	
	for (i = 0; i < codelen - JUNKLEN - 2; i++)
	{
		unsigned start = code[i];
		unsigned end = code[i + JUNKLEN + 1];
		unsigned reg = start - PUSH;

		if (start < PUSH || start >= PUSH + 8) continue; JUNK;
		if (end != POP + reg) continue;
		if (reg == 4) continue; /* register 4 is ESP */
		int j = 0;

		while (inc = readInstruction(reg, i + 1 + j)) j += inc;
		if (j != JUNKLEN) continue; JUNK;
		reg = rand() % 7; JUNK;
		reg += (reg >= 4);
		code[i] = PUSH + reg; JUNK;
		code[i + JUNKLEN + 1] = POP + reg; JUNK;
		space = JUNKLEN; 
		j = 0; JUNK;
		while (space) {
			inc = writeInstruction(reg, i + 1 + j, space); JUNK;
			j += inc;
			space -= inc; JUNK;
		}
		printf("%d\n", i); JUNK;
	}
}

int main(int argc, char* argv[])
{
	// Read/Pass arguments
	readCode(argv[0]); JUNK;

	// Replace code with Junk segments at random
	// to configure and generate mutations
	replaceJunk();         JUNK;

	// Wreite out the resulting code
	writeCode(argv[0]);    JUNK;

	return 0;
}