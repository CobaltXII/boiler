### Balls

Balls is a simple, inaccurate and fun toy for simulating the physics of rigid circular bodies. This simulation supports circle vs. circle collisions, along with circle vs. capsule collisions. It also supports momentum conservation, displacement and gravity. It is oddly satisfying to create hourglasses, slopes, marble runs and other physics demonstrations with this tool.

![Screenshot](https://github.com/CobaltXII/boiler/blob/master/img/s_balls_2.png "Balls on slopes")

### Raster

Raster is a software rasterizer. It is capable of rendering three-dimensional objects to the screen as pixels; this is called rasterization. Currently, this rasterizer supports rotation, scaling, translation, projection, and limited clipping. The clipping is very buggy, so objects must be rendered at a distance. Texturing is not supported, objects are colored with a color derived from their normals. Only flat shading is implemented.

![Screenshot](https://github.com/CobaltXII/boiler/blob/master/img/s_raster_4.png "Stanford bunny")

### Normals

This is an extremely simple normal mapping implementation. This code takes an image (called a normal map) and uses the color values to generate a map of normals. The normals are then used with shading (diffuse) and combined with point lights can generate extremely realistic lighting effects. This program renders a few lights interacting with a normal mapped image in three-dimensional space.

![Screenshot](https://github.com/CobaltXII/boiler/blob/master/img/s_normals_3.png "Schizophrenic normal mapped coin")

### Barycentric

Barycentric is a triangle renderer. It renders triangles using a hard-coded shader, which can manipulate the triangle in any way. The renderer uses barycentric coordinates to 'weigh' the vertex data of the triangle, and then uses those weights to fetch texture information or generate a color value. The renderer is moderately fast, and I hope to use it in my future projects. Barycentric currently has color and texture interpolation support.

![Screenshot](https://github.com/CobaltXII/boiler/blob/master/img/s_barycentric_1.png "Colorful triangle")

### Additive

Additive is an additive blending demonstration. It features three circles, which are red, green and blue. The circles pulse, spin, and swap positions at a fast pace. Additive blending is a form of color blending, which closely mimics the effect produced by blending physical lights. Additive blending can be used in many applications, such as dynamic lighting, realistic particle effects, glowing sprites, and rendering.

![Screenshot](https://github.com/CobaltXII/boiler/blob/master/img/s_additive_1.png "Additive blending")

### Sierpiński

Sierpiński is a simple program to render the Sierpiński triangle fractal. I thought of the algorithm to render the fractal myself, but I am sure that someone else has thought of a similar (if not identical) algorithm before. No recursive functions are used, just a vector of triangles that mutate on demand. I find watching the Sierpiński triangle progress very satisfying, and the resulting pattern is beautiful.

![Screenshot](https://github.com/CobaltXII/boiler/blob/master/img/s_sierpinski_1.png "Recursive triforce (Twister#6074)")

### Cloth

Cloth is a cloth simulation using Verlet integration. The cloth simulation is made up of tiny point masses arranged in a net or grid pattern, which are linked together with rigid constraints. The constraints try hard to keep the two points they are tethered to a certain distance from each other, but sometimes external forces are too great; the constraints pop. Currently, this project supports untextured, rippable cloth, and textured, unrippable cloth.

![Screenshot](https://github.com/CobaltXII/boiler/blob/master/img/s_cloth_2.png "Canadian flag")

### Dither

Dither is a tiny dithering library. It supports black and white thresholding, random dithering, Gaussian dithering, and ordered dithering. It also supports splicing color images into their one-dimensional channels, and joining grayscale images to create color images. It includes Bayer matrices of varying sizes, cluster dot matrices, horizontal line matrices and vertical line matrices. Gamma correction is also implemented. It can be used for 1-bit grayscale dithering, or 3-bit color dithering.

![Screenshot](https://github.com/CobaltXII/boiler/blob/master/img/s_dither_bayer_8_gamma_rgb.png "Gamma corrected 3-bit Bayer ordered dithering")