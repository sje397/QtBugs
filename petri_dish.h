#include "world_params.h"

#include <QObject>

#include <QHash>
#include <QList>
#include <QPixmap>
#include <QPainter>
#include <QColor>
#include <QDebug>
#include <QSize>
#include <QDataStream>

#include <QThread>
#include <QWaitCondition>
#include <QMutex>
#include <QMutexLocker>
#include <QVector>

#include "bug.h"
#include "petriwidget.h"

#define BOUND(x, y)   (x >= 0 ? ((x) % (y)) : ((x) + (y)))

#define WRAP_X(d,x)		(d % 3 == 0 ? (x == 0 ? width - 1 : x - 1) : (d % 3 == 2 ? (x == width - 1 ? 0 : x + 1) : x))
#define WRAP_Y(d,y)		(d / 3 == 0 ? (y == 0 ? height - 1 : y - 1) : (d / 3 == 2 ? (y == height - 1 ? 0 : y + 1) : y))

#define X(d, x)			(d % 3 == 0 ? x - 1 : (d % 3 == 2 ? x + 1 : x))
#define Y(d,y)			(d / 3 == 0 ? y - 1 : (d / 3 == 2 ? y + 1 : y))

class PetriDish: public QObject {
	Q_OBJECT

	friend QDataStream& operator<<(QDataStream& stream, const PetriDish &dish);
	friend QDataStream& operator>>(QDataStream& stream, PetriDish &dish);

public:
	typedef enum {VM_DEFAULT, VM_BUGS, VM_ENERGY, VM_AGE, VM_DNA_SIZE, VM_DNA_VALUE, VM_TERRAIN} ViewMode;

	PetriDish();
	void init(WorldParams params);
	void cleanup();
	virtual ~PetriDish();

	int get_total_energy() const {QMutexLocker l(&dataMutex); return world_params.total_energy;}
	int get_bug_energy() const {QMutexLocker l(&dataMutex); return bugEnergy;}
	int get_energy_energy() const {QMutexLocker l(&dataMutex); return energyEnergy;}

	unsigned long get_time() const {QMutexLocker l(&dataMutex); return time;}
	unsigned long get_max_gen() const {QMutexLocker l(&dataMutex); return max_gen;}
	unsigned long get_population() const {QMutexLocker l(&dataMutex); return population;}

	void set_view_mode(ViewMode vm);
	void set_show_byte(int byte);

	WorldParams get_world_params() {QMutexLocker l(&dataMutex); return world_params;}
	void set_world_params(const WorldParams &params) {QMutexLocker l(&dataMutex); world_params = params;}

	// warning - this bug may be deleted if the step function is called (it will emit the 'dying' signal)
	Bug *get_closest_bug(int x, int y);

	void set_widget(PetriWidget *w);

		QVector<QByteArray> get_dna_data();

public slots:
	void set_default_view_mode();
	void set_bugs_view_mode();
	void set_energy_view_mode();
	void set_age_view_mode();
	void set_dna_size_view_mode();
	void set_dna_view_mode();
	void set_terrain_view_mode();
	void enable_histogram_calc(bool enable);

	void step();

	void check_integrity();
	void update_all_pixels();

		void balance();

signals:
	void changed();
	void dnaData(const QVector<QByteArray> &dna_data, int population);

protected:
	mutable QMutex dataMutex;

	WorldParams world_params;
	int energyEnergy, bugEnergy;
	quint64 time, max_gen, max_age, population;

	QList<Bug *> *bugList; // a buglist per location
	QList<int> listHash; // a list of hashes for locations that have a non-zero pop
	int *energy;	// an energy value per location
	int *terrain;	// a terrain value per location
	unsigned char *bugCol, *viewCol;	// a color per location
	int *bugTotal; // total bug energy for each location

	int width, height;
	PetriWidget *petriWidget;

	void update_pixel(int x, int y);
	void update_pixel(int hash);
	unsigned char get_bug_color_at(int hash);
	unsigned char find_bug_color_at(int hash);
	unsigned char get_view_color_at(int hash);
	unsigned char find_view_color_at(int hash);

	void get_vision_at(int x, int y, unsigned char vis[9]);

	ViewMode viewMode;
	int showByte;
	bool calc_histogram;

	void removeBug(Bug *b, int e, int x, int y);

public: //only so we can call it from a static function using QtConcurrent::blockingMap
	void do_eating_and_seeing(int hash);
};

QDataStream& operator<<(QDataStream& stream, const PetriDish &dish);
QDataStream& operator>>(QDataStream& stream, PetriDish &dish);
