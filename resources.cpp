//
//  resources.cpp
//  Inventory
//
//  Created by Emily Rizzo on 2/23/17.
//  Copyright © 2017 Emily Rizzo. All rights reserved.
//


#include "graph.cpp"


//global variables
Graph g;                        //graph manipulated by main()
vector<string> SAVES;           //list of files which have been saved since the program started running
string currentFile;             //most recent file saved or read into g


// main() helper function definition
string getCommand(string msg);                                                              //prompts for command and prints msg
string runCommand(vector<string> command, string action, string item, int size);            //executes command
string printSaves();            //prints current saves
string toLower(string s);       //converts s to lowercase string
string help();                  //prints list of commands

int main(int argc, const char * argv[]) {

    vector<string> deps;
    
    string input;
    string msg;
    string action;
    string item;

    int size;
    vector<string> command;
    currentFile = DEFAULT_FILE;
    
    g.readFile(DEFAULT_FILE);      //read in the initial graph from text file
    cout << "\n";
    
    while (input != "exit" && input != "q"){
        g.printGraph();                         //on loop, print graph and current message, then prompt for command
        input = getCommand(msg);
        msg = "";               //reset msg, action, and item after printing
        action = "";
        item = "";
        
        //parse input into appropriate vars
        command = split(toLower(input));
        size = (int)command.size();
        string fileName = DEFAULT_FILE;
        if (size > 0){
            action = command[0];
        }
        if (size >1){
            item = command[1];
        }
        
        //execute the command and retrieve any messages
        msg = runCommand(command, action, item, size);
        
    }
    g.removeGraph();
    return 0;
}



// ==============
// MAIN() HELPERS
// ==============


//helper function to prompt for command and print msg
string getCommand(string msg){
    string input;
    if (msg.size() != 0){
        cout << "\n\n >>> " << msg << "\n\n";
    }
    cout << "\nCommands: \"delete <item>\", \"add <item>\", \"read <file>\", \"save <file>\", \"new\", \"sort\" (\"q\" to quit)";
    cout << "\nEnter a command (\"help\" for info): ";
    getline(cin, input);
    return input;
}

//helper function to process and execute command
string runCommand(vector<string> command, string action, string item, int size){
    string msg;
    if (action == "delete"){        //delete a node in the graph (make unusable, and update graph);
        if (size == 2){
            if (g.deleteNode(item)){            //checks to see that node is in graph
                msg = "node \"" + item + "\" DELETED";
            } else {
                msg = "ERROR: node \"" + item + "\" not in graph.";
            }
        } else {
            msg = "ERROR: please specify a single node to delete, ex: \"delete ore\"";
        }
    } else if (action == "add"){        //add or update a node
        if (size == 2){                         //if size of command is 2, adding a node with no dependencies
            string deps = g.addNode(item);
            if (deps.size() > 0){                   //if the requested item is missing a dependency, user cannot add to graph.
                msg = "ERROR: cannot add \"" + item + "\" without first adding missing dependencies: " + deps;
            } else {
                msg = item + " added";
            }
        } else if (size > 2){       //if size of command > 2, adding a node with dependencies - or updating dependencies for old node
            vector<string> dependencies = slice(command, 2, size);
            msg = "node \"" + item + "\" ADDED";
            g.addNewNode(item, dependencies);
        } else {
            msg = "ERROR: please specify a node to add, ex: \"add ore\"";
        }
    } else if (action == "help"){           //print help menu
        msg = help();
        
    } else if (action == "sort"){           //sort the graph by name of node, or by 'usability' (usable nodes w/ shortest dependency lists first)
        if (item == "-n"){
            g.sortGraphNAME();
            msg = "sorted graph by name (use \"sort -u\" to sort by usability)";
        } else {
            g.sortGraphUSE();
            msg = "sorted graph by usability (use \"sort -n\" to sort by name)";
        }
    } else if (action == "read"){
        if (size == 2){         //if command size = 2, a file name was specified
            if (checkFile(item)){
                g.removeGraph();
                if (g.readFile(item)){          //checks to see if the file was opened
                    msg = "Read graph from " + item;
                    currentFile = item;
                } else {
                    msg = "ERROR: Could not find file: " + item + "\n       Please enter an accurate filename, or \"read\" to read default file again.";
                }
            } else {
                msg = "ERROR: \"" + item + "\" is not the correct file type. Only \"" + FILE_EXT + "\" files are permitted.";
            }
        } else if (size == 1){       //if no file was specified, read from DEFAULT_FILE
            currentFile = DEFAULT_FILE;
            g.removeGraph();
            g.readFile(DEFAULT_FILE);
            msg = "Read graph from default file: " + DEFAULT_FILE + " (specify file with \"read <filename>\"";
        } else {                    //give error if too many filenames were specified.
            msg = "ERROR: please specify a single filename, ex: \"read resources.txt\"";
        }
    } else if (action == "save"){           //saving a file
        if (size == 2){             //if command size = 2, either a file name was specified or -l flag was passsed
            if (item == "-l"){
                msg = printSaves();
            } else {
                if (checkFile(item)){
                    currentFile = item;
                    string f = g.saveFile(item);
                    SAVES.push_back(item);
                    msg = "Successfully saved graph to file " + f;
                } else {
                    msg = "ERROR: \"" + item + "\" is not the correct file type. Only \"" + FILE_EXT + "\" files are permitted.";
                }
            }
        } else if (size == 1){          //if no file name is specified, save to the most recently read filename (or give error if no current file)
            if (currentFile != ""){
                string f = g.saveFile(currentFile);
                msg = "Successfully saved graph to file " + f;
            } else {
                msg = "ERROR: No current file. Please specify filename using \"save <filename>\"";
            }
        } else {
            msg = "ERROR: please specify a single filename, ex: \"save resources.txt\"";
        }
    } else if (action == "new"){            //creating new, empty graph. Prompt user to confirm deletion of current graph before erasing.
        string confirm;
        cout << "Are you sure you want to delete the current graph and create a new, empty one? Y or N: ";
        getline(cin, confirm);
        if (toLower(confirm) == "y"){
            msg = "Graph deleted. New graph created.";
            g.removeGraph();
            currentFile = "";
        } else {
            msg = "Did not delete graph. Use \"save <filename>\" to save current graph";
        }
    } else if (action == "file"){           //print out current file and DEFAULT_FILE
        msg = "CURRENT FILE: " + currentFile + "   DEFAULT FILE: " + DEFAULT_FILE;
    }
    else {                              //unrecognized command
        msg = "ERROR: \"" + action + "\" not a valid command. Type \"help\" for list of commands.";
    }
    
    return msg;
}

//helper function to print out list of saves added since program started
string printSaves(){
    if (SAVES.size() == 0){
        return "No Current Saves :(";
    }
    string s = "Current Saves:\n";
    for (int i = 0; i < SAVES.size(); i++){
        s+= SAVES[i] + "\n";
    }
    return s;
}

//returns lowercase version of s
string toLower(string s){
    string l = s;
    for (int i = 0; i<l.size(); i++){
        l[i] = tolower(l[i]);
    }
    return l;
}


//helper function to print out list of commands
string help(){
    return "COMMANDS: \n\n\
\"delete <item>\"  .  .  .  .  .  .  . //deletes node <item>\n\
\"add <item>\"  .  .  .  .  .  .  .  . //adds node <item> to graph, or makes usable if already in graph\n\
\"add <item> <dep1> <dep2> ...\"  .  . //adds <item> to graph w/ dependencies <dep1>, <dep2> ... <depN>\n\
                                            or updates existing dependencies\n\
\"sort <-u or -n>\"   .  .  .  .  .  . //sorts graph by usability (default, -u) or name (-n)\n\
\"read <filename>\"   .  .  .  .  .  . //reads a graph from <filename>, or from DEFAULT_FILE if\n\
                      .  .  .  .  .  .      <filename> is not specified.\n\
\"save <filename>\"   .  .  .  .  .  . //saves the current graph to the provided <filename>, or to\n\
                                            the most recently read file if <filename> is not specified.\n\
\"save -l\"  .  .  .  .  .  .  .  .  . //prints out list of files saved since starting program\n\
\"new\"   .  .  .  .  .  .  .  .  .  . //creates a new empty graph\n\
\"file\"  .  .  .  .  .  .  .  .  .  . //prints out current file name, and default file name\n\
\"q\" or \"exit\" .  .  .  .  .  .  .  . //quits the program\n";
}





