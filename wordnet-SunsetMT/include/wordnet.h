#include <set>
#include <string>
#include <vector>
#include <iostream>
#include <map>
#include <fstream>
#include <algorithm>
#include <iterator>

class Digraph {
public:
    void setHyp(const std::map<int, std::vector<int>> &hyp) {
        for (auto &i: hyp) {
            for (auto &j: i.second) {
                parents[i.first].push_back(j);
            }
        }
    }

    const std::vector<int> &get(int id) {
        return parents[id];
    }

private:
    std::map<int, std::vector<int>> parents;
};

class ShortestCommonAncestor {

public:

    ShortestCommonAncestor(Digraph &dg)
            : dg(dg) {}

    ShortestCommonAncestor() {}

    void setHyp(const std::map<int, std::vector<int>> &hyp) {
        dg.setHyp(hyp);
    }

    // calculates length of shortest common ancestor path from node with id 'v' to node with id 'w'
    int length(int v, int w) {
        std::map<int, Node> path;
        buildPath(v, ShortestCommonAncestor::path::v, path, 0);
        buildPath(w, ShortestCommonAncestor::path::w, path, 0);
        int minLen = std::numeric_limits<int>::max();
        for (auto &i: path) {
            if (i.second.vDepth + i.second.wDepth < minLen) {
                minLen = i.second.vDepth + i.second.wDepth;
            }
        }
        return minLen;
    }

    // returns node id of shortest common ancestor of nodes v and w
    int ancestor(int v, int w) {
        std::map<int, Node> path;
        buildPath(v, ShortestCommonAncestor::path::v, path, 0);
        buildPath(w, ShortestCommonAncestor::path::w, path, 0);
        int id = 0;
        int minLen = std::numeric_limits<int>::max();
        for (auto &i: path) {
            if (i.second.vDepth + i.second.vDepth < minLen) {
                minLen = i.second.vDepth + i.second.wDepth;
                id = i.first;
            }
        }
        return id;
    }

    // calculates length of shortest common ancestor path from node subset 'subset_a' to node subset 'subset_b'
    int length_subset(const std::set<int> &subset_a, const std::set<int> &subset_b) {
        int minLen = std::numeric_limits<int>::max();
        for (auto &i: subset_a) {
            for (auto &j: subset_b) {
                int curLen = length(i, j);
                if (curLen < minLen) {
                    minLen = curLen;
                }
            }
        }
        return minLen;
    }

    // returns node id of shortest common ancestor of node subset 'subset_a' and node subset 'subset_b'
    int ancestor_subset(const std::set<int> &subset_a, const std::set<int> &subset_b) {
        int minLen = std::numeric_limits<int>::max();
        int id = std::numeric_limits<int>::max();
        for (auto &i: subset_a) {
            for (auto &j: subset_b) {
                int curLen = length(i, j);
                int curId = ancestor(i, j);
                if (curLen < minLen) {
                    minLen = curLen;
                    id = curId;
                }
            }
        }
        return id;
    }

private:
    Digraph dg;

    enum path{
        v,w
    };

    struct Node {
        int vDepth = std::numeric_limits<int>::max() / 2;
        int wDepth = std::numeric_limits<int>::max() / 2;
    };

    void buildPath(int id, ShortestCommonAncestor::path flag, std::map<int, Node> &Path, int depth) {
        if (flag == path::v) Path[id].vDepth = std::min(Path[id].vDepth, depth);
        else Path[id].wDepth = std::min(Path[id].wDepth, depth);
        for (auto &i: dg.get(id)) {
            buildPath(i, flag, Path, depth + 1);
        }
    }
};

class WordNet {

public:

    WordNet(const std::string &synsets_fn, const std::string &hypernyms_fn) {
        readf(synsets_fn, input::synsets);
        readf(hypernyms_fn, input::hypernyms);
    }

    using Iterator = std::set<std::string>::iterator;

    // get iterator to list all nouns stored in WordNet
    Iterator nouns() {
        return allWords.begin();
    }

    Iterator end() {
        return allWords.end();
    }

    // returns 'true' iff 'word' is stored in WordNet
    bool is_noun(const std::string &word) const {
        return allWords.find(word) != allWords.end();
    }

    // returns gloss of "shortest common ancestor" of noun1 and noun2
    std::string sca(const std::string &noun1, const std::string &noun2) {
        int id = G.ancestor_subset(linkedWords[noun1], linkedWords[noun2]);
        return gloss[id];
    }

    // calculates distance between noun1 and noun2
    int distance(const std::string &noun1, const std::string &noun2) {
        return G.length_subset(linkedWords[noun1], linkedWords[noun2]);
    }

private:
    std::set<std::string> allWords;
    std::map<int, std::string> gloss;
    ShortestCommonAncestor G;
    std::map<std::string, std::set<int>> linkedWords;

    enum input{
        synsets,hypernyms
    };

    std::vector<std::string> split(std::string &str, char delimeter) {
        std::vector<std::string> ans;
        std::string t;
        for (auto &i : str) {
            if (i == delimeter) {
                ans.push_back(t);
                t.clear();
            } else t.push_back(i);
        }
        if (!t.empty()) ans.push_back(t);
        return ans;
    }

    void readf(std::string str, input name) {
        std::fstream fin;
        fin.open(str);
        std::string tmp;
        std::map<int, std::vector<int>> hyp;
        while (std::getline(fin, tmp)) {
            std::vector<std::string> a = split(tmp, ',');
            if (name == input::synsets) {
                int id = std::stol(a.front().c_str());
                gloss[id] = a.back();
                std::vector<std::string> words = split(a[1], ' ');
                for (auto &i: words) {
                    linkedWords[i].insert(id);
                    allWords.insert(i);
                }
            } else {
                std::vector<int> Nums;
                for (auto &i: a) {
                    Nums.push_back(std::stol(i.c_str()));
                }
                int id = Nums.front();

                std::copy(++Nums.begin(), Nums.end(), back_inserter(hyp[id]));
            }
        }
        if (name == input::hypernyms) G.setHyp(hyp);
    }
};

class Outcast {

public:
    Outcast(WordNet &wordnet)
            : m_wordnet(wordnet) {}

    // returns outcast word
    std::string outcast(const std::vector<std::string> &nouns) {
        if (nouns.size() <= 2) return "";
        int maxdist = 0;
        int ans = -1;
        for (std::size_t i = 0; i < nouns.size(); ++i) {
            int dist = 0;
            for (auto &j: nouns) {
                dist += m_wordnet.distance(nouns[i], j);
            }
            if (dist > maxdist) {
                ans = i;
                maxdist = dist;
            }
        }
        return nouns[ans];
    }

private:
    WordNet m_wordnet;
};