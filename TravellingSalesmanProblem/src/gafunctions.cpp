#include "gafunctions.h"

#include <QRandomGenerator>
#include <QSet>
#include <QDebug>

#include <random>

ga::individual_t ga::takeOne(QList<individual_t>& population) {
    std::uniform_real_distribution<double> d(0.0, 1.0);
    double r = d(*QRandomGenerator::global());
    double sum = 0;
    for(auto i : population) {
        sum += i.fitness;
        if(r < sum) {
            return i;
        }
    }
}


void ga::calculateFitness(const GraphMatrix& matrix, individual_t& individual) {
    individual.pathLength = calculateDistance(matrix, individual.path);
    individual.fitness =  1.0 / (individual.pathLength);
}

double ga::calculateDistance(const GraphMatrix& matrix, const QList<int>& individual) {
    double result = 0;
    for(int i = 0; i < individual.count() - 1; ++i) {
        result += matrix[individual.at(i)][individual.at(i + 1)];
    }
    result += matrix[individual.last()][individual.first()];
    return result;
}

void ga::normalizeFitness(QList<ga::individual_t>& population) {
    double sum = 0;
    for(const auto& p : population) {
        sum += p.fitness;
    }
    std::for_each(population.begin(), population.end(), [&sum](individual_t& value) {
        value.fitness /= sum;
    });
}

QList<QList<int>> ga::pmx(const QList<int>& p1, const QList<int>& p2) {
    std::uniform_int_distribution<int> d(0, p1.count() - 1);
    int position = d(*QRandomGenerator::global());
    std::uniform_int_distribution<int> countD(1, p1.count() - position);
    int count = countD(*QRandomGenerator::global());
    QList<int> child1(p1.count());
    QList<int> child2(p2.count());
    QList<int> tmpSlice1(count), tmpSlice2(count);
    std::copy(p1.begin() + position, p1.begin() + position + count, child2.begin() + position);
    std::copy(p2.begin() + position, p2.begin() + position + count, child1.begin() + position);
    std::copy(p1.begin() + position, p1.begin() + position + count, tmpSlice1.begin());
    std::copy(p2.begin() + position, p2.begin() + position + count, tmpSlice2.begin());

    /// create mapping
    MappingList mappings;
    // warning!! not cache friendly
    for(int i = position; i < position + count; ++i) {
        mappings.appendMapping({p1.at(i), p2.at(i)});
    }

    for(int i = 0; i < p1.count(); ++i) {
        if(i == position) {
            i = position + count;
            if(i >= p1.count()) {
                break;
            }
        }
        int gen = p1.at(i);
        if(tmpSlice2.contains(gen)) {
            child1[i] = mappings.mapSecond(gen);
        } else {
            child1[i] = gen;
        }
    }

    for(int i = 0; i < p2.count(); ++i) {
        if(i == position) {
            i = position + count;
            if(i >= p2.count()) {
                break;
            }
        }
        int gen = p2.at(i);
        if(tmpSlice1.contains(gen)) {
            child2[i] = mappings.mapFirst(gen);
        } else {
            child2[i] = gen;
        }
    }

    return {child1, child2};
}


QList<QList<int>> ga::pos(QList<int>& p1, QList<int>& p2) {
    std::uniform_int_distribution<int> d(1, p1.count() / 2);
    std::uniform_int_distribution<int> positionsDist(0, p1.count() - 1);
    QList<int> child1(p1.count());
    QList<int> child2(p2.count());
    child1.fill(-1);
    child2.fill(-1);
    int count = d(*QRandomGenerator::global());
    QSet<int> positions;
    for(int i = 0; i < count; ++i) {
        positions.insert(positionsDist(*QRandomGenerator::global()));
    }

    // Fill children
    for(const auto& p : positions) {
        child1[p] = p2.at(p);
    }
    for(const auto& p : positions) {
        child2[p] = p1.at(p);
    }
    // Fill children
    int j = 0;
    for(int i = 0; i < p1.count(); ++i) {
        if(child1.contains(p1.at(i))) {
            continue;
        }
        for(;j < child1.count(); ++j) {
            if(child1.at(j) == -1) {
                child1[j] = p1.at(i);
                j++;
                break;
            }
        }
    }

    j = 0;
    for(int i = 0; i < p1.count(); ++i) {
        if(child2.contains(p2.at(i))) {
            continue;
        }
        for(;j < child2.count(); ++j) {
            if(child2.at(j) == -1) {
                child2[j] = p2.at(i);
                j++;
                break;
            }
        }
    }

    return {child1, child2};
}

QList<QList<int>> ga::ox2(QList<int>& p1, QList<int>& p2) {
    std::uniform_int_distribution<int> countDist(1, p1.count() / 2);
    std::uniform_int_distribution<int> positionsDist(0, p1.count() - 1);
    QList<int> child1(p1.count());
    QList<int> child2(p2.count());

    int count = countDist(*QRandomGenerator::global());
    QSet<int> positions;
    for(int i = 0; i < count; ++i) {
        positions.insert(positionsDist(*QRandomGenerator::global()));
    }
    QList<int> p2Slice;
    for(const auto& p : positions) {
        p2Slice.append(p2.at(p));
    }
    QList<int> p1Slice;
    for(const auto& p : positions) {
        p1Slice.append(p1.at(p));
    }
    // Create childrens
    int sliceIndex = 0;
    for(int i = 0; i < p1.count(); ++i) {
        if(p2Slice.contains(p1.at(i))) {
            child1[i] = p2Slice.at(sliceIndex);
            sliceIndex++;
        } else {
            child1[i] = p1[i];
        }
    }
    sliceIndex = 0;
    for(int i = 0; i < p2.count(); ++i) {
        if(p1Slice.contains(p2.at(i))) {
            child2[i] = p1Slice.at(sliceIndex);
            sliceIndex++;
        } else {
            child2[i] = p2[i];
        }
    }
    return {child1, child2};
}

void ga::swapMutation(QList<int>& individual) {
    int first = QRandomGenerator::global()->bounded(0, individual.count() - 1);
    int second = QRandomGenerator::global()->bounded(0, individual.count() - 1);
    while(first == second) {
        second = QRandomGenerator::global()->bounded(0, individual.count() - 1);
    }
    individual.swapItemsAt(first, second);
}


void ga::inversionMutation(QList<int>& individual) {
    std::uniform_int_distribution<int> d(0, individual.count() - 1);
    int position = d(*QRandomGenerator::global());
    std::uniform_int_distribution<int> countD(1,individual.count() - position);
    int count = countD(*QRandomGenerator::global());
    std::reverse(individual.begin() + position, individual.begin() + position + count);
}
