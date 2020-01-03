#include <stdio.h>
#include <stdlib.h>
#include <math.h>

typedef struct {
  unsigned i;
  unsigned j;
} edge;

typedef struct {
  unsigned id;
  float pr;
} vertex;

/* adj list */
typedef struct {
  edge *e;
  unsigned len;
  unsigned cap;
} vec;

typedef struct {
  vec *v;
  unsigned n_vertices;
  unsigned n_edges;
} graph;

void vec_init(vec *v, unsigned cap) {
  v->e = calloc(cap, sizeof(edge));
  if (NULL != v->e) {
    v->cap=cap;
    v->len=0;
  }
}

void vec_release(vec *v) {
  if (NULL != v->e) {
    free(v->e);
    v->e = NULL;
    v->cap = 0;
    v->len = 0;
  }
}

void vec_resize(vec *v, unsigned cap) {
  v->e =
    realloc(v->e, sizeof(edge) * cap);
  v->cap = cap;
}

void vec_append(vec *v, edge *e) {
  if ((v->len+1) >= v->cap)
    vec_resize(v,
      v->cap == 0 ? 1 :
      2 * v->cap);
    v->e[v->len++] = *e;
}

#define SELF_LINKS 0

int pr_cmp(const void *a, const void *b) {
  float d = ((vertex*)a)->pr - ((vertex*)b)->pr;
  if (d > 0) return -1;
  else return 1;
}

void readtxt(graph *g,
  const char *fname) {

  FILE *f = fopen(fname, "r");
  if (!f) {
    fprintf(stderr,
    "couldn't open '%s'\n", fname);
    return;
  }

  unsigned N,M;

  fscanf(f, "%u %u", &N, &M);
  printf("%s, N=%u, M=%u\n",
    fname, N, M);

  g->n_vertices = N;
  g->n_edges = M;
  g->v = (vec*)calloc(N, sizeof(vec));

  unsigned i, j, n=0;
  while (2 == fscanf(f, "%u %u",
    &i, &j)) {
      if (i != j || SELF_LINKS)
      vec_append(&(g->v[i]), &(edge){i,j});
      vec_append(&(g->v[j]), &(edge){j,i});
      n++;
  }

  printf("read %u edges\n", n);
}

void edge_print(void *ptr) {
  edge *e = (edge*)ptr;
  printf("(%d %d)", e->i, e->j);
}

void vec_print(vec *v,
  void (*edge_printer)(void *)) {
  for (unsigned i=0; i<v->len; i++) {
    if (i == 0 || i==(v->len-1))
      edge_printer(&(v->e[i]));
    printf("%s",
    (v->len-1)==i ?
    "\n" : ".");
  }
}

void graph_print(graph *g) {
  for (unsigned i=0;
    i<g->n_vertices; i++) {
    vec_print(&(g->v[i]),
       edge_print);
  }
}

void graph_release(graph *g) {
  for (unsigned i=0; i<g->n_vertices; i++)
      vec_release(&(g->v[i]));
  free(g->v);
}

int main(int argc, char **argv) {

  char *fname =
  argc > 1 ?
  argv[1] : "web-NotreDame.txt";

  graph g;

  readtxt(&g, fname);

  unsigned i, j, k=0;
  unsigned N = g.n_vertices;
  float *p = malloc(N * sizeof(float));
  float *pp = malloc(N * sizeof(float));
  float initial = 1.0 / (float)N;
  float d = 0.85;
  for (i=0; i<N; i++)
    p[i] = initial;

  float err;

  do {

    for (i=0; i<N; i++)
      pp[i] = 0.0f;

    for (i=0; i<N; i++)
      for (j=0; j<g.v[i].len; j++) {
        unsigned idx = g.v[i].e[j].j;
        pp[i] += 1.0f / g.v[idx].len * p[idx];
      }

    for (i=0; i<N; i++)
      pp[i]=d*pp[i]+(1.0f-d)/(float)N;

    err = .0f;
    for (i=0; i<N; i++)
      err += fabs(pp[i] - p[i]);

    for (i=0; i<N; i++)
      p[i] = pp[i];

    k++;
  } while (err > 1e-5);

  printf("err = %f\n", err);

  vertex *vt = malloc(N * sizeof(vertex));

  for (i=0; i<N; i++) {
    vt[i].id=i;
    vt[i].pr=p[i];
  }

  qsort(vt, N, sizeof(vertex), pr_cmp);

  for (i=0; i<10; i++)
    printf("vt[%6u]: %f\n", vt[i].id, vt[i].pr);

  free(p);
  free(pp);
  free(vt);
  graph_release(&g);

  return 0;

}
