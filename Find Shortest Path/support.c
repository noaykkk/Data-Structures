#include<stdlib.h>
#include<stdio.h>
#include<limits.h>
#include<float.h>
#include <stdbool.h> 
#include <assert.h>
#include<math.h>

#include"graph.h"
#include"support.h"


double find_max(double **arr, int n, bool *con);
/* This function is used to find the shortest path and print it 
 */
void find_shortest_path(graph *G, int src, int dest, int Verbose){
  // Find the shortest path and print
      double **arr = graph_shortest_path(G, src, Verbose);
      if(arr[dest][2] != inf){
        printf("The shortest cost from %d to %d is %f, the path is ", src, dest, arr[dest][1]);
        path_print(arr, src, dest);
        printf(".\n");
      }else{
        printf("A path does not exist between %d and %d\n", src, dest);
      }
      
      // Free the array
      matrix_destruct(arr, G->vex_num);
}

/* This function is used to pritf the path 
 */
void path_print(double **arr, int src, int p){
    if(p == src){
      printf("%d", p);
    }else{
      path_print(arr, src, (int)arr[p][2]);
      printf(" -- %d", p);
    }
}


/* This function is used to calculate the weight for strongly connected directed graph with N vertices
   weight = |(𝑖 − 𝑗)/1.5| + (𝑖 − 𝑗 + 1.5)^2 + 1.5j if i != j
          = 0                                     if i == j
 */
double weight(int i, int j)
{
    if(i == j){
      return 0;
    }else{
      double p1 = (i-j)/1.5;
      double p2 = (i-j+1.5)*(i-j+1.5);
      double p3 = 1.5*j;
      if(p1 < 0){
        p1 = -p1;
      }
      return p1 + p2 + p3;
    }
}

/* This function is used to find the network diameter and print it
 */
void find_diameter(graph *G, int Verbose){
    int i = 0, src = 0, dest = 0;
    double **arrl, **arrr, maxl = 0, maxr = 0;
    bool Con = false;       // boolean variable to determine the graph is connected or not
    bool *con = &Con;
    // Do G->vex_num times shortest path find, then compare the results to find the maximum and print the path
      // First start with the first vertex of graph, otherwise (0)
    arrl = graph_shortest_path(G, 0, Verbose);
    for(i = 1; i < G->vex_num; i++){
      // Then find shortest path of vertex (1), compare two matrix to get the maximum, free the matrix which not include the maximum
      arrr = graph_shortest_path(G, i, Verbose);
      maxl = find_max(arrl, G->vex_num, con);
      maxr = find_max(arrr, G->vex_num, con);
      if(maxl < maxr){
        src = i;      // reset the source
        matrix_destruct(arrl, G->vex_num);
        arrl = arrr;
        maxl = maxr;
      }else{
        matrix_destruct(arrr, G->vex_num);
      }
      // Continue thoes operation with next vertex until the last vertex of graph
    }

    // print the maximum, path and connection
    for(i = 0; i < G->vex_num; i++){
      if(arrl[i][1] == maxl){
        dest = i;
      }
    }
    printf("The deameter is %f, and the path is ", maxl);
    path_print(arrl, src, dest);
    printf(".\n");
    if(*con == true){
      printf("The graph is not connected.\n");
    }
    // free the matrix
    matrix_destruct(arrl, G->vex_num);
}

/* Function to find the maximum in the shortest path in a matrix
 * It would return the max value and determine the graph is connected or nor
 */
double find_max(double **arr, int n, bool *con){
    // do n times loop find the maximum and return
    int i = 0;
    double max = 0;
    for(i = 0; i < n; i++){
      if(arr[i][1] != inf){
        if(max < arr[i][1] && arr[i][2] != inf){
          max = arr[i][1];
        }        
      }else{
        *con = true;
      }
    }
    return max;
}


/* Function to find the shortest path then print the path
 * The difference between find_shortest_path is after print the path, the path would be remove 
 * and the function would find next shortest path and print until there is no path from source to destination
 */
void find_link_disjoint_path(graph *G, int src, int dest, int Verbose){
    double **arr;
    int v = -1, p = -1;  // vertex and its predecessor
    // A loop function to implement the link-disjoint path
    while(1){
      arr = graph_shortest_path(G, src, Verbose);
      // If the path found, print and remove
      if(arr[dest][2] != inf){
        printf("The shortest cost from %d to %d is %f, the path is ", src, dest, arr[dest][1]);
        path_print(arr, src, dest);
        printf(".\n");
        // Remove the edges
        v = dest;
        while(v != src){
          p = (int)arr[v][2];
          G->source[p][v] = inf;
          v = p;
        }
        // Free matrix
        matrix_destruct(arr, G->vex_num);
        // After removing, back to graph and search the next path from source to destination
      }
      // If there is no path between source and dest, print the statement
      else{
        printf("There are no path still between %d and %d\n", src, dest);
        break;
      }     
    }   
}

/* Function to calculate random graph weights
 */
 double rand_weight(double C, double D){
   double M = 0, F = 0;
   M = log10(1 + pow(1/C, 2));
   F = log10(1 + pow(1/(D+C/100), 2));
   return  M/F;
 }
