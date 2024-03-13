//Задание 1
#include <iostream>
#include <fstream>
#include <map>
#include <string>
#include <algorithm>
#include <vector>
#include <omp.h>

using namespace std;

typedef map<string, int> StrIntMap;

void cleanWord(string& word) {
    word.erase(remove_if(word.begin(), word.end(), [](char c) { return !isalnum(c); }), word.end());
}

void countWords(ifstream& in, StrIntMap& words) {
    string s;
    while (in >> s) {
        cleanWord(s);
#pragma omp critical
        ++words[s];
    }
}

bool sortByValue(const pair<string, int>& a, const pair<string, int>& b) {
    return a.second > b.second;
}

int main() {
    string filename = "text.txt";
    ifstream in(filename);

    if (!in) {
        cout << "error" << endl;
        exit(EXIT_FAILURE);
    }

    StrIntMap w;

#pragma omp parallel
    {
#pragma omp for
        countWords(in, w);
    }

    vector<pair<string, int>> sortedWords(w.begin(), w.end());
    sort(sortedWords.begin(), sortedWords.end(), sortByValue);

    for (const auto& pair : sortedWords) {
        cout << pair.first << " : " << pair.second << " times " << endl;
    }

    return 0;
}
