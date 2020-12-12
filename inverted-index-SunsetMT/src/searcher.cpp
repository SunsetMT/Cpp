#include "searcher.h"




bool quoteSearch(int startNum, const std::map<std::string, std::vector<std::pair<std::string, int>>> &m, int moves,
                 int cur,
                 const std::vector<std::string> &words, const std::string &filename)  {

    if (cur == moves) {
        return true;
    }
    else {

        if(m.find(words[cur]) != m.end()){
            for (auto &z: m.at(words[cur])) {
                if (z.second == startNum && z.first == filename) {
                    if(quoteSearch(startNum + 1, m, moves, cur + 1, words, filename)) return  true;
                }
            }
        }


        for (auto &j: m) {
            if (words[cur] == j.first) {
                for (auto &z: j.second) {
                    if (z.second == startNum && z.first == filename) {
                        if(quoteSearch(startNum + 1, m, moves, cur + 1, words, filename)) return  true;
                    }
                }
            }
        }
        return false;
    }
}

std::vector<std::string> getWords(std::string t)  {
    std::vector<std::string> strs;
    std::string temp;
    for (std::size_t i = 0; i < t.size(); i++) {
        if (std::isspace(t[i]) || std::ispunct(t[i])) {
            t.erase(t.begin() + i);
            i--;
            continue;
        }
        if (std::isalnum(t[i])) {
            while (!std::isspace(t[i])) {
                temp.push_back(t[i]);
                i++;
                if (i == t.size()) break;
            }
            while (std::ispunct(temp.back())) {
                t.erase(t.begin() + --i);
                temp.pop_back();
            }
            strs.push_back(temp);
            temp = "";
        }
    }
    return strs;
}


void intersect(std::set<std::string> &s1, std::set<std::string> &s2, std::vector<std::string> &s3)  {
    std::set_intersection(s1.begin(), s1.end(),
                          s2.begin(), s2.end(),
                          std::back_inserter(s3));
    s1.clear();
    for (auto &j :s3) {
        s1.insert(j);
    }
    s2.clear();
    s3.clear();
}




void Searcher::add_document(const Searcher::Filename &filename, std::istream &strm) {
    for(auto & i : DocsName){
        if(filename == i){
            remove_document(filename);
        }
    }
    DocsName.push_back(filename);
    std::string t;
    std::string t1;
    while(strm >> t1){
        t.push_back(' ');
        t += t1;
    }

    std::vector<std::string> words = getWords(t);

    for(size_t i = 0; i < words.size(); i++){
        Num[words[i]].push_back({filename, i});
    }
    if(t.empty()){
        Num[""].push_back({filename, 0});
        return;
    }
}

void Searcher::remove_document(const Searcher::Filename &filename) {
    std::string t;
    for(std::size_t i = 0; i < DocsName.size(); i++) {
        if (filename == DocsName[i]) {
            DocsName.erase(DocsName.begin() + i);
        }
    }
    std::string temp;

    for(auto &item: Num){
        item.second.erase(std::remove_if(item.second.begin(), item.second.end(),
                                         [filename](std::pair <std::string, int> pair) {return pair.first == filename; }), item.second.end());
    }
}




std::pair<Searcher::DocIterator, Searcher::DocIterator> Searcher::search(const std::string &query) const {
    int numOfSpecSymb = 0;
    bool emptyCheck = true;
    for (size_t i = 0; i < query.size(); i++) {
        if (query[i] == '\"') numOfSpecSymb++;
    }
    for (size_t i = 0; i < query.size(); i++) {
        if (!(isspace(query[i]) || ispunct(query[i]))) {
            emptyCheck = false;
            break;
        }
    }
    if (numOfSpecSymb % 2 != 0) throw Searcher::BadQuery("Bad Input");
    if (emptyCheck) throw Searcher::BadQuery("trying to find empty string ("")");
    std::vector<std::string> que;
    std::vector<std::string> quotes;
    std::string temp;
    std::string buff;


    //обработка ковычек
    for (std::size_t i = 0; i < query.size(); i++) {
        if (query[i] == '\"') {
            i++;
            while (i < query.size()) {
                if (query[i] == '\"') {
                    break;
                }
                buff += query[i];
                i++;
            }
            quotes.push_back(buff);
            buff.clear();
        }
        if (std::isspace(query[i]) || std::ispunct(query[i])) continue;
        if (std::isalnum(query[i])) {
            while (!std::isspace(query[i])) {
                temp.push_back(query[i]);
                i++;
                if (i == query.size()) break;
            }
            while (std::ispunct(temp.back())) {
                temp.pop_back();
            }
            que.push_back(temp);
            temp = "";
        }
    }

    std::set<std::string> ans1;
    std::set<std::string> ans2;
    std::vector<std::string> FinalAns;
    temp = "";
    std::vector<std::vector<std::string>> extendedQuotes; // цитаты разбитые на слова

    //удаление знаков пунктуации из цитат
    for (const auto &i: quotes) {
        std::vector<std::string> words = getWords(i);
        extendedQuotes.push_back(words);
    }

    int cnt = 0;
    for (size_t i = 0; i < que.size(); i++) {
        //std::map<std::string, std::vector<std::pair<std::string, int>>> k = Num;
        if(Num.find(que[i]) != Num.end()) {
            if (cnt == 0) {
                for (auto &iter: Num.at((que.at(i)))) {
                    ans1.insert(iter.first);
                }
                cnt++;
            } else {
                for (auto &iter: Num.at((que.at(i)))) {
                    ans2.insert(iter.first);
                }
                intersect(ans1, ans2, FinalAns);
            }
        }
    }
    for (std::size_t i = 0; i < extendedQuotes.size(); i++) {
        int curStart = 0;

        if(Num.find(extendedQuotes[i].front()) != Num.end()){
            for(auto &z: Num.at(extendedQuotes[i].front())){
                curStart = z.second; // первое слово i-ой цитаты имеет номер curStart в файле z.first
                bool c1 = false;
                c1 = quoteSearch(curStart + 1, Num, extendedQuotes[i].size(), 1, extendedQuotes[i], z.first);
                if (c1) {
                    ans2.insert(z.first);
                }
            }
        }
    }

    if (!que.empty() && !extendedQuotes.empty()) {
        intersect(ans1, ans2, FinalAns);
    }
    if (!extendedQuotes.empty() && que.empty()) {
        ans1 = ans2;
    }

    Searcher::DocIterator ansIt1;
    Searcher::DocIterator ansIt2;
    for (auto &i: ans1) {
        ansIt1.push(i);
    }
    ansIt2 = Searcher::DocIterator(ansIt1, ans1.size());

    return {ansIt1, ansIt2};
}
