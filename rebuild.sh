# Build all projects

clang++ -o additive/additive.app additive/additive.cpp -std=c++11 -lSDL2 -Ofast

clang++ -o balls/balls.app balls/balls.cpp -std=c++11 -lSDL2 -Ofast

clang++ -o barycentric/barycentric_color.app barycentric/barycentric_color.cpp -std=c++11 -lSDL2 -Ofast
clang++ -o barycentric/barycentric_textured.app barycentric/barycentric_textured.cpp -std=c++11 -lSDL2 -Ofast

clang++ -o cloth/cloth_untextured.app cloth/cloth_untextured.cpp -std=c++11 -lSDL2 -Ofast
clang++ -o cloth/cloth_textured.app cloth/cloth_textured.cpp -std=c++11 -lSDL2 -Ofast

clang++ -o dither/dither.app dither/dither.cpp -std=c++11 -lSDL2 -Ofast

clang++ -o neural/neural_two_rgb.app neural/neural_two_rgb.cpp -std=c++11 -lSDL2 -Ofast
clang++ -o neural/neural_two_xor.app neural/neural_two_xor.cpp -std=c++11 -lSDL2 -Ofast

clang++ -o normals/normal.app normals/normal.cpp -std=c++11 -lSDL2 -Ofast

clang++ -o raster/raster.app raster/raster.cpp -std=c++11 -lSDL2 -Ofast

clang++ -o sierpinski/sierpinski.app sierpinski/sierpinski.cpp -std=c++11 -lSDL2 -Ofast