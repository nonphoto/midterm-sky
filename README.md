# Sky
A shader that simulates atmospheric scattering and its effect on clouds.

I completed this project as a midterm assignment for Computational Graphics at Williams College with Benji Jones as a teammate. I plan on porting this to shadertoy soon.

## Results
![The final render][f0]

- [The final shader][shader]
- [A short film of the shader][film]
- [An unedited clip at the regular framrate][clip]

## Goals
Our goal was to create a shader that combines atmospheric scattering and clouds. We wanted it to render fast enough to interact with in real-time and be procedural, with no modeling or scene definition.

## Overview
The final GLSL shader we wrote has essentially two parts. The atmosphere shader works by simulating the scattering of light particles in the atmosphere. Clouds are drawn by simulating how light scatters in a density field. We used the [G3D Engine](http://g3d.sourceforge.net) at the request of our Professor, Morgan McGuire.

### Atmospheric Scattering
In order to explain how atmospheric scattering works, let's start with a simple model of light in the Earth's atmosphere. Simply stated, the sun emits light and some of it goes into your eyes. If this were all there is to it, then all we would see when we walk outside would be a perfect circle of light in a pitch black sky. However, we know that the Earth's atmosphere is composed of billions of tiny particles, and all of these particles scatter light to varying degrees. This is how we can see the sky; light from the sun enters the atmosphere and hits some point in the volume of the atmosphere that scatters it toward your eye. Additionally, this means that light coming directly from the sun is attenuated and appears less intense.

To simulate this, we wrote the following two functions, and assume that the atmosphere is of uniform density:
- `directLight(vec3 P, vec3 w)` calculates the color of light at `P` coming directly from `w` using a fixed step ray-marching algorithm. Light is attenuated each step by an amount that depends on the constant scattering coefficients. This is used later to calculate the color of the clouds.
- `scatteredLight(vec3 P, vec3 w)` calculates the total light at `P` that was scattered from direction `w` using a fixed step ray-marching algorithm. Each step along the ray simulates a point at which direct light is scattered toward `P`. To this end, we do a secondary ray-march at each step in the direction of the sun that calculates the amount of direct light at that sample position (in a similar way as `directLight()`, which is then used to calculate the amount of light that is scattered at each sample position.

This is still a slightly simplified model, because different sizes of particles scatter light in different ways. The angular distribution of scattered light is governed by the phase function, which we use to calculate the amount of scattered light. For our purposes there are really two kinds of scattering, and two phase functions:
- Large particles like dust and aerosols scatter all wavelengths of light almost equally. This creates an effect known as Mie scattering (Results fig. 1). Mie scattering is generally what makes the sun and distant objects appear hazy and indistinct.
- Small particles (particles with a diameter on the order of the wavelength of the light itself) scatter shorter wavelengths more easily. This is called Rayleigh scattering (Results fig. 2), which is responsible for the sky's blue color at midday. At sunrise and sundown, light from the sun must travel through more atmosphere to get to the camera, which is why it appears red.

![Figure 1][f1]
![Figure 2][f2]
![Figure 3][f3]

We add these two types of scattering functions together in `directLight()` and `scatteredLight()`. Even though light can scatter more than once, this is enough to produce a reasonable graphical approximation of the atmosphere because multiple scattering doesn't significantly improve the visual quality of the rendered image. Source code for a shader that simulates atmospheric scattering only can be found [here][atmosphere-shader].

### Clouds
Like the atmosphere, clouds are a semi-transparent volume. Unlike the atmosphere, however, we can't assume that clouds have uniform density (in fact they almost never do). We didn't want to do any modeling, so we used a noise function to generate an infinite field of clouds. The noise function (which can run in a full procedural mode in addition to the faster lookup table mode) is borrowed from Inigo Quilez's "Clouds" shader.<br>

The `map(vec3 X)` function returns a density float value for any point `X`. This function mixes noise samples at different scales to produce a more complex, cloud-like density distribution. Because the density distribution interpolates smoothly between high and low values, this creates clouds that are most dense in the center and less dense toward the edges.

We render the clouds with a ray-marching algorithm that accumulates the color of the cloud at each point scaled by the color's alpha channel, which is calculated using `map()`. If we linearly interpolate the cloud color by it's density value from black to white, we get unlit clouds (Results fig. 4).

![Figure 4][f4]
![Figure 5][f5]

To render clouds with accurate lighting, we need to calculate the color at each step in the ray-march with shadows in mind. This is tricky because clouds cast shadows on themselves, which means that the amount that any given point the cloud is in shadow depends on the collective density along a continuous line from the point to the location of the light source. A secondary ray-march would be prohibitively costly, so we approximate the shadow by choosing a single point a fixed step along the line and using `map()` to calculate the density there. We use this secondary density value to interpolate between the direct and ambient light colors for the clouds, where a low density value produces more direct light color, and a high density value produces more ambient light color. Source code for our cloud shader can be found [here][clouds-shader].

However, we also want the direct and scattered light colors in the clouds to reflect the color of the sky. This is easily done once we combine the two shaders into [our final shader][shader]. The direct light color is calculated by calling `directLight()` in the direction of the sun. The ambient light color is approximated by calling `scatteredLight()` for some vertical direction, which approximates the average color of the sky.

All of these factors combine to create a believable render of the sky and clouds (Results fig. 0), but there is still more to do. Here are some possible improvements for the future:
- A night cycle and moon. Currently, we only render the sun and night is pitch black.
- Performance improvements. Our shader runs in realtime, but begins to struggle at resolutions higher than 720x480.
- Crepuscular rays, aerial perspective and terrain. It should be possible to incorporate terrain into our shader, which could itself introduce some interesting atmospheric lighting effects. 

## References
We can't claim to take all of the credit. These references were critical in helping us understand how to implement our shader:
- [Clouds](https://www.shadertoy.com/view/XslGRr) by Inigo Quilez (iq)
- [TekF Clouds](https://www.shadertoy.com/view/lssGRX) shader by Ben Weston (TekF)
- [Sunsets](https://www.shadertoy.com/view/lss3DS) by Even Entam (ThiSpawn)
- [GPU Gems 16:Accurate Atmospheric Scattering](http://http.developer.nvidia.com/GPUGems2/gpugems2_chapter16.html) by Sean O'Neil
- [Atmospheric Scattering](http://www.scratchapixel.com/old/lessons/3d-advanced-lessons/simulating-the-colors-of-the-sky/atmospheric-scattering/) on scratchapixel.com

[f0]: doc-files/final.png "Figure 0: The Final Product"
[f1]: doc-files/mie.png "Figure 1: Mie scattering"
[f2]: doc-files/rayleigh.png "Figure 2: Rayleigh scattering"
[f3]: doc-files/both.png "Figure 3: Mie and Rayleigh combined"
[f4]: doc-files/no-shadows.png "Figure 4: Clouds without shadows"
[f5]: doc-files/shadows.png "Figure 5: Clouds with shadows"

[film]: journal/sky-film.mp4 "Short film"
[clip]: 2014-10-21_002_midterm-sky_r3409_g3d_r5430__Full_Day_cycle_with_better_clouds.mp4 "Journal clip"
[shader]: data-files/sky.pix "Final shader"
[atmosphere-shader]: data-files/sunsets.pix "Atmosphere shader"
[clouds-shader]: data-files/clouds.pix "Clouds shader"

(c) 2014, Jonas Luebbers and Benji Jones
