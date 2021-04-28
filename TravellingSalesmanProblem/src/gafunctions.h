#pragma once

#include <QList>
#include "common.h"

namespace ga {

    struct individual_t {
        double fitness;
        double pathLength;
        QList<int> path;
        bool operator==(const individual_t& rhs) const {
            return path == rhs.path;
        }
    };

    ///
    /// \brief takeOne - takes one individual from population by roulette selection
    /// \param population - list of individuals
    /// \param fitness - list of calculated fitness
    /// \return returns taken individual
    ///
    individual_t takeOne(QList<individual_t>& population);

    ///
    /// \brief calculateFitness - calculates value = 1 / distance
    /// \param matrix - graph matrix
    /// \param individual - path
    /// \return returns calculated value
    ///
    void calculateFitness(const GraphMatrix& matrix, individual_t& individual);

    double calculateDistance(const GraphMatrix& matrix, const QList<int>& individual);

    ///
    /// \brief normalizeFitness
    /// \param fitness
    ///
    void normalizeFitness(QList<individual_t>& population);

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
    /// \brief swapMutation - swaps two random elements in array
    /// \param individual
    ///
    void swapMutation(QList<int>& individual);

    ///
    /// \brief inversionMutation
    /// \param individual
    ///
    void inversionMutation(QList<int>& individual);

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
