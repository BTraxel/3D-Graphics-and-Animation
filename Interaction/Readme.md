This repository showcases an interactive application built using OpenGL. The project incorporates various 3D models created by other creators, and proper attribution to the original creators is provided. Here is an overview of the contents and objectives of this repository:

Attributed Models:

    "Industrial Bulkhead Wall Lamp Lowpoly" by iwanPlays, licensed under CC-BY-4.0.
        https://sketchfab.com/3d-models/industrial-bulkhead-wall-lamp-lowpoly-4c50439bcbc247629fe2194862d7acc9

    "Medieval sword" by LowSeb, licensed under CC-BY-4.0.
        https://sketchfab.com/3d-models/medieval-sword-da574cba504e4b83a3293f3d3bd067fb

    "Western barrel" by FFeller, licensed under CC-BY-4.0.
        https://sketchfab.com/3d-models/western-barrel-b6d5cdb0a41340e0bbb27935ec4b955f

    "Wooden Crate" by Memorie, licensed under CC-BY-4.0.
        https://sketchfab.com/3d-models/wooden-crate-2a3ac54357bb4b92bfbc119111c09fd1

Project Overview:

    The project aims to recreate a scene similar to the UE5 (Unreal Engine 5) project.
    The use of OpenGL led to certain complexities and deviations from the original objective.
    The application utilizes vertices to create a house-like structure, incorporating various 3D models and ambient lighting.
    There may be some issues with vertex rendering and lighting that result in unexpected behavior.
    The application features a first-person camera perspective, which is bound to a human's height, and includes a torchlight system.
    Interactivity is a key feature, allowing users to perform actions such as opening doors (E), toggling the torchlight (F), manipulating a sword (UP, DOWN, LEFT, RIGHT keys), inverting polarity (Q), and adjusting the torchlight diameter (Z, X).
    Attempts were made to implement a skybox, although these feature encountered challenges.
    One primary challenge faced was the loading of textures, resulting in issues with texture indexing across models. This is identified as a priority for resolution.
    Future plans for the project include implementing an instancing system for drawing trees and introducing directional lighting.
    Efforts have been made to include a custom glTF model, although loading issues have been encountered.

This repository serves as a work-in-progress interactive application that combines 3D models, OpenGL, and interactivity. It provides insights into the development process and challenges faced during the project's creation.
