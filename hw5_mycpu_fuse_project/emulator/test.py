#!/usr/bin/env python3

import random
import sys

sys.dont_write_bytecode = True

from device import Lram, Pram


def main():
    test_vector = list(range(0, 200))
    random.shuffle(test_vector)

    pram = Pram()
    lram = Lram()

    lram[:] = bytes(test_vector)
    pram[:] = b"[u16:200:400]add(u8:0:100, u8:100:200)"

    pram.run(lram)

    view = memoryview(lram)[200:400].cast("H")
    error_count = 0

    for i in range(len(view)):
        expected = test_vector[i] + test_vector[i + 100]
        if view[i] != expected:
            print("Error:", i, view[i], test_vector[i], test_vector[i + 100])
            error_count += 1

    return error_count


if __name__ == "__main__":
    raise SystemExit(main())