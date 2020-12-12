#pragma once

#include <string>
#include <utility>
#include <stdexcept>
#include <vector>
#include <map>
#include <memory>
#include <algorithm>
#include <set>
#include <iostream>
#include <fstream>


class Searcher {
public:
    using Filename = std::string; // or std::filesystem::path

    // index modification
    void add_document(const Filename &filename, std::istream &strm);

    void remove_document(const Filename &filename);


    // queries
    class DocIterator {
    public:



        using difference_type = std::ptrdiff_t;
        using value_type = const std::string;
        using pointer = value_type *;
        using reference = value_type &;
        using iterator_category = std::forward_iterator_tag;

        friend bool operator==(const DocIterator &lhs, const DocIterator &rhs) {
            return lhs.m_vec == rhs.m_vec && lhs.m_current == rhs.m_current;
        }

        friend bool operator!=(const DocIterator &lhs, const DocIterator &rhs) {
            return !(lhs == rhs);
        }

        reference operator*() const {
            return m_vec[m_current];
        }

        DocIterator operator++(int) {
            auto tmp = *this;
            operator++();
            return tmp;
        }

        DocIterator &operator++() { //pre
            m_current++;
            return *this;
        }

        friend class Searcher;

    private:
        DocIterator() {}

        DocIterator(const DocIterator &it, size_t pos) {
            m_vec = it.m_vec;
            m_current = pos;
        }

        void push(const std::string &s) {
            m_vec.push_back(s);
        }
        std::vector<std::string> m_vec;
        std::size_t m_current = 0;
    };


    class BadQuery : public std::exception {
    private:
        std::string m_error = "Search query syntax error:";
    public:
        BadQuery(std::string error)
                : m_error(error) {}

        const char *what() const noexcept { return m_error.c_str(); }
    };


    std::pair<DocIterator, DocIterator> search(const std::string &query) const;

private:


    std::vector<std::string> DocsName;
    std::map<std::string, std::vector<std::pair<std::string, int>>> Num;
};
