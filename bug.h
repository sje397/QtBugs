#ifndef _BUG_INC
#define _BUG_INC

#include "processor.h"
#include "dna.h"
#include "energy_node.h"

#include <QDataStream>
#include <QPointer>

class Bug: public QObject, public EnergyNode {
	Q_OBJECT

	friend QDataStream& operator<<(QDataStream& stream, Bug &bug);
public:
	Bug(QDataStream &in);
	Bug(int data_size, float mut, int steps_per_update, int stack_size);
	Bug(Bug *mummy, Bug *daddy, float mut, int max_size, int min_size, int steps_per_update, int stack_size);
	Bug(Bug *parent, float mut, int max_size, int min_size, int steps_per_update, int stack_size);

	virtual ~Bug();

        void set_vision(unsigned char color[9], unsigned char altitude[9], unsigned char energy[9]);
	void update();

	bool collidesWithBug(const Bug *other) const;

	void add_child() {children++;}

	DNA &get_dna() {return dna;}
	Processor &get_processor() {return processor;}

	unsigned char get_color() const {return color;}
	int get_dir() const {return dir;}
	unsigned char get_split() const {return split;}
	unsigned char get_action() const {return action;}

	unsigned long get_age() const {return age;}
	unsigned long get_children() const {return children;}
	unsigned long get_generation() const {return generation;}
	bool get_marked() const {return marked;}
	void mark() {marked = true;}
	void clear_mark() {marked = false;}
	Bug *get_mum() {return mum;}
	Bug *get_dad() {return dad;}

	int get_payload() {return payload;}
	void set_payload(int p) {payload = p;}
signals:
	void dying();
	void changed();

protected:
	QByteArray data; // the virtual machine's ROM memory, and the genetic material
	DNA dna;
	Processor processor;
	unsigned char dir;
	unsigned char split;
	unsigned char action;

	int payload; // terrain being carried

	unsigned char color;
	quint64 age, children, generation;
	bool marked;
	QPointer<Bug> mum, dad;
};

QDataStream& operator<<(QDataStream& stream, Bug &bug);

#endif
