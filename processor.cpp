#include "processor.h"
#include <QDebug>

QDataStream& operator<<(QDataStream& stream, const Processor &proc) {
	stream << proc.ip << proc.sp << proc.bp << proc.acc << proc.steps_per_update << proc.ram << proc.stack << QByteArray((char *)proc.out, OUTPUTS);
	return stream;
}

QDataStream& operator>>(QDataStream& stream, Processor &proc) {
	QByteArray out;
	stream >> proc.ip >> proc.sp >> proc.bp >> proc.acc >> proc.steps_per_update >> proc.ram >> proc.stack >> out;
	for(int i = 0; i < OUTPUTS; i++)
		proc.out[i] = out[i];
	return stream;
}

Processor::Processor(): ip(0), sp(0), bp(0), acc(0), steps_per_update(1) {
	stack.resize(16);
}

Processor::Processor(int steps, int stack_size): ip(0), sp(0), bp(0), acc(0), steps_per_update(steps) {
	for(int i = 0; i < INPUTS; i++)
		in[i] = 0;
	
	for(int i = 0; i < OUTPUTS; i++)
		out[i] = 0;

	stack.resize(stack_size);
}

void Processor::set_data(const QByteArray &rom) {
	ram = rom;
}

void Processor::update() {
	//	for(int i = 0; i < OUTPUTS; i++)
	//		out[i] = 0;
	//	out[2] = 0;
	for(int i = 0; i < steps_per_update; i++) 
		step();
}

void Processor::step() {
	//qDebug() << "ram size:" << ram.size();
	Instruction instruction = (Instruction)((int)(unsigned char)ram[inc_ip()] % MAX_I);
	//qDebug() << "inst:" << instruction;
	int addr;
	switch(instruction) {
	case I_PUSH:
		//qDebug() << "push";
		if(sp < stack.size()) stack[sp++] = (unsigned char)acc;
		break;
	case I_POP:
		//qDebug() << "pop";
		if(sp > bp) acc = (unsigned char)stack[--sp];
		break;

	case I_PUSHA:
		//qDebug() << "pusha";
		addr = addy();
		if(sp < stack.size()) stack[sp++] = ram[addr];
		break;
	case I_POPA:
		//qDebug() << "popa";
		addr = addy();
		if(sp > bp) ram[addr] = stack[--sp];
		break;
		
	case I_SUBROUTINE:
		//qDebug() << "subroutine";
		addr = addy();
		if(sp <= stack.size() - 4) {
			stack[sp++] = ((ip >> 8) & 0xff);
			stack[sp++] = (ip & 0xff);
			stack[sp++] = ((bp >> 8) & 0xff);
			stack[sp++] = (bp & 0xff);
			bp = sp;
			ip = addr;
		}
		break;
	case I_RET:
		//qDebug() << "ret";
		if(bp >= 4) {
			sp = bp;
			bp = make_addy(stack[sp - 1], stack[sp - 2]);
			ip = make_addy(stack[sp - 3], stack[sp - 4]);
			sp -= 4;
		}
		break;

	case I_IN: acc = in[(unsigned char)ram[inc_ip()] % INPUTS]; break;
	case I_OUT: out[(unsigned char)ram[inc_ip()] % OUTPUTS] = acc; break;

	case I_JMP: ip = addy(); break;
	case I_JMPZ:
		addr = addy();
		if(acc == 0) ip = addr;
		break;
	case I_JMPNZ:
		addr = addy();
		if(acc != 0) ip = addr;
		break;

	case I_LOAD:
		acc = (unsigned char)ram[addy()];
		break;
	case I_STORE:
		ram[addy()] = (unsigned char)acc;
		break;

	case I_INC: acc++; break;
	case I_DEC: acc--; break;

	case I_NOT: acc = ~acc; break;
	case I_AND: acc = (acc & (unsigned char)ram[inc_ip()]); break;
	case I_OR: acc = (acc | (unsigned char)ram[inc_ip()]); break;
	case I_NAND: acc = ~(acc & (unsigned char)ram[inc_ip()]); break;
	case I_NOR: acc = ~(acc | (unsigned char)ram[inc_ip()]); break;
	case I_XOR: acc = (acc ^ (unsigned char)ram[inc_ip()]); break;

	case I_ADD: acc += (unsigned char)ram[inc_ip()]; break;
	case I_SUB: acc -= (unsigned char)ram[inc_ip()]; break;
	case I_MUL: acc *= (unsigned char)ram[inc_ip()]; break;
	case I_DIV:
		{
			unsigned char val = (unsigned char)ram[inc_ip()];
			if(val != 0) acc /= val;
		}
		break;

	case I_RAND:
#ifdef ENABLE_RAND
		acc = qrand() % 0xff;
#endif
		break;

	default: // I_NOOP;
		break;
	}
}

#define MAX_DISASSEMBLE_LINES 2000
const char *pneumonics[Processor::MAX_I] = {
	"NOOP", "PUSH", "POP", "PUSHA", "POPA", "CALL", "RET", "IN", "OUT", "JMP", "JMPZ", "JMPNZ", 
	"LOAD", "STORE", "INC", "DEC", "NOT", "AND", "OR", "NAND", "NOR", "XOR", 
	"ADD", "SUB", "MUL", "DIV", "RAND"
};

int inst_bytes[Processor::MAX_I] = {
	1, 1, 1, 3, 3, 3, 1, 2, 2, 3, 3, 3,
	3, 3, 1, 1, 1, 2, 2, 2, 2, 2,
	2, 2, 2, 2, 1
};

QString Processor::disassemble_from(int address) {
	if(address >= ram.size()) return "Out of Range";

	QString str = "<html><body><table>";

	int ip_save = ip;
	ip = address;
	int lines = 0;
	int v;
	bool wrap = false;
	while(ip >= address && lines < MAX_DISASSEMBLE_LINES && !wrap) {
		wrap = false;
		str += QString("<tr><td>%1:</td>").arg(ip, 4, 16, QLatin1Char('0'));

		Instruction instruction = (Instruction)((int)(unsigned char)ram[ip] % MAX_I);
		if(ip + inst_bytes[(int)instruction] >= ram.size())
			wrap = true;
		str += "<td>";
		for(int i = 0; i < inst_bytes[(int)instruction]; i++) {
			v = (int)(unsigned char)ram[(ip + i) % ram.size()];
			if(v == 0)
				str += QString("<font color='red'>%1</font> ").arg(v, 2, 16, QLatin1Char('0'));
			else
				str += QString("%1 ").arg(v, 2, 16, QLatin1Char('0'));
		}
		str += "</td>";

		inc_ip();
		//qDebug() << "inst:" << instruction;
		int addr;
		switch(instruction) {
		case I_NOOP:
			str += "<td>NOOP</td>";
			break;
		case I_PUSH:
			str += "<td>PUSH</td>";
			break;
		case I_POP:
			str += "<td>POP</td>";
			break;

		case I_PUSHA:
			//qDebug() << "pusha";
			addr = addy();
			str += QString("<td>PUSHA [%1]</td>").arg(addr);
			break;
		case I_POPA:
			//qDebug() << "popa";
			addr = addy();
			str += QString("<td>POPA [%1]</td>").arg(addr);
			break;
			
		case I_SUBROUTINE:
			addr = addy();
			str += QString("<td>CALL [%1]</td>").arg(addr);
			break;
		case I_RET:
			str += "<td>RET</td>";
			break;

		case I_IN: str += QString("<td>IN {%1}</td>").arg((int)(unsigned char)ram[inc_ip()] % INPUTS); break;
		case I_OUT: str += QString("<td>OUT {%1}</td>").arg((int)(unsigned char)ram[inc_ip()] % OUTPUTS); break;
			
		case I_JMP:
			addr = addy();
			str += QString("<td>JUMP [%1]</td>").arg(addr);
			break;
		case I_JMPZ:
			addr = addy();
			str += QString("<td>JMPZ [%1]</td>").arg(addr);
			break;
		case I_JMPNZ:
			addr = addy();
			str += QString("<td>JMPNZ [%1]</td>").arg(addr);
			break;

		case I_LOAD:
			addr = addy();
			str += QString("<td>LOAD [%1]</td>").arg(addr);
			break;
		case I_STORE:
			addr = addy();
			str += QString("<td>STORE [%1]</td>").arg(addr);
			break;

		case I_INC: str += "<td>INC</td>"; break;
		case I_DEC: str += "<td>DEC</td>"; break;

		case I_NOT: str += "<td>NOT</td>"; break;
		case I_AND: str += QString("<td>AND %1</td>").arg((int)(unsigned char)ram[inc_ip()]); break;
		case I_OR: str += QString("<td>OR %1</td>").arg((int)(unsigned char)ram[inc_ip()]); break;
		case I_NAND: str += QString("<td>NAND %1</td>").arg((int)(unsigned char)ram[inc_ip()]); break;
		case I_NOR: str += QString("<td>NOR %1</td>").arg((int)(unsigned char)ram[inc_ip()]); break;
		case I_XOR: str += QString("<td>XOR %1</td>").arg((int)(unsigned char)ram[inc_ip()]); break;

		case I_ADD: str += QString("<td>ADD %1</td>").arg((int)(unsigned char)ram[inc_ip()]); break;
		case I_SUB: str += QString("<td>SUB %1</td>").arg((int)(unsigned char)ram[inc_ip()]); break;
		case I_MUL: str += QString("<td>MUL %1</td>").arg((int)(unsigned char)ram[inc_ip()]); break;
		case I_DIV: str += QString("<td>DIV %1</td>").arg((int)(unsigned char)ram[inc_ip()]); break;

		case I_RAND:
#ifdef ENABLE_RAND
			str += "<td>RAND</td>";
#else
			str += "<td>NOOP</td>";
#endif
			break;
			
		default: // I_NOOP;
			str += "<td>NOOP</td>";
			break;
		}
		str += "</tr>";
		lines++;
	}

	ip = ip_save;

	str += "</table></body></html>";
	return str;
}
