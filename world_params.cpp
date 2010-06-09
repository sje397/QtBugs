#include "world_params.h"

WorldParams::WorldParams():
	min_data(256), max_data(512),
	max_age(6000),
	child_energy(80), min_energy(20), max_energy(400),
	min_e_energy(10), max_e_energy(40),
	move_energy(1), stay_energy(1),
	total_energy(60000), min_bug_energy(10000), min_energy_energy(40000),
	pd_width(240), pd_height(240),
	steps_per_update(32), stack_size(64),
        mutation(0.01f),
	energy_layout(EL_DOT),
	kill_on_edge(true)
{
}
QDataStream& operator<<(QDataStream& stream, const WorldParams &params) {
	stream << (quint16)3; // version number
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
	if(ver == 3) {
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
		stream >> params.kill_on_edge;
	} else {
		stream >> params.min_data >> params.max_data;
		stream >> params.max_age;
		float ce, mine, maxe, minee, maxee, movee, staye, totale, minbuge, minenergye;

		//stream >> params.child_energy >> params.min_energy >> params.max_energy;
		//stream >> params.min_e_energy >> params.max_e_energy;
		//stream >> params.move_energy >> params.stay_energy;
		//stream >> params.total_energy >> params.min_bug_energy >> params.min_energy_energy;
		stream >> ce >> mine >> maxe >> minee >> maxee >> movee >> staye >> totale >> minbuge >> minenergye;
		params.child_energy = (int)ce;
		params.min_energy = (int)mine;
		params.max_energy = (int)maxe;
		params.min_e_energy = (int)minee;
		params.max_e_energy = (int)maxee;
		params.move_energy = (int)movee;
		params.stay_energy = (int)staye;
		params.total_energy = (int)totale;
		params.min_bug_energy = (int)minbuge;
		params.min_energy_energy = (int)minenergye;

		stream >> params.pd_width >> params.pd_height;
		stream >> params.steps_per_update >> params.stack_size;
		stream >> params.mutation;
		stream >> l; params.energy_layout = (EnergyLayout)l;
		if(ver > 1) {
			stream >> params.kill_on_edge;
		}
	}
	return stream;
}

