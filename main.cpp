#include <iostream>
#include<vector>
#include<fstream>
#include<math.h>

using namespace std;

// Data structure that represents a process in the tree
struct prc {
	int total_runtime; 				// holds the key
	int weight;						//the weight of the process
	int p_id;						//process id of current process
	float curr_time; 				//holds current time
	float wait_runtime; 			//Per process runtime
	float start_time;
	float end_time;
	prc *parent; 					// pointer to the parent
	prc *left; 						// pointer to left child
	prc *right; 					// pointer to right child
	int color; 						// 1 is red, 0 is Black
};
typedef prc *prc_ptr;				//Create a default pointer for process

struct processor{
	
	int proc_id;					//Unique processor ID given for processor
	int capacity;					//The processing power of the processor only relevant in Asymmetric
	
};
typedef processor *processor_ptr;	//Create a default pointer for processor


// An red_black is made to implement the operations for processes
class red_black {
	
private:
	prc_ptr root;							//Pointer for root
	prc_ptr terminal_node;					//Pointer for nil nodes

	//Delete fixes,utility functions to handle deletion 
	void fix_delete(prc_ptr x);
	void rb_interchange(prc_ptr u, prc_ptr v);
	prc *deleteprc_utility(prc_ptr p);
	
	// fix insertion utility function
	void insert_fix(prc_ptr k);

public:
	red_black() {
		process_id=-1;						//Starting ID given to 1st process,increments for each process inserted
		processor_id=-1;					//Starting ID given to 1st processor,increments for each processor(max of 8 processors)
		processor_count=1;
		fair_clock=0;						//Initialize clock
		round_robin=0;						//Used for symmetrical multiprocessing
		terminal_node = new prc;			//terminal node
		terminal_node->color = 0;
		terminal_node->left = NULL;
		terminal_node->right = NULL;
		root = terminal_node;				//Set terminal node as root during start
	}
	
	float fair_clock;						//System wide run CPU clock
	int process_id;							//Variable to give each process a unique ID
	int process_count;						//no of processes, count decrements each time a process exits
	
	int processor_id;						//Variable to give each processor a unique ID
	int processor_count;					//no of processors
	
	int round_robin; 						//Used for context switching in symmetrical multiprocessing
	
	float pro_cycle;						//The cycle of a the process cycle
	float total_duration;					//Total duration of all processes
	float total_priorities;					//Total priorities of all processes
	float slice;							//The time slice for each process run to be incremented
	
	processor_ptr current_exec;				//Pointer that keeps track of current processor that is executing 
	vector<prc *> table;					//vector containing pointers to all processes
	vector<processor_ptr> cpu;				//vector containing pointers to all processes
	
	int enter_processes();					//Function used to insert processes initially
	void create_processors(int capacity);	//Function that creates processors for given input capacity(MAX OF 8)
	
	void show_entered();					//Function to show entered processes with details such as priority and id
	void finalize();						//Function showing the fair clock at the end
	
	prc_ptr minimum(prc_ptr p) ;				//Find the leftmost-child process
 
	void rotate_left(prc_ptr x);				// rotate left
	void rotate_right(prc_ptr x);			// rotate right

	void insert(int key,int weight);		//Function which inserts all processes at start taking parameters total runtime and priority
	int insert(prc *p);						//Function to insert executed processes back to the tree just taking a pointer

	prc_ptr getRoot(){return this->root;}	//Returns root

	prc *deleteprc();						//Function that deletes a process from the tree,adds runtime to make sure process runs in CFS order
	prc *deleteprc_symm();					//Same as deleteprc but used for symmetrical multiprocessing, adds processor ID at end 
};

int main() {
	red_black obj;
	
	//FILE HANDLING
	ofstream outfile;
	outfile.open("process_report.txt");
	
	obj.process_count=obj.enter_processes();			//Enter time quantums of processes seperated by space returns total no of processes entered

	cout<<"CFS SCHEDULING SYSTEM :"<<endl;
	cout<<"-----------------------"<<endl;
	outfile<<"CFS SCHEDULING SYSTEM :"<<endl;
	outfile<<"-----------------------"<<endl;
	
	obj.show_entered();
	
	int flag=1;											//Flag to keep scheduler running, controlled by insert function inside
	while(flag){
		
		prc *x=obj.deleteprc();
		
		flag=obj.insert(x);
	}
	
	obj.finalize();
	
	cout<<"-----------------------------------------------------------------------"<<endl;
	cout<<"Do you wish to extend the system to handle symmetrical multiprocessing?"<<endl<<"enter Y or N"<<endl;
	char input;
	cin>>input;
	if(input=='Y'){			//If Y, just get content from initial RBT and make a new one for extend, skipped encapsulation due to time constraints
		
		red_black obj_symm;	//New RBT
		obj_symm.pro_cycle=obj.pro_cycle;
		obj_symm.process_count=obj.table.size();		//Get size
		for(int i=0;i<obj.table.size();i++){
			
			//cout<<"FAIR CLOCK "<<obj_symm.fair_clock<<endl;
			obj_symm.insert(obj.table[i]->total_runtime,obj.table[i]->weight);
			obj_symm.total_duration+=obj_symm.table[i]->total_runtime;			//Calculate total durations
			obj_symm.total_priorities+=obj_symm.table[i]->weight;			//Calculate total priorities
		}
		
		if(obj_symm.total_duration>obj_symm.pro_cycle){
			
			obj_symm.slice=obj_symm.pro_cycle/obj_symm.total_priorities;
			//cout<<"slice"<<slice<<endl;
		}
		ofstream outfile2
		;
		outfile2.open("process_report.txt",std::ios_base::app);
		cout<<"CFS SCHEDULING SYSTEM WITH SYMMETRIC MULTIPROCESSING : "<<endl;
		cout<<"-----------------------"<<endl;
		outfile2<<"CFS SCHEDULING SYSTEM WITH SYMMETRIC MULTIPROCESSING : "<<endl;
		outfile2<<"-----------------------"<<endl;
		obj_symm.show_entered();
		
		cout<<"Enter number of processors in CPU (Limit to either 2,4, or 8): "<<endl;
		cin>>obj_symm.processor_count;
		for(int k=0;k<obj_symm.processor_count;k++){
			
			obj_symm.create_processors(1); 				//Capacity not taken into consideration as Symmetric
			
		}
		
		flag=1;											//Flag to keep scheduler running, controlled by insert function inside
		int index=0;									//Index used to traverse through the vector
		while(flag){
			
			prc *y=obj_symm.deleteprc_symm();			//Same as deleteprc function but modified to cope symmetric multiprocessing
			
			flag=obj_symm.insert(y);
		}
		outfile2.close();
		obj_symm.finalize();	
	}
	cout<<"-----------------------"<<endl;
	
	cout<<"Thank you for using the system"<<endl;
	
	outfile.close();
	
	return 0;
}

//Delete functions
prc* red_black::deleteprc(){
	prc *x=minimum(root);					//Get the leftmost node of the RBT 
	//cout<<"total_runtime of min"<<x->total_runtime<<endl;
	prc *y=deleteprc_utility(x);
	//cout<<"total_runtime of deleted"<<y->total_runtime<<endl;
	
	ofstream outfile;
	outfile.open("process_report.txt",std::ios_base::app);
	if(y->start_time==0){
		y->start_time=fair_clock;
	}
	cout<<endl<<"T="<<fair_clock<<"\t"<<"RAN PROCESS WITH ID "<<y->p_id<<" NOW CONTEXT SWITCHING"<<endl;
	outfile<<"T="<<fair_clock<<"\t"<<"RAN PROCESS WITH ID "<<y->p_id<<" NOW CONTEXT SWITCHING"<<endl;
	return y;
}

prc* red_black::deleteprc_symm(){
	
	prc *x=minimum(root);					//Get the leftmost node of the RBT 
	//cout<<"total_runtime of min"<<x->total_runtime<<endl;
	current_exec=cpu[round_robin];
	prc *y=deleteprc_utility(x);
	//cout<<"total_runtime of deleted"<<y->total_runtime<<endl;
	
	ofstream outfile;
	outfile.open("process_report.txt",std::ios_base::app);
	
	if(y->start_time==0){
		y->start_time=fair_clock;
	}
	
	cout<<endl<<"T="<<fair_clock<<"\t"<<"RAN PROCESS WITH ID "<<y->p_id<<" ON PROCESSOR "<<cpu[round_robin]->proc_id<<" NOW CONTEXT SWITCHING"<<endl;
	outfile<<"T="<<fair_clock<<"\t"<<"RAN PROCESS WITH ID "<<y->p_id<<" ON PROCESSOR "<<cpu[round_robin]->proc_id<<" NOW CONTEXT SWITCHING"<<endl;
	
	round_robin=(round_robin+1)%processor_count;
	//round_robin++;
	
	return y;
}

prc* red_black::deleteprc_utility(prc_ptr p) {

	prc_ptr z = p;
	prc_ptr x, y;

	y = z;
	int y_original_color = y->color;
	if (z->left == terminal_node) {
		x = z->right;
		rb_interchange(z, z->right);
	} else if (z->right == terminal_node) {
		x = z->left;
		rb_interchange(z, z->left);
	} else {
		y = minimum(z->right);
		y_original_color = y->color;
		x = y->right;
		if (y->parent == z) {
			x->parent = y;
		} else {
			rb_interchange(y, y->right);
			y->right = z->right;
			y->right->parent = y;
		}

		rb_interchange(z, y);
		y->left = z->left;
		y->left->parent = y;
		y->color = z->color;
	}
	
	if (y_original_color == 0){					//Only if black
		fix_delete(x);
	}
	//delete z;
	if(pro_cycle>=total_duration){
		if(processor_count>1){
			if(current_exec->proc_id%processor_count==0){
				fair_clock++;
			}
		}else{
			
			fair_clock++;						//If the process quantum is very large
		}
		

	}
	else if(p->total_runtime-p->curr_time>slice){
		if(processor_count>1){
			if(current_exec->proc_id%processor_count==0){
				fair_clock+=slice;
			}
		}else{
			
			fair_clock+=slice;					//if remaining time is greater than a slice
		}						

	}
	else{
		if(processor_count>1){
			if(current_exec->proc_id%processor_count==0){
				fair_clock+=p->total_runtime-p->curr_time;	//if remaining time of process is less than a slice
			}
		}else{
			
			fair_clock+=p->total_runtime-p->curr_time;	//if remaining time of process is less than a slice
		}
		
	}
	
	//cout<<"fair_clock"<<fair_clock<<endl;
	
	return z;
}

void red_black::fix_delete(prc_ptr x) {
		prc_ptr temp;
		while (x != root && x->color == 0) {
			if (x==x->parent->left) {			//Check if it's the left tree
				temp=x->parent->right;
				if (temp->color == 1) {
					// case 1 of Double black 
					temp->color = 0;
					x->parent->color = 1;
					rotate_left(x->parent);
					temp=x->parent->right;
				}

				if (temp->left->color == 0 && temp->right->color == 0) {
					// case 2 of double black
					temp->color = 1;
					x = x->parent;
				} else {
					if (temp->right->color == 0) {
						// case 3 of double black
						temp->left->color = 0;
						temp->color = 1;
						rotate_right(temp);
						temp = x->parent->right;
					} 

					// case 4 of double black
					temp->color = x->parent->color;
					x->parent->color = 0;
					temp->right->color = 0;
					rotate_left(x->parent);
					x = root;
				}
			} else {
				temp = x->parent->left;
				if (temp->color == 1) {
					// Double black case 1
					temp->color = 0;
					x->parent->color = 1;
					rotate_right(x->parent);
					temp = x->parent->left;
				}

				if (temp->right->color == 0 && temp->right->color == 0) {
					//Double black case 2
					temp->color = 1;
					x = x->parent;
				} else {
					if (temp->left->color == 0) {
						//Double black case 3
						temp->right->color = 0;
						temp->color = 1;
						rotate_left(temp);
						temp=x->parent->left;
					} 

					//Double black case 4
					temp->color = x->parent->color;
					x->parent->color = 0;
					temp->left->color = 0;
					rotate_right(x->parent);
					x=root;
				}
			} 
		}
		x->color = 0;
	}
	
void red_black::rb_interchange(prc_ptr u, prc_ptr v){
	if (u->parent == NULL) {
		root = v;
	} else if (u == u->parent->left){
		u->parent->left = v;
	} else {
		u->parent->right = v;
	}
	v->parent = u->parent;
}

//Insert functions
void red_black::insert(int key,int weight) {

	process_id++;
	prc_ptr p = new prc;
	p->parent = NULL;
	p->total_runtime = key;
	p->weight = weight;
	p->p_id = process_id;
	p->curr_time=0;
	p->wait_runtime=weight;
	p->start_time=0;
	p->left = terminal_node;
	p->right = terminal_node;
	p->color = 1; // new p of an RBT must be red
	
	//Insert into vector
	table.push_back(p);

	prc_ptr y = NULL;
	prc_ptr x = this->root;

	while (x != terminal_node) {
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
	insert_fix(p);
}

int red_black::insert(prc *p) {
	
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
	p->left = terminal_node;
	p->right = terminal_node;
	p->color = 1; // new p must be red
	
	//cout<<"process time"<<p->curr_time<<endl;
	//cout<<"PROCESS COUNT"<<process_count<<endl;
	ofstream outfile;
	outfile.open("process_report.txt",std::ios_base::app);		//2nd parameter for appending
	
	if(p->curr_time>=p->total_runtime){
		p->end_time=fair_clock;
		cout<<"\tPROCESS WITH ID "<<p->p_id<<" COMPLETED"<<endl;
		outfile<<"\tPROCESS WITH ID "<<p->p_id<<" COMPLETED"<<endl;
		process_count--;
		
		slice=pro_cycle/(total_priorities-(4-p->weight));
		if(process_count<=0){
			cout<<endl<<"CONTEXT SWITCH OVERHEAD= "<<(unsigned)fair_clock-total_duration<<endl;
			outfile<<endl<<"CONTEXT SWITCH OVERHEAD= "<<(unsigned)fair_clock-total_duration<<endl;
			
			cout<<endl<<"ALL PROCESSES SCHEDULED AND COMPLETED"<<endl;
			outfile<<endl<<"ALL PROCESSES SCHEDULED AND COMPLETED"<<endl;
			outfile<<endl;
			return 0;
		}else{
			return 1;
		}	
	}

	prc_ptr y = NULL;
	prc_ptr x = this->root;

	while (x != terminal_node) {
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
	insert_fix(p);
}

void red_black::insert_fix(prc_ptr temp){
	prc_ptr u;
	while (temp->parent->color == 1) {
		if (temp->parent == temp->parent->parent->right) {
			u = temp->parent->parent->left; // uncle
			if (u->color == 1) {
				// uncle red
				u->color = 0;
				temp->parent->color = 0;
				temp->parent->parent->color = 1;
				temp=temp->parent->parent;
			} else {
				if (temp ==temp->parent->left) {
					// case if left
					temp=temp->parent;
					rotate_right(temp);
				}
				// case 2
				temp->parent->color = 0;
				temp->parent->parent->color = 1;
				rotate_left(temp->parent->parent);
			}
		} else {
			u =temp->parent->parent->right; // uncle

			if (u->color == 1) {
				// mirror case uncle red
				u->color = 0;
				temp->parent->color = 0;
				temp->parent->parent->color = 1;
				temp=temp->parent->parent;	
			} else {
				if (temp==temp->parent->right) {
					// mirror case if left
					temp=temp->parent;
					rotate_left(temp);
				}
				// mirror of case 2
				temp->parent->color = 0;
				temp->parent->parent->color = 1;
				rotate_right(temp->parent->parent);
			}
		}
		if (temp== root) {
			break;
		}
	}
	root->color = 0;
}

int red_black::enter_processes(){
	cout<<"COMPLETE FAIR SCHEDULER SYSTEM"<<endl;
	int n;
	cout<<"Enter number of processes to be inserted: "<<endl;
	cin>>n;
	cout<<"Enter time quantum of processor: "<<endl;
	cin>>pro_cycle;
	
	float scheduler[n][2];											//Create an array to contain the processes
	cout<<endl<<"Enter priority and time quantum of processes to be inserted: "<<endl;
	cout<<"PLEASE LIMIT priority TO 1,2 or 3"<<endl;
	cout<<"lesser the number higher the priority(0<priority<=3)"<<endl;
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

void red_black::finalize(){
	
	ofstream outfile;

	outfile.open("process_report.txt",ios_base::app);
	
	cout<<"TURNAROUND TIMES: "<<endl<<endl;
	outfile<<"TURNAROUND TIMES: "<<endl<<endl;
	for(int i=0;i<table.size();i++){
		
		cout<<"PROCESS "<<table[i]->p_id<<endl;
		outfile<<"PROCESS "<<table[i]->p_id<<endl;
		cout<<"-------"<<endl<<endl;
		outfile<<"-------"<<endl<<endl;
		
		cout<<"START TIME = "<<(table[i]->start_time)<<endl;
		outfile<<"START TIME = "<<(table[i]->start_time)<<endl;
		
		cout<<"END TIME = "<<(table[i]->end_time)<<endl;
		outfile<<"END TIME = "<<(table[i]->end_time)<<endl;
		
		cout<<"TURNAROUND TIME = "<<(table[i]->end_time)-(table[i]->start_time)<<endl<<endl;
		outfile<<"TURNAROUND TIME = "<<(table[i]->end_time)-(table[i]->start_time)<<endl<<endl;
	}
	
	cout<<"CPU CLOCK TIME = "<<fair_clock<<endl;
	outfile<<"CPU CLOCK TIME = "<<fair_clock<<endl;
	outfile<<"-----------------"<<endl<<endl;
}

void red_black::show_entered(){
	
	ofstream outfile;
	outfile.open("process_report.txt",std::ios_base::app);
	
	cout<<"CPU CLOCK TIME AT START = "<<fair_clock<<endl;
	outfile<<"CPU CLOCK TIME AT START = "<<fair_clock<<endl;
	
	cout<<"INSERTED PROCESSES :"<<endl;
	outfile<<"INSERTED PROCESSES :"<<endl;
	
	for(int i=0;i<table.size();i++){
		cout<<"PROCESS ID= "<<table[i]->p_id<<" DURATION = "<<table[i]->total_runtime<<" PRIORITY ="<<table[i]->weight<<endl;
		outfile<<"PROCESS ID= "<<table[i]->p_id<<" DURATION = "<<table[i]->total_runtime<<" PRIORITY ="<<table[i]->weight<<endl;
	}
	
	outfile<<endl;
}
//rotations
void red_black::rotate_left(prc_ptr x) {
	prc_ptr y = x->right;
	x->right = y->left;
	if (y->left != terminal_node) {
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

void red_black::rotate_right(prc_ptr x) {
		prc_ptr y = x->left;
		x->left = y->right;
		if (y->right != terminal_node) {
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

prc_ptr red_black::minimum(prc_ptr p) {				
	while (p->left != terminal_node) {
		p = p->left;
	}
	return p;
}

//Processor functions
void red_black::create_processors(int capacity){			//Make necessary processors:
	
	processor_id++;
	processor_ptr q= new processor;
	q->proc_id=processor_id;
	q->capacity=capacity;

	cpu.push_back(q);			//Insert processor into vector
}
