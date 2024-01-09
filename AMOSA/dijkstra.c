#include <stdio.h>
#include <limits.h>
#include <stdbool.h>
#include <stdlib.h>

#define V 9

typedef struct {
    int* path;
    int pathLength;
    int distance;
} Result;

int minDistance(int dist[], bool sptSet[]) {
    int min = INT_MAX, min_index;
    for (int v = 0; v < V; v++)
        if (sptSet[v] == false && dist[v] <= min)
            min = dist[v], min_index = v;
    return min_index;
}

Result* dijkstra(int graph[V][V], int src, int dest) {
    int dist[V];
    int parent[V];
    bool sptSet[V];

    for (int i = 0; i < V; i++)
        dist[i] = INT_MAX, sptSet[i] = false, parent[i] = -1;

    dist[src] = 0;

    for (int count = 0; count < V - 1; count++) {
        int u = minDistance(dist, sptSet);
        sptSet[u] = true;
        for (int v = 0; v < V; v++) {
            if (!sptSet[v] && graph[u][v] && dist[u] != INT_MAX && dist[u] + graph[u][v] < dist[v]) {
                dist[v] = dist[u] + graph[u][v];
                parent[v] = u;
            }
        }
    }

    int current = dest;
    int* path = (int*)malloc(V * sizeof(int));
    int length = 0;

    while (current != -1 && current != src) {
        path[length++] = current;
        current = parent[current];
    }
    path[length++] = src;

    Result* res = (Result*)malloc(sizeof(Result));
    res->path = path;
    res->pathLength = length;
    res->distance = dist[dest];

    return res;
}

int main() {
    int graph[V][V] = {
        { 0, 4, 0, 0, 0, 0, 0, 8, 0 },
        { 4, 0, 8, 0, 0, 0, 0, 11, 0 },
        { 0, 8, 0, 7, 0, 4, 0, 0, 2 },
        { 0, 0, 7, 0, 9, 14, 0, 0, 0 },
        { 0, 0, 0, 9, 0, 10, 0, 0, 0 },
        { 0, 0, 4, 14, 10, 0, 2, 0, 0 },
        { 0, 0, 0, 0, 0, 2, 0, 1, 6 },
        { 8, 11, 0, 0, 0, 0, 1, 0, 7 },
        { 0, 0, 2, 0, 0, 0, 6, 7, 0 }
    };

    int src, dest;
    src = 1;
    dest = 3;
    Result* result = dijkstra(graph, src, dest);

    printf("Path from %d to %d: ", src, dest);
    for (int i = result->pathLength - 1; i >= 0; i--) {
        printf("%d ", result->path[i]);
    }
    printf("\n");
    printf("Shortest distance: %d\n", result->distance);

    free(result->path);
    free(result);

    return 0;
}
