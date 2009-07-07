#include <QPoint>
#include <QColor>


class EnergyNode: public QPoint {
public:
	EnergyNode():QPoint(0, 0), energy(0) {}
	EnergyNode(int e, int x, int y);
	virtual ~EnergyNode() {}

	int get_energy() const {return energy;}
	void set_energy(int e) {energy = e;}
	void dec_energy(int e) {energy -= e;}
	void inc_energy(int e) {energy += e;}

	/*
	bool operator==(const EnergyNode &other) const {
		return x() == other.x() && y() == other.y();
	}
	*/
	
protected:
	int energy;
};
