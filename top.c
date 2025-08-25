#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// Function to convert subset index to binary representation
void subset_to_binary(int subset, int n, char* binary) {
    for (int i = 0; i < n; i++) {
        binary[i] = (subset & (1 << i)) ? '1' : '0';
    }
    binary[n] = '\0';
}

// Function to print subset in readable format
void print_subset(int subset, int n) {
    printf("{");
    bool first = true;
    if (subset == 0) {
        printf("∅");
    } else {
        for (int i = 0; i < n; i++) {
            if (subset & (1 << i)) {
                if (!first) printf(",");
                printf("%c", 'a' + i);
                first = false;
            }
        }
    }
    printf("}");
}

// Check if a topology string is valid
bool is_valid_topology(char* topology, int n) {
    int total_subsets = 1 << n;
    
    // Rule 1: Empty set (subset 0) and full set (subset 2^n - 1) must be in topology
    if (topology[0] != '1' || topology[total_subsets - 1] != '1') {
        return false;
    }
    
    // Rule 2: Arbitrary unions must be in topology
    for (int i = 0; i < total_subsets; i++) {
        if (topology[i] == '1') {
            for (int j = i + 1; j < total_subsets; j++) {
                if (topology[j] == '1') {
                    int union_set = i | j;
                    if (topology[union_set] == '0') {
                        return false;
                    }
                }
            }
        }
    }
    
    // Rule 3: Finite intersections must be in topology
    for (int i = 0; i < total_subsets; i++) {
        if (topology[i] == '1') {
            for (int j = i + 1; j < total_subsets; j++) {
                if (topology[j] == '1') {
                    int intersection = i & j;
                    if (topology[intersection] == '0') {
                        return false;
                    }
                }
            }
        }
    }
    
    return true;
}

// Recursive function to generate all possible topologies
void generate_topologies(char* topology, int pos, int n, int* count) {
    int total_subsets = 1 << n;
    
    if (pos == total_subsets) {
        if (is_valid_topology(topology, n)) {
            (*count)++;
            printf("Topology %d: %s\n", *count, topology);
            printf("Contains subsets: ");
            bool first = true;
            for (int i = 0; i < total_subsets; i++) {
                if (topology[i] == '1') {
                    if (!first) printf(", ");
                    print_subset(i, n);
                    first = false;
                }
            }
            printf("\n\n");
        }
        return;
    }
    
    // Skip if this position is already determined by topology rules
    if (topology[pos] != '2') {
        generate_topologies(topology, pos + 1, n, count);
        return;
    }
    
    // Try setting this subset to 0 (not in topology)
    topology[pos] = '0';
    
    // Check if this violates any immediate rules
    bool valid_0 = true;
    
    // If this is empty set or full set, it must be in topology
    if (pos == 0 || pos == (1 << n) - 1) {
        valid_0 = false;
    }
    
    if (valid_0) {
        generate_topologies(topology, pos + 1, n, count);
    }
    
    // Try setting this subset to 1 (in topology)
    topology[pos] = '1';
    
    // When adding a set to topology, we must also add all necessary unions and intersections
    bool valid_1 = true;
    char temp_topology[total_subsets + 1];
    strcpy(temp_topology, topology);
    
    // Add necessary unions
    for (int i = 0; i < pos; i++) {
        if (temp_topology[i] == '1') {
            int union_set = i | pos;
            if (union_set < total_subsets && temp_topology[union_set] == '0') {
                valid_1 = false;
                break;
            } else if (union_set < total_subsets && temp_topology[union_set] == '2') {
                temp_topology[union_set] = '1';
            }
        }
    }
    
    // Add necessary intersections
    if (valid_1) {
        for (int i = 0; i < pos; i++) {
            if (temp_topology[i] == '1') {
                int intersection = i & pos;
                if (temp_topology[intersection] == '0') {
                    valid_1 = false;
                    break;
                } else if (temp_topology[intersection] == '2') {
                    temp_topology[intersection] = '1';
                }
            }
        }
    }
    
    if (valid_1) {
        strcpy(topology, temp_topology);
        generate_topologies(topology, pos + 1, n, count);
    }
    
    // Reset for backtracking
    topology[pos] = '2';
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Usage: %s <n>\n", argv[0]);
        printf("Where n is the number of elements (3-7)\n");
        return 1;
    }
    
    int n = atoi(argv[1]);
    if (n < 3 || n > 7) {
        printf("Error: n must be between 3 and 7\n");
        return 1;
    }
    
    int total_subsets = 1 << n;
    printf("Generating all topologies on set {");
    for (int i = 0; i < n; i++) {
        if (i > 0) printf(",");
        printf("%c", 'a' + i);
    }
    printf("} with %d subsets\n\n", total_subsets);
    
    // Initialize topology string: all positions unset (2)
    char* topology = malloc(total_subsets + 1);
    for (int i = 0; i < total_subsets; i++) {
        topology[i] = '2';
    }
    topology[total_subsets] = '\0';
    
    // Empty set and full set must be in topology
    topology[0] = '1';  // Empty set
    topology[total_subsets - 1] = '1';  // Full set
    
    int count = 0;
    generate_topologies(topology, 0, n, &count);
    
    printf("Total number of topologies found: %d\n", count);
    
    free(topology);
    return 0;
}