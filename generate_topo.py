import math
import sys

def main():
    N = 16
    k = 100
    x = 20
    
    if len(sys.argv) > 1:
        N = int(sys.argv[1])
        
    with open('config.txt', 'w') as f:
        f.write(f"N={N}\n")
        f.write(f"k={k}\n")
        f.write(f"x={x}\n")
        
    edges = set()
    
    for i in range(N):
        # Ring connection (predecessor / successor)
        edges.add(tuple(sorted((i, (i + 1) % N))))
        
        # Chord finger connections
        num_fingers = int(math.log2(N))
        for j in range(num_fingers + 1):
            target = (i + int(math.pow(2, j))) % N
            if i != target:
                edges.add(tuple(sorted((i, target))))
            
    with open('topo.txt', 'w') as f:
        for u, v in sorted(list(edges)):
            f.write(f"{u} {v}\n")

if __name__ == '__main__':
    main()
