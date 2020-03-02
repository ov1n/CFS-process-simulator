#include <iostream>
#include<vector>
#include<fstream>

using namespace std;

// data structure that represents a node in the tree
struct Node {
	int data; // holds the key
	int weight;	//the weight of the process
	int p_id;		//process id of current process
	int curr_time; //holds current time
	int wait_runtime; //Per process runtime
	Node *parent; // pointer to the parent
	Node *left; // pointer to left child
	Node *right; // pointer to right child
	int color; // 1 -> Red, 0 -> Black
};

typedef Node *NodePtr;

// class RBTree implements the operations in Red Black Tree
class RBTree {
private:
	NodePtr root;
	NodePtr TNULL;
	// fix the rb tree modified by the delete operation
	void fixDelete(NodePtr x) {
		NodePtr s;
		while (x != root && x->color == 0) {
			if (x == x->parent->left) {
				s = x->parent->right;
				if (s->color == 1) {
					// case 3.1
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


	void rbTransplant(NodePtr u, NodePtr v){
		if (u->parent == NULL) {
			root = v;
		} else if (u == u->parent->left){
			u->parent->left = v;
		} else {
			u->parent->right = v;
		}
		v->parent = u->parent;
	}

	Node *deleteNodeHelper(NodePtr node) {
		// find the node containing key
		NodePtr z = node;
		NodePtr x, y;

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
		fair_clock++;
		return z;
	}
	
	// fix the red-black tree
	void fixInsert(NodePtr k){
		NodePtr u;
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

	void printHelper(NodePtr root, string indent, bool last) {
		// print the tree structure on the screen
	   	if (root != TNULL) {
		   /*cout<<indent;
		   if (last) {
		      cout<<"R----";
		      indent += "     ";
		   } else {
		      cout<<"L----";
		      indent += "|    ";
		   }
            
           string sColor = root->color?"RED":"BLACK";
		   //cout<</*root->data<<"("<<sColor<<")"<<"p_id :"<<root->p_id<<" time qun :"<<root->data<<" curr time:"<<root->curr_time<<endl;*/
		   printHelper(root->left, indent, false);
		   printHelper(root->right, indent, true);
		}
		// cout<<root->left->data<<endl;
	}

public:
	RBTree() {
		process_id=-1;
		fair_clock=0;		//Initialize clock
		TNULL = new Node;
		TNULL->color = 0;
		TNULL->left = NULL;
		TNULL->right = NULL;
		root = TNULL;
	}
	
	int fair_clock;					//System wide run CPU clock
	int process_id;					//Variable to give each process a unique ID
	int process_count;				//no of processes, count decrements each time a process exits
	vector<Node *> table;
	
	int enter_processes(){
		cout<<"COMPLETE FAIR SCHEDULER SYSTEM"<<endl;
		int n;
		cout<<"Enter number of processes to be inserted: "<<endl;
		cin>>n;
		
		int scheduler[n][2];											//Create an array to contain the processes
		cout<<endl<<"Enter time quantum and weight of processes to be inserted: "<<endl;
		cout<<"lesser the number higher the priority(>0)"<<endl;
		int temp1;														//Temporary variable to store time quantum
		int temp2;
		
		for(int i=0;i<n;i++){
			cin>>temp1;
			cin>>temp2;
			//insert time quantums into an array;
			scheduler[i][0]=temp1;
			scheduler[i][1]=temp2;
			
		}
		
		for(int i=0;i<n;i++){
			//cout<<"I "<<i;
			//Make initial RBT
			insert(scheduler[i][0],scheduler[i][1]);
			
		}
		
		return n;					//Returns n to be used later for deletion and controling the scheduling process
	}
	
	void finalize(){
		
		ofstream outfile;
	
		outfile.open("process_report.txt",ios_base::app);
		cout<<"FAIR CLOCK TIME "<<fair_clock<<endl;
		outfile<<"FAIR CLOCK TIME "<<fair_clock<<endl;
		outfile<<"-----------------"<<endl;
	}
	
	void show_entered(){
		
		ofstream outfile;
		outfile.open("process_report.txt",std::ios_base::app);
		
		outfile<<"CFS SCHEDULING SYSTEM :"<<endl;
		outfile<<"-----------------------"<<endl;
		
		cout<<"INSERTED PROCESSES :"<<endl;
		outfile<<"INSERTED PROCESSES :"<<endl;
		
		
		for(int i=0;i<table.size();i++){
			cout<<"PROCESS ID= "<<table[i]->p_id<<" DURATION = "<<table[i]->data<<" PRIORITY ="<<table[i]->weight<<endl;
			outfile<<"PROCESS ID= "<<table[i]->p_id<<" DURATION = "<<table[i]->data<<" PRIORITY ="<<table[i]->weight<<endl;
		}
		
		outfile<<endl;
	}
	// find the node with the minimum key
	NodePtr minimum(NodePtr node) {
		while (node->left != TNULL) {
			node = node->left;
		}
		return node;
	}

	// rotate left at node x
	void leftRotate(NodePtr x) {
		NodePtr y = x->right;
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

	// rotate right at node x
	void rightRotate(NodePtr x) {
		NodePtr y = x->left;
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
		NodePtr node = new Node;
		node->parent = NULL;
		node->data = key;
		node->weight = weight;
		node->p_id = process_id;
		node->curr_time=0;
		node->wait_runtime=weight;
		node->left = TNULL;
		node->right = TNULL;
		node->color = 1; // new node must be red
		
		//Insert into vector
		table.push_back(node);

		NodePtr y = NULL;
		NodePtr x = this->root;

		while (x != TNULL) {
			y = x;
			if (node->wait_runtime < x->wait_runtime) {
				x = x->left;
			} else {
				x = x->right;
			}
		}

		// y is parent of x
		node->parent = y;
		if (y == NULL) {
			root = node;
		} else if (node->wait_runtime < y->wait_runtime) {
			y->left = node;
		} else {
			y->right = node;
		}

		// if new node is a root node, simply return
		if (node->parent == NULL){
			node->color = 0;
			return;
		}

		// if the grandparent is null, simply return
		if (node->parent->parent == NULL) {
			return;
		}
		
		// Fix the tree
		fixInsert(node);
	}
	
	int insert(Node *node) {
		
		node->parent = NULL;
		//node->data = 1; // CHANGE TEST VALUE
		node->curr_time++;
		node->wait_runtime+=node->weight;
		node->left = TNULL;
		node->right = TNULL;
		node->color = 1; // new node must be red
		
		//cout<<"process time"<<node->curr_time<<endl;
		//cout<<"PROCESS COUNT"<<process_count<<endl;
		ofstream outfile;
		outfile.open("process_report.txt",std::ios_base::app);		//2nd parameter for appending
		
		if(node->curr_time>=node->data){
			cout<<"\tPROCESS WITH ID "<<node->p_id<<" COMPLETED"<<endl;
			outfile<<"\tPROCESS WITH ID "<<node->p_id<<" COMPLETED"<<endl;
			process_count--;
			if(process_count<=0){
				cout<<endl<<"ALL PROCESSES SCHEDULED AND COMPLETED"<<endl;
				outfile<<endl<<"ALL PROCESSES SCHEDULED AND COMPLETED"<<endl;
				outfile<<endl;
				return 0;
			}else{
				return 1;
			}	
		}

		NodePtr y = NULL;
		NodePtr x = this->root;

		while (x != TNULL) {
			y = x;
			if (node->wait_runtime < x->wait_runtime) {
				x = x->left;
			} else {
				x = x->right;
			}
		}

		// y is parent of x
		node->parent = y;
		if (y == NULL) {
			root = node;
		} else if (node->wait_runtime < y->wait_runtime) {
			y->left = node;
		} else {
			y->right = node;
		}

		// if new node is a root node, simply return
		if (node->parent == NULL){
			node->color = 0;
			return 1;
		}

		// if the grandparent is null, simply return
		if (node->parent->parent == NULL) {
			return 1;
		}

		// Fix the tree
		fixInsert(node);
	}

	NodePtr getRoot(){
		return this->root;
	}

	// delete the node from the tree
	Node *deleteNode() {
		Node *x=minimum(root);
		//cout<<"data of min"<<x->data<<endl;
		Node *y=deleteNodeHelper(x);
		//cout<<"data of deleted"<<y->data<<endl;
		
		ofstream outfile;
		outfile.open("process_report.txt",std::ios_base::app);
		
		cout<<endl<<"T="<<fair_clock<<"\t"<<"RUNNING PROCESS WITH ID "<<y->p_id<<endl;
		outfile<<"T="<<fair_clock<<"\t"<<"RUNNING PROCESS WITH ID "<<y->p_id<<endl;
		return y;
	}

	// print the tree structure on the screen
	void prettyPrint() {
	    if (root) {
    		printHelper(this->root, "", true);
	    }
	}

};

int main() {
	RBTree bst;
	
	//FILE HANDLING
	ofstream outfile;
	outfile.open("process_report.txt");
	bst.process_count=bst.enter_processes();			//Enter time quantums of processes seperated by space returns total no of processes entered
	bst.show_entered();
	//cout<<"count"<<count<<endl;
	
	bst.prettyPrint();
	int flag=1;											//Flag to keep scheduler running, controlled by insert function inside
	while(flag){
		
		Node *x=bst.deleteNode();
		
		bst.prettyPrint();
		flag=bst.insert(x);
		bst.prettyPrint();
	}
	
	bst.finalize();
	
	outfile.close();
	
	return 0;
}
