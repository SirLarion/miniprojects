# Container class for storing and handling a network of libraries and their dependencies
class LibraryNetwork:
    def __init__(self):
        self.libs = []
        self.dependencies = []
        self.processed = {}

    def read_libnet_file(self, filename):
        try:
            libn_file = open(filename, "r")
            libn_data = libn_file.read()
            libn_file.close()

            data_blocks = libn_data.split('#')
            print(data_blocks)
            for block in data_blocks[:-1]:
                lines = block.split('\n')
                lib = lines[0]
                self.add_lib(lib)
                for dep in lines[1:-1]:
                    self.add_dependency(lib, dep)

            for proc in data_blocks[-1].split('\n')[:-1]:
                self.processed[proc] = True

            return True

        except Exception as e:
            print(f"Reading {filename} failed")

    def write_libnet_file(self, filename):
        try:
            libn_file = open(filename, "w")
            libs = self.libs[:]
            while len(libs) > 0:
                lib = libs.pop(0)
                libn_file.write(lib + '\n')
                i = 0
                deps = self.dependencies[:]
                while True:
                    try:
                        dep = deps[i]
                        if dep[0] == lib:
                            libn_file.write(dep[1] + '\n')
                            del deps[i]
                        else:
                            i += 1 
                    except:
                        break
                libn_file.write("#")

            for proc in self.processed:
                libn_file.write(proc + '\n')
            
            libn_file.close()
            return True

        except Exception as e:
            print(f"Writing {filename} failed: {e}")
            return False

    def add_lib(self, lib):
        self.libs.append(lib)

    def add_dependency(self, lib, dependency):
        self.dependencies.append((lib, dependency))

    def contains(self, lib):
        return lib in self.libs

    def is_processed(self, lib):
        return self.processed.get(lib, False)

    def set_processed(self, lib):
        self.processed[lib] = True

    def get_libs(self):
        return self.libs

    def get_deps(self):
        return self.dependencies

    def size(self):
        return len(self.libs)

    def print_libs(self):
        for lib in self.libs:
            print(lib)

    def print_deps(self):
        for dep in self.dependencies:
            print(dep)

