#include <iostream>
#include <iomanip>
#include <string>
#include <list>
#include <vector>
#include <sstream>
#include <fstream>
using namespace std;


//static, external variables containing defaults settings
static string DEFAULT_FILE = "resources.txt";      //default file to be read when the program starts. this file cannot be overwritten by the program.
static string FILE_EXT = ".txt";                   //the extension for file types to be read (set to "" if no file type should be specified)
static string DIR = "RESOURCE-FILES/";             //default directory where resource files are stored

//==================
// Global Functions
//==================


//checks that the file is the appropriate type (has FILE_EXT)
static bool checkFile(string file){
    int location = (int)file.find(FILE_EXT);
    if (FILE_EXT != ""){         //if a file type is specified (i.e. if file input should be checked)
        if (location == -1){        //if extension not in filename
            return false;
        }
        if ((location != (file.size() - FILE_EXT.size()))){     //if extension not at end of filename
            return false;
        }
    }
    return true;
}

//creates a copy of the filename
static string createCopy(string file){
    int size = (int) file.size();
    string s;
    int i = 0;
    while (file[i] != '.' && i < size){
        s += file[i];
        i++;
    }
    s+="-copy";
    for (int j = i; j < size; j++){
        s+=file[j];
    }
    return s;
}

//splits string on delimiter into vector
static vector<string> split(string s, char delim =' '){
    vector<string> spl;
    string temp;
    stringstream ss (s);
    while (getline(ss, temp, delim)){
        spl.push_back(temp);
    }
    
    return spl;
    
}

//returns a slice of a vector of strings from begin to end
static vector<string> slice(vector<string> list, int begin, int end){
    vector<string> sl;
    for (int i = begin; i < end; i++){
        if (i > list.size()){
            break;
        } else {
            sl.push_back(list[i]);
        }
    }
    return sl;
}

//============
// NODE CLASS
//============
class Node{
public:
    string name;                //name of node
    vector<Node*> uses;         //list of dependencies that the node uses
    vector<Node*> usedby;       //list of nodes that use this node
    bool usable;
    
    Node(string n);
    
    //print out Node in format "<Name>  || <Usable?> || <Dep1>, <Dep2>,...<DepN>"
    void toString();
    void toString(int width);       //(width is formatting variable)
    
    //update the nodes usability to u, and update all nodes in UsedBy
    void updateNode(bool u);
    
    //checks if any dependencies are unusable, and updates node's usability
    void checkDeps();
    
    //add n as a dependency
    void addUses(Node *n);
    
    //add n to "usedby" list
    void addUsedby(Node *n);
};

//============
// GRAPH CLASS
//============

class Graph{
public: vector<Node*> nodes;
    int longestItem;
    
    //Adds a new node to the graph with a list of dependencies
    void addNewNode (string name, vector<string> dependsOn);
    
    //adds a new node to the graph w/ no dependencies
    void addNewNode (string name);
    
    //loads graph from file fileName
    bool readFile (string fileName);
    
    //saves graph in readable format to file fileName
    string saveFile (string fileName);
    
    //returns the node if it is in the graph, or adds and returns a new node if not
    Node* contains(string name);
    
    //prints out the current graph
    void printGraph();
    
    //delete node called by command "delete <itemname>" updates node as unusable
    bool deleteNode(string name);
    
    //adds node called by command "add <itemname> udpates node as usable
    string addNode(string name);
    
    void removeGraph();
    
    //updates longestName for formatting purposes
    void checkLongest(string name);
    
    //sorts graph by usability
    void sortGraphUSE();
    
    //sorts graph by node name
    void sortGraphNAME();
    
    //returns index of node "name" if in nodes list, or -1 if not found
    int inGraph(string name);

};




