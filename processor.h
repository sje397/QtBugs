#ifndef _PROCESSOR_INC
#define _PROCESSOR_INC

#include <QByteArray>

#define INPUTS		(9 + 9 + 9 + 2)		// 9 colours, 9 altitudes, 9 energies, a short for amount of current bug energy (pain?)
#define OUTPUTS         (1 + 1 + 1 + 1)		// 1 byte for direction, 1 for colour, 1 for 'split', 1 for 'action'

// uncommend to allow the RAND instruction
#define ENABLE_RAND

#include <QDataStream>
#include <QDebug>

class Processor {
	friend class ProcessorPage;
	friend QDataStream& operator<<(QDataStream& stream, const Processor &proc);
	friend QDataStream& operator>>(QDataStream& stream, Processor &proc);
public:
	Processor();
	Processor(int steps, int stack_size);
	virtual ~Processor() {}
	void set_data(const QByteArray &rom);
	void set_steps_per_update(int steps) {steps_per_update = steps;}

	void update();

	int size() {return ram.size();}
	QByteArray &get_ram() {return ram;}

	void set_in(int index, unsigned char val) {in[index % INPUTS] = val;}
	void set_in_multi(int start_index, int num, unsigned char *vals) {
		memcpy(in + start_index, vals, num);
	}
	unsigned char get_out(int index) {return out[index % OUTPUTS];}

	QString disassemble_from(int address);

	typedef enum {I_NOOP, I_PUSH, I_POP, I_PUSHA, I_POPA, I_SUBROUTINE, I_RET, I_IN, I_OUT, I_JMP, I_JMPZ, I_JMPNZ, I_LOAD, I_STORE, I_INC, I_DEC, I_NOT, I_AND, I_OR, I_NAND, I_NOR, I_XOR, I_ADD, I_SUB, I_MUL, I_DIV, I_RAND, MAX_I} Instruction;

protected:
	void step();

	QByteArray ram;

	inline int inc_ip() {
		int val = ip; ip = (ip + 1) % ram.size();
		//qDebug() << "ram size:" << ram.size() << "ip:" << ip;
		return val;
	}

	inline int make_addy(char low, char hi) {
		return (((unsigned char)hi << 8) + (unsigned char)low) % ram.size();
	}

	inline int addy() {
		return  make_addy(ram[inc_ip()], ram[inc_ip()]);
	}

	int ip, sp, bp;
	unsigned char acc;

	unsigned char in[INPUTS];
	unsigned char out[OUTPUTS];

	QByteArray stack;

	int steps_per_update;
};

QDataStream& operator<<(QDataStream& stream, const Processor &proc);
QDataStream& operator>>(QDataStream& stream, Processor &proc);

#endif
