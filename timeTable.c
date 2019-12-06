#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "stackqueue.h"
#define DAYS 31										//Day입니다
#define SPEED (500.0)								// 비행기의 속력입니다 500km/hour


typedef struct point {
	char name;
	int x, y;
	int distance, needTime, departureTime;			// 기존의 distance이외에 needtime과 departureTime, paths[26], pathsNum이 추가되었습니다.
	struct point* next;								//distance는 adjency list의 edge부분에서만 활성화되며, default값은 -1이고, addpath로 등록시 두 city간 거리가 기록됩니다.
	char paths[26];									//needTime은 다이스트라 알고리즘의 해당 city까지 필요한 cost로서, 경로상의 city와 departureTime과 distance에 의해 산출됩니다.
	int pathsNum;									//paths[26]은 해당 city까지 최단거리로 오는 city의 리스트입니다. pathsNum은 paths[26]의 길이를 나타냅니다
}City;
typedef struct a {
	int num;
	City** heads;
}Graph;

Graph* CreateGraph(int num);						// num개의 city가 있는 기본 그래프를 생성합니다.
void DestroyGraph(Graph* pgraph);					// 그래프 하나를 전부 메모리에 반환합니다.
void AddPath(Graph* pgraph, char src, char dest);	//pgraph내의 2개 city간에 bidriectional edge를 1개 생성합니다
void AddPaths(Graph* pgraph, int num);				//pgraph내에 num개만큼 랜덤하게 AddPath를 실행합니다.
int PathExists(Graph* pgraph, char src, char dst);  // AddPath에만 활용되는 함수입니다. 기존 그래프에 동일 path가 생성되어있는지 확인합니다.
Graph** CreateTimeTable(Graph* pgraph);				//기본 그래프 pgraph에 departure time을 할당한 31가지 그래프를 만듭니다.
void PrintGraph(Graph* pgraph);						//1개 graph의 내용을 출력합니다.
City* ShortestPath(Graph* pgraph, int src, int dst);	// pgraph내에 내용을 기록한 후 최종 목적도시를 반환합니다. 
			//여기서 needTime을 참조하면 도착예정시간, paths를 참조하면 경로상city가 담긴array, pathsNum은 pathsArray의 길이를 나타냅니다.
City* ExtractMin(City** Q, int* qNum);				// ShortestPath내에서만 쓰이는 함수로서 수도코드의 ExtractMin과 동일합니다.
void PrintArray(char* arr, int num);			// char array인쇄.

int main() {
	srand(time(NULL));
	int day;
	char src;
	printf("Graph project\n");
	printf("Enter the day: "); scanf("%d", &day); getchar();
	//day = 31;
	printf("Enter the source city (a~z): "); scanf("%c", &src);
	//src = e;
	Graph* pgraph = CreateGraph(26);			//26개 city를 생성합니다.
	AddPaths(pgraph, 100);						//해당 26개 city간에 path를 무작위로 100개를 생성합니다.
	Graph** pgraphs = CreateTimeTable(pgraph);	//위에 생성된 path에 무작위 departureTime을 부여한 graph 31개를 생성합니다.
	PrintGraph(pgraphs[day - 1]);						// 3+1=4일자의 depatureTime이 담긴 그래프를 출력합니다.
	printf("%d일자에 %c도시에서 이동가능한 도시의 정보는 아래와 같습니다.\n", day, src);
	for (char dst = 'a'; dst <= 'z'; dst++) {	// 임의의 목적지에 대하여 출력
		if (dst == src) continue;
		City* dstCity = ShortestPath(pgraphs[4 - 1], src, dst);
		if (dstCity->needTime > 0) {				//제대로 찾음 ㅎ
			printf("%c to %c : %2dh %2dm ", src, dst, dstCity->needTime / 60, dstCity->needTime % 60);
			PrintArray(dstCity->paths, dstCity->pathsNum);
		}
		//else printf("a to %c is no path\n",i);
	}
	DestroyGraph(pgraph);						//메모리해제
	for (int i = 0; i < DAYS; i++) DestroyGraph(pgraphs[i]);
	return 0;
}
void PrintArray(char* arr, int num) {
	printf("(");
	for (int i = 0; i < num; i++) {
		printf("%c", arr[i]);
		if (i != num - 1) printf(" ");
	}
	printf(")\n");
}
Graph* CreateGraph(int num) {
	Graph* pgraph = (Graph*)malloc(sizeof(Graph));
	City* head;
	if (NULL == pgraph) return NULL;
	pgraph->num = num;
	pgraph->heads = (City * *)malloc(sizeof(City*) * num * 2);
	if (NULL == pgraph->heads) return NULL;
	for (int i = 0; i < num; i++) {
		head = (City*)malloc(sizeof(City));
		if (NULL == pgraph->heads || NULL == head) return NULL;
		(pgraph->heads)[i] = head;
		head->name = 'a' + i;
		head->next = NULL;
		head->x = (rand() % 6001) - 3000;
		head->y = (rand() % 6001) - 3000;
		head->distance = -1;
		head->needTime = -1;
		head->departureTime = -1;
	}
	return pgraph;
}
void DestroyGraph(Graph* pgraph) {
	City* cur, * tmp;
	for (int i = 0; i < pgraph->num; i++) {
		cur = pgraph->heads[i];
		while (cur != NULL) {
			tmp = cur;
			cur = cur->next;
			free(tmp);
		}
	}
	free(pgraph->heads);
	free(pgraph);
	return;
}
void AddPath(Graph* pgraph, char src, char dest) {
	if (PathExists(pgraph, src, dest) == 1) {
		printf("Path exists!\n");
		return;
	}
	City* newCity1, * newCity2, * head1, * head2, * cur;
	int distance;
	head1 = pgraph->heads[dest - 'a'];
	head2 = pgraph->heads[src - 'a'];
	distance = (head1->x - head2->x) * (head1->x - head2->x) +
		(head1->y - head2->y) * (head1->y - head2->y);//blabla
	distance = (int)(sqrt(distance));

	newCity1 = (City*)malloc(sizeof(City));
	newCity2 = (City*)malloc(sizeof(City));
	if (NULL == newCity1 || NULL == newCity2) return;
	*newCity1 = *head1;
	*newCity2 = *head2;
	newCity1->next = NULL;
	newCity2->next = NULL;
	newCity1->distance = distance;
	newCity2->distance = distance;
	newCity1->needTime = -1;
	newCity2->needTime = -1;

	cur = head1;
	while (cur->next != NULL) cur = cur->next;
	cur->next = newCity2;
	cur = head2;
	while (cur->next != NULL) cur = cur->next;
	cur->next = newCity1;
}
void AddPaths(Graph* pgraph, int num) {
	City* cur;
	int src, dst;
	for (int i = 0; i < num; i++) {
		src = rand() % 26 + 'a';
		dst = rand() % 26 + 'a';
		while (src == dst) {
			dst = rand() % 26 + 'a';
		}
		AddPath(pgraph, src, dst);
	}
}
int PathExists(Graph* pgraph, char src, char dst) {
	int result = 0;
	City** heads = pgraph->heads, * cur;
	cur = heads[src - 'a'];
	while (cur->next != NULL) {
		//printf("hi");
		cur = cur->next;
		//printf("%c", cur->name);
		if (cur->name == dst) {
			result = 1;
			break;
		}
	}
	//printf("\n");
	return result;
}
Graph** CreateTimeTable(Graph* pgraph) {
	Graph** graphs = (Graph * *)malloc(sizeof(Graph*) * DAYS);
	City** heads1, ** heads2, * cur1, * cur2, * new;
	//
	for (int i = 0; i < DAYS; i++) {
		heads1 = pgraph->heads;
		heads2 = (City * *)malloc(sizeof(City*) * (pgraph->num));
		if (NULL == graphs) return;
		graphs[i] = (Graph*)malloc(sizeof(Graph));
		graphs[i]->heads = heads2;
		graphs[i]->num = pgraph->num;
		for (int j = 0; j < pgraph->num; j++) {
			cur1 = heads1[j];
			cur2 = (City*)malloc(sizeof(City));
			heads2[j] = cur2;
			*(cur2) = *(cur1);
			cur2->next = NULL;
			while (cur1->next != NULL) {
				cur1 = cur1->next;
				cur2->next = (City*)malloc(sizeof(City));
				cur2 = cur2->next;
				*(cur2) = *(cur1);
				cur2->next = NULL;
				cur2->departureTime = rand() % (24 * 60);
			}
		}
	}
	return graphs;
}
void PrintGraph(Graph* pgraph) {
	printf("City|x-cor|y-cor|distance|departureTime|arriveTime\n");
	City* cur, ** heads = pgraph->heads;
	int arrive;
	for (int i = 0; i < 26; i++) {
		printf("==================================================\n");
		cur = heads[i];
		printf("%3c |%5d|%5d|\n\n", cur->name, cur->x, cur->y);
		while (cur->next != NULL) {
			cur = cur->next;
			arrive = cur->departureTime + (int)(cur->distance / SPEED * 60);
			printf("%3c |%5d|%5d|%8d| %4dh %4dm | %3dh %3dm\n", cur->name, cur->x, cur->y,
				cur->distance, cur->departureTime / 60, cur->departureTime % 60, arrive / 60, arrive % 60);
		}
	}
	printf("==================================================\n");
	return;
}
City* ExtractMin(City** Q, int* qNum) {
	City* result = NULL;
	int min = (1 << 31) - 1, mindex = -1;
	for (int i = 0; i < *qNum; i++) {
		if (Q[i]->needTime == -1) continue;
		if (min > (Q[i]->needTime)) {
			min = Q[i]->needTime;
			mindex = i;
		}
	}
	if (mindex == -1) return NULL;
	//deleting
	for (int i = 0; i < *qNum; i++) {
		if (i == mindex) {
			result = Q[i];
			for (int j = i; j < (*qNum) - 1; j++) {
				Q[j] = Q[j + 1];
			}
			break;
		}
	}
	//printf("%c ", result->name);
	(*qNum)--;
	return result;
}
City* ShortestPath(Graph* pgraph, int src, int dst) {
	if (src == dst) {
		printf("source can't equal destination.");
		return -1;
	}
	char oldPaths[20] = { 0, };
	City** heads = pgraph->heads, ** Q, * u = NULL, * adj, * v;
	Q = (City * *)malloc(sizeof(City*) * pgraph->num);
	int qNum = pgraph->num, pathTime, oldNeedTime, oldPathsNum, arrive;
	for (int i = 0; i < pgraph->num; i++) {
		if (i == src - 'a') {
			heads[i]->needTime = 0;
			heads[i]->pathsNum = 1;
			heads[i]->paths[0] = src;
		}
		else {
			heads[i]->needTime = -1;
			heads[i]->pathsNum = 0;
		}

	}
	Queue* S = CreateQueue();
	for (int i = 0; i < pgraph->num; i++) Q[i] = heads[i];
	while (qNum > 0) {
		u = ExtractMin(Q, &qNum);
		if (u == NULL) break;
		EnQueue(S, u);
		if (u->name == dst) break;
		adj = u;
		while (adj->next != NULL) {
			adj = adj->next;
			v = heads[adj->name - 'a'];
			arrive = adj->departureTime + (int)(adj->distance / SPEED * 60);
			//pathTime += (adj->departureTime - u->needTime);// waiting Time 출발시간이 소요시간보다적은경우는 없다 아래서걸러짐
			oldNeedTime = v->needTime;
			for (int i = 0; i < v->pathsNum; i++) {
				oldPaths[i] = v->paths[i];
			}
			oldPathsNum = v->pathsNum;
			if (v->needTime == -1 || v->needTime > arrive) {
				v->needTime = arrive;
				for (int i = 0; i < u->pathsNum; i++) v->paths[i] = u->paths[i];
				v->paths[u->pathsNum] = v->name;
				v->pathsNum = u->pathsNum + 1;
			}
			if (u->needTime > adj->departureTime) {//여기에서 걸러짐
				v->needTime = oldNeedTime;
				for (int i = 0; i < v->pathsNum; i++) v->paths[i] = oldPaths[i];
				v->pathsNum = oldPathsNum;
			}
		}
	}
	free(S);
	//if (u != NULL) printf("\n");
	//for (int i = 0; i < heads[dst - 'a']->pathsNum; i++) printf("%c ", heads[dst - 'a']->paths[i]);
	//result = heads[dst - 'a']->needTime;
	return heads[dst - 'a'];
}
