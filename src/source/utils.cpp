//
// Created by amtoaer on 2021/5/31.
//

#include "../header/utils.h"
#include "../header/view.h"

extern View view;

vector <string> split(string str, string pattern) {
    string::size_type pos;
    vector <string> result;
    str += pattern;
    int size = str.size();

    for (int i = 0; i < size; i++) {
        pos = str.find(pattern, i);
        if (pos < size) {
            string s = str.substr(i, pos - i);
            if (s.length() != 0) {
                result.push_back(s);
            }
            i = pos + pattern.size() - 1;
        }
    }
    return result;
}

string next(string name){
    if (view.cur_path.at(view.cur_path.size()-1)=='/'){
        return view.cur_path+name;
    }
    return view.cur_path+"/"+name;
}
