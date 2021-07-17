#include <stdio.h>			// adding libraries
#include <stdlib.h>
#include<time.h>

#define N 10000				// number of people in the simulation
#define total_edge 30000
#define si short int 		// defining type
#define re 0.2				// recovery rate 
#define tr 0.5				// transmission rate
#define t_max 300			// time bound of the simulation
#define max_edge 3000		// maximum edge possible to node

si c_time = 0;				// variable to save current time
si inf_time[N];				// infection time of a person will be saved in this array
si edge_count[N] = {0};

si inf_now[t_max+1];		// it saves the number of infected people at the end of n-th day
si recovered_now[t_max+1];
si S[N],I[N+1],R[N+1];		// these arrays are to save the states of a people 
							// S->suseptable I-> infected R->recovered last element of I saves the total 
							// number of infected people
void fill(){				// this fuction sets these arrays to its initial values
	for(si i=0;i<N;i++){
		S[i] = 1;
		I[i] = 0;
		R[i] = 0;
		inf_time[i] = t_max+1;
	}
	I[N] = 0;
	R[N] = 0;
}


/* cereating graph */
struct node  	 	 	 			// it is the basic node to save the connection of the graph
{
	si adj;							// it this shows a node is connected to another node adj
	struct node* next;				// for saving more connection
};

typedef struct node* Node;			// typedefing

Node creat(si x){					// creat a node with a adjoint value
	Node temp = malloc(sizeof(struct node));
	temp->adj = x;
	return temp;
}



Node graph[N];							// global variable for the graph

void push_in_graph(si i,si position){	// it saves a connection in a in graph
	Node temp = creat(i);				// first it creat a node with adjoint value
	temp->next = graph[position];		// then it saves it to its correspondonding position
	graph[position] = temp;
}
void print(Node k){						// it print print all the connection of a node
	Node temp = k;
	while(temp!=NULL){
		printf("%d  ",temp->adj );
		temp = temp->next;
	}
}

int find_in_graph(si to_find , si j){	// it finds it a connection is present in a graph or not
	Node temp = graph[j];				// it goes to a node and check its adjecency list
	while(temp!=NULL){
		if(temp->adj == to_find)return 1;	// if found return 1 
			
		temp = temp->next;
	}
	return 0;							// else return 0
}

/* Priority queue and its functions */

struct event{							// struct for each even of the queue we will make it in heap fashion
	char effect;						// it tell if its a recovry event(R) or transmission (T)
	si node;							// the number of node on which event is operating 
	si time;							// the time of happening of the event
	struct event* left;					// links for heap
	struct event* right;
	struct event* parent;
	int size;							// size of the heap rooted at this node
};

typedef struct event* Event;

Event creat_event(char ef,si n,si t){		// this fuction allocate memory and insert given informations 
	Event temp = malloc(sizeof(struct event));
	temp->effect = ef;
	temp->node = n;
	temp->time = t;
	temp->size = 1;							
	return temp;
}

struct queue{							// global variable for queue
	Event root;
}global;

typedef struct queue* Queue;			

Queue g = &global;						// pointer for the queue

void insert_in_bottom(Event e){			// it finds the right position of the queue and insert the given node there
	Event temp = g->root;				// starts with the root
	if(temp == NULL){					// if root is empty then insert at root
		g->root = e;
		return;}
	while(temp->size > 2){				// else if the the size if greater than 2 
		temp->size++;					// in increase the parent size and move down in the heap
		if(temp->left->size == temp->right->size)temp = temp->left;		
		// if both side have same size that mean
		// heap is full so it move to left and start a new level or depth

		else{							// if left side size is in the form of 2^n then move right
			if(((temp->left->size + 1 )& temp->left->size)==0)temp = temp->right;
			else temp = temp->left;		// else move left
		}
		
	}
	if(temp->size == 2){				// if the temp size fall to 2 that mean left side have child
		temp->size++;
		temp->right = e;				// so it insert at right
		e->parent = temp;
	}
	else{
		temp->size++;					// else it insert at left
		temp->left = e;
		e->parent = temp;
	}
}

void swap(Event a, Event b){			// this function swap the information of two node
	char temp_c;						// it dose not swap the size because size is the inherent property 
	int temp_int;						// of that position so as its links its not connected to the information it
	temp_c = a->effect ;				// holds
	a->effect = b->effect;
	b->effect = temp_c;
	temp_int = a->node ;
	a->node = b->node;
	b->node = temp_int;
	temp_int = a->time;
	a->time = b->time;
	b->time = temp_int;
}

void push_up(Event e){				// it pushes up a element of a node till it finds its right position 
	Event temp = e;					//  it takes a dummy variable
	if(g->root == e)return;			
	while(temp->time < temp->parent->time){		// it swap with its parent till it find it
		swap(temp,temp->parent);				// satisfy the heap property
		temp = temp->parent;
		if(g->root == temp)return;				// if temp become root then it stops
	}
}

void insert_in_queue(Event e){					// we insert in queue with using above two function back to back
	insert_in_bottom(e);						// in word it first insert at it last position	
	push_up(e);									// then find its right position 
}

Event find_bottom(void){						// it find the last element in the queue
	Event temp = g->root;						// start at root 

	/* the idea here is if left and right child have same size the they are completely filled 
		if not 
		then if right child is in 2^n form that means left side is in partial filled condition 
		else it means right is in its partial filled condition */

	while(temp->size > 2){
		if(temp->right->size == temp->left->size)temp = temp->right;
		else if(((temp->right->size + 1 ) & temp->right->size)==0)temp = temp->left;
		else temp = temp->right;
	}
	if(temp->size == 2)temp = temp->left;	// if size hit 2 that mean only right child is present 
	return temp;							// if size is 1 that means current is the last element
}	

void push_down(Event e){					// push down the top element to suitable position 
	Event temp = e;							// takes a dummy variable
		if(temp->size==1)return;			// if it hits the bottom end the process
		else if(temp->size == 2){
			if(temp->time >= temp->left->time){		// only left child is present then
				swap(temp,temp->left);				// if parent have less priority it swaps
				return;
			}
		}
		else{
			if(temp->left->time < temp->right->time){		// else it swaps with the child which have
				if(temp->time >= temp->left->time){			// more priory 
					swap(temp,temp->left);					// if needed
					push_down(temp->left);					// recursive call for the next level of heap
				}
			}
			else{
				if(temp->time >= temp->right->time){
					swap(temp,temp->right);
					push_down(temp->right);
				}
			}

		}
}

Event take_out_min(void){									// it takes out the maximum priority element 
															// and also maintain the queue
	Event min = creat_event(g->root->effect,g->root->node,g->root->time);		// coping the data to a new node
	Event last = find_bottom();				// find the last elemet of the queue
	if(last == g->root){					// if last and the root is same make the queue empty 
		g->root = NULL;						
		free(last);
		return min;
	}
	swap(last,g->root);						// else swap values of last and the root
	last = last->parent;
	if(last->size==3){						// process of deleting the the last element
		Event t_del = last->right;
		last->right =NULL;
		free(t_del);
	} else {								// if the parent have size 3 delete rigth child
		Event t_del = last->left;			// else left child 
		last->left =NULL;
		free(t_del);
	}	
	while(last != g->root){					// decrease the size of all the parents by 1
		last->size--;						// as one child of them has been deleted
		last = last->parent;
	}
	g->root->size--;						// including root
	push_down(g->root);						// pushing down to its right position 
	return min;								// returning the min time event
}

void print_queue(Event e){					// a pre print of a heap or heap
	if(e != NULL){
		printf("%d %d %d %c\n",e->size,e->time ,e->node,e->effect);
		print_queue(e->left);
		print_queue(e->right);
	}
}

/* Event processing */

si action_time(si current_time, float rate){	// it gives out a value from exponential variate
	si success = 100*rate;						// if the random event come is success region 
	while(rand()%100 >= success){				// it gives out time at which success happened
		current_time++;
	}
	return current_time;
}

void process_event(Event e){					// self explanetory 
	if(e->effect == 'R'){						// if e is a recovry event 
		if(I[e->node]){							// check if the node is still infected or not
			I[e->node] = 0;						// if yes then make the node recovered
			I[N]--;
			R[N]++;
			R[e->node] = 1;
		}
	}
	else{										// if it is a transmission event
		if(S[e->node]){							// and if the node is suceptible 
			I[e->node] =1;						// make it infected 
			S[e->node] =0;						
			I[N]++;								// increasing the count of infected people
			si r_time = action_time(e->time+1,re);		// finding its recovery time
			Node temp = graph[e->node];					// now finding transmission time for each of its neighbor
			while(temp != NULL){						
				si t_time = action_time(e->time+1,tr);	// finding transmission time 

				// if transmission is happeing before source node's recovery and before t_max and 
				// previously assingened transmission time is greater than this transmission

				if(t_time <= r_time && t_time <= t_max && t_time < inf_time[temp->adj]){	
					insert_in_queue(creat_event('T',temp->adj,t_time));		// enqueue a transmission event
				}
				temp = temp->next;						// moving to next neighbor
			}
			if(r_time < t_max)							// if recovery is happeing before t_max add a recobvery event
				insert_in_queue(creat_event('R',e->node,r_time));
		}
	}


}



/*sumalating in main*/

int main(){

	fill();								// discussed before
	srand(time(NULL));					// seeding the rand function
	si a ,b;

	for(int i =0;i<total_edge;i++){			// add edges to the graph
		a = rand()%N;						//	select two node randomly
		b = rand()%N;
		if(edge_count[a]>max_edge || edge_count[b]>max_edge){i--;continue;} // checking if its cross the edge limit
		if(a==b){i--;continue;}						// if they arent same 
		else if(find_in_graph(a,b)){i--;continue;}	// and the edge is not pre existing 
		push_in_graph(a,b);							// add the undirected edge to the graph
		push_in_graph(b,a);
		edge_count[a]++;
		edge_count[b]++;
	}

	for(si i=0;i<=t_max;i++)inf_now[i]=-1,recovered_now[i]=-1;	// it store how many people are infected in a given day
	insert_in_queue(creat_event('T',rand()%N,0));				// making one person infected initially to start the simulation
	insert_in_queue(creat_event('T',rand()%N,0));
	/*if we have random number of infected at start
		we can simply run this loop 
		int start = rand()%N
		for(int i=0; i<start;i++){
			insert_in_queue(creat_event('T',rand()%N,0));
		}
	*/	


	Event now = take_out_min();					// taking out from queue and processing the event
	process_event(now);
	while(g->root != NULL){						// while queue is not empty
		now = take_out_min();					// keep processing 
		recovered_now[now->time] = R[N];
		inf_now[now->time] = I[N];				// storing daily infected number
		process_event(now);
	}
	inf_now[now->time] = I[N];
	recovered_now[now->time] = R[N];
	for(si i=0;i<=t_max;i++){
		if(inf_now[i]==-1)inf_now[i]=inf_now[i-1];	// filling the days where there werent any new transmision
		if(recovered_now[i]==-1)recovered_now[i]=recovered_now[i-1];
		}
	printf("Day   suceptible  infected  recovered\n");
	FILE* f = fopen("plot.txt","w");
	for(si i=0;i<=t_max;i++){
		printf("%d,%10d,%8d,%9d\n",i,N-inf_now[i]-recovered_now[i],inf_now[i],recovered_now[i]);// printing the data generated
		fprintf(f,"%d%10d%8d%9d\n",i,N-inf_now[i]-recovered_now[i],inf_now[i],recovered_now[i]);// printing in file
		}
		

}