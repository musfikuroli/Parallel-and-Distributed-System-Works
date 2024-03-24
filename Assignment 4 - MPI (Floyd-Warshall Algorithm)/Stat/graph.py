import subprocess
import matplotlib.pyplot as plt

def compare_files(file1_path, file2_path):
    # Read the contents of the first file
    with open(file1_path, 'r') as file1:
        content1 = file1.read()

    # Read the contents of the second file
    with open(file2_path, 'r') as file2:
        content2 = file2.read()

    # Check if the files have the same content
    return content1 == content2


def clean():
    clean_serial = 'cd ../Serial && rm -rf stat.txt'
    clean_parallel = 'cd ../Parallel && rm -rf stat.txt'
    clean_pipeline = 'cd ../Pipeline && rm -rf stat.txt'
    subprocess.run(clean_serial, shell=True, capture_output=True, text=True)
    subprocess.run(clean_parallel, shell=True, capture_output=True, text=True)
    subprocess.run(clean_pipeline, shell=True, capture_output=True, text=True)

def generateInput(n):
    command = 'cd .. && g++-13 input_generator.c && ./a.out '+str(n)
    result = subprocess.run(command, shell=True, capture_output=True, text=True)
    print("Output:", result.stdout)
    print("Return Code:", result.returncode)

def runSerial(n):
    command = 'cd ../Serial && mpicc -o serialFloyd serialFloyd.c -lm && mpirun -np 1 ./serialFloyd '+str(n)
    result = subprocess.run(command, shell=True, capture_output=True, text=True)
    print("Output:", result.stdout)
    print("Return Code:", result.returncode)


def runParallel(n,p):
    command = 'cd ../Parallel && mpicc -o parallelFloyd parallelFloyd.c -lm && mpirun --oversubscribe -np '+str(p)+' ./parallelFloyd '+str(n)
    result = subprocess.run(command, shell=True, capture_output=True, text=True)
    print("Output:", result.stdout)
    print("Return Code:", result.returncode)


def runPipeline(n,p):
    command = 'cd ../Pipeline && mpicc -o pipelineFloyd pipelineFloyd.c -lm && mpirun --oversubscribe -np '+str(p)+' ./pipelineFloyd '+str(n)
    result = subprocess.run(command, shell=True, capture_output=True, text=True)
    print("Output:", result.stdout)
    print("Return Code:", result.returncode)

def generateStat():
    n = [240,480,720,960,1200]
    p = [4,9,16,25,36]
    clean()
    for nodes in n:
        print("Current number of nodes ", nodes)
        generateInput(nodes)
        runSerial(nodes)
        for processes in p:
            runParallel(nodes,processes)
            runPipeline(nodes,processes)
            if compare_files('../Serial/output.txt','../Parallel/output.txt') == True :
                print("Output of ../Serial/output.txt and ../Parallel/output.txt are equal")
            else :
                print("Output of ../Serial/output.txt and ../Parallel/output.txt are not equal")

            if compare_files('../Serial/output.txt','../Pipeline/output.txt') == True :
                print("Output of ../Serial/output.txt and ../Pipeline/output.txt are equal")
            else :
                print("Output of ../Serial/output.txt and ../Pipeline/output.txt are not equal")
            #print("Ran parallel and pipeline with number of processors:",processes)
            #input("Press enter to continue")

def parse_data(file_path):
    # Initialize an empty dictionary to store the parsed data
    data = {}

    # Read the file and parse the content
    with open(file_path, 'r') as file:
        lines = file.readlines()

    for line in lines:
        # Split the line into key and value parts
        parts = line.strip().split()

        # Convert key and inner_key to appropriate types
        num_of_nodes = int(parts[0])
        num_of_processors = int(parts[1])
        runtime = float(parts[2])

        # Create or update the nested dictionary
        if num_of_nodes not in data:
            data[num_of_nodes] = {}
        data[num_of_nodes][num_of_processors] = runtime

    return data

def showGraph(file_path):
    parallel_data = parse_data(file_path)
    serial_data = parse_data('../Serial/stat.txt')
    for num_of_nodes, inner_data in parallel_data.items():
        for num_of_processors,_ in inner_data.items():
            inner_data[num_of_processors] = serial_data[num_of_nodes][1]/inner_data[num_of_processors]
        inner_keys = list(inner_data.keys())
        speedup = list(inner_data.values())
        
        # Determine the color of the line
        line_color = plt.gca()._get_lines.get_next_color()
        
        # Plot the line graph
        plt.plot(inner_keys, speedup, label=f'Number of Nodes {num_of_nodes}', color=line_color)
        
        # Mark the points with scatter markers of the same color as the line
        plt.scatter(inner_keys, speedup, color=line_color, zorder=5)  # zorder ensures scatter markers are above lines

    plt.xlabel('Number of Processors')
    plt.ylabel('Speedup')
    plt.legend()

    # Set the size of the figure
    plt.gcf().set_size_inches(10, 8)  # Adjust the dimensions as needed

    plt.savefig(file_path.split("/")[-2])
    plt.show()

generateStat()
showGraph('../Parallel/stat.txt')
showGraph('../Pipeline/stat.txt')
