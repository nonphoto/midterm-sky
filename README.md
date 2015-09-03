# Sky
Sky is a GLSL shader that simulates atmospheric scattering and its effect on clouds.

I completed this project as a midterm assignment for Computational Graphics at Williams College with Benji Jones as a teammate. I plan on porting this to [Shadertoy](www.shadertoy.com) soon.

We The final GLSL shader we wrote consists of two parts. The first part draws the sky background by simulating the scattering of light particles in the atmosphere. Clouds are drawn in front of the sky by approximating how light scatters in a density field. We used the [G3D Engine](http://g3d.sourceforge.net) at the request of our Professor, Morgan McGuire.

Some results:

![The final render][f0]

- [The final shader][shader]
- [A short film of the shader][film]
- [An unedited clip at the regular framrate][clip]


### Rendering Technique

This project lends itself well to a rendering technique called [raycasting](www.wikipedia.com). Real life objects get their color from light bouncing off of them and into your eye or a camera. Raycasting is a method of determining the color of each pixel on the screen by approximating how light bounces off of things in a scene. Unlike in real life, the rays go from the camera outward and only follow a subset of the many possible paths light can take. 

You might see why raycasting works well here. The sky gets its color from sunlight bouncing off of the particles that make up our atmosphere in an effect called atmospheric scattering. This means we can simulate each ray of light from the sun to the camera with a raycast in the opposite direction.

### Atmosphere
For the purpose of rendering a clear sky, there are really only two ways that light can scatter in the atmosphere:
- Large particles like dust and aerosols scatter all wavelengths of light almost equally. This creates an effect known as Mie scattering. Mie scattering is generally what makes the sun and distant objects appear hazy and indistinct.
- Small particles (about the size of the wavelength of the light itself) scatter shorter wavelengths more easily. This is called Rayleigh scattering, which is responsible for the sky's blue color at midday. At sunrise and sundown, light from the sun must travel through more atmosphere to get to the camera, which is why it appears red.

![Figure 1][f1]
![Figure 2][f2]
![Figure 3][f3]

The above images show the results of a shader that simulates Mie scattering, then Rayleigh scattering, then the two combined.

### Clouds

Unlike particles in the atmosphere, water particles in clouds are quite large but vary a lot in density. We used a 3 dimensional [noise function](www.wikipedia.com) to generate the density randomly. Like before, a raycasting algorithm determines the color of each visible point in the scene, but this time we need to take shadows into account. Clouds actually cast shadows on themselves, so calculate how much any given point inside the cloud is in shadow, the algorithm performs a secondary raycast straight toward the sun to determine how far within the cloud it is, and reduces the light that reaches that point accordingly.

Finally, the color of the clouds isn't just the color of the light coming from the sun, but the result of all of the scattered light in the atmosphere. For the sake of speed, the algorithm interpolates between an ambient light color and direct light color calculated from the atmospheric scattering results rather than perform another raycast for each part of the cloud. 

![Figure 4][f4]
![Figure 5][f5]

All of these factors combined provided us with some pretty believable results, but there is still more we could do to make the rendering more realistic:
- A night cycle and moon. Currently, we only render the sun and it is pitch black at night.
- Performance improvements. Our shader runs in realtime, but begins to struggle at resolutions higher than 720x480.
- Crepuscular rays, aerial perspective and terrain. Terrain in particular could introduce some interesting atmospheric lighting effects as it creates shadows in the atmosphere.

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
[clip]: journal/2014-10-21_002_midterm-sky_r3409_g3d_r5430__Full_Day_cycle_with_better_clouds.mp4 "Journal clip"
[shader]: data-files/sky.pix "Final shader"
[atmosphere-shader]: data-files/sunsets.pix "Atmosphere shader"
[clouds-shader]: data-files/clouds.pix "Clouds shader"
[mainpage]: /mainpage.md "Technical write-up"
