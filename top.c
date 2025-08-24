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
    *top = 1ULL; // Start with just the empty set (bit 0)
}

void Copy(TOP src, TOP dest) {
    *dest = *src;
}

int is_valid_topology_partial(unsigned long long topology, int check_up_to) {
    // Check if empty set is included
    if (!(topology & 1)) return 0;
    
    // Check closure under finite intersections (only for sets we've checked so far)
    for (int i = 0; i <= check_up_to; i++) {
        if (!(topology & (1ULL << i))) continue;
        
        for (int j = i + 1; j <= check_up_to; j++) {
            if (!(topology & (1ULL << j))) continue;
            
            int intersection = i & j;
            if (intersection <= check_up_to && !(topology & (1ULL << intersection))) return 0;
        }
    }
    
    // Check closure under unions (only for sets we've checked so far)
    for (int i = 0; i <= check_up_to; i++) {
        if (!(topology & (1ULL << i))) continue;
        
        for (int j = i + 1; j <= check_up_to; j++) {
            if (!(topology & (1ULL << j))) continue;
            
            int union_set = i | j;
            if (union_set <= check_up_to && !(topology & (1ULL << union_set))) return 0;
        }
    }
    
    return 1;
}

int is_valid_topology(unsigned long long topology) {
    int num_subsets = 1 << n_elements;
    
    // Check if empty set and full set are included
    if (!(topology & 1)) return 0;
    if (!(topology & (1ULL << (num_subsets - 1)))) return 0;
    
    return is_valid_topology_partial(topology, num_subsets - 1);
}

void Next(TOP src, TOP dest) {
    unsigned long long current = *src;
    int num_subsets = 1 << n_elements;
    
    // Find the next valid topology
    do {
        current++;
        
        // Check if we've exceeded the valid range
        if (current >= (1ULL << num_subsets)) {
            *dest = 0;
            return;
        }
        
        // Early pruning: check if current candidate is valid for the sets processed so far
        // This helps eliminate many invalid topologies early
        int valid_so_far = 1;
        for (int i = 0; i < num_subsets; i++) {
            if (!is_valid_topology_partial(current, i)) {
                valid_so_far = 0;
                break;
            }
        }
        
    } while (!is_valid_topology(current));
    
    *dest = current;
}

void Render(TOP top) {
    unsigned long long topology = *top;
    
    if (topology == 0) {
        return;
    }
    
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
        fprintf(stderr, "Where n is the number of elements (3-5)\n");
        return 1;
    }
    
    n_elements = atoi(argv[1]);
    
    if (n_elements < 3 || n_elements > 5) {
        fprintf(stderr, "Error: n must be between 3 and 5 inclusive\n");
        return 1;
    }
    
    unsigned long long current_topology;
    unsigned long long next_topology;
    TOP current = &current_topology;
    TOP next = &next_topology;
    
    Init(current);
    Next(current, next);
    Copy(next, current);
    
    while (*current != 0) {
        Render(current);
        Next(current, next);
        Copy(next, current);
    }
    
    return 0;
}
