#include "sailing.h"

/*
 * testlibpq.c
 *
 *      Test the C version of libpq, the PostgreSQL frontend library.
 */
#include <libpq-fe.h>
#include <stdio.h>
#include <stdlib.h>
#include <bsd/err.h>
#include <bsd/sys/tree.h>

struct node {
  RB_ENTRY(node) entry;
  int i;
};

int intcmp(struct node *, struct node *);
void print_tree(struct node *);

int intcmp(struct node *e1, struct node *e2) {
  return (e1->i < e2->i ? -1 : e1->i > e2->i);
}

RB_HEAD(inttree, node) head = RB_INITIALIZER(&head);
RB_PROTOTYPE(inttree, node, entry, intcmp)
RB_GENERATE(inttree, node, entry, intcmp)

int testdata[] = {20, 16, 17, 13, 3,  6,  1,  8, 2,  4,
                  10, 19, 5,  9,  12, 15, 18, 7, 11, 14};

void print_tree(struct node *n) {
  struct node *left, *right;

  if (n == NULL) {
    printf("nil");
    return;
  }
  left = RB_LEFT(n, entry);
  right = RB_RIGHT(n, entry);
  if (left == NULL && right == NULL)
    printf("%d", n->i);
  else {
    printf("%d(", n->i);
    print_tree(left);
    printf(",");
    print_tree(right);
    printf(")");
  }
}

int crazy(void) {
  int i;
  struct node *n;

  for (i = 0; i < sizeof(testdata) / sizeof(testdata[0]); i++) {
    if ((n = malloc(sizeof(struct node))) == NULL)
      err(1, NULL);
    n->i = testdata[i];
    RB_INSERT(inttree, &head, n);
  }

  RB_FOREACH(n, inttree, &head) { printf("%d\n", n->i); }
  print_tree(RB_ROOT(&head));
  printf("\n");
  return (0);
}
static void exit_nicely(PGconn *conn) {
  PQfinish(conn);
  exit(1);
}

void printPG(PGresult *res) {
  /* first, print out the attribute names */
  int nFields;
  int i;
  int j;
  nFields = PQnfields(res);
  for (i = 0; i < nFields; i++)
    printf("%-15s", PQfname(res, i));
  printf("\n\n");

  /* next, print out the rows */
  for (i = 0; i < PQntuples(res); i++) {
    for (j = 0; j < nFields; j++)
      printf("%-15s", PQgetvalue(res, i, j));
    printf("\n");
  }
}

int sailing_demo(void) {
  const char *conninfo;
  PGconn *conn;
  PGresult *res;
  int nFields;
  int i, j;

  conninfo = "dbname = demo";

  /* Make a connection to the database */
  conn = PQconnectdb(conninfo);

  /* Check to see that the backend connection was successfully made */
  if (PQstatus(conn) != CONNECTION_OK) {
    fprintf(stderr, "Connection to database failed: %s", PQerrorMessage(conn));
    exit_nicely(conn);
  }

  /*
   * Our test case here involves using a cursor, for which we must be inside
   * a transaction block.  We could do the whole thing with a single
   * PQexec() of "select * from pg_database", but that's too trivial to make
   * a good example.
   */

  /* Start a transaction block */
  res = PQexec(conn, "BEGIN");
  if (PQresultStatus(res) != PGRES_COMMAND_OK) {
    fprintf(stderr, "BEGIN command failed: %s", PQerrorMessage(conn));
    PQclear(res);
    exit_nicely(conn);
  }

  /*
   * Should PQclear PGresult whenever it is no longer needed to avoid memory
   * leaks
   */
  PQclear(res);

  /*
   * Fetch rows from pg_database, the system catalog of databases
   */
  res = PQexec(conn, "DECLARE myportal CURSOR FOR select * from pg_database");
  if (PQresultStatus(res) != PGRES_COMMAND_OK) {
    fprintf(stderr, "DECLARE CURSOR failed: %s", PQerrorMessage(conn));
    PQclear(res);
    exit_nicely(conn);
  }
  PQclear(res);

  res = PQexec(conn, "FETCH ALL in myportal");
  if (PQresultStatus(res) != PGRES_TUPLES_OK) {
    fprintf(stderr, "FETCH ALL failed: %s", PQerrorMessage(conn));
    PQclear(res);
    exit_nicely(conn);
  }

  printPG(res);

  PQclear(res);

  /* close the portal ... we don't bother to check for errors ... */
  res = PQexec(conn, "CLOSE myportal");
  PQclear(res);

  /* end the transaction */
  res = PQexec(conn, "END");
  PQclear(res);

  /* close the connection to the database and cleanup */
  PQfinish(conn);

  crazy();

  return 1000;
}

//int sailing_demo(void) { return 1000; }
