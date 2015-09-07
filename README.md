# Atmospheric Scattering
Sky is a GLSL shader that simulates atmospheric scattering and its effect on clouds. I completed this project as a midterm assignment for Computational Graphics at Williams College with Benji Jones as a teammate. The final shader we wrote consists of two parts. The first part draws the sky background by simulating the scattering of light particles in the atmosphere. Clouds are drawn in front of the sky by approximating how light scatters in a density field. We used the [G3D Engine](http://g3d.sourceforge.net) at the request of our Professor, Morgan McGuire.

![The final render][f0]

- [The final shader][shader]
- [A short film of the shader][film]
- [An unedited clip at the regular framerate][clip]


## Rendering Technique
Real life objects get their color from light bouncing off of them and into your eye or a camera. Raycasting, the technique used in this project, is a method of determines the color of each pixel on the screen by approximating how light bounces off of things in a scene. You might see why raycasting works well here. The sky gets its color from sunlight bouncing off of the particles that make up our atmosphere in an effect called atmospheric scattering.

## Atmosphere
For the purpose of rendering a clear sky, there are really only two ways that light can scatter in the atmosphere:
- Large particles like dust and aerosols scatter all wavelengths of light almost equally. This creates an effect known as Mie scattering. Mie scattering is generally what makes the sun and distant objects appear hazy and indistinct.
- Small particles (about the size of the wavelength of the light itself) scatter more light at shorter wavelengths. This is called Rayleigh scattering, which is responsible for the sky's blue color at midday, and its reddish color at sunrise and sundown.

![Figure 1][f1]
![Figure 2][f2]
![Figure 3][f3]

## Clouds
Unlike particles in the atmosphere, water particles in clouds are quite large but vary a lot in density. We used a 3 dimensional noise function to generate the density randomly. A raymarch (stepping through a raycast at regular intervals) calculates the visible density of the clouds at each pixel. Clouds cast shadows on them selves, so a secondary raycast determines how much any given point inside the cloud is in shadow, and interpolates between the ambient light colors of the sky to produce the final result.

![Figure 4][f4]
![Figure 5][f5]

## References
We can't claim to take all of the credit. These references were critical in helping us understand how to implement our shader:
- [Clouds](https://www.shadertoy.com/view/XslGRr) by Inigo Quilez (iq)
- [TekF Clouds](https://www.shadertoy.com/view/lssGRX) shader by Ben Weston (TekF)
- [Sunsets](https://www.shadertoy.com/view/lss3DS) by Even Entam (ThiSpawn)
- [GPU Gems 16:Accurate Atmospheric Scattering](http://http.developer.nvidia.com/GPUGems2/gpugems2_chapter16.html) by Sean O'Neil
- [Atmospheric Scattering](http://www.scratchapixel.com/old/lessons/3d-advanced-lessons/simulating-the-colors-of-the-sky/atmospheric-scattering/) on scratchapixel.com

[f0]: doc-files/final.png "A complete rendering"
[f1]: doc-files/mie.png "Mie scattering"
[f2]: doc-files/rayleigh.png "Rayleigh scattering"
[f3]: doc-files/both.png "Mie and Rayleigh scattering combined"
[f4]: doc-files/no-shadows.png "Clouds without shadows"
[f5]: doc-files/shadows.png "Clouds with shadows"

[film]: journal/sky-film.mp4 "Short film"
[clip]: journal/2014-10-21_002_midterm-sky_r3409_g3d_r5430__Full_Day_cycle_with_better_clouds.mp4 "Journal clip"
[shader]: data-files/sky.pix "Final shader"
[atmosphere-shader]: data-files/sunsets.pix "Atmosphere shader"
[clouds-shader]: data-files/clouds.pix "Clouds shader"
[mainpage]: /mainpage.md "Technical write-up"
