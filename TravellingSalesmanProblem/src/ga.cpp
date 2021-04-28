#include "ga.h"

#include <QDebug>
#include <QSet>
#include <QRandomGenerator>

#include <random>


void GeneticAlgorithm::start(const GraphMatrix& matrix) {
    m_CurrentGeneration = 0;
    if(matrix.isEmpty()) {
        return;
    }
    m_Matrix = matrix;
    m_CurrentPopulation.clear();
    m_BestSoFar = {-1.0, 0.0, {}};
    /// Generate initial population
    QList<int> tmp;
    tmp.reserve(matrix.count());
    for(int i = 0; i < matrix.count(); ++i) {
        tmp.append(i);
    }

    std::random_device rd;
    std::mt19937 g(rd());
    for(int i = 0; i < m_PopulationSize; ++i) {
        std::shuffle(tmp.begin(), tmp.end(), g);
        ga::individual_t individual {0.0, 0.0, tmp};
        ga::calculateFitness(m_Matrix, individual);
        m_CurrentPopulation.append(individual);
    }
    findBestPath();
    /// Begin interations
    iterate();
}

int GeneticAlgorithm::getPopulationSize() const noexcept {
    return m_PopulationSize;
}

void GeneticAlgorithm::setPopulationSize(int populationSize) noexcept {
    m_PopulationSize = populationSize;
    emit populationSizeChanged();
}

int GeneticAlgorithm::getGenerations() const noexcept {
    return m_Generations;
}

void GeneticAlgorithm::setGenerations(int generations) noexcept {
    m_Generations = generations;
    emit generationsChanged();
}

int GeneticAlgorithm::getCrossoverType() const noexcept {
    return static_cast<int>(m_Crossover);
}

void GeneticAlgorithm::setCrossoverType(int type) noexcept {
    m_Crossover = static_cast<Crossover::Enum>(type);
    emit crossoverTypeChanged();
}

int GeneticAlgorithm::getMutationType() const noexcept {
    return static_cast<int>(m_Mutation);
}

void GeneticAlgorithm::setMutationType(int type) noexcept {
    m_Mutation = static_cast<Mutation::Enum>(type);
    emit mutationTypeChanged();
}

void GeneticAlgorithm::iterate() {
    if(m_CurrentGeneration < m_Generations) {
        ga::normalizeFitness(m_CurrentPopulation);
        QList<QPair<ga::individual_t, ga::individual_t>> parentsPairs;
        while(parentsPairs.count() * 2 < m_CurrentPopulation.count()) {
            ga::individual_t parentA = ga::takeOne(m_CurrentPopulation);
            ga::individual_t parentB = ga::takeOne(m_CurrentPopulation);
            while(parentA == parentB) {
                parentB = ga::takeOne(m_CurrentPopulation);
            }
            parentsPairs.append({parentA, parentB});
        }
        m_CurrentPopulation.clear();
        m_CurrentPopulation.squeeze();
        for(auto p : parentsPairs) {
            GATask *task = new GATask(nullptr, p.first, p.second, m_Matrix, m_Crossover, m_Mutation);
            task->moveToThread(nullptr);
            m_Executor.putTask(task);
        }
        m_CurrentGeneration++;
    } else {
        emit finished(m_BestSoFar.pathLength, m_BestSoFar.path);
    }
}

void GeneticAlgorithm::findBestPath() {
    auto bestFitness = std::max_element(m_CurrentPopulation.begin(), m_CurrentPopulation.end(), [](const ga::individual_t& lhs, const ga::individual_t& rhs){
        return lhs.fitness < rhs.fitness;
    });
    double best = (*bestFitness).fitness;
    if(best > m_BestSoFar.fitness) {
        m_BestSoFar = m_CurrentPopulation.at(std::distance(m_CurrentPopulation.begin(), bestFitness));;
    }
}


void GeneticAlgorithm::onGATaskFinished(QList<ga::individual_t> children) {
    m_CurrentPopulation.append(children);
    if(m_CurrentPopulation.count() == m_PopulationSize) {
        findBestPath();
        iterate();
    }
}


GATask::GATask(QObject *parent, const ga::individual_t &p1, const ga::individual_t &p2, const GraphMatrix &matrix,
               Crossover::Enum crossoverType, Mutation::Enum mutationType) : Task(parent),
                 m_Parent1(p1), m_Parent2(p2), m_Matrix(matrix), m_CrossoverType(crossoverType), m_MutationType(mutationType) {

}

void GATask::run() {
    QList<QList<int>> children;
    switch (m_CrossoverType) {
        case Crossover::Enum::PMX: {
            children = ga::pmx(m_Parent1.path, m_Parent2.path);
            break;
        }
        case Crossover::Enum::POS: {
            children = ga::pos(m_Parent1.path, m_Parent2.path);
            break;
        }
        case Crossover::Enum::OX2: {
            children = ga::ox2(m_Parent1.path, m_Parent2.path);
            break;
        }
    }
    std::uniform_real_distribution<double> d(0.0, 1.0);
    double mutationChance = d(*QRandomGenerator::global());
    if(mutationChance <= 0.2) {
        switch (m_MutationType) {
            case Mutation::Enum::SWAP: {
                ga::swapMutation(children.first());
                break;
            }
            case Mutation::Enum::INVERSION: {
                ga::inversionMutation(children.first());
                break;
            }
        }

    }
    mutationChance = d(*QRandomGenerator::global());
    if(mutationChance <= 0.2) {
        switch (m_MutationType) {
            case Mutation::Enum::SWAP: {
                ga::swapMutation(children.last());
                break;
            }
            case Mutation::Enum::INVERSION: {
                ga::inversionMutation(children.last());
                break;
            }
        }
    }
    ga::individual_t first = {0.0, 0.0, children.first()};
    ga::individual_t second = {0.0, 0.0, children.last()};
    ga::calculateFitness(m_Matrix, first);
    ga::calculateFitness(m_Matrix, second);
    emit finished({first, second});
}
