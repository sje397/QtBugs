#ifndef _DNA_INC
#define _DNA_INC

#include <QByteArray>

class MutationTable {
public:
	float add_chunk;
	float remove_chunk;
	float swap_chunks;
	float reverse_section;
	float swap_bit;
	float random_byte;
};

class DNA {
public:
	DNA();
	DNA(QByteArray &d, float mut);
	virtual ~DNA() {}
	
	void set_data(QByteArray *d) {data = d;}
	void blend(const DNA &mum, const DNA &dad, int max_size, int min_size);
	const QByteArray &getData() const {return *data;}

	int size() const {return data->size();}
	
	void set_mutation(float mut) {mutation = mut;}
	float get_mutation() const {return mutation;}
protected:
	QByteArray *data;
	float mutation;
	
	void add_chunk(int max_size);
	void remove_chunk(int min_size);
	void swap_chunks();
	void reverse_section();
};

#endif
