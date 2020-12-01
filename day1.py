import itertools
import math
from typing import Iterable, Tuple, List


def sums_to_2020(xs: Iterable[int]) -> Tuple[int, int]:
    for (i1, i2) in itertools.combinations(xs, 2):
        if i1 + i2 == 2020:
            return (i1, i2)
    raise ValueError("no numbers supplied add to 2020")

def sums_to_2020_part_2(xs: Iterable[int]) -> Tuple[int, int, int]:
    for items in itertools.combinations(xs, 3):
        if sum(items) == 2020:
            return items
    raise ValueError("no numbers supplied add to 2020")


def day1():
    with open("./day1_input.txt") as d1:
        lines = d1.readlines()
    nums = [int(l) for l in lines]
    res: Tuple[int, int] = sums_to_2020(nums)
    print(res, ":", res[0] * res[1])
    # part 2
    res2 : Tuple[int, int, int] = sums_to_2020_part_2(nums)
    print(res2, ":", math.prod(res2))


if __name__ == "__main__":
    day1()
