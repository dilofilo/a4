#ifndef NETWORK_H
#define NETWORK_H
#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cstdlib>
#include <unordered_map>
#include <iterator>
// Our graph consists of a list of nodes where each node is represented as follows:
class Graph_Node{
public:
	string Node_Name;  // Variable name 
	int idx_in_network;
    vector<int> Children; // Children of a particular node - these are index of nodes in graph.
	vector<string> Parents; // Parents of a particular node- note these are names of parents
	
    int nvalues;  // Number of categories a variable represented by this node can take
	vector<string> values; // Categories of possible values
	unordered_map<string,int> valueToInt; //takes in a value and convert it to position.
    
    vector<float> CPT; // conditional probability table as a 1-d array . Look for BIF format to understand its meaning
    vector<int> counts; //Corresponding count for the CPT.
	// Constructor- a node is initialised with its name and its categories
    Graph_Node(string name,int n,vector<string> vals)
	{
		Node_Name=name;
		nvalues=n;
		values=vals;
	}
	string get_name()
	{
		return Node_Name;
	}
	vector<int> get_children()
	{
		return Children;
	}

    void print(std::ostream& o){
        o << "current_node: " << Node_Name << "\nParents: ";
        std::copy( Parents.begin(), Parents.end() ,std::ostream_iterator<string>(o, ", ")); 
        
        o << '\n';
        for (int i = 0; i < CPT.size(); i++){
            o << CPT[i] << " ";
        }

        o << '\n';
    }


	vector<string> get_Parents()
	{
		return Parents;
	}
	vector<float> get_CPT()
	{
		return CPT;
	}
	int get_nvalues()
	{
		return nvalues;
	}
	vector<string> get_values()
	{
		return values;
	}
	void set_CPT(vector<float> new_CPT)
	{
		CPT.clear();
		CPT=new_CPT;
	}
    void set_Parents(vector<string> Parent_Nodes)
    {
        Parents.clear();
        Parents=Parent_Nodes;
    }
    // add another node in a graph as a child of this node
    int add_child(int new_child_index )
    {
        for(int i=0;i<Children.size();i++)
        {
            if(Children[i]==new_child_index)
                return 0;
        }
        Children.push_back(new_child_index);
        return 1;
    }
};

 // The whole network represted as a list of nodes
class Network{
public:
	vector <Graph_Node> Pres_Graph;
    unordered_map< std::string,int > name_to_idx;
	int addNode(Graph_Node node)
	{
		Pres_Graph.push_back(node);
        name_to_idx[node.Node_Name] = Pres_Graph.size()-1;
		return 0;
	}
    
    
	int netSize()
	{
		return Pres_Graph.size();
	}
    // get the index of node with a given name
    int get_index(string val_name)
    {
        int hashsays = name_to_idx[val_name];
        if ( hashsays>=0 && hashsays<Pres_Graph.size() && (Pres_Graph[hashsays].Node_Name==val_name)) {
            return hashsays;
        } else {
            return -1;
        }
        //f the shit that follows.
        vector<Graph_Node>::iterator listIt;
        int count=0;
        for(listIt=Pres_Graph.begin();listIt!=Pres_Graph.end();listIt++)
        {
            if(listIt->get_name().compare(val_name)==0)
                return count;
            count++;
        }
        return -1;
    }
// get the node at nth index
    vector<Graph_Node>::iterator get_nth_node(int n)
    {
        vector<Graph_Node>::iterator listIt = Pres_Graph.begin();
        listIt += n;
        return listIt;
        //f the shit that follows.

        int count=0;
        for(listIt=Pres_Graph.begin();listIt!=Pres_Graph.end();listIt++)
        {
            if(count==n)
                return listIt;
            count++;
        }
        return listIt; 
    }
    //get the iterator of a node with a given name
    vector<Graph_Node>::iterator search_node(string val_name)
    {
        vector<Graph_Node>::iterator listIt = Pres_Graph.begin();
        int hashsays = name_to_idx[val_name];
        if ( hashsays>=0 && hashsays<Pres_Graph.size()&&(Pres_Graph[hashsays].Node_Name == val_name)) {
            return Pres_Graph.begin() + hashsays;
        } else {
            cout<<"node not found\n";
            return Pres_Graph.end();    
        }

        
        for(listIt=Pres_Graph.begin();listIt!=Pres_Graph.end();listIt++)
        {
            if(listIt->get_name().compare(val_name)==0)
                return listIt;
        }    
        cout<<"node not found\n";
        return listIt;
    }
	
    void fixup();
};

void Network::fixup() {
    for (int i=0; i<Pres_Graph.size(); ++i) {
        Pres_Graph[i].counts.resize( Pres_Graph[i].CPT.size() , 0);
    }
}


void read_network(Network& Alarm , std::string fname)
{
	//Network Alarm;
	string line;
	int find=0;
  	ifstream myfile(fname.c_str()); 
  	string temp;
  	string name;
  	vector<string> values;
  	
    if (myfile.is_open())
    {
    	while (! myfile.eof() )
    	{
    		stringstream ss;
      		getline (myfile,line);
      		
      		
      		ss.str(line);
     		ss>>temp;
     		
     		
     		if(temp.compare("variable")==0)
     		{
                    
     				ss>>name;
     				getline (myfile,line);
                   
     				stringstream ss2;
     				ss2.str(line);
     				for(int i=0;i<4;i++)
     				{
     					
     					ss2>>temp;
     					
     					
     				}
     				values.clear();
     				unordered_map<string , int> t_valueToInt;
                    while(temp.compare("};")!=0)
     				{
                        t_valueToInt[temp] = values.size(); //get the value to int conversion.
     					values.push_back(temp);
     					
     					ss2>>temp;
    				}
     				Graph_Node new_node(name,values.size(),values);
                    new_node.idx_in_network = Alarm.Pres_Graph.size();
     				new_node.valueToInt = t_valueToInt;

                    int pos=Alarm.addNode(new_node);

     				
     		}
     		else if(temp.compare("probability")==0)
     		{
                    
     				ss>>temp;
     				ss>>temp;
     				
                    vector<Graph_Node>::iterator listIt;
                    vector<Graph_Node>::iterator listIt1;
     				listIt=Alarm.search_node(temp);
                    int index=Alarm.get_index(temp);
                    ss>>temp;
                    values.clear();
     				while(temp.compare(")")!=0)
     				{
                        listIt1=Alarm.search_node(temp);
                        listIt1->add_child(index);
     					values.push_back(temp);
     					
     					ss>>temp;

    				}
                    listIt->set_Parents(values);
    				getline (myfile,line);
     				stringstream ss2;
                    
     				ss2.str(line);
     				ss2>> temp;
                    
     				ss2>> temp;
                    
     				vector<float> curr_CPT;
                    string::size_type sz;
     				while(temp.compare(";")!=0)
     				{
     					curr_CPT.push_back(atof(temp.c_str()));
     					
     					ss2>>temp;  
    				}
                    
                    listIt->set_CPT(curr_CPT);
     		}
            else
            {
                
            }	
    	}    	
    	if(find==1)
    	myfile.close();
  	}  	
  	Alarm.fixup();
    //return Alarm;
}

#endif