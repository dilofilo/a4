#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <fstream>
#include <sstream>
#include <cstdlib>


using namespace std;

class Graph_Node{

private:
	string Node_Name;
	vector<int> Children;
	vector<string> Parents;
	int nvalues;
	vector<string> values;
	vector<double> CPT;

public:
	//Graph_Node(string name, vector<Graph_Node*> Child_Nodes,vector<Graph_Node*> Parent_Nodes,int n, vector<string> vals,vector<double> curr_CPT)
	Graph_Node(string name,int n,vector<string> vals)
	{
		Node_Name=name;
		//Children=Child_Nodes;
		//Parents=Parent_Nodes;
		nvalues=n;
		values=vals;
		//CPT=curr_CPT;

	}
	string get_name()
	{
		return Node_Name;
	}
	vector<int> get_children()
	{
		return Children;
	}
	vector<string> get_Parents()
	{
		return Parents;
	}
	vector<double> get_CPT()
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
	void set_CPT(vector<double> new_CPT)
	{
		CPT.clear();
		CPT=new_CPT;
	}
    void set_Parents(vector<string> Parent_Nodes)
    {
        Parents.clear();
        Parents=Parent_Nodes;
    }
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



class network{

	list <Graph_Node> Pres_Graph;

public:
	int addNode(Graph_Node node)
	{
		Pres_Graph.push_back(node);
		return 0;
	}
    list<Graph_Node>::iterator getNode(int i)
    {
        int count=0;
        list<Graph_Node>::iterator listIt;
        for(listIt=Pres_Graph.begin();listIt!=Pres_Graph.end();listIt++)
        {
            if(count++==i)
                break;
            
        }
        return listIt;
    }
	int netSize()
	{
		return Pres_Graph.size();
	}
    int get_index(string val_name)
    {
        list<Graph_Node>::iterator listIt;
        int count=0;
        for(listIt=Pres_Graph.begin();listIt!=Pres_Graph.end();listIt++)
        {
            if(listIt->get_name().compare(val_name)==0)
                return count;
            count++;
        }
        return -1;
    }

    list<Graph_Node>::iterator get_nth_node(int n)
    {
       list<Graph_Node>::iterator listIt;
        int count=0;
        for(listIt=Pres_Graph.begin();listIt!=Pres_Graph.end();listIt++)
        {
            if(count==n)
                return listIt;
            count++;
        }
        return listIt; 
    }

    list<Graph_Node>::iterator search_node(string val_name)
    {
        list<Graph_Node>::iterator listIt;
        for(listIt=Pres_Graph.begin();listIt!=Pres_Graph.end();listIt++)
        {
            if(listIt->get_name().compare(val_name)==0)
                return listIt;
        }
    
            cout<<"node not found\n";
        return listIt;
    }
	

};

network read_network()
{
	network Alarm;
	string line;
	int find=0;
  	ifstream myfile("AILA.bif"); 
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
                    //cout<<line<<"\n";
     				ss>>name;
     				getline (myfile,line);
                    //cout<<line<<"\n";
     				stringstream ss2;
     				ss2.str(line);
     				for(int i=0;i<4;i++)
     				{
     					
     					ss2>>temp;
     					//cout<<"printing"<<temp<<"n";
     					
     				}
     				values.clear();
     				while(temp.compare("};")!=0)
     				{
     					values.push_back(temp);
     					//cout<<"here"<<temp<<"\n";
     					ss2>>temp;
    				}
     				Graph_Node new_node(name,values.size(),values);
     				int pos=Alarm.addNode(new_node);

     				//std::cout<<"Node Added\n";
     				//break;
     		}
     		else if(temp.compare("probability")==0)
     		{
                    //cout<<line<<"\n";
     				ss>>temp;
     				ss>>temp;
     				//ss>>temp;
                    list<Graph_Node>::iterator listIt;
                    list<Graph_Node>::iterator listIt1;
     				listIt=Alarm.search_node(temp);
                    int index=Alarm.get_index(temp);
                    //cout<<listIt->get_name();
                    //cout<<"test"<<temp;
                    //exit(0);
                    values.clear();
                    ss>>temp;
     				while(temp.compare(")")!=0)
     				{
                        
                        listIt1=Alarm.search_node(temp);
                        listIt1->add_child(index);
     					values.push_back(temp);
     					//cout<<"here"<<temp<<"\n";
     					ss>>temp;

    				}
                    listIt->set_Parents(values);
    				getline (myfile,line);
     				stringstream ss2;
                    
     				ss2.str(line);
     				ss2>> temp;
                    //cout<<"\t"<<temp<<" ";
     				ss2>> temp;
                    
     				vector<double> curr_CPT;
                    string::size_type sz;
     				while(temp.compare(";")!=0)
     				{
                       // cout<<"-1 ";
     					curr_CPT.push_back(atof(temp.c_str()));
     					//cout<<"here"<<temp<<"\n";
     					ss2>>temp;
                       
                        

    				}
                    //cout<<temp<<"\n";
                    //cout<<curr_CPT.size()<<"\n";
                    listIt->set_CPT(curr_CPT);


     		}
            else
            {
                //cout<<line<<"\n";
                
            }
     		
     		

    		
    		//myfile.close();
    	}
    	//cout<<line;
    	if(find==1)
    	myfile.close();
  	}
  	
  	return Alarm;
}

int print_sample(int* current_sample,network Alarm)
{

    vector<string> Values;
    int t= rand()%Alarm.netSize();
    for(int i=0;i<Alarm.netSize();i++)
    {
        list<Graph_Node>::iterator current_Node=Alarm.get_nth_node(i);
        Values=current_Node->get_values();
        if(current_sample[i]>=Values.size())
        {
            cout<<" Issue here"<<current_sample[i]<<i;
            exit(0);
        }
        if(t==i)
            cout<<"\"?\"";
        else
            cout<<Values[current_sample[i]];
	//cout<<current_sample[i];
        if(i<Alarm.netSize()-1)
            cout<<" ";
    }

    //cout<<"\n";
}
int print_net(network Alarm)
{
    for(int i=0;i<Alarm.netSize();i++)
    {
        list<Graph_Node>::iterator listIt=Alarm.get_nth_node(i);
        vector<string> pars=listIt->get_Parents();
        cout<<"\n"<<listIt->get_name()<<"parents-";
        for(int j=0;j<pars.size();j++)
            cout<<pars[j];
        //vector<double> CPT=listIt->get_CPT
    }
}
int sampling_ithnode(network Alarm,int* current_sample,int k);
vector<double> calc_prob(network Alarm, int k, int* current_sample)
{
    list<Graph_Node>::iterator current_Node=Alarm.getNode(k);
    vector<int> Child_Nodes=current_Node->get_children();
    int n=current_Node->get_nvalues();
    vector<string> pars= current_Node->get_Parents();
    vector<double> cpt_table=current_Node->get_CPT();
    vector<double> current_p;
    list<Graph_Node>::iterator listIt;
    vector<int> pres_val;
    vector<int> num_val;
    
    for(int j=0;j<n;j++)
    {
        vector<int> pres_val;
        vector<int> num_val;
        pres_val.push_back(j);
        num_val.push_back(n);
        
        for(int i=0;i<pars.size();i++)
        {
            
            listIt=Alarm.search_node(pars[i]);
            int ind=Alarm.get_index(pars[i]);
            if(current_sample[ind]==-1)
                current_sample[ind]=sampling_ithnode(Alarm,current_sample,ind);
            pres_val.push_back(current_sample[ind]);
            num_val.push_back(listIt->get_nvalues());
        }
        
        int cpt_index=0;
        for(int l=0;l<pres_val.size();l++)
        {
            
            if(l!=0)
                cpt_index*=num_val[l];
            cpt_index+=pres_val[l];
        }
        
        current_p.push_back(0);
       
        if(j>0)
            current_p[j]=current_p[j-1]+cpt_table[cpt_index];
        else
            current_p[j]=cpt_table[cpt_index];
        

    }
   for(int i=0;i<current_p.size();i++)
   {
    if(current_p[i]<0)
    {
      cout<<"issue in prob"<<k;
     exit(0);
    }
      //cout<<current_p[i];
   }
  return current_p;


}

int sampling_ithnode(network Alarm,int* current_sample,int k)
{
    list<Graph_Node>::iterator current_Node=Alarm.get_nth_node(k);
    vector<string> Values=current_Node->get_values();
    int n=current_Node->get_nvalues();
    string feature=current_Node->get_name();
    vector<double> cpt_table=current_Node->get_CPT();
    vector<double> prob=calc_prob(Alarm, k,current_sample);
    if((k==0)&&prob.size()>2)
    {
        cout<<"Issues here\n";
        exit(0);
    }
    //cout<<"here"<<k<<prob.size();
    double t = (double)rand() / RAND_MAX; 
            
    int l;
    for(l=0;l<prob.size();l++)
    {
        if(t<prob[l])
            break;
    }
     if((k==0)&&(l>=2))
    {
        cout<<"Issues here"<<t<<prob[0]<<prob[1]<<"\n";
        exit(0);
    }
    return l;

}

int main()
{
	network Alarm;
	Alarm=read_network();
    
    int count=0;
    vector<string> Values;
    vector<double> cpt_table;
    string feature;
    int n;

    int current_sample[Alarm.netSize()];
    for(int i=0;i<500000;i++)
    {
        for(int k=0;k<Alarm.netSize();k++)
            current_sample[k]=-1;
        for(int k=0;k<Alarm.netSize();k++)
        {
            if(current_sample[k]!=-1)
                continue;
            current_sample[k]=sampling_ithnode(Alarm,current_sample,k);

            
            
            //cout<<Values[t]<<" ";
        }

        
        print_sample(current_sample,Alarm);
        if(i<500000-1)
        cout<<"\n";
    }
    

	//cout<<Alarm.netSize();
	
}




