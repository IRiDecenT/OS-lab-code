import random
import time
from collections import deque

class MemoryManager:
    def __init__(self, memory_size, page_size):
        self.memory_size = memory_size
        self.page_size = page_size
        self.page_bitmap = [0] * (memory_size // page_size)
        self.free_pages = memory_size // page_size
        self.page_queue = deque(maxlen=memory_size // page_size)

    def allocate_pages(self, num_pages):
        if self.free_pages < num_pages:
            return None
        else:
            allocated_pages = []
            for _ in range(num_pages):
                page = self.page_bitmap.index(0)
                self.page_bitmap[page] = 1
                self.free_pages -= 1
                allocated_pages.append(page)
                self.page_queue.append(page)
            return allocated_pages

    def deallocate_pages(self, pages):
        for page in pages:
            self.page_bitmap[page] = 0
            self.free_pages += 1

    def replace_page_fifo(self, new_page):
        # FIFO页面替换算法，从队头取出最早进入的页面
        replaced_page = self.page_queue.popleft()
        self.page_queue.append(new_page)
        return replaced_page

class Process:
    def __init__(self, process_id, memory_manager):
        self.process_id = process_id
        self.memory_manager = memory_manager
        self.page_table = {}
        self.page_faults = 0

    def generate_logical_address(self):
        # Generate a logical address based on the specified probability distribution
        return random.randint(0, 2**14 - 1)

    def access_memory(self, logical_address):
        page_number = logical_address // self.memory_manager.page_size
        if page_number in self.page_table:
            # Page is already in memory, access directly
            physical_address = self.page_table[page_number]
            content = self.read_memory(physical_address)
        else:
            # Page fault, load the page into memory
            if self.memory_manager.free_pages < 10:
                # Perform page replacement using FIFO algorithm
                replaced_page = self.memory_manager.replace_page_fifo(page_number)

                # Update page table
                del self.page_table[replaced_page]
                self.page_table[page_number] = replaced_page

                # Load the page content from disk
                content = self.load_page_from_disk(page_number)
            else:
                # Allocate pages for the process
                allocated_pages = self.memory_manager.allocate_pages(9)
                if allocated_pages is not None:
                    # Update page table
                    self.page_table = {i: page for i, page in enumerate(allocated_pages)}
                    # Update memory manager
                    self.memory_manager.deallocate_pages(allocated_pages)

                    # Load the page content from disk
                    content = self.load_page_from_disk(page_number)
                else:
                    # Wait for free pages or perform page replacement
                    # ...

                # Increment page fault count
                self.page_faults += 1

        # Simulate random sleep
        sleep_time = random.randint(0, 100) / 1000
        time.sleep(sleep_time)

        # Output access information
        print(f"Process {self.process_id}: Access - Logical Address: {logical_address}, "
              f"Content: {content}, Physical Address: {physical_address}")

    def load_page_from_disk(self, page_number):
        # Read the content of the page from disk
        file_path = f"job_{self.process_id}.txt"
        with open(file_path, 'r') as file:
            file.seek(page_number * 256)
            content = file.read(256)
        return content

    def read_memory(self, physical_address):
        # Read the content of the memory at the given physical address
        # Simulated memory content, replace with actual memory content
        return f"Memory Content at {physical_address}"

# Simulation
memory_manager = MemoryManager(memory_size=2**14, page_size=256)
processes = [Process(process_id=i, memory_manager=memory_manager) for i in range(12)]

for _ in range(200):
    for process in processes:
        logical_address = process.generate_logical_address()
        process.access_memory(logical_address)

# Calculate page fault rates
for process in processes:
    page_fault_rate = process.page_faults / 200
    print(f"Process {process.process_id}: Page Fault Rate: {page_fault_rate}")
