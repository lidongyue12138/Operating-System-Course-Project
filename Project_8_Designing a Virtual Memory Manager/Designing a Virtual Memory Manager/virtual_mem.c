#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <errno.h>

/* Global definitions. */
#define PAGE_SIZE 256 // Page size, in bytes.
#define PAGE_ENTRIES 256 // Max page table entries.
#define PAGE_NUM_BITS 8 // Page number size, in bits.
#define FRAME_SIZE 256 // Frame size, in bytes.
#define FRAME_ENTRIES 256 // Number of frames in physical memory.
#define MEM_SIZE (FRAME_SIZE * FRAME_ENTRIES) // Mem size
#define TLB_ENTRIES 16 // Max TLB entries.

/* Global variables. */
int virtual; // Virtual address.
int page_number; // Page number.
int offset; // Offset.
int physical; // Physical address.
int frame_number; // Frame number.
int value;
int page_table[PAGE_ENTRIES]; // Page table.
int tlb[TLB_ENTRIES][2]; // Translation look-aside buffer.
int tlb_front = -1; // TLB front index, queue data structure.
int tlb_back = -1; // TLB back index, queue data structure.
char memory[MEM_SIZE]; // Physical memory. Each char is 1 byte.
int mem_index = 0; // Points to beginning of first empty frame.

/* Statistics variables. */
int fault_counter = 0; // Count the page faults.
int tlb_counter = 0; // TLB hit counter.
int address_counter = 0; // Counts addresses read from file.
float fault_rate; // Fault rate.
float tlb_rate; // TLB hit rate.

/* Functions declarations. */
int get_physical(int virtual);
int get_offset(int virtual);
int get_page_number(int virtual);
void initialize_page_table(int n);
void initialize_tlb(int n);
int consult_page_table(int page_number);
int consult_tlb(int page_number);
void update_tlb(int page_number, int frame_number);
int get_frame();

int main(int argc, char *argv[]) {
    /* File I/O variables. */
    char* in_file; 
    char* out_file;
    char* store_file; 
    char* store_data; 
    int store_fd; 
    char line[8]; 
    FILE* in_ptr; 
    FILE* out_ptr;
    
    /* Initialize page_table, set all elements to -1. */
    initialize_page_table(-1);
    initialize_tlb(-1);

    /* Get command line arguments. */
    if (argc != 4) {
        printf("Enter input, output, and store file names!");

        exit(EXIT_FAILURE);
    }
    else {
        /* Get the file names from argv[]. */
        in_file = argv[1];
        out_file = argv[2];
        store_file = argv[3];

        /* Open the address file. */
        if ((in_ptr = fopen(in_file, "r")) == NULL) {
            printf("Input file could not be opened.\n");

            exit(EXIT_FAILURE);
        }

        /* Open the output file. */
        if ((out_ptr = fopen(out_file, "a")) == NULL) {
            printf("Output file could not be opened.\n");

            exit(EXIT_FAILURE);
        }

        /* Open the store file. */
        store_fd = open(store_file, O_RDONLY);
        store_data = mmap(0, MEM_SIZE, PROT_READ, MAP_SHARED, store_fd, 0);
        /* Check that the mmap call succeeded. */
        if (store_data == MAP_FAILED) {
            close(store_fd);
            printf("Error mmapping the backing store file!");
            exit(EXIT_FAILURE);
        }

        /* Loop through the input file one line at a time. */
        while (fgets(line, sizeof(line), in_ptr)) {
            virtual = atoi(line);
            address_counter++;

            
            page_number = get_page_number(virtual);
            offset = get_offset(virtual);

            /* Use page_number to find frame_number in TLB, if it exists. */
            frame_number = consult_tlb(page_number);

            if (frame_number != -1) {
                /* TLB lookup succeeded. */
                physical = frame_number + offset;

                /* Fetch the value directly from memory. */
                value = memory[physical];
            }
            else {
                /* TLB lookup failed. */
                frame_number = consult_page_table(page_number);

                if (frame_number != -1) {
                    /* No page fault. */
                    physical = frame_number + offset;

                    /* Update TLB. */
                    update_tlb(page_number, frame_number);

                    /* Fetch the value directly from memory. */
                    value = memory[physical];
                }
                else {
                    /* Page fault! */
                    int page_address = page_number * PAGE_SIZE;

                    /* Check if a free frame exists. */ 
                    if (mem_index != -1) {
                        /* Success, a free frame exists. */
                        /* Store the page from store file into memory frame. */
                        memcpy(memory + mem_index, 
                               store_data + page_address, PAGE_SIZE);

                        frame_number = mem_index;
                        physical = frame_number + offset;

                        value = memory[physical];

                        /* Update page_table with correct frame number. */
                        page_table[page_number] = mem_index;
                        
                        /* Update TLB. */
                        update_tlb(page_number, frame_number);


                        if (mem_index < MEM_SIZE - FRAME_SIZE) {
                            mem_index += FRAME_SIZE;
                        }
                        else {
                            mem_index = -1;
                        }
                    }
                    else {
                        /* Failed, no free frame in memory exists. */
                        /* Swap! */
                    }
                }
            }

            fprintf(out_ptr, "Virtual address: %d ", virtual); 
            fprintf(out_ptr, "Physical address: %d ", physical);
            fprintf(out_ptr, "Value: %d\n", value);
        }

        /* Calculate rates. */
        fault_rate = (float) fault_counter / (float) address_counter;
        tlb_rate = (float) tlb_counter / (float) address_counter;

        /* Print the statistics to the end of the output file. */
        fprintf(out_ptr, "Number of Translated Addresses = %d\n", address_counter); 
        fprintf(out_ptr, "Page Faults = %d\n", fault_counter);
        fprintf(out_ptr, "Page Fault Rate = %.3f\n", fault_rate);
        fprintf(out_ptr, "TLB Hits = %d\n", tlb_counter);
        fprintf(out_ptr, "TLB Hit Rate = %.3f\n", tlb_rate);

        fclose(in_ptr);
        fclose(out_ptr);
        close(store_fd);
    }

    return EXIT_SUCCESS;
}

/* Calculate and return the physical address. */
int get_physical(int virtual) {
    physical = get_page_number(virtual) + get_offset(virtual);

    return physical;
}

/* Calculate and return the page number. */
int get_page_number(int virtual) {
    return (virtual >> PAGE_NUM_BITS);
}

/* Calculate and return the offset value. */
int get_offset(int virtual) {
    int mask = 255;

    return virtual & mask;
}

/* Sets all page_table elements to integer n. */
void initialize_page_table(int n) {
    for (int i = 0; i < PAGE_ENTRIES; i++) {
        page_table[i] = n;
    }
}

/* Sets all TLB elements to integer n. */
void initialize_tlb(int n) {
    for (int i = 0; i < TLB_ENTRIES; i++) {
        tlb[i][0] = -1;
        tlb[i][1] = -1;
    }
}

/* Takes a page_number and checks for a corresponding frame number. */
int consult_page_table(int page_number) {
    if (page_table[page_number] == -1) {
        fault_counter++;
    }

    return page_table[page_number];
}

/* Takes a page_number and checks for a corresponding frame number. */
int consult_tlb(int page_number) {
    for (int i = 0; i < TLB_ENTRIES; i++) {
        if (tlb[i][0] == page_number) {
            /* TLB hit! */
            tlb_counter++;

            return tlb[i][1];
        }
    }
    /* TLB miss! */
    return -1;
}

void update_tlb(int page_number, int frame_number) {
    /* Use FIFO policy. */
    if (tlb_front == -1) {
        tlb_front = 0;
        tlb_back = 0;

        /* Update TLB. */
        tlb[tlb_back][0] = page_number;
        tlb[tlb_back][1] = frame_number;
    }
    else {
        tlb_back = (tlb_back + 1) % TLB_ENTRIES;

        /* Insert new TLB entry in the back. */
        tlb[tlb_back][0] = page_number;
        tlb[tlb_back][1] = frame_number;
    }

    return;
}
