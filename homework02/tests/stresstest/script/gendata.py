from sys import argv, exit
from os import chdir, getcwd


FILES_QTY = 500
FILE_MIN_SIZE = 2**20 # 1Mb
INIT_MATCH_QTY = 1
STEP_MATCH_QTY = 1


def gen_str(f, token: str, qty: int, min_size: int):
    sep_len = ((min_size - len(token) * qty) // qty)
    for _ in range(qty):
        f.write(token)
        for _ in range(sep_len):
            f.write('_')


if __name__ == '__main__':
    if len(argv) < 3:
        print(f"Usage:\n\t{argv[0]} <path> <token>")
        exit(1)

    init_cwd = getcwd()
    chdir(argv[1])

    match_qty = INIT_MATCH_QTY
    step = STEP_MATCH_QTY

    for i in range(FILES_QTY):
        with open(f'matches_{match_qty}.txt', 'w') as f:
            gen_str(f, argv[2], match_qty, FILE_MIN_SIZE)
        match_qty += step

    chdir(init_cwd)
