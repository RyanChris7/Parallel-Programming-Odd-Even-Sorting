#include <cstdlib>
#include <fstream>
#include <iostream>
#include <chrono>

int compare(const int *a, const int *b) {
    return *a < *b;
}

int main (int argc, char **argv) {

    int num_elements; // number of elements to be sorted
    num_elements = atoi(argv[1]); // convert command line argument to num_elements

    int *elements = new int[num_elements]; // store elements
    int *sorted_elements = new int[num_elements]; // store sorted elements

    std::ifstream input(argv[2]);
    int element;
    int i = 0;
    while (input >> element) {
        elements[i] = element;
        i++;
    }
    std::cout << "actual number of elements:" << i << std::endl;

    std::chrono::high_resolution_clock::time_point t1;
    std::chrono::high_resolution_clock::time_point t2;
    std::chrono::duration<double> time_span;
    t1 = std::chrono::high_resolution_clock::now(); // record time

    // Sorting Process
    
    i = 0;
    while(i < num_elements) {
        for (int j = (i % 2) + 1; j < num_elements; j+= 2) {
            if(compare(&elements[j], &elements[j - 1]))
                std::swap(elements[j], elements[j - 1]);
        }
        i++;
    }
    
    /* Placing into sorted_elements array */

    for (int i = 0; i < num_elements; i++) {
        sorted_elements[i] = elements[i];
    }

    /* Sorting algorithm END */

    t2 = std::chrono::high_resolution_clock::now();  
    time_span = std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1);
    std::cout << "Student ID: " << "119010507" << std::endl; // replace it with your student id
    std::cout << "Name: " << "Ryan Christopher" << std::endl; // replace it with your name
    std::cout << "Assignment 1" << std::endl;
    std::cout << "Run Time: " << time_span.count() << " seconds" << std::endl;
    std::cout << "Input Size: " << num_elements << std::endl;
    std::cout << "Process Number: " << 1 << std::endl;   
    
    std::ofstream output(argv[2]+std::string(".seq.out"), std::ios_base::out);
    for (int i = 0; i < num_elements; i++) {
        output << sorted_elements[i] << std::endl;
    }
    
    delete[] elements;
    delete[] sorted_elements;
    return 0;
}


