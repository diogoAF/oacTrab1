#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define MEM_SIZE 8192

int memoria[MEM_SIZE];

enum OPCODES {
	EXT=0x00,
	LH=0x21, 	 SB=0x28, 	 BLEZ=0x06,
	SLTIU=0x0B,	 J=0x02,  	 LW=0x23,
	LHU=0x25,	 SH=0x29,	 BGTZ=0x07,
	ANDI=0x0C,	 JAL=0x03,	  LB=0x20,
	LUI=0x0F,	 BEQ=0x04,	 ADDI=0x08,
	ORI=0x0D,	  LBU=0x24,	 SW=0x2B,
	BNE=0x05,	 SLTI=0x0A,	 XORI=0x0E
};

enum FUNC {
	ADD=0x20, SUB=0x22, MUL=0x18, DIV=0x1A, AND=0x24,
	OR=0x25, XOR=0x26, NOR=0x27, SLT=0x2A, JR=0x08,
	SLL=0x00, SRL=0x02, SRA=0x03, SYSCALL=0x0c, MFHI=0x10,
	MFLO=0x12
};

void doADD();//OK
void doOR();//OK
void doSLL();//OK
void doSUB();//OK
void doXOR();//OK
void doSRL();//OK
void doMUL();//OK
void doNOR();//OK
void doSRA();//OK
void doDIV();//OK
void doSLT();//OK
void doSYSCALL();//OK
void doMFHI();//OK
void doMFLO();//OK
void doAND();//OK
void doJR();//OK
void doLH();
void doSLTIU();//OK
void doLHU();
void doANDI();//OK
void doLUI();//OK
void doORI();//OK
void doBNE();//OK
void doSB();
void doJ();
void doSH();
void doJAL();
void doBEQ();//OK
void doLBU();
void doSLTI();//OK
void doBLEZ();
void doLW();//OK
void doBGTZ();
void doLB();
void doADDI();//OK
void doSW();
void doXORI();//OK

/*Variaveis que nao armazenam dados, apenas instrucoes*/
unsigned int pc, ri;
unsigned int opcode, rd, rt, rs, shamt, funct;
unsigned int imediato, endereco;

/*Registradores*/
int bancoDeRegistradores[32];
int hi, lo;

//Só recebe um quando recebe um syscall de fim de programa
char verificaSaida;

int read_int(unsigned int end); // lê um inteiro alinhado - endereços múltiplos de 4
int read_byte(unsigned int end); // lê um byte - retorna inteiro com sinal
int read_ubyte(unsigned int end); // lê um byte - retorna inteiro sem sinal
void write_int(unsigned int end, int dado); // escreve um inteiro alinhado na memória
void write_byte(unsigned int end, char byte); // escreve um byte na sua posição na memória

void dump_mem(int start, int end, char format);//imprime o conteudo da memoria a partir do endereco start até o end
void dump_reg(char format);//imprime o conteudo dos registradores do MIPS, incluindo o banco de registradores e os registradores pc, hi e lo

//Funcoes principais em ordem de chamada
void run();
void step();
int	 fetch();
void decode();
void execute();

//Funcoes auxiliares das principais
int set_offset();
void openData();
void openText();

/** \brief Funcao principal
 *
 * \return int
 *
 */
int main(){
	pc = 0;
	verificaSaida = 0;
    openData();
    openText();
	run();
	return 0;
}

/** \brief Abre o arquivo que contem o binario do .data
 *
 * \return void
 *
 */
void openData(){
    FILE *data, *text;
	int i, aux=0x2000, dadoTemporario;
	char byte;

	data = fopen("data.bin", "rb+");
	while(!feof(data) && (aux >> 2) < MEM_SIZE){
		dadoTemporario = 0;
		for(i=0; i<4; i++){
			fread(&byte, sizeof(char), 1, data);
			dadoTemporario += (byte & 0xFF) << (8*i);
		}
		memoria[aux>>2] = dadoTemporario;
		aux += 4;
	}
	fclose(data);
}

/** \brief Abre o arquivo que contem o binario do .text
 *
 * \return void
 *
 */
void openText(){
    FILE *data, *text;
	int i, aux=0, dadoTemporario;
	char byte;

	text = fopen("text.bin", "rb+");
	while(!feof(text) && aux < 0x2000){
		dadoTemporario = 0;
		for(i=0; i<4; i++){
			fread(&byte, sizeof(char), 1, text);
			dadoTemporario += (byte & 0xFF) << (8*i);
		}
		memoria[aux>>2] = dadoTemporario;
		aux += 4;
	}
	fclose(text);
}

/*Funcao criada com a finalidade de se chamar a funcao step() dentro de
*um loop limitado a condicao do tamanho e se o programa deve ou não continuar
*rodando determinado por uma variavel de parada*/
void run(){
	//Garantir que pc nao invada a memoria de dados e que o programa ainda nao foi encerrado
	while(pc<0x2000 && !verificaSaida){
		step();
	}
}

//Funcao responsavel por chamar as funcoes fetch, decode e execute
void step(){
	fetch();
	decode();
	execute();
}

//Funcao responsavel pela leitura das instruções da memoria e coloca em ri, atualizando o pc para a proxima instrucao
int	 fetch(){
	ri = memoria[pc>>2];
	pc = pc + 4;
	return 0;
}

//Funcao responsavel por extrair todos os campos da instrucao lidas pela funcap fetch
void decode(){
	opcode = (ri>>26) & 0x3F;
	rs = (ri>>21) & 0x1F;
	rt = (ri>>16) & 0x1F;
	rd = (ri>>11) & 0x1F;
	shamt = (ri>>6) & 0x1F;
	funct = ri & 0x3F;
	imediato = ri & 0xFFFF;
	endereco = ri & 0x3FFFFFF;
}

//Funcao responsavel por executar os comandos decodificado pela funcao decode
void execute (){
		//Switch criado para cada um dos casos existentes no enum do OPCODE
		switch(opcode){

			case EXT:
			    switch(funct){
                case ADD:
                    doADD();
                    break;
                case OR:
                    doOR();
                    break;
                case SLL:
                    doSLL();
                    break;
                case SUB:
                    doSUB();
                    break;
                case XOR:
                    doXOR();
                    break;
                case SRL:
                    doSRL();
                    break;
                case MUL:
                    doMUL();
                    break;
                case NOR:
                    doNOR();
                    break;
                case SRA:
                    doSRA();
                    break;
                case DIV:
                    doDIV();
                    break;
                case SLT:
                    doSLT();
                    break;
                case SYSCALL:
                    doSYSCALL();
                    break;
                case AND:
                    doAND();
                    break;
                case JR:
                    doJR();
                    break;
				case MFHI:
					doMFHI();
					break;
				case MFLO:
					doMFLO();
					break;
			    }
				break;

			case LH:
			    doLH();
				break;

			case SLTIU:
			    doSLTIU();
				break;

			case LHU:
			    doLHU();
				break;

			case ANDI:
			    doANDI();
				break;

			case LUI:
			    doLUI();
				break;

			case ORI:
			    doORI();
				break;

			case BNE:
			    doBNE();
				break;

			case SB:
			    doSB();
				break;

			case J:
			    doJ();
				break;

			case SH:
			    doSH();
				break;

			case JAL:
			    doJAL();
				break;

			case BEQ:
			    doBEQ();
				break;

			case LBU:
			    doLBU();
				break;

			case SLTI:
			    doSLTI();
				break;

			case BLEZ:
			    doBLEZ();
				break;

			case LW:
			    doLW();
				break;

			case BGTZ:
			    doBGTZ();
				break;

			case LB:
			    doLB();
				break;

			case ADDI:
			    doADDI();
				break;

			case SW:
			    doSW();
				break;

			case XORI:
			    doXORI();
				break;
		}
}

/** \brief Realiza a operacao MFHI
 *
 * \return void
 *
 */
void doMFHI(){
	bancoDeRegistradores[rd] = hi;
}

/** \brief Realiza a operacao MFLO
 *
 * \return void
 *
 */
void doMFLO(){
	bancoDeRegistradores[rd] = lo;
}

/** \brief Realiza a operacao LW
 *
 * \return void
 *
 */
void doLW(){
    if((imediato & 0x8000) != 0){
        imediato = imediato | 0xFFFF0000; // Extensao de sinal do imediato
    }
    bancoDeRegistradores[rt] = read_int(bancoDeRegistradores[rs]+imediato);
}

/** \brief Realiza a operacao ADDi
 *
 * \return void
 *
 */
void doADDI(){
    bancoDeRegistradores[rt] = bancoDeRegistradores[rs] + imediato;
}

/** \brief Realiza a operacao ADD
 *
 * \return void
 *
 */
void doADD(){
	bancoDeRegistradores[rd] = bancoDeRegistradores[rs] + bancoDeRegistradores[rt];
}

/** \brief Realiza a operacao OR
 *
 * \return void
 *
 */
void doOR(){
	bancoDeRegistradores[rd] = bancoDeRegistradores[rs] | bancoDeRegistradores[rt];
}

/** \brief Realiza a operacao SLL
 *
 * \return void
 *
 */
void doSLL(){
	bancoDeRegistradores[rd] = bancoDeRegistradores[rt] << bancoDeRegistradores[shamt];
}

/** \brief Realiza a operacao SUB
 *
 * \return void
 *
 */
void doSUB(){
	bancoDeRegistradores[rd] = bancoDeRegistradores[rs] - bancoDeRegistradores[rt];
}

/** \brief Realiza a operacao XOR
 *
 * \return void
 *
 */
void doXOR(){
	bancoDeRegistradores[rd] = bancoDeRegistradores[rs] ^ bancoDeRegistradores[rt];
}

/** \brief Realiza a operacao SRL
 *
 * \return void
 *
 */
void doSRL(){
    bancoDeRegistradores[rd] = bancoDeRegistradores[rt] >> shamt;
}

/** \brief Realiza a operacao MUL
 *
 * \return void
 *
 */
void doMUL(){
	long long int result;
	result = (long long int) bancoDeRegistradores[rs] * (long long int) bancoDeRegistradores[rt];
	hi = (result >> 32) & 0xFFFFFFFF;
	lo = result & 0xFFFFFFFF;
}

/** \brief Realiza a operacao NOR
 *
 * \return void
 *
 */
void doNOR(){
	bancoDeRegistradores[rd] = ~(bancoDeRegistradores[rs] | bancoDeRegistradores[rt]);
}

/** \brief Realiza a operacao SRA
 *
 * \return void
 *
 */
void doSRA(){
    unsigned int n,somatorio=0;
    for(n=1;n<=shamt;n++){
        somatorio += pow(2,32-n);
    }
    bancoDeRegistradores[rd] =  bancoDeRegistradores[rt]>>shamt + somatorio*(bancoDeRegistradores[rt]>>31);
}

/** \brief Realiza a operacao DIV
 *
 * \return void
 *
 */
void doDIV(){
    lo = bancoDeRegistradores[rs]/bancoDeRegistradores[rt];
    hi = bancoDeRegistradores[rs]%bancoDeRegistradores[rt];
}

/** \brief Realiza a operacao SLT
 *
 * \return void
 *
 */
void doSLT(){
    if(bancoDeRegistradores[rs] < bancoDeRegistradores[rt]){
		bancoDeRegistradores[rd] = 1;
	}else{
		bancoDeRegistradores[rd] = 0;
	}
}

/** \brief Realiza aa operacoes do Syscall, imprimir int, imprimir string e encerrar o programa
 *
 * \return void
 *
 */
void doSYSCALL(){
    int inicioString, byte;

	switch (bancoDeRegistradores[2]){
		case 1:
			printf("%d\n", bancoDeRegistradores[4]);
			break;
		case 4:
			inicioString = bancoDeRegistradores[4];
			do{
				byte = ((char)read_ubyte(inicioString)) & 0xFF;
				if(byte != '\0')
					printf("%c", byte);
				inicioString++;
			}while(byte != '\0');
			printf("\n");
			break;
		case 10:
			verificaSaida = 1;
			break;
	}
}

/** \brief Realiza a operacao AND
 *
 * \return void
 *
 */
void doAND(){
	bancoDeRegistradores[rd] = bancoDeRegistradores[rs] & bancoDeRegistradores[rt];
}

/** \brief Realiza a operacao JR
 *
 * \return void
 *
 */
void doJR(){
    pc = bancoDeRegistradores[rs];
}

/** \brief Realiza a operacao LH
 *
 * \return void
 *
 */
void doLH(){
    //Does Something Wonderfull
}

/** \brief Realiza a operacao SLTIU
 *
 * \return void
 *
 */
void doSLTIU(){
    if(bancoDeRegistradores[rs] < imediato){
		bancoDeRegistradores[rt] = 1;
	}else{
		bancoDeRegistradores[rt] = 0;
	}
}

/** \brief Realiza a operacao LHU
 *
 * \return void
 *
 */
void doLHU(){
    //Does Something Wonderfull
}

/** \brief Realiza a operacao ANDi
 *
 * \return void
 *
 */
void doANDI(){
	bancoDeRegistradores[rt] = bancoDeRegistradores[rs] & imediato;
}

/** \brief Realiza a operacao LUI
 *
 * \return void
 *
 */
void doLUI(){
	bancoDeRegistradores[rt] = (imediato << 16);
}

/** \brief Realiza a operacao ORI
 *
 * \return void
 *
 */
void doORI(){
	bancoDeRegistradores[rt] = bancoDeRegistradores[rs] | imediato;
}

/** \brief Realiza a operacao BNE
 *
 * \return void
 *
 */
void doBNE(){
    if(bancoDeRegistradores[rs] != bancoDeRegistradores[rt]){
        pc = shamt<<2;
    }
}

/** \brief Realiza a operacao SB
 *
 * \return void
 *
 */
void doSB(){
    //Does Something Wonderfull
}

/** \brief Realiza a operacao Jumper
 *
 * \return void
 *
 */
void doJ(){
    //Does Something Wonderfull
}

/** \brief Realiza a operacao SH
 *
 * \return void
 *
 */
void doSH(){
    //Does Something Wonderfull
}

/** \brief Realiza a operacao JAL
 *
 * \return void
 *
 */
void doJAL(){
    //Does Something Wonderfull
}

/** \brief Realiza a operacao BEQ
 *
 * \return void
 *
 */
void doBEQ(){
    if(bancoDeRegistradores[rs] == bancoDeRegistradores[rt]){
        pc = shamt<<2;
    }
}

/** \brief Realiza a operacao LBU
 *
 * \return void
 *
 */
void doLBU(){
    //Does Something Wonderfull
}

/** \brief Realiza a operacao SLTI
 *
 * \return void
 *
 */
void doSLTI(){
    if(bancoDeRegistradores[rs] < (int) imediato){
		bancoDeRegistradores[rt] = 1;
	}else{
		bancoDeRegistradores[rt] = 0;
	}
}

/** \brief Realiza a operacao BLEZ
 *
 * \return void
 *
 */
void doBLEZ(){
    //Does Something Wonderfull
}

/** \brief Realiza a operacao BGTZ
 *
 * \return void
 *
 */
void doBGTZ(){
    //Does Something Wonderfull
}

/** \brief Realiza a operacao LB
 *
 * \return void
 *
 */
void doLB(){
    //Does Something Wonderfull
}

/** \brief Realiza a operacao SW
 *
 * \return void
 *
 */
void doSW(){
    //Does Something Wonderfull
}

/** \brief Realiza a operacao XORi
 *
 * \return void
 *
 */
void doXORI(){
	bancoDeRegistradores[rt] = bancoDeRegistradores[rs] ^ imediato;
}

/** \brief Le um inteiro da memoria
 *
 * \param end unsigned int - endereco do inteiro na memoria
 * \return int - o dado armazenado na memoria ou codigo de erro
 *
 */
int read_int(unsigned int end){
	if(end%4 == 0){
		return (memoria[end>>2]);
	}
	else{
		verificaSaida = 1;
		return -1;
	}
}

/** \brief Escreve um inteiro na memoria
 *
 * \param end unsigned int - enderenco onde o dado deve ser armazenado
 * \param dado int - dado que sera armazenado
 * \return void
 *
 */
void write_int(unsigned int end, int dado){
    if(end%4 == 0){
		memoria[end>>2] = dado;
	}
	else{
		verificaSaida = 1;
	}
}

/** \brief le um byte inteiro com sinal
 *
 * \param end unsigned int - endereco onde da memoria onde o byte esta armazenado
 * \return int - byte recuperado da memoria
 *
 */
int read_ubyte(unsigned int end){
	int aux = memoria[end >> 2];
	int ubyte;

	ubyte = (aux >> (8*(end%4)) & 0xFF);
	return ubyte;
}


/** \brief le um byte inteiro SEM sinal
 *
 * \param end unsigned int - endereco onde da memoria onde o byte esta armazenado
 * \return int - byte recuperado da memoria
 *
 */
int read_byte(unsigned int end){

}
