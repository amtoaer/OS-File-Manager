//
// Created by lenovo on 2021-06-03.
//

#include "../header/inode.h"

bool Dinode::canWrite(int id) {
    if (rwct.user_id == id) return true;
    else if (find(rwct.raw_group.begin(), rwct.raw_group.end(), id) != rwct.raw_group.end()) {
        return true;
    } else if (find(rwct.w_group.begin(), rwct.w_group.end(), id) != rwct.w_group.end()) {
        return true;
    }
    return false;
}

bool Dinode::canRead(int id) {
    if (rwct.user_id == id) return true;
    else if (find(rwct.raw_group.begin(), rwct.raw_group.end(), id) != rwct.raw_group.end()) {
        return true;
    } else if (find(rwct.r_group.begin(), rwct.r_group.end(), id) != rwct.r_group.end()) {
        return true;
    }
    return false;
}
