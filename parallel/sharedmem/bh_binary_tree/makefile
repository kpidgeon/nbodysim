GCCFLAGS = -g -Wall -Wextra -fopenmp -O3

make: DistributionSampler.cpp Vec3D.cpp Particle.cpp Gravity.cpp BinaryBHTree.cpp main.cpp BaseBinaryNode.cpp
	mpicxx $(GCCFLAGS) DistributionSampler.cpp Vec3D.cpp Particle.cpp Gravity.cpp BinaryBHTree.cpp BaseBinaryNode.cpp main.cpp -o main
