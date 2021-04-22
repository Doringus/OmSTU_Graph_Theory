#include "ga.h"

#include <QDebug>
#include <QSet>
#include <QRandomGenerator>

#include <random>


void GeneticAlgorithm::start(const GraphMatrix& matrix) {
    if(matrix.isEmpty()) {
        return;
    }
    m_BestSoFar = -1;
    m_Matrix = matrix;
    m_CurrentFitness.clear();
    m_CurrentPopulation.clear();
    /// Generate initial population
    QList<int> tmp;
    tmp.reserve(matrix.count());
    for(int i = 0; i < matrix.count(); ++i) {
        tmp.append(i);
    }

    std::random_device rd;
    std::mt19937 g(rd());
    for(int i = 0; i < 20; ++i) {
        std::shuffle(tmp.begin(), tmp.end(), g);
        m_CurrentPopulation.append(tmp);
    }
    calculateFitness();
    findBestPath();
    /// Begin interations
    // number of generations

    std::uniform_real_distribution<double> d(0.0, 1.0);
    for(int i = 0; i < 60; ++i) {
        Population nextGeneration;
        nextGeneration.reserve(m_CurrentPopulation.count());
        while (nextGeneration.count() < m_CurrentPopulation.count()) {
            QList<int> parentA = ga::takeOne(m_CurrentPopulation, m_CurrentFitness);
            QList<int> parentB = ga::takeOne(m_CurrentPopulation, m_CurrentFitness);
            while(parentA == parentB) {
                parentB = ga::takeOne(m_CurrentPopulation, m_CurrentFitness);
            }

            QList<QList<int>> children = ga::pmx(parentA, parentB);

            double mutationChance = d(*QRandomGenerator::global());
            if(mutationChance <= 0.2) {
                ga::swapMutation(children.first());
            }
            mutationChance = d(*QRandomGenerator::global());
            if(mutationChance <= 0.2) {
                ga::swapMutation(children.last());
            }

            nextGeneration.append(children);
        }
        qDebug() << m_BestSoFar;
        m_CurrentPopulation = nextGeneration;
        calculateFitness();
        findBestPath();
    }
    double result = ga::calculateDistance(m_Matrix, m_BestSoFarPath);
    emit finished(result, m_BestSoFarPath);
}

void GeneticAlgorithm::findBestPath() {
    auto bestFitness = std::max_element(m_CurrentFitness.begin(), m_CurrentFitness.end());
    double best = *bestFitness;
    if(best > m_BestSoFar) {
        m_BestSoFar = best;
        m_BestSoFarPath = m_CurrentPopulation.at(std::distance(m_CurrentFitness.begin(), bestFitness));
    }
}

void GeneticAlgorithm::calculateFitness() {
    m_CurrentFitness.clear();
    for(const auto& individual : m_CurrentPopulation) {
        m_CurrentFitness.append(ga::calculateFitness(m_Matrix, individual));
    }
    ga::normalizeFitness(m_CurrentFitness);
}

const QList<int> ga::takeOne(const Population& population, const QList<double>& fitness) {
    std::uniform_real_distribution<double> d(0.0, 1.0);
    double r = d(*QRandomGenerator::global());
    int i = 0;
    while (r > 0 && i < population.count()) {
        r -= fitness[i];
        ++i;
    }
    i--;
    return population.at(i);
}


double ga::calculateFitness(const GraphMatrix& matrix, const QList<int>& individual) {
    return 1.0 / (calculateDistance(matrix, individual) + 1);
}

double ga::calculateDistance(const GraphMatrix& matrix, const QList<int>& individual) {
    double result = 0;
    for(int i = 0; i < individual.count() - 1; ++i) {
        result += matrix[individual.at(i)][individual.at(i + 1)];
    }
    result += matrix[individual.last()][individual.first()];
    return result;
}

void ga::normalizeFitness(QList<double>& fitness) {
    double sum = 0;
    for(const auto& f : fitness) {
        sum += f;
    }
    std::for_each(fitness.begin(), fitness.end(), [&sum](double& value) {
        value /= sum;
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

    return QList<QList<int>> {child1, child2};
}


QList<QList<int>> ga::pos(const QList<int> &p1, const QList<int> &p2) {
    std::uniform_int_distribution<int> d(0, p1.count() / 2);
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



    return QList<QList<int>>();
}

void ga::swapMutation(QList<int>& individual) {
    int first = QRandomGenerator::global()->bounded(0, individual.count() - 1);
    int second = QRandomGenerator::global()->bounded(0, individual.count() - 1);
    while(first == second) {
        second = QRandomGenerator::global()->bounded(0, individual.count() - 1);
    }
    individual.swapItemsAt(first, second);
}



