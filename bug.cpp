#include "bug.h"
#include <cmath>
#include <QDebug>

QDataStream& operator<<(QDataStream& stream, Bug &bug) {
	stream << bug.age << bug.children << bug.generation;
	stream << bug.energy;
	stream << bug.dir << bug.color << bug.split;
	stream << bug.data;
	stream << bug.get_dna().get_mutation();
	stream << bug.processor;
	return stream;
}

Bug::Bug(QDataStream &in): EnergyNode(), dna(), processor() {
	float mutation;
	
	in >> age >> children >> generation;
	in >> energy;
	in >> dir >> color >> split;
	in >> data;
	in >> mutation;
	in >> processor;
	dna.set_data(&data);
	dna.set_mutation(mutation);
}

Bug::Bug(int data_size,  float mut, int steps_per_update, int stack_size): EnergyNode(), dna(data, mut), processor(steps_per_update, stack_size), dir(4), split(0), age(0), children(0), generation(0) {
	data.resize(data_size);
	for(int i = 0; i < data_size; i++) {
		data[i] = qrand() % 256;
	}	
	processor.set_data(data);
	
	/*
	int r, g, b, a;
	r = 0xff - processor.get_out(1); // subtract so that default
	g = 0xff - processor.get_out(2);
	b = 0xff - processor.get_out(3);
	//a = processor.get_out(4);
	a = 0xff;
	
	color.setRgb(r, g, b, a);		
	*/

	dir = (processor.get_out(0) + 4) % 9; // 4 is stay still - default out values are 0
	color = processor.get_out(1) % 254 + 1; // col 255 reserved for energy, col 0 for background
	split = processor.get_out(2);
}

Bug::Bug(Bug *mummy, Bug *daddy, float mut, int max_size, int min_size, int steps_per_update, int stack_size): EnergyNode(), dna(data, mut), processor(steps_per_update, stack_size), dir(4), split(0), age(0), children(0) {
	data.reserve(max_size);

	mum = mummy;
	dad = daddy;
	dna.blend(mum->get_dna(), dad->get_dna(), max_size, min_size);
	
	setX(mum->x());
	setY(mum->y());
	mum->add_child();
	dad->add_child();
	
	generation = qMax(mum->generation + 1, dad->generation + 1);

	processor.set_data(data);

	dir = (processor.get_out(0) + 4) % 9; // 4 is stay still - default out values are 0
	color = processor.get_out(1) % 254 + 1; // col 255 reserved for energy, col 0 for background
	split = processor.get_out(2);
}

Bug::Bug(Bug *parent, float mut, int max_size, int min_size, int steps_per_update, int stack_size): EnergyNode(), dna(data, mut), processor(steps_per_update, stack_size), dir(4), split(false), age(0), children(0) {
	data.reserve(max_size);
	dna.set_from(parent->get_dna(), max_size, min_size);

	setX(parent->x());
	setY(parent->y());
	parent->add_child();

	generation = parent->get_generation() + 1;

	processor.set_data(data);

	dir = (processor.get_out(0) + 4) % 9; // 4 is stay still - default out values are 0
	color = processor.get_out(1) % 254 + 1; // col 255 reserved for energy, col 0 for background
	split = processor.get_out(2);
}

Bug::~Bug() {
	emit dying();
}

void Bug::set_vision(unsigned char vision[9]) {
	// read vision into start of memory
	//qDebug() << "setting inputs";
	//for(int i = 0; i < 9; i++) {
	//	processor.set_in(i, vision[i]);
	//}
	processor.set_in_multi(0, 9, vision);

	updated = false;
}

void Bug::update() {
	processor.set_in(9, (unsigned char)((int)energy / 256));
	processor.set_in(9 + 1, (unsigned char)((int)energy % 256));
	
	// do one step of the processor
	//qDebug() << "processor step";
	processor.update();
	
	//qDebug() << "getting outputs";
	// read and apply direction
	dir = (processor.get_out(0) + 4) % 9; // 4 is stay still - default out values are 0		
	// read and apply new colour
	color = processor.get_out(1) % 254 + 1; // col 255 reserved for energy, col 0 for background
	split = processor.get_out(2);
	age++;
	
	updated = true;
	emit changed();
}
