#pragma once

#include <QObject>
#include <QList>
#include <QPair>

#include "ialgorithm.h"

typedef QList<QList<int>> Population;

/**
 *  Parallel (on CPU) GA built in master-slave model
 *  Used modified version - PREGA (https://www.hindawi.com/journals/tswj/2014/178621/)
 *  Crossovers: PMX, POS, OX2
 *  Mutations: swap genes(elements in array), take random sub tour and place it in random place
*/


namespace ga {
    ///
    /// \brief takeOne - takes one individual from population by roulette selection
    /// \param population - list of individuals
    /// \param fitness - list of calculated fitness
    /// \return returns taken individual
    ///
    const QList<int> takeOne(const Population& population, const QList<double>& fitness);

    ///
    /// \brief calculateFitness - calculates value = 1 / distance
    /// \param matrix - graph matrix
    /// \param individual - path
    /// \return returns calculated value
    ///
    double calculateFitness(const GraphMatrix& matrix, const QList<int>& individual);

    double calculateDistance(const GraphMatrix& matrix, const QList<int>& individual);

    ///
    /// \brief normalizeFitness
    /// \param fitness
    ///
    void normalizeFitness(QList<double>& fitness);

    ///
    /// \brief pmx crossover function
    /// \param p1 - parent 1
    /// \param p2 - parent 2
    /// \return returns children
    ///
    QList<QList<int>> pmx(const QList<int>& p1, const QList<int>& p2);

    ///
    /// \brief pos - position based crossover
    /// \param p1 - parent 1
    /// \param p2 - parent 2
    /// \return returns children
    ///
    QList<QList<int>> pos(QList<int>& p1, QList<int>& p2);

    ///
    /// \brief ox2 - order based crossover
    /// \param p1
    /// \param p2
    /// \return
    ///
    QList<QList<int>> ox2(QList<int>& p1, QList<int>& p2);

    ///
    /// \brief swapMutation
    /// \param individual
    ///
    void swapMutation(QList<int>& individual);

    /// mapping for pmx crossover
    class MappingList {
        QList<QPair<int, int>> m_Mappings;
    public:
        MappingList() {}
        void appendMapping(QPair<int, int> mapping) {
            m_Mappings.append(std::move(mapping));
        }

        QList<QPair<int, int>> getMappings() const noexcept {
            return m_Mappings;
        }

        int mapFirst(int gen) {
            auto hasPair = [&](int first) -> std::optional<int> {
                for(const auto& m : m_Mappings) {
                    if(m.first == first) {
                        return m.second;
                    }
                }
                return std::nullopt;
            };
            int result = gen;
            std::optional<int> pair = hasPair(gen);
            while(pair) {
                result = *pair;
                pair = hasPair(result);
            }
            return result;
        }

        int mapSecond(int gen) {
            auto hasPair = [&](int second) -> std::optional<int> {
                for(const auto& m : m_Mappings) {
                    if(m.second == second) {
                        return m.first;
                    }
                }
                return std::nullopt;
            };
            int result = gen;
            std::optional<int> pair = hasPair(gen);
            while(pair) {
                result = *pair;
                pair = hasPair(result);
            }
            return result;
        }
    };
}

class GeneticAlgorithm : public IAlgorithm {
    Q_OBJECT

    Q_PROPERTY(int populationSize READ getPopulationSize WRITE setPopulationSize NOTIFY populationSizeChanged)
    Q_PROPERTY(int generations READ getGenerations WRITE setGenerations NOTIFY generationsChanged)
public:
    GeneticAlgorithm(QObject *parent = nullptr) : IAlgorithm(parent), m_Generations(20), m_PopulationSize(10) {}
    Q_INVOKABLE virtual void start(const GraphMatrix& matrix) override;

    int getPopulationSize() const noexcept;
    void setPopulationSize(int populationSize) noexcept;

    int getGenerations() const noexcept;
    void setGenerations(int generations) noexcept;
signals:
    void finished(double optimalPath, QList<int> path);
    void populationSizeChanged();
    void generationsChanged();
private:
    void findBestPath();
    void calculateFitness();
private:
    GraphMatrix m_Matrix;
    Population m_CurrentPopulation;
    QList<double> m_CurrentFitness;
    double m_BestSoFar;
    int m_PopulationSize, m_Generations;
    QList<int> m_BestSoFarPath;
};
