#pragma once

#include <QObject>
#include <QList>
#include <QPair>

#include "ialgorithm.h"
#include "gaexecutor.h"
#include "gafunctions.h"

typedef QList<QList<int>> Population;

/**
 *  Parallel (on CPU) GA built in master-slave model
 *  Crossovers: PMX, POS, OX2
 *  Mutations: swap, inversion
*/

class Crossover : public QObject {
    Q_OBJECT
public:
    enum class Enum {
        PMX = 0,
        POS = 1,
        OX2 = 2
    };
    Q_ENUM(Enum)
};

class Mutation : public QObject {
    Q_OBJECT
public:
    enum class Enum {
        SWAP = 0,
        INVERSION = 1
    };
    Q_ENUM(Enum)
};


class GATask : public Task {
    Q_OBJECT
public:
    GATask(QObject *parent, const ga::individual_t& p1, const ga::individual_t& p2, const GraphMatrix& matrix,
           Crossover::Enum crossoverType, Mutation::Enum mutationType);

    virtual void run() override;
signals:
    void finished(QList<ga::individual_t> children);
private:
    ga::individual_t m_Parent1, m_Parent2;
    const GraphMatrix& m_Matrix;
    Crossover::Enum m_CrossoverType;
    Mutation::Enum m_MutationType;
};



class GeneticAlgorithm : public IAlgorithm {
    Q_OBJECT

    Q_PROPERTY(int populationSize READ getPopulationSize WRITE setPopulationSize NOTIFY populationSizeChanged)
    Q_PROPERTY(int generations READ getGenerations WRITE setGenerations NOTIFY generationsChanged)
    Q_PROPERTY(int crossoverType READ getCrossoverType WRITE setCrossoverType NOTIFY crossoverTypeChanged)
    Q_PROPERTY(int mutationType READ getMutationType WRITE setMutationType NOTIFY mutationTypeChanged)
public:
    GeneticAlgorithm(QObject *parent = nullptr) : IAlgorithm(parent), m_Generations(20), m_PopulationSize(10), m_CurrentGeneration(0) {
        m_Executor.setThreadPool(new StaticThreadPool(this));
        connect(&m_Executor, &GAExecutor::taskFinished, this, &GeneticAlgorithm::onGATaskFinished);
        m_Crossover = Crossover::Enum::PMX;
        m_Mutation = Mutation::Enum::SWAP;

    }
    Q_INVOKABLE virtual void start(const GraphMatrix& matrix) override;

    int getPopulationSize() const noexcept;
    void setPopulationSize(int populationSize) noexcept;

    int getGenerations() const noexcept;
    void setGenerations(int generations) noexcept;

    int getCrossoverType() const noexcept;
    void setCrossoverType(int type) noexcept;

    int getMutationType() const noexcept;
    void setMutationType(int type) noexcept;

    void iterate();

signals:
    void finished(double optimalPath, QList<int> path);
    void pathReady(double distance); // for profiler
    void populationSizeChanged();
    void generationsChanged();
    void crossoverTypeChanged();
    void mutationTypeChanged();
private:
    void findBestPath();
private slots:
    void onGATaskFinished(QList<ga::individual_t> children);
private:
    GraphMatrix m_Matrix;
    QList<ga::individual_t> m_CurrentPopulation;
    ga::individual_t m_BestSoFar;
    int m_PopulationSize, m_Generations, m_CurrentGeneration;
    QList<int> m_BestSoFarPath;
    GAExecutor m_Executor;
    Crossover::Enum m_Crossover;
    Mutation::Enum m_Mutation;
};

class GACudaWrapper : public IAlgorithm {
    Q_OBJECT
public:
    GACudaWrapper(QObject* parent = nullptr) : IAlgorithm(parent) {}
    Q_INVOKABLE virtual void start(const GraphMatrix& matrix) override;
signals:
    void finished(double optimalPath, QList<int> path);
    void pathReady(double distance); // for profiler
};
