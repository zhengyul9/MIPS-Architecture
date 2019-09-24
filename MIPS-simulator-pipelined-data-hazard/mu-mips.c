#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>

#include "mu-mips.h"

int ENABLE_FORWARDING = 0;
int stall = 0;
uint32_t ID_EX_rs = 0;
uint32_t ID_EX_rt = 0;
uint32_t EX_MEM_RegisterRd = 0;
uint32_t EX_MEM_RegisterRt = 0;
uint32_t MEM_WB_RegisterRt = 0;
uint32_t MEM_WB_RegisterRd = 0;
int EX_MEM_RegWrite = 1;
int MEM_WB_RegWrite = 1;
int forwardA = 0;
int forwardB = 0;
uint32_t prevInstr = 0;
/*************************************************************
       Print out a list of commands available                                                                  
**************************************************************
*/
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
	printf("show\t-- print the current content of the pipeline registers\n");
	printf("?\t-- display help menu\n");
	printf("forward\t Set/reset forwarding\n");
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
	handle_pipeline();
	CURRENT_STATE = NEXT_STATE;
	CYCLE_COUNT++;
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
	printf("# Cycles Executed\t: %u\n", CYCLE_COUNT);
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
			if (buffer[1] == 'h' || buffer[1] == 'H'){
				show_pipeline();
			} else {
				runAll(); 
			}
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
		case 'f':
			if (scanf("%d", &ENABLE_FORWARDING) != 1) {
				break;
			}
			ENABLE_FORWARDING == 0 ? printf("Forwarding OFF\n") : printf("Forwarding ON\n");
			break;
		default:
			printf("Invalid Command.\n");
			break;
	}
}
//  COPIED OVER FROM LAB 1 - calebs code


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
		
	IF_ID.PC = 0;
        IF_ID.IR = 0;
        IF_ID.A = 0;
        IF_ID.B = 0;
        IF_ID.imm = 0;
        IF_ID.ALUOutput = 0;
        IF_ID.LMD = 0;
        ID_EX.PC = 0;
        ID_EX.IR = 0;
        ID_EX.A = 0;
        ID_EX.B = 0;
        ID_EX.imm = 0;
        ID_EX.ALUOutput = 0;
        ID_EX.LMD = 0;
        EX_MEM.PC = 0;
        EX_MEM.IR = 0;
        EX_MEM.A = 0;
        EX_MEM.B = 0;
        EX_MEM.imm = 0;
        EX_MEM.ALUOutput = 0;
        EX_MEM.LMD = 0;
        MEM_WB.PC = 0;
        MEM_WB.IR = 0;
        MEM_WB.A = 0;
        MEM_WB.B = 0;
        MEM_WB.imm = 0;
        MEM_WB.ALUOutput = 0;
        MEM_WB.LMD = 0;
	
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
/* maintain the pipeline                                                                                           */ 
/************************************************************/
void handle_pipeline() {
	/*INSTRUCTION_COUNT should be incremented when instruction is done*/
	/*Since we do not have branch/jump instructions, INSTRUCTION_COUNT should be incremented in WB stage */
	WB();
	MEM();
	EX();
	ID();
	IF();
}

/************************************************************/
/* writeback (WB) pipeline stage:                                                                          */ 
/************************************************************/
void WB() {
	if(MEM_WB.IR == 0) {
		return;
	}
	uint32_t instruction = MEM_WB.IR;
	prevInstr = instruction;
    uint32_t op = (instruction & 0xFC000000)>>26; //opcode
    uint32_t bk = instruction & 0x0000003F; //back of 32 bits to id the op code
    uint32_t rt = (instruction & 0x001F0000) >> 16; //destination
    uint32_t rd = (instruction & 0x0000F800) >> 11; //reg destination (register)
	switch( op ) {
		case 0x00: //Reg-Reg
        		if(bk == 0xC && MEM_WB.ALUOutput == 0xA){
        			RUN_FLAG = FALSE;
        			MEM_WB.ALUOutput = 0x0;
        			break;
            	}
            	NEXT_STATE.REGS[rd] = MEM_WB.ALUOutput;
            	break;
		case 0x23: //LW
            	NEXT_STATE.REGS[rt] = MEM_WB.LMD;
            	break;
		case 0x2B: //SW
				break;
		default: //Reg-Imm
            	NEXT_STATE.REGS[rt] = MEM_WB.ALUOutput;
            	break;
    }
	INSTRUCTION_COUNT++;
	if(stall != 0 ) {
		stall--;
	}
}
/************************************************************/
/* memory access (MEM) pipeline stage:                                                          */ 
/************************************************************/
void MEM() {
		MEM_WB.IR = EX_MEM.IR;
		uint32_t instruction, data, opcode;
		instruction = MEM_WB.IR;
		MEM_WB_RegisterRt = (instruction & 0x001F0000) >> 16; //reg destination (register)
	    MEM_WB_RegisterRd = (instruction & 0x0000F800) >> 11;
		EX_MEM.A = ID_EX.A;
		opcode = (MEM_WB.IR & 0xFC000000) >> 26;
		if(MEM_WB.IR == 0) {
			return;
		}
		switch(opcode){
			case 0x20: //LB
            	data = mem_read_32(EX_MEM.ALUOutput);
            	MEM_WB.LMD = ((data & 0x000000FF) & 0x8000) > 0 ? (data | 0xFFFFFF00) : ( data & 0x000000FF);
            	break;
        	case 0x21: //LH
            	data = mem_read_32(EX_MEM.ALUOutput);
            	MEM_WB.LMD = ((data & 0x000000FF) & 0x8000) > 0 ? (data | 0xFFFF0000) : ( data & 0x0000FFFF);
            	break;
        	case 0x23: //LW
            	data = mem_read_32(EX_MEM.ALUOutput);
            	MEM_WB.LMD = data;
            	break;
        	case 0x28: //SB
            	data = mem_read_32(EX_MEM.ALUOutput);
            	data = (data & 0xFFFFFF00) | (EX_MEM.B & 0x000000FF);
            	mem_write_32(EX_MEM.ALUOutput, EX_MEM.B);
            	break;
        	case 0x29: //SH
            	data = mem_read_32(EX_MEM.ALUOutput);
            	data = (data & 0xFFFF0000) | (EX_MEM.B & 0x0000FFFF);
            	mem_write_32(EX_MEM.ALUOutput, EX_MEM.B);
            	break;
        	case 0x2B: //SW
            	mem_write_32(EX_MEM.ALUOutput, EX_MEM.B);
            	break;
        	default:
        		MEM_WB.ALUOutput = EX_MEM.ALUOutput;
           		break;
    	}
}

/************************************************************/
/* execution (EX) pipeline stage:                                                                          */ 
/************************************************************/
void EX() {
	if(stall == 0) {

	EX_MEM.IR = ID_EX.IR;
	uint32_t instruction = EX_MEM.IR;

	uint32_t op = (instruction & 0xFC000000) >> 26; //opcode
	EX_MEM_RegisterRd = (instruction & 0x0000F800) >> 11; //reg destination (register)
	uint32_t bk = instruction & 0x0000003F; //back of 32 bits to id the op code
	uint32_t rt = (instruction & 0x001F0000) >> 16; //destination
	uint32_t im = instruction & 0x0000FFFF;
	EX_MEM_RegisterRt = rt;
	if(EX_MEM.IR == 0) {
		return;
	}
	if(ENABLE_FORWARDING){
		ID_EX.A = CURRENT_STATE.REGS[ID_EX_rs];
		ID_EX.B = CURRENT_STATE.REGS[ID_EX_rt];
		ID_EX.imm = im;
			if(forwardA == 0x10){
				switch((MEM_WB.IR & 0xFC000000) >> 26){
				    case 0x23: //LW
				    	ID_EX.A = MEM_WB.LMD;
				        break;
				    default:
				        ID_EX.A = EX_MEM.ALUOutput;
				        break;
				}
			} else if (forwardB  == 0x10){
				switch((MEM_WB.IR & 0xFC000000) >> 26){
				    case 0x23: //LW
				    	ID_EX.B = MEM_WB.LMD;
				        break;
				    default:
				        ID_EX.B = EX_MEM.ALUOutput;
				        break;
				}
			}
			if(forwardA == 0x01){
				switch((MEM_WB.IR & 0xFC000000) >> 26){
                	case 0x23: //LW
                		ID_EX.A = MEM_WB.LMD;
                		break;
                	default:
                		ID_EX.A = MEM_WB.ALUOutput;
                		break;
				}
			} else if (forwardB == 0x01){
					switch((MEM_WB.IR & 0xFC000000) >> 26){
						case 0x23: //LW
							ID_EX.B = MEM_WB.LMD;
							break;
						default:
							ID_EX.B = MEM_WB.ALUOutput;
							break;
					}
					switch((prevInstr & 0xFC000000) >> 26) {
						case 0x23: //LW
							ID_EX.B = MEM_WB.LMD;
							break;
						default:
							ID_EX.B = MEM_WB.ALUOutput;
							break;
					}
			}
	}
        forwardA = 0x00;
        forwardB = 0x00;
	switch(op) {
		case 0x00://backend case or Bk
			switch( bk ) {
				case 0x00://SLL
					EX_MEM.ALUOutput = ID_EX.A << ID_EX.B;
					break;
				case 0x02://SRL
					EX_MEM.ALUOutput = ID_EX.A >> ID_EX.B;
					break;
					case 0x03://SRA
						if(( ID_EX.A & 0x80000000) == 1) {
							EX_MEM.ALUOutput = ~(~ID_EX.A >> ID_EX.B);
						} else {
							EX_MEM.ALUOutput = ID_EX.A >> ID_EX.B;
						}
						break;
					case 0x0C: //SYSCALL
						EX_MEM.ALUOutput = 0xA;
						break;
					case 0x10:// MFHI
						EX_MEM.ALUOutput = CURRENT_STATE.HI;
						break;
					case 0x11:// MTHI
						CURRENT_STATE.HI = ID_EX.A;
						break;
					case 0x12: // MFLO
						EX_MEM.ALUOutput = CURRENT_STATE.LO;
						break;
					case 0x13://MTLO
						CURRENT_STATE.LO = ID_EX.A;
						break;
					case 0x18: { //MULT
						uint64_t productone, producttwo, multi;
						if((ID_EX.A & 80000000) == 0x80000000) {
							productone = 0xFFFFFFFF00000000 | ID_EX.A;
						} else {
							productone = 0x00000000FFFFFFFF & ID_EX. A;
						}
						if((CURRENT_STATE.REGS[rt] & 0x80000000) == 0x80000000 ) {
							producttwo = 0xFFFFFFFF00000000 | ID_EX.B;
						} else {
							producttwo = 0x00000000FFFFFFFF & ID_EX.B;
						}
						multi = productone * producttwo;
						NEXT_STATE.HI = ( multi  & 0xFFFFFFFF00000000);
						NEXT_STATE.LO = ( multi * 0x00000000FFFFFFFF) >> 32;
						break;
					}
					case 0x19: {                 //MULTU

						uint64_t product;
						product = (uint64_t)ID_EX.A * (uint64_t)ID_EX.B;
						NEXT_STATE.LO = (product & 0x00000000FFFFFFFF);
						NEXT_STATE.HI = (product & 0xFFFFFFFF00000000) >> 32;
						break;
					}
					case 0x1A: //DIV
						if(ID_EX.B != 0) {
							NEXT_STATE.LO = (int32_t)ID_EX.A / (int32_t)ID_EX.B;
							NEXT_STATE.HI = (int32_t)ID_EX.A % (int32_t)ID_EX.B;
						}
						break;
					case 0x1B: //DIVU
						if(ID_EX.B != 0) {
							NEXT_STATE.LO = ID_EX.A / ID_EX.B;
							NEXT_STATE.HI = ID_EX.A % ID_EX.B;
						}
						break;
					case 0x20: //ADD
						EX_MEM.ALUOutput = ID_EX.A + ID_EX.B;
						break;
					case 0x21: //ADDU
						EX_MEM.ALUOutput = ID_EX.A + ID_EX.B;
						break;
					case 0x22: //SUB
						EX_MEM.ALUOutput = ID_EX.A - ID_EX.B;
						break;
					case 0x23: //SUBU
						EX_MEM.ALUOutput = ID_EX.A - ID_EX.B;
						break;
					case 0x24: //AND
						EX_MEM.ALUOutput = ID_EX.A & ID_EX.B;
						break;
					case 0x25: //OR
						EX_MEM.ALUOutput = ID_EX.A | ID_EX.B;
						break;
					case 0x26: //XOR
						EX_MEM.ALUOutput = ID_EX.A ^ ID_EX.B;
						break;
					case 0x27: //NOR
						EX_MEM.ALUOutput = ~(ID_EX.A | ID_EX.B);
						break;
					case 0x2A: //SLT
						if(ID_EX.A < ID_EX.B){
							EX_MEM.ALUOutput = 0x1;
						} else {
							EX_MEM.ALUOutput = 0x0;
						}
						break;
				}
				break; //end of Reg-Reg functions
			case 0x08: //ADDI
				EX_MEM.ALUOutput = ID_EX.A + ID_EX.imm;
            	break;
        	case 0x09: //ADDIU
           	 	EX_MEM.ALUOutput = ID_EX.A + ((ID_EX.imm & 0x8000) > 0 ? (ID_EX.imm |0xFFFF0000) : (ID_EX.imm & 0x0000FFFF));
          	 	break;
      		case 0x0A: //SLTI
        	    if((int32_t)ID_EX.A - (int32_t)((ID_EX.imm & 0x8000) > 0 ? (ID_EX.imm |0xFFFF0000) : (ID_EX.imm & 0x0000FFFF)) < 0){
               		EX_MEM.ALUOutput = 0x1;
           	 	} else {
                	EX_MEM.ALUOutput = 0x0;
           	 	}
        	    break;
       		case 0x0C: //ANDI
           		EX_MEM.ALUOutput = ID_EX.A ^ (ID_EX.imm & 0x0000FFFF);
            	break;
        	case 0x0D: //ORI
            	EX_MEM.ALUOutput = ID_EX.A | (ID_EX.imm & 0x0000FFFF);
           	 	break;
        	case 0x0E: //XORI
            	EX_MEM.ALUOutput = ID_EX.A ^ ID_EX.imm;
           	 	break;
        	case 0x0F: //LUI
           	 	EX_MEM.ALUOutput = ID_EX.imm << 16;
           	 	break;
       	    case 0x20: //LB
        	    EX_MEM.ALUOutput = ID_EX.A + ID_EX.imm;
        	    break;
	        case 0x21: //LH
	        	EX_MEM.ALUOutput = ID_EX.A + ID_EX.imm;
	            break;
	        case 0x23: //LW
	        	EX_MEM.ALUOutput = ID_EX.A + ((ID_EX.imm & 0x8000) > 0 ? (ID_EX.imm |0xFFFF0000) : (ID_EX.imm & 0x0000FFFF));
	        	EX_MEM.B = ID_EX.B;
  	            break;
	        case 0x28: //SB
        	    EX_MEM.ALUOutput = ID_EX.A + ID_EX.imm;
        	    break;
     		case 0x29: //SH
        	    EX_MEM.ALUOutput = ID_EX.A + ID_EX.imm;
        	    break;
      	 	case 0x2B: //SW
      	 		EX_MEM.ALUOutput = ID_EX.A + ((ID_EX.imm & 0x8000) > 0 ? (ID_EX.imm |0xFFFF0000) : (ID_EX.imm & 0x0000FFFF));
      	 		EX_MEM.B = ID_EX.B;
       		    break;
      		default:
      			printf("Instruction is not implemented at 0x%x\n", EX_MEM.IR);
           	break;
	}
	}
}	//function end

/************************************************************/
/* instruction decode (ID) pipeline stage:                                                         */ 
/************************************************************/
void ID() {
	if(stall == 0){

	    ID_EX.IR = IF_ID.IR;
		uint32_t instruction = ID_EX.IR;
		uint32_t op = (instruction & 0xFC000000) >>26; //opcode
		uint32_t bk = instruction & 0x0000003F; //back of 32 bits to id the op code
		ID_EX_rs = (instruction & 0x03E00000) >> 21; //source
		ID_EX_rt = (instruction & 0x001F0000) >> 16; //destination
		uint32_t im = instruction & 0x0000FFFF; //immediate / offset change to 32bits for the sign-extend
		uint32_t h = (instruction & 0x0000007C0) >> 6; //for shift instructions) **** know as SA in the BOOK

		if((EX_MEM_RegWrite && (EX_MEM_RegisterRd != 0)) && (EX_MEM_RegisterRd == ID_EX_rs)) {
			if(ENABLE_FORWARDING == 1) {
				forwardA = 0x10;
			} else {
				stall = 2;
			}
		}
		if((EX_MEM_RegWrite && (EX_MEM_RegisterRd != 0)) && (EX_MEM_RegisterRd == ID_EX_rt)) {
			if(ENABLE_FORWARDING == 1) {
				forwardB = 0x10;
			} else {
				stall = 2;
			}
		}
		if((EX_MEM_RegWrite && (EX_MEM_RegisterRt != 0)) && (EX_MEM_RegisterRt == ID_EX_rs)) {
			if(ENABLE_FORWARDING == 1) {
				forwardA = 0x10;
			} else {
				stall = 2;
			}
		}
		if((EX_MEM_RegWrite && (EX_MEM_RegisterRt != 0)) && (EX_MEM_RegisterRt == ID_EX_rt)) {
			if(ENABLE_FORWARDING == 1) {
				forwardB = 0x10;
			} else {
				stall = 2;
			}
		}
		if((MEM_WB_RegWrite && (MEM_WB_RegisterRt != 0)) && (MEM_WB_RegisterRt == ID_EX_rs)) {
			if(ENABLE_FORWARDING == 1) {
				forwardA = 0x01;
			} else {
				stall = 1;
			}
		}
		if((MEM_WB_RegWrite && (MEM_WB_RegisterRt != 0)) && (MEM_WB_RegisterRt == ID_EX_rt)) {
			if(ENABLE_FORWARDING == 1) {
				forwardB = 0x01;
			} else {
				stall = 1;
			}
		}
		if((MEM_WB_RegWrite && (MEM_WB_RegisterRd != 0)) && (MEM_WB_RegisterRd == ID_EX_rs)) {
			if(ENABLE_FORWARDING == 1) {
				forwardA = 0x01;
			} else {
				stall = 1;
			}
		}
		if((MEM_WB_RegWrite && (MEM_WB_RegisterRd != 0)) && (MEM_WB_RegisterRd == ID_EX_rt)) {
			if(ENABLE_FORWARDING == 1) {
				forwardB = 0x01;
			} else {
				stall = 1;
			}
		}

		if(op == 0X00) {
			switch(bk) {
				case 0x00://SLL last of the bk shouldSIM be 000000
					ID_EX.A = CURRENT_STATE.REGS[ID_EX_rs];
					ID_EX.B = CURRENT_STATE.REGS[h];
					ID_EX.imm = 0;
					break;
				case 0x02://SRL last of the bk should be 000010
					ID_EX.A = CURRENT_STATE.REGS[ID_EX_rs];
					ID_EX.B = CURRENT_STATE.REGS[h];
					ID_EX.imm = 0;
					break;
				case 0x03://SRA last of the bk should be 000011
					ID_EX.A = CURRENT_STATE.REGS[ID_EX_rs];
					ID_EX.B = CURRENT_STATE.REGS[h];
					ID_EX.imm = 0;
					break;
				default: //Reg-Reg

					ID_EX.A = CURRENT_STATE.REGS[ID_EX_rs];
					ID_EX.B = CURRENT_STATE.REGS[ID_EX_rt];
					ID_EX.imm = 0;
					break;
			}
		} else { //immediate
			ID_EX.A = CURRENT_STATE.REGS[ID_EX_rs];
			ID_EX.B = CURRENT_STATE.REGS[ID_EX_rt];
			ID_EX.imm = im;
		}
	if(stall != 0) {
		ID_EX.IR = 0x00;
	}
	}
}
/************************************************************/
/* instruction fetch (IF) pipeline stage:                                                              */ 
/************************************************************/
void IF() {
	/*IMPLEMENT THIS*/
	if(stall == 0) {
		IF_ID.IR = mem_read_32(CURRENT_STATE.PC);
		IF_ID.PC = CURRENT_STATE.PC + 4; //incrementing to the next address
		NEXT_STATE.PC = IF_ID.PC; // new instruction
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
void show_pipeline(){
	printf("\nCurrent PC:   %x\n", CURRENT_STATE.PC);
        printf("IF/ID.IR      %x\n", IF_ID.IR);
        printf("IF/ID.PC      %x\n", IF_ID.PC);
        printf("ID/EX.IR      %x\n", ID_EX.IR);
        printf("ID/EX.A       %x\n", ID_EX.A);
        printf("ID/EX.B       %x\n", ID_EX.B);
        printf("ID/EX.imm     %x\n", ID_EX.imm);
        printf("EX/MEM.IR     %x\n", EX_MEM.IR);
        printf("EX/MEM.ALUOutput %x\n", EX_MEM.ALUOutput);
        printf("MEM/WB.IR     %x\n", MEM_WB.IR);
        printf("MEM/WB.ALUOutput %x\n", MEM_WB.ALUOutput);
        printf("MEM/WB.LMD    %x\n\n", MEM_WB.LMD);     
}

/************************************************************/
/* Print the current pipeline                                                                                    */ 
/************************************************************/
void print_program(){
	int i;
	uint32_t addr;
	for(i=0; i<PROGRAM_SIZE; i++)
	{
		addr = MEM_TEXT_BEGIN + (i*4);
		printf("[0x%x]\t", addr);
		print_instruction(addr);
	}
}
/***************************************************************/
/* main                                                                                                                                   */
/***************************************************************/
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

