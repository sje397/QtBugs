#include "dna.h"
#include <cstdlib>

MutationTable default_mutation_table = {
	0.11f,	// add chunk
	0.09f,	// remove chunk
	0.1f,	// swap chunks
	0.1f,	// reverse section
	0.4f,	// swap bit
	0.2f	// random byte
};

DNA::DNA(): data(0), mutation(0) {
}

DNA::DNA(QByteArray &d, float mut): data(&d), mutation(mut) {
}

void DNA::blend(const DNA &mum, const DNA &dad, int max_size, int min_size) {
	const QByteArray &mumData = mum.getData();
	const QByteArray &dadData = dad.getData();
	const QByteArray *current;
	//int coin = qrand() % 2, s;
	data->resize(max_size);
	//int s;
	//data->resize(s = qMax(mumData.size(),dadData.size()));

	unsigned int combo = qrand() % 2;
	current = (combo ? &mumData : &dadData);
	for(int i = 0; i < current->size(); i++) {
		// crosover half-way through dna		
		if(i > 0 && current->at(i) == 0) {// && current->at(i - 1) == 0) { // crossover value - NOOP
			combo = (combo + 1) % 2;
			current = (combo ? &mumData : &dadData);
			if(current->size() <= i) break;
		}
		(*data)[i] = (*current)[i];
	}
	data->resize(current->size());

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
		if(r < default_mutation_table.swap_chunks) {
			swap_chunks();
			return;
		}
		r -= default_mutation_table.swap_chunks;
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
		}
	}
}

void DNA::add_chunk(int max_size) {
	if(data->size() >= max_size) return;

	int len = qrand() % qMin(max_size - data->size(), data->size()),
		pos = qrand() % (data->size() - len);
	
	if(len > 0) data->append(data->mid(pos, len));
}

void DNA::remove_chunk(int min_size) {
	if(data->size() == min_size) return;

	int len = qrand() % qMin(data->size() - min_size, data->size()),
		pos = qrand() % (data->size() - len);
	
	if(len > 0) {
		QByteArray new_val = data->left(pos);
		new_val.append(data->mid(pos + len));
		*data = new_val;
	}
}

void DNA::swap_chunks() {
	int len = qrand() % (data->size()/2),
		pos = qrand() % (data->size()/2 - len);
	
	if(len > 0) {
		QByteArray temp = data->mid(pos, len);
		for(int i = 0; i < temp.size(); i++) {
			(*data)[pos + i] = (*data)[pos + data->size()/2 + i];
			(*data)[pos + data->size()/2 + i] = temp[i];
		}
	}
}

void DNA::reverse_section() {
	int len = qrand() % data->size(),
		pos = qrand() % (data->size() - len);
	
	if(len > 0) {
		QByteArray temp = data->mid(pos, len);
		for(int i = 0; i < temp.size(); i++) {
			(*data)[pos + i] = temp[temp.size() - 1 - i];
		}
	}
}

