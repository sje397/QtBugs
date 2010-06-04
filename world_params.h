#ifndef _WORLD_PARAMS_H
#define _WORLD_PARAMS_H

#include <QDataStream>

typedef enum {EL_RANDOM, EL_DOT, EL_TWODOTS, EL_RING, EL_RECT, EL_MANYDOTS, EL_CHANGE} EnergyLayout;

class WorldParams {
public:
	WorldParams();

	int min_data, max_data;
	quint64 max_age;
	int child_energy, min_energy, max_energy;
	int min_e_energy, max_e_energy;
	int move_energy, stay_energy;
	int total_energy, min_bug_energy, min_energy_energy;
	int pd_width, pd_height;
	int steps_per_update, stack_size;
	float mutation;
	EnergyLayout energy_layout;
	bool kill_on_edge;
};

QDataStream& operator<<(QDataStream& stream, const WorldParams &params);
QDataStream& operator>>(QDataStream& stream, WorldParams &params);

#endif
