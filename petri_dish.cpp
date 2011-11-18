#include "petri_dish.h"

#include <cstdlib>
#include <cmath>
#include <QDebug>
#include <QtConcurrentMap>
#include <QTime>

#ifndef M_PI
#define M_PI		3.141592653589793238462643383279502884197169399375105820974944592
#endif

#define MAX_TERRAIN 10

#include <QPainter>
#include <QBrush>

PetriDish::PetriDish(): QObject(),
    energyEnergy(0), bugEnergy(0),
    time(0), max_gen(0), max_age(0), population(0),
    bugList(0), energy(0), bugCol(0), viewCol(0), bugTotal(0),
    viewMode(VM_DEFAULT), showByte(0), calc_histogram(false)
{
}

void PetriDish::set_widget(PetriWidget *w) {
    petriWidget = w;
}

void PetriDish::init(WorldParams params) {
    cleanup();

    {
        QMutexLocker l(&dataMutex);

        world_params = params;
        width = world_params.pd_width;
        height = world_params.pd_height;

        time = 0;

        petriWidget->set_image_size(width, height);

        bugList = new QList<Bug *>[width * height];
        energy = new int[width * height];
        terrain = new int[width * height];
        bugTotal = new int[width * height];
        bugCol = new unsigned char [width * height];
        viewCol = new unsigned char [width * height];
        poison = new bool [width * height];
        for(int i = 0; i < width * height; i++) {
            energy[i] = bugTotal[i] = 0;
            bugCol[i] = viewCol[i] = 0;
            terrain[i] = 1;
            poison[i] = false;
        }

        int x, y, hash, count = params.poison;
        while(count > 0) {
            x = qrand() % width;
            y = qrand() % height;
            hash = x * height + y;
            if(!poison[hash]) {
                poison[hash] = true;
                count--;
            }
        }
    }

    if(calc_histogram) {
        emit dnaData(get_dna_data(), population);
    }

    update_all_pixels();
}

void PetriDish::cleanup() {
    QMutexLocker l(&dataMutex);
    if(energy) {delete[] energy; energy = 0;}
    if(terrain) {delete[] terrain; terrain = 0;}
    if(bugTotal) {delete[] bugTotal; bugTotal = 0;}
    if(bugCol) {delete[] bugCol; bugCol = 0;}
    if(viewCol) {delete[] viewCol; viewCol = 0;}
    if(poison) {delete[] poison; poison = 0;}

    if(bugList) {
        QListIterator<int> i(listHash);
        int hash;
        while(i.hasNext()) {
            hash = i.next();
            //for(int hash = 0; hash < width * height; hash++)
            for(int j = 0; j < bugList[hash].size(); j++)
                delete bugList[hash].at(j);
        }
        delete[] bugList;
        bugList = 0;
    }

    listHash.clear();

    bugEnergy = energyEnergy = 0;
    max_gen = max_age = population = 0;
}

QDataStream& operator<<(QDataStream& stream, const PetriDish &dish) {
    QMutexLocker l(&dish.dataMutex);
    stream << dish.time;
    //stream << (int)dish.viewMode;

    for(int i = 0; i < dish.width * dish.height; i++) {
        stream << dish.energy[i];
        stream << dish.terrain[i];
        stream << dish.poison[i];
        stream << dish.bugList[i].size();
        QListIterator<Bug *> j(dish.bugList[i]);
        while(j.hasNext())
            stream << *j.next();
    }

    return stream;
}

QDataStream& operator>>(QDataStream& stream, PetriDish &dish) {
    QMutexLocker l(&dish.dataMutex);
    stream >> dish.time;
    //int vm;
    //stream >> vm;
    //dish.viewMode = (PetriDish::ViewMode)vm;

    Bug *bug;
    int size;
    for(int i = 0; i < dish.width * dish.height; i++) {
        stream >> dish.energy[i];
        stream >> dish.terrain[i];
        stream >> dish.poison[i];
        if(dish.energy[i] > 0) {
            dish.energyEnergy += dish.energy[i];
        }

        stream >> size;
        if(size > 0) {
            for(int j = 0; j < size; j++) {
                bug = new Bug(stream);
                dish.bugList[i].append(bug);
                dish.population++;
                dish.bugEnergy += bug->get_energy();
                dish.bugTotal[i] += bug->get_energy();
            }
            dish.listHash.append(i);
        }
    }

    dish.update_all_pixels();
    return stream;
}


const unsigned char EnergyColor = 255;
const unsigned char PoisonColor = 254;
const unsigned char MaxBugColor = 252;
const unsigned char BkgColor = 0;

PetriDish::~PetriDish() {
    cleanup();
}

void PetriDish::set_view_mode(ViewMode vm) {
    {
        QMutexLocker l(&dataMutex);
        viewMode = vm;
        update_all_pixels();
    }
    emit changed();
}

void PetriDish::set_show_byte(int byte) {
    {
        QMutexLocker l(&dataMutex);
        showByte = byte;
        update_all_pixels();
    }
    emit changed();
}

void PetriDish::set_default_view_mode() {
    set_view_mode(VM_DEFAULT);
}

void PetriDish::set_bugs_view_mode() {
    set_view_mode(VM_BUGS);
}

void PetriDish::set_energy_view_mode() {
    set_view_mode(VM_ENERGY);
}

void PetriDish::set_age_view_mode() {
    set_view_mode(VM_AGE);
}

void PetriDish::set_dna_size_view_mode() {
    set_view_mode(VM_DNA_SIZE);
}

void PetriDish::set_dna_view_mode() {
    set_view_mode(VM_DNA_VALUE);
}

void PetriDish::set_terrain_view_mode() {
    set_view_mode(VM_TERRAIN);
}

void PetriDish::update_pixel(int x, int y) {
    //image.setPixel(x, y, colorTable[get_view_color_at(x, y)].rgb());
    //painter->setPen(colorTable[get_view_color_at(x, y)]);
    //painter->drawPoint(x, y);
    int hash = x * height + y;
    bugCol[hash] = find_bug_color_at(hash);
    viewCol[hash] = find_view_color_at(hash);
    petriWidget->update_pixel(x, y, viewCol[hash]);
}

void PetriDish::update_pixel(int hash) {
    //int x = hash % height, y = hash / height;
    int x = hash / height, y = hash % height;
    //painter->setPen(colorTable[get_view_color_at(x, y)]);
    //painter->drawPoint(x, y);
    bugCol[hash] = find_bug_color_at(hash);
    viewCol[hash] = find_view_color_at(hash);
    petriWidget->update_pixel(x, y, viewCol[hash]);
}

void PetriDish::update_all_pixels() {
    for(int x = 0; x < width; x++) {
        for(int y = 0; y < height; y++) {
            //image.setPixel(x, y, get_view_color_at(x, y).rgba());
            update_pixel(x, y);
        }
    }
    petriWidget->update();
}

unsigned char PetriDish::get_bug_color_at(int hash) {
    return bugCol[hash];
}

unsigned char PetriDish::find_bug_color_at(int hash) {
    if(bugList[hash].size()) {//listHash.contains(hash)) {
        return (bugList[hash].first()->get_color() % MaxBugColor) + 1;
    } else {
        if(poison[hash]) return PoisonColor;
        else if(energy[hash]) return EnergyColor;
        else return BkgColor;
    }
}

unsigned char PetriDish::get_view_color_at(int hash) {
    return viewCol[hash];
}

unsigned char PetriDish::find_view_color_at(int hash) {
    switch(viewMode) {
    case VM_DEFAULT:
        return bugCol[hash];
    case VM_BUGS:
    {
        if(bugList[hash].size()) //listHash.contains(hash))
            //return bugList[hash].first()->get_color();
            return bugCol[hash];
    }
    break;
    case VM_ENERGY:
    {
        if(bugList[hash].size()) //listHash.contains(hash))
            return (int)((MaxBugColor - 1) * (bugList[hash].first()->get_energy() - world_params.min_energy) / (float)(world_params.max_energy - world_params.min_energy)) + 1;
    }
    break;
    case VM_AGE:
    {
        if(bugList[hash].size()) //listHash.contains(hash))
            return (int)((MaxBugColor - 1) - (MaxBugColor - 1) * bugList[hash].first()->get_age() / (float)world_params.max_age) + 1;
    }
    break;
    case VM_DNA_SIZE:
    {
        if(bugList[hash].size()) //listHash.contains(hash))
            return (int)((MaxBugColor - 1) * (bugList[hash].first()->get_dna().size() - world_params.min_data) / (float)(world_params.max_data - world_params.min_data)) + 1;
    }
    break;
    case VM_DNA_VALUE:
    {
        if(bugList[hash].size()) { //listHash.contains(hash))
            const QByteArray &dna = bugList[hash].first()->get_dna().getData();
            if(dna.size() > showByte) {
                return ((unsigned char)dna.at(showByte) % MaxBugColor) + 1;
            }
        }
    }
    break;
    case VM_TERRAIN:
        if(bugList[hash].size()) return EnergyColor;
        return terrain[hash] == 1 ? BkgColor : (terrain[hash] == 0 ? 1 : (int)(terrain[hash]/(float)MAX_TERRAIN * 127 + 127));
        break;
    }
    return BkgColor;
}

void PetriDish::get_vision_at(int x, int y, unsigned char vis[9]) {
    int nx, ny;
    for(int d = 0; d < 9; d++) {
        //vis[d] = get_bug_color_at(WRAP_X(d, x), WRAP_Y(d, y));

        nx = WRAP_X(d, x); ny = WRAP_Y(d, y);
        if(world_params.kill_on_edge && (nx>= width || nx < 0 || ny >= height || ny < 0))
            vis[d] = 0;
        else
            vis[d] = bugCol[nx * height + ny];
    }
}

void PetriDish::get_terrain_at(int x, int y, unsigned char ter[9]) {
    int nx, ny;
    for(int d = 0; d < 9; d++) {
        //vis[d] = get_bug_color_at(WRAP_X(d, x), WRAP_Y(d, y));

        nx = WRAP_X(d, x); ny = WRAP_Y(d, y);
        if(world_params.kill_on_edge && (nx>= width || nx < 0 || ny >= height || ny < 0))
            ter[d] = 0;
        else
            ter[d] = (unsigned char)(terrain[x * height + y]/(float)MAX_TERRAIN * 255);
    }
}

void PetriDish::get_energy_at(int x, int y, unsigned char en[9]) {
    int nx, ny;
    for(int d = 0; d < 9; d++) {
        //vis[d] = get_bug_color_at(WRAP_X(d, x), WRAP_Y(d, y));

        nx = WRAP_X(d, x); ny = WRAP_Y(d, y);
        if(world_params.kill_on_edge && (nx>= width || nx < 0 || ny >= height || ny < 0))
            en[d] = 0;
        else
            en[d] = (unsigned char)(energy[x * height + y]/(float)world_params.max_e_energy * 255);
    }
}

void get_coords(int &x, int &y, EnergyLayout layout, int width, int height, quint64 time) {
    float a, r;
    int d;
    switch(layout) {
    case EL_RANDOM:
        x = qrand() % width;
        y = qrand() % height;
        break;
    case EL_DOT:
        a = M_PI * 2 * (qrand() / (float)RAND_MAX);
        r = qMin(width/5, height/5) * sqrt(qrand() / (float)RAND_MAX);
        x = (int)(width / 2 + r * sin(a));
        y = (int)(height / 2 + r * cos(a));
        break;
    case EL_TWODOTS:
        a = M_PI * 2 * (qrand() / (float)RAND_MAX);
        r = qMin(width/10, height/10) * sqrt(qrand() / (float)RAND_MAX);
        if(qrand() % 2) {
            x = (int)(width / 4 + r * sin(a));
            y = (int)(height / 2 + r * cos(a));
        } else {
            x = (int)(width * 3 / 4 + r * sin(a));
            y = (int)(height / 2 + r * cos(a));
        }
        break;
    case EL_MANYDOTS:
        a = M_PI * 2 * (qrand() / (float)RAND_MAX);
        r = qMin(width/32, height/32) * sqrt(qrand() / (float)RAND_MAX);
        d = qrand() % 16;
        x = (int)(width / 5 * ((d / 4) + 1) + r * sin(a));
        y = (int)(height / 5 * ((d % 4) + 1) + r * cos(a));
        break;
    case EL_RING:
        a = M_PI * 2 * (qrand() / (float)RAND_MAX);
        r = qMin(width/10, height/10) * sqrt(qrand() / (float)RAND_MAX) + qMin(width/10, height/10);
        x = (int)(width / 2 + r * sin(a));
        y = (int)(height / 2 + r * cos(a));
        break;
    case EL_RECT:
        x = qrand() % width / 2 + width / 4;
        y = qrand() % height / 4 + height * 3 / 8;
        break;
    case EL_CHANGE:
        if((time / 2000) % 2) {
            a = M_PI * 2 * (qrand() / (float)RAND_MAX);
            r = qMin(width/5, height/5) * sqrt(qrand() / (float)RAND_MAX);
            x = (int)(width / 2 + r * sin(a));
            y = (int)(height / 2 + r * cos(a));
        } else {
            a = M_PI * 2 * (qrand() / (float)RAND_MAX);
            r = qMin(width/10, height/10) * sqrt(qrand() / (float)RAND_MAX);
            if(qrand() % 2) {
                x = (int)(width / 4 + r * sin(a));
                y = (int)(height / 2 + r * cos(a));
            } else {
                x = (int)(width * 3 / 4 + r * sin(a));
                y = (int)(height / 2 + r * cos(a));
            }
        }
        break;
    }
}

void PetriDish::balance() {
    int e = 0;
    int remainingEnergy = world_params.total_energy - bugEnergy - energyEnergy;
    int x = 0, y = 0;
    int hash;

    // add new random bugs
    //qDebug() << "adding random bugs";
    //check_integrity();
    Bug *bug;
    while(bugEnergy < world_params.min_bug_energy && remainingEnergy >= world_params.max_energy) {
        e = (int)(qrand()/(float)RAND_MAX * (world_params.max_energy - world_params.min_energy)) + world_params.min_energy;
        do {
            // initially place bugs in energy
            get_coords(x, y, world_params.energy_layout, width, height, time);
            hash = x * height + y;
        } while(poison[hash]);

        bugEnergy += e;
        remainingEnergy -= e;

        if(world_params.max_data > world_params.min_data)
            bug = new Bug((qrand() % (world_params.max_data - world_params.min_data)) + world_params.min_data, world_params.mutation, world_params.steps_per_update, world_params.stack_size);
        else
            bug = new Bug(world_params.min_data, world_params.mutation, world_params.steps_per_update, world_params.stack_size);
        bug->set_energy(e);
        bugTotal[hash] += e;
        //bug->setX(qrand() % width);
        //bug->setY(qrand() % height);

        population++;

        if(bugList[hash].size() == 0) listHash.append(hash);
        bugList[hash].prepend(bug);
        update_pixel(x, y);
    }

    // add new energy
    //qDebug() << "adding energy";
    //check_integrity();
    int min_e, max_e;
    while(energyEnergy < world_params.min_energy_energy && remainingEnergy >= world_params.max_e_energy) {
        do {
            get_coords(x, y, world_params.energy_layout, width, height, time);
            hash = x * height + y;
        } while(poison[hash]);

        max_e = world_params.max_e_energy - energy[hash];
        if((min_e = world_params.min_energy - energy[hash]) > 0) {
            if(max_e > min_e)
                e = (int)(qrand()/(float)RAND_MAX * (max_e -  min_e)) + min_e;
            else
                e = min_e;
        }else
            continue;

        energyEnergy += e;
        remainingEnergy -= e;

        energy[hash] += e;
        update_pixel(x, y);
    }
}

void PetriDish::removeBug(Bug *bug, int e, int x, int y) {
    //dataMutex.lock();
    bugEnergy -= e;
    population--;
    //dataMutex.unlock();

    bugTotal[x * height + y] -= e;
    terrain[x * height + y] += bug->get_payload();
    //update_pixel(x, y);
    delete bug;
}

/*
unsigned char vis[9];
void set_vis_and_update(Bug *b) {
 b->set_vision(vis);
 b->clear_mark();
 b->update();
}
*/

void PetriDish::do_eating_and_seeing(int hash) {
    int size = bugList[hash].size();
    if(size == 0) {
        qDebug() << "0!!!(1)";
        return;
    }
    int total_bug_e, e2b = 0, eSub = 0;
    Bug *bug;

    QListIterator<Bug *> i(bugList[hash]);

    // modify terrain
    while(i.hasNext()) {
        bug = i.next();
        //if(bug->get_generation() > 10) {
            switch(bug->get_action() % 128) {
            case 0: // bug drops terrain
                if(bug->get_payload() > 0 && abs(terrain[hash]) < MAX_TERRAIN) {
                    terrain[hash] += bug->get_payload();
                    bug->set_payload(0);
                }
                break;
            case 1: // bug collects terrain
                if(bug->get_payload() == 0 && terrain[hash] > 0) {
                    terrain[hash] -= 1;
                    bug->set_payload(1);
                }
                break;

            // the following three cases might help flatten

            case 2: // bug collects terrain if elevation > 1
                if(bug->get_payload() == 0 && terrain[hash] > 1) {
                    terrain[hash] -= 1;
                    bug->set_payload(1);
                }
                break;
            case 3: // bug drops terrain if elevation == 0
                if(bug->get_payload() > 0 && terrain[hash] == 0) {
                    terrain[hash] += bug->get_payload();
                    bug->set_payload(0);
                }
                break;
            case 4: // bug drops terrain if elevation == 0, AND collects terrain if elevation > 1
                if(bug->get_payload() == 0 && terrain[hash] > 1) {
                    terrain[hash] -= 1;
                    bug->set_payload(1);
                }
                if(bug->get_payload() > 0 && terrain[hash] == 0) {
                    terrain[hash] += bug->get_payload();
                    bug->set_payload(0);
                }
                break;
            default:
                // do nothing
                break;
            }
        //}
    }
    i.toFront();

    if(energy[hash] > 0 && (total_bug_e = bugTotal[hash]) < world_params.max_energy * size) {
        int e;
        while(i.hasNext()) {
            bug = i.next();
            e = bug->get_energy();
            if(energy[hash] >= world_params.max_energy - e) {
                bug->inc_energy(world_params.max_energy - e);
                e2b += world_params.max_energy - e;
                energy[hash] -= world_params.max_energy - e;
            } else {
                bug->inc_energy(energy[hash]);
                e2b += energy[hash];
                energy[hash] = 0;
            }
        }
        i.toFront();

        if(energy[hash] < world_params.min_e_energy) {
            eSub += energy[hash];
            energy[hash] = 0;
        }
    }

    dataMutex.lock();
    bugEnergy += e2b;
    energyEnergy -= (e2b + eSub);
    dataMutex.unlock();

    // we have changed the energy but we still all see the same because only energy hidden under bugs has changed
    int x = hash / height;
    int y = hash % height;


    unsigned char vis[9], ter[9], en[9];
    get_vision_at(x, y, vis);
    get_terrain_at(x, y, ter);
    get_energy_at(x, y, ter);

    Bug *b;
    while(i.hasNext()) {
        //taskQueue.queueTask(new See(i.next(), vis));
        b = i.next();
        b->set_vision(vis, ter, en);
        b->clear_mark();
        b->update();
    }

    //get_vision_at(x, y, vis);
    //QtConcurrent::blockingMap(bugList[hash], set_vis_and_update);
}

PetriDish *dish;
void static_eat_see(int hash) {
    dish->do_eating_and_seeing(hash);
}

void updateBug(Bug *bug) {
    bug->update();
}

void PetriDish::step() {
    {
        //QMutexLocker locker(&dataMutex);
        //dataMutex.lock();

        max_gen = 0;
        max_age = 0;
        //dataMutex.unlock();

        // eating, seeing
        //qDebug() << "eating, seeing";
        //check_integrity();
        unsigned int size;
        Bug *bug;
        int hash,x, y;
        /*
        QListIterator<int> j(listHash);
        while(j.hasNext()) {
            hash = j.next();
            do_eating_and_seeing(hash);
        }
        */
        // grr segfaults
        dish = this;
        QtConcurrent::blockingMap(listHash, static_eat_see);

        // update, move, change energy and remove dead
        //qDebug() << "update, move, charge energy and remove dead";
        //check_integrity();
        int dir, nx, ny;//, val;
        int e, move_cost;

        //QMutexLocker locker(&dataMutex);
        QSet<int> newPos;
        QMutableListIterator<int> k(listHash);
        while(k.hasNext()) {
            hash = k.next();
            x = hash / height;
            y = hash % height;
            size = bugList[hash].size();

            if(size == 0) qDebug() << "0!!!(2)";
            //qDebug() << size << "bugs here";
            QMutableListIterator<Bug *> i(bugList[hash]);
            while(i.hasNext()) {
                bug = i.next();
                if(!bug->get_marked()) {
                    e = bug->get_energy();
                    dir = bug->get_dir();
                    if(dir == 4) {
                        move_cost = world_params.stay_energy;
                        nx = x; ny = y;
                    } else {
                        if(world_params.kill_on_edge) {
                            nx = X(dir, x);
                            ny = Y(dir, y);
                        } else {
                            nx = WRAP_X(dir, x);
                            ny = WRAP_Y(dir, y);
                        }
                        // falling cost is like staying still, climbing could cost you your life
                        if(nx >= 0 && nx < width && ny >= 0 && ny < height) {
                            int climb = terrain[nx * height + ny] - terrain[x * height + y];
                            if(climb < 0) move_cost = world_params.stay_energy;
                            else move_cost = world_params.move_energy * (climb + 1);
                            //  dir = 4;
                            //  nx = x; ny = y; move_cost = world_params.stay_energy;
                        } else
                            move_cost = world_params.move_energy;

                        move_cost *= bug->get_payload() + 1;
                    }

                    if(bug->get_age() >= world_params.max_age
                            || e - move_cost < world_params.min_energy
                            || nx < 0 || nx >= width || ny < 0 || ny >= height || poison[nx * height + ny])
                    {
                        //qDebug() << "** killing old bug";
                        i.remove();
                        size--;
                        if(size == 0) k.remove();

                        removeBug(bug, e, x, y);
                        bug = 0;
                    } else {
                        //dataMutex.lock();
                        bugEnergy -= move_cost;
                        if(bug->get_age() > max_age) max_age = bug->get_age();
                        if(bug->get_generation() > max_gen) max_gen = bug->get_generation();
                        //dataMutex.unlock();

                        bug->dec_energy(move_cost);
                        if(dir==4) { // staying still
                            //qDebug() << "** bug stays";
                            bugTotal[hash] -= move_cost;
                            //check_integrity();
                        } else {
                            bugTotal[hash] -= e;

                            i.remove();
                            size--;
                            if(size == 0) k.remove();

                            int nhash = nx * height + ny;
                            newPos.insert(nhash);
                            bugList[nhash].append(bug);
                            bugTotal[nhash] += (e - move_cost);

                            //update_pixel(nx, ny);
                        }

                        bug->mark();
                        /*
                        if(bug->get_dna().getData().size() > showByte) {
                            val = (unsigned char)bug->get_dna().getData().at(showByte);
                            //dataMutex.lock();
                            histogram[val] = histogram[val] + 1;
                            //dataMutex.unlock();
                        }
                        */
                    }
                }
            }
            update_pixel(hash);
        }

        QSetIterator<int> m(newPos);
        while(m.hasNext()) {
            hash = m.next();
            if(!listHash.contains(hash)) {
                listHash.append(hash);
                update_pixel(hash);
            }
        }

        // have babies
        //qDebug() << "have babies";
        //check_integrity();
        int i1, i2, r;
        Bug *mum, *dad;
        //for(int x = 0; x < width; x++) {
        //for(int y = 0; y < height; y++) {
        //hash =x * height + y;
        QListIterator<int> l(listHash);
        while(l.hasNext()) {
            hash = l.next();
            x = hash / height;
            y = hash % height;
            size = bugList[hash].size();
            if(size == 0) qDebug() << "0!!!(3)";

            if(size > 1) {
                QListIterator<Bug *> i(bugList[hash]);
                i1 = 0;
                while(i.hasNext()) {
                    mum = i.next();

                    if(mum->get_energy() >= world_params.child_energy + world_params.min_energy) {
                        r = qrand() % 255 + 1;
                        if(mum->get_split() < r) {
                            //if(mum->get_split() > 0) {
                            i2 = qrand() % (size - 1);
                            if(i2 >= i1) i2++;
                            //if(i1 == i2) i2 = (i2 + 1) % size;
                            //i2 = (i1 + 1) % size;
                            dad = bugList[hash].at(i2);

                            if(dad->get_split() < r) {
                                bug = new Bug(mum, dad, world_params.mutation, world_params.max_data, world_params.min_data, world_params.steps_per_update, world_params.stack_size);

                                bug->set_energy(world_params.child_energy);
                                mum->dec_energy(world_params.child_energy);

                                bugList[hash].append(bug);

                                population++;

                                //update_pixel(x, y);

                                if(bug->get_generation() > max_gen) max_gen = bug->get_generation();
                            }
                        }
                    }
                    i1++;
                }
            }
            /* just create copies - no combination
            Bug *parent;
            if(size > 0) {
                QListIterator<Bug *> i(bugList[hash]);
                int r;
                while(i.hasNext()) {
                    parent = i.next();
                    r = qrand() % 255 + 1;
                    if(parent->get_split() > r && parent->get_energy() > world_params.min_energy + world_params.child_energy) {
                        bug = new Bug(parent, world_params.mutation, world_params.max_data, world_params.min_data, world_params.steps_per_update, world_params.stack_size);

      if(bug->get_dna().getData().size() > showByte) {
       val = (unsigned char)bug->get_dna().getData().at(showByte);
       histogram[val] = histogram[val] + 1;
      }

      bug->set_energy(world_params.child_energy);
      parent->dec_energy(world_params.child_energy);

      bugList[hash].append(bug);

      population++;

      update_pixel(x, y);

      if(bug->get_generation() > max_gen) max_gen = bug->get_generation();
     }
    }
   }
   */
        }

        balance();

        //qDebug() << "step end";
        //check_integrity();

        //taskQueue.waitCompleted();

        time++;
    }

    emit changed();
    //emit histData(histogram, population);

    if(calc_histogram) {
        emit dnaData(get_dna_data(), population);
    }
}

Bug *PetriDish::get_closest_bug(int x, int y) {
    QMutexLocker l(&dataMutex);
    int hash, hx, hy, chash = -1;
    float dist, mindist = 10;
    QListIterator<int> i(listHash);
    while(i.hasNext()) {
        hash = i.next();
        hx = hash / height;
        hy = hash % height;
        dist = sqrt((float)(x - hx) * (x - hx) + (y - hy) * (y - hy));
        if(dist < mindist) {
            mindist = dist;
            chash = hash;
        }
    }

    if(chash != -1 && bugList[chash].size())
        return bugList[chash].first();

    return 0;
}

void PetriDish::check_integrity() {
    //QMutexLocker locker(&dataMutex);
    unsigned int count = 0, count_e = 0;
    int hash;
    Bug *bug;
    int totalE = 0, totalEE = 0;
    for(int x = 0; x  < width; x++) {
        for(int y = 0; y < height; y++) {
            hash = x * height + y;
            if(bugList[hash].size()) {
                if(!listHash.contains(hash)) qDebug() << "unlisted occupied position at" << x << "," << y;
                QListIterator<Bug *> i(bugList[hash]);
                while(i.hasNext()) {
                    bug = i.next();
                    count++;
                    totalE += bug->get_energy();
                    if(bug->get_energy() > world_params.max_energy) qDebug() << "bug with too much energy!" << bug->get_energy() << ">" << world_params.max_energy;
                    if(bug->get_energy() < world_params.min_energy) qDebug() << "bug with too little energy!";
                }
            }
            if(energy[hash]) {
                count_e++;
                totalEE += energy[hash];
                if(energy[hash] > world_params.max_e_energy) qDebug() << "node with too much energy!";
                if(energy[hash] < world_params.min_e_energy) qDebug() << "node with too little energy!";
            }
        }
    }
    if(count != population) qDebug() << "pop:" << population << "count:" << count;
    if(bugEnergy != totalE) qDebug() << "bugEnergy:" << bugEnergy << "totalE:" << totalE;
    if(energyEnergy != totalEE) qDebug() << "energyEnergy:" << energyEnergy << "totalEE:" << totalEE;
}

void PetriDish::enable_histogram_calc(bool enable) {
    calc_histogram = enable;
}

QVector<QByteArray> PetriDish::get_dna_data() {
    QVector<QByteArray> dat;

    int hash;
    QMutexLocker locker(&dataMutex);
    QListIterator<int> i(listHash);
    while(i.hasNext()) {
        hash = i.next();
        foreach(Bug *b, bugList[hash]) {
            dat.append(b->get_dna().getData());
        }
    }
    return dat;
}
