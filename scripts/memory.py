import argparse

GAMEBOY_MEMORY_OFFSETS = [
    { "name": "ROM Bank 00", "offset": 0, "length": 16 * (1 << 10) },
]

class GameboyMemory:
    def __read_number__(raw: bytearray, address: int, size: int) -> int:
        return int.from_bytes(raw[address:address+size], 'little')

    def __parse_reset_vectors__(self, raw: bytearray):
        self.reset_vectors = []
        for vector in range(8):
            vector_value = GameboyMemory.__read_number__(raw, vector * 8, 2)
            self.reset_vectors.append(vector_value)

    def __init__(self, filename):
        raw = bytearray(open(filename, "rb").read())
        self.__parse_reset_vectors__(raw)


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("-i", "--input_file", help="Path to gameboy memory dump")
    args = parser.parse_args()

    gameboy_mem = GameboyMemory(args.input_file)
    print(gameboy_mem.reset_vectors)

    return 0

if __name__ == "__main__":
    main()