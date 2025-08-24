#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef unsigned long long *TOP;

extern void Init(TOP top);
extern void Next(TOP src, TOP dest);
extern void Render(TOP top);
extern void Copy(TOP src, TOP dest);

static int n_elements = 0;

void Init(TOP top) {
    *top = (1ULL << 63) | 1ULL;
}

void Copy(TOP src, TOP dest) {
    *dest = *src;
}

int is_valid_topology(unsigned long long topology) {
    int num_subsets = 1 << n_elements;
    
    // Check if empty set and full set are included
    if (!(topology & 1)) return 0; // Empty set must be included
    if (!(topology & (1ULL << (num_subsets - 1)))) return 0; // Full set must be included
    
    // Check closure under arbitrary unions
    for (int i = 0; i < num_subsets; i++) {
        if (!(topology & (1ULL << i))) continue;
        
        for (int j = i + 1; j < num_subsets; j++) {
            if (!(topology & (1ULL << j))) continue;
            
            int union_set = i | j;
            if (!(topology & (1ULL << union_set))) return 0;
        }
    }
    
    // Check closure under finite intersections
    for (int i = 0; i < num_subsets; i++) {
        if (!(topology & (1ULL << i))) continue;
        
        for (int j = i + 1; j < num_subsets; j++) {
            if (!(topology & (1ULL << j))) continue;
            
            int intersection = i & j;
            if (!(topology & (1ULL << intersection))) return 0;
        }
    }
    
    return 1;
}

void Next(TOP src, TOP dest) {
    unsigned long long current = *src;
    
    // Check if we've reached the end (MSB is 0)
    if (!(current & (1ULL << 63))) {
        *dest = 0;
        return;
    }
    
    // Remove the termination bit temporarily
    current &= ~(1ULL << 63);
    
    // Find the next valid topology
    do {
        current++;
        
        // Check if we've exceeded the valid range for n elements
        if (current >= (1ULL << (1ULL << n_elements))) {
            *dest = 0; // End of enumeration
            return;
        }
        
    } while (!is_valid_topology(current));
    
    // Add the termination bit back
    *dest = current | (1ULL << 63);
}

void Render(TOP top) {
    unsigned long long topology = *top;
    
    // Check for end of enumeration
    if (!(topology & (1ULL << 63))) {
        return;
    }
    
    // Remove the termination bit
    topology &= ~(1ULL << 63);
    
    int num_subsets = 1 << n_elements;
    int first = 1;
    
    printf("{");
    
    for (int i = 0; i < num_subsets; i++) {
        if (topology & (1ULL << i)) {
            if (!first) printf(", ");
            first = 0;
            
            printf("{");
            int subset_first = 1;
            
            for (int j = 0; j < n_elements; j++) {
                if (i & (1 << j)) {
                    if (!subset_first) printf(",");
                    subset_first = 0;
                    printf("%c", 'a' + j);
                }
            }
            printf("}");
        }
    }
    
    printf("}\n");
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <n>\n", argv[0]);
        fprintf(stderr, "Where n is the number of elements (3-6)\n");
        return 1;
    }
    
    n_elements = atoi(argv[1]);
    
    if (n_elements < 3 || n_elements > 6) {
        fprintf(stderr, "Error: n must be between 3 and 6 inclusive\n");
        return 1;
    }
    
    unsigned long long current_topology;
    unsigned long long next_topology;
    TOP current = &current_topology;
    TOP next = &next_topology;
    
    // Initialize to the first topology
    Init(current);
    
    // Generate and display all topologies
    while (*current != 0) {
        Render(current);
        Next(current, next);
        Copy(next, current);
    }
    
    return 0;
}
