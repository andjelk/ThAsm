/*
 * Thontelix Assembler - /main.cpp
 */
#include<cstdio>
#include<cstdlib>
#include<cstring>
#include<vector>
#include<malloc.h>
#include<cctype>
#include<getopt.h>
struct option option_table[]=
{
		{"format", required_argument, 0, 'f'},
		{"arch", required_argument, 0, 'a'},
		{"if", required_argument, 0, 'i'},
		{"of", required_argument, 0, 'o'},
		{"help", no_argument, 0, 'h'},
		{0, 0, 0, 0}
};
bool __eof__=false;
inline void end_f()
{
	__eof__=true;
}
char *asm_inst_text[]=
{
	"end"
};
const unsigned int asm_inst_c=sizeof(asm_inst_text)/sizeof(asm_inst_text[0]);
void (*asm_inst[asm_inst_c])()=
{
	end_f
};

unsigned int compileErrors;
char usage_str[]="Usage: %s [f,format] [a,arch] [i,if input_file] [o,of output_file]\n";
char vstring[]="0.0.1";
FILE* usage_prompt_io=stderr;
FILE* ifl;
FILE* ofl;
char* ifile;
char* ofile;
#define _HALT 0x00
#define _MOV 0x01
#define _AND 0x02
#define _XOR 0x03
#define _OR 0x04
#define _NOT 0x05
#define _ADD 0x06
#define _SUB 0x07
#define _IO_SEND 0x08
#define _IO_REC 0x09
#define UNDEFINED_INSTRUCTION 0xFF
#define BYTE 1
#define WORD 2
#define DWORD 4
#define QWORD 8
struct instruction
{
	unsigned char opcode;
	char text[12];
	unsigned char oper_c;
	unsigned char opcode_ex;
	unsigned short opcode_op;
	unsigned char opcode_t;
	unsigned long long int mnemonic[2];
};
instruction inst[]=
{
	{_HALT, "halt", 1},
	{_MOV, "mov", 4},
	{_AND, "and", 4},
	{_XOR, "xor", 4},
	{_OR, "or", 4},
	{_NOT, "not", 4},
	{_ADD, "add", 4},
	{_SUB, "sub", 4},
	{_IO_SEND, "io_send", 4},
	{_IO_REC, "io_rec", 4},
	{UNDEFINED_INSTRUCTION, "undef", 1}
};
const int instruc_c=sizeof(inst)/sizeof(inst[0]);
#include<vector>
std::vector<instruction> obuf;
enum file_format
{
	PURE_BINARY=1
}form;
enum architecture
{
	TH_32=1,
	TH_64=2
}arch;
#define O_D_ISREG 1
#define O_S_ISREG 2
#define O_SIZE 12
#define O_SIZE_8BIT 0
#define O_SIZE_16BIT 4
#define O_SIZE_32BIT 8
#define O_SIZE_64BIT 12
#define O_D_INDEX_64BIT 16
#define O_D_INDEX_32BIT 32
#define O_D_INDEX_16BIT 64
#define O_S_INDEX_64BIT 128
#define O_S_INDEX_32BIT 256
#define O_S_INDEX_16BIT 512
#define O_D_ADD_MNEM 1024
#define O_S_ADD_MNEM 2048
#define O_D_NOT_REG_POINTER 4096
#define O_S_NOT_REG_POINTER 8192
#define O_S_NULL_REG 16384
#define O_PRESENT 32768
#define DestinationOp 0
#define SourceOp 1
#define O_D_A_REGISTER 0b0000
#define O_D_B_REGISTER 0b0001
#define O_D_C_REGISTER 0b0010
#define O_D_D_REGISTER 0b0011
#define O_D_EIP_REGISTER 0b0100
#define O_D_EPAR_REGISTER 0b1000
#define O_D_ESAR_REGISTER 0b1100
#define O_D_ETAR_REGISTER 0b1001
#define O_D_EFLAGS_REGISTER 0b1010
#define O_S_A_REGISTER 0b0000*0b10000
#define O_S_B_REGISTER 0b0001*0b10000
#define O_S_C_REGISTER 0b0010*0b10000
#define O_S_D_REGISTER 0b0011*0b10000
#define O_S_EIP_REGISTER 0b0100*0b10000
#define O_S_EPAR_REGISTER 0b1000*0b10000
#define O_S_ESAR_REGISTER 0b1100*0b10000
#define O_S_ETAR_REGISTER 0b1001*0b10000
#define O_S_EFLAGS_REGISTER 0b1010*0b10000
#define O_OP_SZ 65536
char *file_format_ext[]=
{
	"\0","bin"
};
char *reg_text[]=
{
	"eax", "ebx", "ecx", "edx", "epar", "esar", "etar", "eip", "eflags",//32 bit
	"axl", "bxl", "cxl", "dxl", "axh", "bxh", "cxh", "dxh", //16 bit low
	"all", "bll", "cll", "dll", "alh", "blh", "clh", "dlh", //8 bit low < low
	"ahl", "bhl", "chl", "dhl", "ahh", "bhh", "chh", "dhh" //8 bit high < low
};
unsigned int reg_conf_dest[]=
{//Destination
	O_D_A_REGISTER*O_OP_SZ,
	O_D_B_REGISTER*O_OP_SZ,
	O_D_C_REGISTER*O_OP_SZ,
	O_D_D_REGISTER*O_OP_SZ,
	O_D_EPAR_REGISTER*O_OP_SZ,
	O_D_ESAR_REGISTER*O_OP_SZ,
	O_D_ETAR_REGISTER*O_OP_SZ,
	O_D_EIP_REGISTER*O_OP_SZ,
	O_D_EFLAGS_REGISTER*O_OP_SZ,
	O_D_A_REGISTER*O_OP_SZ,
	O_D_B_REGISTER*O_OP_SZ,
	O_D_C_REGISTER*O_OP_SZ,
	O_D_D_REGISTER*O_OP_SZ,
	O_D_A_REGISTER*O_OP_SZ|O_D_INDEX_32BIT,
	O_D_B_REGISTER*O_OP_SZ|O_D_INDEX_32BIT,
	O_D_C_REGISTER*O_OP_SZ|O_D_INDEX_32BIT,
	O_D_D_REGISTER*O_OP_SZ|O_D_INDEX_32BIT,
	O_D_A_REGISTER*O_OP_SZ,
	O_D_B_REGISTER*O_OP_SZ,
	O_D_C_REGISTER*O_OP_SZ,
	O_D_D_REGISTER*O_OP_SZ,
	O_D_A_REGISTER*O_OP_SZ|O_D_INDEX_32BIT,
	O_D_B_REGISTER*O_OP_SZ|O_D_INDEX_32BIT,
	O_D_C_REGISTER*O_OP_SZ|O_D_INDEX_32BIT,
	O_D_D_REGISTER*O_OP_SZ|O_D_INDEX_32BIT,
	O_D_A_REGISTER*O_OP_SZ|O_D_INDEX_16BIT,
	O_D_B_REGISTER*O_OP_SZ|O_D_INDEX_16BIT,
	O_D_C_REGISTER*O_OP_SZ|O_D_INDEX_16BIT,
	O_D_D_REGISTER*O_OP_SZ|O_D_INDEX_16BIT,
	O_D_A_REGISTER*O_OP_SZ|O_D_INDEX_32BIT|O_D_INDEX_16BIT,
	O_D_B_REGISTER*O_OP_SZ|O_D_INDEX_32BIT|O_D_INDEX_16BIT,
	O_D_C_REGISTER*O_OP_SZ|O_D_INDEX_32BIT|O_D_INDEX_16BIT,
	O_D_D_REGISTER*O_OP_SZ|O_D_INDEX_32BIT|O_D_INDEX_16BIT
};
unsigned int reg_conf_src[]=
{//Source
	O_S_A_REGISTER*O_OP_SZ,
	O_S_B_REGISTER*O_OP_SZ,
	O_S_C_REGISTER*O_OP_SZ,
	O_S_D_REGISTER*O_OP_SZ,
	O_S_EPAR_REGISTER*O_OP_SZ,
	O_S_ESAR_REGISTER*O_OP_SZ,
	O_S_ETAR_REGISTER*O_OP_SZ,
	O_S_EIP_REGISTER*O_OP_SZ,
	O_S_EFLAGS_REGISTER*O_OP_SZ,
	O_S_A_REGISTER*O_OP_SZ,
	O_S_B_REGISTER*O_OP_SZ,
	O_S_C_REGISTER*O_OP_SZ,
	O_S_D_REGISTER*O_OP_SZ,
	O_S_A_REGISTER*O_OP_SZ|O_S_INDEX_32BIT,
	O_S_B_REGISTER*O_OP_SZ|O_S_INDEX_32BIT,
	O_S_C_REGISTER*O_OP_SZ|O_S_INDEX_32BIT,
	O_S_D_REGISTER*O_OP_SZ|O_S_INDEX_32BIT,
	O_S_A_REGISTER*O_OP_SZ,
	O_S_B_REGISTER*O_OP_SZ,
	O_S_C_REGISTER*O_OP_SZ,
	O_S_D_REGISTER*O_OP_SZ,
	O_S_A_REGISTER*O_OP_SZ|O_S_INDEX_32BIT,
	O_S_B_REGISTER*O_OP_SZ|O_S_INDEX_32BIT,
	O_S_C_REGISTER*O_OP_SZ|O_S_INDEX_32BIT,
	O_S_D_REGISTER*O_OP_SZ|O_S_INDEX_32BIT,
	O_S_A_REGISTER*O_OP_SZ|O_S_INDEX_16BIT,
	O_S_B_REGISTER*O_OP_SZ|O_S_INDEX_16BIT,
	O_S_C_REGISTER*O_OP_SZ|O_S_INDEX_16BIT,
	O_S_D_REGISTER*O_OP_SZ|O_S_INDEX_16BIT,
	O_S_A_REGISTER*O_OP_SZ|O_S_INDEX_32BIT|O_S_INDEX_16BIT,
	O_S_B_REGISTER*O_OP_SZ|O_S_INDEX_32BIT|O_S_INDEX_16BIT,
	O_S_C_REGISTER*O_OP_SZ|O_S_INDEX_32BIT|O_S_INDEX_16BIT,
	O_S_D_REGISTER*O_OP_SZ|O_S_INDEX_32BIT|O_S_INDEX_16BIT
};
unsigned int reg_conf_sz[]=
{//Size
	O_SIZE_32BIT,
	O_SIZE_32BIT,
	O_SIZE_32BIT,
	O_SIZE_32BIT,
	O_SIZE_32BIT,
	O_SIZE_32BIT,
	O_SIZE_32BIT,
	O_SIZE_32BIT,
	O_SIZE_32BIT,
	O_SIZE_16BIT,
	O_SIZE_16BIT,
	O_SIZE_16BIT,
	O_SIZE_16BIT,
	O_SIZE_16BIT,
	O_SIZE_16BIT,
	O_SIZE_16BIT,
	O_SIZE_16BIT,
	O_SIZE_8BIT,
	O_SIZE_8BIT,
	O_SIZE_8BIT,
	O_SIZE_8BIT,
	O_SIZE_8BIT,
	O_SIZE_8BIT,
	O_SIZE_8BIT,
	O_SIZE_8BIT,
	O_SIZE_8BIT,
	O_SIZE_8BIT,
	O_SIZE_8BIT,
	O_SIZE_8BIT,
	O_SIZE_8BIT,
	O_SIZE_8BIT,
	O_SIZE_8BIT,
	O_SIZE_8BIT
};
unsigned int* reg_conf[3]=
{
	reg_conf_dest,
	reg_conf_src,
	reg_conf_sz
};
const unsigned reg_count=sizeof(reg_text)/sizeof(reg_text[0]);

bool sameIOFn=false;
int option_index;
char line[4096];
char choice;
unsigned int _err=0;
instruction find_inst(char* str);
unsigned con_reg(char *str, char *t, char *s, instruction* ins, unsigned int ds);
unsigned long long find_num(char *str);
int main(int argc, char *argv[])
{
	int opt;
	opt=getopt_long(argc, argv, "a:f:i:o:h",option_table, &option_index);
	do
	{
		switch(opt)
		{
		case 'a':
		{
			if(!strcmp(optarg, "th32"))
			{
				arch=TH_32;
			}
			else if(!strcmp(optarg, "th64"))
			{
				arch=TH_64;
			}
			else
			{
				fprintf(stderr, "Unimplemented architecture \"%s\".", optarg);
				exit(EXIT_FAILURE);
			}
			break;
		}
		case 'f':
		{
			if(!strcmp(optarg, "bin"))
			{
				form=PURE_BINARY;
			}
			else
			{
				fprintf(stderr, "Unimplemented file format \"%s\".", optarg);
				exit(EXIT_FAILURE);
			}
			break;
		}
		case 'i':
		{
			strcpy((ifile=(reinterpret_cast<char*>(malloc(strlen(optarg))))), optarg);
			if(!ofile)
			{
				ofile=strcpy(reinterpret_cast<char*>(calloc(1,strlen(optarg)+4)), optarg);
				sameIOFn=true;
			}
			break;
		}
		case 'o':
		{
			if(ofile)free(ofile);
			ofile = strcpy(reinterpret_cast<char*>(malloc(strlen(optarg))), optarg);
			break;
		}
		case 'y':
		case 'n':
			choice=opt;
			break;
		case 'h':
			usage_prompt_io=stdout;
			printf("Thontelix Assembler v. %s\n", vstring);
		default:
			fprintf(usage_prompt_io, usage_str, argv[0]);
			exit((usage_prompt_io==stdout)?EXIT_SUCCESS:EXIT_FAILURE);
		}
	}while((opt=getopt_long(argc, argv, "a:f:i:o:h",option_table, &option_index)) != -1);
	if(!ifile)
	{
		fputs("Must specify input file",stderr);
		exit(EXIT_FAILURE);
	}
	if(!form)
	{
		fputs("Must specify file format",stderr);
		exit(EXIT_FAILURE);
	}
	if(sameIOFn){
		int i=0;
		for(;ofile[i];i++)
		{
			if(ofile[i]=='.')
			{
				strcpy(ofile+i+1, file_format_ext[form]);
				break;
			}
		}
		if(!ofile[i])
		{
			ofile[i]='.';
			strcpy(ofile+i+1, file_format_ext[form]);
		}
	}
	if(arch==TH_64)
	{
		fputs("TH_64 unimplemented.\n", stderr);
		exit(EXIT_FAILURE);
	}
	ifl=fopen(ifile, "rb");
	if(!ifl)
	{
		fprintf(stderr, "Unable to open input file : %s\n", ifile);
		exit(EXIT_SUCCESS);
	}
	unsigned int li=0;
	do
	{
		char *tk[6];
		fgets(line, sizeof(line), ifl);
		static instruction cur;
		if(line[0]=='$')
		{
			char *__t=strchr(line ,'\n');
			if(__t)__t[0]=0;
			unsigned i_ai=0;
			for(;i_ai<asm_inst_c;i_ai++)
			{
				if(!strcmp(asm_inst_text[i_ai], line+1))break;
			}
			if(i_ai==asm_inst_c)
			{
				fprintf(stderr, "[Error] : Undefined assembler instruction \"%s\" at line %u, stopping compilation\n", line+1, li);
				fclose(ifl);
				exit(EXIT_FAILURE);
			}
			asm_inst[i_ai]();
		}
		else
		{
			char* _ret_strchr=strchr(line, ';');
			if(!_ret_strchr)
			{
				fprintf(stderr, "[Error] : Expected a ';' at line %u\n" , li);
				compileErrors++;
				goto end_not_compile;
			}
			_ret_strchr[0]=0;
			if((tk[0]=strtok(line, " \t")))
			{
				cur=find_inst(tk[0]);
				if(cur.oper_c>1)
				{
					unsigned int i=0;
					while((tk[++i]=strtok(0, " \t")))
					{
						if(i==cur.oper_c)
							if(strtok(0, " \t"))
							{
								fprintf(stderr, "[Error] : Too many operands at line %u\n", li);
								compileErrors++;
								goto end_not_compile;
							}
					}
					if(i<cur.oper_c)
					{
						fprintf(stderr, "[Error] : Not enough operands for instruction \"%s\" at line %u\n", tk[0], li);
						compileErrors++;
						goto end_not_compile;
					}
					if(strlen(tk[2])>1U)
					{
						fprintf(stderr, "[Error] : Direction operand must consist of one character (line %u)\n", li);
						compileErrors++;
						goto end_not_compile;
					}
					char *st_dest,*st_src;
					switch(tk[2][0])
					{
					case '>':
						st_dest=tk[3];
						st_src=tk[1];
						break;
					case '<':
						st_dest=tk[1];
						st_src=tk[3];
						break;
					default:
						fprintf(stderr, "[Error] : Invalid direction operand at line %u\n", li);
						compileErrors++;
						goto end_not_compile;
					}
					char *td = strchr(st_dest, '~');
					if(!td)
					{
						fprintf(stderr, "[Error] : Expected tilde in destination operand at line %u\n", li);
						compileErrors++;
						goto end_not_compile;
					}
					td[0]=0;
					td++;
					char *sd = strchr(td, '-');
					if(sd)
					{
						sd[0]=0;
						sd++;
					}
					unsigned ret_con_reg;
					switch((ret_con_reg=con_reg(st_dest,td,sd,&cur,DestinationOp)))
					{
					case 0:break;
					case 1:
						//Invalid destination operand type descriptor
						fprintf(stderr, "[Error] : Invalid destination operand type descriptor \"%s\" at line %u\n", td, li);
						compileErrors++;
						goto end_not_compile;
					default:
						fprintf(stderr, "[Error] : Invalid destination operand at line %u (ERR : %u)\n", li, ret_con_reg);
						compileErrors++;
						goto end_not_compile;
					}
					td = strchr(st_src, '~');
					if(!td)
					{
						fprintf(stderr, "[Error] : Expected tilde in source operand at line %u\n", li);
						compileErrors++;
						goto end_not_compile;
					}
					td[0]=0;
					td++;
					sd = strchr(td, '-');
					if(sd)
					{
						sd[0]=0;
						sd++;
					}
					switch((ret_con_reg=con_reg(st_src,td,sd,&cur,SourceOp)))
					{
					case 0:break;
					case 1:
						//Invalid source operand type descriptor
						fprintf(stderr, "[Error] : Invalid source operand type descriptor \"%s\" at line %u\n", td, li);
						compileErrors++;
						goto end_not_compile;
					default:
						fprintf(stderr, "[Error] : Invalid source operand at line %u (ERR : %u)\n", li,ret_con_reg);
						compileErrors++;
						goto end_not_compile;
					}
					if(!(cur.opcode_op&O_PRESENT))
					{
						fprintf(stderr, "[Error] : Operand size not specified at line %u\n", li);
						compileErrors++;
						goto end_not_compile;
					}
				}
				obuf.push_back(cur);
			}
		}
		end_not_compile:
		li++;
		if(feof(ifl))
		{
			fprintf(stderr, "[Error] : No end of file specified. Append a line storing \"$end\".\n");
			fclose(ifl);
			exit(EXIT_FAILURE);
		}
	}while(!__eof__);
	fclose(ifl);
	if(compileErrors)
	{
		fprintf(stderr, "%u errors during compilation.\n", compileErrors);
		exit(EXIT_FAILURE);
	}
	switch(form)
	{
	case PURE_BINARY:
		ofl=fopen(ofile, "wb");
		for(std::vector<instruction>::iterator it=obuf.begin(); it!=obuf.end();it++)
		{
			static instruction incr;
			incr=*it;
			fwrite(&(incr.opcode), sizeof(unsigned char), 1, ofl);
			fwrite(&(incr.opcode_ex), sizeof(unsigned char), 1, ofl);
			if(incr.opcode_op)
			{
				fwrite(&(incr.opcode_op), sizeof(unsigned short), 1, ofl);
				if((incr.opcode_op&O_D_ISREG)||((incr.opcode_op&O_S_ISREG)&&(!(incr.opcode_op&O_S_NULL_REG))))fwrite(&(incr.opcode_t), sizeof(unsigned char), 1, ofl);
				for(int i=0;i<2;i++)
				{
					if(i==0&&!(incr.opcode_op&O_D_ADD_MNEM))continue;
					if(i==1&&!(incr.opcode_op&O_S_ADD_MNEM))break;
					switch(incr.opcode_op&O_SIZE)
					{
						case O_SIZE_64BIT:
						{
							fwrite(incr.mnemonic+i, sizeof(unsigned long long), 1, ofl);
							break;
						}
						case O_SIZE_32BIT:
						{
							fwrite(incr.mnemonic+i, sizeof(unsigned int), 1, ofl);
							break;
						}
						case O_SIZE_16BIT:
						{
							fwrite(incr.mnemonic+i, sizeof(unsigned short), 1, ofl);
							break;
						}
						case O_SIZE_8BIT:
						{
							fwrite(incr.mnemonic+i, sizeof(unsigned char), 1, ofl);
							break;
						}
					}
				}
			}
		}
		fclose(ofl);
		break;
	}
	puts(ofile);
	exit(EXIT_SUCCESS);
}
#include<cctype>
instruction find_inst(char *str)
{
	unsigned int i=0,j=0;
	unsigned int k=strlen(str);
	for(;i<instruc_c;i++)
	{
		if(strlen(inst[i].text)!=k)continue;
		for(;j<k;j++)if(str[j]!=inst[i].text[j])break;
		if(j==k)
		{
			break;
		}
		j=0;
	}
	return inst[i];
}

unsigned con_reg(char *str, char *t, char *s, instruction* ins, unsigned int ds)
{
	unsigned tt;
	if(!strcmp(t, "val"))tt=1;
	else if(!strcmp(t, "ptr"))tt=2;
	else
	{
		return 1;
	}
	unsigned i=0;
	for(;i<reg_count;i++)
	{
		if(!strcmp(str, reg_text[i]))break;
	}
	if(i<reg_count)
	{
		bool __chk_tt=(tt==1);
		if(ds==DestinationOp)
		{
			ins->opcode_op|=O_D_ISREG;
			if(__chk_tt)ins->opcode_op|=O_D_NOT_REG_POINTER;
		}
		else
		{
			ins->opcode_op|=O_S_ISREG;
			if(__chk_tt)ins->opcode_op|=O_S_NOT_REG_POINTER;
		}
		*(reinterpret_cast<unsigned int*>(&(ins->opcode_op)))|=reg_conf[ds][i];
		if(ins->opcode_op&O_PRESENT)
		{
			if((ins->opcode_op&O_SIZE)!=reg_conf_sz[i]) return 6;
		}
		else ins->opcode_op|=reg_conf_sz[i];
		ins->opcode_op|=O_PRESENT;
		return 0;
	}
	else
	{
		//Null register
		unsigned char szor;
		if(s)
		{
			if(ins->opcode_op&O_PRESENT)
			{
				return 4;
			}
			if(!strcmp(s, "byte"))
				szor=O_SIZE_8BIT;
			else if(!strcmp(s, "word"))
				szor=O_SIZE_16BIT;
			else if(!strcmp(s, "dword"))
				szor=O_SIZE_32BIT;
			else if(!strcmp(s, "qword"))
				szor=O_SIZE_64BIT;
			else
			{
				return 5;
			}
			ins->opcode_op|=szor;
			ins->opcode_op|=O_PRESENT;
		}
		if(ds==DestinationOp)
		{
			if(tt==1)
			{
				return 2;
			}
			ins->opcode_op|=O_D_NOT_REG_POINTER;
			ins->opcode_op|=O_D_ADD_MNEM;
			ins->mnemonic[DestinationOp]=find_num(str);
		}
		else
		{
			if(tt==1)
			{
				ins->opcode_op|=O_S_ISREG;
				ins->opcode_op|=O_S_NULL_REG;
			}
			ins->opcode_op|=O_S_NOT_REG_POINTER;
			ins->opcode_op|=O_S_ADD_MNEM;
			ins->mnemonic[SourceOp]=find_num(str);
		}
		return _err;
	}
}
unsigned long long __atoll(char *str, int base)
{
	int strl=strlen(str);
	int alph=base-10;
	unsigned long long multi=1,ret=0;
	for(int i=strl-1;i>=0;i--)
	{
		char tc;
		if(isdigit(str[i]))
		{
			tc=str[i]-'0';
		}
		else
		{
			if(!((str[i]>='A')&&(str[i]<('A'+alph))))
			{
				_err=512;
				return 0ULL;
			}
			tc=str[i]-'A'+10;
		}
		ret+=multi*tc;
		multi*=base;
	}
	return ret;
}
unsigned long long find_num(char *str)
{
	//TODO
	if(isdigit(str[0]))
	{
		if(str[0]=='0')
		{
			if((str[1]=='x')||(str[1]=='X'))
			{
				//Hexadecimal
				return __atoll(str+2, 16);
			}
			else if((str[1]=='b')||(str[1]=='B'))
			{
				//Binary
				return __atoll(str+2, 2);
			}
			else if(isdigit(str[1]))
			{
				//Octal
				return __atoll(str+1, 8);
			}
			else {
				_err=str[1];
				return 0ULL;
			}
		}
		else
		{
			//Decimal
			return __atoll(str,10);
		}
	}
	else
	{
		_err=768;
		return 0ULL;
	}
}

