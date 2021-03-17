#include<stdlib.h>
#include<stdio.h>
#include<limits.h>
#include<float.h>
#include <stdbool.h> 
#include <assert.h>

#include"graph.h"

/* Allocates a new, empty graph
*/
graph *graph_construct (int NumVertices){
  int i = 0, j = 0;
  // alloc the graph and edges
  graph *G = (graph *)malloc(sizeof(graph));
  double **T = (double**)malloc(NumVertices * sizeof(double *));
  for(i = 0; i < NumVertices; i++){
    T[i] = (double *)malloc(NumVertices * sizeof(double));
  }
  // initialization
  for(i = 0; i < NumVertices; i++){
    for(j = 0; j < NumVertices; j++){
      if(i == j){
        T[i][j] = 0;
      }
      else{
        T[i][j] = inf;
      }
    }
  }
  G->vex_num = NumVertices;
  G->source = T;

  return G;
}

void graph_destruct (graph *G){
  int i = 0;
  for(i = 0; i < G->vex_num; i++){
    free(G->source[i]);
  }
  free(G->source);
  free(G);
}

/* Add a new vertex into the graph.
 * The funciton will return 0 if the vertex is already in the graph, return 1 if insertion successful.
 * If the graph is empty, the vertex would be source vertex.
 */
 int graph_add_edge(graph *G, int link_src, int link_dest, double link_weight){
   assert(link_src < G->vex_num);
   assert(link_dest < G->vex_num);
   assert(link_weight >= 0);
   if(G->source[link_src][link_dest] == inf){
     G->source[link_src][link_dest] = link_weight;
     return 1;
   }else{
     G->source[link_src][link_dest] = link_weight;
     return 0;
   }
 }

/* The implement of Dijkstra algorithm
 * The function would return a double matrix which includes shortest path
 */ 
double **graph_shortest_path(graph *G, int path_src, int Verbose){
  int count_added = 1;
  int i = 0, w = path_src,  u = 0;
  double min_cost = 0;
  assert(path_src < G->vex_num);
  if (Verbose) {
      printf("Starting at node %d\n", path_src);
  }

  // Initialize W 
  bool W[G->vex_num];
  for(i = 0; i < G->vex_num; i++){
    W[i] = false;
  }
  // Allocate a new return matrix
  // The first column of the matrix is the edge, the second column is the shortest distance 
  // from source edge to destination edge and the third is the edge predecessor
  double **arr = (double **) malloc(G->vex_num * sizeof(double *));
  for(i = 0; i < G->vex_num; i++){
    arr[i] = (double *) malloc(3 * sizeof(double));
  }
  // Initialize the matrix
  for(i = 0; i < G->vex_num; i++){
    arr[i][0] = (double)i;
    arr[i][1] = G->source[path_src][i];
    arr[i][2] = inf;
  }
  arr[path_src][1] = 0;   // initialize start source

  while(count_added != G->vex_num){
    w = -1;
    min_cost = inf;

    // Find the shortest distance
    for(i = 0; i < G->vex_num; i++){
      if(W[i] == false){
        if(arr[i][1] < min_cost){
          min_cost = arr[i][1];
          w = i;
        }
      }
    }

    // confirm the vertex
    if(w == -1){
      break;
    }
    W[w] = true;
    count_added++;
    if (Verbose) {
        printf("%d: %d confirmed with cost %g and predecessor %d\n", 
                count_added, w, min_cost, (int)arr[w][2]);
    }

    // update the shortest distance
    for(u = 0; u < G->vex_num; u++){
      if(W[u] == false && G->source[w][u] < inf){
        if(arr[w][1] + G->source[w][u] <= arr[u][1]){
          arr[u][1] = arr[w][1] + G->source[w][u];
          arr[u][2] = (double)w;
          if (Verbose) {
            printf("\tdest %d has lower cost %g with predecessor %d\n", u, arr[u][1], w);
          }
        }       
      }
    }
  }
  if (Verbose) {
      printf("Found %d nodes, including source\n", count_added);
  }
  return arr;
}

void graph_debug_print(graph *G){
  int i = 0, j =0;
  for(i = 0; i < G->vex_num; i++){
    for(j = 0; j < G->vex_num; j++){
        if(G->source[i][j] == inf){
            printf("inf, ");
        }
        else{
            printf("%3f, ", G->source[i][j]);
        }
    }
    printf("\n");
  }
}


/* This function is used to free the matrix which include the cost and predecessor
 */
void matrix_destruct(double **arr, int n){
    int i =0;
      for(i = 0; i < n; i++){
        free(arr[i]);
      }
      free(arr);
}
