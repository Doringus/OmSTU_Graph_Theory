#include <iostream>
#include <stdio.h>

#include <cuda.h>
#include <curand.h>
#include <curand_kernel.h>
#include <thrust/host_vector.h>
#include <thrust/device_vector.h>
#include <thrust/tuple.h>
#include <thrust/generate.h>
#include <thrust/random.h>
#include <thrust/sort.h>
#include <thrust/copy.h>

#define gpuErrchk(ans) { gpuAssert((ans), __FILE__, __LINE__); }
inline void gpuAssert(cudaError_t code, const char *file, int line, bool abort=true)
{
   if (code != cudaSuccess)
   {
      fprintf(stderr,"GPUassert: %s %s %d\n", cudaGetErrorString(code), file, line);
      if (abort) exit(code);
   }
}

////////// Global variables section //////////

__constant__ int BLOCK_COUNT;
__constant__ int THREADS_PER_BLOCK;


////////// End section //////////

////////// Device functions section //////////

__device__
int getThreadId() {
    int blockId  = blockIdx.y * gridDim.x + blockIdx.x;
    int threadId = blockId * blockDim.x + threadIdx.x;
    return threadId;
}

__device__
int getBlockId() {
    return blockIdx.y * gridDim.x + blockIdx.x;
}

__device__
int get1dIndex(int row, int col, int width) {
    return width * row + col;
}

__device__
int takeParent(double* fitness, int startIndex, double prob) {
    int i;
    double sum = 0;
    for(i = startIndex; i < startIndex + THREADS_PER_BLOCK; ++i) {
        sum += fitness[i];
        if(prob < sum) {
            return i;
        }
    }
    return --i;
}

////////// End section //////////




////////// Kernels section //////////

__global__
void initRandomKernel(unsigned int seed, curandState_t* states) {
    int id = getThreadId();
    curand_init(seed, id, 0, &states[id]);
}

__global__
void uniformRandomSelectionKernel(curandState_t* states, double* selectionRandomArray) {
    selectionRandomArray[getThreadId()] = curand_uniform(&states[getThreadId()]);
}

__global__
void crossoverRandomPointsKernel(curandState_t* states, int* crossoverPointsArray, int max) {
    crossoverPointsArray[getThreadId()] = curand(&states[getThreadId()]) % (max - 1);
}

///
/// \brief fitnessKernel calculates fit for each individual (thread per individual)
/// \param matrix - graph matrix
/// \param population
/// \param fitness - array for result of calculation
/// \param populationSize
/// \param matrixSize - number of rows(or columns) in matrix
///
__global__
void fitnessKernel(int* matrix, int* population, double* fitness, int populationSize, int matrixSize) {
    double fit = 0;
    int startIndex = getThreadId() * matrixSize;
    for(int i = startIndex; i < startIndex + matrixSize - 1; ++i) {
        fit += matrix[get1dIndex(population[i], population[i + 1], matrixSize)];
    }
    fit += matrix[get1dIndex(population[startIndex + matrixSize - 1], population[startIndex], matrixSize)];
    fit = 1.0 / fit;
    fitness[getThreadId()] = fit;
    __syncthreads();
    // normilize fitness to roulette selection
    // calculate sum of fitness in block
    double sum = 0;
    startIndex = getBlockId() * THREADS_PER_BLOCK;
    for(int i = startIndex; i < startIndex + THREADS_PER_BLOCK; ++i) {
        sum += fitness[i];
    }
    fit = fit / sum;

//    printf("%f %d\n", fit, getThreadId());
    __syncthreads();
    fitness[getThreadId()] = fit;
}

///
/// \brief breedKernel makes roulette selection, and perfroms OX crossover. Thread per two individuals
/// \param population
/// \param nextGeneration
/// \param fitness
/// \param probability - array with random numbers for roulette selection
/// \param crossoverPoints - array with random numbers for crossover
/// \param size - genes count in individual
///
__global__
void breedKernel(int* population, int* nextGeneration, double* fitness, double* probability, int* crossoverPoints, int size) {
    // thread runs roulette twice and finds two parents and starts crossover

    int startIndex = getBlockId() * THREADS_PER_BLOCK;
    int parent1Index = takeParent(fitness, startIndex, probability[getThreadId() * 2]);
    int parent2Index = takeParent(fitness, startIndex, probability[getThreadId() * 2 + 1]);
    // start ox crossover
    int firstPoint = crossoverPoints[getThreadId() * 2];
    int secondPoint = crossoverPoints[getThreadId() * 2 + 1];
    if(firstPoint > secondPoint) {
        int tmp = firstPoint;
        firstPoint = secondPoint;
        secondPoint = tmp;
    }
    for(int i = firstPoint; i < secondPoint; ++i) {
        // child 1
        nextGeneration[get1dIndex(getThreadId() * 2, i, size)] = population[get1dIndex(parent1Index, i, size)];
        // child 2
        nextGeneration[get1dIndex(getThreadId() * 2 + 1, i, size)] = population[get1dIndex(parent2Index, i, size)];
    }
    int placeIndex1 = secondPoint;
    int placeIndex2 = secondPoint;
    for(int i = 0; i < size; ++i) {
        int index = (secondPoint + i) % size;
        int parent1Gene = population[get1dIndex(parent1Index, index, size)];
        int parent2Gene = population[get1dIndex(parent2Index, index, size)];
        bool isInSlice = false;
        for(int j = firstPoint; j < secondPoint; ++j) {
            if(population[get1dIndex(parent1Index, j, size)] == parent2Gene) {
                isInSlice = true;
                break;
            }
        }
        if(!isInSlice) {
            nextGeneration[get1dIndex(getThreadId() * 2, placeIndex1 % size, size)] = parent2Gene;
            placeIndex1++;
        }
        isInSlice = false;
        for(int j = firstPoint; j < secondPoint; ++j) {
            if(population[get1dIndex(parent2Index, j, size)] == parent1Gene) {
                isInSlice = true;
                break;
            }
        }
        if(!isInSlice) {
            nextGeneration[get1dIndex(getThreadId() * 2 + 1, placeIndex2 % size, size)] = parent1Gene;
            placeIndex2++;
        }
    }
}

__global__
void mutationKernel(int* population, double* prob, int* randomPoints, int size) {
    int index = getThreadId();
    if(prob[index] < 0.2f) {
        // swap mutation;
        int tmp = population[get1dIndex(index, randomPoints[index * 2], size)];
        population[get1dIndex(index, randomPoints[index * 2], size)] = population[get1dIndex(index, randomPoints[index * 2 + 1], size)];
        population[get1dIndex(index, randomPoints[index * 2 + 1], size)] = tmp;
    }
}

__global__
void copyToMigrationPoolKernel(int* population, int* pool, int genesPerThread, int size) {
    int index = getBlockId() * THREADS_PER_BLOCK * size + threadIdx.x * genesPerThread;
    for(int i = 0; i < genesPerThread; ++i) {
        pool[getThreadId() + i] = population[index];
        ++index;
    }
}

__global__
void copyFromMigrationPoolKernel(int* population, int* pool, int genesPerThread, int size) {
    int to = ((getBlockId() + 1) % BLOCK_COUNT) * THREADS_PER_BLOCK * size + threadIdx.x * genesPerThread;
    for(int i = 0; i < genesPerThread; ++i) {
        population[to] = pool[getThreadId() + i];
        ++to;
    }
}

////////// End section //////////

extern "C"
int* gaCuda(int* graphMatrix, int matrixSize, int* population, int populationPerIslandSize, int populationSize) {
    // Cuda configuration
    int threadsPerBlock = populationPerIslandSize;
    int numBlocks = 4;
    int migrationPoolPerIsland = 4;
    // Thread per gene
    int threadsNumberForMigration = migrationPoolPerIsland * matrixSize;
    // best individual
    int* best = (int*)malloc(matrixSize * sizeof(int));

    int* population1d;
    int* nextPopulation1d;
    int* migrationPool;
    double* fitnessArray;
    double* selectionRandomArray;
    int* randomPointsArray; // points for crossover and mutation
    int* matrixArray;
    curandState_t* states;
    // Allocate memory
    cudaMalloc(&matrixArray, matrixSize * matrixSize * sizeof(int));
    gpuErrchk(cudaPeekAtLastError());
    cudaMalloc(&population1d, populationSize * matrixSize * sizeof(int));
    gpuErrchk(cudaPeekAtLastError());
    cudaMalloc(&nextPopulation1d, populationSize * matrixSize * sizeof(int));
    gpuErrchk(cudaPeekAtLastError());
    cudaMalloc(&migrationPool, migrationPoolPerIsland * numBlocks * matrixSize * sizeof(int));
    gpuErrchk(cudaPeekAtLastError());
    cudaMalloc(&fitnessArray, populationSize * sizeof(double));
    gpuErrchk(cudaPeekAtLastError());
    cudaMalloc(&states, populationPerIslandSize * 2 * numBlocks * sizeof(curandState_t));
    gpuErrchk(cudaPeekAtLastError());
    cudaMalloc(&selectionRandomArray, populationPerIslandSize * numBlocks * sizeof(double));
    gpuErrchk(cudaPeekAtLastError());
    cudaMalloc(&randomPointsArray, populationPerIslandSize * 2 * numBlocks * sizeof(int));
    gpuErrchk(cudaPeekAtLastError());
    // Fill memory
    cudaMemcpy(matrixArray, graphMatrix, matrixSize * matrixSize * sizeof(int), cudaMemcpyHostToDevice);
    gpuErrchk(cudaPeekAtLastError());
    cudaMemcpy(population1d, population, populationSize * matrixSize * sizeof(int), cudaMemcpyHostToDevice);
    gpuErrchk(cudaPeekAtLastError());
    cudaMemcpyToSymbol(BLOCK_COUNT, &numBlocks, sizeof(int), 0, cudaMemcpyHostToDevice);
    gpuErrchk(cudaPeekAtLastError());
    cudaMemcpyToSymbol(THREADS_PER_BLOCK, &threadsPerBlock, sizeof(int), 0, cudaMemcpyHostToDevice);
    gpuErrchk(cudaPeekAtLastError());
    cudaMemset(nextPopulation1d, -1, populationSize * matrixSize * sizeof(int));
    gpuErrchk(cudaPeekAtLastError());

    fitnessKernel<<<numBlocks, threadsPerBlock>>>(matrixArray, population1d, fitnessArray, populationPerIslandSize, matrixSize);
    for(int i = 0; i < 2000; ++i) {
        initRandomKernel<<<numBlocks, threadsPerBlock>>>(time(0), states);
        gpuErrchk(cudaPeekAtLastError());
        uniformRandomSelectionKernel<<<numBlocks, threadsPerBlock>>>(states, selectionRandomArray);
        gpuErrchk(cudaPeekAtLastError());
        crossoverRandomPointsKernel<<<numBlocks, threadsPerBlock>>>(states, randomPointsArray, matrixSize);
        gpuErrchk(cudaPeekAtLastError());
        breedKernel<<<numBlocks, threadsPerBlock / 2>>>(population1d, nextPopulation1d, fitnessArray, selectionRandomArray, randomPointsArray, matrixSize);
        mutationKernel<<<numBlocks, threadsPerBlock>>>(nextPopulation1d, selectionRandomArray, randomPointsArray, matrixSize);
        gpuErrchk(cudaPeekAtLastError());
        fitnessKernel<<<numBlocks, threadsPerBlock>>>(matrixArray, nextPopulation1d, fitnessArray, populationPerIslandSize, matrixSize);
        gpuErrchk(cudaPeekAtLastError());
        if(i % 100 == 0 && i != 0) {
            copyToMigrationPoolKernel<<<numBlocks, threadsNumberForMigration>>>(population1d, migrationPool, 1, matrixSize);
            gpuErrchk(cudaPeekAtLastError());
            copyFromMigrationPoolKernel<<<numBlocks, threadsNumberForMigration>>>(population1d, migrationPool, 1, matrixSize);
            gpuErrchk(cudaPeekAtLastError());
        }
        cudaDeviceSynchronize();
        cudaMemcpy(population1d, nextPopulation1d,  populationSize * matrixSize * sizeof(int), cudaMemcpyDeviceToDevice);
        cudaMemset(nextPopulation1d, -1, populationSize * matrixSize * sizeof(int));
        gpuErrchk(cudaPeekAtLastError());
    }
    int index = thrust::max_element(thrust::device, fitnessArray, fitnessArray + populationSize) - fitnessArray;
    cudaMemcpy(best, population1d + index * matrixSize, matrixSize * sizeof(int), cudaMemcpyDeviceToHost);
    gpuErrchk(cudaPeekAtLastError());

    cudaDeviceSynchronize();
    cudaFree(population1d);
    cudaFree(nextPopulation1d);
    cudaFree(fitnessArray);
    cudaFree(matrixArray);
    cudaFree(states);
    cudaFree(randomPointsArray);
    cudaFree(selectionRandomArray);
    cudaFree(migrationPool);
    return best;
}
