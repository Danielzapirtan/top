#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef struct {
    int n;
    int power;
    char* topology;
    int count;
} TopologyGenerator;

bool is_topology_valid(TopologyGenerator* gen, int pos);
bool is_union_closed(TopologyGenerator* gen);
bool is_intersection_closed(TopologyGenerator* gen);
bool contains_universe(TopologyGenerator* gen);
bool contains_empty_set(TopologyGenerator* gen);
void print_topology_with_letters(TopologyGenerator* gen);
void generate_topologies(TopologyGenerator* gen, int pos);
void print_progress(TopologyGenerator* gen, int pos);

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Usage: %s <n>\n", argv[0]);
        printf("Where n is between 3 and 7\n");
        return 1;
    }

    int n = atoi(argv[1]);
    if (n < 3 || n > 7) {
        printf("Error: n must be between 3 and 7\n");
        return 1;
    }

    TopologyGenerator gen;
    gen.n = n;
    gen.power = 1 << n;  // 2^n
    gen.topology = (char*)malloc((gen.power + 1) * sizeof(char));
    gen.count = 0;

    // Initialize topology with '2' (unset)
    for (int i = 0; i < gen.power; i++) {
        gen.topology[i] = '2';
    }
    gen.topology[gen.power] = '\0';

    printf("Generating topologies for set of %d elements: {", n);
    for (int i = 0; i < n; i++) {
        printf("%c", 'a' + i);
        if (i < n - 1) printf(", ");
    }
    printf("}\n\n");

    generate_topologies(&gen, 0);

    printf("\nTotal topologies found: %d\n", gen.count);
    free(gen.topology);
    return 0;
}

bool is_topology_valid(TopologyGenerator* gen, int pos) {
    // Check if we've filled all positions
    if (pos == gen->power) {
        return contains_empty_set(gen) && 
               contains_universe(gen) &&
               is_union_closed(gen) &&
               is_intersection_closed(gen);
    }
    return true;
}

bool contains_universe(TopologyGenerator* gen) {
    // Universe set is the last one (all 1's in binary)
    return gen->topology[gen->power - 1] == '1';
}

bool contains_empty_set(TopologyGenerator* gen) {
    // Empty set is the first one (all 0's in binary)
    return gen->topology[0] == '1';
}

bool is_union_closed(TopologyGenerator* gen) {
    // For all pairs of sets in topology, their union must also be in topology
    for (int i = 0; i < gen->power; i++) {
        if (gen->topology[i] != '1') continue;
        for (int j = 0; j < gen->power; j++) {
            if (gen->topology[j] != '1') continue;
            
            int union_set = i | j;
            if (gen->topology[union_set] != '1') {
                return false;
            }
        }
    }
    return true;
}

bool is_intersection_closed(TopologyGenerator* gen) {
    // For all pairs of sets in topology, their intersection must also be in topology
    for (int i = 0; i < gen->power; i++) {
        if (gen->topology[i] != '1') continue;
        for (int j = 0; j < gen->power; j++) {
            if (gen->topology[j] != '1') continue;
            
            int intersection_set = i & j;
            if (gen->topology[intersection_set] != '1') {
                return false;
            }
        }
    }
    return true;
}

void print_topology_with_letters(TopologyGenerator* gen) {
    printf("Topology %d: %s\n", ++gen->count, gen->topology);
    
    printf("Sets in topology:\n");
    for (int i = 0; i < gen->power; i++) {
        if (gen->topology[i] == '1') {
            printf("{");
            bool first = true;
            for (int j = 0; j < gen->n; j++) {
                if (i & (1 << j)) {
                    if (!first) printf(", ");
                    printf("%c", 'a' + j);
                    first = false;
                }
            }
            if (first) printf("∅"); // Empty set
            printf("} ");
        }
    }
    printf("\n");
}

void generate_topologies(TopologyGenerator* gen, int pos) {
    if (pos == gen->power) {
        if (is_topology_valid(gen, pos)) {
            print_topology_with_letters(gen);
        }
        return;
    }

    // Try setting current position to 0 (not in topology)
    gen->topology[pos] = '0';
    if (is_topology_valid(gen, pos + 1)) {
        generate_topologies(gen, pos + 1);
    }

    // Try setting current position to 1 (in topology)
    gen->topology[pos] = '1';
    if (is_topology_valid(gen, pos + 1)) {
        generate_topologies(gen, pos + 1);
    }

    // Backtrack
    gen->topology[pos] = '2';
}

void print_progress(TopologyGenerator* gen, int pos) {
    static int last_percent = -1;
    int percent = (pos * 100) / gen->power;
    
    if (percent != last_percent) {
        printf("\rProgress: %d%%", percent);
        fflush(stdout);
        last_percent = percent;
    }
}