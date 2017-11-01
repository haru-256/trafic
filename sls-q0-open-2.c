/**************************************************************************/
/* sls-q0-1  ���o�͂̃��[�����ύX                                         */
/*  �Z���̏��Ԃ�1�̎��ԃX�e�b�v��1�񂵂��ς����Ȃ�
 */
/**************************************************************************/

#include <math.h>
#include <stdio.h>
#include <stdlib.h> /* 乱数を使うために include */

//#define L 1001    		/* �Z���̌� */
//#define Middle 500    		/* �Z���̌� */
#define L 50      /* 列の長さ */
#define Middle 10 /* セルの個数 */
//#define T 20000		/* 計算回数 */
//#define TR 2000
//#define Tflow 1000
#define T 1000 /* 計算回数 */
#define TR 100
#define Tflow 50 /* 画像の縦の長さ 時間を表す*/

FILE *fp_output;
char fo[65];

int road[L], car[L + 2], car_previous[L + 2], car_new[L + 2];
int car_velocity[L + 2];
int road2[L];
int density[L], current_density[L];

/* def of Node */

typedef struct {
  int x;
  int v;
} Data;

typedef struct tagListNode {
  struct tagListNode *prev;
  struct tagListNode *next;
  Data data;
} ListNode;

/* def of List */

typedef struct {
  ListNode *first;
  ListNode *last;
  int CarNumber;
} List;

double alpha, beta, gamma_f, p, q0;
double alpha_c;
double alpha_ini, alpha_fin, delta_alpha;
int iterationNumber, iter;

/* 関数プロトタイプ宣言 */
double get_uniform_random(void);

void InitList(List *list1);
void copyList(List *list2, List *list1);
void prepend(List *list1, Data data1);
void insert(List *list1, Data data1, ListNode *node);
void printList(List *list1);
void printList1(List *list1, int i1);
void RemoveFront(List *list);
void RemoveRear(List *list);
void ClearList(List *list);

int main(int argc, char **argv) {
  int buf;
  Data tmpdata;
  ListNode *newnode, *thisnode, *nextnode, *startnode;

  int i;
  int itime;
  int x, v, xp1;
  int vsum = 0;
  int n_alpha;
  double rho, current;

  List pastList, presentList, newList, tmpList;
  List *list1;

  // alpha  = 1.0/3.0;
  // beta   = 1.0;
  gamma_f = 0.0;
  p = 0.75;
  q0 = 1.0;

  alpha_c = 1.0 - sqrt(1.0 - p);
  printf("alpha_c = %f\n", alpha_c);

  /*******  output file name ************* *****************/
  printf(" output file name?\n");
  scanf("%s", fo);
  fp_output = fopen(fo, "w");

  beta = alpha_c + 0.2;
  alpha_ini = 0.1;

  alpha_fin = 1.0;
  /* 0.1~1.0まで幾つのalphaをとるか */
  printf("itertionNumber: ");
  scanf("%d", &iterationNumber);

  /* ある値 alpha について何回のデータを作るか */
  printf("n_alpha: ");
  scanf("%d", &n_alpha);

  /* beta固定でalphaを変化させていく */
  delta_alpha = (alpha_fin - alpha_ini) / (double)iterationNumber;

  /*******  loop of alpha begins *****************/

  for (iter = 0; iter < iterationNumber; iter++) {

    alpha = alpha_ini + delta_alpha * (double)iter;
    /*ある値 alpha について複数回のデータを作る*/
    for (i = 0; i < n_alpha; i++) {

      /*******  list , road �̍ŏ��̒l�������� *****************/

      InitList(&pastList);
      InitList(&presentList);

      for (i = 0; i < L; i++) {
        road[i] = 0;

        density[i] = 0;
        current_density[i] = 0;
      }

      /*******  ���Ԃ̃��[�v  *****************/
      for (itime = 0; itime < T; itime++) {

        vsum = 0;

        //      printf("=========  itime = %d\n ",itime);

        copyList(&newList, &presentList);

        /*******  �Ԃ��Ȃ��ꍇ ���͂ƍ����̂� *****************/
        if (newList.CarNumber == 0) { //  *******  �Ԃ��Ȃ��ꍇ�@*****************/

          /*--------------input ------------*/
          if (get_uniform_random() < alpha) { //���͂���
            tmpdata.x = 0;
            tmpdata.v = 1;
            prepend(&newList, tmpdata);
            newList.CarNumber += 1;
          }

          /*--------------on ramp ------------*/

          if (get_uniform_random() < gamma_f) {
            tmpdata.x = Middle;
            tmpdata.v = 1;

            insert(&newList, tmpdata, newList.first);

            newList.CarNumber += 1;
            //      printf(" insert insert \n");
          }

        } else { //  *******  �Ԃ������ꍇ�@*****************/

          startnode = newList.first;

          if (newList.first->data.x > 0) {

            /********* input of a car **************/

            if (get_uniform_random() < alpha) {
              tmpdata.x = 0;
              tmpdata.v = 1;
              prepend(&newList, tmpdata);
              newList.CarNumber += 1;
              //      printf("169  input \n ");
            }
          }

          /*******  �Ԃ̈ړ�  *****************/

          for (thisnode = startnode; thisnode != NULL;
               thisnode = thisnode->next) {

            tmpdata = thisnode->data;
            x = tmpdata.x;
            v = tmpdata.v;

            if (x == L - 1) { // x = L-1

              if (get_uniform_random() < beta) { // �o��
                RemoveRear(&newList);
                newList.CarNumber -= 1;

                //      printf(" output \n ");

              } else {
                thisnode->data.v = 0;
              }
            } else { // x < L-1

              if (v == 0) { // slow to start effect
                if (get_uniform_random() < q0) {
                  v = 1;
                  //      printf("q0  x = %d v = %d\n ",x,v);
                }
              }

              nextnode = thisnode->next;

              if (nextnode != NULL) {
                tmpdata = thisnode->next->data;
                xp1 = tmpdata.x;
              } else {
                xp1 = L;
              }

              v = fmin(v, xp1 - x - 1);

              if (get_uniform_random() < 1.0 - p) { // random breaking
                v = fmax(0, v - 1);
              }
              thisnode->data.x = x + v;
              thisnode->data.v = v;
              vsum = vsum + v;

              if (x + v < Middle && xp1 > Middle) { // ����

                if (get_uniform_random() < gamma_f) {
                  tmpdata.x = Middle;
                  tmpdata.v = 1;
                  insert(&newList, tmpdata, thisnode);

                  //      printf("insert  x = %d xp1 = %d\n ",x,xp1);

                  newList.CarNumber += 1;
                  thisnode = thisnode->next;
                }
              }

            } // close if(x == L-1)

          } //	for(thisnode = startnode; thisnode != NULL; thisnode = thisnode
            //-> next ) closed

        } //  *******  �Ԃ������ꍇ�̏������I��  /

        ClearList(&pastList);
        pastList = presentList;
        presentList = newList;

        //   ---------------  output results 1

        if (itime > T - TR - 1) {

          for (i = 0; i < L; i++) {
            road2[i] = 0;
          }

          i = 1;

          if (presentList.CarNumber > 0) {

            for (thisnode = presentList.first; thisnode != NULL;
                 thisnode = thisnode->next) {
              tmpdata = thisnode->data;
              x = tmpdata.x;
              v = tmpdata.v;
              //     road2[x] = i;
              road2[x] = 1;
              density[x] += 1;
              current_density[x] += v;
              i++;
            }
          }

          rho = (double)presentList.CarNumber / (double)L;
          current = (double)vsum / (double)L;

          if (itime > T - Tflow - 1) {

            // fprintf(fp_output,"%d\t", itime);

            // fprintf(fp_output,"%le  %le \t", rho, current);
            if (itime == T - 1) {
              for (i = 0; i < L - 1; i++) {
                fprintf(fp_output, "%d, ", road2[i]);
              }
              fprintf(fp_output, "%d", road2[L - 1]);
            } else {
              for (i = 0; i < L; i++) {
                fprintf(fp_output, "%d,", road2[i]);
              }
            }

            /* データを横一列に並べるために改行をなくす */
            // fprintf(fp_output,"\n");
          }
        }
      } // ------end of time loop
      // alpha の値を出力
      fprintf(fp_output, ",%f\n", alpha);
    }
  } // ------end of alpha loop

  fclose(fp_output);

  return 0;
}

/** [0,1) ���l�������Ԃ�. ���炩���� srand() �� seed ��set */
double get_uniform_random(void) {
  /*
     rand()�� 0 �ȏ� RAND_MAX �ȉ��̕����Ȃ������������_���ɕԂ�.
  */
  return ((double)rand()) / ((double)RAND_MAX + 1.0);
}

/*******  copy list1 to list2 ************************/
void copyList(List *list2, List *list1) {
  ListNode *newnode, *thisnode, *lastnode;

  InitList(list2);
  list2->CarNumber = list1->CarNumber;

  for (thisnode = list1->first; thisnode != NULL; thisnode = thisnode->next) {

    newnode = (ListNode *)malloc(sizeof(ListNode));
    newnode->data = thisnode->data;
    newnode->next = NULL;
    newnode->prev = NULL;

    lastnode = list2->last;

    if (lastnode != NULL) {
      lastnode->next = newnode;
      newnode->prev = lastnode;
      list2->last = newnode;
    } else {
      list2->first = newnode;
      list2->last = newnode;
    }
  }

  /************ write list2 ************************/
  //      printf("list2 137 \n");
  //      printList(list2);
}

void prepend(List *list1, Data data1) {

  ListNode *newnode, *firstnode;

  newnode = (ListNode *)malloc(sizeof(ListNode));
  newnode->data = data1;
  newnode->next = NULL;
  newnode->prev = NULL;

  firstnode = list1->first;

  if (firstnode != NULL) {
    //      printf(" 201 \n");

    newnode->next = firstnode;
    firstnode->prev = newnode;
    list1->first = newnode;
  } else {
    //      printf(" 207 \n");

    list1->first = newnode;
    list1->last = newnode;
  }

  /************ write list1 ************************/
  //      printf("list1 170 \n");
  //      printList(list1);
}

void insert(List *list1, Data data1, ListNode *node) {

  ListNode *newnode;

  newnode = (ListNode *)malloc(sizeof(ListNode));
  newnode->data = data1;
  newnode->next = NULL;
  newnode->prev = NULL;

  if (node == NULL) { // �擪�ɒǉ�
    if (list1->first != NULL) {
      //      printf(" 201 \n");
      (newnode->next) = (list1->first);
      (list1->first)->prev = newnode;
      (list1->first) = newnode;
    } else {
      list1->first = newnode;
      list1->last = newnode;
    }
  } else if ((list1->last) == node) { // �Ō��ɒǉ�

    newnode->next = node->next;
    newnode->prev = node;
    node->next = newnode;

  } else { // ���Ԃɒǉ�

    newnode->next = node->next;
    newnode->prev = node;
    (node->next)->prev = newnode;
    node->next = newnode;
  }
}

/************ print contents of a list ************************/
void printList(List *list1) {

  ListNode *thisnode;
  Data tmpdata;

  for (thisnode = list1->first; thisnode != NULL; thisnode = thisnode->next) {
    tmpdata = thisnode->data;
    printf("x = %d v = %d \n", tmpdata.x, tmpdata.v);
  }
}

/************ print contents of a list ************************/
void printList1(List *list1, int i1) {

  ListNode *thisnode;
  Data tmpdata;

  if (i1 == 0) {
    thisnode = list1->first;
    if (thisnode != NULL) {
      tmpdata = thisnode->data;
      printf("first x = %d v = %d \n", tmpdata.x, tmpdata.v);
    } else {
      printf("thisnode is NULL \n");
    }
  } else if (i1 == 1) {
    thisnode = list1->last;
    if (thisnode != NULL) {
      tmpdata = thisnode->data;
      printf("last x = %d v = %d \n", tmpdata.x, tmpdata.v);
    } else {
      printf("thisnode is NULL \n");
    }
  } else {
    printf("CarNumber= %d\n", list1->CarNumber);
  }
}

/************ initialization of a list ************************/
void InitList(List *list1) {
  list1->first = NULL;
  list1->last = NULL;
  list1->CarNumber = 0;
}

/***�擪�m�[�h���폜***/
void RemoveFront(List *list) {
  if (list->first != NULL) {
    ListNode *ptr = (list->first)->next;
    free(list->first);
    list->first = ptr;
  }
}

/****�S�m�[�h���폜****/
void ClearList(List *list) {
  while (list->first != NULL) {
    RemoveFront(list);
  }
  list->last = NULL;
}

/***�����m�[�h���폜***/
void RemoveRear(List *list) {
  if (list->first != NULL) {
    if ((list->first)->next == NULL) {
      RemoveFront(list);
      list->first = NULL;
      list->last = NULL;
    } else {

      ListNode *ptr = (list->last)->prev;
      free(list->last);
      //		list->first = list->current = ptr;
      ptr->next = NULL;

      list->last = ptr;
    }
  }
}
