#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>

#include "mu-mips.h"

/***************************************************************/
/* Print out a list of commands available                                                                  */
/***************************************************************/
void help() {        
	printf("------------------------------------------------------------------\n\n");
	printf("\t**********MU-MIPS Help MENU**********\n\n");
	printf("sim\t-- simulate program to completion \n");
	printf("run <n>\t-- simulate program for <n> instructions\n");
	printf("rdump\t-- dump register values\n");
	printf("reset\t-- clears all registers/memory and re-loads the program\n");
	printf("input <reg> <val>\t-- set GPR <reg> to <val>\n");
	printf("mdump <start> <stop>\t-- dump memory from <start> to <stop> address\n");
	printf("high <val>\t-- set the HI register to <val>\n");
	printf("low <val>\t-- set the LO register to <val>\n");
	printf("print\t-- print the program loaded into memory\n");
	printf("?\t-- display help menu\n");
	printf("quit\t-- exit the simulator\n\n");
	printf("------------------------------------------------------------------\n\n");
}

/***************************************************************/
/* Read a 32-bit word from memory                                                                            */
/***************************************************************/
uint32_t mem_read_32(uint32_t address)
{
	int i;
	for (i = 0; i < NUM_MEM_REGION; i++) {
		if ( (address >= MEM_REGIONS[i].begin) &&  ( address <= MEM_REGIONS[i].end) ) {
			uint32_t offset = address - MEM_REGIONS[i].begin;
			return (MEM_REGIONS[i].mem[offset+3] << 24) |
					(MEM_REGIONS[i].mem[offset+2] << 16) |
					(MEM_REGIONS[i].mem[offset+1] <<  8) |
					(MEM_REGIONS[i].mem[offset+0] <<  0);
		}
	}
	return 0;
}

/***************************************************************/
/* Write a 32-bit word to memory                                                                                */
/***************************************************************/
void mem_write_32(uint32_t address, uint32_t value)
{
	int i;
	uint32_t offset;
	for (i = 0; i < NUM_MEM_REGION; i++) {
		if ( (address >= MEM_REGIONS[i].begin) && (address <= MEM_REGIONS[i].end) ) {
			offset = address - MEM_REGIONS[i].begin;

			MEM_REGIONS[i].mem[offset+3] = (value >> 24) & 0xFF;
			MEM_REGIONS[i].mem[offset+2] = (value >> 16) & 0xFF;
			MEM_REGIONS[i].mem[offset+1] = (value >>  8) & 0xFF;
			MEM_REGIONS[i].mem[offset+0] = (value >>  0) & 0xFF;
		}
	}
}

/***************************************************************/
/* Execute one cycle                                                                                                              */
/***************************************************************/
void cycle() {                                                
	handle_instruction();
	CURRENT_STATE = NEXT_STATE;
	INSTRUCTION_COUNT++;
}

/***************************************************************/
/* Simulate MIPS for n cycles                                                                                       */
/***************************************************************/
void run(int num_cycles) {                                      
	
	if (RUN_FLAG == FALSE) {
		printf("Simulation Stopped\n\n");
		return;
	}

	printf("Running simulator for %d cycles...\n\n", num_cycles);
	int i;
	for (i = 0; i < num_cycles; i++) {
		if (RUN_FLAG == FALSE) {
			printf("Simulation Stopped.\n\n");
			break;
		}
		cycle();
	}
}

/***************************************************************/
/* simulate to completion                                                                                               */
/***************************************************************/
void runAll() {                                                     
	if (RUN_FLAG == FALSE) {
		printf("Simulation Stopped.\n\n");
		return;
	}

	printf("Simulation Started...\n\n");
	while (RUN_FLAG){
		cycle();
	}
	printf("Simulation Finished.\n\n");
}

/***************************************************************/ 
/* Dump a word-aligned region of memory to the terminal                              */
/***************************************************************/
void mdump(uint32_t start, uint32_t stop) {          
	uint32_t address;

	printf("-------------------------------------------------------------\n");
	printf("Memory content [0x%08x..0x%08x] :\n", start, stop);
	printf("-------------------------------------------------------------\n");
	printf("\t[Address in Hex (Dec) ]\t[Value]\n");
	for (address = start; address <= stop; address += 4){
		printf("\t0x%08x (%d) :\t0x%08x\n", address, address, mem_read_32(address));
	}
	printf("\n");
}

/***************************************************************/
/* Dump current values of registers to the teminal                                              */   
/***************************************************************/
void rdump() {                               
	int i; 
	printf("-------------------------------------\n");
	printf("Dumping Register Content\n");
	printf("-------------------------------------\n");
	printf("# Instructions Executed\t: %u\n", INSTRUCTION_COUNT);
	printf("PC\t: 0x%08x\n", CURRENT_STATE.PC);
	printf("-------------------------------------\n");
	printf("[Register]\t[Value]\n");
	printf("-------------------------------------\n");
	for (i = 0; i < MIPS_REGS; i++){
		printf("[R%d]\t: 0x%08x\n", i, CURRENT_STATE.REGS[i]);
	}
	printf("-------------------------------------\n");
	printf("[HI]\t: 0x%08x\n", CURRENT_STATE.HI);
	printf("[LO]\t: 0x%08x\n", CURRENT_STATE.LO);
	printf("-------------------------------------\n");
}

/***************************************************************/
/* Read a command from standard input.                                                               */  
/***************************************************************/
void handle_command() {                         
	char buffer[20];
	uint32_t start, stop, cycles;
	uint32_t register_no;
	int register_value;
	int hi_reg_value, lo_reg_value;

	printf("MU-MIPS SIM:> ");

	if (scanf("%s", buffer) == EOF){
		exit(0);
	}

	switch(buffer[0]) {
		case 'S':
		case 's':
			runAll(); 
			break;
		case 'M':
		case 'm':
			if (scanf("%x %x", &start, &stop) != 2){
				break;
			}
			mdump(start, stop);
			break;
		case '?':
			help();
			break;
		case 'Q':
		case 'q':
			printf("**************************\n");
			printf("Exiting MU-MIPS! Good Bye...\n");
			printf("**************************\n");
			exit(0);
		case 'R':
		case 'r':
			if (buffer[1] == 'd' || buffer[1] == 'D'){
				rdump();
			}else if(buffer[1] == 'e' || buffer[1] == 'E'){
				reset();
			}
			else {
				if (scanf("%d", &cycles) != 1) {
					break;
				}
				run(cycles);
			}
			break;
		case 'I':
		case 'i':
			if (scanf("%u %i", &register_no, &register_value) != 2){
				break;
			}
			CURRENT_STATE.REGS[register_no] = register_value;
			NEXT_STATE.REGS[register_no] = register_value;
			break;
		case 'H':
		case 'h':
			if (scanf("%i", &hi_reg_value) != 1){
				break;
			}
			CURRENT_STATE.HI = hi_reg_value; 
			NEXT_STATE.HI = hi_reg_value; 
			break;
		case 'L':
		case 'l':
			if (scanf("%i", &lo_reg_value) != 1){
				break;
			}
			CURRENT_STATE.LO = lo_reg_value;
			NEXT_STATE.LO = lo_reg_value;
			break;
		case 'P':
		case 'p':
			print_program(); 
			break;
		default:
			printf("Invalid Command.\n");
			break;
	}
}

/***************************************************************/
/* reset registers/memory and reload program                                                    */
/***************************************************************/
void reset() {   
	int i;
	/*reset registers*/
	for (i = 0; i < MIPS_REGS; i++){
		CURRENT_STATE.REGS[i] = 0;
	}
	CURRENT_STATE.HI = 0;
	CURRENT_STATE.LO = 0;
	
	for (i = 0; i < NUM_MEM_REGION; i++) {
		uint32_t region_size = MEM_REGIONS[i].end - MEM_REGIONS[i].begin + 1;
		memset(MEM_REGIONS[i].mem, 0, region_size);
	}
	
	/*load program*/
	load_program();
	
	/*reset PC*/
	INSTRUCTION_COUNT = 0;
	CURRENT_STATE.PC =  MEM_TEXT_BEGIN;
	NEXT_STATE = CURRENT_STATE;
	RUN_FLAG = TRUE;
}

/***************************************************************/
/* Allocate and set memory to zero                                                                            */
/***************************************************************/
void init_memory() {                                           
	int i;
	for (i = 0; i < NUM_MEM_REGION; i++) {
		uint32_t region_size = MEM_REGIONS[i].end - MEM_REGIONS[i].begin + 1;
		MEM_REGIONS[i].mem = malloc(region_size);
		memset(MEM_REGIONS[i].mem, 0, region_size);
	}
}

/**************************************************************/
/* load program into memory                                                                                      */
/**************************************************************/
void load_program() {                   
	FILE * fp;
	int i, word;
	uint32_t address;

	/* Open program file. */
	fp = fopen(prog_file, "r");
	if (fp == NULL) {
		printf("Error: Can't open program file %s\n", prog_file);
		exit(-1);
	}

	/* Read in the program. */

	i = 0;
	while( fscanf(fp, "%x\n", &word) != EOF ) {
		address = MEM_TEXT_BEGIN + i;
		mem_write_32(address, word);
		printf("writing 0x%08x into address 0x%08x (%d)\n", word, address, address);
		i += 4;
	}
	PROGRAM_SIZE = i/4;
	printf("Program loaded into memory.\n%d words written into memory.\n\n", PROGRAM_SIZE);
	fclose(fp);
}

/************************************************************/
/* decode and execute instruction                                                                     */ 
/************************************************************/
void handle_instruction()
{
	uint32_t instruction = mem_read_32(CURRENT_STATE.PC);
	
	uint32_t op = instruction & 0xFC000000; //opcode
	uint32_t bk = instruction & 0x0000003F; //back of 32 bits to id the op code
	uint8_t rs = (instruction & 0x03E00000) >> 21; //source
	uint8_t rt = (instruction & 0x001F0000) >> 16; //destination
	uint16_t im = instruction & 0x0000FFFF; //immediate / offset
	uint8_t rd = (instruction & 0x0000F800) >> 11; //reg destination (register)
	uint8_t h = (instruction & 0x0000007C0) >> 6; //shift amount

	NEXT_STATE.PC = CURRENT_STATE.PC + 4;
	switch(op) {
		case 0x00000000: //ADD, ADDU, AND, NOR, OR, SLT, SUB, SUBU, XOR, SLL, SRL, DIV, DIVU, MULT, MULTU, SRA, MFHI, MFLO, MTHI, MTLO, JR, JALR
			switch(bk) {
				case 0x00000020: //ADD
					NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rs] + CURRENT_STATE.REGS[rt];	
					break;
				case 0x00000021: //ADDU
					NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rs] + CURRENT_STATE.REGS[rt];
					break;
				case 0x00000024: //AND
					NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rs] & CURRENT_STATE.REGS[rt];
					break;
				case 0x00000027: //NOR
					NEXT_STATE.REGS[rd] = ~(CURRENT_STATE.REGS[rs] | CURRENT_STATE.REGS[rt]);
					break;
				case 0x00000025: //OR
					NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rs] | CURRENT_STATE.REGS[rt];
					break;
				case 0x0000002A: //SLT
					if (CURRENT_STATE.REGS[rs] < CURRENT_STATE.REGS[rt]) {
						NEXT_STATE.REGS[rd] = 0xFFFFFFFF;
					} else {
						NEXT_STATE.REGS[rd] = 0x0;
					}
					break;
				case 0x00000022: //SUB
					NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rs] - CURRENT_STATE.REGS[rt];
					break;
				case 0x00000023: //SUBU
					NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rs] - CURRENT_STATE.REGS[rt];
					break;
				case 0x00000026: //XOR
					NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rs] ^ CURRENT_STATE.REGS[rt];

					break;
				case 0x00000000: //SLL
					NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rt] << h;
					break;
				case 0x00000002: //SRL
					NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rt] >> h;

					break;
				case 0x0000001A: //DIV
					NEXT_STATE.LO = CURRENT_STATE.REGS[rs] / CURRENT_STATE.REGS[rt];
					NEXT_STATE.HI = CURRENT_STATE.REGS[rs] % CURRENT_STATE.REGS[rt];
					break;
				case 0x0000001B: //DIVU
					NEXT_STATE.LO = (0x0 || CURRENT_STATE.REGS[rs]) / (0x0 || CURRENT_STATE.REGS[rt]); //might be wrong
					NEXT_STATE.HI = (0x0 || CURRENT_STATE.REGS[rs]) % (0x0 || CURRENT_STATE.REGS[rt]); //might be wrong

					break;
				case 0x00000018: //MULT
					NEXT_STATE.LO = (CURRENT_STATE.REGS[rs] * CURRENT_STATE.REGS[rt]) & 0x00000000FFFFFFFF;
					NEXT_STATE.HI = (CURRENT_STATE.REGS[rs] * CURRENT_STATE.REGS[rt]) & 0xFFFFFFFF00000000 >> 32;
					break;
				case 0x00000019: //MULTU
					NEXT_STATE.LO = (CURRENT_STATE.REGS[rs] * CURRENT_STATE.REGS[rt]) & 0x00000000FFFFFFFF;
					NEXT_STATE.HI = (CURRENT_STATE.REGS[rs] * CURRENT_STATE.REGS[rt]) & 0xFFFFFFFF00000000 >> 32;
					break;
				case 0x00000003: //SRA
					NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rt] >> h;
					break;
				case 0x00000010: //MFHI
					NEXT_STATE.REGS[rd] = CURRENT_STATE.HI;
					break;
				case 0x00000012: //MFLO
					NEXT_STATE.REGS[rd] = CURRENT_STATE.LO;
					break;
				case 0x00000011: //MTHI
					NEXT_STATE.HI= CURRENT_STATE.REGS[rs];

					break;
				case 0x00000013: //MTLO
					NEXT_STATE.LO = CURRENT_STATE.REGS[rs];

					break;
				case 0x00000008: //JR
					NEXT_STATE.PC = CURRENT_STATE.REGS[rs];	
					break;
				case 0x00000009: //JALR
					NEXT_STATE.REGS[rd] = CURRENT_STATE.PC + 8;
					NEXT_STATE.PC = CURRENT_STATE.REGS[rs];
					break;
				case 0x0000000C: //SYSTEM CALL
					if(CURRENT_STATE.REGS[2] == 0xA) {
						RUN_FLAG = FALSE;
					}
					printf("SYSTEM CALL\n");	
					break;
			}
			break;
		case 0x04000000:
			switch(rt) {
				case 0x00000000: //BLTZ
					if((CURRENT_STATE.REGS[rs] || 0x80000000) == 0x80000000) {
						NEXT_STATE.PC = CURRENT_STATE.PC + (im << 2); 		
					}
					break;
				case 0x00010000: //BGEZ
					if((CURRENT_STATE.REGS[rs] || 0x80000000) == 0x0) {
						NEXT_STATE.PC = CURRENT_STATE.PC + (im << 2); 		
					}

					break;
			}
			break;
		case 0x20000000: //ADDI
			NEXT_STATE.REGS[rt] = CURRENT_STATE.REGS[rs] + im;
			break;
		case 0x24000000: //ADDIU
			NEXT_STATE.REGS[rt] = CURRENT_STATE.REGS[rs] + im;
			break;
		case 0x30000000: //ANDI
			NEXT_STATE.REGS[rt] = 0x0 || (CURRENT_STATE.REGS[rs] & im);
			break;
		case 0x34000000: //ORI
			NEXT_STATE.REGS[rt] = CURRENT_STATE.REGS[rs] | im;
			break;
		case 0x28000000: //SLTI
			if (CURRENT_STATE.REGS[rs] < im ) {
				NEXT_STATE.REGS[rt] = 0;
			} else {
				NEXT_STATE.REGS[rt] = 1;
			}
			break;
		case 0x38000000: //XORI
			NEXT_STATE.REGS[rt] = CURRENT_STATE.REGS[rs] ^ im;

			break;
		case 0x10000000: //BEQ
			if(CURRENT_STATE.REGS[rs] == CURRENT_STATE.REGS[rt]) {
				NEXT_STATE.PC = CURRENT_STATE.PC + (im << 2); 		
			}
			break;
		case 0x14000000: //BNE
			if(CURRENT_STATE.REGS[rs] != CURRENT_STATE.REGS[rt]) {
				NEXT_STATE.PC = CURRENT_STATE.PC + (im << 2); 		
			}

			break;
		case 0x18000000: //BLEZ
			if(CURRENT_STATE.REGS[rs] != 0x00000000 || (CURRENT_STATE.REGS[rs] || 0x80000000) == 0x80000000) {
				NEXT_STATE.PC = CURRENT_STATE.PC + (im << 2); 		
			}

			break;
		case 0x1C000000: //BGTZ
			if(CURRENT_STATE.REGS[rs] != 0x00000000 && (CURRENT_STATE.REGS[rs] || 0x80000000) == 0x0) {
				NEXT_STATE.PC = CURRENT_STATE.PC + (im << 2); 		
			}

			break;
		case 0x08000000: //J
			{
			uint32_t temp = instruction & 0x03FFFFFF;
			temp = temp << 2;
			NEXT_STATE.PC = temp;
		 	break;
			}
		case 0x0C000000: //JAL
			{
			uint32_t temp = instruction & 0x03FFFFFF;
			temp = temp << 2;
			NEXT_STATE.PC = temp;
			NEXT_STATE.REGS[31] = CURRENT_STATE.PC + 8;
			break; 
			}
		case 0x80000000: //LB
			{
				uint32_t temp = CURRENT_STATE.REGS[rs] + im;
				NEXT_STATE.REGS[rt] = temp;
				
			}
			break;
		case 0x84000000: //LH
			{
				uint32_t temp = CURRENT_STATE.REGS[rs] + im;
			}
			break;
		case 0x8C000000: //LW
			{
				uint32_t temp = CURRENT_STATE.REGS[rs] + im;
				NEXT_STATE.REGS[rt] = mem_read_32(temp);
			}
			break;
		case 0xA0000000: //SB
			{
				uint32_t temp = CURRENT_STATE.REGS[rs] + im;
				mem_write_32(temp, (uint32_t) CURRENT_STATE.REGS[rt]); 
				
			}
			break;
		case 0xA4000000: //SH
			{
				uint32_t temp = CURRENT_STATE.REGS[rs] + im;
				mem_write_32(temp, (uint32_t) CURRENT_STATE.REGS[rt]); 
			}
			break;
		case 0xAC000000: //SW
			{
				uint32_t temp = CURRENT_STATE.REGS[rs] + im;
				mem_write_32(temp, (uint32_t) CURRENT_STATE.REGS[rt]); 
				
			}

			break;
		case 0x3C000000: //LUI
			NEXT_STATE.REGS[rt] = (im << 16);
			break;
	}

}


/************************************************************/
/* Initialize Memory                                                                                                    */ 
/************************************************************/
void initialize() { 
	init_memory();
	CURRENT_STATE.PC = MEM_TEXT_BEGIN;
	NEXT_STATE = CURRENT_STATE;
	RUN_FLAG = TRUE;
}

/************************************************************/
/* Print the program loaded into memory (in MIPS assembly format)    */ 
/************************************************************/
void print_program(){
	int i;
	uint32_t addr;
	
	for(i=0; i<PROGRAM_SIZE; i++){
		addr = MEM_TEXT_BEGIN + (i*4);
		printf("[0x%x]\t", addr);
		print_instruction(addr);
	}
}

/************************************************************/
/* Print the instruction at given memory address (in MIPS assembly format)    */
/************************************************************/
void print_instruction(uint32_t addr){
	/*IMPLEMENT THIS*/

	char string[64];
	
	uint32_t instruction = mem_read_32(addr);
	
	uint32_t op = instruction & 0xFC000000; //opcode
	uint32_t bk = instruction & 0x0000003F; //back of 32 bits to id the op code
	uint8_t rs = (instruction & 0x03E00000) >> 21; //source
	uint8_t rt = (instruction & 0x001F0000) >> 16; //destination
	uint16_t im = instruction & 0x0000FFFF; //immediate / offset
	uint8_t rd = (instruction & 0x0000F800) >> 11; //reg destination (register)
	uint8_t h = (instruction & 0x0000007C0) >> 6; //for shift instructions)
	switch(op) {
		case 0x00000000: //ADD, ADDU, AND, NOR, OR, SLT, SUB, SUBU, XOR, SLL, SRL, DIV, DIVU, MULT, MULTU, SRA, MFHI, MFLO, MTHI, MTLO, JR, JALR
			switch(bk) {
				case 0x00000020: //ADD
					sprintf(string, "ADD $%d, $%d, $%d\n", rd, rs, rt);
					break;
				case 0x00000021: //ADDU
					sprintf(string, "ADDU $%d, $%d, $%d\n", rd, rs, rt);
					break;
				case 0x00000024: //AND
					sprintf(string, "AND $%d, $%d, $%d\n", rd, rs, rt);
					break;
				case 0x00000027: //NOR
					sprintf(string, "NOR $%d, $%d, $%d\n", rd, rs, rt);
					break;
				case 0x00000025: //OR
					sprintf(string, "OR $%d, $%d, $%d\n", rd, rs, rt);
					break;
				case 0x0000002A: //SLT
					sprintf(string, "SLT $%d, $%d, $%d\n", rd, rs, rt);
					break;
				case 0x00000022: //SUB
					sprintf(string, "SUB $%d, $%d, $%d\n", rd, rs, rt);
					break;
				case 0x00000023: //SUBU
					sprintf(string, "SUBU $%d, $%d, $%d\n", rd, rs, rt);
					break;
				case 0x00000026: //XOR
					sprintf(string, "XOR $%d, $%d, $%d\n", rd, rs, rt);
					break;
				case 0x00000000: //SLL
					sprintf(string, "SLL $%d, $%d, 0x%X\n", rd, rt, h);
					break;
				case 0x00000002: //SRL
					sprintf(string, "SRL $%d, $%d, %d\n", rd, rt, h);
					break;
				case 0x0000001A: //DIV
					sprintf(string, "DIV $%d, $%d\n", rs, rt);
					break;
				case 0x0000001B: //DIVU
					sprintf(string, "DIVU $%d, $%d\n", rs, rt);
					break;
				case 0x00000018: //MULT
					sprintf(string, "MULT $%d, $%d\n", rs, rt);
					break;
				case 0x00000019: //MULTU
					sprintf(string, "MULTU $%d, $%d\n", rs, rt);
					break;
				case 0x00000003: //SRA
					sprintf(string, "SRA $%d, $%d, %d\n", rd, rt, h);
					break;
				case 0x00000010: //MFHI
					sprintf(string, "MFHI $%d\n", rd);
					break;
				case 0x00000012: //MFLO
					sprintf(string, "MFLO $%d\n", rd);
					break;
				case 0x00000011: //MTHI
					sprintf(string, "MTHI $%d, $%d, $%d\n", rd, rs, rt);
					break;
				case 0x00000013: //MTLO
					sprintf(string, "MTLO $%d, $%d, $%d\n", rd, rs, rt);
					break;
				case 0x00000008: //JR
					sprintf(string, "JR $%d\n", rs);
					break;
				case 0x00000009: //JALR
					sprintf(string, "JALR $%d, $%d, $%d\n", rd, rs, rt);
					break;
				case 0x0000000C: //SYSTEM CALL
					sprintf(string, "SYSTEMCALL\n");
					break;
			}
			break;
		case 0x04000000:
			switch(rt) {
				case 0x00000000:
					sprintf(string, "BLTZ $%d, %d\n", rs, im);
					break;
				case 0x00000008:
					sprintf(string, "BGEZ $%d, %d\n", rs, im);
					break;
			}
			break;
		case 0x20000000:
			sprintf(string, "ADDI $%d, $%d, 0x%04X\n", rt, rs, im);
			break;
		case 0x24000000:
			sprintf(string, "ADDIU $%d, $%d, 0x%04X\n", rt, rs, im);
			break;
		case 0x30000000:
			sprintf(string, "ANDI $%d, $%d, 0x%04X\n", rt, rs, im);
			break;
		case 0x34000000:
			sprintf(string, "ORI $%d, $%d, 0x%04X\n", rt, rs, im);
			break;
		case 0x28000000:
			sprintf(string, "SLTI $%d, $%d, 0x%04X\n", rt, rs, im);
			break;
		case 0x38000000:
			sprintf(string, "XORI $%d, $%d, 0x%04X\n", rt, rs, im);
			break;
		case 0x10000000:
			sprintf(string, "BEQ $%d, $%d, %d\n", rs, rt, im);
			break;
		case 0x14000000:
			sprintf(string, "BNE $%d, $%d, %d\n", rs, rt, im);
			break;
		case 0x18000000:
			sprintf(string, "BLEZ $%d, %d\n", rs, im);
			break;
		case 0x1C000000:
			sprintf(string, "BGTZ $%d, %d\n", rs, im);
			break;
		case 0x08000000:
			sprintf(string, "J 0x%X\n", instruction & 0x03FFFFFF);
		 	break;
		case 0x0C000000:
			sprintf(string, "JAL %u\n", instruction & 0x03FFFFFF);
			break;
		case 0x80000000:
			sprintf(string, "LB $%d, %d ($%d) \n", rt, im, rs);
			break;
		case 0x84000000:
			sprintf(string, "LH $%d, $%d, %d\n", rs, rt, im);
			break;
		case 0x8C000000:
			sprintf(string, "LW $%d, %d ($%d)\n", rt, im, rs);
			break;
		case 0xA0000000:
			sprintf(string, "SB $%d, $%d($%d)\n", rt, im, rs);
			break;
		case 0xA4000000:
			sprintf(string, "SH $%d, 0x%04X, %d\n", rs, rt, im);
			break;
		case 0xAC000000:
			sprintf(string, "SW $%d, 0x%04X ($%d)\n", rt, im, rs);
			break;
		case 0x3C000000:
			sprintf(string, "LUI $%d, 0x%04X\n", rt, im);
			break;
	}
	printf("%s", string);
}

/***************************************************************
				main                                                           
***************************************************************/
int main(int argc, char *argv[]) {                              
	printf("\n**************************\n");
	printf("Welcome to MU-MIPS SIM...\n");
	printf("**************************\n\n");
	
	if (argc < 2) {
		printf("Error: You should provide input file.\nUsage: %s <input program> \n\n",  argv[0]);
		exit(1);
	}

	strcpy(prog_file, argv[1]);
	initialize();
	load_program();
	help();
	while (1){
		handle_command();
	}
	return 0;
}
