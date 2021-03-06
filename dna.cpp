#include "dna.h"
#include <cstdlib>

#define CROSSOVER_VAL	0

MutationTable default_mutation_table = {
        0.011f,	// add chunk
        0.009f,	// remove chunk
	0.1f,	// move chunk
        0.01f,	// reverse chunk
        0.58f,	// swap bit
        0.29f,	// random byte
	0.00f,   // add crossover
};

DNA::DNA(): data(0), mutation(0) {
}

DNA::DNA(QByteArray &d, float mut): data(&d), mutation(mut) {
}

void DNA::blend(const DNA &mum, const DNA &dad, int max_size, int min_size) {
	const QByteArray &mumData = mum.getData();
	const QByteArray &dadData = dad.getData();
	const QByteArray *current, *other;
	//int coin = qrand() % 2, s;
	data->resize(qMax(mumData.size(), dadData.size()));
	//int s;
	//data->resize(s = qMax(mumData.size(),dadData.size()));

	unsigned int combo = qrand() % 2;
	current = (combo ? &mumData : &dadData);
	other = (combo == 0 ? &mumData : &dadData);
	int i = 0;
	for(; i < current->size(); i++) {
		if(i < other->size()) {
			(*data)[i] = (current->at(i) & 0xf)  + (other->at(i) & 0xf0);
			// crossover - ensure it exists in both parents genes or we end up collecting all the crossovers and having too many
			if(current->at(i) == CROSSOVER_VAL && other->at(i) == CROSSOVER_VAL) {
				combo = (combo + 1) % 2;
				current = (combo ? &mumData : &dadData);
				other = (combo == 0 ? &mumData : &dadData);
			}
		} else
			(*data)[i] = current->at(i);
	}
	data->resize(i);

	mutate(max_size, min_size);
}

void DNA::set_from(const DNA &parent, int max_size, int min_size) {
	*data = parent.getData();
	mutate(max_size, min_size);
}

void DNA::mutate(int max_size, int min_size) {
	// random mutation
	if(qrand()/(float)RAND_MAX < mutation) {
		// yep, we have a mutation - figure out the type according to the mutation table
		float r = qrand()/(float)RAND_MAX;
		if(r < default_mutation_table.add_chunk) {
			add_chunk(max_size);
			return;
		}
		r -= default_mutation_table.add_chunk;
		if(r < default_mutation_table.remove_chunk) {
			remove_chunk(min_size);
			return;
		}
		r -= default_mutation_table.remove_chunk;
		if(r < default_mutation_table.move_chunk) {
			move_chunk();
			return;
		}
		r -= default_mutation_table.move_chunk;
		if(r < default_mutation_table.reverse_section) {
			reverse_section();
			return;
		}
		r -= default_mutation_table.reverse_section;
		if(r < default_mutation_table.swap_bit) {
			int pos = qrand() % data->size();
			unsigned char mask = 1 << (qrand() % 8);

			(*data)[pos] = ((*data)[pos] ^ mask);
			return;
		}
		r -= default_mutation_table.swap_bit;
		if(r < default_mutation_table.random_byte) {
			int pos = qrand() % data->size();
			(*data)[pos] = qrand() % 0xff;
			return;
		}
		r -= default_mutation_table.random_byte;
		if(r < default_mutation_table.add_crossover) {
			int pos = qrand() % data->size();
			(*data)[pos] = CROSSOVER_VAL; // crossover value
			return;
		}
	}
}

void DNA::add_chunk(int max_size) {
	if(data->size() >= max_size) return;

	int len = qrand() % qMin(max_size - data->size(), data->size()),
	pos2 = qrand() % (data->size() - len),
	pos1 = qrand() % data->size();

	if(len > 0) data->insert(pos1, data->mid(pos2, len));
}

void DNA::remove_chunk(int min_size) {
	if(data->size() <= min_size) return;

	int len = qrand() % (data->size() - min_size),
	pos = qrand() % (data->size() - len);

	if(len > 0) data->remove(pos, len);
}

void DNA::move_chunk() {
	int len = qrand() % data->size(),
	pos1 = qrand() % (data->size() - len),
	pos2 = qrand() % (data->size() - len);

	if(len > 0) {
		QByteArray temp = data->mid(pos1, len);
		data->remove(pos1, len);
		data->insert(pos2, temp);
	}
}

void DNA::reverse_section() {
	int len = qrand() % (data->size()),
	pos = qrand() % (data->size() - len);

	if(len > 0) {
		QByteArray temp = data->mid(pos, len);
		for(int i = 0; i < temp.size(); i++) {
			(*data)[pos + i] = temp.at(temp.size() - 1 - i);
		}
	}
}

