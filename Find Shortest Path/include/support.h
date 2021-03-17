/* support.h
 */

/* prototype function definitions */
void find_shortest_path(graph *G, int src, int dest, int Verbose);
void path_print(double **arr, int src, int p);
double weight(int i, int j);
void find_diameter(graph *G, int Verbose);
void find_link_disjoint_path(graph *G, int src, int dest, int Verbose);
double rand_weight(double C, double D);
/* commands specified to vim. ts: tabstop, sts: soft tabstop sw: shiftwidth */
/* vi:set ts=8 sts=4 sw=4 et: */
