#!/usr/bin/env python3
# Mirror of permutations_backtrack_assign.ad (increment backtracking, println each leaf).

n = 8
a = [0, 0, 0, 0, 0, 0, 0, 0]


def check(k):
    i = 0
    while i < k:
        if a[i] == a[k]:
            return False
        i = i + 1
    return True


def back(k):
    if k == n:
        print(a)
    else:
        a[k] = 1
        while a[k] <= n:
            if check(k):
                back(k + 1)
            a[k] = a[k] + 1


if __name__ == "__main__":
    back(0)
