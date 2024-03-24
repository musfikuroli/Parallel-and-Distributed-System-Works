generateInput:
	g++ input_generator.c -o input_generator
	./input_generator 1200

pipelineFloyd:
	cd Pipeline && mpicc -o pipelineFloyd pipelineFloyd.c -lm && mpirun -np 4 --oversubscribe ./pipelineFloyd

parallelFloyd:
	cd Parallel && mpicc -o parallelFloyd parallelFloyd.c -lm && mpirun -np 4 --oversubscribe ./parallelFloyd

serialFloyd:
	cd Serial && mpicc -o serialFloyd serialFloyd.c -lm && mpirun -np 1 --oversubscribe ./serialFloyd