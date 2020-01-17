import os
import sys

path = sys.argv[1]

files = os.listdir(path)

for idx, file in enumerate(files):
    os.rename(os.path.join(path, file), os.path.join(path, ''.join(["{:05d}".format(idx), '_', file])))
