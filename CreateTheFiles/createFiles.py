import os

# Project structure with GUI
project_name = "WormholeGateway"
structure = {
    "src": [
        "main.cpp",
        "Wormhole.hpp",
        "Wormhole.cpp",
        "physics.c",
        "visualization.c",
        "utils.c"
    ],
    "include": [
        "physics.h",
        "visualization.h",
        "utils.h"
    ],
    "gui": [
        "Renderer.hpp",
        "Renderer.cpp",
        "GuiManager.hpp",
        "GuiManager.cpp"
    ],
    "assets": [
        "fonts/",
        "shaders/",
        "textures/"
    ],
    ".": [
        "CMakeLists.txt",
        "Makefile"
    ]
}

def create_project():
    # Create root directory
    if not os.path.exists(project_name):
        os.makedirs(project_name)
    
    # Create directories and files
    for folder, items in structure.items():
        if folder == ".":
            folder_path = project_name
        else:
            folder_path = os.path.join(project_name, folder)
            if not os.path.exists(folder_path):
                os.makedirs(folder_path)
        
        # Create files or subdirectories
        for item in items:
            if folder == "assets":
                # Create asset subdirectories
                asset_path = os.path.join(folder_path, item)
                if not os.path.exists(asset_path):
                    os.makedirs(asset_path)
            else:
                # Create files
                file_path = os.path.join(folder_path, item)
                with open(file_path, 'w') as f:
                    # Add header comment based on file type
                    if item.endswith('.cpp') or item.endswith('.hpp'):
                        f.write("// WormholeGateway - C++ file\n")
                    elif item.endswith('.c') or item.endswith('.h'):
                        f.write("/* WormholeGateway - C file */\n")
                    elif item == 'CMakeLists.txt':
                        f.write("# WormholeGateway - CMake build file\n")
                    elif item == 'Makefile':
                        f.write("# WormholeGateway - Makefile\n")
                    # Leave files empty otherwise
    
    # Create build directory
    build_path = os.path.join(project_name, "build")
    if not os.path.exists(build_path):
        os.makedirs(build_path)
    
    print(f"✅ Project '{project_name}' created successfully!")
    print("\n📁 Structure:")
    
    # Print structure
    for folder, items in structure.items():
        if folder == ".":
            for item in items:
                print(f"├── {item}")
        elif folder == "assets":
            print(f"├── {folder}/")
            for item in items:
                print(f"│   └── {item}")
        else:
            print(f"├── {folder}/")
            for item in items:
                print(f"│   ├── {item}")
    
    print(f"└── build/")

if __name__ == "__main__":
    create_project()