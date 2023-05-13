import numpy as np
import os
import sys
from multiprocessing import Pool

class Simulator:
    def __init__(self, alpha=0.2, beta=10) -> None:
        self.p_normal = 0.99
        self.alpha = alpha
        self.beta = beta

        self.s_init =  1
        self.s_max = 1
        self.s_pun = self.s_init / self.beta
        self.s_awd = self.s_pun / self.p_normal * (1 - self.p_normal) / self.alpha

        print("awd: ", self.s_awd)
        print("pun: ", self.s_pun)

        lt_bound = (self.alpha * self.p_normal) / (1 + (self.alpha - 1) * self.p_normal)
        st_bound = self.alpha * self.beta * self.p_normal / (1-self.p_normal) + 2 * self.beta
        print(lt_bound, st_bound)
    def simulate_one(self, rounds: int) -> bool:
        score = self.s_init
        for i in range(rounds):
            rd = np.random.random()
            if rd > self.p_normal:
                score -= self.s_pun
            else:
                score += self.s_awd
                if score >= self.s_max:
                    score = self.s_max
            if score < 0:
                return i + 1
        return rounds

    def simulate_all(self, num_nodes: int, cores: int=15) -> float:
        rounds = 86400
        rounds_arr = [rounds] * num_nodes

        results = None
        with Pool(cores) as pool:
            results = pool.map(self.simulate_one, rounds_arr)

        results = np.array(results)
        survive_rate = np.count_nonzero(results == rounds) / num_nodes
        avg_survive = np.mean(results) / rounds
        print(survive_rate, avg_survive)
        

def main():
    alpha = sys.argv[1]
    beta = sys.argv[2]
    sim = Simulator(alpha, beta)

    sim.simulate_all(10000)

if __name__ == "__main__":
    main()