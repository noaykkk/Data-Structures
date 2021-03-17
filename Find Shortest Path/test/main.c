#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <unistd.h>

#include"graph.h"
#include"support.h"


/* constants used with Global variables */

#define TRUE 1
#define FALSE 0
#define PI acos(-1.0)

/* Global variables for command line parameters */
int Graphtype = 0;
int NumVertices = 0;
int AdjVer = 0;
int GraphOp = 0;
int Source = 0;
int Dest = 0;
int Time = 0;
int Seed ;
int Verbose = FALSE;

/* prototypes for functions in this file only */
void getCommandLine(int argc, char **argv);
graph *unitGraph(int type);
void operation(graph *G);


int main(int argc, char **argv)
{
    clock_t start, end;
    getCommandLine(argc, argv);

    /*---- build graph types and operation */
    graph *G = unitGraph(Graphtype);
    start = clock();
    operation(G);
    end = clock();
    if(Time == TRUE){
      printf("  After operation, time=%g\n",
                1000*((double)(end-start))/CLOCKS_PER_SEC);
    }
  
    graph_destruct (G);
   
}

graph *unitGraph(int type)
{
    graph *G;
    if(type == 1){
      //  Weakly-connected directed graph
      G = graph_construct(7);
      graph_add_edge(G, 1, 2, 3);
      graph_add_edge(G, 1, 6, 5);
      graph_add_edge(G, 2, 3, 7);
      graph_add_edge(G, 2, 4, 3);
      graph_add_edge(G, 2, 6, 10);
      graph_add_edge(G, 3, 4, 5);
      graph_add_edge(G, 3, 5, 1);
      graph_add_edge(G, 4, 5, 6);
      graph_add_edge(G, 5, 6, 7);
      graph_add_edge(G, 6, 0, 4);
      graph_add_edge(G, 6, 2, 5);
      graph_add_edge(G, 6, 3, 8);
      graph_add_edge(G, 6, 4, 9);
    }else if(type == 2){
      // Directed graph with symmetric weights
      G = graph_construct(10);
      graph_add_edge(G, 0, 1, 2);
      graph_add_edge(G, 0, 2, 1);
      graph_add_edge(G, 0, 3, 6);
      graph_add_edge(G, 1, 0, 2);
      graph_add_edge(G, 1, 4, 1);
      graph_add_edge(G, 1, 8, 3);
      graph_add_edge(G, 2, 0, 1);
      graph_add_edge(G, 2, 4, 3);
      graph_add_edge(G, 2, 5, 7);
      graph_add_edge(G, 3, 0, 6);
      graph_add_edge(G, 3, 5, 1);
      graph_add_edge(G, 3, 9, 5);
      graph_add_edge(G, 4, 1, 1);
      graph_add_edge(G, 4, 2, 3);
      graph_add_edge(G, 4, 6, 4);
      graph_add_edge(G, 5, 2, 7);
      graph_add_edge(G, 5, 3, 1);
      graph_add_edge(G, 5, 7, 4);
      graph_add_edge(G, 6, 4, 4);
      graph_add_edge(G, 6, 7, 5);
      graph_add_edge(G, 6, 8, 1);
      graph_add_edge(G, 7, 5, 4);
      graph_add_edge(G, 7, 6, 5);
      graph_add_edge(G, 7, 9, 1);
      graph_add_edge(G, 8, 1, 3);
      graph_add_edge(G, 8, 6, 1);
      graph_add_edge(G, 8, 9, 4);
      graph_add_edge(G, 9, 3, 5);
      graph_add_edge(G, 9, 7, 1);
      graph_add_edge(G, 9, 8, 4);
    }else if(type == 3){
      // Strongly-connected directed graph with N vertices
      int link_src = 0, link_dest = 0;
      G = graph_construct(NumVertices);
      for(link_src = 0 ; link_src < NumVertices; link_src++)
         for(link_dest = 0 ; link_dest < NumVertices; link_dest++) {
             if (link_src == link_dest) continue;
                graph_add_edge(G, link_src, link_dest, weight(link_src, link_dest));
          }
    }else if(type == 4){
      // Random graphs
      int i = 0, j = 0, min = NumVertices, max = 0, count = 0, num = 0;
      double C = 0, D = 0, ave = 0;
      double X[NumVertices], Y[NumVertices];
      G = graph_construct(NumVertices);

        // Initialization
      for(i = 0; i < NumVertices; i++){
        X[i] = drand48();
        Y[i] = drand48();
      }
      X[0] = 0.25; Y[0] = 0; X[NumVertices - 1] = 0.75; Y[NumVertices - 1] = 1.0;
      // Determine radius C
      double R = (double)AdjVer; double N = (double)NumVertices;
      C = sqrt(R/N/PI);

      // fill the weight
      for (i = 0; i < NumVertices; i++){
        count = 0;
        for(j = 0; j < NumVertices; j++){         
          // depends on the values of i, j, D and C there are three statements
          // 1. weight = 0 if i == j
          // 2. M/F if D <= C
          // 3. otherwise
          if(i == j){
            G->source[i][j] = 0;
          }else {
            // determine distance D 
            D = sqrt(pow(X[i]-X[j], 2) + pow(Y[i]-Y[j], 2)); 
            if(D <= C){
              G->source[i][j] = rand_weight(C, D); 
              count++;
            }  
          }
        }
        num += count;
        if(count <= min){
          min = count;
        }
        if(count >= max){
          max = count;
        }
      }
      ave = (double)num/NumVertices;
      printf("The total is : %d \nThe max is : %d \nThe min is : %d\nThe average is : %f\n", num, max, min, ave);
    }
    return G;
}


void operation(graph *G){
    if(GraphOp == 1){
      find_shortest_path(G, Source, Dest, Verbose);
    }else if(GraphOp == 2){
      // Find the nerwork diameter
      find_diameter(G, Verbose);
    }else if(GraphOp == 3){
      // Find the multiple link-disjoint paths
      find_link_disjoint_path(G, Source, Dest, Verbose);
    }
}



void getCommandLine(int argc, char **argv)
{
    /* optopt--if an unknown option character is found
     * optind--index of next element in argv 
     * optarg--argument for option that requires argument 
     * "x:" colon after x means argument required
     */
    int c;
    int index;

    while ((c = getopt(argc, argv, "g:n:a:h:s:d:r:vt")) != -1)
        switch(c) {
            case 'g': Graphtype = atoi(optarg);       break;
            case 'n': NumVertices = atoi(optarg);     break;
            case 'a': AdjVer = atoi(optarg);          break;
            case 'h': GraphOp = atoi(optarg);         break;
            case 's': Source = atoi(optarg);          break;
            case 'd': Dest = atoi(optarg);            break;
            case 'r': Seed = atoi(optarg);            break;
            case 'v': Verbose = TRUE;                 break;
            case 't': Time = TRUE;                    break;
            case '?':
                if (isprint(optopt))
                    fprintf(stderr, "Unknown option %c.\n", optopt);
                else
                    fprintf(stderr, "Unknown option character `\\x%x'.\n", optopt);
            default:
                printf("Lab6 command line options\n");
                printf("General options ---------\n");
                printf("  -g 1|2|3|4\n");
                printf("            Type of graph\n");
                printf("  -n        number of vertices in graph\n");
                printf("  -a        approximate number of adjacent vertices \n");
                printf("  -h 1|2|3\n");
                printf("            Graph operation\n");
                printf("  -v        turn on verbose prints (default off)\n");
                printf("  -s        number of the source vertex\n");
                printf("  -d        number of the destination vertex\n");
                printf("  -r 123    seed for the uniform random number generator\n");
                exit(1);
        }
    for (index = optind; index < argc; index++)
        printf("Non-option argument %s\n", argv[index]);
}
