## Demo 
https://github.com/user-attachments/assets/2a7823a9-b2a6-4c8c-89ef-a0911d3eb683

## Description
For this project, I created a 3D articulated puppet using a hierarchical model with interactive joints. The puppet is structured as a tree-based skeleton, where body parts are connected through parent-child relationships. When a joint moves, all dependent parts follow, mimicking real-world articulation. It supports 15 degrees of freedom, with undo/redo functionality for joint movements. Built using OpenGL and Lua.

## Compilation Instructions
make  
./A3 Assets/puppet.lua
