#include <QPoint>
#include <QColor>


class EnergyNode: public QPoint {
public:
	EnergyNode():QPoint(0, 0), energy(0) {}
	EnergyNode(float e, int x, int y);
	virtual ~EnergyNode() {}

	float get_energy() const {return energy;}
	void set_energy(float e) {energy = e;}
	void dec_energy(float e) {energy -= e;}
	void inc_energy(float e) {energy += e;}

	/*
	bool operator==(const EnergyNode &other) const {
		return x() == other.x() && y() == other.y();
	}
	*/
	
protected:
	float energy;
};
