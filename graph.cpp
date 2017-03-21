#include "resources.h"

//  ========================
//    NODE CLASS FUNCTIONS
//  ========================

//constructor
Node::Node(string n){
    name = n;
    usable = true;
}

//print out Node in format "<Name>  || <Usable?> || <Dep1>, <Dep2>,...<DepN>"
void Node::toString(){
    toString((int)name.size()+5);
}

void Node::toString(int width){
    cout << setfill(' ') << setw(width) << left << name;
    cout << " || ";
    if (usable){
        cout << "   o   ";
    } else {
        cout << "   x   ";
    }
    cout << " || ";
    string s = "";
    for(int i = 0; i <uses.size(); i++){
        s += uses[i]->name;
        if (!uses[i]->usable){
            s+= "(x)";
        }
        s+=", ";
    }
    cout << s;
    cout << "\n";
}

//update the nodes usability to u, and update all nodes in usedBy
void Node::updateNode(bool u) {
    if (usable == u){       //if no change, do not modify usedby (for cycles)
        return;
    } else {
        usable = u;
        for (int i = 0; i < usedby.size(); i++){
            usedby[i]->updateNode(u);
        }
        checkDeps();
    }
}

//checks if any dependencies are unusable, and updates node's usability
void Node::checkDeps(){
    for (int i = 0; i < uses.size(); i++){
        if (!uses[i]->usable){
            usable = false;
        }
    }
}

//add n as a dependency
void Node::addUses(Node *n){
    if (!n->usable){        //if the new dependency is not usable, update this node to be unusable
        updateNode(false);
    }
    for (int i = 0; i < uses.size(); i++){      //if the new dependency already exists in the list, do not add
        if (n->name == uses[i]->name){
            return;
        }
    }
    uses.push_back(n);
}

//add n to "usedby" list
void Node::addUsedby(Node *n){
    for (int i = 0; i < usedby.size(); i++){
        if (n->name == usedby[i]->name){        //if n is already in usedby, do not add
            return;
        }
    }
    usedby.push_back(n);
}

//  ====================
//    NODE COMPARATORS
//  ====================

//compares nodes by node name
bool compareNodesNAME(Node *i, Node *j){
    return i->name < j->name;
}

//compares nodes by usability: usable nodes with shortest dependency lists first.
bool compareNodesUSE(Node *iNode, Node *jNode){
    bool i = iNode->usable;
    bool j = jNode->usable;
    int iSize = (int) iNode->uses.size();
    int jSize = (int) jNode->uses.size();
    string iName = iNode->name;
    string jName = jNode->name;
    if (i && !j){
        return true;
    } else if (j && !i){           //if j is usable and i is not, swap
        return false;
    } else {           //if both are usable or both are unusable
        if (iSize < jSize){     //sort by size of "uses" list
            return true;
        } else if (iSize > jSize){
            return false;
        } else {
            return iName < jName;       //if "uses" list is same size, sort by node name
        }
    }
}

//  =========================
//    GRAPH CLASS FUNCTIONS
//  =========================

//Adds a new node to the graph with a list of dependencies
void Graph::addNewNode (string name, vector<string> dependsOn){
    Node* n = contains(name);
    n->usable = true;
    for (int i = 0; i < dependsOn.size(); i++){
        string depname = dependsOn[i];
        Node* dep = contains(depname);
        n->addUses(dep);
        dep->addUsedby(n);
    }
    n->checkDeps();
}

//adds a new node to the graph w/ no dependencies
void Graph::addNewNode (string name){
    checkLongest(name);
    nodes.push_back(new Node(name));
}

//loads graph from file fileName
bool Graph::readFile (string fileName){
    cout <<"Reading in graph from " << fileName << "...\n";
    ifstream file;
    file.open(DIR + fileName);
    string line;
    if (file.is_open()){
        longestItem = 0;
        while (getline(file, line)){
            vector<string> items = split(line, ' ');
            vector<string> dependencies = slice(items, 1, (int)items.size());
            cout << items[0] << " ADDED \n";
            addNewNode(items[0], dependencies);
        }
//        currentFile = fileName;
        return true;
    } else {
        return false;
    }
    
}

//saves graph in readable format to file fileName
string Graph::saveFile (string fileName){
    if (fileName == DEFAULT_FILE){
        fileName = createCopy(fileName);
        cout << "Cannot overwrite default file. Creating copy at: " << fileName << "\n";
    }
    cout << "Saving graph to " << fileName << "...\n";
    ofstream file;
    file.open(DIR + fileName, ios::trunc);
    for (int i = 0; i < nodes.size(); i++){
        file << nodes[i]->name << " ";
        for (int j = 0; j < nodes[i]->uses.size(); j++){
            file << nodes[i]->uses[j]->name << " ";
        }
        file << "\n";
    }
    file.close();
    return fileName;
}


//returns the node if it is in the graph, or adds and returns a new node if not
Node* Graph::contains(string name){
    int i = inGraph(name);
    if (i != -1){
        return nodes[i];
    }
    checkLongest(name);
    Node* ret = new Node(name);
    nodes.push_back(ret);
    return ret;
}

//prints out the current graph
void Graph::printGraph(){
    int size = (int) nodes.size();
    cout << "o = usable, x = unusable\n\n";
    int barWidth = ((longestItem + 6)*2) + 15;
    if (longestItem < 5){
        cout << setw(5) << setfill(' ') << left << "Item";
    } else {
        cout << setw(longestItem) << setfill(' ') << left << "Item";
    }
    cout << " || Usable? || ";
    cout << "Uses\n";
    cout << setw(barWidth) << setfill('=') << right << "\n";
    for (int i = 0; i < size; i++){
        nodes[i]->toString(longestItem);
    }
    
}

//delete node called by command "delete <itemname>" updates node as unusable
bool Graph::deleteNode(string name){
    int i = inGraph(name);
    if (i == -1){
        return false;
    } else {
        Node* n = nodes[i];
        n->updateNode(false);
        return true;
    }
}


//add node called by command "add <itemname> udpates node as usable. returns list of missing dependencies if node cannot be added
string Graph::addNode(string name){
    string deps = "";
    Node* n = contains(name);
    for (int i = 0; i < n->uses.size(); i++){
        if (!(n->uses[i]->usable)){
            deps+=n->uses[i]->name;
        }
    }
    if (deps.size() == 0){
        n->updateNode(true);
    }
    return deps;
}

//clear graph and free memory allocated to nodes
void Graph::removeGraph(){
    longestItem = 0;
    for(int i=0 ; i < nodes.size() ; i++)
        delete nodes[i];
    nodes.clear();
}


//updates longestName for formatting purposes
void Graph::checkLongest(string name){
    if (name.size() > longestItem){
        longestItem = (int)name.size();
    }
}

//sorts graph by usability
void Graph::sortGraphUSE(){
    sort(nodes.begin(), nodes.end(), compareNodesUSE);
}

//sorts graph by node name
void Graph::sortGraphNAME(){
    sort(nodes.begin(), nodes.end(), compareNodesNAME);
}

//returns index of node "name" if in nodes list, or -1 if not found
int Graph::inGraph(string name){
    for(int i = 0; i < nodes.size(); i++){
        if (nodes[i]->name == name){
            return i;
        }
    }
    return -1;
}







