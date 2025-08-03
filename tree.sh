#!/bin/bash
tree -I 'build|.git|*.o|*.obj|*.pyc|__pycache__' -L 7 | wl-copy
