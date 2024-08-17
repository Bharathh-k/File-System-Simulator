#include <iostream>
#include <unordered_map>
#include <vector>
#include <string>
#include <stack>

using namespace std;

class Node {
public:
    string name;
    bool isFile;
    string content;  
    unordered_map<string, Node*> children;

    Node(string name, bool isFile) : name(name), isFile(isFile), content("") {}
};

class FileSystem {
private:
    Node* root;
    stack<Node*> undoStack; 

public:
    FileSystem() {
        root = new Node("/", false);  
    }

    vector<string> splitPath(const string& path) {
        vector<string> result;
        size_t start = 0, end = 0;
        while ((end = path.find('/', start)) != string::npos) {
            if (end != start) {
                result.push_back(path.substr(start, end - start));
            }
            start = end + 1;
        }
        if (start < path.size()) {
            result.push_back(path.substr(start));
        }
        return result;
    }

    Node* navigateTo(const string& path) {
        vector<string> components = splitPath(path);
        Node* current = root;
        for (const string& component : components) {
            if (current->children.find(component) != current->children.end()) {
                current = current->children[component];
            } else {
                cout << "Directory or file not found: " << component << endl;
                return nullptr;
            }
        }
        return current;
    }

    void create(string path, bool isFile) {
        Node* parent = navigateTo("/");
        vector<string> components = splitPath(path);
        string newName = components.back();
        components.pop_back();

        for (const string& component : components) {
            if (parent->children.find(component) == parent->children.end()) {
                cout << "Invalid path component: " << component << endl;
                return;
            }
            parent = parent->children[component];
        }

        if (parent->children.find(newName) != parent->children.end()) {
            cout << (isFile ? "File" : "Directory") << " already exists: " << newName << endl;
            return;
        }

        Node* newNode = new Node(newName, isFile);
        parent->children[newName] = newNode;
        cout << (isFile ? "File" : "Directory") << " created: " << newName << endl;
    }

    void deleteNode(string path) {
        Node* parent = navigateTo("/");
        vector<string> components = splitPath(path);
        string targetName = components.back();
        components.pop_back();

        for (const string& component : components) {
            if (parent->children.find(component) == parent->children.end()) {
                cout << "Invalid path component: " << component << endl;
                return;
            }
            parent = parent->children[component];
        }

        if (parent->children.find(targetName) == parent->children.end()) {
            cout << "File or directory not found: " << targetName << endl;
            return;
        }

        undoStack.push(parent->children[targetName]);
        parent->children.erase(targetName);
        cout << "Deleted: " << targetName << endl;
    }

    void display(Node* node, string indent = "") {
        cout << indent << (node->isFile ? "File: " : "Dir: ") << node->name << endl;
        for (auto it = node->children.begin(); it != node->children.end(); ++it) {
            display(it->second, indent + "  ");
        }
    }


    void display() {
        display(root);
    }

    void undoDelete() {
        if (!undoStack.empty()) {
            Node* node = undoStack.top();
            undoStack.pop();
            Node* parent = navigateTo("/");
            parent->children[node->name] = node;
            cout << "Undo successful, restored: " << node->name << endl;
        } else {
            cout << "Nothing to undo!" << endl;
        }
    }
};

int main() {
    FileSystem fs;

    fs.create("/documents", false);
    fs.create("/documents/photos", false);
    fs.create("/music", false);

    fs.create("/documents/file1.txt", true);
    fs.create("/documents/photos/photo1.png", true);
    fs.create("/music/song1.mp3", true);

    cout << "File system structure:" << endl;
    fs.display();

    fs.deleteNode("/documents/file1.txt");

    fs.undoDelete();

    cout << "\nFile system after undo:" << endl;
    fs.display();

    return 0;
}
