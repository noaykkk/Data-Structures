/* graph.h
 */

#define inf FLT_MAX


typedef struct graph_chunk{
  int vex_num;
  double **source;
} graph;


/* prototype definitions for functions in graph.c */
graph *graph_construct (int NumVertices);
void graph_destruct (graph *G);
int graph_add_edge(graph *G, int link_src, int link_dest, double link_weight);
double **graph_shortest_path(graph *G, int path_src, int Verbose);
void graph_debug_print(graph *G);
void matrix_destruct(double **arr, int n);

/* commands specified to vim. ts: tabstop, sts: soft tabstop sw: shiftwidth */
/* vi:set ts=8 sts=4 sw=4 et: */
