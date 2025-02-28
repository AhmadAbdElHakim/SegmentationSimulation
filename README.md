# Memory Management Simulator

A memory management simulator focused on non-contiguous segmentation allocation. Written in C++ using Qt, it visually demonstrates how memory segments are allocated to processes, simulating the dynamics of segmentation in operating systems. This educational tool is designed to help users understand memory fragmentation, segmentation overhead, and the overall impact of non-contiguous memory allocation strategies.

## Available Algorithms

The user can choose among several scheduling algorithms:
- **First-fit allocation**
- **Best-fit allocation**
- **Deallocation of processes and reserved segments**

## Example Walkthrough

1 - **User inputs holes starting address and size** <br/> <img width="683" alt="Image" src="https://github.com/user-attachments/assets/4e041c9b-98cc-4108-8e9e-89b8f35fcce1" /> <br/>
2 - **User inputs the first process and selects first fit**<br/> <img width="683" alt="Image" src="https://github.com/user-attachments/assets/b87eaa79-f9d4-4748-9693-ac9117ceb0fe" /> <br/>
3 - **User inputs the second process and selects best fit**<br/> <img width="683" alt="Image" src="https://github.com/user-attachments/assets/3d284e3d-1ed8-459d-bb45-ba55105f0671" /> <br/>
4 - **User tries to enter a process that doesn’t fit**<br/> <img width="683" alt="Image" src="https://github.com/user-attachments/assets/5a8d1ccb-14b8-47fc-bf66-a7382852e262" /> <br/>
5 - **User inputs the third process and selects best fit**<br/> <img width="683" alt="Image" src="https://github.com/user-attachments/assets/272d71aa-b85b-412a-8777-39c4ad9169bf" /> <br/>
6 - **Segmentation tables of the processes**<br/> <img width="605" alt="Image" src="https://github.com/user-attachments/assets/e59f86cf-8296-41a7-8355-d180f4c0d545" />  <br/>
7 - **User deallocates a process**<br/> <img width="683" alt="Image" src="https://github.com/user-attachments/assets/3f6d53ff-6426-46aa-9146-8c55c3ffd481" /> <br/>
8 - **User deallocates a reserved segment**<br/>  <img width="683" alt="Image" src="https://github.com/user-attachments/assets/297c326b-a65e-40d0-a690-66ef8f91172d" /> <br/>

## Features

- **Visual Simulation:** Graphically represents segmentation memory allocation, aiding in the understanding of memory management concepts.
- **User Interaction:** Allows users to input and modify segment parameters to observe how changes affect memory allocation.
- **Educational Tool:** Serves as a learning aid for students and professionals interested in operating systems and memory management.
- **Customization:** Easily configure simulation parameters.
- **Error Handling** <br/> <img width="472" alt="Image" src="https://github.com/user-attachments/assets/ea9c4543-263a-44a1-ae43-bdb404af32d3" />

### Building from Source

1. **Clone the Repository:**
   ```bash
   git clone https://github.com/AhmadAbdElHakim/Scheduler.git
