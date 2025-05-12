#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <stack>
#include <random>
#include <limits>
#include <iomanip>
#include <sstream>
#include <algorithm>

using namespace std;

class WordGraph {
private:
    struct Edge {
        string target;
        int weight;
        Edge(string t, int w) : target(t), weight(w) {}
    };

    unordered_map<string, vector<Edge>> adjList;
    unordered_set<string> words;

    // 辅助函数
    vector<string> splitText(const string& text) const {
        vector<string> tokens;
        string token;
        for (char c : text) {
            if (isalpha(c)) {
                token += tolower(c);
            }
            else if (c == ' ' || c == '\n') {
                if (!token.empty()) {
                    tokens.push_back(token);
                    token.clear();
                }
            }
        }
        if (!token.empty()) {
            tokens.push_back(token);
        }
        return tokens;
    }

    string preprocessWord(const string& word) const {
        string result;
        for (char c : word) {
            if (isalpha(c)) {
                result += tolower(c);
            }
        }
        return result;
    }

    void addEdge(const string& src, const string& dest) {
        for (auto& edge : adjList[src]) {
            if (edge.target == dest) {
                edge.weight++;
                return;
            }
        }
        adjList[src].emplace_back(dest, 1);
    }

public:
    // 构建图
    void buildGraphFromFile(const string& filename) {
        ifstream file(filename);
        if (!file.is_open()) {
            cerr << "Error: Could not open file " << filename << endl;
            return;
        }

        string line;
        string fullText;
        while (getline(file, line)) {
            fullText += line + " ";
        }
        file.close();

        vector<string> tokens = splitText(fullText);
        if (tokens.empty()) return;

        for (size_t i = 0; i < tokens.size() - 1; ++i) {
            string word1 = preprocessWord(tokens[i]);
            string word2 = preprocessWord(tokens[i + 1]);

            if (!word1.empty() && !word2.empty()) {
                words.insert(word1);
                words.insert(word2);
                addEdge(word1, word2);
            }
        }
    }

    // 展示有向图（邻接表）
    void showDirectedGraph() const {
        cout << "Directed Graph:" << endl;
        cout << "----------------" << endl;
        for (const auto& entry : adjList) {
            cout << entry.first << " -> ";
            for (const auto& edge : entry.second) {
                cout << edge.target << "(" << edge.weight << ") ";
            }
            cout << endl;
        }
        cout << "----------------" << endl;
    }

    // 查询桥接词
    string queryBridgeWords(const string& word1, const string& word2) const {
        string w1 = preprocessWord(word1);
        string w2 = preprocessWord(word2);

        if (words.find(w1) == words.end() && words.find(w2) == words.end()) {
            return "No \"" + word1 + "\" and \"" + word2 + "\" in the graph!";
        }
        else if (words.find(w1) == words.end()) {
            return "No \"" + word1 + "\" in the graph!";
        }
        else if (words.find(w2) == words.end()) {
            return "No \"" + word2 + "\" in the graph!";
        }

        vector<string> bridgeWords;
        if (adjList.find(w1) != adjList.end()) {
            for (const auto& edge1 : adjList.at(w1)) {
                string potentialBridge = edge1.target;
                if (adjList.find(potentialBridge) != adjList.end()) {
                    for (const auto& edge2 : adjList.at(potentialBridge)) {
                        if (edge2.target == w2) {
                            bridgeWords.push_back(potentialBridge);
                            break;
                        }
                    }
                }
            }
        }

        if (bridgeWords.empty()) {
            return "No bridge words from \"" + word1 + "\" to \"" + word2 + "\"!";
        }
        else if (bridgeWords.size() == 1) {
            return "The bridge word from \"" + word1 + "\" to \"" + word2 + "\" is: " + bridgeWords[0];
        }
        else {
            string result = "The bridge words from \"" + word1 + "\" to \"" + word2 + "\" are: ";
            for (size_t i = 0; i < bridgeWords.size(); ++i) {
                if (i == bridgeWords.size() - 1) {
                    result += "and " + bridgeWords[i];
                }
                else {
                    result += bridgeWords[i] + ", ";
                }
            }
            return result;
        }
    }

    // 生成新文本
    string generateNewText(const string& inputText) const {
        vector<string> tokens = splitText(inputText);
        if (tokens.size() < 2) return inputText;

        string result;
        random_device rd;
        mt19937 gen(rd());

        for (size_t i = 0; i < tokens.size() - 1; ++i) {
            string word1 = preprocessWord(tokens[i]);
            string word2 = preprocessWord(tokens[i + 1]);

            result += tokens[i] + " ";

            if (words.find(word1) != words.end() && words.find(word2) != words.end()) {
                vector<string> bridges;
                if (adjList.find(word1) != adjList.end()) {
                    for (const auto& edge1 : adjList.at(word1)) {
                        string potentialBridge = edge1.target;
                        if (adjList.find(potentialBridge) != adjList.end()) {
                            for (const auto& edge2 : adjList.at(potentialBridge)) {
                                if (edge2.target == word2) {
                                    bridges.push_back(potentialBridge);
                                    break;
                                }
                            }
                        }
                    }
                }

                if (!bridges.empty()) {
                    uniform_int_distribution<> dis(0, bridges.size() - 1);
                    string selected = bridges[dis(gen)];
                    result += selected + " ";
                }
            }
        }
        result += tokens.back();
        return result;
    }

    // 计算最短路径(Dijkstra算法)
    string calcShortestPath(const string& word1, const string& word2) const {
        string w1 = preprocessWord(word1);
        string w2 = preprocessWord(word2);

        if (words.find(w1) == words.end() || words.find(w2) == words.end()) {
            return "One or both words not found in the graph!";
        }

        unordered_map<string, int> distances;
        unordered_map<string, string> predecessors;
        for (const auto& word : words) {
            distances[word] = numeric_limits<int>::max();
        }
        distances[w1] = 0;

        priority_queue<pair<int, string>, vector<pair<int, string>>, greater<pair<int, string>>> pq;
        pq.push({ 0, w1 });

        while (!pq.empty()) {
            string current = pq.top().second;
            int currentDist = pq.top().first;
            pq.pop();

            if (currentDist > distances[current]) continue;

            if (adjList.find(current) != adjList.end()) {
                for (const auto& edge : adjList.at(current)) {
                    string neighbor = edge.target;
                    int newDist = currentDist + edge.weight;

                    if (newDist < distances[neighbor]) {
                        distances[neighbor] = newDist;
                        predecessors[neighbor] = current;
                        pq.push({ newDist, neighbor });
                    }
                }
            }
        }

        if (distances[w2] == numeric_limits<int>::max()) {
            return "No path from \"" + word1 + "\" to \"" + word2 + "\"!";
        }

        stack<string> path;
        string current = w2;
        while (current != w1) {
            path.push(current);
            current = predecessors[current];
        }
        path.push(w1);

        stringstream ss;
        ss << "The shortest path from \"" << word1 << "\" to \"" << word2 << "\" is: ";
        while (!path.empty()) {
            ss << path.top();
            path.pop();
            if (!path.empty()) ss << " -> ";
        }
        ss << " (length: " << distances[w2] << ")";
        return ss.str();
    }

    // 计算PageRank
    double calcPageRank(const string& word, int iterations = 100, double damping = 0.85) const {
        string w = preprocessWord(word);
        if (words.find(w) == words.end()) return -1.0;

        unordered_map<string, double> pr;
        unordered_map<string, double> newPr;
        const double N = words.size();
        const double initialValue = 1.0 / N;

        for (const auto& wd : words) {
            pr[wd] = initialValue;
        }

        for (int iter = 0; iter < iterations; ++iter) {
            double danglingSum = 0.0;

            for (const auto& wd : words) {
                if (adjList.find(wd) == adjList.end() || adjList.at(wd).empty()) {
                    danglingSum += pr[wd] / N;
                }
            }

            for (const auto& wd : words) {
                double sum = 0.0;

                for (const auto& entry : adjList) {
                    const string& src = entry.first;
                    for (const auto& edge : entry.second) {
                        if (edge.target == wd) {
                            sum += pr[src] / adjList.at(src).size();
                        }
                    }
                }

                newPr[wd] = (1.0 - damping) / N + damping * (sum + danglingSum);
            }

            pr = newPr;
        }

        return pr[w];
    }

    // 随机游走（原功能保留）
    string randomWalk() const {
        if (words.empty()) return "";

        random_device rd;
        mt19937 gen(rd());
        uniform_int_distribution<> dis(0, words.size() - 1);

        auto it = words.begin();
        advance(it, dis(gen));
        string current = *it;

        unordered_set<string> visitedEdges;
        vector<string> path;
        path.push_back(current);

        while (true) {
            if (adjList.find(current) == adjList.end() || adjList.at(current).empty()) {
                break;
            }

            const auto& edges = adjList.at(current);
            uniform_int_distribution<> edgeDis(0, edges.size() - 1);
            int idx = edgeDis(gen);
            const Edge& chosenEdge = edges[idx];

            string edgeKey = current + "->" + chosenEdge.target;
            if (visitedEdges.find(edgeKey) != visitedEdges.end()) {
                break;
            }

            visitedEdges.insert(edgeKey);
            current = chosenEdge.target;
            path.push_back(current);
        }

        string result;
        for (size_t i = 0; i < path.size(); ++i) {
            if (i != 0) result += " ";
            result += path[i];
        }

        return result;
    }

    // ==== 新增文件输出功能 ====
    bool writePathToFile(const vector<string>& path, const string& filename) const {
        // 检查文件名合法性
        if (filename.empty() || filename.find_first_of("\\/:*?\"<>|") != string::npos) {
            cerr << "Error: Invalid filename" << endl;
            return false;
        }

        ofstream outFile(filename);
        if (!outFile.is_open()) {
            cerr << "Error: Unable to open file " << filename << endl;
            return false;
        }

        // 批量写入优化
        stringstream buffer;
        for (const auto& node : path) {
            buffer << node << " ";
        }
        string content = buffer.str();
        if (!content.empty()) {
            content.pop_back(); // 移除末尾空格
        }
        outFile << content;

        outFile.close();
        return true;
    }

    string randomWalkToFile(const string& filename = "") const {
        string walkResult = randomWalk();

        if (!filename.empty()) {
            vector<string> nodes;
            istringstream iss(walkResult);
            string node;
            while (iss >> node) {
                nodes.push_back(node);
            }

            if (!writePathToFile(nodes, filename)) {
                cerr << "Warning: Failed to write walk result to file" << endl;
            }
        }

        return walkResult;
    }
    
};

int main(int argc, char* argv[]) {
    WordGraph graph;

    if (argc < 2) {
        cout << "Usage: " << argv[0] << " <filename>" << endl;
        return 1;
    }

    string filename = argv[1];
    graph.buildGraphFromFile(filename);

    int choice;
    do {
        cout << "\n===== Text Graph Processor =====" << endl;
        cout << "1. Show directed graph" << endl;
        cout << "2. Query bridge words" << endl;
        cout << "3. Generate new text" << endl;
        cout << "4. Calculate shortest path" << endl;
        cout << "5. Calculate PageRank" << endl;
        cout << "6. Random walk (with file output)" << endl;
        cout << "0. Exit" << endl;
        cout << "Enter your choice: ";
        cin >> choice;

        cin.ignore(); // 清除输入缓冲区

        switch (choice) {
        case 1:
            graph.showDirectedGraph();
            break;
        case 2: {
            string word1, word2;
            cout << "Enter word1: ";
            getline(cin, word1);
            cout << "Enter word2: ";
            getline(cin, word2);
            cout << graph.queryBridgeWords(word1, word2) << endl;
            break;
        }
        case 3: {
            string inputText;
            cout << "Enter text: ";
            getline(cin, inputText);
            cout << "Generated text: " << graph.generateNewText(inputText) << endl;
            break;
        }
        case 4: {
            string word1, word2;
            cout << "Enter word1: ";
            getline(cin, word1);
            cout << "Enter word2: ";
            getline(cin, word2);
            cout << graph.calcShortestPath(word1, word2) << endl;
            break;
        }
        case 5: {
            string word;
            cout << "Enter word: ";
            getline(cin, word);
            double pr = graph.calcPageRank(word);
            if (pr >= 0) {
                cout << "PageRank of \"" << word << "\": " << fixed << setprecision(4) << pr << endl;
            }
            else {
                cout << "Word not found in graph!" << endl;
            }
            break;
        }
        case 6: {
            string filename;
            cout << "Enter output filename (leave empty for screen only): ";
            getline(cin, filename);

            string walk = graph.randomWalkToFile(filename);
            cout << "Random walk: " << walk << endl;
            if (!filename.empty()) {
                cout << "Results saved to " << filename << endl;
            }
            break;
        }
        case 0:
            cout << "Exiting..." << endl;
            break;
        default:
            cout << "Invalid choice. Try again." << endl;
        }
    } while (choice != 0);

    return 0;
}