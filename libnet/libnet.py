from GitScraper import GitScraper
from LibraryNetwork import LibraryNetwork
from libnet_visualizer import *

from queue import Queue
import pprint
import sys
import time

# Libnet
# ------------------------------------------------------------------------- #
# Builds and visualizes a network of Python libraries and their dependencies
# based on their GitHub repositories. The network is naive in the sense that
# it only takes into count the libraries that A: have a repository in GitHub
# and B: whose GitHub repository contains a requirements.txt file from where
# its dependencies can be scraped.
# 
# This was done as a course project for Aalto University course TU-C9270
# (Introduction to networks)
# ------------------------------------------------------------------------- #

path = "./snapshot/"
net_snapshot_file = "libn.txt" 
queue_snapshot_file = "queue.txt"

def write_snapshot(queue, libn):
    try:
        if not libn.write_libnet_file(path+net_snapshot_file):
            return False
        if not queue.empty():
            q_file = open(path+queue_snapshot_file, "w")
            first = ""
            while True:
                if first == "":
                    first = queue.get()
                    q_file.write(first + '\n')
                    queue.put(first)
                else:
                    current = queue.get()
                    if current == first:
                        break
                    q_file.write(current + '\n')
                    queue.put(current)

            q_file.close()
        return True

    except Exception as e:
        print(f"Writing snapshot failed: {e}")


def read_previous_snapshot(queue, libn):
    try:
        if not libn.read_libnet_file(path+net_snapshot_file):
            return False

        q_file = open(path+queue_snapshot_file, "r")
        q_data = q_file.read()
        q_file.close()

        q_list = q_data.split('\n')

        for r in q_list[:-1]:
            lib = r.split('/')[1]
            queue.put(r)

        return True

    except Exception as e:
        print(f"Reading previous snapshot failed: {e}")
        return False
    
def start(N=10, continue_from_prev=False):
    print(f"Building library network of {N} libraries")

    # Github authentication with OAuth2
    env_f = open(".env", "r")
    token = env_f.read()
    env_f.close()

    gs = GitScraper(token)
    libnet = LibraryNetwork()
    prev_loaded = False

    if continue_from_prev:
        print("Loading previous snapshot")
        repos = Queue()
        prev_loaded = read_previous_snapshot(repos, libnet)
    
    if not prev_loaded:
        print("Scraping Github...")
        repos_dom = gs.get_bs_dom("https://github.com/trending/python?since=monthly")
        repos = gs.get_repos_from_dom(repos_dom)

    # Mainloop, keep going until N nodes
    while libnet.size() <= N:
        if repos.empty():
            repos = gs.get_filler_repos()
        lib = repos.get()
        
        # Check if current lib has been "processed" i.e. the lib and all of its dependencies
        # are already in the network
        if not libnet.is_processed(lib):
            print(f"Processing {lib}")
            if not libnet.contains(lib):
                if libnet.size() == N:
                    break;
                libnet.add_lib(lib)
                print(f"Added {lib}")

            dependencies = gs.get_requirements_from_search(lib)
            for dep in dependencies:
                cur_size = libnet.size()
                print(f"Size of Libnet: {cur_size}")
                if cur_size > 0 and cur_size % 15 == 0:
                    # Save after every 15 nodes added
                    write_snapshot(repos, libnet)

                dep_lib = gs.get_repo_from_search(dep)
                if dep_lib is not None:
                    if not libnet.contains(dep_lib):
                        if libnet.size() == N:
                            break;
                        repos.put(dep_lib)
                        libnet.add_lib(dep_lib)
                        print(f"Added {dep_lib}")

                    libnet.add_dependency(lib, dep_lib)
                    print(f"Added dependency {lib} -> {dep_lib}")

            # Set processed only after going through all dependencies
            libnet.set_processed(lib)
            print(f"Processed {lib}")

    write_snapshot(repos, libnet)
    libnet.print_libs()
    libnet.print_deps()
    visualize_libnet(libnet)


if len(sys.argv) > 1:
    try:
        N = int(sys.argv[1])
        if len(sys.argv) > 2:
            cont = sys.argv[2] == "--continue" or sys.argv[2] == "-c"
            start(N, cont)
        else:
            start(N)
    except ValueError:
        print(f"Invalid node amount given as argument: '{sys.argv[1]}'")
        pass
else:
    start()

