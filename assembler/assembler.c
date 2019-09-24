#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>

const char * instruction[44]={"ADD","ADDU","ADDI","ADDIU","SUB","SUBU","MULT","MULTU","DIV","DIVU","AND","ANDI","OR","ORI","XOR","XORI","NOR","SLT","SLTI","SLL","SRL","SRA","LW","LB","LH","LUI","SW","SB","SH","MFHI","MFLO","MTHI","MTLO","BEQ","BNE","BLEZ","BLTZ","BGEZ","BGTZ","J","JR","JAL","JALR","SYSCALL\n"};

const char * opcode[44]={"000000","000000","001000","001001","000000","000000","000000","000000","000000","000000","000000","001100","000000","001101","000000","001110","000000","000000","001010","000000","000000","000000","100011","100000","100001","001111","101011","101000","101001","000000","000000","000000","000000","000100","000101","000110","000001","000001","000111","000010","000000","000011","000000","0000000c"};

const char * regname[59] = {"0X2\n","0X1001\n","ZERO","AT","V0","V1","A0","A1","A2","A3","T0","T1","T2","T3","T4","T5","T6","T7","S0","S1","S2","S3","S4","S5","S6","S7","T8","T9","K0","K1","GP","SP","FP","RA","0(","0X1(","0X2(","0X3(","0X4(","0X5(","0X6(","0X7(","0X8(","0X9(","0XA(","0XB(","0XC(","0XD(","0XE(","0XF(","0X10(","0X14(","0X18(","0X1C(","0X20(","0X24(","-4(","-8(","-12("};

const char * regnum[59] = {"00010","0001000000000001","00000","00001","00010","00011","00100","00101","00110","00111","01000","01001","01010","01011","01100","01101","01110","01111","10000","10001","10010","10011","10100","10101","10110","10111","11000","11001","11010","11011","11100","11101","11110","11111","00000","00001","00010","00011","00100","00101","00110","00111","01000","01001","01010","01011","01100","01101","01110","01111","10000","10100","11000","10100","11000","1111111111111111","1111111111111110","1111111111111100"};

const char * hex1[59] = {"A0\n","T0\n","T1\n","T2\n","0X1001\n","0\n","0X1\n","0X2\n","0X3\n","0X4\n","0X5\n","0X6\n","0X7\n","0X8\n","0X9\n","0XA\n","0XB\n","0XC\n","0XD\n","0XE\n","0XF\n","-4\n","-8\n","-12\n","-16\n","-32\n","-56\n","ZERO)\n","AT)\n","V0)\n","V1)\n","A0)\n","A1)\n","A2)\n","A3)\n","T0)\n","T1)\n","T2)\n","T3)\n","T4)\n","T5)\n","T6)\n","T7)\n","S0)\n","S1)\n","S2)\n","S3)\n","S4)\n","S5)\n","S6)\n","S7)\n","T8)\n","T9)\n","K0)\n","K1)\n","GP)\n","SP)\n","FP)\n","RA)\n"};

const char * binar[59] ={"00100","01000","01001","01010","0001000000000001","00000","00001","00010","00011","00100","00101","00110","00111","01000","01001","01010","01011","01100","01101","01110","01111","1111111111111111","1111111111111110","1111111111111100","1111111111111011","1111111111110111","1111111111110000","00000","00001","00010","00011","00100","00101","00110","00111","01000","01001","01010","01011","01100","01101","01110","01111","10000","10001","10010","10011","10100","10101","10110","10111","11000","11001","11010","11011","11100","11101","11110","11111"};


/* binary to hex converter: */
char *padBinary(char *, int );
char *binaryToHex(char *, int );
int validate(char *hex, int length);
char valueOf(char *halfByte);
void hex_print(char *, char *);

int main(int argc, char *argv[])
 {
 	size_t buffer_size = 30;
	char *buffer = malloc(buffer_size * sizeof(char));
    	int i = 0, j = 0;
    	char *arg0 = malloc(10 * sizeof(char));
    	char *ins = malloc(10 * sizeof(char));
	char *arg1 = malloc(10 * sizeof(char));
	char *arg2 = malloc(10 * sizeof(char));
	char *arg3 = malloc(10 * sizeof(char));
	char *argnum1 = malloc(17 * sizeof(char));
	char *argnum2 = malloc(17 * sizeof(char));
	char *argnum3 = malloc(17 * sizeof(char));
	char *stream = malloc(32 * sizeof(char));
	char *hexa = malloc(8 * sizeof(char));
	char *hex = malloc(8 * sizeof(char));
	//char *temp = malloc(32 * sizeof(char));

   	if (argc < 2) {
		printf("Error: You should provide input file.\nUsage: %s <input program> \n\n",  argv[0]);
		exit(1);
		}

	FILE * fp;

	fp = fopen(argv[1], "r");
	if (fp == NULL) {
		printf("Error: Can't open program file %s\n", argv[1]);
		exit(-1);
					}
    	
    while (getline(&buffer, &buffer_size, fp) != -1)

    {    	
   		char *token;
        	token = malloc(20 * sizeof(char));
        	//printf("%s", buffer);		// test buffer
		token = strtok (buffer, " ,.$");
        						       	
		while (token != NULL)			
   	 	{
			
			if(j == 0)
			{				
				for (i=0;i<44;i++)
				{				 
					if( strcmp(token, instruction[i]) == 0 )  
					{
							
					strcpy(arg0, opcode[i]);
					strcpy(ins, token);
					//printf("arg0: %s\n",token);
					break;
					}
				 
				} 
			}
			else if(j == 1) //argument 1
			{
				strcpy(arg1, token);
				//printf("%s\n",arg1);
				for (i=0;i<32;i++)
				{				 
					if( strcmp(arg1, regname[i]) == 0 ) 
					{						
					strcpy(argnum1, regnum[i]);
					//printf("arg1: %s\n",arg1);
					break;
					}
				}
			}

			else if(j == 2) //argument 2
			{
				strcpy(arg2, token);
				//printf("%s\n",arg2);
				for (i=0;i<54;i++)
				{	
					//strcat(strcat(binar, "("),;	
					//temp = binar[i];|| strcmp(arg2,strcat(temp,"("))
					if( strcmp(arg2, regname[i]) == 0 ) 
					{						
					strcpy(argnum2, regnum[i]);
					//printf("arg2: %s\n",arg2);
					break;
					}
				
				}
			}

			else if(j == 3) //argument 3
			{
				strcpy(arg3, token);
				//printf("arg3: %s\n",arg3);
				//printf("hex1: %s\n",hex1[0]);	
				for (i=0;i<53;i++)
				{	
					//temp = regname[i];
					//printf("arg3:%s",arg3,hex1[]);			 
					if( strcmp(arg3, hex1[i]) == 0)  
					{
					//printf("%s\n",arg3);
					//printf("arg3 num: %s, %d , %s\n",hex1[i], i, binar[i]);					
					strcpy(argnum3, binar[i]);
					
					break;
					}
				
				}
			}
			else 
				;
			j++;
			token = strtok (NULL, " ,.$");
			//printf("%d\n", j);	    
		}
		j = 0;
	//printf("%s\n",ins);
	
	if( strcmp(ins, "ADD") == 0 ) 
	{
		strcpy(stream, arg0);
		strcat(stream, argnum2);
		strcat(stream, argnum3);		
		strcat(stream, argnum1);
		strcat(stream, "00000100000");
		//printf("stream:%s\n",stream); // print binary
		hexa = binaryToHex(stream,32); 
		hex_print(hex,hexa);	// print hex
	}
	
	if( strcmp(ins, "ADDU") == 0 )  
	{
		strcpy(stream, arg0);
		strcat(stream, argnum2);
		strcat(stream, argnum3);		
		strcat(stream, argnum1);
		strcat(stream, "00000100001");
		//printf("stream:%s \n",stream); 
		hexa = binaryToHex(stream,32);
		hex_print(hex,hexa);
	}
	
	if( strcmp(ins, "ADDI") == 0 )  
	{
		strcpy(stream, arg0);
		strcat(stream, argnum2);
		strcat(stream, argnum1);
		if(strcmp( arg3, "-4") != 0 || strcmp(arg3, "-8") != 0 || strcmp(arg3, "-12") != 0 || strcmp(arg3, "0X1001") != 0)
			strcat(stream, "00000000000");		
		strcat(stream, argnum3);
		//printf("stream:%s\n",stream);
		hexa = binaryToHex(stream,32);
		hex_print(hex,hexa);
	}
	
	if( strcmp(ins, "ADDIU") == 0 )  
	{
		strcpy(stream, arg0);
		strcat(stream, argnum2);
		strcat(stream, argnum1);
		if(strcmp( arg3, "-4") != 0 || strcmp(arg3, "-8") != 0 || strcmp(arg3, "-12") != 0)
			strcat(stream, "00000000000");
		strcat(stream, argnum3);
		//printf("stream:%s\n",stream);
		hexa = binaryToHex(stream,32);
		hex_print(hex,hexa);
	}
	
	if( strcmp(ins, "SUB") == 0 )  
	{
		strcpy(stream, arg0);
		strcat(stream, argnum2);
		strcat(stream, argnum3);		
		strcat(stream, argnum1);
		strcat(stream, "00000100010");
		//printf("stream:%s\n",stream);
		hexa = binaryToHex(stream,32);
		hex_print(hex,hexa);
	}
	
	if( strcmp(ins, "SUBU") == 0 )  
	{
		strcpy(stream, arg0);
		strcat(stream, argnum2);
		strcat(stream, argnum3);		
		strcat(stream, argnum1);
		strcat(stream, "00000100011");
		//printf("stream:%s\n",stream);
		hexa = binaryToHex(stream,32);
		hex_print(hex,hexa);
	}
	
	
	if( strcmp(ins, "MULT") == 0 )  
	{
		strcpy(stream, arg0);
		strcat(stream, argnum1);		
		strcat(stream, argnum2);
		strcat(stream, "0000000000");
		strcat(stream, "011000");
		//printf("stream:%s\n",stream);
		hexa = binaryToHex(stream,32);
		hex_print(hex,hexa);
	}
	
	if( strcmp(ins, "MULTU") == 0 )  
	{
		strcpy(stream, arg0);
		strcat(stream, argnum1);		
		strcat(stream, argnum2);
		strcat(stream, "0000000000");
		strcat(stream, "011001");
		//printf("stream:%s\n",stream);
		hexa = binaryToHex(stream,32);
		hex_print(hex,hexa);
	}
	
	if( strcmp(ins, "DIV") == 0 )  
	{
		strcpy(stream, arg0);
		strcat(stream, argnum1);		
		strcat(stream, argnum2);
		strcat(stream, "0000000000");
		strcat(stream, "011010");
		//printf("stream:%s\n",stream);
		hexa = binaryToHex(stream,32);
		hex_print(hex,hexa);
	}
	
	if( strcmp(ins, "DIVU") == 0 )  
	{
		strcpy(stream, arg0);
		strcat(stream, argnum1);		
		strcat(stream, argnum2);
		strcat(stream, "0000000000");
		strcat(stream, "011011");
		//printf("stream:%s\n",stream);
		hexa = binaryToHex(stream,32);
		hex_print(hex,hexa);
	}
	
	if( strcmp(ins, "AND") == 0 )  
	{
		strcpy(stream, arg0);
		strcat(stream, argnum2);
		strcat(stream, argnum3);
		strcat(stream, argnum1);
		strcat(stream, "00000100100");
		//printf("stream:%s\n",stream);
		hexa = binaryToHex(stream,32);
		hex_print(hex,hexa);
	}
	
	if( strcmp(ins, "ANDI") == 0 )  
	{
		strcpy(stream, arg0);
		strcat(stream, argnum2);
		strcat(stream, argnum1);
		if(strcmp( arg3, "-4") != 0 || strcmp(arg3, "-8") != 0 || strcmp(arg3, "-12") != 0)
			strcat(stream, "00000000000");
		strcat(stream, argnum3);
		//printf("stream:%s\n",stream);
		hexa = binaryToHex(stream,32);
		hex_print(hex,hexa);
	}
	
	if( strcmp(ins, "OR") == 0 )  
	{
		strcpy(stream, arg0);
		strcat(stream, argnum2);
		strcat(stream, argnum3);
		strcat(stream, argnum1);
		strcat(stream, "00000100101");
		//printf("stream:%s\n",stream);
		hexa = binaryToHex(stream,32);
		hex_print(hex,hexa);
	}
	
	if( strcmp(ins, "ORI") == 0 )  
	{
		strcpy(stream, arg0);
		strcat(stream, argnum2);
		strcat(stream, argnum1);
		if(strcmp( arg3, "-4") != 0 || strcmp(arg3, "-8") != 0 || strcmp(arg3, "-12") != 0)
			strcat(stream, "00000000000");
		strcat(stream, argnum3);
		//printf("stream:%s\n",stream);
		hexa = binaryToHex(stream,32);
		hex_print(hex,hexa);
	}
	
	if( strcmp(ins, "XOR") == 0 )  
	{
		strcpy(stream, arg0);
		strcat(stream, argnum2);
		strcat(stream, argnum3);
		strcat(stream, argnum1);
		strcat(stream, "00000100110");
		//printf("stream:%s\n",stream);
		hexa = binaryToHex(stream,32);
		hex_print(hex,hexa);
	}
	
	if( strcmp(ins, "XORI") == 0 )  
	{
		strcpy(stream, arg0);
		strcat(stream, argnum2);
		strcat(stream, argnum1);
		if(strcmp( arg3, "-4") != 0 || strcmp(arg3, "-8") != 0 || strcmp(arg3, "-12") != 0)
			strcat(stream, "00000000000");
		strcat(stream, argnum3);
		//printf("stream:%s\n",stream);
		hexa = binaryToHex(stream,32);
		hex_print(hex,hexa);
	}
	
	if( strcmp(ins, "NOR") == 0 )  
	{
		strcpy(stream, arg0);
		strcat(stream, argnum2);
		strcat(stream, argnum3);
		strcat(stream, argnum1);
		strcat(stream, "00000100111");
		//printf("stream:%s\n",stream);
		hexa = binaryToHex(stream,32);
		hex_print(hex,hexa);
	}
	
	if( strcmp(ins, "SLT") == 0 )  
	{
		strcpy(stream, arg0);
		strcat(stream, argnum2);
		strcat(stream, argnum3);
		strcat(stream, argnum1);
		strcat(stream, "00000101010");
		//printf("stream:%s\n",stream);
		hexa = binaryToHex(stream,32);
		hex_print(hex,hexa);
	}
	
	if( strcmp(ins, "SLTI") == 0 )  
	{
		strcpy(stream, arg0);
		strcat(stream, argnum2);
		strcat(stream, argnum1);
		if(strcmp( arg3, "-4") != 0 || strcmp(arg3, "-8") != 0 || strcmp(arg3, "-12") != 0)
			strcat(stream, "00000000000");
		strcat(stream, argnum3);
		//printf("stream:%s\n",stream);
		hexa = binaryToHex(stream,32);
		hex_print(hex,hexa);
	}
	
	if( strcmp(ins, "SLL") == 0 )  
	{
		strcpy(stream, arg0);
		strcat(stream, "00000");
		strcat(stream, argnum2);
		strcat(stream, argnum1);
		strcat(stream, argnum3);
		strcat(stream, "000000");
		//printf("stream:%s\n",stream);
		hexa = binaryToHex(stream,32);
		hex_print(hex,hexa);
	}
	
	if( strcmp(ins, "SRL") == 0 )  
	{
		strcpy(stream, arg0);
		strcat(stream, "00000");
		strcat(stream, argnum2);
		strcat(stream, argnum1);
		strcat(stream, argnum3);
		strcat(stream, "000010");
		//printf("stream:%s\n",stream);
		hexa = binaryToHex(stream,32);
		hex_print(hex,hexa);
	}
	
	if( strcmp(ins, "SRA") == 0 )  
	{
		strcpy(stream, arg0);
		strcat(stream, "00000");
		strcat(stream, argnum2);
		strcat(stream, argnum1);
		strcat(stream, argnum3);
		strcat(stream, "000011");
		//printf("stream:%s\n",stream);
		hexa = binaryToHex(stream,32);
		hex_print(hex,hexa);
	}
	
	if( strcmp(ins, "LW") == 0 )  
	{
		strcpy(stream, arg0);
		strcat(stream,argnum3);
		strcat(stream,argnum1);
		if(strcmp( arg2, "-4") != 0 || strcmp(arg2, "-8") != 0 || strcmp(arg2, "-12") != 0)
			strcat(stream, "00000000000");
		strcat(stream,argnum2);
		//printf("stream:%s\n",stream);
		hexa = binaryToHex(stream,32);
		hex_print(hex,hexa);
	}
	
	if( strcmp(ins, "LB") == 0 )  
	{
		strcpy(stream, arg0);
		strcat(stream,argnum3);
		strcat(stream,argnum1);
		if(strcmp( arg2, "-4") != 0 || strcmp(arg2, "-8") != 0 || strcmp(arg2, "-12") != 0)
			strcat(stream, "00000000000");
		strcat(stream,argnum2);
		//printf("stream:%s\n",stream);
		hexa = binaryToHex(stream,32);
		hex_print(hex,hexa);
	}
	
	if( strcmp(ins, "LH") == 0 )  
	{
		strcpy(stream, arg0);
		strcat(stream,argnum3);
		strcat(stream,argnum1);
		if(strcmp( arg2, "-4") != 0 || strcmp(arg2, "-8") != 0 || strcmp(arg2, "-12") != 0)
			strcat(stream, "00000000000");
		strcat(stream,argnum2);
		//printf("stream:%s\n",stream);
		hexa = binaryToHex(stream,32);
		hex_print(hex,hexa);
	}
	
	if( strcmp(ins, "LUI") == 0 )  
	{
		strcpy(stream, arg0);
		strcat(stream, "00000");
		strcat(stream, argnum1);
		//strcat(stream, "00000000000");
		//printf("arg2:%s ", arg2);
		strcat(stream, argnum2);
		//printf("stream:%s\n",stream);
		hexa = binaryToHex(stream,32);
		hex_print(hex,hexa);
	}
	
	if( strcmp(ins, "SW") == 0 )  
	{
		strcpy(stream, arg0);
		strcat(stream,argnum3);
		//printf("args: %s %s %s",arg1,arg2,arg3);
		strcat(stream,argnum1);
		/*if(strcmp( arg2, "-4") != 0 || strcmp(arg2, "-8") != 0 || strcmp(arg2, "-12") != 0)
			strcat(stream, "00000000000");*/
		strcat(stream, "00000000000");
		strcat(stream,argnum2);
		//printf("stream:%s\n",stream);
		hexa = binaryToHex(stream,32);
		hex_print(hex,hexa);
	}

	/* J JAL?*/	

	if( strcmp(ins, "SB") == 0 )  
	{
		strcpy(stream, arg0);
		strcat(stream,argnum3);
		strcat(stream,argnum1);
		if(strcmp( arg2, "-4") != 0 || strcmp(arg2, "-8") != 0 || strcmp(arg2, "-12") != 0)
			strcat(stream, "00000000000");
		strcat(stream,argnum2);
		//printf("stream:%s\n",stream);
		hexa = binaryToHex(stream,32);
		hex_print(hex,hexa);
	}
	
	if( strcmp(ins, "SH") == 0 )  
	{
		strcpy(stream, arg0);
		strcat(stream,argnum3);
		strcat(stream,argnum1);
		if(strcmp( arg2, "-4") != 0 || strcmp(arg2, "-8") != 0 || strcmp(arg2, "-12") != 0)
			strcat(stream, "00000000000");
		strcat(stream,argnum2);
		//printf("stream:%s\n",stream);
		hexa = binaryToHex(stream,32);
		hex_print(hex,hexa);
	}
	
	if( strcmp(ins, "MFHI") == 0 )  
	{
		strcpy(stream, arg0);
		strcat(stream, "0000000000");
		strcat(stream, argnum1);
		strcat(stream, "00000010000");
		//printf("stream:%s\n",stream);
		hexa = binaryToHex(stream,32);
		hex_print(hex,hexa);
	}
	
	if( strcmp(ins, "MFLO") == 0 )  
	{
		strcpy(stream, arg0);
		strcat(stream, "0000000000");
		strcat(stream, argnum1);
		strcat(stream, "00000010010");
		//printf("stream:%s\n",stream);
		hexa = binaryToHex(stream,32);
		hex_print(hex,hexa);
	}	
	
	if( strcmp(ins, "MTHI") == 0 )  
	{
		strcpy(stream, arg0);
		strcat(stream, argnum1);
		strcat(stream, "000000000000000010001");
		//printf("stream:%s\n",stream);
		hexa = binaryToHex(stream,32);
		hex_print(hex,hexa);
	}	
	
	if( strcmp(ins, "MTLO") == 0 )  
	{
		strcpy(stream, arg0);
		strcat(stream, argnum1);
		strcat(stream, "000000000000000010011");
		//printf("stream:%s\n",stream);
		hexa = binaryToHex(stream,32);
		hex_print(hex,hexa);
	}	
	
	if( strcmp(ins, "BEQ") == 0 )  
	{
		strcpy(stream, arg0);
		strcat(stream, argnum1);
		strcat(stream, argnum2);
		if(strcmp( arg3, "-4") != 0 || strcmp(arg3, "-8") != 0 || strcmp(arg3, "-12") != 0)
			strcat(stream, "00000000000");
		strcat(stream, argnum3);
		//printf("stream:%s\n",stream);
		hexa = binaryToHex(stream,32);
		hex_print(hex,hexa);
	}
	
	
	
	if( strcmp(ins, "BNE") == 0 )  
	{
		strcpy(stream, arg0);
		strcat(stream, argnum1);
		strcat(stream, argnum2);
		strcat(stream, argnum3);
		//printf("arg3: %s, arg3:%s",arg3,argnum3);
		//printf("stream:%s\n",stream);
		hexa = binaryToHex(stream,32);
		hex_print(hex,hexa);
	}	
	
	if( strcmp(ins, "BLEZ") == 0 )  
	{
		strcpy(stream, arg0);
		strcat(stream, argnum1);
		strcat(stream, "00000");
		if(strcmp( arg2, "-4") != 0 || strcmp(arg2, "-8") != 0 || strcmp(arg2, "-12") != 0)
			strcat(stream, "00000000000");
		strcat(stream, argnum2);
		//printf("stream:%s\n",stream);
		hexa = binaryToHex(stream,32);
		hex_print(hex,hexa);
	}
	
	if( strcmp(ins, "BLTZ") == 0 )  
	{
		strcpy(stream, arg0);
		strcat(stream, argnum1);
		strcat(stream, "00000");
		strcat(stream, "00000000000");
		strcat(stream, argnum2);
		//printf("stream:%s\n",stream);
		hexa = binaryToHex(stream,32);
		hex_print(hex,hexa);
	}
	
	if( strcmp(ins, "BGEZ") == 0 )  
	{
		strcpy(stream, arg0);
		strcat(stream, argnum1);
		strcat(stream, "00001");
		if(strcmp( arg2, "-4") != 0 || strcmp(arg2, "-8") != 0 || strcmp(arg2, "-12") != 0)
			strcat(stream, "00000000000");
		strcat(stream, argnum2);
		//printf("stream:%s\n",stream);
		hexa = binaryToHex(stream,32);
		hex_print(hex,hexa);
	}
	
	if( strcmp(ins, "BGTZ") == 0 )  
	{
		strcpy(stream, arg0);
		strcat(stream, argnum1);
		strcat(stream, "00000");
		if(strcmp( arg2, "-4") != 0 || strcmp(arg2, "-8") != 0 || strcmp(arg2, "-12") != 0)
			strcat(stream, "00000000000");
		strcat(stream, argnum2);
		//printf("stream:%s\n",stream);
		hexa = binaryToHex(stream,32);
		hex_print(hex,hexa);
	}
	
	if( strcmp(ins, "J") == 0 )  
	{
		;
	}
	
	if( strcmp(ins, "JR") == 0 )  
	{
		strcpy(stream, arg0);
		strcat(stream, argnum1);
		strcat(stream, "000000000000000001000");
		//printf("stream:%s\n",stream);
		hexa = binaryToHex(stream,32);
		hex_print(hex,hexa);
	}
	
	if( strcmp(ins, "JAL") == 0 )  
	{
		;
	}
	/*J JAL?*/
	if( strcmp(ins, "JALR") == 0 )  
	{	
		strcpy(stream, arg0);
		if(j >= 2)  // JALR rd, rs
		{
			strcat(stream, argnum2);
			strcat(stream, "00000");
			strcat(stream, argnum1);
			strcat(stream, "00000001001");
		}
		else 
		{
			strcat(stream, argnum1);
			strcat(stream, "000000000000000001001");
		}
		//printf("stream:%s\n",stream);
		hexa = binaryToHex(stream,32);
		hex_print(hex,hexa);
	}
	
/*"1ADD","1ADDU","1ADDI","1ADDIU","1SUB","1SUBU","1MULT","1MULTU","1DIV","1DIVU","1AND","1ANDI","1OR","1ORI","1XOR","1XORI","1NOR","1SLT","1SLTI","1SLL","1SRL","1SRA","LW","LB","LH","1LUI","SW","SB","SH","1MFHI","1MFLO","1MTHI","1MTLO","1BEQ","1BNE","1BLEZ","1BLTZ","1BGEZ","1BGTZ","J","1JR","JAL","1JALR","SYSCALL"*/
	
	

	if( strcmp(ins, "SYSCALL\n") == 0 )  
	{	
		
		strcpy(stream, "0000000c");
		printf("%s\n",stream);
	} 
	
        free(arg0) ;
	free(token) ;	
	}
	
	fclose(fp);
	free(ins) ;
	free(arg1) ;
	free(arg2) ;
	free(arg3) ;
	free(buffer) ;
	free(argnum1) ;
	free(argnum2) ;
	free(argnum3) ;
	return 0;
}

 void hex_print(char *hex, char *hexa)
 {
	 char *np = hex, *op = hexa;
	 do {
		if (*op != ' ')
			*np++ = *op;
		} while (*op++);
	printf("%s\n", hex);
 }
 
 char *padBinary(char *binary, int length)
    {
        int i;
        int padding = 8 - length % 8;
        int paddedBinaryLength = padding + length + 1;
        char *paddedBinary;
        if(padding == 8 || padding == 0)
            return binary;
        paddedBinary = (char *)malloc(paddedBinaryLength);
        if(paddedBinary == NULL)
            exit(1);
        for(i = 0; i < padding; ++i)
            *paddedBinary++ = '0';
        while(*binary != '\0')
            *paddedBinary++ = *binary++;
        *paddedBinary = '\0';
        paddedBinary -= paddedBinaryLength - 1;
        return paddedBinary;
    }

char *binaryToHex(char *binary, int length)
    {
        int i;
        char *hex;
        char *paddedBinary = padBinary(binary, length);
        int hexLength = strlen(paddedBinary) / 4; // We need one hex symbol for every 4 binary symbols
        hexLength = hexLength + hexLength/2 + 1; // Make place for a space after every two symbols + the null terminater
        hex = (char *)malloc(hexLength);
        if(hex == NULL)
            exit(1);
        for(i = 0; i < length; i += 8)
        {
            char halfByte[5];
			// copy the 4 binary digits and decode them to one hex digit
            memcpy(halfByte, paddedBinary, 4);
            halfByte[4] = '\0';
            *hex++ = valueOf(halfByte);
            paddedBinary += 4;

            memcpy(halfByte, paddedBinary, 4);
            halfByte[4] = '\0';
            *hex++ = valueOf(halfByte);
            paddedBinary += 4;

            *hex++ = ' ';
        }
        *hex = '\0';
        hex -= hexLength - 1;
        return hex;
    }
	
int validate(char *binary, int length)
{
    while(*binary != '\0')
    {
        if(*binary == '0' || *binary == '1')
        {
            binary++;
        }
        else
        {
            printf("The input should be a hexadecimal number, containing only digits(0-9) and the first 6 letters(a-f).\n");
            return 0;
        }
    }
    
    return 1;
}

char valueOf(char *halfByte)
{
	if(strcmp(halfByte, "0000") == 0)
		return '0';
	if(strcmp(halfByte, "0001") == 0)
		return '1';
	if(strcmp(halfByte, "0010") == 0)
		return '2';
	if(strcmp(halfByte, "0011") == 0)
		return '3';
	if(strcmp(halfByte, "0100") == 0)
		return '4';
	if(strcmp(halfByte, "0101") == 0)
		return '5';
	if(strcmp(halfByte, "0110") == 0)
		return '6';
	if(strcmp(halfByte, "0111") == 0)
		return '7';
	if(strcmp(halfByte, "1000") == 0)
		return '8';
	if(strcmp(halfByte, "1001") == 0)
		return '9';
	if(strcmp(halfByte, "1010") == 0)
		return 'A';
	if(strcmp(halfByte, "1011") == 0)
		return 'B';
	if(strcmp(halfByte, "1100") == 0)
		return 'C';
	if(strcmp(halfByte, "1101") == 0)
		return 'D';
	if(strcmp(halfByte, "1110") == 0)
		return 'E';
	if(strcmp(halfByte, "1111") == 0)
		return 'F';
	return 0;
}
