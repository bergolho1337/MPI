import sys
import numpy as np
import matplotlib.pyplot as plt

TOTAL_NUMBER_SIMULATIONS = 16

def plot_functions():

    for i in range(TOTAL_NUMBER_SIMULATIONS):
        # Get the data from the output file
        data = np.genfromtxt("../outputs/simulation-%d.dat" % (i))

        # Plot the curve
        plt.plot(data[:,0],data[:,1],label="sim=%d" % i, linewidth=3.0)

    plt.grid()
    plt.xlabel("x",fontsize=15)
    plt.ylabel("y",fontsize=15)
    plt.title("Simulation comparison",fontsize=14)
    plt.legend(loc=0,fontsize=14)
    plt.show()
	#plt.savefig("ap.pdf")


def main():
	
	if len(sys.argv) != 1:
		print("-------------------------------------------------------------------------")
		print("Usage:> python %s" % sys.argv[0])
		print("-------------------------------------------------------------------------")
		return 1

	plot_functions()


if __name__ == "__main__":
	main()
