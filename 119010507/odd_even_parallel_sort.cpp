#include <mpi.h>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <chrono>

bool not_match(bool level, int rank, const int *displace_array){
    return !(level == displace_array[rank] % 2);
}

int compare(const int *x, const int *y) {
    return *x < *y;
}

void sort(int *my_elements, int num_my_elements, const int *displace_array, bool level, int rank) {
    size_t start = (displace_array[rank] + level) % 2;
    for (start; start < num_my_elements; start += 2) {
        if(compare(&my_elements[start], &my_elements[start - 1]))
            std::swap(my_elements[start], my_elements[start - 1]);
    } 
}


/* Main function */
int main (int argc, char **argv){

    MPI_Init(&argc, &argv); 

    static int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);  
    
    static int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    static uint num_elements; // number of elements to be sorted
    num_elements = atoi(argv[1]); // convert command line argument to num_elements

    int *elements;
    int *sorted_elements; // store sorted elements

    if (rank == 0) { // read inputs from file (master process)
        elements = new int[num_elements];
        std::ifstream input(argv[2]);
        int element;
        int i = 0;
        while (input >> element) {
            elements[i] = element;
            i++;
        }
        std::cout << "actual number of elements:" << i << std::endl;
    }


    // Sorting Process
    int *displace_array = new int[world_size]; 
    int *tally_array = new int[world_size]; 


    int remainder = num_elements % world_size; 

    int num_my_elements = (rank < remainder) ? num_elements / world_size + 1 : num_elements / world_size;
    int *my_elements = new int[num_my_elements];

    int store; 
    size_t i = 0; 
    while(i < world_size) {
        tally_array[i] = (i < remainder) ? num_elements / world_size + 1 : num_elements / world_size;
        displace_array[i] = store;
        store += tally_array[i]; 
        i++;
    }

    MPI_Barrier(MPI_COMM_WORLD);

    /* Start timer */
    std::chrono::high_resolution_clock::time_point t1;
    std::chrono::high_resolution_clock::time_point t2;
    std::chrono::duration<double> time_span;
    if (rank == 0) { 
        t1 = std::chrono::high_resolution_clock::now(); // record time
    }

    MPI_Scatterv(elements, tally_array, displace_array, MPI_INT, my_elements, num_my_elements, MPI_INT, 0, MPI_COMM_WORLD);

    if(rank == 0) delete[] elements; 
    bool level = 0; 
    while (i < num_elements) {

        sort(my_elements, num_my_elements, displace_array, level, rank);

        if(rank != world_size - 1 && not_match(level, rank + 1, displace_array)) { 
            int n;
            MPI_Recv(&n, 1, MPI_INT, rank + 1, i, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            if(compare(&n, &my_elements[num_my_elements - 1]))
                    std::swap(n, my_elements[num_my_elements - 1]);
            MPI_Send(&n, 1, MPI_INT, rank + 1, i, MPI_COMM_WORLD);
        }

        if(rank != 0 && not_match(level, rank, displace_array)) { 
            MPI_Send(&my_elements[0], 1, MPI_INT, rank - 1, i, MPI_COMM_WORLD);
            MPI_Recv(&my_elements[0], 1, MPI_INT, rank - 1, i, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }

        level = !level;
        MPI_Barrier(MPI_COMM_WORLD);
        i++;
    }
    
    if(rank == 0) sorted_elements = new int[num_elements];
    MPI_Gatherv(my_elements, num_my_elements, MPI_INT, sorted_elements, tally_array, displace_array, MPI_INT, 0, MPI_COMM_WORLD); 


    /* TODO END */
    
    if (rank == 0){ // record time (only executed in master process)
        t2 = std::chrono::high_resolution_clock::now();  
        time_span = std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1);
        std::cout << "Student ID: " << "119010507" << std::endl; // replace it with your student id
        std::cout << "Name: " << "Ryan Christopher" << std::endl; // replace it with your name
        std::cout << "Assignment 1" << std::endl;
        std::cout << "Run Time: " << time_span.count() << " seconds" << std::endl;
        std::cout << "Input Size: " << num_elements << std::endl;
        std::cout << "Process Number: " << world_size << std::endl; 
    }
    
    delete[] tally_array;
    delete[] displace_array;
    delete[] my_elements;

    if (rank == 0){ // write result to file (only executed in master process)
        std::ofstream output(argv[2]+std::string(".parallel.out"), std::ios_base::out);
        for (int i = 0; i < num_elements; i++) {
            output << sorted_elements[i] << std::endl;
        }
    }
    
    delete[] sorted_elements;
    MPI_Finalize();
    
    return 0;
}


