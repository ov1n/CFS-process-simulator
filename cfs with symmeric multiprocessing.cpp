#include <iostream>
#include<vector>
#include<fstream>
#include<math.h>

using namespace std;

// Data structure that represents a process in the tree
struct prc {
	int total_runtime; // holds the key
	int weight;	//the weight of the process
	int p_id;		//process id of current process
	float curr_time; //holds current time
	float wait_runtime; //Per process runtime
	prc *parent; // pointer to the parent
	prc *left; // pointer to left child
	prc *right; // pointer to right child
	int color; // 1 is red, 0 is Black
};
typedef prc *prcPtr;

struct processor{
	
	int proc_id;
	int capacity;
	
};
typedef processor *processorPtr;


// An RBTree is made to implement the operations for processes
class RBTree {
	
private:
	prcPtr root;
	prcPtr TNULL;
	// fix the rb tree modified by the delete operation
	void fixDelete(prcPtr x) {
		prcPtr s;
		while (x != root && x->color == 0) {
			if (x == x->parent->left) {			//Check if it's the left tree
				s = x->parent->right;
				if (s->color == 1) {
					// case 1 of Double black 
					s->color = 0;
					x->parent->color = 1;
					leftRotate(x->parent);
					s = x->parent->right;
				}

				if (s->left->color == 0 && s->right->color == 0) {
					// case 3.2
					s->color = 1;
					x = x->parent;
				} else {
					if (s->right->color == 0) {
						// case 3.3
						s->left->color = 0;
						s->color = 1;
						rightRotate(s);
						s = x->parent->right;
					} 

					// case 3.4
					s->color = x->parent->color;
					x->parent->color = 0;
					s->right->color = 0;
					leftRotate(x->parent);
					x = root;
				}
			} else {
				s = x->parent->left;
				if (s->color == 1) {
					// case 3.1
					s->color = 0;
					x->parent->color = 1;
					rightRotate(x->parent);
					s = x->parent->left;
				}

				if (s->right->color == 0 && s->right->color == 0) {
					// case 3.2
					s->color = 1;
					x = x->parent;
				} else {
					if (s->left->color == 0) {
						// case 3.3
						s->right->color = 0;
						s->color = 1;
						leftRotate(s);
						s = x->parent->left;
					} 

					// case 3.4
					s->color = x->parent->color;
					x->parent->color = 0;
					s->left->color = 0;
					rightRotate(x->parent);
					x = root;
				}
			} 
		}
		x->color = 0;
	}


	void rbTransplant(prcPtr u, prcPtr v){
		if (u->parent == NULL) {
			root = v;
		} else if (u == u->parent->left){
			u->parent->left = v;
		} else {
			u->parent->right = v;
		}
		v->parent = u->parent;
	}

	prc *deleteprcHelper(prcPtr p) {
		
		prcPtr z = p;
		prcPtr x, y;

		y = z;
		int y_original_color = y->color;
		if (z->left == TNULL) {
			x = z->right;
			rbTransplant(z, z->right);
		} else if (z->right == TNULL) {
			x = z->left;
			rbTransplant(z, z->left);
		} else {
			y = minimum(z->right);
			y_original_color = y->color;
			x = y->right;
			if (y->parent == z) {
				x->parent = y;
			} else {
				rbTransplant(y, y->right);
				y->right = z->right;
				y->right->parent = y;
			}

			rbTransplant(z, y);
			y->left = z->left;
			y->left->parent = y;
			y->color = z->color;
		}
		
		if (y_original_color == 0){		//Only if black
			fixDelete(x);
		}
		//delete z;
		if(pro_cycle>=total_duration){
			fair_clock++;						//If the process quantum is very large

		}
		else if(p->total_runtime-p->curr_time>slice){
			fair_clock+=slice;					//if remaining time is greater than a slice

		}
		else{
			fair_clock+=p->total_runtime-p->curr_time;	//if remaining time of process is less than a slice

		}
		
		//cout<<"fair_clock"<<fair_clock<<endl;
		
		return z;
	}
	
	// fix the red-black tree
	void fixInsert(prcPtr k){
		prcPtr u;
		while (k->parent->color == 1) {
			if (k->parent == k->parent->parent->right) {
				u = k->parent->parent->left; // uncle
				if (u->color == 1) {
					// case 3.1
					u->color = 0;
					k->parent->color = 0;
					k->parent->parent->color = 1;
					k = k->parent->parent;
				} else {
					if (k == k->parent->left) {
						// case 3.2.2
						k = k->parent;
						rightRotate(k);
					}
					// case 3.2.1
					k->parent->color = 0;
					k->parent->parent->color = 1;
					leftRotate(k->parent->parent);
				}
			} else {
				u = k->parent->parent->right; // uncle

				if (u->color == 1) {
					// mirror case 3.1
					u->color = 0;
					k->parent->color = 0;
					k->parent->parent->color = 1;
					k = k->parent->parent;	
				} else {
					if (k == k->parent->right) {
						// mirror case 3.2.2
						k = k->parent;
						leftRotate(k);
					}
					// mirror case 3.2.1
					k->parent->color = 0;
					k->parent->parent->color = 1;
					rightRotate(k->parent->parent);
				}
			}
			if (k == root) {
				break;
			}
		}
		root->color = 0;
	}

public:
	RBTree() {
		process_id=-1;
		processor_id=-1;
		fair_clock=0;		//Initialize clock
		round_robin=0;		//Used for symmetrical multiprocessing
		TNULL = new prc;
		TNULL->color = 0;
		TNULL->left = NULL;
		TNULL->right = NULL;
		root = TNULL;
	}
	
	float fair_clock;						//System wide run CPU clock
	int process_id;							//Variable to give each process a unique ID
	int process_count;						//no of processes, count decrements each time a process exits
	
	int processor_id;							//Variable to give each processor a unique ID
	int processor_count;						//no of processors
	
	int round_robin; 		//USed for context switching in symmetrical multiprocessing
	
	float pro_cycle;							//The cycle of a the process cycle
	float total_duration;						//Total duration of all processes
	float total_priorities;					//Total priorities of all processes
	float slice;							//The time slice for each process run to be incremented
	
	
	vector<prc *> table;					//vector containing pointers to all processes
	vector<processorPtr> cpu;					//vector containing pointers to all processes
	
	int enter_processes(){
		cout<<"COMPLETE FAIR SCHEDULER SYSTEM"<<endl;
		int n;
		cout<<"Enter number of processes to be inserted: "<<endl;
		cin>>n;
		cout<<"Enter time quantum of processor: "<<endl;
		cin>>pro_cycle;
		
		float scheduler[n][2];											//Create an array to contain the processes
		cout<<endl<<"Enter priority and time quantum of processes to be inserted: "<<endl;
		cout<<"lesser the number higher the priority(>0)"<<endl;
		float getPriority;														//Temporary variable to store time quantum
		float getBursttime;
		total_duration=0;
		total_priorities=0;
		for(int i=0;i<n;i++){
			cin>>getPriority;
			cin>>getBursttime;
			//insert time quantums into an array;
			scheduler[i][0]=getBursttime;				//Priority
			scheduler[i][1]=getPriority;				//Time duration
			total_duration+=scheduler[i][0];			//Calculate total durations
			total_priorities+=scheduler[i][1];			//Calculate total priorities
			
		}
		
		for(int i=0;i<n;i++){
			//cout<<"I "<<i;
			//Make initial RBT
			insert(scheduler[i][0],scheduler[i][1]);
			
		}
		
		if(total_duration<=pro_cycle){
			cout<<"CPU BURST-TIME IS GREATER THAN OR EQUAL TO TOTAL RUNTIME OF ALL PROCESSES"<<endl;
		}else{
			
			//cout<<"pro_cycle"<<pro_cycle<<endl;
			//cout<<"total priorities"<<total_priorities<<endl;
			//cout<<"total time"<<total_duration<<endl;
			slice=pro_cycle/total_priorities;			//total_priorities=sum of all priorities
			//printf("%f\n",slice);
			//cout<<"slice"<<slice<<endl;
		}
		
		return n;					//Returns n to be used later for deletion and controling the scheduling process
	}
	
	void extend_symmetrical(){
		
		
	}
	
	void finalize(){
		
		ofstream outfile;
	
		outfile.open("process_report.txt",ios_base::app);
		cout<<"FAIR CLOCK TIME = "<<fair_clock<<endl;
		outfile<<"FAIR CLOCK TIME = "<<fair_clock<<endl;
		outfile<<"-----------------"<<endl<<endl;
	}
	
	void show_entered(){
		
		ofstream outfile;
		outfile.open("process_report.txt",std::ios_base::app);
		
		cout<<"FAIR CLOCK TIME AT START = "<<fair_clock<<endl;
		outfile<<"FAIR CLOCK TIME AT START = "<<fair_clock<<endl;
		
		cout<<"INSERTED PROCESSES :"<<endl;
		outfile<<"INSERTED PROCESSES :"<<endl;
		
		
		for(int i=0;i<table.size();i++){
			cout<<"PROCESS ID= "<<table[i]->p_id<<" DURATION = "<<table[i]->total_runtime<<" PRIORITY ="<<table[i]->weight<<endl;
			outfile<<"PROCESS ID= "<<table[i]->p_id<<" DURATION = "<<table[i]->total_runtime<<" PRIORITY ="<<table[i]->weight<<endl;
		}
		
		outfile<<endl;
	}
	// find the leftmost process
	prcPtr minimum(prcPtr p) {
		while (p->left != TNULL) {
			p = p->left;
		}
		return p;
	}

	// rotate left 
	void leftRotate(prcPtr x) {
		prcPtr y = x->right;
		x->right = y->left;
		if (y->left != TNULL) {
			y->left->parent = x;
		}
		y->parent = x->parent;
		if (x->parent == NULL) {
			this->root = y;
		} else if (x == x->parent->left) {
			x->parent->left = y;
		} else {
			x->parent->right = y;
		}
		y->left = x;
		x->parent = y;
	}

	// rotate right
	void rightRotate(prcPtr x) {
		prcPtr y = x->left;
		x->left = y->right;
		if (y->right != TNULL) {
			y->right->parent = x;
		}
		y->parent = x->parent;
		if (x->parent == NULL) {
			this->root = y;
		} else if (x == x->parent->right) {
			x->parent->right = y;
		} else {
			x->parent->left = y;
		}
		y->right = x;
		x->parent = y;
	}

	// insert the key to the tree in its appropriate position
	// and fix the tree
	void insert(int key,int weight) {
		// Ordinary Binary Search Insertion
		process_id++;
		prcPtr p = new prc;
		p->parent = NULL;
		p->total_runtime = key;
		p->weight = weight;
		p->p_id = process_id;
		p->curr_time=0;
		p->wait_runtime=weight;
		p->left = TNULL;
		p->right = TNULL;
		p->color = 1; // new p of an RBT must be red
		
		//Insert into vector
		table.push_back(p);

		prcPtr y = NULL;
		prcPtr x = this->root;

		while (x != TNULL) {
			y = x;
			if (p->wait_runtime < x->wait_runtime) {
				x = x->left;
			} else {
				x = x->right;
			}
		}

		// y is parent of x
		p->parent = y;
		if (y == NULL) {
			root = p;
		} else if (p->wait_runtime < y->wait_runtime) {
			y->left = p;
		} else {
			y->right = p;
		}

		// if new p is a root p, simply return
		if (p->parent == NULL){
			p->color = 0;
			return;
		}

		// if the grandparent is null, simply return
		if (p->parent->parent == NULL) {
			return;
		}
		
		// Fix the tree
		fixInsert(p);
	}
	
	int insert(prc *p) {
		
		p->parent = NULL;
		//p->total_runtime = 1; // CHANGE TEST VALUE
		//IF CONDITION FOR CURRENT TIME

		if(pro_cycle>=total_duration){
			p->curr_time++;
		}else if(p->total_runtime-p->curr_time<slice){
			p->curr_time=p->total_runtime;
		}
		else{
			p->curr_time+=slice;
		}
		p->wait_runtime+=p->weight;
		p->left = TNULL;
		p->right = TNULL;
		p->color = 1; // new p must be red
		
		//cout<<"process time"<<p->curr_time<<endl;
		//cout<<"PROCESS COUNT"<<process_count<<endl;
		ofstream outfile;
		outfile.open("process_report.txt",std::ios_base::app);		//2nd parameter for appending
		
		if(p->curr_time>=p->total_runtime){
			cout<<"\tPROCESS WITH ID "<<p->p_id<<" COMPLETED"<<endl;
			outfile<<"\tPROCESS WITH ID "<<p->p_id<<" COMPLETED"<<endl;
			process_count--;
			
			slice=pro_cycle/(total_priorities-(4-p->weight));
			if(process_count<=0){
				cout<<endl<<"CONTEXT SWITCH OVERHEAD= "<<fair_clock-total_duration<<endl;
				outfile<<endl<<"CONTEXT SWITCH OVERHEAD= "<<fair_clock-total_duration<<endl;
				
				cout<<endl<<"ALL PROCESSES SCHEDULED AND COMPLETED"<<endl;
				outfile<<endl<<"ALL PROCESSES SCHEDULED AND COMPLETED"<<endl;
				outfile<<endl;
				return 0;
			}else{
				return 1;
			}	
		}

		prcPtr y = NULL;
		prcPtr x = this->root;

		while (x != TNULL) {
			y = x;
			if (p->wait_runtime < x->wait_runtime) {
				x = x->left;
			} else {
				x = x->right;
			}
		}

		// y is parent of x
		p->parent = y;
		if (y == NULL) {
			root = p;
		} else if (p->wait_runtime < y->wait_runtime) {
			y->left = p;
		} else {
			y->right = p;
		}

		// if new p is a root p, simply return
		if (p->parent == NULL){
			p->color = 0;
			return 1;
		}

		// if the grandparent is null, simply return
		if (p->parent->parent == NULL) {
			return 1;
		}

		// Fix the tree
		fixInsert(p);
	}

	prcPtr getRoot(){
		return this->root;
	}

	// delete the p from the tree
	prc *deleteprc() {
		prc *x=minimum(root);					//Get the leftmost node of the RBT 
		//cout<<"total_runtime of min"<<x->total_runtime<<endl;
		prc *y=deleteprcHelper(x);
		//cout<<"total_runtime of deleted"<<y->total_runtime<<endl;
		
		ofstream outfile;
		outfile.open("process_report.txt",std::ios_base::app);
		
		cout<<endl<<"T="<<fair_clock<<"\t"<<"RUNNING PROCESS WITH ID "<<y->p_id<<endl;
		outfile<<"T="<<fair_clock<<"\t"<<"RUNNING PROCESS WITH ID "<<y->p_id<<endl;
		return y;
	}
	
	prc *deleteprc_symm() {
		prc *x=minimum(root);					//Get the leftmost node of the RBT 
		//cout<<"total_runtime of min"<<x->total_runtime<<endl;
		prc *y=deleteprcHelper(x);
		//cout<<"total_runtime of deleted"<<y->total_runtime<<endl;
		
		ofstream outfile;
		outfile.open("process_report.txt",std::ios_base::app);
		
		cout<<endl<<"T="<<fair_clock<<"\t"<<"RUNNING PROCESS WITH ID "<<y->p_id<<" ON PROCESSOR "<<cpu[round_robin]->proc_id<<endl;
		outfile<<"T="<<fair_clock<<"\t"<<"RUNNING PROCESS WITH ID "<<y->p_id<<" ON PROCESSOR "<<cpu[round_robin]->proc_id<<endl;
		
		round_robin=(round_robin+1)%processor_count;
		//round_robin++;
		
		return y;
	}
	
	void create_processors(int capacity){			//Make necessary processors:
		
		processor_id++;
		processorPtr q= new processor;
		q->proc_id=processor_id;
		q->capacity=capacity;
		/*DELETE
		
		cout<<"prc id"<<q->proc_id<<endl;
		cout<<"prc capacity"<<q->capacity<<endl;
		*/
		
		cpu.push_back(q);			//Insert processor into vector
	}

};

int main() {
	RBTree bst;
	
	//FILE HANDLING
	ofstream outfile;
	outfile.open("process_report.txt");
	
	bst.process_count=bst.enter_processes();			//Enter time quantums of processes seperated by space returns total no of processes entered

	cout<<"CFS SCHEDULING SYSTEM :"<<endl;
	cout<<"-----------------------"<<endl;
	outfile<<"CFS SCHEDULING SYSTEM :"<<endl;
	outfile<<"-----------------------"<<endl;
	
	bst.show_entered();
	
	int flag=1;											//Flag to keep scheduler running, controlled by insert function inside
	while(flag){
		
		prc *x=bst.deleteprc();
		
		flag=bst.insert(x);
	}
	
	bst.finalize();
	
	cout<<"-----------------------------------------------------------------------"<<endl;
	cout<<"Do you wish to extend the system to handle symmetrical multiprocessing?"<<endl<<"enter Y or N"<<endl;
	char input;
	cin>>input;
	if(input=='Y'){			//If Y, just get content from initial RBT and make a new one for extend, skipped encapsulation due to time constraints
		
		RBTree bst_symm;	//New RBT
		bst_symm.pro_cycle=bst.pro_cycle;
		bst_symm.process_count=bst.table.size();		//Get size
		for(int i=0;i<bst.table.size();i++){
			
			//cout<<"FAIR CLOCK "<<bst_symm.fair_clock<<endl;
			bst_symm.insert(bst.table[i]->total_runtime,bst.table[i]->weight);
			bst_symm.total_duration+=bst_symm.table[i]->total_runtime;			//Calculate total durations
			bst_symm.total_priorities+=bst_symm.table[i]->weight;			//Calculate total priorities
		}
		
		if(bst_symm.total_duration>bst_symm.pro_cycle){
			
			bst_symm.slice=bst_symm.pro_cycle/bst_symm.total_priorities;
			//cout<<"slice"<<slice<<endl;
		}
		
		outfile.open("process_report.txt",std::ios_base::app);
		cout<<"CFS SCHEDULING SYSTEM WITH SYMMETRIC MULTIPROCESSING : "<<endl;
		cout<<"-----------------------"<<endl;
		outfile<<"CFS SCHEDULING SYSTEM WITH SYMMETRIC MULTIPROCESSING : "<<endl;
		outfile<<"-----------------------"<<endl;
		bst_symm.show_entered();
		
		cout<<"Enter number of processors in CPU (Limit to either 1,2 4, or 8): "<<endl;
		cin>>bst_symm.processor_count;
		for(int k=0;k<bst_symm.processor_count;k++){
			
			bst_symm.create_processors(1); 			//Capacity not taken into consideration as Symmetric
			
		}
		
		flag=1;											//Flag to keep scheduler running, controlled by insert function inside
		int index=0;									//Index used to traverse through the vector
		while(flag){
			
			prc *y=bst_symm.deleteprc_symm();				//Same as deleteprc function but modified to cope symmetric multiprocessing
			
			flag=bst_symm.insert(y);
		}
		
		
	}
	//bst.extend_symmetrical()
	outfile.close();
	
	return 0;
}
