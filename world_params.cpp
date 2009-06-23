#include "world_params.h"

WorldParams::WorldParams():
	min_data(512), max_data(2048),
	max_age(6000),
	child_energy(40.0f), min_energy(20.0f), max_energy(400.0f),
	min_e_energy(10.0f), max_e_energy(40.0f),
	move_energy(1.0f), stay_energy(1.0f),
	total_energy(100000), min_bug_energy(40000), min_energy_energy(40000),
	pd_width(240), pd_height(240),
	steps_per_update(32), stack_size(64),
	mutation(0.1f),
	energy_layout(EL_DOT),
	kill_on_edge(false)
{
}
QDataStream& operator<<(QDataStream& stream, const WorldParams &params) {
	stream << (quint16)2; // version number
	stream << params.min_data << params.max_data;
	stream << params.max_age;
	stream << params.child_energy << params.min_energy << params.max_energy;
	stream << params.min_e_energy << params.max_e_energy;
	stream << params.move_energy << params.stay_energy;
	stream << params.total_energy << params.min_bug_energy << params.min_energy_energy;
	stream << params.pd_width << params.pd_height;
	stream << params.steps_per_update << params.stack_size;
	stream << params.mutation;
	stream << (int)params.energy_layout;
	stream << params.kill_on_edge;
	return stream;
}

QDataStream& operator>>(QDataStream& stream, WorldParams &params) {
	quint16 ver;
	int l;
	stream >> ver;
	stream >> params.min_data >> params.max_data;
	stream >> params.max_age;
	stream >> params.child_energy >> params.min_energy >> params.max_energy;
	stream >> params.min_e_energy >> params.max_e_energy;
	stream >> params.move_energy >> params.stay_energy;
	stream >> params.total_energy >> params.min_bug_energy >> params.min_energy_energy;
	stream >> params.pd_width >> params.pd_height;
	stream >> params.steps_per_update >> params.stack_size;
	stream >> params.mutation;
	stream >> l; params.energy_layout = (EnergyLayout)l;
	if(ver > 1) {
		stream >> params.kill_on_edge;
	}
	return stream;
}

